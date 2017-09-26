#include <stdio.h>
#include <stdlib.h>

#include "tcp.h"
#include "mscommon.h"
#include "TcpClient.h"

#define TCP_SELECT_WARITING_TIMEOUT  10	 	 //10ms	
#define THREAD_IDLE_TIMEOUT			 10      //10ms
#define TCP_KEEPALIVE_TIMEOUT		 1*1000  //1000*THREAD_IDLE_TIMEOUT
#define TCP_CONNECT_TIMEOUT			 5*1000	 //5s


#ifdef WIN32
#define strerror(X) "-1"
#define ex_errno WSAGetLastError()
#define is_wouldblock_error(r) ((r)==WSAEINTR||(r)==WSAEWOULDBLOCK)
#define is_connreset_error(r) ((r)==WSAECONNRESET || (r)==WSAECONNABORTED || (r)==WSAETIMEDOUT || (r)==WSAENETRESET || (r)==WSAENOTCONN)
#else
#define ex_errno errno
#define closesocket close
#endif
#ifndef is_wouldblock_error
#define is_wouldblock_error(r) ((r)==EINTR||(r)==EWOULDBLOCK||(r)==EAGAIN)
#define is_connreset_error(r) ((r)==ECONNRESET || (r)==ECONNABORTED || (r)==ETIMEDOUT || (r)==ENETRESET || (r)==ENOTCONN)
#endif



#define MAX_DATA_BUF_LENGTH	4000
typedef struct recv_data_t
{
	int  cur_offset;
	int  max_len;
	int  data_len;
	char data_buf[MAX_DATA_BUF_LENGTH];
}recv_data_t;

typedef struct keepalive_data_t
{
	unsigned short hlen;
	unsigned short blen;
}keepalive_data_t;

static bool	gTcpReconnectState = false;
static recv_data_t* gRecvData = NULL;
static char gServerAddr[128] = {0};
static int gServerPort = 0;
static CTcpClient* gTCPTransport = NULL;
static bool_t g_initialized = FALSE;
static bool_t gTlvEnabled = FALSE;
static bool_t gIPv6Enabled = FALSE;

typedef void (*pEVcbfunc)(int ev_type, tcp_event_node_t* message);
static pEVcbfunc EVENT_CALLBACK_FUNC = NULL; 		/*App callback function*/

static void *event_thread_func(void *arg);
static int build_transport(void);
static int create_transport(void);
static void delete_transport(void);
static int send_keepalive(CTcpClient* pTransport);
static int read_message(CTcpClient* pTransport);
static int send_message(CTcpClient* pTransport, const char* sbuf, int slen);
static bool is_connect_ok(void);
static bool is_nonblock(void);
static bool is_transport_create(void);
static bool find_message(const char* rbuf, int rlen);
static bool cb_transport_event(int ev_type, const char* msg, int slen);

static int update_reconnect_state(bool state);
static bool is_reconnect(void);

static volatile int exit_flag = false;
static ms_thread_t thread_id = 0;
static ms_mutex_t sock_lock;


bool find_message(const char* rbuf, int rlen)
{
	int ev_len = 0;
	char ev_buf[MAX_MSG_LEN] = {0};

	if (NULL == rbuf || rlen == 0)
	{
	    ms_error("find_message: recv msg error.");
		return false;
	}

	if (gRecvData != NULL)
	{
	    ms_report("find_message: recv msg into buffer. isTlv[%d]", gTlvEnabled);
		memcpy(gRecvData->data_buf + gRecvData->cur_offset, rbuf, rlen);
		gRecvData->cur_offset += rlen;

		while ( gRecvData->cur_offset >= 4 )
		{
			if (gRecvData->data_len == 0)
			{
			    if ( !gTlvEnabled )
			    {
    				unsigned short hlen, blen;
    				memcpy(&hlen, gRecvData->data_buf, 2);
    				memcpy(&blen, gRecvData->data_buf + 2, 2);
    				gRecvData->data_len = ntohs(hlen) + ntohs(blen);
				}
#if defined(SKT_PROTOCOL_SUPPORT_ENABLED)    				
				else
				{
				    // msg total len
    			    unsigned int pkgLen;
                    memcpy((char *)&pkgLen, gRecvData->data_buf, sizeof(unsigned int));
                    gRecvData->data_len = ntohl(pkgLen);                    
				}
#endif
			}

			if ( (!gTlvEnabled && gRecvData->cur_offset >= gRecvData->data_len + 4)
			    || (gTlvEnabled && gRecvData->cur_offset >= gRecvData->data_len))
			{
			    if ( !gTlvEnabled )
			    {
				    ev_len = gRecvData->data_len + 4;
				}
#if defined(SKT_PROTOCOL_SUPPORT_ENABLED)
                else
                {
                    ev_len = gRecvData->data_len;
                }
#endif
				memcpy(ev_buf, gRecvData->data_buf, ev_len);
				cb_transport_event(TCP_RECV_EV, ev_buf, ev_len);
				
				if (gRecvData->cur_offset > ev_len)
				{
					gRecvData->cur_offset -= ev_len;
					memcpy(gRecvData->data_buf, gRecvData->data_buf + ev_len, gRecvData->cur_offset);
				}
				else
				{
					gRecvData->cur_offset = 0;
				}
				gRecvData->data_len = 0;
				
				continue;
			}
			break;
		}
	}

	return true;
}

int send_keepalive(CTcpClient* pTransport)
{
	keepalive_data_t keepalive_data;
	char keepbuf[1024] = {0};
	int keeplen = 0;
	if (NULL == pTransport)
		return -1;

	memset(&keepalive_data, 0, sizeof(keepalive_data));

	keepalive_data.hlen = htons(0);
	keepalive_data.blen = htons(0);
	keeplen = sizeof(keepalive_data);
	memcpy(keepbuf, &keepalive_data, keeplen);
	ms_report("tcp send keepalive  ok");
	
	return send_message(pTransport, keepbuf, keeplen);
}

int send_message(CTcpClient* pTransport, const char* sbuf, int slen)
{
	int res = 0;
	if (NULL== pTransport || NULL == sbuf || slen == 0)
		return -1;
	
	ms_mutex_lock(&sock_lock);
	res = pTransport->Send(sbuf, slen);
	ms_mutex_unlock(&sock_lock);
	
	return res;
}

int read_message(CTcpClient* pTransport)
{
	int rlen = 0;
	char rbuf[MAX_MSG_LEN] = {0};

	if (pTransport == NULL)
		return -1;

	ms_mutex_lock(&sock_lock);
	rlen = pTransport->Receive(rbuf, MAX_MSG_LEN);
	ms_mutex_unlock(&sock_lock);
	if ( rlen == 0 || (rlen < 0 && !is_wouldblock_error(ex_errno)))
	{
		delete_transport();
		ms_error("read_message: rlen(%d)", rlen);
		cb_transport_event(TCP_DISCONNECT, NULL, 0);
	}
	else if (rlen > 0)
	{
	    ms_report("read_message: recv msg. rlen(%d)", rlen);
		if (!find_message(rbuf, rlen))
		{
		    ms_error("read_message: parse msg error.");
		}
	}	
	
	return 0;
}


void *event_thread_func(void *arg)
{
	int res = 0;
	int keepalive_cnt = 0;
		
	while (!exit_flag)
	{
	    if (is_reconnect())
		{
		    delete_transport();
			cb_transport_event(TCP_DISCONNECT, NULL, 0);
			build_transport();
		}
		
		if (gTCPTransport)
		{
			res = gTCPTransport->Select(TCP_SELECT_WARITING_TIMEOUT, true, false);
			if (res < 0)
			{
				delete_transport();
				cb_transport_event(TCP_DISCONNECT, NULL, res);
			}
			else if (res > 0)
			{
				read_message(gTCPTransport);	
			}
			else if(keepalive_cnt++ > TCP_KEEPALIVE_TIMEOUT)
			{
				keepalive_cnt = 0;
				cb_transport_event(TCP_SENDKEEPALIVE, NULL, 0);
			}
		}
		ms_usleep(THREAD_IDLE_TIMEOUT*1000);
	}
	
	ms_report("tcp event_thread  end");
	return NULL;
}

void start_event_thread(void)
{
	exit_flag = false;
	ms_thread_create(&thread_id,NULL,event_thread_func,NULL);
}

void stop_event_thread(void)
{
	if (exit_flag)
		return;
	
	exit_flag = true;

	int waiting_time = 2000;
	int *ptime = &waiting_time;
#ifdef _WIN32
	ms_thread_join(thread_id, (void**)&ptime);
#else
	ms_thread_join(thread_id,NULL);
#endif
	thread_id = 0;
}

bool is_transport_create(void)
{
	return gTCPTransport != NULL ? true : false;
}


bool is_nonblock(void)
{
	if (gTCPTransport == NULL)
		return false;
	
	return gTCPTransport->IsNonBlock();
}

bool is_connect_ok(void)
{
	int res = 0;
	bool is_ok = false;
	
	if (gTCPTransport == NULL)
		return is_ok;
	
	res = gTCPTransport->Select(TCP_CONNECT_TIMEOUT, true, true);
	if (res > 0)
	{
		if (!gTCPTransport->IsError())
			is_ok = true;
	}

	return is_ok;
}

int update_reconnect_state(bool state)
{
	gTcpReconnectState = state;
	return 0;
}

bool is_reconnect(void)
{
	return gTcpReconnectState;
}

bool cb_transport_event(int ev_type, const char* msg, int slen)
{
	tcp_event_node_t ev_msg;

	if (EVENT_CALLBACK_FUNC == NULL)
		return false;

	memset(&ev_msg, 0, sizeof(ev_msg));
	
	switch(ev_type)
	{
		case TCP_RECV_EV:
			
			ev_msg.type = TCP_RECV_EV;
			ev_msg.len = slen;
			memcpy(ev_msg.msgbuf, msg, slen);
			EVENT_CALLBACK_FUNC(TCP_RECV_EV, &ev_msg);
			break;
		case TCP_DISCONNECT:
			strcpy(ev_msg.msgbuf, "tcp socket disconnect!!!");
			ev_msg.len = strlen(ev_msg.msgbuf);
			ev_msg.type = TCP_DISCONNECT;
			EVENT_CALLBACK_FUNC(ev_type, &ev_msg);
			ms_error("tcp transport connect faild(%d)!!", slen);
			break;
		case TCP_CONNECTED:
			strcpy(ev_msg.msgbuf, "tcp socket connect ok!!!");
			ev_msg.len = strlen(ev_msg.msgbuf);
			ev_msg.type = TCP_CONNECTED;
			EVENT_CALLBACK_FUNC(ev_type, &ev_msg);
			break;
		case TCP_SENDKEEPALIVE:
			strcpy(ev_msg.msgbuf, "tcp socket need to send keepalive!!!");
			ev_msg.len = strlen(ev_msg.msgbuf);
			ev_msg.type = TCP_SENDKEEPALIVE;
			EVENT_CALLBACK_FUNC(ev_type, &ev_msg);
			break;
		default:
			break;
	}

	return true;
}

int build_transport(void)
{
	int iRet = 0;

	/*create socket*/
	if (!is_transport_create())
	{
        iRet = create_transport();
	}

    if (0 == iRet)	
	{
		cb_transport_event(TCP_CONNECTED, NULL, 0);

        //connect successfully, set connection state
		update_reconnect_state(false);  
	}
	else
	{
		cb_transport_event(TCP_DISCONNECT, NULL, iRet);
		update_reconnect_state(false);
	}

	return iRet;
}

/*
* return 0 connect successfully, else failed
*/
int create_transport(void)
{
	int res  = 0;
	
	gTCPTransport = new CTcpClient((bool)gIPv6Enabled);
	if (gTCPTransport == NULL)
		return false;

#ifdef WIN32
	/*set socket is nonblock*/
	gTCPTransport->SetNonBlock(true);
#endif
	res = gTCPTransport->Connect(gServerAddr, gServerPort);
	if ((res < 0) || (res > 0 && !is_connect_ok()))
	{
		delete_transport();
		return -1;
	}
	
	return 0; 
}

void delete_transport(void)
{
	if (gTCPTransport != NULL)
	{
		ms_mutex_lock(&sock_lock);
		delete gTCPTransport;
		gTCPTransport = NULL;
		ms_mutex_unlock(&sock_lock);
	}
}

int tcp_set_config(const char* server_ip, int server_port)
{
	if (server_ip == NULL)
		return -1;

	strcpy(gServerAddr, server_ip);
	gServerPort = server_port;

	update_reconnect_state(true);
	
	return 0;
}

int tcp_set_tlv_enabled(int enabled)
{
    gTlvEnabled = enabled ? TRUE : FALSE;

    return 0;
}

int tcp_set_ipv6_enabled(int enabled)
{
    gIPv6Enabled = enabled ? TRUE : FALSE;
    
    return 0;
}

char* tcp_get_event_name(int evType, char* name)
{
    switch( evType )
    {
        case TCP_RECV_EV:
            strcpy(name, "RECV");
        break;

        case TCP_DISCONNECT:
            strcpy(name, "DISCONNECT");
        break;

        case TCP_CONNECTED:
            strcpy(name, "CONNECTED");
        break;

        case TCP_SENDKEEPALIVE:
            strcpy(name, "SEND KEEP ALIVE");
        break;

        case TCP_SEND_EV:
            strcpy(name, "SEND");
        break;

        case TCP_RECONNECTED:
            strcpy(name, "RECONNECTED");
        break;

        default:
            strcpy(name, "UNKNOWN");
        break;
    }

    return name;
}


int tcp_init(void)
{
	if (g_initialized) return 0;
	g_initialized=TRUE;
#ifdef _WIN32
	InitSocketClass();
#endif

	gRecvData = new(recv_data_t);
	if (gRecvData == NULL)
		return -1;
	memset(gRecvData, 0, sizeof(recv_data_t));

	/*init socket lock*/
	ms_mutex_init(&sock_lock, NULL);
		
	//start event thread
	start_event_thread();

	return 0;
}

void tcp_destroy(void)
{
	if (!g_initialized) 
		return;

	stop_event_thread();
	delete_transport();
#ifdef _WIN32	
	UninitSocketClass();
#endif
	if (gRecvData != NULL)
		delete gRecvData;
	gRecvData = NULL;

	ms_mutex_destroy(&sock_lock);

	g_initialized = FALSE;
}

int tcp_eventcallback(void (*cbfunc)(int msg_type, tcp_event_node_t* message))
{
	EVENT_CALLBACK_FUNC = cbfunc;
	return 0;
}

int tcp_send(const char* data, int len)
{	
	int res = 0;
	if (data == NULL || len == 0)
		return -1;
	
	return send_message(gTCPTransport, data, len);
}

int tcp_send_keeaplive(void)
{	
	return send_keepalive(gTCPTransport);
}


int tcp_force_closed(void)
{
	delete_transport();
	return 0;
}


