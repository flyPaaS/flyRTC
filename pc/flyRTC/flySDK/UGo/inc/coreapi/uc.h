/******************************************************************************

  Copyright (C), 2001-2011, DCN Co., Ltd.

 ******************************************************************************
  File Name     : uc.h
  Version       : Initial Draft
  Author        : gonghuojin
  Created       : 2013/10/8
  Last Modified :
  Description   : ugo core manage header file
  Function List :
  History       :
  1.Date        : 2013/10/8
    Author      : gonghuojin
    Modification: Created file

******************************************************************************/
#ifndef UGO_CORE_H
#define UGO_CORE_H

#include "cfg.h"
#include "comm.h"
#include "pm.h"

typedef enum uc_tran_mode_E
{
	UC_TR_MODE_P2P  = 0,
	UC_TR_MODE_RTPP = 1
}uc_tran_mode_e;

#if defined(ANDROID)
int uc_set_android_obj(media_jni_env_pm_t* env);
#endif

int uc_init(int iDateSource, IN uc_cb_vtable_t* pvtable);
void uc_destroy(void);

int uc_set_rtpp_cfg(IN void* prtpp);
int uc_set_cfg(IN void* pcfg);
int uc_set_tcp_cfg(IN void* pcfg);
int uc_set_ice_cfg(IN void* pcfg);


int uc_get_cfg(OUT void* pcfg);
//add by yuanwenhai 2014.04.29
int uc_get_tcp_cfg(OUT void* pcfg);
int uc_get_ice_cfg(OUT void* pcfg);
//add by luochengcong 20141023
int uc_get_call_info_report(OUT void* pCallInfo);

int uc_update_tcp_state(IN int tcp_state);
int uc_update_system_state(IN int sys_state);
int uc_get_state(void);
int uc_is_incall(void);

int uc_get_callmode(void);

int uc_recv_message(IN int rlen, IN const char* message);
int uc_enable_logs(const char* fpath);

int uc_get_codecs(OUT void* pcodeclist);
int uc_set_codecs(IN void* pcodeclist);
int uc_set_directcallfec_enable(IN bool enabled);

int uc_start_video(IN int mode);
int uc_stop_video(IN int mode);
int uc_set_video_cemara(IN int w, IN int h, IN int p, IN int f, IN int idx);
int uc_get_video_codecs(OUT void* pcodeclist);
int uc_set_video_codecs(IN void* pcodeclist);
/* add by Vinton.Liu on 2016-05-30 for video fec and nack config begin */
int uc_set_video_fec_enabled(IN bool enabled);
int uc_set_video_nack_enabled(IN bool enabled);
/* Vinton.Liu add end */

int uc_login(IN const char *ssid, IN int netmode);
int uc_logout(void);
int uc_ios_push_call_rsp(IN const char* callid, IN int vpsid, IN int reason);
int uc_call_dialing(IN const ugo_call_dialing_para_t* pCallParam);
int uc_call_hangup(IN int reason);
int uc_call_answer(void);

int uc_conference_call_dialing(IN const ugo_conf_dialing_para_t *dial_param);//add by wuzhaoyang20140605
int uc_conference_invite_parties(IN const ugo_conf_dialing_para_t *dial_param);
int uc_delete_conference(IN int reason);
int uc_set_process_running(IN int eventcode, int bRunning);
int uc_get_process_running(IN int eventcode);

#ifdef ICE_ENABLED
int uc_change_ice_mode(); // yangjj add
#endif

#endif


