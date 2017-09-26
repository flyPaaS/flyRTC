/******************************************************************************

  Copyright (C), 2001-2011, DCN Co., Ltd.

 ******************************************************************************
  File Name     : p2p.h
  Version       : Initial Draft
  Author        : gonghuojin
  Created       : 2013/11/14
  Last Modified :
  Description   : p2p param api file header
  Function List :
  History       :
  1.Date        : 2013/11/14
    Author      : gonghuojin
    Modification: Created file

******************************************************************************/
#ifndef P2P_H
#define P2P_H

#include "cfg.h"
#include "comm.h"

#ifdef ICE_ENABLED
#include "ice.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef IN
#define IN 
#endif

#ifndef OUT
#define OUT
#endif

#ifndef P2P_OK
#define P2P_OK 0
#endif

#ifndef P2P_ERR
#define P2P_ERR -1
#endif

#define ICE_MAX_COMP  					4		//1 only audio rtp, 2 audio rtp and rtcp, 4 video	 rtp and rtcp
#define ICE_MAX_HOST					1       //max ice host candidate
#define ICE_NOMINATION_REGULAR          0      	//nomination regular strategy

/**p2p event  notification callback prototype*/
typedef void (*p2p_event_cb_t)(int ev_result, unsigned elapsed_time, ice_bestcandpairs_t* ice_bcandpairs);

/**p2p recv packet callback prototype*/
typedef void (*p2p_recv_cb_t)(int comp_id, const void* data, int len, const char* remote_addr);

/**
 * This structure holds all callbacks that the application should implement.
 *	None is mandatory.
**/
typedef struct p2p_callback_vtable
{
	p2p_recv_cb_t	 recv_cb;	 /*recv callback*/
	p2p_event_cb_t	 event_cb;	 /*event callback*/
} p2p_cb_vtable_t;


#if defined(ANDROID)
int p2p_set_android_obj(void* jvm, void* context);
#endif

int p2p_init(void);
void p2p_destroy(void);

int p2p_set_cfg(IN ice_config_t* pcfg);
int p2p_cbvtable(IN p2p_cb_vtable_t* pvtable);

void p2p_clean_param(void);
void p2p_store_ev_param(IN ice_event_msg_t* param);
void p2p_store_relay_param(IN const ice_sdpinfo_t* ice_sdp);

int p2p_is_detection_finished(void);
OUT int p2p_get_transport_mode(void);
int p2p_get_local_ice_sdp(OUT ice_sdpinfo_t* pice);
int p2p_get_remote_ice_sdp(OUT ice_sdpinfo_t* pice);
int p2p_update_ice_media_stream(void);
int p2p_start_detection(void);
int p2p_stop_detection(void);

int p2p_do_ice_event_msg(IN ice_event_msg_t* pev);
int p2p_do_ice_recv_msg(IN ice_rtp_msg_t* pev);
int p2p_do_ice_send_msg(IN ice_rtp_msg_t* pev);
int p2p_do_ice_detection_tm(void);
int p2p_do_ice_checkline_tm(void);
int p2p_do_ice_sendping_tm(void);


#ifdef __cplusplus
}
#endif


#endif

#endif /* P2P_H */
