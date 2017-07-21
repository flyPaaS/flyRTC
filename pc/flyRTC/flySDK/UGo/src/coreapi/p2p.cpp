#include <string.h>
#include "tmtable.h"
#include "p2p.h"
#include "me.h"
#include "mq.h"
#include "pcp.h"


#ifdef ICE_ENABLED

static p2p_cb_vtable_t 	g_p2pcbtable;  /*p2p callback table*/
static ice_sess_param_t	g_iceparam;   /*ice callback param*/
static void reset_cbtable(void);
static void set_cbtable(p2p_cb_vtable_t* pvtable);
static void reset_ice_param(void);

const int  kMinImprovement = 50; /*50ms*/
const int  kMinPplValue = 5; /*ppl 5*/

static volatile int 	g_threadexit = FALSE;
static ms_thread_t 		g_threadid = 0;

static int get_total_delay(int rtt, int ppl);
static int get_ice_candpair(const unsigned int cmp_id, const ice_bestcandpairs_t* ice_ev, ice_candpair_t* candpair);
static void p2p_thread_start(void);
static void p2p_thread_stop(void);
#if defined(ANDROID)
static int p2p_iteration_thread(void *arg);
#else
static void *p2p_iteration_thread(void *arg);
#endif
static void process_ice_event(ice_event_node_t* pev);
static int p2p_filter_recv_packet(int compid, char* remaddr);
static int p2p_get_remote_addr(IN char *remaddr, int& port, char* ipaddr);

#if defined(ANDROID)
int p2p_set_android_obj(void* jvm, void* context)
{
    return iceapi_set_android_obj(jvm, context);
}
#endif

void p2p_thread_start(void)
{
    int ret = 0;
	g_threadexit = FALSE;
#if defined(ANDROID)
    iceapi_create_thread("p2pEvent", &p2p_iteration_thread, NULL, 5);
#else
	ret = ms_thread_create(&g_threadid, NULL, p2p_iteration_thread, NULL);
    if (ret != 0)
    {
      ms_error("p2p_thread_start: create p2p thread error!");
    }
#endif	
}

void p2p_thread_stop(void)
{
#if defined(ANDROID)
    g_threadexit = TRUE;
    iceapi_join_thread();
    g_threadid = 0;
#else
	int waiting_time = 2000;
	int *ptime = &waiting_time;

	g_threadexit = TRUE;
	ms_thread_join(g_threadid, (void**)&ptime);
	g_threadid = 0;
#endif	
}

#if defined(ANDROID)
int p2p_iteration_thread(void *arg)
{
	void* evdata = NULL;
	MSList* evlist = NULL;

	//while (!g_threadexit)
	{ 
		/*ice event*/
		evdata = mq_get_node(MQ_ICE_TP);
		if(evdata)
		{ 
			process_ice_event((ice_event_node_t*)evdata);
			mq_remove_node(MQ_ICE_TP, evdata);
		}
        else
        {
            ms_usleep(5 * 1000);  
        }
	}
	
	return 0;
}
#else
void *p2p_iteration_thread(void *arg)
{
	void* evdata = NULL;
	MSList* evlist = NULL;

	while (!g_threadexit)
	{ 
		/*ice event*/
		evdata = mq_get_node(MQ_ICE_TP);
		if(evdata)
		{ 
			process_ice_event((ice_event_node_t*)evdata);
			mq_remove_node(MQ_ICE_TP, evdata);
		}
        else //yangjj modify 2016-3-25
        {
          ms_usleep(5 * 1000);
        }
		
	}

	ms_report("p2p_iteration_thread  exit ok!!!");
	
	return NULL;
}
#endif

void process_ice_event(ice_event_node_t* pev)
{
	if (pev != NULL)
	{
		switch(pev->ev_type)
		{
			case eICE_EVENT_MSG_TP:
				p2p_do_ice_event_msg((ice_event_msg_t*)pev->ev_param);
				break;
			case eICE_RECV_MSG_TP:
				p2p_do_ice_recv_msg((ice_rtp_msg_t*)pev->ev_param);
				break;
			case eICE_SEND_MSG_TP:
				p2p_do_ice_send_msg((ice_rtp_msg_t*)pev->ev_param);
				break;
			default:
				ms_error("process_ice_event: not support ev_type = %d", pev->ev_type);
				break;
		}

		/*free ev param object*/
		if (pev->ev_param)
			ms_free(pev->ev_param);
		pev->ev_param = NULL;
	}
}

int get_total_delay(int rtt, int ppl)
{
	int total_delay = rtt;

	if (ppl > kMinPplValue)
	{
		total_delay += ppl*ppl;
	}
	
	return total_delay;
}

int get_ice_candpair(const unsigned int cmp_id, const ice_bestcandpairs_t* ice_ev, ice_candpair_t* candpair)
{
	int res = RES_ERR;

	if (ice_ev == NULL || candpair == NULL )
		return RES_ERR;

	for (int i= 0; i < ice_ev->candpairs_num; i++)
	{
		if (cmp_id == (unsigned int)ice_ev->bcandpairs[i].comp_id)
		{
			memcpy(candpair, &ice_ev->bcandpairs[i], sizeof(ice_candpair_t));
			res = RES_OK;
			break;
		}
	}

	return res;
}


void reset_cbtable(void)
{
	memset(&g_p2pcbtable, 0, sizeof(g_p2pcbtable));
}

void set_cbtable(p2p_cb_vtable_t* pvtable)
{
	if (pvtable != NULL)
		memcpy(&g_p2pcbtable, pvtable, sizeof(g_p2pcbtable));
}

void reset_ice_param(void)
{
	memset(&g_iceparam, 0, ICE_SESS_PARAM_SIZE);
	g_iceparam.evtpara.mode = -1;
}

int p2p_set_cfg(ice_config_t* pcfg)
{
  if (pcfg == NULL)
  {
    ms_error("p2p_set_cfg failed.");
    return P2P_ERR;
  }
	return iceapi_set_config(pcfg);
}

int p2p_cbvtable(p2p_cb_vtable_t* pvtable)
{
	set_cbtable(pvtable);
	return P2P_OK;
}

int p2p_init()
{	
	iceapi_event_callback(g_p2pcbtable.event_cb);
	iceapi_recv_callback(g_p2pcbtable.recv_cb);

    if (iceapi_init() < 0)
    {
      ms_error("ice init error!!!");
      return P2P_ERR;
    }
	reset_ice_param();

    /*start p2p event thread*/
	p2p_thread_start();
    
	return P2P_OK;
}

void p2p_destroy(void)
{
	p2p_thread_stop();
	iceapi_destroy();
}

void p2p_clean_param(void)
{
	reset_ice_param();
}

void p2p_store_ev_param(IN ice_event_msg_t* param)
{		
	memcpy(&g_iceparam.evtpara, param, ICE_EV_MSG_T_SIZE);
}

void p2p_store_relay_param(IN const ice_sdpinfo_t* ice_sdp)
{
    unsigned int i = 0;
    
    if ( NULL == ice_sdp )
    {
        return;
    }

    strncpy(g_iceparam.relayinfo.ice_defaddr, ice_sdp->ice_defaddr, \
        MAX_ICE_ADDR_LEN -1 );
    
    for ( i = 0; i < ICE_MAX_COMP; i++ )
    {
        g_iceparam.relayinfo.ice_defport[i] = ice_sdp->ice_defport[i];
    }
}

int p2p_is_detection_finished(void)
{
	return (g_iceparam.evtpara.mode == -1 ? FALSE : TRUE);
}

OUT int p2p_get_transport_mode(void)
{
	return g_iceparam.evtpara.mode;
}

int p2p_get_local_ice_sdp(OUT ice_sdpinfo_t* pice)
{
	ice_authinfo_t iLocalauth;
	ice_candidates_t iLocalcands;

	if (pice == NULL)
		return P2P_ERR;

	cm_resst(pice, sizeof(ice_sdpinfo_t));
	cm_resst(&iLocalauth, sizeof(iLocalauth));
	cm_resst(&iLocalcands, sizeof(iLocalcands));

	if (iceapi_get_ice_authinfo(&iLocalauth) < 0)
	{
		ms_warning("iceapi_get_ice_authinfo:get local auth info faild!!!");
		return P2P_ERR;
	}

	if (iceapi_get_local_candidates(&iLocalcands) < 0)
	{
		ms_warning("iceapi_get_local_candidates:get local candidates info faild!!!");
		return P2P_ERR;
	}

	memcpy(&pice->ice_auth, &iLocalauth, sizeof(iLocalauth));
	memcpy(&pice->ice_cands, &iLocalcands, sizeof(iLocalcands));
	
	return P2P_OK;
}

int p2p_get_remote_ice_sdp(OUT ice_sdpinfo_t* pice)
{
	int cand_num = 0;
	int i = 0;
	pcp_ice_info_t iceparam;
	pcp_relay_info_t relay;

	if (NULL == pice)
		return P2P_ERR;

	cm_resst(&iceparam, sizeof(iceparam));
	if (pcp_remote_ice_params(&iceparam) < 0)
		return P2P_ERR;

	cm_resst(&relay, sizeof(relay));
	if (pcp_media_relay_params(&relay) < 0)
		return P2P_ERR;

	if (pcp_media_stream_enabled(AUDIO))
	{
		/*remote audio rtp default relay info */
		pice->ice_defport[eICE_COMP_AUDIO_RTP-1] = relay.a_port;
		strcpy(pice->ice_defaddr, relay.m_ip);

		/*if max comp > 1, need rtcp default addr*/
		if (ICE_MAX_COMP > 1)
		{
			pice->ice_defport[eICE_COMP_AUDIO_RTCP-1] = relay.a_port+1;
			strcpy(pice->ice_defaddr, relay.m_ip);
		}
	}

	if (pcp_media_stream_enabled(VIDEO))
	{
		/*remote video rtp default relay info */
		pice->ice_defport[eICE_COMP_VIDEO_RTP-1] = relay.v_port;
		strcpy(pice->ice_defaddr, relay.m_ip);

		/*if max comp > 1, need rtcp default addr*/
		if (ICE_MAX_COMP > 1)
		{
			pice->ice_defport[eICE_COMP_VIDEO_RTCP-1] = relay.v_port+1;
			strcpy(pice->ice_defaddr, relay.m_ip);
		}
	}
	
	pice->ice_cands.cand_num = iceparam.cdd_num;
	strcpy(pice->ice_auth.ice_pwd, iceparam.pwd);
	strcpy(pice->ice_auth.ice_ufrag, iceparam.ufrag);
	
	for(i = 0; i< iceparam.cdd_num; i++)
	{
		strcpy(pice->ice_cands.cand_info[i].str, iceparam.cdd_arr[i].attr_desc);
		pice->ice_cands.cand_info[i].str_len = strlen(iceparam.cdd_arr[i].attr_desc);
	}	

	return P2P_OK;
}


int p2p_update_ice_media_stream(void)
{
	ice_sdpinfo_t iceparam;
	cm_resst(&iceparam, sizeof(iceparam));
	if (p2p_get_remote_ice_sdp(&iceparam) < 0)
	{
		ms_error("start_media_stream:get remote param error, media start faild!!!");
		return RES_ERR;
	}
	
	if (!iceapi_is_start_nego() && !p2p_is_detection_finished())
	{
		iceapi_set_remote_sdp_info(&iceparam);
        p2p_store_relay_param(&iceparam);
		ms_report("start_media_stream:update ice remote info ok!!!");
	}
	else
	{
		iceapi_update_relay_info(&iceparam);
		
		p2p_store_relay_param(&iceparam);
		ms_report("start_media_stream:update ice relay info ok!!!");
	}	

	return P2P_OK;
}


int p2p_start_detection(void)
{
	int res = P2P_OK;

	if (!iceapi_is_start_nego() && !p2p_is_detection_finished())
	{
		res = iceapi_start_nego();
		tmt_start_timer(TM_P2P_DETECTION_T0); //8s
		ms_report("p2p_start_detection: start p2p detection timeout timer ok!!!");
	}

	return res;
}

int p2p_stop_detection(void)
{
	/* Added by gonghuojin, 2014/5/28  close all p2p timer*/
	tmt_stop_all_timers();
	
	/*delete ice session*/
	iceapi_delete_session();
	
	/*clean ice param*/
	p2p_clean_param();

	return P2P_OK;
}

int p2p_do_ice_event_msg(IN ice_event_msg_t* pev)
{
	ice_candpair_t pair;
	pcp_session_media_t m_stream;

	if (pev != NULL)
	{
		/*stop ice timeout timer*/
		tmt_stop_timer(TM_P2P_DETECTION_T0);
		ms_report("process_ice_event: stop p2p detection timeout timer!!!");

		/*if ice have timeout, we drop this event*/
		if (p2p_is_detection_finished())
		{
			ms_report("process_ice_event: p2p have finish or timeout, drop this event!!!");
			return -1;
		}

#ifdef EXTERNAL_TRANSPORT
		if (pev->mode == ICE_P2P)
		{
			/*start ping timer*/
			tmt_start_timer(TM_P2P_SENDPING_T2);
			/*start check timer*/
			tmt_start_timer(TM_P2P_CHECKLINE_T1);
			iceapi_update_mode(ICE_P2P);
			me_update_ice_mode(eME_ICE_P2P_MD);
			ms_report("process_ice_event: start p2p ping and line check timer!!!");
		}
		else
		{
			iceapi_update_mode(ICE_RTPP);
            me_update_ice_mode(eME_ICE_RTPP_MD);
			ms_report("process_ice_event: p2p nego fiald! use rtpp line!!");
		}
#else
		/* delete ice session */
		iceapi_delete_session();
		ms_report("process_ice_event: ice session delete!!!");
#endif

		/*if ice nego success, update best candidate pairs*/
		if (pev->mode == ICE_P2P)
		{
			/*update audio*/
			if (pcp_media_stream_enabled(AUDIO))
			{
				cm_resst(&pair, sizeof(pair));
				if (get_ice_candpair(eICE_COMP_AUDIO_RTP, &pev->best_cands, &pair) == 0)
				{
					cm_resst(&m_stream, sizeof(m_stream));
	
					pcp_best_mediastream(AUDIO, &m_stream);
					m_stream.trans_mode = pev->mode;
					m_stream.local_port = pair.local_port;
					strcpy(m_stream.local_addr, pair.local_addr);
					m_stream.remote_port = pair.remote_port;
					strcpy(m_stream.remote_addr, pair.remote_addr);
					pcp_update_mediastream(AUDIO, &m_stream);					
					ms_report("process_ice_event:update best audio media addr ok!!!");
				}
			}
	
			/*update video*/
			if ( pcp_media_stream_enabled(VIDEO))
			{
				cm_resst(&pair, sizeof(pair));
				if (get_ice_candpair(eICE_COMP_VIDEO_RTP, &pev->best_cands, &pair) == 0)
				{
					cm_resst(&m_stream, sizeof(m_stream));
					
					pcp_best_mediastream(VIDEO, &m_stream);
					m_stream.trans_mode = pev->mode;
					m_stream.local_port = pair.local_port;
					strcpy(m_stream.local_addr, pair.local_addr);
					m_stream.remote_port = pair.remote_port;
					strcpy(m_stream.remote_addr, pair.remote_addr);
					pcp_update_mediastream(VIDEO, &m_stream);
					ms_report("process_ice_event:update best video media addr ok!!!");
				}
			}
		}

		/*store ice param*/
		p2p_store_ev_param(pev);
	}

	return 0;
}

/*  get ipaddr and port from remote addr string which combine in pj_sockaddr_print func
  * remote addr format, eg:
  * ipv4: 192.168.1.110:51334 
  * ipv6: [2000:0000:0000:0000:0001:2345:6789:abcd]:8080 
  */
static int p2p_get_remote_addr(IN char *remaddr, int& port, char* ipaddr)
{
    char *ptr = NULL;
    char delim[] = ":";
    
    if ( NULL == remaddr || NULL == ipaddr )
    {
        return -1;
    }

    // ipv4
    if ( remaddr[0] != '[' )
    {
        ptr = strtok(remaddr, delim);
        if ( NULL != ptr )
        {
            strcpy(ipaddr, ptr);
        }

        ptr = strtok(NULL, delim);
        if ( NULL != ptr )
        {
            port = atoi(ptr);
        }
    }
    else //ipv6
    {
        char * tmp = NULL;
        ptr = strstr(remaddr, "]");
        strncpy(ipaddr, &remaddr[1], ptr - (remaddr + 1));
        
        if ( NULL != ptr )
        {
            tmp = strtok(ptr, delim);
            tmp = strtok(NULL, delim);
            if ( NULL != tmp )
            {
                port = atoi(tmp);
            }
        }
    }

    return 0;
}

/* 
* Try to drop packets when follow conditions happened
* 1. best paris size is zero or default relay addr is empty
* 2. remote addr not match, mode p2p 
* 3. def addr not match, mode rtpp */
static int p2p_filter_recv_packet(int compid, char* remaddr)
{
    char ipaddr[MAX_IP_ADDRSTR_LEN] = { 0 };
	int port = 0;
	
    /* drop it if from unknown source */
    if ( NULL == remaddr )
    {
        ms_error("p2p_filter_recv_packet remote addr is unknown.");
        return -1;
    }

    if ( g_iceparam.evtpara.best_cands.candpairs_num == 0 
        && g_iceparam.relayinfo.ice_defaddr[0] == '\0' )
    {
        ms_error("p2p_filter_recv_packet relay and candpairs addr unknown.");
        return -1;
    }
    
    p2p_get_remote_addr(remaddr, port, ipaddr);

    /* add by VintonLiu on 20151231 for one side is p2p, another side is rtpp, cause singlepass */
    /* if remote peer was negotiated failed, will use internal voice channel, for avoid singlepass, 
        * need the local side force switch to rtpp channel that in ice to send packets.
        * 150 = (1000/20) * 3 */
    if ( g_iceparam.recvPkts <= 150 && iceapi_get_mode() == ICE_P2P )
    {
        if ( (!strcmp(ipaddr, g_iceparam.relayinfo.ice_defaddr))
            && (port == g_iceparam.relayinfo.ice_defport[compid-1])
            && (g_iceparam.recvPkts == 150) )
        {
            iceapi_update_mode(ICE_RTPP);
            me_update_ice_mode(eME_ICE_RTPP_MD);
            g_iceparam.force_switch = TRUE;
            ms_report("p2p_filter_recv_packet: force switch to rtpp.");
        }
        g_iceparam.recvPkts++;
    }
    /* VintonLiu add end */

    if ( (!strcmp(ipaddr, g_iceparam.evtpara.best_cands.bcandpairs[compid-1].remote_addr)
            && port == g_iceparam.evtpara.best_cands.bcandpairs[compid-1].remote_port)
         || (!strcmp(ipaddr, g_iceparam.relayinfo.ice_defaddr)
            && port == g_iceparam.relayinfo.ice_defport[compid-1]))
    {
        return 0;
    }

#if 1 //Debug
    ms_error("p2p_filter_recv_packet: recv addr[%s:%d]", ipaddr, port);
    ms_error("p2p_filter_recv_packet: best addr[%s:%d]", \
            g_iceparam.evtpara.best_cands.bcandpairs[compid-1].remote_addr, \
            g_iceparam.evtpara.best_cands.bcandpairs[compid-1].remote_port);
    ms_error("p2p_filter_recv_packet: def addr[%s:%d]", \
            g_iceparam.relayinfo.ice_defaddr, \
            g_iceparam.relayinfo.ice_defport[compid-1]);
#endif
    return -1;
}

int p2p_do_ice_recv_msg(IN ice_rtp_msg_t* pev)
{
	recv_pm_t pm;
		
	if ( NULL == pev )
		return -1;

	if (pev->rlen == 0)
		return -1;

	if ( pev->compid < eICE_COMP_AUDIO_RTP 
	    || pev->compid > eICE_COMP_VIDEO_RTCP )
    {
        ms_error("on_recv_data_cb: comp id=%d unknow.", pev->compid);
        return -1;
    }
    
    if ( p2p_filter_recv_packet(pev->compid, pev->remaddr) < 0 )
    {
        ms_error("p2p_do_ice_recv_msg drop the packet.");
        return -1;
    }
    
	res_st(&pm, sizeof(pm));

	switch(pev->compid)
	{
		case eICE_COMP_AUDIO_RTP:
			pm.media_type = eME_MEDIA_AUDIO;
			pm.data_type = eME_MEDIA_RTP;
			pm.data = pev->rbuf;
			pm.len = pev->rlen;
			me_recv_data_packet(&pm);
			break;
		case eICE_COMP_AUDIO_RTCP:
			pm.media_type = eME_MEDIA_AUDIO;
			pm.data_type = eME_MEDIA_RTCP;
			pm.data = pev->rbuf;
			pm.len = pev->rlen;
			me_recv_data_packet(&pm);
			break;
		case eICE_COMP_VIDEO_RTP:
			pm.media_type = eME_MEDIA_VIDEO;
			pm.data_type = eME_MEDIA_RTP;
			pm.data = pev->rbuf;
			pm.len = pev->rlen;
			me_recv_data_packet(&pm);
			break;
		case eICE_COMP_VIDEO_RTCP:
			pm.media_type = eME_MEDIA_VIDEO;
			pm.data_type = eME_MEDIA_RTCP;
			pm.data = pev->rbuf;
			pm.len = pev->rlen;
			me_recv_data_packet(&pm);
			break;
		default:
			ms_error("on_recv_data_cb: comp id=%d unknow.", pev->compid);
			break;
	}

	return 0;
}

int p2p_do_ice_send_msg(IN ice_rtp_msg_t* pev)
{
	if (NULL == pev)
		return -1;

	if (pev->rlen == 0)
		return -1;
	
	return iceapi_send_media(pev->compid, pev->rbuf, pev->rlen);;
}

/*ice send ping data 1s timer callback*/
int p2p_do_ice_sendping_tm(void)
{
	/*send ping data ,this we only need ping one comp*/
	iceapi_send_ping(eICE_COMP_AUDIO_RTP);
	iceapi_send_ping(eICE_COMP_AUDIO_RTCP);//yangjj add

  if (pcp_media_stream_enabled(VIDEO))
  {
    iceapi_send_ping(eICE_COMP_VIDEO_RTP);
    iceapi_send_ping(eICE_COMP_VIDEO_RTCP);//yangjj add
  }
	

	return RES_OK;
}

/*ice line check 30s timer callback*/
int p2p_do_ice_checkline_tm(void)
{
    bool is_p2p = false; //add by yangjj 2016-2-29
	int rtpp_delay = 0;
	int p2p_delay = 0;
	int neteq_rtt = 0;

	ice_pingline_t ice_line;
	if (pcp_get_normalcall_to_conference() != TRUE)
	{
		cm_resst(&ice_line, sizeof(ice_line));
		/*get ice line rtt and ppl value*/
		iceapi_get_line(eICE_COMP_AUDIO_RTP, &ice_line);	

        //modify by yangjj 2016-2-29   ms_report
        
        //When the remote ICE to create success, and started using p2p transmission, 
        //allow the local to p2p and RTPP switch.When a delay greater than zero, 
        //and packet loss rate is less than thirty percent, the remote ICE has to create success.
        if (ice_line.p2p_rtt > 0 && ice_line.p2p_ppl < 30)
        {
          g_iceparam.force_switch = FALSE;
        }

        //Choose 300 ms because consider audio delay does not affect the hearing within 300 ms delay
        
        if (ice_line.p2p_rtt < 300 && ice_line.p2p_ppl < 25)
        {
          //When p2p_rtt is less than 300 milliseconds, consider RTPP at the same time under the condition \
          // of less than 300 milliseconds to reduce RTPP server stress, prefer using p2p.
          is_p2p = true;
        }
        else
        {
          if (ice_line.rtpp_rtt < 300 && ice_line.rtpp_ppl < 25)
          {
            is_p2p = false;
          }
          else if (ice_line.rtpp_rtt > ice_line.p2p_rtt)
          {
            is_p2p = true;
          }
          else
          {
            is_p2p = false;
          }
        }
        
        if (is_p2p)
        {
          if (iceapi_get_mode() != ICE_P2P && !g_iceparam.force_switch)
          {
            iceapi_update_mode(ICE_P2P);
            me_update_ice_mode(eME_ICE_P2P_MD);
            pcp_trace_line_change(ICE_P2P);
            ms_report("do_ice_checkline_tm: ##best line change to p2p.##");
          }
        }
        else
        {
          if (iceapi_get_mode() != ICE_RTPP)
          {
            iceapi_update_mode(ICE_RTPP);
            me_update_ice_mode(eME_ICE_RTPP_MD);
            pcp_trace_line_change(ICE_RTPP);
            ms_report("do_ice_checkline_tm: ##best line change to rtpp.##");
          }
        }

        ms_report("do_ice_checkline_tm:rtpp_rtt[%d],p2p_rtt[%d],rtpp_ppl[%d],p2p_ppl[%d],ice_mode[%d].\n", ice_line.rtpp_rtt,
          ice_line.p2p_rtt, ice_line.rtpp_ppl, ice_line.p2p_ppl, iceapi_get_mode());
		//modify by yangjj end
	}
	
	return RES_OK;
}

/*ice nego timeout event 8s  timer callback*/
int p2p_do_ice_detection_tm(void)
{
	ms_report("do_ice_detection_tm: ice detection timeout!!!");

	/*store ice param*/
	ice_event_msg_t ice_ev;
	res_st(&ice_ev, sizeof(ice_ev));
	ice_ev.mode = ICE_RTPP;
	p2p_store_ev_param(&ice_ev);

	/*update ice mode*/
	iceapi_update_mode(ICE_RTPP);
	me_update_ice_mode(eME_ICE_RTPP_MD);

	return RES_OK;
}


#endif

