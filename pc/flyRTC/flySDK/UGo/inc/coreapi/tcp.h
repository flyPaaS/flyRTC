#ifndef TCP_TRANSPORT_H_
#define TCP_TRANSPORT_H_

enum tcp_event_type
{
	TCP_RECV_EV		  = 0,
	TCP_DISCONNECT	  = 1,
	TCP_CONNECTED	  = 2,
	TCP_SENDKEEPALIVE = 3,
	TCP_SEND_EV		  = 4,
	TCP_RECONNECTED   = 5
};

#define MAX_MSG_LEN		2049
typedef struct tcp_event_node_t
{
	int  type;
	int  len;
	char msgbuf[MAX_MSG_LEN];
}tcp_event_node_t;

#define TCP_EVENT_NODE_T_SIZE  (sizeof(tcp_event_node_t))

int tcp_set_config(const char* server_ip, int server_port);

int tcp_set_tlv_enabled(int enabled);

int tcp_set_ipv6_enabled(int enabled);

char* tcp_get_event_name(int evType, char* name);

int tcp_init(void);

void tcp_destroy(void);

int tcp_eventcallback(void (*cbfunc)(int ev_type, tcp_event_node_t* message));

int tcp_send(const char* data, int len);

int tcp_send_keeaplive(void);

int tcp_force_closed(void);

#endif
