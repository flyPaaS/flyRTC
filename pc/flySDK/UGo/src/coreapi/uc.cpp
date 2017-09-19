#include "logtrace.h"
#include "tmtable.h"

#include "uc.h"
#include "tcp.h"
#include "me.h"
#include "mq.h"
#include "pcp.h"
#include "pcp_errcode.h"
#include "cfg.h"
#include "pcp_cfg.h"


static volatile int 	ethread_flag = FALSE;
static ms_thread_t 		ethread_id   = 0;

#ifdef ICE_ENABLED
#include "p2p.h"
static void on_ice_ev_cb(int ev_result, unsigned elapsed_time, ice_bestcandpairs_t* ice_bcandpairs);
static int update_p2p_cfg(void);
#endif
static int init_pcp_cfg(void);
static int update_pcp_cfg(void);
static int update_tcp_cfg(void);
static int update_me_cfg(uc_cb_vtable_t* pvtable);

static int is_call_temporary_reason(int reason);
static int is_call_error_reason(int reason);
static int is_call_ice_enabled(void); 
static int is_in_trans_support(void);
static int uc_call_ack(void);
static int uc_call_ringing(void);
static int uc_call_in_trans_req();
static int uc_call_in_trans_rsp();
static int update_media_stream_ice_info(void);
static int force_start_media_stream(int uflag);
static int start_media_stream(void);
static int stop_media_stream();
static int force_early_media_enabled(bool enable);
static int update_early_media_enabled(void);
static int update_media_stream(void);
static int restart_media_stream(void);
static int restart_media_stream2(void);
static int get_version(char* version);
static int check_codecs(const pcp_codec_list_t* pcodecs, int pt);
static int check_port(int port, int type);

static void *ev_iteration_thread(void *arg);
static void ev_thread_start(void);
static void ev_thread_stop(void);

static void on_tm_cb(int timeid);
static void on_tcp_send_cb(const char* msg, int slen);
static void on_tcp_cb(int ev_type, tcp_event_node_t* message);
static void on_meg_cb(int ev_type, int ev_reason, const char* something, void* param, int size);
static void on_pcp_cb(int ev_type, int ev_reason, const char* something, void* param, int size);
static void on_pcp_register_cb(int ev_type, int ev_reason, const char* something);
static void on_trace(int level, const char* something);
static void on_state_cb(int state);

#ifdef EXTERNAL_TRANSPORT
static int on_send_data_cb(int media_type, int data_type, const void* data, int len);
static void on_recv_data_cb(int comp_id, const void* data, int len, const char* remote_addr);
#endif

static void process_tcp_event(tcp_event_node_t* pev);
static void process_pcp_event(pcp_event_node_t* pev);
static void process_meg_event(ME_event_node_t* pev);
static void process_log_event(trace_event_node_t* pev);
static void process_tmt_event(tm_event_node_t* pev);

#if defined(ANDROID)
int uc_set_android_obj(media_jni_env_pm_t* env)
{
#ifdef ICE_ENABLED    
    p2p_set_android_obj(env->jvm, env->context);
#endif

    return me_set_android_obj(env);
}
#endif

int check_port(int port, int type)
{
	int def_port = kDefaultAudioPort;
	
	if (type == VIDEO)
		def_port = kDefaultVideoPort;
	
	return (port==def_port?0:port);
}

int check_codecs(const pcp_codec_list_t* pcodecs, int pt)
{
	int i = 0;
	
	if (NULL == pcodecs)
		return -1;

	for (i = 0; i < pcodecs->num; i++)
	{
		if (pcodecs->codec_arr[i].pt == pt)
			return i;
	}

	return -1;
}

static int init_pcp_cfg(void)
{
    pcp_cb_vtable_t vtable;

    vtable.register_event_cb = on_pcp_register_cb;
	vtable.call_event_cb     = on_pcp_cb;
	vtable.call_send_cb      = on_tcp_send_cb;
	vtable.call_log_cb       = on_trace;
	vtable.call_state_cb     = on_state_cb;

    return pcp_set_cb_vtable(&vtable);
}

int update_pcp_cfg(void)
{
	pcp_cfg_t pcp_cfg;

	cm_resst(&pcp_cfg, sizeof(pcp_cfg));
	pcp_cfg.proxy.update_flag = PCP_UPDATE_OPEN | PCP_CONFER_OPEN;
    pcp_cfg.proxy.update_flag |= (PCP_FEC_OPEN | PCP_DIRECTFEC_OPEN);
    pcp_cfg.proxy.update_flag |= PCP_DIRECTFEC_MULRIPLE_OPEN;
    // tmp closed cause vps not ready
    if ( 0 && pm_is_tlv_enabled() )
    {
        pcp_cfg.proxy.update_flag |= PCP_IN_TRANS_OPEN;
    }
	pcp_cfg.proxy.ice_type         = PCP_ICE_AUTO_TP;
	pcp_cfg.proxy.ice_enabled      = pm_is_local_ice_enabled();
	pcp_cfg.proxy.rc4_enabled      = pm_is_local_rc4_enabled();
	pcp_cfg.proxy.compress_enabled = pm_is_compress_enabled();
	pcp_cfg.proxy.video_enabled    = pm_is_local_video_enabled();
	pcp_cfg.proxy.platform         = pm_get_platform();
	pcp_cfg.proxy.atype            = pm_get_account_type();
	pcp_cfg.proxy.tlv_enabled      = pm_is_tlv_enabled();
    pcp_cfg.proxy.ipv6_enabled     = pm_is_ipv6_enabled();
    pcp_cfg.proxy.csrv_enabled     = pm_is_csrv_enabled();

	get_version(pcp_cfg.proxy.version);
	pm_get_uid(pcp_cfg.proxy.uid);
	pm_get_userid(pcp_cfg.proxy.userid);
	pm_get_userphone(pcp_cfg.proxy.phone);
	pm_get_brand(pcp_cfg.proxy.brand);
	pm_get_nickname(pcp_cfg.proxy.nickname);
    pm_get_localipaddr(pcp_cfg.proxy.localipaddr);

	ms_report("update_pcp_cfg:ice_type[%d],rc4[%d],video[%d], tlv[%d]!", \
	        pcp_cfg.proxy.ice_type,	pcp_cfg.proxy.rc4_enabled, \
	        pcp_cfg.proxy.video_enabled, pcp_cfg.proxy.tlv_enabled);
	
	return pcp_set_config(&pcp_cfg);
}


int update_tcp_cfg()
{
	if (pm_is_tcp_enabled())
	{
		char svr_s[64];
		sv_addr_t sv_addr;
		cm_resst(&sv_addr, sizeof(sv_addr_t));
		pm_get_tcp_serveraddr(svr_s);
		cm_paser_address(svr_s, &sv_addr);
		tcp_set_config(sv_addr.sv_ip, sv_addr.sv_port);
		tcp_set_tlv_enabled(pm_is_tlv_enabled());
        tcp_set_ipv6_enabled(pm_is_ipv6_enabled());
		tcp_eventcallback(on_tcp_cb);
		tcp_init();
	
		ms_report("tcp_init ok!!!!!!");	
	}

	return PCP_OK;
}

int update_me_cfg(uc_cb_vtable_t* pvtable)
{
	ME_cb_vtable_t me_vtable;

	cm_resst(&me_vtable, sizeof(ME_cb_vtable_t));
	me_vtable.event_cb = on_meg_cb;
	/*if use external transport */
#if defined (EXTERNAL_TRANSPORT) && defined (ICE_ENABLED) 
	me_vtable.send_cb       = on_send_data_cb;
#else
	me_vtable.send_cb  = NULL;
#endif
	me_vtable.log_cb   = NULL;
    me_vtable.encrypt  = pvtable->encrypt;
    me_vtable.decrypt  = pvtable->decrypt;
	me_vtable.screenshot_cb = pvtable->screenshot_cb;
	me_vtable.extmedia_cb   = pvtable->extmedia_cb;
	
    /* add by Vinton.Liu on 2016-03-21 for support external audio device push pcm to and pull pcm from engine begin */
	me_vtable.initplayout_cb = pvtable->initplayout_cb;
	me_vtable.initrecording_cb = pvtable->initrecording_cb;
	me_vtable.pull_capture_cb = pvtable->pull_capture_cb;
	me_vtable.push_render_cb = pvtable->push_render_cb;
    /* Vinton.Liu add end */

	return me_callback_vtable(&me_vtable);
}

int is_call_ice_enabled(void)
{
	int enabled = FALSE;
#ifdef ICE_ENABLED
	enabled = pcp_ice_enabled();
#endif
	return enabled;
}

int is_in_trans_support(void)
{
    return pcp_in_trans_support_enabled();
}

int is_call_temporary_reason(int reason)
{
	int istmpreason = FALSE;

	if (reason <= PCP_TMP_REASON_MAX_CODE 
		&& reason >= PCP_TMP_REASON_MIN_CODE)
	{
		istmpreason = TRUE;
	}

	return istmpreason;
}


int is_call_error_reason(int reason)
{
	int iserrreason = FALSE;

	if (reason != PCP_SUCCESS && !is_call_temporary_reason(reason))
	{
		iserrreason = TRUE;
	}

	return iserrreason;
}


int update_media_stream_ice_info(void)
{
#ifdef ICE_ENABLED
	p2p_update_ice_media_stream();
	p2p_start_detection();
#endif
	return RES_OK;
}

int force_start_media_stream(int uflag)
{
	int state = PCP_STATE_WAIT;	
	ME_audio_stream_info_t maudio;
	ME_video_stream_info_t mvideo;
	if (eME_Connected != me_get_state())
	{
		pcp_bool trace_flag = FALSE;
		pcp_bool relay_flag = FALSE;
		pcp_relay_info_t relay;
		pcp_session_media_t m_stream;

#if 0   //modify by wuzhaoyang for fixed the reinvite problem 20150529
    	if (!is_call_ice_enabled())
#else
		if (me_get_ice_mode())
#endif
    	{
    		me_update_ice_mode(eME_ICE_RTPP_MD);
    		/*if it is update restart, we need to get relay info*/
    		if (uflag)
    		{
    			relay_flag = TRUE;
    			cm_resst(&relay, sizeof(relay));
    			pcp_media_relay_params(&relay);
    		}
    	}
    	
    	if (pcp_media_stream_enabled(AUDIO))
    	{
    		cm_resst(&m_stream, sizeof(m_stream));
    		pcp_best_mediastream(AUDIO, &m_stream);//get audio media info from pcp
    		if (relay_flag)
    		{
    			strcpy(m_stream.remote_addr, relay.m_ip);
    			m_stream.remote_port = relay.a_port;
    			pcp_update_mediastream(AUDIO, &m_stream);
    		}

        	cm_resst(&maudio, sizeof(maudio));
        	maudio.iPayLoadType     = m_stream.session_pt;
			//ms_report("wuzhaoyang maudio.iPayloadType = %d", maudio.iPayLoadType);//add by wuzhaoyang20141124 for debug
            maudio.iRemoteAudioPort = m_stream.remote_port;
            maudio.iLocalAudioPort = m_stream.local_port;
            strcpy(maudio.cRemoteAudioIp, m_stream.remote_addr);
#ifdef EXTERNAL_TRANSPORT
            if (p2p_get_transport_mode() != ICE_P2P)
              maudio.ucExTransportEnable = FALSE;
            else
              maudio.ucExTransportEnable = TRUE;
#else
        	maudio.ucExTransportEnable = FALSE;
#endif
            /* add by VintonLiu on 20151202 for direct call */
            if ( PCP_DIRECT_CM == uc_get_callmode() )
			{
			    maudio.isDirectCall = TRUE;
			}
			else
			{
			    maudio.isDirectCall = FALSE;
			}
            /* VintonLiu add end */
            
			
			me_set_audiostream(&maudio);//set audio medio info to engine

			if (maudio.isDirectCall == TRUE)//directcall
			{
				if (pcp_direct_fec_enabled())
				{
					ms_report("direct call start_media_stream:the RTPP support FEC function!!!");
					me_enable_audio_fec(TRUE);
					if (pcp_direct_mulriple_fec_enabled())
					{
						if (me_set_directcallfec_status(TRUE) < 0)
						{
							ms_error("direct call start_media_stream:me_set_directcallfec_status fail!!!");
						}
						ms_report("direct call start_media_stream:the RTPP support mulriple FEC function!!!");						
					}
					else
					{
						if (me_set_directcallfec_status(FALSE) < 0)
						{
							ms_error("direct call start_media_stream:me_set_directcallfec_status fail!!!");
						}
						ms_report("direct call start_media_stream:the RTPP not support mulriple FEC function!!!");
					}
				}
				else
				{
					ms_report("direct call start_media_stream:the RTPP not support FEC function!!!");
					me_enable_audio_fec(FALSE);
					if (me_set_directcallfec_status(FALSE) < 0)
					{
						ms_error("direct call start_media_stream:me_set_directcallfec_status fail!!!");
					}
					ms_report("direct call start_media_stream:the RTPP not support mulriple FEC function!!!");
				}
			}
			else//free call
			{
				if (pcp_audio_fec_enabled())
				{
					ms_report("free call start_media_stream:the peer support FEC function!!!");
					me_enable_audio_fec(TRUE);
				}
				else
				{
					ms_report("free call start_media_stream:the peer not support FEC function!!!");
					me_enable_audio_fec(FALSE);
				}
			}
    		trace_flag = TRUE;
    		ms_report("start_media_stream:audio media start ok!!!");
    	}//end if (AUDIO)

		if ( pcp_media_stream_enabled(VIDEO))
		{
			ugo_cfg_t ucfg;
	        uc_get_cfg(&ucfg);

			state = uc_get_state();
			if ((state == PCP_STATE_RINGING) 
			  && (pcp_get_role() == PCP_ANSWER)
			  && (ucfg.video_enabled != eUGo_VIE_RING_PREVIEW ))
			{
				ms_report("when ringing, disable video for answer");
			}
			else 
			{
				cm_resst(&m_stream, sizeof(m_stream));
				pcp_best_mediastream(VIDEO, &m_stream);
				if (relay_flag)
				{
					strcpy(m_stream.remote_addr, relay.m_ip);
					m_stream.remote_port   = relay.v_port;
					pcp_update_mediastream(VIDEO, &m_stream);
				}

				cm_resst(&mvideo, sizeof(mvideo));
				mvideo.iPayLoadType        = m_stream.session_pt;
				mvideo.iRemoteVideoPort    = m_stream.remote_port;
                mvideo.iLocalVideoPort     = m_stream.local_port;
				strcpy(mvideo.cRemoteVideoIp, m_stream.remote_addr);
#ifdef EXTERNAL_TRANSPORT
                if (p2p_get_transport_mode() != ICE_P2P)
                  mvideo.ucExTransportEnable = FALSE;
                else
                  mvideo.ucExTransportEnable = TRUE;
#else
				mvideo.ucExTransportEnable = FALSE;
#endif
                /* if use IM2.0 than use rtp channel to send video rotation angle for old version */
                if ( is_in_trans_support() )
                {
                    mvideo.iMediaSignalEnable = FALSE;
                }
                else
                {
                    mvideo.iMediaSignalEnable = TRUE;
                }

                /* add by Vinton.Liu on 2016-05-30 for video fec and nack config begin */
                mvideo.vieFecEnabled = pcp_video_fec_enabled();
                mvideo.vieNackEnabled = pcp_video_nack_enabled();
                /* Vinton.Liu add end */
                
				pcp_through_info_t through;
				pcp_get_remote_through_params(&through);
				me_video_set_remote_dec_resolution(through.resolution.uiMaxDecodeDefinition);
				if (through.resolution.rotate == 0 || through.resolution.rotate==180)
					me_video_set_remote_camera_resolution(through.resolution.camera_width, through.resolution.camera_height);
				else
					me_video_set_remote_camera_resolution(through.resolution.camera_height, through.resolution.camera_width);
				me_video_set_remote_fps(through.decode_video_fps);
				me_video_set_remote_bitrate(through.start_bitrate, through.min_bitrate, through.max_bitrate);
				/* Modified By MNY@2014-08-20
				0x1. Set capture, codec, channel parameters
				0x2. Start send
				0x4. Receive listen
				*/
				ms_report("through.resolution.rotate = %d", through.resolution.rotate);
				if (me_video_set_stream(&mvideo) == -1)
				{
					ms_error("me_video_set_stream error");
				}
				 
#ifndef ANDROID			
				me_video_start(31);
#endif

				trace_flag = TRUE;
				ms_report("start_media_stream:video media start ok!!!");
			}
		}//end if (VIDEO)

		if (trace_flag)
		{
			/*trace best media stream info*/
			pcp_trace_best_mediastream();
		}
	}

	return RES_OK;
}

int start_media_stream(void)
{
	ugo_cfg_t ucfg;

	uc_get_cfg(&ucfg);

    me_create_audiostream();

    if (pcp_media_stream_enabled(VIDEO))
    {
      me_video_create_stream();
    }


	if (is_call_ice_enabled() 
        && ucfg.video_enabled != eUGo_VIE_RING_PREVIEW )
	{		
		/*update media stream ice info and start detection*/
		update_media_stream_ice_info();
	}

	return RES_OK;
}

int stop_media_stream(void)
{
	if (me_is_start())
	{
		me_delete_audiostream();
		ms_report("stop_media_stream:stop audio ok!!");
	}
	/* 
	Modified By MNY@20140820
	Video's release work should be independent from audio
	1. Stop send & receive;
	2. Disconnect and Release capture device;
	3. Delete video channel;
	4. Reset video parameters;
	*/
	{
		me_video_delete_stream();
		ms_report("stop_media_stream:stop video ok!!");
	}

#ifdef ICE_ENABLED
	{
		p2p_stop_detection();
		ms_report("stop_media_stream:p2p stop detection ok!!");
	}
#endif
	return RES_OK;
}

int force_early_media_enabled(bool enable)
{
	if (enable)
	{
		me_enable_audio_playout(enable);
		me_enable_audio_receive(enable);
		me_enable_audio_send(enable);
	}

	return RES_OK;
}

int update_early_media_enabled(void)
{
	char early_enabled = true;
	pcp_call_param_t* pm = (pcp_call_param_t*)pcp_call_pm();
	
	if (pm == NULL)
		return RES_ERR;

	if ((pcp_get_role() == PCP_ANSWER)
		|| (pm->calltype == PCP_FREE_CALL_TP))
	{
		early_enabled = false;
	}
    
	return force_early_media_enabled(early_enabled);
}

int restart_media_stream(void)
{
	if (me_is_start())
	{
		me_delete_audiostream();
		ms_report("restart_media_stream:stop audio ok!!");

		me_video_delete_stream();

		ms_report("restart_media_stream:stop video ok!!");
	}	


    if (pcp_media_stream_enabled(AUDIO))
      me_create_audiostream();    
    
    if (pcp_media_stream_enabled(VIDEO))	
	  me_video_create_stream();/* Start video preview */

	/*force to start media stream*/
	force_start_media_stream(TRUE);

	/*force star early media*/
	force_early_media_enabled(TRUE);

    /*updata call state*/
    me_set_state(eME_Connected);
	
	return RES_OK;
}

int restart_media_stream2(void)
{
    me_set_state(eME_Running);
    
	/*force to start media stream*/
	force_start_media_stream(TRUE);

	/*force star early media*/
	force_early_media_enabled(TRUE);

	/*updata call state*/
	me_set_state(eME_Connected);

	return RES_OK;
}


int update_media_stream(void)
{
#if 0   //modify by wuzhaoyang for fixed the reinvite problem 20150529
	if (!is_call_ice_enabled())
#else
	if (me_get_ice_mode())
#endif
	{
		/*if ice is disabled, restart media stream*/
		restart_media_stream2();
	}
	else
	{
		/*update media stream ice info*/
		update_media_stream_ice_info();
	}
	
	return RES_OK;
}

int get_version(char* version)
{
	return me_get_version(version);
}

#ifdef ICE_ENABLED
int update_p2p_cfg(void)
{
	char stun_addr[128] = {0};
	char localip[UGO_MAX_IP_ADDR_LEN] = { 0 };

	/*update p2p cfg param*/
	ice_config_t ice_cfg;
	cm_resst(&ice_cfg, sizeof(ice_cfg));
	ice_cfg.regular = ICE_NOMINATION_REGULAR;    //nomination strategy 
	ice_cfg.max_comp = ICE_MAX_COMP;			 //max components
	ice_cfg.max_host = ICE_MAX_HOST;			 //max host address
	pm_get_ice_stunaddr(stun_addr);
	ice_cfg.stun_srv.str_len = strlen(stun_addr);
    ice_cfg.ipv6_enabled = pm_get_ice_ipv6_enabled();
	if (ice_cfg.stun_srv.str_len != 0)
	{
		ice_cfg.stun_flag = true;
		strcpy(ice_cfg.stun_srv.str, stun_addr);//Todo yuanwenhai 2014.04.28  
	}
	
	pm_get_localipaddr(localip);
	ice_cfg.local_ip.str_len = strlen(localip);
	if (ice_cfg.local_ip.str_len > 0)
	{
		strncpy(ice_cfg.local_ip.str, localip, UGO_MAX_IP_ADDR_LEN);
	}

	p2p_set_cfg(&ice_cfg);

	return RES_OK;
}
#endif

/**********************************
	module callback function
	
***********************************/
void on_meg_cb(int ev_type, int ev_reason, const char* something, void* param, int size)
{
	mslist_handle_t* mshd = NULL;
	ME_event_node_t*  evNode = NULL;
    int len = 0;

    if ( ev_type == eME_LOG_TRACE_EV )
    {
        int level;
        level = (ev_reason == eME_REASON_LOG_LEVEL_ERR ? kME_TraceError : kME_TraceReport);
        pm_uc_trace_log_cb(level, (char*)something);
    }
    else
    {
    	mshd = mq_get_mslist(MQ_MEG_TP);
    	if (mshd == NULL)
    		return ;
    	
    	evNode = (ME_event_node_t*)ms_malloc0(ME_EVENT_NODE_T_SIZE + size);
    	if (evNode == NULL)
            return;
            
        evNode->ev_type = ev_type;
        evNode->ev_reason = ev_reason;
        
    	if ( something != NULL )
        {
            len = strlen(something) < sizeof(evNode->something) ? strlen(something) : sizeof(evNode->something);
            strncpy(evNode->something, (char*)something, len);
        }

        if ( param != NULL )
        {
            memcpy(&evNode->param, param, size);
        }

        mslist_append(mshd, (void*)evNode);
    }
}

void on_pcp_cb(int ev_type, int ev_reason, const char* something, void* param, int size)
{
    mslist_handle_t* mshd = NULL;
    pcp_event_node_t* evNode = NULL;
    int len = 0;

    mshd = mq_get_mslist(MQ_PCP_TP);
    if (mshd == NULL)
        return ;

    evNode = (pcp_event_node_t*)ms_malloc0(PCP_EVENT_NODE_T_SIZE + size);
    if (evNode == NULL)
        return;

    evNode->ev_type = ev_type;
    evNode->ev_reason = ev_reason;

    if ( something != NULL )
    {
        len = strlen(something) < sizeof(evNode->something) ? strlen(something) : sizeof(evNode->something);
        strncpy(evNode->something, (char*)something, len);
    }

    if ( param != NULL )
    {
        memcpy(&evNode->param, param, size);
    }

    mslist_append(mshd, (void*)evNode);
}

void on_pcp_register_cb(int ev_type, int ev_reason, const char* something)
{
	mslist_handle_t* mshd = NULL;
	pcp_event_node_t* evNode = NULL;
	int len = 0;

	mshd = mq_get_mslist(MQ_PCP_TP);
	if (mshd == NULL)
		return ;
	
	evNode = (pcp_event_node_t*)ms_malloc0(PCP_EVENT_NODE_T_SIZE);
	if (evNode == NULL)
		return;

	evNode->ev_type = ev_type;
	evNode->ev_reason = ev_reason;
	if ( something != NULL )
	{
    	len = strlen(something) < sizeof(evNode->something) ? strlen(something) : sizeof(evNode->something);
    	strncpy(evNode->something, (char*)something, len);
    }
    
	mslist_append(mshd, (void*)evNode);
}

void on_tcp_send_cb(const char* msg, int slen)
{
	tcp_event_node_t ev_msg;

    if ( msg == NULL )
    {
        ms_error("on_tcp_send_cb error msg.");
        return;
    }
	cm_resst(&ev_msg, sizeof(ev_msg));
	ev_msg.type = TCP_SEND_EV;
	ev_msg.len = slen;
	memcpy(ev_msg.msgbuf, msg, slen);

	on_tcp_cb(TCP_SEND_EV, &ev_msg);
}


void on_tcp_cb(int ev_type, tcp_event_node_t* message)
{
	mslist_handle_t* mshd = NULL;
	tcp_event_node_t* evNode = NULL;

	mshd = mq_get_mslist(MQ_TCP_TP);
	if (mshd == NULL)
		return ;
	
	evNode= (tcp_event_node_t*)ms_malloc0(TCP_EVENT_NODE_T_SIZE);
	if (evNode)
	{	
		memcpy(evNode, message, TCP_EVENT_NODE_T_SIZE);
		mslist_append(mshd, (void*)evNode);
	}
}

void on_trace(int level, const char* something)
{
	mslist_handle_t* mshd = NULL;
	trace_event_node_t* evNode = NULL;

	mshd = mq_get_mslist(MQ_LOG_TP);
	if (mshd == NULL)
		return ;
	
	evNode = (trace_event_node_t*)ms_malloc(TRACE_EVENT_NODE_T_SIZE);
	if (evNode == NULL)
		return;

	evNode->level= level;
	strcpy(evNode->something, (char*)something);

	mslist_append(mshd, (void*)evNode);
}

void on_tm_cb(int timeid)
{
	mslist_handle_t* mshd = NULL;
	tm_event_node_t* evNode = NULL;
	
	mshd = mq_get_mslist(MQ_TMT_TP);
	if (mshd == NULL)
		return ;

	evNode = (tm_event_node_t*)ms_malloc0(TM_EVENT_NODE_T_SIZE);
	if (evNode == NULL)
	{
		ms_error("on_tm_cb: ms_malloc0 tm_event_node error!!!");
		return ;
	}

	evNode->tm_type = tmt_get_timer_type(timeid);
	tmt_get_timer_name(timeid, evNode->tm_name);
	
	mslist_append(mshd, (void*)evNode);
}


void on_state_cb(int state)
{
	pm_uc_call_state_cb(state);
}

#ifdef ICE_ENABLED
void on_ice_ev_cb(int ev_result, unsigned elapsed_time, ice_bestcandpairs_t* ice_bcandpairs)
{
	mslist_handle_t* mshd = NULL;
	ice_event_node_t* evNode = NULL;

	mshd = mq_get_mslist(MQ_ICE_TP);
	if (mshd == NULL)
		return ;
	
	evNode = (ice_event_node_t*)ms_malloc0(ICE_EVENT_NODE_SIZE);
	if (evNode == NULL)
	{
		ms_error("on_ice_cb: ms_malloc0 error!!!");
		return ;
	}
	
	ice_event_msg_t* ev_msg = (ice_event_msg_t*)ms_malloc0(ICE_EV_MSG_T_SIZE);
	if ( ev_msg && ice_bcandpairs )
	{
		ms_report("on_ice_cb: ev_result:%d, elapsed_time:%d(ms)\n", ev_result, elapsed_time);
		for (int i = 0; i<ice_bcandpairs->candpairs_num; i++)
		{
			ms_report("bestpairs[%d][%s:%d------>%s:%d]\n", i,
						ice_bcandpairs->bcandpairs[i].local_addr,
						ice_bcandpairs->bcandpairs[i].local_port,
						ice_bcandpairs->bcandpairs[i].remote_addr,
						ice_bcandpairs->bcandpairs[i].remote_port);
		}

		ev_msg->mode = ev_result;
		ev_msg->elapsed_time = elapsed_time;
		memcpy(&ev_msg->best_cands, ice_bcandpairs, sizeof(ice_bestcandpairs_t));

		evNode->ev_type = eICE_EVENT_MSG_TP;
		evNode->ev_param = (void*)ev_msg;
		sprintf(evNode->something, "on_ice_cb: event message callback!!!");
		
		mslist_append(mshd, (void*)evNode);
	}
	else
	{
		ms_free(evNode);
		ms_error("on_ice_cb: event msg object ms_malloc0 error!!");
	}
}
#endif


#ifdef EXTERNAL_TRANSPORT
int on_send_data_cb(int media_type, int data_type, const void* data, int len)
{
	if (NULL == data || len == 0)
		return -1;

	int compid = -1;

	switch(media_type)
	{
		case eME_MEDIA_AUDIO:
			if (data_type == eME_MEDIA_RTP)
			{
				compid = eICE_COMP_AUDIO_RTP;
			}
			else if (data_type == eME_MEDIA_RTCP)
			{
				compid = eICE_COMP_AUDIO_RTCP;
			}
			
			break;
		case eME_MEDIA_VIDEO:
			if (data_type == eME_MEDIA_RTP)
			{
				compid = eICE_COMP_VIDEO_RTP;
			}
			else if (data_type == eME_MEDIA_RTCP)
			{
				compid = eICE_COMP_VIDEO_RTCP;
			}
			
			break;
		default:
			ms_error("on_send_data_cb: media type=%d unknow.", media_type);
			break;
	}

	if (compid >= 0)
	{
		ice_rtp_msg_t* send_msg = (ice_rtp_msg_t*)ms_malloc0(ICE_RTP_MSG_T_SIZE);
		if (send_msg)
		{
			send_msg->rlen = len;
			send_msg->compid = compid;
			memcpy(send_msg->rbuf, data, len);
			
#ifndef DIRECT_SEND_RECEIVE
            mslist_handle_t* mshd = NULL;
    		ice_event_node_t* evNode = NULL;

    		mshd = mq_get_mslist(MQ_ICE_TP);
    		if (mshd == NULL)
    		{
    			ms_error("on_send_data_cb: mq_get_mslist error!!!");
    			ms_free(send_msg);
    			return -1;
    		}
    	
    		evNode = (ice_event_node_t*)ms_malloc0(ICE_EVENT_NODE_SIZE);
    		if (evNode == NULL)
    		{
    			ms_error("on_send_data_cb: ms_malloc0 error!!!");
    			ms_free(send_msg);
    			return -1;
    		}

    		evNode->ev_type = eICE_SEND_MSG_TP;
			evNode->ev_param = (void*)send_msg;
			sprintf(evNode->something, "on_send_data_cb: send message callback!!!");
			
			mslist_append(mshd, (void*)evNode);
#else
			p2p_do_ice_send_msg(send_msg);
			ms_free(send_msg);
#endif
            return 0;
		}
		else
		{
			ms_error("on_send_data_cb: send msg object ms_malloc0 error!!!");
		}
	}

	return -1;
}

void on_recv_data_cb(int comp_id, const void* data, int len, const char* remote_addr)
{
	if (data == NULL || len == 0)
		return ;
	
	ice_rtp_msg_t* recv_msg = (ice_rtp_msg_t*)ms_malloc0(ICE_RTP_MSG_T_SIZE);
	if (recv_msg)
	{
		recv_msg->rlen = len;
		recv_msg->compid = comp_id;
		memcpy(recv_msg->rbuf, data, len);
		if ( NULL != remote_addr )
		{
		    strncpy(recv_msg->remaddr, remote_addr, MAX_IP_ADDRSTR_LEN-1);
		}

#ifndef DIRECT_SEND_RECEIVE2		
        mslist_handle_t* mshd = NULL;
    	ice_event_node_t* evNode = NULL;

    	mshd = mq_get_mslist(MQ_ICE_TP);
    	if (mshd == NULL)
    	{
    		ms_error("on_recv_data_cb: mq_get_mslist error!!!");
    		ms_free(recv_msg);
    		return ;
    	}
    	
    	evNode = (ice_event_node_t*)ms_malloc0(ICE_EVENT_NODE_SIZE);
    	if (evNode == NULL)
    	{
    		ms_error("on_recv_data_cb: ms_malloc0 error!!!");
    		ms_free(recv_msg);
    		return;
    	}

	    evNode->ev_type = eICE_RECV_MSG_TP;
		evNode->ev_param = (void*)recv_msg;
		sprintf(evNode->something, "on_recv_data_cb: recv message callback!!!");
		
		mslist_append(mshd, (void*)evNode);
#else
		p2p_do_ice_recv_msg(recv_msg);
		ms_free(recv_msg);
#endif
        
	}
	else
	{
		ms_error("on_recv_data_cb: recv msg object ms_malloc0 error!!!");
	}
}

#endif


/**********************************
		module Event Processing
		
***********************************/
void process_tcp_event(tcp_event_node_t* pev)
{
    char evName[64] = {0};
    if ( NULL == pev )
    {
        ms_error("process_tcp_event: error event node.");
        return;
    }
    
    ms_report("process_tcp_event: type[%s]", 
            tcp_get_event_name(pev->type, evName));
	if (pev != NULL)
	{
		switch (pev->type)
		{
		case TCP_SEND_EV:
			if (pm_is_tcp_enabled())
			{
			    int res = 0;
				res = tcp_send(pev->msgbuf, pev->len);
			}
			else
				pm_uc_send_msg_cb(pev->msgbuf, pev->len);
			break;
            
		case TCP_RECV_EV:
			pcp_recv_message(pev->msgbuf, pev->len);
			break;
            
		case TCP_DISCONNECT:			
			ms_report("tcp connect faild!!!");
			uc_call_hangup(PCP_HUNGUPTCPDISCONNECTED); //notify app that call have been hangup
			
			if (pm_is_tcp_enabled())
				pm_uc_event_cb(eUGo_TCPTRANSPORT_EV, eUGo_TCP_DISCONNECTED, 
				                "Server connect failed!!!", NULL);
			break;
            
		case TCP_CONNECTED:
			ms_report("tcp connect ok!!!");
			if (pm_is_tcp_enabled())
			{
			    if ( !pm_is_tlv_enabled() )
			    {
				    pcp_build_version();
				}
				pm_uc_event_cb(eUGo_TCPTRANSPORT_EV, eUGo_TCP_CONNECTED, 
                                "Server connected ok!!!", NULL);
			}
			
			break;
            
		case TCP_RECONNECTED:
			ms_report("tcp reconnect ok!!!");
			pcp_build_route_update();
			if (pm_is_tcp_enabled())
			{
				pm_uc_event_cb(eUGo_TCPTRANSPORT_EV, eUGo_TCP_RECONNECTED, 
                                "Server reconnected ok!!!", NULL);
			}

#ifdef ICE_ENABLED
            //yangjj add
            ugo_cfg_ice_t ice_cfg;
            pm_get_ice_cfg(&ice_cfg);
            if (ice_cfg.ice_enabled == true)
            {
              if (iceapi_is_instance() == 1)
              {
                iceapi_destroy_instance();
              }

              iceapi_create_instance();
            }
            //end
#endif

#if defined(SKT_PROTOCOL_SUPPORT_ENABLED)
            /* try to re login */
            if (pm_is_tcp_enabled() && pm_is_tlv_enabled())
            {
			    pcp_build_relogin();
			}
#endif			
			break;

		case TCP_SENDKEEPALIVE:
			if (pm_is_tcp_enabled())
			{
				ms_report("tcp need to send keepalive!!!");
				if ( !pm_is_tlv_enabled() )
				{
				    tcp_send_keeaplive();
				}
#if defined(SKT_PROTOCOL_SUPPORT_ENABLED)				
				else
				{
				    pcp_build_heartbeat();
				}
#endif
			}
			
			break;
		default:
			break;
		}
	}
}

static int bRunning_PCP_START_VIDEO_EV = 0;
static int bRunning_PCP_STOP_VIDEO_EV = 0;
static int bRunning_PCP_SET_CAMARA_EV = 0;
int uc_set_process_running(IN int eventcode, int bRunning)
{
	if(pcp_media_stream_enabled(VIDEO)== 0)
	{
		bRunning_PCP_START_VIDEO_EV = 0;
		bRunning_PCP_STOP_VIDEO_EV = 0;
		bRunning_PCP_SET_CAMARA_EV = 0;
		return 0;
	}
	switch (eventcode)
	{
	case PCP_START_VIDEO_EV:
		bRunning_PCP_START_VIDEO_EV = bRunning;
		return 1;
	case PCP_STOP_VIDEO_EV:
		bRunning_PCP_STOP_VIDEO_EV = bRunning;
		return 1;
	case PCP_SET_CEMERA_EV:
		bRunning_PCP_SET_CAMARA_EV = bRunning;
		return 1;
	default:
		break;
	}
	return 0;
}
int uc_get_process_running(IN int eventcode)
{
	switch (eventcode)
	{
	case PCP_START_VIDEO_EV:
		return bRunning_PCP_START_VIDEO_EV;
	case PCP_STOP_VIDEO_EV:
		return bRunning_PCP_STOP_VIDEO_EV;		
	case PCP_SET_CEMERA_EV:
		return bRunning_PCP_SET_CAMARA_EV;
	default:
		break;
	}
	return 0;
}

#ifdef ICE_ENABLED
int uc_change_ice_mode() // yangjj add
{
  int retval = -1;
  if (uc_get_state() != PCP_STATE_CONNECT)
  {
    return retval;
  }
  
  int mode = iceapi_get_mode();
  ugo_cfg_ice_t ice_cfg;
  pm_get_ice_cfg(&ice_cfg);
  if (mode == ICE_RTPP && ice_cfg.ice_enabled == true)
  {
    retval = iceapi_update_mode(ICE_P2P);
    me_update_ice_mode(eME_ICE_P2P_MD);
  }
  else
  {
    retval = iceapi_update_mode(ICE_RTPP);
    me_update_ice_mode(eME_ICE_RTPP_MD);
  }
  
  return retval;
}
#endif

void process_pcp_event(pcp_event_node_t* pev)
{
	ME_ENV_cfg_t stEnvCfg;
	char evName[64] = {0};
	
	memset(&stEnvCfg, 0x00, sizeof(ME_ENV_cfg_t));
    if ( NULL == pev )
    {
        ms_error("process_pcp_event: pev is NULL!!");
        return;
    }

    ms_report("process_pcp_event: evt[%s] reason[%d]", \
            pcp_get_event_name(pev->ev_type, evName), pev->ev_reason);
	switch (pev->ev_type)
	{
	case PCP_NONE_REGISTER_EV:
    {
		if (pev->ev_reason != PCP_SUCCESS)
		{
			tcp_force_closed();
			ms_report("tcp force closed by server!!!");
			pm_uc_event_cb(eUGo_TCPTRANSPORT_EV, eUGo_TCP_DISCONNECTED, pev->something, NULL);
		}
	}
	break;
    
	case PCP_CALLDIALING_EV:
    {
		/* Call error, stop video preview if needed */
		if (is_call_error_reason(pev->ev_reason))
		{
			ms_report("stop_media_stream!!!");
            if ( pm_is_tlv_enabled() )
            {
                tmt_stop_timer(TM_SEND_IN_TRANS_REQ_T3);   
            }
			stop_media_stream();
		}
		else if (is_call_temporary_reason(pev->ev_reason))
		{
			//connecting between send invite and don't receive response;
			if(pev->ev_reason == PCP_CONNECTING)
			{
			    /* record dial start time */
			    pcp_update_start_time();

				uc_set_process_running(PCP_SET_CEMERA_EV, 0);
				uc_set_process_running(PCP_START_VIDEO_EV, 0);
				uc_set_process_running(PCP_STOP_VIDEO_EV, 0);
                
			    //create audiostream but we only start playout---modify by Rookie
			    me_create_audiostream();
				me_enable_audio_playout(true);

				/* Modified By MNY@2014-08-19
				1. Caller support video, then start video preview
				*/
			    if( 1 == pcp_media_stream_enabled(VIDEO) )
			    {
					me_video_create_stream();
#ifndef ANDROID
					me_video_start(0x01+0x08);
#endif
				}
			}
			else if (pev->ev_reason == PCP_RINGING)
			{
				/* Modified By MNY@2014-08-20
				1. if callee dose not support video, shutdown video preview
				2. Disconnect and release video device
				3. Delete video channel
				*/
				if ( pcp_media_stream_enabled(VIDEO) == 0 )
				{
					me_video_delete_stream();
				}

                ugo_cfg_t ucfg;
                uc_get_cfg(&ucfg);
                if (is_call_ice_enabled() 
                    && ucfg.video_enabled != eUGo_VIE_RING_PREVIEW)
                {
                  /*update media stream ice info and start detection*/
                  update_media_stream_ice_info();
                }

				/*if it is ring msg update early state*/
				update_early_media_enabled();

                /* Add by VintonLiu on 20151207 for direct call open FEC(1,1) */
                if ( uc_get_callmode() == PCP_DIRECT_CM )
                {
                  force_start_media_stream(FALSE);
    			  me_set_net_analyze(false);//close net status analyze, and default open FEC(1,1),
				}
				/* VintonLiu add end */
			}
			else
			{
			    /* direct notify reason to do here */
                /* del by vinton on 20140902 for ring file play/stop by app  */
                #if 0
				/*stop file play*/
				me_stop_file();
                #endif
                /* vinton del end */

                /* Add by VintonLiu on 20151207 for direct call open FEC(1,1) */
                if ( uc_get_callmode() == PCP_DIRECT_CM )
                {
    				me_set_net_analyze(false);//close net status analyze, and default open FEC(1,1),
				}
				/* VintonLiu add end */
			}
		}
		else 
		{
			/*it is answer msg send ack, start media stream and update media state*/				
			/*send call ack msg*/
			uc_call_ack();	
            
            /*force to start media stream*/
            force_start_media_stream(FALSE);
           

			/*force early media enable*/
			force_early_media_enabled(true);
			
#ifdef ANDROID
			ugo_cfg_t ucfg;
			uc_get_cfg(&ucfg);

			if (ucfg.video_enabled != eUGo_VIE_RING_PREVIEW )
			{
				me_video_stop(0x01);
			}
#endif

			/*update media state to eME_Connected*/
			me_set_state(eME_Connected);

            if ( is_in_trans_support())
            {
                tmt_start_timer(TM_SEND_IN_TRANS_REQ_T3);
            }

            /* update call info */
            pcp_update_callinfo_mmode(me_get_ice_mode());
            if ( pm_is_tcp_enabled() )
            {
                char svr_s[64] = {0};
        		sv_addr_t sv_addr;
        		cm_resst(&sv_addr, sizeof(sv_addr_t));
        		pm_get_tcp_serveraddr(svr_s);
                pcp_update_callinfo_sgw(svr_s);
            }
		}
        
#ifdef EARLY_MEDIA_PLAY_BY_RTPRECEIVE
		{	
			pcp_call_param_t* callpm = (pcp_call_param_t*)pcp_call_pm();
			if ( callpm->calltype != PCP_DIRECT_CALL_TP || pev->ev_reason != PCP_RINGING)
#endif
			{
				pm_uc_event_cb(eUGo_CALLDIALING_EV, pev->ev_reason, pev->something, pcp_call_pm());
			}
/* add by vinton on 20140826 for compile error */
#ifdef EARLY_MEDIA_PLAY_BY_RTPRECEIVE
		}
#endif
        /* vinton add end */
	}
	break;

	case PCP_CALLINCOMING_EV:
	{
	    pcp_call_param_t* pCallpm = (pcp_call_param_t*)pev->param;
	    
		if (pev->ev_reason == PCP_SUCCESS)
		{
			uc_call_ringing();
            start_media_stream();
#ifdef __APPLE__
            ugo_cfg_t ucfg;
            UGo_get_config(UGO_CFG_PARAM_MODULE_ID, &ucfg, 0);

            if (ucfg.video_enabled == eUGo_VIE_RING_PREVIEW )
            {
                me_enable_audio_playout(true);
            }
#else		
			me_enable_audio_playout(true);
#endif      
		}

		if (pCallpm != NULL && pCallpm->meetingflag == 1)
		{
			//set agc if meeting mode
			me_get_cfg(ME_ENV_CFG_MODULE_ID, &stEnvCfg);
			if (stEnvCfg.dialogScene == 0)
			{
				stEnvCfg.dialogScene = 1;
				me_set_cfg(ME_ENV_CFG_MODULE_ID, &stEnvCfg);
			}
		}
		
		pm_uc_event_cb(eUGo_CALLINCOMING_EV, pev->ev_reason, pev->something, pCallpm);
	}
		break;
		
	case PCP_CALLANSWER_EV:
    {
		if ( pev->ev_reason == PCP_SUCCESS )
		{
			/*force to start media stream*/
			force_start_media_stream(FALSE);

			/*force early media enable*/
			force_early_media_enabled(true);

			/*update media state to eME_Connected*/
			me_set_state(eME_Connected);

            if ( is_in_trans_support() )
            {
                tmt_start_timer(TM_SEND_IN_TRANS_REQ_T3);
            }

			/* update call info */
			pcp_update_callinfo_mmode(me_get_ice_mode());
			{
				ugo_cfg_tcp_t tcpCfg;
				uc_get_tcp_cfg(&tcpCfg);
				pcp_update_callinfo_sgw(tcpCfg.tcp_srvaddr);
			}
		}

		pm_uc_event_cb(eUGo_CALLANSWER_EV, pev->ev_reason, pev->something, NULL);
	}
	break;
        
	case PCP_CALLHUNGUP_EV:
	{
		char log[MAX_TRACE_BUF_SIZE] = {0};

        if ( is_in_trans_support() )
        {
            tmt_stop_timer(TM_SEND_IN_TRANS_REQ_T3);   
        }
        
        /* update call info, update mic mute state, must before media stop */
		pcp_update_callinfo_mic_ismute(me_get_mic_mute());
		pcp_update_callinfo_mmode(me_get_ice_mode());

        /* Here get meet flag from session, so session couldn't have been deleted */
        if ((pcp_get_meetflag() == 0) 
            || ((pcp_get_meetflag() == 1) && (pcp_get_conftype() == E_PCP_CONF_TYPE_NORMAL)))
        {
    		if(pcp_get_normalcall_to_conference() == TRUE){
    			pcp_set_normalcall_to_conference(FALSE);
    		}
    		stop_media_stream();
        }
        
		pcp_get_call_trace(log);
		if ( log[0] != '\0' )
		{
		    pm_uc_trace_log_cb(kME_TraceReport, log);
            ms_report("%s", log);
        }
        
		/* Release call session */
		pcp_release_call_session();
        
		/* If it is destroy, we not need to callback event  */
		if (pev->ev_reason != PCP_SUCCESS)
		{
			pm_uc_event_cb(eUGo_CALLHUNGUP_EV, pev->ev_reason, pev->something, NULL);
		}
	}
    break;
    
	case PCP_CALLUPDATE_EV:
    {
        if ( (pcp_get_meetflag() == 0)
            || ( (pcp_get_meetflag() == 1) && ((pcp_get_role() == PCP_ANSWER)
                || (pcp_get_conftype() == E_PCP_CONF_TYPE_NORMAL))))
        {
            update_media_stream();
        }
	}
	break;
        
	case PCP_CALLTIMEOUT_EV:
    {
		if ( pev->ev_reason == PCP_IN_TRANS_TIMEOUT )
		{
            // if in trans rsp timeout, try send video angle via media channel
            me_video_send_state_packets(1);
		}
        else
		    uc_call_hangup(pev->ev_reason);
	}
	break;
    
	case PCP_CONFERENCE_EV://add by wuzhaoyang20140605
    {
		if ( pev->ev_reason == PCP_GETSTATE )
		{
            if ( pcp_get_confstate() != E_PCP_CONF_STATE_CLOSED )
            {
			    pm_uc_event_cb(eUGo_CONFERENCE_EV, pev->ev_reason, pev->something, &pev->param);
            }
		}
		else if (pev->ev_reason == PCP_CALLEE_CHANGEMODE)
		{
#ifdef ICE_ENABLED
			iceapi_update_mode(ICE_RTPP);
            me_update_ice_mode(eME_ICE_RTPP_MD);//yangjj add
#endif
			pcp_set_normalcall_to_conference(TRUE);
			me_enable_audio_fec(FALSE);//conference close fec
			build_convert_conference_rsp();
			pm_uc_event_cb(eUGo_CONFERENCE_EV, pev->ev_reason, pev->something, NULL);
		}
		else if(pev->ev_reason == PCP_CALLER_CHANGEMODE)
		{
#ifdef ICE_ENABLED
			iceapi_update_mode(ICE_RTPP);
            me_update_ice_mode(eME_ICE_RTPP_MD);//yangjj add
#endif
			pcp_set_normalcall_to_conference(TRUE);
			me_enable_audio_fec(FALSE);//conference close fec	
			pm_uc_event_cb(eUGo_CONFERENCE_EV, pev->ev_reason, pev->something, NULL);
		}
		else if (pev->ev_reason == PCP_SUCCESS)
		{
		    if ( pcp_get_conftype() == E_PCP_CONF_TYPE_NORMAL )
            {      
    			if(eME_Connected != me_get_state())
    			{
    				/*force to start media stream*/
    				force_start_media_stream(FALSE);

    				/*force early media enable*/
    				force_early_media_enabled(true);
                    
    				/*update media state to eME_Connected*/
    				me_set_state(eME_Connected);
    			}
            }
			pm_uc_event_cb(eUGo_CONFERENCE_EV, pev->ev_reason, pev->something, NULL);
		}
		else if(pev->ev_reason == PCP_CONNECTING)
		{
			//create audiostream but we only start playout---modify by Rookie
			if ( pcp_get_conftype() == E_PCP_CONF_TYPE_NORMAL)
            {         
    			me_create_audiostream();
    			me_enable_audio_playout(true);
    			if(pm_is_local_video_enabled())
    			{
    				// video meeting, To be continue...
    				;
    			}
            }
			pm_uc_event_cb(eUGo_CONFERENCE_EV, pev->ev_reason, pev->something, NULL);
		}
        else if ( pev->ev_reason == PCP_CONF_FINISHED )
        {
            if ( pcp_get_conftype() == E_PCP_CONF_TYPE_NORMAL )
            {
                stop_media_stream();
            }
            pm_uc_event_cb(eUGo_CONFERENCE_EV, pev->ev_reason, pev->something, NULL);
        }
		else
		{
			pm_uc_event_cb(eUGo_CONFERENCE_EV, pev->ev_reason, pev->something, NULL);
		}

        if ( (pev->ev_reason != PCP_CONF_FINISHED)
            || (pcp_get_confstate() != E_PCP_CONF_STATE_CLOSED))
        {
            if ( pcp_get_conftype() == E_PCP_CONF_TYPE_NORMAL )
            {
                bool fecStatus = TRUE;
                
#ifdef ICE_ENABLED
        		if (ICE_RTPP == iceapi_get_mode())
        		{
        			iceapi_update_mode(ICE_RTPP);
                    me_update_ice_mode(eME_ICE_RTPP_MD);//yangjj add
        		}
#endif
        		if (FALSE == pcp_get_normalcall_to_conference())
        		{
        			pcp_set_normalcall_to_conference(TRUE);
        		}
            
        		fecStatus = me_get_audio_fec();
        		if ( fecStatus )
        		{
        			me_enable_audio_fec(FALSE);//conference close fec
        		}
                
                //set agc if meeting mode
                me_get_cfg(ME_ENV_CFG_MODULE_ID, &stEnvCfg);
                if (stEnvCfg.dialogScene == 0)
                {
                    stEnvCfg.dialogScene = 1;
                    me_set_cfg(ME_ENV_CFG_MODULE_ID, &stEnvCfg);
                }
            }
        }

        if ( pcp_get_confstate() == E_PCP_CONF_STATE_CLOSED )
        {
            if ( pcp_get_conftype() == E_PCP_CONF_TYPE_NORMAL )
            {
                stop_media_stream();
            }
            pcp_release_call_session();
            pm_uc_event_cb(eUGo_CONFERENCE_EV, PCP_CONF_FINISHED, pev->something, NULL);
        }
	}
	break;

	case PCP_DTMF_EV:
	{
	    /* Add by Vinton.Liu on 20151123 for play dtmf tone to local */
	    pcp_dtmf_event_node_t* dtmf_ev = (pcp_dtmf_event_node_t*)pev->param;
	    if ( NULL != dtmf_ev )
	    {
	        me_play_dtmf(dtmf_ev->dtmf_str[0]);
	    }
	    /* Vinton.Liu add end */
	    pm_uc_event_cb(eUGo_GETDTMF_EV, pev->ev_reason, pev->something, NULL);
	}
    	break;

	case PCP_START_VIDEO_EV:
	{
		if (uc_is_incall())
		{
			pcp_video_para_t* pVideoPara = (pcp_video_para_t*)pev->param;

			if (pVideoPara != NULL)
			{
				/* add by Vinton.Liu on 2016-01-28 for remote preview on ringing state begin */
				/* In the Ringing state, only can open local preview except video_enable = 2.  */
				if ((pVideoPara->video_mode == 0x1F)
					&& (uc_get_state() == PCP_STATE_RINGING)
					&& (pm_is_local_video_enabled() != eUGo_VIE_RING_PREVIEW))
				{
					pVideoPara->video_mode = 0x01;
				}
				/* Vinton.Liu add end */
				me_video_start(pVideoPara->video_mode);
			}
		}
		ms_report("PCP_START_VIDEO_EV: start video ok.");
		uc_set_process_running(PCP_START_VIDEO_EV, 0);
	}
    break;
		
	case PCP_STOP_VIDEO_EV:
	{
		if (uc_is_incall())
		{
			pcp_video_para_t* pVideoPara = (pcp_video_para_t*)pev->param;
			if (pVideoPara != NULL)
			{
				me_video_stop(pVideoPara->video_mode);
			}
		}
		ms_report("PCP_STOP_VIDEO_EV: stop video ok.");
		uc_set_process_running(PCP_STOP_VIDEO_EV, 0);
	}
	break;
    
	case PCP_SET_CEMERA_EV:
	{
		if (uc_get_state() == PCP_STATE_CONNECT)
		{
			ME_video_camera_param_t para;
			pcp_video_para_t* pVideoPara = (pcp_video_para_t*)pev->param;

			memset(&para, 0, sizeof(para));
			if (pVideoPara != NULL)
			{
				para.usCameraIdx = pVideoPara->camera_idx;
				para.ucMaxFps = pVideoPara->camera_fps;
				para.usHeight = pVideoPara->camera_height;
				para.usWidth = pVideoPara->camera_width;
				para.usRotateAngle = pVideoPara->camera_rotate;
				//para.ucExternformate = pVideoPara->externFrameFormate;
			}

			me_video_set_camera(&para);
		}
		ms_report("PCP_SET_CEMERA_EV: set camera ok.");
		uc_set_process_running(PCP_SET_CEMERA_EV, 0);
	}
    break;

    case PCP_IN_TRANS_REQ_EV:
    {
        if ( uc_is_incall() )
        {
            pcp_call_trans_state_t *trans_state = (pcp_call_trans_state_t*)pev->param;
            
            uc_call_in_trans_rsp();
            
            if ( trans_state )
            {
                me_video_set_remote_sendrotation(trans_state->vie_angle);
                if ( trans_state->vie_angle & 0x08 )
                {
                    uc_call_in_trans_req();
                }
            }
            
            pm_uc_event_cb(eUGo_CALL_TRANS_STATE_EV, 
                            pev->ev_reason, 
                            pev->something, 
                            pev->param);
        }
    }
    break;
    
    case PCP_IN_TRANS_RSP_EV:
    {
        /* Reason 4 meas remote peer not support, so stop period timer to stop send msg */
        if ( pev->ev_reason == 4 )
        {
            /* if remote peer not support via signal channel, then forward compatible old version */
            me_video_set_media_signal_status(TRUE);
            tmt_stop_timer(TM_SEND_IN_TRANS_REQ_T3);
        }
    }
    break;
    
	default:
		break;
	}

    ms_report("process_pcp_event: evt[%s] reason[%d] end.", \
            pcp_get_event_name(pev->ev_type, evName), pev->ev_reason);
}


void process_meg_event(ME_event_node_t* pev)
{
	int level = 0;

	if  (pev != NULL)
	{
		switch (pev->ev_type)
		{
		case eME_RTP_TIMEOUT_EV:
        {
            if ( uc_get_state() == PCP_STATE_CONNECT )
            {
                ms_error(" RTP time out!!!");
    			uc_call_hangup(PCP_HUNGUPRTPTIMEOUT);
    			
                //update call info callstate
                pcp_update_callinfo_cstate(E_PCP_CSTATE_RTP_TIMEOUT_FAILED);
            }
		}
			break;
		case eME_RTP_UPSINGLEPASS_EV:
        {
            if ( uc_get_state() == PCP_STATE_CONNECT )
            {
    			pcp_update_callinfo_spreason(pev->ev_reason);
                /* mod by Vinton.Liu on 2016-03-02 for add send failed error code begin */
                if ( pev->ev_reason == eME_REASON_START_SEND_FAIL )
                {
                    pm_uc_event_cb(eUGo_UPSINGLEPASS_EV, eUGo_Reason_StartSendFailed, pev->something, NULL);
                }
                else
                {
    			    pm_uc_event_cb(eUGo_UPSINGLEPASS_EV, pev->ev_reason, pev->something, NULL);
    			}
                /* Vinton.Liu mod end */

    			ms_error("upsingle pass event. reason[%d](%s)!!!", pev->ev_reason, pev->something);
                //update call info callstate to singlepass
                if ( pev->ev_reason != eME_REASON_RTP_RESTORE_OK )
                {
                    pcp_update_callinfo_cstate(E_PCP_CSTATE_UP_SINGLEPASS_FAILED);
                }
                else
                {
                    pcp_update_callinfo_cstate(E_PCP_CSTATE_NORMAL);
                }
            }
		}
		    break;
		case eME_RTP_DNSINGLEPASS_EV:
        {
            if ( uc_get_state() == PCP_STATE_CONNECT )
            {
                pcp_update_callinfo_spreason(pev->ev_reason);
			
    			/* BEGIN: Added by gonghuojin, 2013/12/24   PN:receive packet singlepass restart */
    			if (eME_REASON_LOCAL_RECEIVE_FAIL == pev->ev_reason
    				&& uc_get_state() == PCP_STATE_CONNECT)
    			{
    				restart_media_stream();
    				ms_report("receive packet singlepass, restart audio media stream!!!");
                   	//update call info callstate to singlepass
                    pcp_update_callinfo_cstate(E_PCP_CSTATE_NORMAL);
    			}
    			else
                {
                    //update call info callstate to singlepass
                    if ( pev->ev_reason != eME_REASON_RTP_RESTORE_OK )
                    {                    
                        pcp_update_callinfo_cstate(E_PCP_CSTATE_DOWN_SINGLEPASS_FAILED);
                    }
                    else
                    {
                        pcp_update_callinfo_cstate(E_PCP_CSTATE_NORMAL);
                    }
                }
    			pm_uc_event_cb(eUGo_DNSINGLEPASS_EV, pev->ev_reason, pev->something, NULL);
            }
			ms_error("dnsingle pass event. reason[%d](%s)", pev->ev_reason, pev->something);
		}
			break;

		case eME_NETWORK_STATE_EV:
            if ( uc_get_state() == PCP_STATE_CONNECT )
            {
                ME_network_state_t *net = (ME_network_state_t*)pev->param;
    		    
                pm_uc_event_cb(eUGo_NETWORK_EV, pev->ev_reason, pev->something, pev->param);
                ms_report("state:%d ice: %d, rtt: %d, lost: %d(s) %d(r), jitter: %d(s) %d(r)",
                    pev->ev_reason, net->ice, net->rttMs, net->uplinkLost,
                    net->downlinkLost, net->upJitter, net->downJitter);
            }
			break;
		
		case eME_VIDEO_NETWORK_STATE_EV:
		    if ( uc_get_state() == PCP_STATE_CONNECT )
		    {
                ME_video_network_state_t *net = (ME_video_network_state_t*)pev->param;

    		    pm_uc_event_cb(eUGo_VIDEO_NETWORK_EV, pev->ev_reason, pev->something, pev->param);

				char strNet[1024] = { 0 };
				char rtPOT[1024] = { 0 };
				sprintf(strNet, "vie state: %d, ice: %d, rtt: %d, lost: %d(s) %d(r)," \
					" rate: %d(s) %d(r), res: %dx%d(s) %dx%d(r)," \
					" frame: %d(s) %d(r) pt: %d(s) %d(r) codec: %s(s) %s(r) RelayCnt: %d",
					pev->ev_reason, net->ice, net->rttMs, net->uplinkLost, net->downlinkLost,
					net->bitrate_bps, net->rec_bitrate, net->width, net->height,
					net->decode_width, net->decode_height, net->target_fps, net->decode_fps,
					net->EncoderPt, net->DecoderPt, net->encCodec, net->decCodec, net->rtPOT_nCount);

				for (int i = 0; i < net->rtPOT_nCount; i++) {
					sprintf(rtPOT, " Relay:%d{IP: %d(s) %d(r) Flow_a: %dKB(s) %dKB(r) Flow_v: %dKB(s) %dKB(r)} ", i, net->rtPOT_SendIP[i], net->rtPOT_RecvIP[i], net->rtPOT_SendValue_a[i], net->rtPOT_RecvValue_a[i], net->rtPOT_SendValue_v[i], net->rtPOT_RecvValue_v[i]);
					strcat(strNet, rtPOT);
				}
				ms_report(strNet);
		    }
			break;

		case eME_EARLY_MEDIA_EV:
			//ring callback  here we use pcp ringing message,so drop this event
#ifdef EARLY_MEDIA_PLAY_BY_RTPRECEIVE
            if ( uc_get_state() != PCP_STATE_CONNECT)
			{
				pcp_call_param_t* callpm = (pcp_call_param_t*)pcp_call_pm();
				if ( callpm->calltype == PCP_DIRECT_CALL_TP)
				{
				    me_enable_audio_send(false);
					pm_uc_event_cb(eUGo_CALLDIALING_EV, PCP_RINGING, pev->something, pcp_call_pm());
					ms_report("call early ringing event!!!");
				}
			}
#endif
			break;
		case eME_LOG_TRACE_EV:
        {
			//log trace callback,here we only report error trace log
			if ( uc_get_state() != PCP_STATE_WAIT )
			{
    			level = (pev->ev_reason == eME_REASON_LOG_LEVEL_ERR ? kME_TraceError : kME_TraceReport);
    			pm_uc_trace_log_cb(level, pev->something);
			}
		}
			break;
		case eME_GET_DTMF_EV:
			pm_uc_event_cb(eUGo_GETDTMF_EV, pev->ev_reason, pev->something, NULL);
			break;

        case eME_FILE_PLAY_TO_ENDED:
#ifdef AUTO_TEST_SWITCH
            if ( pcp_get_role() == PCP_OFFER )
            {
                uc_call_hangup(PCP_HUNGUPMYSELF);
    			ms_error("file play to end, hang up!!!");
            }
#endif
            break;
		case eME_REMOTE_VIDEO_STATE_NOTIFY_EV:
			pm_uc_event_cb(eUGo_REMOTE_VIDEO_STATE_NOTIFY_EV, pev->ev_reason, pev->something, NULL);
			break;
		case eME_VIDEO_EV:
			pm_uc_event_cb(eUGo_VIDEO_EV, pev->ev_reason, pev->something, NULL);
		default:
			break;
		}
	}
}

void process_log_event(trace_event_node_t* pev)
{
	pm_uc_trace_log_cb(pev->level, pev->something);
}

void process_tmt_event(tm_event_node_t* pev)
{
	if (NULL == pev)
	{
		ms_error("process_tmt_event: pev is NULL !");
		return ;
	}

	switch (pev->tm_type)
	{
		case TM_P2P_DETECTION_T0:
#ifdef ICE_ENABLED	
			if (!p2p_is_detection_finished())
			{
				p2p_do_ice_detection_tm();
			
				/*force to start media stream*/
				force_start_media_stream(FALSE);
			}
#endif
			break;
		case TM_P2P_CHECKLINE_T1:
#ifdef ICE_ENABLED			
            ugo_cfg_ice_t ice_cfg;
            pm_get_ice_cfg(&ice_cfg);
            if (ice_cfg.ice_enabled == true)//yangjj modify
            {
                p2p_do_ice_checkline_tm();
            }			
#endif
			break;
		case TM_P2P_SENDPING_T2:
#ifdef ICE_ENABLED			
			p2p_do_ice_sendping_tm();
#endif
			break;

		case TM_SEND_IN_TRANS_REQ_T3:
		{
		    uc_call_in_trans_req();
		}
		break;
		
		default:
			break;
	}
}

/**********************************
	module Event Iteration Thread
	
***********************************/
void *ev_iteration_thread(void *arg)
{
	void* evdata = NULL;
	MSList* evlist = NULL;

	while (!ethread_flag)
	{ 
		/*tcp event*/
		evdata = mq_get_node(MQ_TCP_TP);
		if(evdata)
		{
			process_tcp_event((tcp_event_node_t*)evdata);
			mq_remove_node(MQ_TCP_TP, evdata);
		}

		/*me event*/
		evdata = mq_get_node(MQ_MEG_TP);
		if(evdata)
		{
			process_meg_event((ME_event_node_t*)evdata);
			mq_remove_node(MQ_MEG_TP, evdata);
		}
		
		/*pcp event*/
		evdata = mq_get_node(MQ_PCP_TP);
		if(evdata)
		{
			process_pcp_event((pcp_event_node_t*)evdata);
			mq_remove_node(MQ_PCP_TP, evdata);
		}

		/*tmt event*/
		evdata = mq_get_node(MQ_TMT_TP);
		if(evdata)
		{ 
			process_tmt_event((tm_event_node_t*)evdata);
			mq_remove_node(MQ_TMT_TP, evdata);
		}

		/*trace log event*/
		evdata = mq_get_node(MQ_LOG_TP);
		if(evdata)
		{
			process_log_event((trace_event_node_t*)evdata);
			mq_remove_node(MQ_LOG_TP, evdata);
		}
		
		ms_usleep(10 * 1000);
	}

	ms_report("ev_Iteration_thread  end!!!");
	return NULL;
}

void ev_thread_start(void)
{
	ethread_flag = FALSE;
	ms_thread_create(&ethread_id, NULL, ev_iteration_thread, NULL);
}

void ev_thread_stop(void)
{
	int waiting_time = 2000;
	int *ptime = &waiting_time;

	ethread_flag = TRUE;
	ms_thread_join(ethread_id, (void**)&ptime);
	ethread_id = 0;
}

/**********************************
	module api function
	
***********************************/
int uc_init(int iDateSource, IN uc_cb_vtable_t* pvtable)
{
	/*init base log trace*/
	blog_init();

	/*init tmt model*/
	tmt_init(on_tm_cb);
	
	/*init message queue*/
	mq_init();
	
	/*start event idle thread*/
	ev_thread_start();

	if (pvtable == NULL)
		ms_error("uc init, pvtable == NULL");
	/*set cb vtalbe param---this function maybe create pm object*/
	pm_set_cb_vtable(pvtable);
	ms_report("pm_set_cb_vtable ok.");

	/*init media engine*/
	update_me_cfg(pvtable);
	if (me_init(iDateSource) < 0)
	{
		on_trace(kME_TraceError, "uc_init:uc_media_init error.");
		return RES_ERR;
	}
	
	ms_report("me_init ok.");

	/*init pcp */
    init_pcp_cfg();
	update_pcp_cfg();
	if (pcp_init() < 0)
	{
		on_trace(kME_TraceError, "uc_init:pcp init error!!!");
		return RES_ERR;
	}
	
	ms_report("pcp_init ok.");
	
#ifdef ICE_ENABLED
	/*init p2p*/
	/*update p2p cfg callback vtable*/
	p2p_cb_vtable_t vtable;
	cm_resst(&vtable, sizeof(vtable));
	vtable.recv_cb = on_recv_data_cb;
	vtable.event_cb = on_ice_ev_cb;
	p2p_cbvtable(&vtable);

	update_p2p_cfg();
	if (p2p_init() < 0)
	{
		on_trace(kME_TraceError, "uc_init:p2p_init error!!!");		
		/* mod by Vinton.Liu on 20150921 for shutdown ice if init failed. */
#if 0		
		return RES_ERR;
#else
        ugo_cfg_ice_t iceCfg;
        memset(&iceCfg, 0x00, sizeof(ugo_cfg_ice_t));
        uc_set_ice_cfg(&iceCfg);
#endif
        /* Vinton.Liu mod end */
	}
	else
	{
	    ms_report("p2p_init ok.");
	}
#endif

	return RES_OK;
}

void uc_destroy(void)
{
	/*hangup call*/
	uc_call_hangup(PCP_HUNGUPMYSELF); // PCP_SUCCESS
	while (uc_get_state() != PCP_STATE_WAIT)
		ms_sleep(1);

	/*stop event thread*/
	ev_thread_stop();
	ms_report("ev_thread_stop ok!!!!!!");

	/*destroy pcp*/
	pcp_destroy();
	ms_report("pcp_destroy ok!!!!!!");

	/*destroy tcp*/
	tcp_destroy();
	ms_report("tcp_destroy ok!!!!!!");

#ifdef ICE_ENABLED
	p2p_destroy();
	ms_report("p2p_destroy ok!!!!!!");
#endif

	/*destroy media engine*/
	me_destroy();
	ms_report("me_destroy ok!!!");
	
	mq_destroy();
	ms_report("mq_destroy ok!!!!!!");

	pm_remove();
	ms_report("pm_remove ok!!!!!!");
	
	/*destroy base log trace*/
	blog_destroy();
	ms_report("blog_destroy ok!!!!!!");

	tmt_destroy();
	ms_report("tmt_destroy ok!!!!!!");
	
	ms_report("uc_destroy ok!!!");
}

int uc_set_rtpp_cfg(IN void* prtpp)
{
	ugo_rtpp_cfg_t* pcfg = (ugo_rtpp_cfg_t*)prtpp;

    if ( pcfg == NULL )
    {
        ms_error("uc_set_rtpp_cfg failed on NULL param.");
        return RES_ERR;
    }
    
	pcp_rtpp_info_t pcp_rtpp;
	cm_resst(&pcp_rtpp, sizeof(pcp_rtpp));
	pcp_rtpp.rtpp_num = (pcfg->rtpp_num < PCP_MAX_RTPP) ? pcfg->rtpp_num : PCP_MAX_RTPP;
	for (int i = 0; i < pcp_rtpp.rtpp_num; i++)
	{
		pcp_rtpp.rtpp_arr[i].delay = pcfg->rtpps[i].delay;
		pcp_rtpp.rtpp_arr[i].lost = pcfg->rtpps[i].lost;
		strcpy(pcp_rtpp.rtpp_arr[i].ip, pcfg->rtpps[i].ip);
	}
	
	pcp_update_rtpp(&pcp_rtpp);
	pm_set_rtpp_cfg((ugo_rtpp_cfg_t*)prtpp);
	
	return RES_OK;
}

int uc_set_cfg(IN void* pcfg)
{	
	if (pcfg == NULL)
		return RES_ERR;

	pm_set_uc_cfg((ugo_cfg_t*) pcfg);
	update_pcp_cfg();
		
	return RES_OK;
}
int uc_set_tcp_cfg(IN void* pcfg)
{
	if ( pcfg == NULL )
		return RES_ERR;

	pm_set_tcp_cfg((ugo_cfg_tcp_t*)pcfg);
	update_tcp_cfg();
	
	return RES_OK;
}

int uc_set_ice_cfg(IN void* pcfg)
{
	if(pcfg == NULL)
	{
		return RES_ERR;
	}

	pm_set_ice_cfg((ugo_cfg_ice_t*)pcfg);
#ifdef ICE_ENABLED 	
    update_p2p_cfg();
    uc_change_ice_mode(); // yangjj add
#endif

	update_pcp_cfg();

	return RES_OK;
}

int uc_get_cfg(OUT void* pcfg)
{
	return pm_get_uc_cfg((ugo_cfg_t*) pcfg);
}
//add by yuanwenhai 2014.04.29
int uc_get_tcp_cfg(OUT void* pcfg)
{
	return pm_get_tcp_cfg((ugo_cfg_tcp_t*) pcfg);
}

int uc_get_ice_cfg(OUT void* pcfg)
{
	return pm_get_ice_cfg((ugo_cfg_ice_t*)pcfg);
}

int uc_update_tcp_state(IN int tcp_state)
{
	mslist_handle_t* mshd = NULL;
	tcp_event_node_t* evdata = NULL;

	mshd = mq_get_mslist(MQ_TCP_TP);
	if (mshd == NULL)
		return RES_ERR;

	evdata= (tcp_event_node_t*)ms_malloc(TCP_EVENT_NODE_T_SIZE);
	if (evdata)
	{	
		switch(tcp_state)
		{
			case eUGo_TCP_DISCONNECTED:
				evdata->type = TCP_DISCONNECT;
				break;
			case eUGo_TCP_CONNECTED:
				evdata->type = TCP_CONNECTED;
				break;
			case eUGo_TCP_RECONNECTED:
				evdata->type = TCP_RECONNECTED;
				break;
			default:
				break;
		}
		
		evdata->len = 0;
		mslist_append(mshd, (void*)evdata);
	}

	return RES_OK;
}

int uc_update_system_state(IN int sys_state)
{
	int pcp_state = PCP_STATE_WAIT;
	
	if (sys_state == eUGo_SYSCALL_ACTIVE)
	{
		pcp_state = PCP_STATE_SYSACTIVE;
	}
	
	return pcp_update_sysstate(pcp_state);
}

int uc_get_state(void)
{
	return pcp_get_state();		
}

int uc_is_incall(void)
{
    if ( uc_get_state() == PCP_STATE_SYSACTIVE
        || uc_get_state() == PCP_STATE_WAIT )
    {
		return TRUE;
		//return FALSE;
    }

    return TRUE;
}

int uc_get_callmode(void)
{
	return pcp_get_callmode();		
}

int uc_enable_logs(const char* fpath)
{
	return blog_enable_logs(ORTP_REPORT|ORTP_WARNING|ORTP_ERROR, fpath);
}

int uc_get_codecs(OUT void* pcodeclist)
{
	ME_codec_list_t* pmcodecs = (ME_codec_list_t*)pcodeclist;
	ME_codec_list_t tmpCodecList;
	pcp_codec_list_t codecs;
	int i = 0;
	int m_x = 0;
	int res = -1;
	
	if (NULL == pmcodecs)
		return RES_ERR;

    memset(&tmpCodecList, 0x00, ME_CODEC_LIST_T_SIZE);
	if (me_get_codecs(&tmpCodecList) < 0)
		return RES_ERR;

	res_st(&codecs, sizeof(codecs));
	if (pcp_get_audio_codecs(&codecs) < 0)
		return RES_ERR;

	for (i = 0; i < tmpCodecList.num; i++)
	{
	    res = check_codecs(&codecs, tmpCodecList.codecs[i].pltype);
		if ( res != RES_ERR )
		{
			memcpy(&(pmcodecs->codecs[m_x]), &tmpCodecList.codecs[i], \
			    sizeof(ME_codec_info_t));
			pmcodecs->codecs[m_x].enabled = codecs.codec_arr[res].enabled;
			pmcodecs->num = ++m_x;
		}
	}    
    
	return RES_OK;
}

int uc_set_codecs(IN void* pcodeclist)
{
	ME_codec_list_t* pmcodecs = (ME_codec_list_t*)pcodeclist;
	pcp_codec_list_t codecs;
	ME_codec_list_t codec_list;
	int i = 0;
	int m_x = 0;
	
	if (NULL == pmcodecs)
		return RES_ERR;

	res_st(&codecs, sizeof(codecs));
	res_st(&codec_list, ME_CODEC_LIST_T_SIZE);
	if (pcp_get_audio_codecs(&codecs) < 0)
		return RES_ERR;
		
	for (i = 0; i < pmcodecs->num; i++)
	{
	    // filter the codec unsupported now
	    if (check_codecs(&codecs, pmcodecs->codecs[i].pltype) < 0 )
	    {
	        continue;
	    }
	    
		// for pcp sdp
		codecs.codec_arr[m_x].enabled = pmcodecs->codecs[i].enabled;
		codecs.codec_arr[m_x].pt = pmcodecs->codecs[i].pltype;
		strcpy(codecs.codec_arr[m_x].mine, pmcodecs->codecs[i].plname);
		m_x++;

		if ( pmcodecs->codecs[i].enabled )
		{
            // for dynamic set audio codec
			memcpy(&codec_list.codecs[codec_list.num], 
			    &(pmcodecs->codecs[i]), sizeof(ME_codec_info_t));
			codec_list.num++;
		}
	}

	codecs.num = m_x;

	// for dynamic set audio codec
    if ( uc_get_callmode() != eUGo_CM_DIRECT && codec_list.num > 0 )
	{
		me_set_cfg(ME_CODECS_CFG_MODULE_ID, &codec_list);
	}
	return pcp_update_codecs_enabled(&codecs);
}

int uc_set_directcallfec_enable(IN bool enabled)
{
	return pcp_directcallfec_enabled(enabled);
}

int uc_start_video(IN int mode)
{
    pcp_video_para_t videoPara;

	if (uc_get_process_running(PCP_START_VIDEO_EV))
	{
		ms_report("uc_start_video: pre start video not finish.");
		return RES_ERR;
	}

	if (uc_is_incall())
	{
		uc_set_process_running(PCP_START_VIDEO_EV, 1);

		res_st(&videoPara, PCP_VIDEO_PARA_SIZE);
		videoPara.video_mode = mode;
		on_pcp_cb(PCP_START_VIDEO_EV, 0, NULL, (void *)&videoPara, PCP_VIDEO_PARA_SIZE);
	}

    return RES_OK;
}

int uc_stop_video(IN int mode)
{
    pcp_video_para_t videoPara;

	if (uc_get_process_running(PCP_STOP_VIDEO_EV))
	{
		ms_error("uc_stop_video: pre stop video not finish.");
		return RES_ERR;
	}

	if (uc_is_incall())
	{
		uc_set_process_running(PCP_STOP_VIDEO_EV, 1);
    
		res_st(&videoPara, PCP_VIDEO_PARA_SIZE);
		videoPara.video_mode = mode;
		on_pcp_cb(PCP_STOP_VIDEO_EV, 0, NULL, (void *)&videoPara, PCP_VIDEO_PARA_SIZE);
	}
    return RES_OK;
}

int uc_set_video_cemara(IN int w, IN int h, IN int p, IN int f, IN int idx)
{
    pcp_video_para_t videoPara;

	if (uc_get_state() != PCP_STATE_CONNECT)
	{
		ms_report("uc_set_video_cemara: not in connect state.");
		return RES_OK;
	}

    if ( uc_get_process_running(PCP_SET_CEMERA_EV) )
	{
		ms_error("uc_set_video_cemara: pre set camera not finish.");
        return RES_ERR;
	}

    uc_set_process_running(PCP_SET_CEMERA_EV, 1);
    
    res_st(&videoPara, PCP_VIDEO_PARA_SIZE);
    videoPara.camera_width = w;
	videoPara.camera_height = h;
	videoPara.camera_rotate = p;
	videoPara.camera_fps = f;
	videoPara.camera_idx = idx;
 //videoPara.externFrameFormate = ucExternformate;
    on_pcp_cb(PCP_SET_CEMERA_EV, 0, NULL, (void *)&videoPara, PCP_VIDEO_PARA_SIZE);
	ms_report("uc_set_video_cemara: set camera ok.");
    return RES_OK;
}

int uc_get_video_codecs(OUT void* pcodeclist)
{
	ME_video_enc_param_t* pmcodecs = (ME_video_enc_param_t*)pcodeclist;
	
	int i = 0;
	
	pcp_codec_param_t *p;
	int num;
	int *firstPt;
	if(pcodeclist == NULL)
		return 0;

	get_video_codec(&p, &num, &firstPt);
	pmcodecs->usNum = num;
	pmcodecs->ucFirstSelectedPt = *firstPt;
	for(i = 0; i< num; i++)
	{
		pmcodecs->baseCodec[i].enable = p[i].enabled;
		strcpy((char*)pmcodecs->baseCodec[i].ucPlName, p[i].mine);
		pmcodecs->baseCodec[i].ucPlType = p[i].pt;
	}

	return 1;
}

int uc_set_video_codecs(IN void* pcodeclist)
{

	ME_video_enc_param_t* pmcodecs = (ME_video_enc_param_t*)pcodeclist;
	
	int i = 0;
	
	pcp_codec_param_t *p;
	int num;
	int *firstPt;
	if(pcodeclist == NULL)
		return 0;

	get_video_codec(&p, &num, &firstPt);

	if(num != pmcodecs->usNum)
		return 0;
	
	*firstPt = pmcodecs->ucFirstSelectedPt;
	for(i = 0; i< num; i++)
	{
		if(pmcodecs->baseCodec[i].ucPlType == p[i].pt)
		p[i].enabled = pmcodecs->baseCodec[i].enable;
	}

	return 1;
}

/* add by Vinton.Liu on 2016-05-30 for video fec and nack config begin */
int uc_set_video_fec_enabled(IN bool enabled)
{
    return pcp_update_video_fec_enabled(enabled);
}

int uc_set_video_nack_enabled(IN bool enabled)
{
    return pcp_update_video_nack_enabled(enabled);
}
/* Vinton.Liu add end */

int uc_recv_message(IN int rlen, IN const char* message)
{
	mslist_handle_t* mshd = NULL;
	tcp_event_node_t* evdata = NULL;

	if (rlen <= 0 || rlen > MAX_MSG_LEN || NULL == message)
	{
		ms_error("uc_recv_message error!, len is too long or message is NUll!!!");
		on_trace(kME_TraceError, "uc_recv_message:message is too long or is null!!");
		return RES_ERR;
	}

	mshd = mq_get_mslist(MQ_TCP_TP);
	if (mshd == NULL)
		return RES_ERR;

	evdata= (tcp_event_node_t*)ms_malloc0(TCP_EVENT_NODE_T_SIZE);
	if (evdata)
	{	
		memcpy(evdata->msgbuf, message, rlen);
		evdata->len = rlen;
		evdata->type = TCP_RECV_EV;
		mslist_append(mshd, (void*)evdata);
	}

	return RES_OK;
}


int uc_login(IN const char *ssid, IN int netmode)
{
	if (!pm_is_tcp_enabled())
		return RES_ERR;
	
	return pcp_build_login(ssid, netmode);
}

int uc_logout(void)
{
	if (!pm_is_tcp_enabled())
		return RES_ERR;

	uc_call_hangup(PCP_HUNGUPMYSELF);

	return pcp_build_logout();
}

int uc_ios_push_call_rsp(IN const char* callid, IN int vpsid, IN int reason)
{
	if (uc_get_state() != PCP_STATE_WAIT && reason == PCP_SUCCESS)
	{
		reason = PCP_BUSY;
		on_trace(kME_TraceError, "uc_ios_push_call_rsp: is busy!!");
	}
	
	return pcp_build_push_call_rsp(callid, vpsid, reason);
}

int uc_call_dialing(IN const ugo_call_dialing_para_t* pCallParam)
{
	pcp_ice_info_t ice;

	pcp_through_info_t through;
	pcp_through_info_t* pThrough = NULL;

    if ( NULL == pCallParam )
    {
        ms_error("uc_call_dialing: null param.");
        return RES_ERR;
    }

    if ( me_is_load() < 0 )
    {
        ms_error("uc_call_dialing media engine is unloaded.");
        return RES_ERR;
    }
    
	if (uc_get_state() != eUGo_STATE_WAIT)
	{
		ms_error("uc_call_dialing busy now.");
		return RES_ERR;
	}

	memset(&through, 0, sizeof(through));
	
	cm_resst(&ice, sizeof(ice));
#ifdef ICE_ENABLED
	if (pm_is_local_ice_enabled())
	{
		/*clean ice param*/
		p2p_clean_param();
		/*create a offer role session*/
		if (iceapi_create_session('o') == 0)
		{
			ice_sdpinfo_t lice;
			p2p_get_local_ice_sdp(&lice);
			strcpy(ice.pwd, lice.ice_auth.ice_pwd);
			strcpy(ice.ufrag, lice.ice_auth.ice_ufrag);
			ice.cdd_num = lice.ice_cands.cand_num;
			for (int i=0; i<ice.cdd_num; i++)
			{
				strncpy(ice.cdd_arr[i].attr_desc, lice.ice_cands.cand_info[i].str, lice.ice_cands.cand_info[i].str_len);
			}
		}
	}
#endif

	if (pCallParam->video_enabled)
	{
		ME_video_dec_info_t dec;
		memset(&dec, 0, sizeof(dec));
		me_get_cfg(ME_VIDEO_DEC_CFG_MODULE_ID,&dec);
		through.resolution.uiMaxDecodeDefinition = dec.uiMaxDefinition;			
		through.decode_video_fps = dec.ucMaxFramerate;
		through.v0 = VIDEOVERSION;

		ME_video_enc_param_t enc;
		memset(&enc, 0 , sizeof(enc));
		me_get_cfg(ME_VIDEO_ENC_CFG_MODULE_ID,&enc);
		through.max_bitrate = enc.usMaxBitrate;
		through.min_bitrate = enc.usMinBitrate;
		through.start_bitrate = enc.usStartBitrate;		
		
		ME_video_camera_param_t video_camera_para;
		memset(&video_camera_para, 0, sizeof(video_camera_para));
		me_video_get_camera_state(&video_camera_para);
#ifdef __APPLE__
        through.resolution.camera_width = video_camera_para.usHeight;
        through.resolution.camera_height = video_camera_para.usWidth;
#else
        through.resolution.camera_width = video_camera_para.usWidth;
        through.resolution.camera_height = video_camera_para.usHeight;
#endif
#ifdef ANDROID
		through.resolution.rotate = video_camera_para.usRotateAngle;
#endif
        through.video_enabled = pCallParam->video_enabled;
        
		pThrough = &through;
		ms_report("uc_call_dialing: %d , %d x %d, %d, %d, %d", \
            dec.uiMaxDefinition , video_camera_para.usWidth, \
            video_camera_para.usHeight, through.start_bitrate, \
            through.min_bitrate, through.max_bitrate);
	}

    return pcp_build_call_dialing(pCallParam, &ice, pThrough);
}


int uc_call_ack(void)
{
	int ice_mode = UC_TR_MODE_RTPP;
	
#ifdef ICE_ENABLED
	if (p2p_is_detection_finished())
		ice_mode = p2p_get_transport_mode();
#endif

	pcp_build_call_ack(ice_mode);
	
	return RES_OK;
}

int uc_call_ringing(void)
{
    pcp_ice_info_t ice;

	pcp_through_info_t through = { 0 };

	cm_resst(&ice, sizeof(ice));
#ifdef ICE_ENABLED
	if (is_call_ice_enabled())
	{	
		/*create session for answer*/
		if (iceapi_create_session('a') == 0)
		{
			ice_sdpinfo_t lice;
			p2p_get_local_ice_sdp(&lice);
			strcpy(ice.pwd, lice.ice_auth.ice_pwd);
			strcpy(ice.ufrag, lice.ice_auth.ice_ufrag);
			ice.cdd_num = lice.ice_cands.cand_num;
			for (int i=0; i<ice.cdd_num; i++)
			{
				strncpy(ice.cdd_arr[i].attr_desc, lice.ice_cands.cand_info[i].str, lice.ice_cands.cand_info[i].str_len);
			}
		}
	}
#endif
	
	ME_video_dec_info_t dec;
	memset(&dec, 0 , sizeof(dec));
	me_get_cfg(ME_VIDEO_DEC_CFG_MODULE_ID,&dec);
	through.resolution.uiMaxDecodeDefinition = dec.uiMaxDefinition;	
	through.decode_video_fps = dec.ucMaxFramerate;
	through.v0 = VIDEOVERSION;

	ME_video_enc_param_t enc;
	memset(&enc, 0, sizeof(enc));
	me_get_cfg(ME_VIDEO_ENC_CFG_MODULE_ID,&enc);
	through.max_bitrate = enc.usMaxBitrate;
	through.min_bitrate = enc.usMinBitrate;
	through.start_bitrate = enc.usStartBitrate;	

	ME_video_camera_param_t video_camera_para;
	memset(&video_camera_para, 0, sizeof(video_camera_para));
	me_video_get_camera_state(&video_camera_para);
#ifdef __APPLE__
    through.resolution.camera_width = video_camera_para.usHeight;
    through.resolution.camera_height = video_camera_para.usWidth;
#else
    through.resolution.camera_width = video_camera_para.usWidth;
    through.resolution.camera_height = video_camera_para.usHeight;
#endif
	through.resolution.rotate = video_camera_para.usRotateAngle;

	ms_report(" uc call ringing   %d  %d   %d   %d", \
	        dec.ucMaxFramerate, video_camera_para.usWidth, \
	        video_camera_para.usHeight, dec.ucMaxFramerate);	
	
	return pcp_build_call_ringing(&ice, &through);
}

int uc_call_answer(void)
{
    int res = RES_OK;
    
    if ( me_is_load() < 0 )
    {
        on_trace(kME_TraceError, "uc_call_answer media engine not unloaded.");
        return RES_ERR;
    }

    /* mod by VintonLiu on 20151019 for code optimize */
    if ( pcp_get_role() == PCP_ANSWER &&
       (uc_get_state() == PCP_STATE_INVITE 
        || uc_get_state() == PCP_STATE_RINGING ))
    {
        /*send call answer msg*/
        if ( (res = pcp_build_call_answer(NULL)) == RES_ERR )        
        {
            ms_error("uc_call_answer failed.");
        }
    }
    else
    {
        on_pcp_cb(PCP_CALLANSWER_EV, PCP_CALLIDNOTEXIST, "have no call to answer!!", NULL, 0);
    }
    /* VintonLiu mod end */
    
    return res;
}


int uc_call_hangup(IN int reason)
{
    int callRole = pcp_get_role();
    int callState = uc_get_state();
    
    if ( uc_is_incall() )
	{
        if ( callState != PCP_STATE_CONNECT )
	    {
	        if ( callRole == PCP_OFFER )
	        {
	            // hangup by myself or callee no answer
	            if ( reason == PCP_HUNGUPMYSELF )
	            {
    	            /* Caller cancel the outgoing call */
                    reason = PCP_CALLERCANCEL;
                }
                return pcp_build_call_hangup(reason);
	        }
	        else
	        {
	            /* Callee reject the incoming call by self */
	            if ( reason == PCP_HUNGUPMYSELF )
	            {
    	            reason = PCP_REJECT;
    	        }
    	        return pcp_build_call_declined(reason);
	        }
	    }
	    else
        {
   		    return pcp_build_call_hangup(reason);
		}
	}
    else       
    {
        ms_warning("[%s] not any call, needn't hangup.", __FUNCTION__);
	}
	
	return RES_OK;
}

int uc_call_in_trans_req()
{
    pcp_call_trans_state_t trans_state = { 0 };

    if ( !uc_is_incall() )
    {
        return RES_ERR;
    }

#ifdef ICE_ENABLED
    trans_state.p2p_state = p2p_get_transport_mode() == ICE_P2P ? ICE_P2P : ICE_RTPP;
#else
    trans_state.p2p_state = 1;
#endif

    trans_state.mic_state = me_get_mic_mute();
    me_video_get_sendrotation_state(trans_state.vie_angle, 
                                    trans_state.vie_state);
    
    return pcp_build_in_trans_req(&trans_state);
}

int uc_call_in_trans_rsp()
{
    return pcp_build_in_trans_rsp(PCP_SUCCESS);
}

//add by wuzhaoyang20140605
int uc_conference_call_dialing(IN const pcp_conf_dialing_para_t *dial_param)
{	
    if ( me_is_load() < 0 )
    {
        on_trace(kME_TraceError, "uc_conference_call_dialing media engine is unloaded.");
        return RES_ERR;
    }
    
	return pcp_build_conf_call_req(dial_param);
}

int uc_conference_invite_parties(IN const pcp_conf_dialing_para_t *dial_param)
{
    return pcp_build_conf_inv_req(dial_param);
}

int uc_delete_conference(IN int reason)
{
    return pcp_build_conf_del_req(reason);
}

//add by luochengcong20141022
int uc_get_call_info_report(OUT void* pCallInfo)
{
    if ( NULL == pCallInfo ) {
        return RES_ERR;
    }
    
    pcp_get_call_info_report((pcp_call_info_t *)pCallInfo);
    return RES_OK;
}
