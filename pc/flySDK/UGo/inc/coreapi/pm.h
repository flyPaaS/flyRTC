/******************************************************************************

  Copyright (C), 2001-2011, DCN Co., Ltd.

 ******************************************************************************
  File Name     : pm.h
  Version       : Initial Draft
  Author        : gonghuojin
  Created       : 2014/5/29
  Last Modified :
  Description   : uc config param mananger head file
  Function List :
  History       :
  1.Date        : 2014/5/29
    Author      : gonghuojin
    Modification: Created file

******************************************************************************/
#ifndef PM_MANAGER_H
#define PM_MANAGER_H

#include "comm.h"
#include "ugo.h"


/**uc event  notification callback prototype*/
typedef void (*uc_event_cb_t)(int type, int reason, const char* describe, void* param);

/**uc send message  callback prototype*/
typedef void (*uc_send_msg_cb_t)(const char* msg, int slen);

/**uc state changed  notification callback prototype*/
typedef void (*uc_call_state_cb_t)(int state);

/**uc call log notification callback prototype*/
typedef void (*uc_trace_log_cb_t)(int level, const char* describe);

typedef void (*uc_encrypt_cb_t)(ucpaas_uint8* in_data,
							        ucpaas_uint8* out_data,
							        ucpaas_int32 bytes_in,
							        ucpaas_int32* bytes_out);

typedef void (*uc_decrypt_cb_t)(ucpaas_uint8* in_data,
							        ucpaas_uint8* out_data,
							        ucpaas_int32 bytes_in,
							        ucpaas_int32* bytes_out);

typedef void(*uc_screenshot_cb_f)(const ucpaas_uint8* dst_argb, 
                                    ucpaas_int32 dst_stride, 
									ucpaas_int32 width, 
                                    ucpaas_int32 height, 
									ucpaas_int32 islocal,
                                    ScreenType screen_type);   

typedef void (*uc_media_proc_cb_f)(ucpaas_int16* audio_10ms, 
                                   ucpaas_int32 samples,
                                   ucpaas_int32 samplingFreqHz,
                                   ucpaas_boolean isStereo,
                                   ucpaas_int16* audio_out,
                                   ucpaas_int32* samples_out);

/* add by Vinton.Liu on 2016-03-18 for  support external audio device push pcm to and pull pcm from engine begin */
typedef void (*uc_init_playout_cb_f)(ucpaas_uint32 sample_rate, 
                                     ucpaas_uint32 bytes_per_sample,
                                     ucpaas_uint32 num_of_channels);

typedef void (*uc_init_recording_cb_f)(ucpaas_uint32 sample_rate, 
                                     ucpaas_uint32 bytes_per_sample,
                                     ucpaas_uint32 num_of_channels);

typedef int (*uc_pull_capture_data_cb_f)(ucpaas_sint8* audio_data, 
                                            ucpaas_uint32 size);

typedef int (*uc_push_render_data_cb_f)(const ucpaas_sint8* audio_data, 
                                            ucpaas_uint32 size);
/* Vinton.Liu add end */


/**
 * This structure holds all callbacks that the application should implement.
 *	None is mandatory.
**/
typedef struct uc_callback_vtable
{
	uc_event_cb_t		event_cb;	/*event callback*/
	uc_send_msg_cb_t   	send_cb; 	/*msg send callback*/
	uc_call_state_cb_t 	state_cb;	/*notifies state changes*/
	uc_trace_log_cb_t 	log_cb; 	/*notifies log trace */
	uc_encrypt_cb_t     encrypt; 	/*encrypt encode data of rtp*/
	uc_decrypt_cb_t	    decrypt; 	/*decrypt encode data of rtp*/

	uc_screenshot_cb_f  screenshot_cb;		/* remote screenshot callback*/
    uc_media_proc_cb_f  extmedia_cb;        /* external media processing */
    
    /* add by Vinton.Liu on 2016-03-21 for support external audio device push pcm to and pull pcm from engine begin */
    uc_init_playout_cb_f initplayout_cb;            /* init playout param */
    uc_init_recording_cb_f initrecording_cb;        /* init reocrding param */
    uc_pull_capture_data_cb_f pull_capture_cb;      /* pull audio capture data */
    uc_push_render_data_cb_f push_render_cb;        /* push audio playout data */
    /* Vinton.Liu add end */
} uc_cb_vtable_t;

typedef struct uc_cfg_t
{
	ugo_cfg_t		  	  ugo_cfg;  /*ugo config*/
	
	//add by charlie (yuan )2014.04.28
	ugo_cfg_tcp_t		  tcp_cfg;  /*tcp  config*/
	ugo_cfg_ice_t		  ice_cfg;  /*ice  config*/
	//end add by charlie (yuan)
	
	ugo_rtpp_cfg_t	  	  rtpp_cfg; /*rtpp config*/
	uc_cb_vtable_t 		  uc_table; /*uc cb table*/
}uc_cfg_t;
#define UC_CFG_SIZE	(sizeof(uc_cfg_t))


int pm_create(void);
int pm_remove(void);

OUT int pm_is_local_rc4_enabled(void);
OUT int pm_is_local_ice_enabled(void);
OUT int pm_is_local_video_enabled(void);
OUT int pm_is_tcp_enabled(void);
OUT int pm_is_tlv_enabled();
OUT int pm_is_compress_enabled();
OUT int pm_is_ipv6_enabled();
/* add by Vinton.Liu on 2016-06-02 for custom service version begin */
OUT int pm_is_csrv_enabled();
/* Vinton.Liu add end */

/* add by VintonLiu on 20150709 for IM3.0 */
int pm_get_account_type(void);
/* VintonLiu add end */

int pm_set_uc_cfg(IN const ugo_cfg_t* pcfg);
int pm_set_tcp_cfg(IN const ugo_cfg_tcp_t* pcfg);
int pm_set_ice_cfg(IN const ugo_cfg_ice_t* pcfg);
int pm_set_rtpp_cfg(IN const ugo_rtpp_cfg_t* pcfg);
int pm_set_cb_vtable(IN const uc_cb_vtable_t* pvtable);


int pm_get_uc_cfg(OUT ugo_cfg_t* pcfg);
int pm_get_tcp_cfg(OUT ugo_cfg_tcp_t* pcfg);
int pm_get_ice_cfg(OUT ugo_cfg_ice_t* pcfg);
int pm_get_rtpp_cfg(OUT ugo_rtpp_cfg_t* pcfg);

OUT int pm_get_platform(void);
int pm_get_brand(OUT char* brand);
int pm_get_uid(OUT char* uid);
int pm_get_userid(OUT char* userid);
int pm_get_userphone(OUT char* phone);
int pm_get_nickname(OUT char* nickname);
int pm_get_localipaddr(OUT char* ipaddr);
int pm_get_tcp_serveraddr(OUT char* srvaddr);
int pm_get_ice_stunaddr(OUT char* stunaddr);
int pm_get_ice_ipv6_enabled();

void pm_uc_event_cb(IN int type, IN int reason, IN const char* describe, IN void* param);
void pm_uc_send_msg_cb(IN const char* msg, IN int slen);
void pm_uc_call_state_cb(IN int state);
void pm_uc_trace_log_cb(IN int level, IN const char* describe);




#endif

