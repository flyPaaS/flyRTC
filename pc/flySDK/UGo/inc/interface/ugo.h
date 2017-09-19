/***************************************************************************
** 版权所有:  Copyright (c) 2014-2019
** 文件名称:  ugo.h
** 文件标识: 
** 内容摘要:  interface define for ugo softphone 
** 当前版本:  v1.0
** Author  :  Rambo fu
** 完成日期: 2014年9月06日
** 修改记录: 
** 修改日期: 
** 版本号: 
** 修改人: 
** 修改内容: 
***************************************************************************/

#ifndef _UGO_INTERFACE_H_
#define _UGO_INTERFACE_H_

#include "ugo_comm.h"
#include "MediaEngineInterface.h"

#ifndef IN
#define IN
#endif

#ifndef OUT
#define OUT
#endif

/***************************************************************************
**
**						enum/state/event defintion
**
****************************************************************************/
/****UGo module state*****/
enum UGo_State 
{	
	eUGo_STATE_WAIT,		//wait
	eUGo_STATE_INVITE,		//invite 
	eUGo_STATE_RINGING,     //ringing
	eUGo_STATE_CONNECT,		//connect

	eUGo_STATE_PUSHACTIVE, 	//ios push call active
	eUGo_STATE_SYSACTIVE	//system call active
};

/****UGo event type*******/
enum UGo_event_type
{
	eUGo_CALLDIALING_EV,	//call dialing event
	eUGo_CALLINCOMING_EV,	//call incoming event
	eUGo_CALLANSWER_EV,		//call answer event
	eUGo_CALLHUNGUP_EV,		//call hungup event
	eUGo_CALL_TRANS_STATE_EV,    // call in trans state changed ev
	
	eUGo_NETWORK_EV,	    //network state event

	eUGo_VIDEO_NETWORK_EV,	    //network state event
	
	eUGo_UPSINGLEPASS_EV,	//UP RTP single pass
	eUGo_DNSINGLEPASS_EV,	//DN RTP single pass
	
	eUGo_TCPTRANSPORT_EV,   //tcptransport event only use for test

	eUGo_CONFERENCE_EV,     //conference call event  add by wuzhaoyang20140605
	eUGo_GETDTMF_EV,        //get dtmf event add by wuzhaoyang20150423
    
	eUGo_VIDEO_EV,
	eUGo_REMOTE_VIDEO_STATE_NOTIFY_EV
};

/****UGo event(eUGo_CALLDIALING_EV ~ eUGo_CALLHUNGUP_EV) reason**/
enum UGo_event_reason
{
	/**success reason**/
	eUGo_Reason_Success                 = 0,    //success

	/*public reason 1~29*/
	eUGo_Reason_NotAccept               = 1, 	//not accept
	eUGo_Reason_RtppTimeOut             = 2, 	//RTPP RTP timeout
	eUGo_Reason_NoBalance               = 3, 	//nobalance
	eUGo_Reason_UpdateMediaFail         = 4, 	//update media fail
	eUGo_Reason_Busy                    = 5, 	//busy
	eUGo_Reason_Reject                  = 6, 	//reject
	eUGo_Reason_NotFind                 = 7, 	//not find
	eUGo_Reason_TooShort                = 8, 	//Forbidden(callee too short)
	eUGo_Reason_CalleeFrozen            = 9, 	//callee have been frozen
	eUGo_Reason_Freeze                  = 10,	//caller have been frozen
	eUGo_Reason_Expired                 = 11, 	//caller expired
	eUGo_Reason_Cancel                  = 12, 	//Terminater for Cancel 
	eUGo_Reason_Forbidden               = 13, 	//Forbidden(caller binding number)
	eUGo_Reason_NoResponse              = 14, 	//no response timeout
	eUGo_Reason_NetworkDisable          = 15,   //The network is not supported
	eUGo_Reason_UnReachable             = 16,   //Signaling inaccessible(NACK)
	eUGo_Reason_UnableToPush            = 17,   //ios unable to push
	eUGo_Reason_CallidNotExist          = 18,   //callid is not exist
	eUGo_Reason_NoAnswer                = 19, 	//callee have no answer
	eUGo_Reason_ConnectFaild            = 20,	//connect faild
	eUGo_Reason_BlackList               = 23,   // blacklist

	/*client reason 30~49*/
	eUGo_Reason_HungupMyself            = 30,   //call hungup by myself
	eUGo_Reason_HungupPeer              = 31,   //call hungup by peer
	eUGo_Reason_HungupTCPDisconnected   = 32,   //"tcp event:server connect failed!!!"
	eUGo_Reason_HungupRTPTimeout        = 33,   //"medie engine: RTP time out!!!"
	eUGo_Reason_StartSendFailed         = 34,   // start send failed

	/**vps sever reason 50~79**/
	eUGo_Reason_ProxyAuth               = 50, 	//proxy auth 
	eUGo_Reason_MsgHeadError            = 51, 	//message head error
	eUGo_Reason_MsgBodyError            = 52, 	//message body error
	eUGo_Reason_CallIDExist             = 53, 	//callid exist
	eUGo_Reason_MsgTimeOut              = 54, 	//message timeout
	eUGo_Reason_UserIdNotExist          = 55,   // Called UserId not exist

	/* conference reason  add by wuzhaoyang20140621 */
	eUGo_Reason_CONF_NO_EXIST           = 60,  //find the conference fail
	eUGo_Reason_CONF_STATE_ERR          = 61,  //conference state error
	eUGo_Reason_CONF_FULL               = 62,  //conference full
	eUGo_Reason_CONF_CREATE_ERR         = 63,  //create conference fail
	eUGo_Reason_CONF_CALL_FAILED        = 64,  //call procedure fail
	eUGo_Reason_CONF_MEDIA_FAILED       = 65,  //apply media resource fail
	eUGo_Reason_CONF_TER_UNSUPPORT      = 66,  //the peer don't support
	eUGo_Reason_CONF_PARTICIPANT_OVER   = 67,  //too many participant
	eUGo_Reason_CONF_FINISHED           = 68,  //conference finished

	/* conference reason  add by wuzhaoyang20140605 */
	eUGo_Reason_StateNotify             = 70,   //conference state notify
	eUGo_Reason_ActiveModeConvert       = 71,   //active change to conference mode
	eUGo_Reason_PassiveModeConvert      = 72,   //passive change to conference mode

	/*temporary reason 80~98(notify:80~89,other:90~98)*/
	eUGo_Reason_NotifyPeerNotFind       = 80,   //call notify peer uid not find
	eUGo_Reason_NotifyPeerOffLine       = 81,   //call notify peer offline
	eUGo_Reason_NotifyPeerTimeout       = 82,   //call notify free call timeout
	
	eUGo_Reason_Connecting              = 97,	//connecting between send invite and don't receive response
	eUGo_Reason_Ringing                 = 98,   //Ringing response

	/**unkown error**/
	eUGo_Reason_UnkownError             = 99,	//unkown error
    
	/**Pass-through code [10000~20000]**/
	/*
		UCPaas support AS server Pass-through error code to SDK client,
		so if VPS server response error code is [10000~ 20000],
		we need Pass-through it to client.
	*/
};

/****tcptransport state****/
enum UGo_TcpTransport_state
{
	eUGo_TCP_DISCONNECTED,    /*network problem or socket closed*/
	eUGo_TCP_CONNECTED,		  /*tcp work ok */
	eUGo_TCP_RECONNECTED	  /*tcp reconnected ,this will send reconnect msg to cs server*/	
};

/**system call state**/
enum UGo_system_callstate
{
	eUGo_SYSCALL_IDLE,		/*system call is idle*/
	eUGo_SYSCALL_ACTIVE     /*system call is busy*/
};

/**
  *configeration option for UGo, ID value is Corresponding 
  *to special UGo module configuration, UGo_set_config()/
  *UGo_get_config() use it to config special module parameter.
**/
enum  UGo_ModuleID
{
	UGO_CFG_PARAM_MODULE_ID  = 0, /* config UGo prms structure "ugo_cfg_t" */
	UGO_CFG_TCP_MODULE_ID	 = 1, /* config TCP prms structure "ugo_cfg_tcp_t" */
	UGO_CFG_ICE_MODULE_ID	 = 2, /* config ICE prms structure "ugo_cfg_ice_t" */
	UGO_RTPP_CFG_MODULE_ID	 = 3, /* config RTPP prms structure "ugo_rtpp_cfg_t" */
	UGO_EMODEL_MODULE_ID	 = 4  /* config EMODEL prms structure (reservation) */
};

enum ugo_call_mode
{
	eUGo_CM_DIRECT = 4,	/*only try direct mode*/
	eUGo_CM_AUTO   = 5, /*first try free mode, if not success, it will try direct mode*/
	eUGo_CM_FREE   = 6  /*only try free mode*/
};

enum UGo_video_capability
{
    eUGo_VIE_DISABLED = 0,          /* not support video */
    eUGo_VIE_ENABLED = 1,           /* support normal video */
    eUGo_VIE_RING_PREVIEW = 2       /* support video and remote preview on ringing */
};

/************************************************************************
**
**						Struct defintion
**
*************************************************************************/
typedef struct ugo_rtpp_info_tag
{
	ucpaas_int32    delay;	                    /* delay time (ms) */
	ucpaas_int32    lost;                       /* Packet loss rate */
	ucpaas_int8     ip[UGO_MAX_IP_ADDR_LEN];  /* rtpp server ip,eg:121.11.12.123 */
	ucpaas_int8     reserved[16];               /* reserved data ,currently not used */
} ugo_rtpp_info_t;

typedef struct ugo_rtpp_cfg_tag
{
	ucpaas_int32        rtpp_num;
	ugo_rtpp_info_t     rtpps[UGO_RTPP_MAX_NUM];
}ugo_rtpp_cfg_t;
#define UGO_RTPP_CFG_T_SIZE (sizeof(ugo_rtpp_cfg_t))

typedef struct ugo_cfg_tag
{
    ucpaas_int32 rc4_enabled;                   /* rc4 encryption enabled */
    ucpaas_int32 compress_enabled;              /* compress enabled */
    ucpaas_int32 video_enabled;	                /* 0 video unable, 1 video enabled, 2 video Preview*/
    ucpaas_int32 platform;		                /* current platform,value(pc:0x01,ios:0x02 android:0x04) */
    ucpaas_int32 atype;                         /* account type, value(userid:0, client_num:1) */
    ucpaas_int32 tlv_enabled;                   /* im3.0, use sk tlv protocol stack */
    ucpaas_int32 ipv6_enabled;                  /* enable ipv6, ipv4 will be disabled if ipv6 enabled */
	ucpaas_int32 csrv_enabled;                  /* custom services version enabled, 0 disabled, 1 enabled */
    ucpaas_int8  uid[UGO_MAX_USERID_LEN];       /* user count, client_number */
    ucpaas_int8  userid[UGO_MAX_USERID_LEN];    /* userid, for im3.0 */
    ucpaas_int8	 phone[UGO_MAX_PHONE_LEN];      /* user phone number */
    ucpaas_int8	 brand[UGO_MAX_BRAND_LEN];      /* current brand,value(kc,uu,sky,efl,3g,uxin,feniin) */
    ucpaas_int8  nickname[UGO_MAX_NICKNAME_LEN];    /* fnickname */
    ucpaas_int8  localipaddr[UGO_MAX_IP_ADDR_LEN];  /* local ip addr */
    ucpaas_int8  reserved[128];	                /* reserved 16 int  data ,currently not used */
} ugo_cfg_t;
#define UGO_CFG_T_SIZE    (sizeof(ugo_cfg_t))

typedef struct  ugo_cfg_tcp_tag
{
	ucpaas_int32 tcp_enabled;		/* tcp transport enabled  */
	ucpaas_int8 tcp_srvaddr[UGO_MAX_TCP_SERVER_LEN];   /* tcp server address when tcp_enabled is true */
} ugo_cfg_tcp_t;
#define UGO_TCP_CFG_SIZE    (sizeof(ugo_cfg_tcp_t))

typedef struct ugo_cfg_ice_tag
{
	ucpaas_int32 ice_enabled;       /* ice p2p enabled */
	ucpaas_int32 ipv6_enabled;      /* ipv6 enabled */
	ucpaas_int8 stun_server[UGO_MAX_STUN_SERVER_LEN];   /* stun server address when ice_enabled is true */
	ucpaas_int8 reserved[256];      /* reserved 64 int ,currently not used  */
} ugo_cfg_ice_t;
#define UGO_ICE_CFG_SIZE    (sizeof(ugo_cfg_ice_t))

typedef struct ugo_log_trace_pm_tag
{
	ucpaas_int32 level;
	ucpaas_int8  filepath[UGO_MAX_FILEPATH_LEN];
	ucpaas_int8  reserved[8];  /* reserved data ,currently not used */
} ugo_log_trace_pm_t;

typedef struct ugo_tcp_recv_pm_tag
{
	ucpaas_int32 recv_len;
	ucpaas_int8  recv_buf[UGO_MAX_RECV_DATA_LEN];
	ucpaas_int8  reserved[8];  /* reserved data ,currently not used */
} ugo_tcp_recv_pm_t;

/* Emodel info struct */
typedef struct ugo_emodel_info_tag
{
	ME_emodel_info_t        emodel;             /* emodel value of current call */
    ugo_call_info_t         callinfo;           /* call info value of current call */
} ugo_emodel_info_t;
#define UGO_EMODEL_INFO_SIZE    sizeof(ugo_emodel_info_t)

/**ugo event  notification callback prototype*/
typedef void (*ugo_event_cb_t)( ucpaas_int32 ev_type, 
                                    ucpaas_int32 ev_reason, 
                                    const ucpaas_int8* something, 
                                    const void* param);

/**ugo send message  callback prototype*/
typedef void (*ugo_send_msg_cb_t)( const ucpaas_int8* msg, 
                                     ucpaas_int32 slen);

/**ugo trace log notification callback prototype*/
typedef void (*ugo_trace_log_cb_t)( ucpaas_int8* summary, 
                                        ucpaas_int8* detail, 
                                        enum eME_TraceLevel level);

typedef void (*ugo_encrypt_cb_t)( ucpaas_uint8* in_data,
							        ucpaas_uint8* out_data,
							        ucpaas_int32 bytes_in,
							        ucpaas_int32* bytes_out);

typedef void (*ugo_decrypt_cb_t)( ucpaas_uint8* in_data,
							        ucpaas_uint8* out_data,
							        ucpaas_int32 bytes_in,
							        ucpaas_int32* bytes_out);

typedef void(*ugo_screenshot_cb_t)( const ucpaas_uint8* dst_argb, 
                                        ucpaas_int32 dst_stride, 
                                        ucpaas_int32 width, 
                                        ucpaas_int32 height,
                                        ucpaas_int32 islocal,
                                        ScreenType screen_type);

typedef void (*ugo_media_proc_cb_f)( ucpaas_int16* audio_10ms, 
                                     ucpaas_int32 samples,
                                     ucpaas_int32 samplingFreqHz,
                                     ucpaas_boolean isStereo,
                                     ucpaas_int16* audio_out,
                                     ucpaas_int32* samples_out);

/* add by Vinton.Liu on 2016-03-18 for  support external audio device push pcm to and pull pcm from engine begin */
typedef void (*ugo_init_playout_cb_f)(ucpaas_uint32 sample_rate, 
                                     ucpaas_uint32 bytes_per_sample,
                                     ucpaas_uint32 num_of_channels);

typedef void (*ugo_init_recording_cb_f)(ucpaas_uint32 sample_rate, 
                                     ucpaas_uint32 bytes_per_sample,
                                     ucpaas_uint32 num_of_channels);

typedef int (*ugo_read_recording_data_cb_f)(ucpaas_sint8* audio_data, 
                                            ucpaas_uint32 size);

typedef int (*ugo_write_playout_data_cb_f)(const ucpaas_sint8* audio_data, 
                                            ucpaas_uint32 size);
/* Vinton.Liu add end */


/**
 * This structure holds all callbacks that the application should implement.
 *	None is mandatory.
**/
typedef struct ugo_cb_vtable_t
{
	ugo_event_cb_t          event_cb;           /* call event callback */
	ugo_send_msg_cb_t       send_cb; 	        /* msg send callback */
	ugo_trace_log_cb_t 	    log_cb; 	        /* notifies that call log trace */
	ugo_encrypt_cb_t        encrypt; 	        /* encrypt encode data of rtp */
	ugo_decrypt_cb_t        decrypt; 	        /* decrypt encode data of rtp */
	ugo_screenshot_cb_t     screen_shot_cb;	    /* screenshot callback */
	ugo_media_proc_cb_f     ext_mediaproc_cb;   /* external pcm media processing */
	/* add by Vinton.Liu on 2016-03-21 for support external audio device push pcm to and pull pcm from engine begin */
    ugo_init_playout_cb_f initplayout_cb;            /* init playout param */
    ugo_init_recording_cb_f initrecording_cb;        /* init reocrding param */
    ugo_read_recording_data_cb_f read_recording_cb;      /* read audio capture data from external audio device */
    ugo_write_playout_data_cb_f write_playout_cb;        /* write audio playout data to external audio device */
    /* Vinton.Liu add end */
	ucpaas_int32            reserved[3];        /* reserved data ,currently not used */	
} ugo_cb_vtable_t;

/*************************************************************************************
**
**								interface defintion
**
*************************************************************************************/
/************************************************************
Function	: UGo_load_media_engine
Description : load media engine
Input		: None
Output		: None 
Return	    : None
Remark      : None
Modified    : 2015/10/19    V1.0.0  VintonLiu
************************************************************/
ucpaas_int32 UGo_load_media_engine();

/************************************************************
Function	: UGo_unload_media_engine
Description : destroy media engine
Input		: None
Output		: None 
Return	    : None
Remark      : None
Modified    : 2013/1/16    V1.0.0  Rambo Fu
************************************************************/
ucpaas_int32 UGo_unload_media_engine(void);

#if defined(ANDROID)
/************************************************************
Function	: UGo_set_android_obj
Description : set android env obj
Input		: None
Output		: None 
Return	    : Successful return 0 , Failure returns -1
Remark      : None
Modified    : 2015/10/19    V1.0.0  Vinton Liu
************************************************************/
ucpaas_int32 UGo_set_android_obj(media_jni_env_pm_t* env);
#endif

/************************************************************
Function	: UGo_init
Description : init UGo module
Input		: None
Output		: None 
Return	    : Successful return 0 , Failure returns -1
Remark      : None
Modified    : 2012/12/10    v1.0.0  Rookie John
************************************************************/
ucpaas_int32 UGo_init(void);


/************************************************************
Function	: UGo_destroy
Description : destroy UGo module
Input		: None
Output		: None 
Return	    : Successful return 0 , Failure returns -1
Remark      : None
Modified    : 2012/12/10    v1.0.0  Rookie John
************************************************************/
ucpaas_int32 UGo_destroy(void);


/************************************************************
Function	: UGo_callback_vtable
Description : set UGo callback table 
Input		: cb_vtable => (App callback vtable pointer)
Output		: None 
Return	    : Successful return 0 , Failure returns -1
Remark     :  must call before UGo_init
Modified    : 2012/12/10    v1.0.0  Rookie John
************************************************************/
ucpaas_int32 UGo_callback_vtable(IN ugo_cb_vtable_t* cb_vtable);


/************************************************************
Function	: UGo_update_system_callstate
Description : update system call state
Input		: state =>  syetem call state (enum UGo_system_callstate)
Output		: None 
Return	    : Successful return 0 , Failure returns -1
Remark      : None
Modified    : 2012/12/10    v1.0.0  Rookie John
************************************************************/
ucpaas_int32 UGo_update_system_callstate(IN ucpaas_int32 state);


/************************************************************
Function	: UGo_call_dialing
Description : Call requested
Input		: pCallParam => (call dialing param , see ugo_call_dialing_para_t defined)
Output		: None 
Return	    : Successful return 0 , Failure returns -1
Remark      : None
Modified    : 2012/12/10    v1.0.0  Rookie John
************************************************************/
ucpaas_int32 UGo_call_dialing(IN const ugo_call_dialing_para_t* pCallParam);


/************************************************************
Function	: UGo_ios_call_incoming
Description : Answer call requested
Input		: callparam =>  (call incoming rsp for ios, see ugo_ios_incoming_rsp_pm_t defined)
Output		: None 
Return	    : Successful return 0 , Failure returns -1
Remark      : only use by ios
Modified    : 2012/12/10    v1.0.0  Rookie John
************************************************************/
ucpaas_int32 UGo_ios_call_incoming_rsp(IN const ugo_ios_incoming_rsp_pm_t* callparam);

/************************************************************
Function	: UGo_call_answered
Description : Answer call requested
Input		: None
Output		: None 
Return	    : Successful return 0 , Failure returns -1
Remark      : None
Modified    : 2012/12/10    v1.0.0  Rookie John
************************************************************/
ucpaas_int32 UGo_call_answered(void);


/************************************************************
Function	: UGo_call_hangup
Description : Hangup the call
Input		: iReason => the reason of hang up, set 
			  eUGo_Reason_HungupMyself if user hang up, else use
			  external definition of hang up.
Output		: None 
Return	    : Successful return 0 , Failure returns -1
Remark      : None
Modified    : 2012/12/10    v1.0.0  Rookie John
************************************************************/
ucpaas_int32 UGo_call_hangup(int iReason);


/************************************************************
Function	: UGo_record_start
Description : Start record for the call
Input		: filerecord => (the record file info) max len = 256
Output		: None 
Return	    : Successful return 0 , Failure returns -1
Remark      : None
Modified    : 2012/5/18    V1.0.0  Rookie John
************************************************************/
ucpaas_int32 UGo_record_start(IN const media_file_record_pm_t* filerecord);

/************************************************************
Function	: UGo_record_stop
Description : Stop record for the call
Input		: None
Output		: None 
Return	    : Successful return 0 , Failure returns -1
Remark      : None
Modified    : 2012/5/18    V1.0.0  Rookie John
************************************************************/
ucpaas_int32 UGo_record_stop(void);

/************************************************************
Function	: UGo_screenshot_start
Description : start screen for video
Input		: None
Output		: None
Return	    : Successful return 0 , Failure returns -1
Remark      : None
Modified    : 2015/5/7    V1.0.0  Devin Yu
************************************************************/
ucpaas_int32 UGo_screenshot_start(int islocal, ScreenType type);

/************************************************************
Function	: UGo_play_file
Description : Start play audio file
Input	  	: mode => (media file play mod, see enum media_file_play_md_t defined) 
			  fileplay => (media file play param, see media_file_play_pm_t defined)
Output		: None 
Return	    : Successful return 0 , Failure returns -1
Remark      : None
Modified    : 2014/4/2    V1.0.0  Rambo.Fu
************************************************************/
ucpaas_int32 UGo_play_file(IN ucpaas_int32 mode, IN const media_file_play_pm_t* fileplay);

/************************************************************
Function	: UGo_stop_file
Description : Stop play file
Input		: None
Output		: None 
Return	    : Successful return 0 , Failure returns -1
Remark      : None
Modified    : 2014/4/2    V1.0.0  Rambo.Fu
************************************************************/
ucpaas_int32 UGo_stop_file(void);

/************************************************************
Function	: UGo_send_DTMF
Description : Send DTMF message
Input		: c_dtmf => (the char for DTMF),eg: 0~9,*,#,A~D
Output		: None 
Return	    : Successful return 0 , Failure returns -1
Remark      : None
Modified    : 2012/5/18    V1.0.0  Rookie John
************************************************************/
ucpaas_int32 UGo_send_DTMF(IN ucpaas_int8 c_dtmf);

/************************************************************
Function	: UGo_set_mic_mute
Description : Mute set
Input		: enabled => (Whether to enabled mute , TRUE or FALSE)
Output		: None 
Return	    : Successful return 0 , Failure returns -1
Remark      : None
Modified    : 2012/5/18    V1.0.0  Rookie John
************************************************************/
ucpaas_int32 UGo_set_mic_mute(IN ucpaas_int8 enabled);

/************************************************************
Function	: UGo_get_mic_mute
Description : Mic Mute get
Input		: None
Output		: None 
Return	    : return 0 disabled, return 1 enabled
Remark      : None
Modified    : 2014/9/22    V1.0.0  Vinton Liu
************************************************************/
ucpaas_int32 UGo_get_mic_mute();

/************************************************************
Function	: UGo_set_loudspeaker_status
Description : Speaker set
Input		: enabled => (Whether to enabled speaker, TRUE or FALSE)
Output		: None 
Return	    : Successful return 0 , Failure returns -1
Remark      : None
Modified    : 2012/5/18    V1.0.0  Rookie John
************************************************************/
ucpaas_int32 UGo_set_loudspeaker_status(IN ucpaas_int8 enabled);

/************************************************************
Function	: UGo_get_loudspeaker_status
Description : speaker status get
Input		: None
Output		: None 
Return	    : return 0 disabled, return 1 enabled
Remark      : None
Modified    : 2014/9/22    V1.0.0  Vinton Liu
************************************************************/
ucpaas_int32 UGo_get_loudspeaker_status();

/************************************************************
Function	: UGo_get_recording_device_status
Description : get recording device status
Input		: None
Output		: None 
Return	    : return 0 disabled, return 1 enabled
Remark      : None
Modified    : 2016/3/15    V1.0.0  Vinton Liu
************************************************************/
ucpaas_int32 UGo_get_recording_device_status();

/************************************************************
Function	: UGo_set_log_file
Description : Set vogo log file path
Input		: logtrace => (log trace file info,see ugo_log_trace_pm_t defined)
Output		: None
Return	    : Successful return 0 , Failure returns -1
Remark      : None
Modified    : 2012/5/18    V1.0.0  Rookie John
************************************************************/
ucpaas_int32 UGo_set_log_file(IN const ugo_log_trace_pm_t* logtrace);

/************************************************************
Function	: UGo_enable_stack_log
Description :   Set ugo log file path
Input		: filepath => (log trace file path)
Output		: None
Return	    : Successful return 0 , Failure returns -1
Remark      : None
Modified    : 2015/8/28    V1.0.0  Vinton Liu
************************************************************/
int UGo_enable_stack_log(const char* filepath);

/************************************************************
Function	: UGo_get_state
Description : Get current UGo state
Input		: None
Output		: None
Return	    : Success return UGo state, see UGo_State degined, faild return -1
Remark      : None
Modified    : 2012/12/10     v1.0.0  Rookie John
************************************************************/
OUT ucpaas_int32 UGo_get_state(void);


/************************************************************
Function	: UGo_tcp_update_state
Description : Update current tcptransport state
Input		: tcptransport_state => (tcp transport state)
Output		: None
Return	    : Success return 0, Faild return -1
Remark      : None
Modified    : 2012/12/10     v1.0.0  Rookie John
************************************************************/
ucpaas_int32 UGo_tcp_update_state(IN ucpaas_int32 tcp_state);


/************************************************************
Function	: UGo_tcp_recv_msg
Description : Input recv message data 
Input		: recv_len => (tcp recv msg len)
                    : recv_buf => ( tcp recv msg content )
Output		: None 
Return	    : Successful return 0 , Failure returns -1
Remark      : NOne
Modified    : 2012/12/10    v1.0.0  Rookie John
************************************************************/
ucpaas_int32 UGo_tcp_recv_msg(ucpaas_int32 recv_len, void*  recv_buf);

/************************************************************
Function	: UGo_get_version
Description : Get current version
Input		: None
Output		: ugo_version => (ugo verson info)
Return	    : Successful return 0 , Failure returns -1
Remark      : None
Modified    : 2012/12/10    v1.0.0  Rookie John
************************************************************/
ucpaas_int32 UGo_get_version(OUT ucpaas_int8* ugo_version);


/************************************************************
Function	: UGo_set_api
Description : set the os version apilevel
Input		: apilevel
Output		: 
Return	    : Successful return 0 , Failure returns -1
Remark      : None
Modified    : 2013/12/25    v1.0.0  Rookie John
************************************************************/
ucpaas_int32  UGo_set_api(IN ucpaas_int32 apilevel);


/************************************************************
Function	: UGo_get_emodel_value;
Description : get emodel value
Input		: pemodel => (emodel value struct pointer)
Output		: None
Return	    : Success return 0, faild return -1
Remark      : None
Modified    : 2012/12/10     v1.0.0  Rookie John
************************************************************/
ucpaas_int32 UGo_get_emodel_value(OUT ugo_emodel_info_t* pemodel);

/************************************************************
Function	: UGo_set_config;
Description : set UGO common config
Input		: methodID: refer to enum UGo_ModuleID/eME_module_cfg_id
			  cfg_struct: refer to corresponding struction for enum UGo_ModuleID/eME_module_cfg_id
			  version: verison number, default 0
Output		: None
Return	    : Success return 0, faild return -1
Remark      : None
Modified    : 2014/05/08     v1.0.0  Charlie yuan
			  2014/09/06     v1.0.1  Rambo.Fu
************************************************************/
ucpaas_int32 UGo_set_config(ucpaas_int32 methodID, void * cfg_struct, ucpaas_int32 version);

/************************************************************
Function	: Common_UGo_get_config;
Description : get UGo commom config
Input		: methodID: refer to enum UGo_ModuleID/eME_module_cfg_id
			  version: verison number, default 0
Output		: cfg_struct: refer to corresponding struction for enum UGo_ModuleID/eME_module_cfg_id
Return		: Success return 0, faild return -1
Remark		: None
Modified	: 2014/05/08	 v1.0.0  Charlie yuan
		      2014/09/06     v1.0.1  Rambo.Fu
************************************************************/
ucpaas_int32 UGo_get_config(ucpaas_int32 methodID, void * cfg_struct, ucpaas_int32 version);

/************************************************************
Function	: UGo_video_preset;
Description : preset video
Input		: enumProfileLevel
Output		: None
Return	    : Success return 0, faild return -1, -2, -3
Remark      : None
Modified    : 2015/08/17     v1.0.0  devin yu
************************************************************/
ucpaas_int32  UGo_video_preset(ucpaas_int8* key, ucpaas_int8 *value);

/************************************************************
Function	: UGo_video_start;
Description : Start video(open)
Input		: Video working mode
			  0x01: camera
			  0x02: networksend
			  0x04: netwprk recive
			  0x08: local surface
			  0x10: remote surface 
Output		: none
Return		: 0 if succeed, else -1;
Remark		: None
Modified	: 2014/06/30	 v1.0.0  Mao Ningyuan
************************************************************/
ucpaas_int32 UGo_video_start(int send_receive);
/************************************************************
Function	: UGo_video_stop;
Description : Stop video(Closed)
Input		: Video working mode
			  0x01: camera
			  0x02: networksend
			  0x04: netwprk recive
			  0x08: local surface
			  0x10: remote surface
Output		: none
Return		: 0 if succeed, else -1;
Remark		: None
Modified	: 2014/06/30	 v1.0.0  Mao Ningyuan
************************************************************/
ucpaas_int32 UGo_video_stop(int send_receive);

/************************************************************
Function	: UGo_video_start_record;
Description : Start record video(Closed)
Input		: file path, input or output select
Output		: none
Return		: 0 if succeed, else -1;
Remark		: None
Modified	: 2014/09/11	 v1.0.0  Mao Ningyuan
************************************************************/
ucpaas_int32 UGo_video_start_record(const ME_video_record_param_t* record_param);

/************************************************************
Function	: UGo_video_stop_record;
Description : Stop record video(Closed)
Input		: input or output select
Output		: none
Return		: 0 if succeed, else -1;
Remark		: None
Modified	: 2014/09/11	 v1.0.0  Mao Ningyuan
************************************************************/
ucpaas_int32 UGo_video_stop_record(const unsigned int stream_select);

/************************************************************
Function	: UGo_video_set_camera;
Description : Stop record video(Closed)
Input		: Camera parameter
Output		: none
Return		: 0 if succeed, else -1;
Remark		: None
Modified	: 2014/09/18	 v1.0.0  Mao Ningyuan
************************************************************/
ucpaas_int32 UGo_video_set_camera(const ME_video_camera_param_t* param);

/************************************************************
Function	: UGo_video_get_camera_count;
Description : Get camera count
Input		: none
Output		: none
Return		: 0 if succeed, else -1;
Remark		: None
Modified	: 2014/09/18	 v1.0.0  Mao Ningyuan
************************************************************/

ucpaas_int32 UGo_video_get_camera_count(void);
/************************************************************
Function	: UGo_video_get_camera_state;
Description : Get camera state which was used currently
Input		: none
Output		: camera state
Return		: 0 if succeed, else -1;
Remark		: None
Modified	: 2014/09/18	 v1.0.0  Mao Ningyuan
************************************************************/
ucpaas_int32 UGo_video_get_camera_state(ME_video_camera_param_t* camera_state);

/************************************************************
Function	: UGo_video_get_camera_info;
Description : Get camera info
Input		: none
Output		: camera info
Return		: 0 if succeed, else -1;
Remark		: None
Modified	: 2014/09/18	 v1.0.0  Mao Ningyuan
************************************************************/
ucpaas_int32 UGo_video_get_camera_info(const ucpaas_uint32 camera_idx, ME_video_camera_info_t* camera_info);	

/************************************************************
Function	: UGo_video_get_camera_info;
Description : Get camera Capability
Input		: camera idx, cap idx
Output		: camera cap
Return		: 0 if succeed, else -1;
Remark		: None
Modified	: 2014/09/18	 v1.0.0  Mao Ningyuan
************************************************************/
ucpaas_int32 UGo_video_get_camera_capability(const ucpaas_uint8 camera_idx, 
                                                    const ucpaas_uint8 cap_idx, 
                                                    ME_video_camera_capability_t* camera_capability);

/************************************************************
Function	: UGo_video_set_send_recive_rotation;
Description : set video rotation
Input		: 
Output		: 
Return		: 0 if succeed, else -1;
Remark		: None
Modified	: 2015/06/15	 v1.0.0  Xie Chunlai
************************************************************/
ucpaas_int32 UGo_video_set_send_recive_rotation(const ucpaas_uint32 send_rotation, 
                                            const ucpaas_uint32 recive_rotation);

/************************************************************
Function	: UGo_video_incoming_frame;
Description : input video data if extern catpure used!
Input		: 
Output		: 
Return		: 0 if succeed, else -1;
Remark		: None
Modified	: 2015/12/17	 v1.0.0  Xie Chunlai
************************************************************/
ucpaas_int32 UGo_video_incoming_frame(const ucpaas_uint8* data, 
    const ucpaas_uint32 len);

/************************************************************
 Function	: UGo_video_set_filter;
 Description : set filters
 Input		:filter type and sensity
 Output		:none
 Return		: 0 if succeed, else -1;
 Remark		: None
 Modified	: 2016/6/29	 v1.0.0  zhanganlong
 ************************************************************/
ucpaas_int32 UGo_video_set_filter(const ucpaas_int32 filter_type,
                                      const ucpaas_float32 filter_sensisty);


#ifdef AUTO_TEST_SWITCH
/************************************************************
Function	: UGo_start_rtpdump;
Description : start rtp dump on specific direction
Input		: direction
Output		: 
Return		: 0 if succeed, else -1;
Remark		: None
Modified	: 2015/05/04	 v1.0.0  Vinton.Liu
************************************************************/	
ucpaas_int32 UGo_start_rtpdump(const char* fileName, int direction);

/************************************************************
Function	: UGo_stop_rtpdump;
Description : stop rtp dump on specific direction
Input		: direction
Output		: 
Return		: 0 if succeed, else -1;
Remark		: None
Modified	: 2015/05/04	 v1.0.0  Vinton.Liu
************************************************************/	
ucpaas_int32 UGo_stop_rtpdump(int direction);

/************************************************************
Function	: UGo_set_testmode;
Description : set media engine rtp mode
Input		: direction
Output		: 
Return		: 0 if succeed, else -1;
Remark		: None
Modified	: 2015/05/04	 v1.0.0  Vinton.Liu
************************************************************/
ucpaas_int32 UGo_set_testmode(int mode);

/************************************************************
Function	: UGo_get_localIP;
Description : get local receiver listening addr
Input		: 
Output		: 
Return		: 0 if succeed, else -1;
Remark		: None
Modified	: 2015/05/04	 v1.0.0  Vinton.Liu
************************************************************/
ucpaas_int32 UGo_get_localIP(int& port, char *ipAddr);

/************************************************************
Function	: UGo_get_destIP;
Description : get remote addr
Input		: 
Output		: 
Return		: 0 if succeed, else -1;
Remark		: None
Modified	: 2015/05/04	 v1.0.0  Vinton.Liu
************************************************************/
ucpaas_int32 UGo_get_destIP(int& port, char *ipAddr);

ucpaas_int32 UGo_set_encrypt_cb(ugo_encrypt_cb_t encrypt, ugo_decrypt_cb_t decrypt);
#endif /* AUTO_TEST_SWITCH */

//add by wuzhaoyang20140605
/************************************************************
Function	: UGo_conference_call_dialing
Description : Conference Call requested
Input		:  (dial struct, see struct ugo_conf_dialing_para_t)
Output		: None 
Return	    : Successful return 0 , Failure returns -1
Remark      : None
Modified    : 2014/06/05    v1.0.0  wuzhaoyang
************************************************************/
ucpaas_int32 UGo_conference_call_dialing(IN ugo_conf_dialing_para_t *pDialParam);

/************************************************************
Function	: UGo_conference_invite_parties
Description : Conference invite new participants
Input		:  (dial struct, see struct conference_dialing_param_t)
Output		: None 
Return	    : Successful return 0 , Failure returns -1
Remark      : None
Modified    : 2015/04/28    v1.0.0 VintonLiu
************************************************************/
ucpaas_int32 UGo_conference_invite_parties(IN ugo_conf_dialing_para_t *pDialParam);

/************************************************************
Function	: UGo_delete_conference
Description : delete current conference
Input		:  iReason => the reason of hang up
Output		: None 
Return	    : Successful return 0 , Failure returns -1
Remark      : None
Modified    : 2015/04/28    v1.0.0 VintonLiu
************************************************************/
ucpaas_int32 UGo_delete_conference(int reason);


ucpaas_int32 UGo_change_ice_mode(); // yangjj add

//#ifdef IOS_SET_FILEPATH
ucpaas_int32 UGo_set_filepath(char *filepath);//add by wuzhaoyang20160407
//#endif
#endif

