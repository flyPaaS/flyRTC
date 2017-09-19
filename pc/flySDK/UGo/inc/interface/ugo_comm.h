#ifndef UGO_COMM_H_
#define UGO_COMM_H_

#include "typedef.h"

/************************************************************************
**
**						Macro defintion
**
*************************************************************************/

/* define max string len */
#define UGO_MAX_VERSION_LEN         64
#define UGO_MAX_CALLID_LEN          64
#define UGO_MAX_USERID_LEN          32
#define UGO_MAX_NICKNAME_LEN        64
#define UGO_MAX_PHONE_LEN           32
#define UGO_MAX_BRAND_LEN           32
#define UGO_MAX_USER_DATA_LEN       128
#define UGO_MAX_ROOMID_LEN          64
#define UGO_MAX_ROOMNAME_LEN        64
#define UGO_MAX_ROOMPWD_LEN         64
#define UGO_MAX_ROOMREMARK_LEN      64
#define UGO_MAX_STUN_SERVER_LEN     128
#define UGO_MAX_TCP_SERVER_LEN      128
// ipv4 max len 15, ipv6 max len 39
#define UGO_MAX_IP_ADDR_LEN         48
#define UGO_MAX_CODEC_NAME_LEN      16
#define UGO_MAX_FILEPATH_LEN        128
#define UGO_MAX_RECV_DATA_LEN       4096

/* define default value */
#define UGO_CONF_PARTICIPANT_MAX_NUM    16
#define UGO_RTPP_MAX_NUM                10 /*Max rtpplist num*/

/************************************************************************
**
**						Struct defintion
**
*************************************************************************/


/* Normal audio/video call dialing param */
typedef struct ugo_call_dialing_para_tag
{
    ucpaas_int8     tuid[UGO_MAX_USERID_LEN];           /* callee's userid */
    ucpaas_int8     tphone[UGO_MAX_PHONE_LEN];          /* callee's mobie phone */    
    ucpaas_int8     user_data[UGO_MAX_USER_DATA_LEN];   /* user data */
    ucpaas_int32    call_mode;                          /* call mode */
    ucpaas_int32    calltype;		                    /* uxin calltype: 0:normal call 1:uxin liaoyiliao */
    ucpaas_int32    video_enabled;                      /* is video call */
    ucpaas_int8     reserved[16];                       /* reserved data ,currently not used */
} ugo_call_dialing_para_t;
#define UGO_CALL_DIALING_PARA_SIZE  sizeof(ugo_call_dialing_para_t)

/* Conference audio call dialing param */
typedef struct ugo_user_info_tag
{
	ucpaas_int32    call_mode;                  /* call mode,see enum ugo_call_mode */
	ucpaas_int8     uid[UGO_MAX_USERID_LEN];    /* callee uid */
	ucpaas_int8     phone[UGO_MAX_PHONE_LEN];   /* callee phone */
	ucpaas_int8     reserved[16];                 /* reserved,currently not used */
} ugo_user_info_t;

typedef struct ugo_conf_dialing_para_tag
{	
	ucpaas_int32        conftype;
    ucpaas_int8         roomname[UGO_MAX_ROOMNAME_LEN];
    ucpaas_int8         roompwd[UGO_MAX_ROOMPWD_LEN];
    ucpaas_int8         remark[UGO_MAX_ROOMREMARK_LEN];
    ucpaas_int32        user_num;    /* conference participant number */	
	ugo_user_info_t     user_attr[UGO_CONF_PARTICIPANT_MAX_NUM];
} ugo_conf_dialing_para_t;
#define UGO_CONF_DIALING_PARA_SIZE  sizeof(ugo_conf_dialing_para_t)

typedef struct ugo_ios_incoming_rsp_pm_tag
{
	ucpaas_int8  callid[UGO_MAX_CALLID_LEN];
	ucpaas_int32 vpsid; 
	ucpaas_int32 reason;
	ucpaas_int8  reserved[8];  /* reserved data ,currently not used */
} ugo_ios_incoming_rsp_pm_t;
#define UGO_IOS_INCOMING_RSP_PM_SIZE    sizeof(ugo_ios_incoming_rsp_pm_t)

/******* ugo event callback param define *********/
/* Normal audio/video call event callback param struct */
typedef struct ugo_callev_pm_tag
{
	ucpaas_int32 	 videoflag;                             /* video flag, 0:disabled 1:enabled */
	ucpaas_int32     audiofec;	                            /* audio fec flag  0:disabled 1:enabled */
	ucpaas_int32     videofec;                              /* video fec consultation result. 0: failed 1: successed */
	ucpaas_int32     videonack;                             /* video nack consultation result. 0: failed 1: successed */
	ucpaas_int32     directfec;	                            /* direct fec flag  0:disabled 1:enabled */
	ucpaas_int32     directmulriplefec;	                    /* direct mulriple fec flag  0:disabled 1:enabled *///add by wuzhaoyang20151228
	ucpaas_int32 	 calltype;	                            /* call type,0:free call, 1:direct call */
	ucpaas_int32     meetingflag;                           /* 1:conference dialing, 0:normal dialing *///add by wuzhaoyang20150106
	ucpaas_int32     ucalltype;                             /* uxin calltype:0 normal call 1 uxin liaoyiliao */
	ucpaas_int8 	 callid[UGO_MAX_CALLID_LEN];            /* callid */
	ucpaas_int8 	 fuid[UGO_MAX_USERID_LEN];              /* from uid */
	ucpaas_int8      fuserid[UGO_MAX_USERID_LEN];           /* from userid, for im3.0 */
	ucpaas_int8      fphone[UGO_MAX_PHONE_LEN];             /* from phone */
	ucpaas_int8		 fnickname[UGO_MAX_NICKNAME_LEN];       /* from name */
	ucpaas_int8      tuid[UGO_MAX_USERID_LEN];              /* to uid */
	ucpaas_int8      tuserid[UGO_MAX_USERID_LEN];           /* to userid, for im3.0 */
	ucpaas_int8      tphone[UGO_MAX_PHONE_LEN];             /* tphone */
	ucpaas_int8      user_data[UGO_MAX_USER_DATA_LEN];      /* user data */
} ugo_callev_pm_t;

/* Conference audio call event callback param struct */
typedef struct participant_state_tag
{
	ucpaas_int32    call_state;                     /*  participant state */
	ucpaas_int32    reason;
	ucpaas_int8     uid[UGO_MAX_USERID_LEN];        /*  participant uid */
	ucpaas_int8     phone[UGO_MAX_PHONE_LEN];       /*  participant phone */
} participant_state_t;

typedef struct ugo_conf_ntfy_state_tag
{
    ucpaas_int32            roomstate;       /* conference state: 0 idle, 1 create, 2 closed */
	ucpaas_int32            pnum;            /* participant number	in conference */
	participant_state_t     pstate[UGO_CONF_PARTICIPANT_MAX_NUM]; /* participant state in conference */
} ugo_conf_ntfy_state_t;
#define UGO_CONF_NTFY_STATE_SIZE    sizeof(ugo_conf_ntfy_state_t)

/* Call info of the current call */
typedef struct ugo_call_info_tag
{
	ucpaas_int32 cmode;                             /* call mode */
	ucpaas_int32 mmode;                             /* trans mode */
	ucpaas_int32 ctime;                             /* cost time of telephone */
	ucpaas_int32 cstate;                            /* comunicate state */
	ucpaas_int32 role;                              /* call role, offer or answer */
	ucpaas_int8  callid[UGO_MAX_CALLID_LEN];        /* callid */
	ucpaas_int8  mgw[UGO_MAX_IP_ADDR_LEN];        /* ip of media gateway */
	ucpaas_int8  sgw[UGO_MAX_IP_ADDR_LEN];        /* ip of signalling gateway */
	ucpaas_int8  mcodec[UGO_MAX_CODEC_NAME_LEN];    /* codec of media */
	ucpaas_int32 mgwport;                           /* port of media gateway */
	ucpaas_int32 ismute;                            /* if mircrophone is muted when hang up */	
	ucpaas_int32 spreason;
} ugo_call_info_t;

/* passthrough call state */
typedef struct ugo_call_trans_state_tag
{
    ucpaas_int32 vie_angle; /* video angle change */
    ucpaas_int32 vie_state; /* video state, preview/stop or other */
    ucpaas_int32 p2p_state; /* ice p2p state */
    ucpaas_int32 mic_state; /* audio mic device state */
    ucpaas_int32 net_state; /* network state, reserved */
} ugo_call_trans_state_t;

#endif // UGO_COMM_H_


