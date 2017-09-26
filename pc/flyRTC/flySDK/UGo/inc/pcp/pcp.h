/******************************************************************************

  Copyright (C), 2001-2011, DCN Co., Ltd.

 ******************************************************************************
  File Name     : pcp.h
  Version       : Initial Draft
  Author        : gonghuojin
  Created       : 2013/9/23
  Last Modified :
  Description   : pcp manager header
  Function List :
  History       :
  1.Date        : 2013/9/23
    Author      : gonghuojin
    Modification: Created file

******************************************************************************/
#ifndef PCP_H
#define PCP_H

#include "pcp_cfg.h"

PCP_EXTERN_C_START

void null();

/****pcp event type*******/
enum pcp_event_type
{
    PCP_NONE_REGISTER_EV,	/*register event*/	
    PCP_CALLDIALING_EV,		/*call dialing event*/
    PCP_CALLINCOMING_EV,    /*call incoming event*/
    PCP_CALLANSWER_EV,		/*call answer event*/
    PCP_CALLHUNGUP_EV,		/*call hungup event*/
    PCP_CALLUPDATE_EV,		/*call update event*/
    PCP_CALLTIMEOUT_EV,		/*call timeout event*/
    PCP_CONFERENCE_EV,       /* conference state event*/
    PCP_DTMF_EV,
    PCP_START_VIDEO_EV,
    PCP_STOP_VIDEO_EV,
    PCP_SET_CEMERA_EV,
    PCP_IN_TRANS_REQ_EV,     /* recv in trans req msg */
    PCP_IN_TRANS_RSP_EV      /* recv in trans rsp msg */
};

typedef enum pcp_cstate_E
{
	E_PCP_CSTATE_NORMAL = 0,
	E_PCP_CSTATE_UP_SINGLEPASS_FAILED,
	E_PCP_CSTATE_BOTHWAY_BLOCK_FAILED,
	E_PCP_CSTATE_RTP_TIMEOUT_FAILED,
	E_PCP_CSTATE_DOWN_SINGLEPASS_FAILED
}pcp_cstate_e;

typedef enum pcp_conf_type_E
{
    E_PCP_CONF_TYPE_NORMAL = 0, /* normal conf, Originator will add in meeting default */
    E_PCP_CONF_TYPE_ALL_DIRECT = 1, /* Originator would not add in meeting default */
} pcp_conf_type_e;

typedef enum pcp_conf_state_E
{
    E_PCP_CONF_STATE_IDLE = 0,
    E_PCP_CONF_STATE_CREATED,
    E_PCP_CONF_STATE_CLOSED,
} pcp_conf_state_e;

typedef struct pcp_event_node
{
    int  ev_type;
    int  ev_reason;
    char something[256];
    char param[0];
} pcp_event_node_t;
#define PCP_EVENT_NODE_T_SIZE (sizeof(pcp_event_node_t))

typedef ugo_call_info_tag pcp_call_info_t;
typedef ugo_call_dialing_para_tag pcp_call_dialing_para_t;
typedef ugo_conf_dialing_para_tag pcp_conf_dialing_para_t;

////////// COMMON FUNCTIONS ///////////
int  pcp_init();
void pcp_destroy(void);
int pcp_set_cb_vtable(pcp_cb_vtable_t* vtable);
int pcp_set_config(IN pcp_cfg_t* pcfg);
int pcp_update_rtpp(IN pcp_rtpp_info_t* prtpp);
int pcp_recv_message(IN const char* msg, IN int slen);

int pcp_update_sysstate(IN int sys_state);
OUT int pcp_get_state(void);
OUT int pcp_get_callmode(void);
OUT int pcp_get_role(void);
OUT void* pcp_call_pm(void);
int pcp_trace_best_mediastream(void);
int pcp_trace_line_change(IN int mode);

int pcp_audio_fec_enabled(void);
int pcp_direct_fec_enabled(void);
int pcp_direct_mulriple_fec_enabled(void);//add by wuzhaoyang20151228
/* add by Vinton.Liu on 2016-05-30 for video fec and nack config begin */
OUT int pcp_video_fec_enabled(void);
OUT int pcp_video_nack_enabled(void);
/* Vinton.Liu add end */
int pcp_ice_enabled(void);
int pcp_in_trans_support_enabled(void);
int pcp_media_stream_enabled(IN int type);
int pcp_media_relay_params(OUT pcp_relay_info_t* prelay);
int pcp_remote_ice_params(OUT pcp_ice_info_t* pice);
int pcp_best_mediastream(IN int type, OUT pcp_session_media_t* pstream);
int pcp_update_mediastream(IN int type, IN const pcp_session_media_t* pstream);
int pcp_update_codecs_enabled(IN void* pcodeclist);
int pcp_directcallfec_enabled(IN unsigned char fecenable);
int pcp_get_audio_codecs(OUT pcp_codec_list_t* pcodecslist);
int pcp_get_remote_through_params(OUT pcp_through_info_t *through);
int pcp_get_call_info_report(OUT pcp_call_info_t *pCallInfo);//added by luochengcong20141022
int pcp_update_call_info_report(OUT pcp_call_info_t* pCallInfo, IN pcp_session_t* psession);//added by luochengcong20141022
int pcp_reset_call_info_report();
int pcp_update_callinfo_mmode(IN int iceMode);
int pcp_update_callinfo_sgw(IN const char* tcpAddr);
int pcp_update_callinfo_cstate(IN pcp_cstate_e callstate);
int pcp_update_callinfo_mic_ismute(IN int ismute);
int pcp_update_callinfo_spreason(IN int spreason);
/* add by Vinton.Liu on 2016-05-30 for video fec and nack config begin */
int pcp_update_video_fec_enabled(IN bool enabled);
int pcp_update_video_nack_enabled(IN bool enabled);
/* Vinton.Liu add end */

///////// AUTH FUNCTIONS ////////////
int pcp_build_version(void);
int pcp_build_login(IN const char* ssid, IN int netmode);
#if defined(SKT_PROTOCOL_SUPPORT_ENABLED)
int pcp_build_relogin();
int pcp_build_heartbeat(void);
#endif
int pcp_build_logout(void);

///////////// CALL FUNCTIONS ////////////////
int pcp_build_push_call_rsp(IN const char* callid, IN int vpsid, IN int reason);
int pcp_build_call_dialing(IN const pcp_call_dialing_para_t* pCallParam, IN const pcp_ice_info_t* ice, IN pcp_through_info_t *through);
int pcp_build_call_answer(IN const pcp_ice_info_t* ice);
int pcp_build_call_ringing(IN const pcp_ice_info_t* ice, pcp_through_info_t *through);
int pcp_build_call_ack(IN int ice_mode);
int pcp_build_call_hangup(int reason);
int pcp_build_call_declined(IN int reason);
int pcp_build_route_update(void);
/* add by Vinton.Liu on 2016-04-06 for add internal state passthrough signalling begin */
int pcp_build_in_trans_req(IN const pcp_call_trans_state_t* trans_state);
int pcp_build_in_trans_rsp(IN int reason);
/* Vinton.Liu add end */
int pcp_build_conf_call_req(IN const pcp_conf_dialing_para_t *dial_para);
int pcp_build_conf_inv_req(IN const pcp_conf_dialing_para_t *dial_para);
int pcp_build_conf_del_req(int reason);
int build_convert_conference_rsp(void);
int pcp_release_call_session(void);
int pcp_get_call_trace(char *log);


//////////// AUXILIARY FUNCTIONS //////////////////////
int pcp_get_conftype(void);
int pcp_get_confstate(void);
int pcp_get_meetflag(void);
int pcp_set_normalcall_to_conference(IN int ntoc);
int pcp_get_normalcall_to_conference(void);
unsigned int getTimeInMs();
void pcp_update_start_time(void);
void pcp_update_ctime(void);

char* pcp_get_event_name(int evType, char *name);
PCP_EXTERN_C_END


#endif



