/******************************************************************************

  Copyright (C), 2001-2011, DCN Co., Ltd.

 ******************************************************************************
  File Name     : pcp_cfg.h
  Version       : Initial Draft
  Author        : gonghuojin
  Created       : 2013/9/23
  Last Modified :
  Description   : pcp config header
  Function List :
  History       :
  1.Date        : 2013/9/23
    Author      : gonghuojin
    Modification: Created file

******************************************************************************/
#ifndef PCP_CONFIG_H
#define PCP_CONFIG_H

#include "pcp_comm.h"
#include "ugo_comm.h"

PCP_EXTERN_C_START

extern const int kDefaultAudioPort;
extern const int kDefaultVideoPort;

#define MAX_PARTICIPANT_ATTR_NUM    UGO_CONF_PARTICIPANT_MAX_NUM


/*------pcp 	struct  define  -------*/
typedef enum pcp_state
{
	PCP_STATE_WAIT,			//wait
	PCP_STATE_INVITE,		//invite 
	PCP_STATE_RINGING,		//ringing
	PCP_STATE_CONNECT,		//connect

	PCP_STATE_PUSHACTIVE, 	//ios push call active
	PCP_STATE_SYSACTIVE		//system call active	
}pcp_state_t;

enum pcp_call_mode
{
	PCP_DIRECT_CM = 4,//modify by Lion for uxin 2014.05.09
	PCP_AUTO_CM   = 5,
	PCP_FREE_CM   = 6
};

typedef enum pcp_call_type
{
	PCP_FREE_CALL_TP   = 0,
	PCP_DIRECT_CALL_TP = 1
}pcp_call_type;

enum pcp_line_mode
{
	PCP_LINE_P2P_MD  = 0,
	PCP_LINE_RTPP_MD = 1
};

enum pcp_call_role
{
	PCP_OFFER 	= 0,
	PCP_ANSWER 	= 1
};

enum pcp_ice_type
{
	PCP_ICE_STATIC_TP = 0,
	PCP_ICE_AUTO_TP	  = 1
};

enum pcp_update_type
{
	PCP_UPDATE_CLOSE    = 0x00,
	PCP_UPDATE_OPEN     = 0x01,
	PCP_CONFER_OPEN     = 0x02,//add by wuzhaoyang20140607
	PCP_FEC_OPEN        = 0x04,//add by wuzhaoyang20140607
	PCP_DIRECTFEC_OPEN  = 0x08,//add by wuzhaoyang20140607
	PCP_DIRECTFEC_MULRIPLE_OPEN = 0x10, //add by wuzhaoyang20151228
	PCP_IN_TRANS_OPEN   = 0x20,
    /* add by Vinton.Liu on 2016-05-30 for video fec and nack config begin */
	PCP_VIE_FEC_OPEN    = 0x40,
	PCP_VIE_NACK_OPEN   = 0x80
    /* Vinton.Liu add end */
};

typedef enum pcp_media_type_t
{
	AUDIO,
	VIDEO
}pcp_media_type_t;

enum pcp_route_type
{
    PCP_NON_TP 	= 0,
    PCP_CALL_TP = 10,
    PCP_CONF_TP = 30,  //for conference
    PCP_CSRV_TP = 32,  //for custom service version
};

enum pcp_route_ack_type
{
	PCP_ACK_NORMAL_RT	 = 0,
	PCP_ACK_OK_RT 		 = 1,
	PCP_NACK_OFFLINE_RT  = 2,
	PCP_NACK_DISABLED_RT = 3
};

enum pcp_non_option
{
	PCP_NON_LOGIN_OP 	= 1,
	PCP_NON_LOGOUT_OP 	= 4,
	PCP_NON_VERSION_OP  = 5
};

enum pcp_route_option
{
	PCP_CALL_REQ_OP 	= 1,
	PCP_CALL_RSP_OP 	= 2,
	PCP_CALL_ACK_OP 	= 3,
	PCP_CALL_BYE_OP		= 4,
	PCP_UPDATE_REQ_OP 	= 5,
	PCP_UPDATE_RSP_OP	= 6,
	PCP_RING_RSP_OP		= 7,
	PCP_DIR_NOTIFY_OP	= 8,
	PCP_PUSH_RSP_OP		= 9,
	PCP_CALLREQ_ACK_OP 	= 11,
	PCP_ROUTE_UPDATE_OP = 15,
	PCP_DTMF_INFO_OP    = 16,
    /* add by Vinton.Liu on 2016-04-05 for add internal state passthrough signalling begin */
	PCP_IN_TRANS_REQ_OP = 19,
	PCP_IN_TRANS_RSP_OP = 20,
    /* Vinton.Liu add end */

	//conference call op
	PCP_MEET_CALL_REQ_OP 	      = 50,
	PCP_MEET_CALL_RSP_OP          = 51,
	PCP_MEET_CALL_NOTIFY_OP       = 52,
	PCP_CONVERT_CONFERENCE_OP     = 53,
	PCP_CONVERT_CONFERENCE_RSP_OP = 54,
	/* add by VintonLiu on 20150427 for conference call */
	PCP_MEET_INV_REQ_OP   = 60,
	PCP_MEET_INV_RSP_OP   = 61,
	PCP_MEET_DEL_REQ_OP   = 62,
	PCP_MEET_DEL_RSP_OP   = 63,
	/* VintonLiu add end */

	PCP_UNSUPPORT_RSP_OP    = 99
};

/* enc (8bit char), 0 ~ 2 bits for encrypt,  3 ~ 5 bits for compress, 6 ~7 for reserved */
typedef enum pcp_enc_type_en
{
    PCP_ENC_TYPE_NONE       = 0x00,  /* no encrypt and compress */
    PCP_ENC_TYPE_RC4        = 0x01,  /* rc4 encrypt */
    PCP_ENC_TYPE_COMPRESS   = 0x08   /* zlib compress */
}pcp_enc_type_e;

typedef struct pcp_media_option_t
{
	pcp_bool 	flag;		//use flag
	int     	pr;			//priority
	int 		pt;			//pt value
	int 		port;       //rtp port
	char    	type[21];   //audio or video
	char    	mine[21];   //codecs name
}pcp_media_option_t;

#define PCP_MAX_MEDIA_NUM  20
typedef struct pcp_media_list_t
{
	int 			   media_num;
	pcp_media_option_t media_arr[PCP_MAX_MEDIA_NUM];
}pcp_media_info_t;

typedef struct cdd_option_t
{
	char attr_desc[128]; //attribute describe
}cdd_option_t;

#define PCP_MAX_CANDIDATE   8
typedef struct sdp_ice_info_t
{
	char  		  pwd[64];
	char  		  ufrag[64];
	int   		  cdd_num;
	cdd_option_t  cdd_arr[PCP_MAX_CANDIDATE];
}pcp_ice_info_t;

typedef struct pcp_rtpp_option_t
{
	int 	delay;
	int 	lost;
	char 	ip[64];
}pcp_rtpp_option_t;

#define PCP_MAX_RTPP   UGO_RTPP_MAX_NUM
typedef struct pcp_rtpp_info_t
{
	int 			  rtpp_num;
	pcp_rtpp_option_t rtpp_arr[UGO_RTPP_MAX_NUM];
}pcp_rtpp_info_t;

/*added by luochengcong 20141009*/
typedef struct pcp_resolution_info_t
{
    int uiMaxDecodeDefinition;
	int camera_width;
	int camera_height;
	int rotate;
}pcp_resolution_info_t;


#define VIDEOVERSION 1
typedef struct pcp_through_info_t
{
    pcp_resolution_info_t resolution;
    int video_enabled;
	int decode_video_fps;
	int start_bitrate;
	int min_bitrate;
	int max_bitrate;
	unsigned int v0;
}pcp_through_info_t;
/*end by luochengcong 20141009*/

typedef struct pcp_sdp_info_t
{
	char 			 c_ip[UGO_MAX_IP_ADDR_LEN];
	char 			 v_version[21];
	pcp_rtpp_info_t  rtpp;
	pcp_media_info_t media;
	pcp_ice_info_t   ice;
	unsigned int     support_v;
    pcp_through_info_t through; /*added by luochengcong 20141009*/
}pcp_sdp_info_t;

typedef struct pcp_relay_info_t
{
	char m_ip[64];
	int	 a_port;
	int  v_port;
}pcp_relay_info_t;

typedef struct pcp_session_media_t
{
	int  session_pt;
	int	 trans_mode;

	int  local_port;
	char local_addr[UGO_MAX_IP_ADDR_LEN];
	
	int  remote_port;
	char remote_addr[UGO_MAX_IP_ADDR_LEN];
}pcp_session_media_t;

#define MAX_TRACE_BUF_SIZE	1024
typedef struct pcp_trace_t
{
	int 	left;
	int 	used;
	char	trace_buf[MAX_TRACE_BUF_SIZE];
}pcp_trace_t;
#define PCP_TRACE_T_SIZE (sizeof(pcp_trace_t))

typedef struct pcp_codec_param
{
	int enabled;
	int pt;
	char mine[64];
}pcp_codec_param_t;

#define PCP_MAX_CODECS_NUM 	20
typedef struct pcp_codec_list
{
	int num;
	pcp_codec_param_t codec_arr[PCP_MAX_CODECS_NUM];
}pcp_codec_list_t;

typedef struct pcp_participant_state_t
{
	int  call_state;               /* 0  idle, 1 Calling, 2 Ringing, 3 Connected,  4 Disconnected */
	int  reason;
	char uid[UGO_MAX_USERID_LEN];
	char phone[UGO_MAX_PHONE_LEN];
} pcp_participant_state_t;

typedef struct pcp_conf_state_t
{
    int     conftype;                      /* conference type: 0 -- normal conference;  1 -- all direct model */
    int     roomstate;                     /* 0 idle, 1 create, 2 closed */
	char    conference_id[UGO_MAX_CALLID_LEN];  /* maybe use callid */
    char    roomname[UGO_MAX_ROOMNAME_LEN];
    char    roompwd[UGO_MAX_ROOMPWD_LEN];
    char    roomid[UGO_MAX_ROOMID_LEN];
    char    remark[UGO_MAX_ROOMREMARK_LEN];
    int     pnum;            /* participant number */
	pcp_participant_state_t pstate[UGO_CONF_PARTICIPANT_MAX_NUM];
}pcp_conf_state_t;
#define PCP_CONF_STATE_SIZE sizeof(pcp_conf_state_t)

typedef struct ugo_call_trans_state_tag pcp_call_trans_state_t;
#define PCP_CALL_TRANS_STATE_T_SIZE (sizeof(pcp_call_trans_state_t))

typedef struct pcp_session_t
{
	pcp_bool expired_flag;                  /* session expired flag, if true, it can`t be used at all */
	pcp_bool audio_flag;                    /* audio codecs consultation flag */	
	pcp_bool video_flag;                    /* video codecs consultation flag */
	pcp_bool afec_flag;                     /* audio fec consultation flag */
	pcp_bool vfec_flag;                     /* video fec consultation flag */
	pcp_bool vnack_flag;                    /* video nack consultation flag */
	pcp_bool directfec_flag;                /* direct fec consultation flag */
	pcp_bool directfec_mulriple_flag;       /* direct mulriple fec consultation flag *///add by wuzhaoyang20151228
	pcp_bool ice_flag;                      /* ice consultation flag */
	pcp_bool rc4_flag;                      /* rc4 support flag */
	pcp_bool intrans_flag;                  /* in trans req support flag */
	pcp_uint32 start_time;                  /* time in ms when receive connecting event */
	pcp_uint32 ctime;                       /* offset time between receive connecting and ringing event */
	
	int		 meetingflag;
	int 	 mode;		                    /* call mode */
	int  	 role;		                    /* call role, offer or answer */
	int	 	 dsid;   	                    /* vps id */
	int      msg_type;                      /* msg type */
	int      atype;                         /* account type */
	char 	 fuid[UGO_MAX_USERID_LEN];          /* call from uid, client num */
	char     fuserid[UGO_MAX_USERID_LEN];       /* call from userid, for im3.0 */
	char 	 fphone[UGO_MAX_PHONE_LEN];         /* call from phone */
    char     fnickname[UGO_MAX_NICKNAME_LEN];   /* call from nickname */
    char     user_data[UGO_MAX_USER_DATA_LEN];   /* user data */
	char 	 tuid[UGO_MAX_USERID_LEN];          /* call to uid */
	char     tuserid[UGO_MAX_USERID_LEN];       /* call to userid, for im3.0 */
	char 	 tphone[UGO_MAX_PHONE_LEN];         /* call to phone */
	char 	 callid[UGO_MAX_CALLID_LEN];        /* call id */
	pcp_state_t			call_state;         /* call state */
	pcp_relay_info_t	relay;		        /* relay server info */
	pcp_session_media_t audio_stream;       /* best audio stream */
	pcp_session_media_t video_stream;       /* best video stream */
	pcp_sdp_info_t* 	lsdp; 		        /* local sdp info */
	pcp_sdp_info_t*		rsdp; 		        /* remote sdp info */
	pcp_trace_t*		call_log;	        /* call log trace */

	pcp_conf_state_t*  conf_state;//add by wuzhaoyang20140605
	pcp_call_trans_state_t trans_state;     /* call passthrough state */
}pcp_session_t;

/**pcp register event  notification callback prototype*/
typedef void (*pcp_register_event_cb_t)(int type, int reason, const char* describe);

/**pcp call event  notification callback prototype*/
typedef void (*pcp_call_event_cb_t)(int type, int reason, const char* describe, void* param, int size);

/**pcp send message  callback prototype*/
typedef void (*pcp_send_msg_cb_t)(const char* msg, int slen);

/**pcp call state changed  notification callback prototype*/
typedef void (*pcp_call_state_cb_t)(int state);

/**pcp call log notification callback prototype*/
typedef void (*pcp_call_log_cb_t)(int level, const char* describe);


/**
 * This structure holds all callbacks that the application should implement.
 *	None is mandatory.
**/
typedef struct pcp_callback_vtable_tag
{
	pcp_register_event_cb_t register_event_cb;	/*register event callback*/
	pcp_call_event_cb_t		call_event_cb;		/*call event callback*/
	pcp_send_msg_cb_t   	call_send_cb; 		/*msg send callback*/
	pcp_call_state_cb_t 	call_state_cb;		/*notifies call state changes*/
	pcp_call_log_cb_t 		call_log_cb; 		/*notifies that call log trace */
} pcp_cb_vtable_t;

typedef struct pcp_proxy_tag
{
	pcp_bool rc4_enabled;	            /* rc4 encrypt enabled */
	pcp_bool compress_enabled;          /* zlib compress enabled */
	pcp_bool video_enabled;             /* video enabled */ 
	pcp_bool ice_enabled;               /* ice enabled */

	int  ice_type;                      /* ice type 0:not support ice line auto to change 1: support */
	int  update_flag;                   /* update flag:0 not support else support */
	int  platform;
	int  tlv_enabled;                   /* im3.0, use sk tlv protocol stack */
	int  ipv6_enabled;                  /* ipv6 enabled */
	int  csrv_enabled;                  /* custom services version enabled, 0 disabled, 1 enabled */
	int  atype;                         /* account type: userid:0, client_num:1 */
	char uid[UGO_MAX_USERID_LEN];           /* client num */
	char userid[UGO_MAX_USERID_LEN];        /* userid, for im3.0*/
	char phone[UGO_MAX_PHONE_LEN];
	char brand[UGO_MAX_BRAND_LEN];
	char nickname[UGO_MAX_NICKNAME_LEN]; 
	char version[UGO_MAX_VERSION_LEN];
	char localipaddr[UGO_MAX_IP_ADDR_LEN];   /* local ip addr */
} pcp_proxy_t;

typedef struct pcp_cfg_tag
{
	pcp_proxy_t     proxy;
	pcp_rtpp_info_t	rtpp;
	pcp_cb_vtable_t vtable;
} pcp_cfg_t;

typedef struct pcp_video_para_tag
{	
	int      video_mode;
	int      camera_width;
	int      camera_height;
	int      camera_rotate;
	int      camera_fps;
	int      camera_idx;
	int      externFrameFormate;
} pcp_video_para_t;
#define PCP_VIDEO_PARA_SIZE     sizeof(pcp_video_para_t)

/* Dtmf event param */
typedef struct pcp_dtmf_event_node_tag
{
    int  dtmf_int;          /* dtmf int value */
    char dtmf_str[8];       /* dtmf string value */
} pcp_dtmf_event_node_t;
#define PCP_DTMF_EVENT_NODE_SIZE sizeof(pcp_dtmf_event_node_t)

/* Normal call event param */
typedef struct ugo_callev_pm_tag pcp_call_param_t;
#define PCP_CALL_PARAM_SIZE (sizeof(pcp_call_param_t))

/* Conference notify state */
typedef struct ugo_conf_ntfy_state_tag pcp_conf_ntfy_state_t;
#define PCP_CONF_NTFY_STATE_SIZE sizeof(pcp_conf_ntfy_state_t)


int create_cfg(void);
int remove_cfg(void);

OUT int is_local_rc4_enabled(void);
OUT int is_local_compress_enabled(void);
OUT int is_local_ice_enabled(void);
OUT int is_local_video_enabled(void);
OUT int is_local_audio_fec_enabled(void);
/* add by Vinton.Liu on 2016-05-30 for video fec and nack config begin */
OUT int is_local_video_fec_enabled(void);
OUT int is_local_video_nack_enabled(void);
/* Vinton.Liu add end */
OUT int is_local_direct_fec_enabled(void);
OUT int is_local_direct_mulriple_fec_enabled(void);
OUT int pcp_is_tlv_enabled(void);
OUT int pcp_is_ipv6_enabled(void);
OUT int pcp_is_csrv_enabled(void);
int pcp_get_localipaddr(OUT char* ipaddr);
int set_proxy_cfg(IN const pcp_proxy_t* pproxy);
int set_rtpp_cfg(IN const pcp_rtpp_info_t* pcfg);
int set_cb_vtable(IN const pcp_cb_vtable_t* pvtable);
int set_system_state(IN const pcp_state_t state);
int set_randcode(IN int randcode);
int set_login_client_num(char * uid);
/* add by Vinton.Liu on 2016-05-30 for video fec and nack config begin */
int pcp_update_vie_fec_flag(IN const bool enabled);
int pcp_update_vie_nack_flag(IN const bool enabled);
/* Vinton.Liu add end */

OUT int get_system_state(void);
OUT int get_randcode(void);
OUT int get_ice_type(void);
OUT int get_upate_flag(void);
OUT int get_platform(void);
OUT int get_account_type(void);
int get_brand(OUT char* brand);
int get_nickname(OUT char* nickname);
int get_uid(OUT char* uid);
int get_userid(OUT char* uid);
int get_userphone(OUT char* phone);
int get_proxy_version(OUT char* version);
int get_rtpp_cfg(OUT pcp_rtpp_info_t* pcfg);

void register_event_cb(IN int type, IN int reason, IN const char* describe);
void call_event_cb(IN int type, IN int reason, IN const char* describe, IN void* param, IN int size);
void call_send_cb(IN const char* msg, IN int slen);
void call_state_cb(IN int state);
void call_log_cb(IN int level, IN const char* describe);
int get_video_codec(pcp_codec_param_t**p, int *num, int **firstPt);


PCP_EXTERN_C_END

#endif

