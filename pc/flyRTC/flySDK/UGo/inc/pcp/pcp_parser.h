/******************************************************************************

  Copyright (C), 2001-2011, DCN Co., Ltd.

 ******************************************************************************
  File Name     : pcp_parser.h
  Version       : Initial Draft
  Author        : gonghuojin
  Created       : 2013/9/23
  Last Modified :
  Description   : pcp parser header
  Function List :
  History       :
  1.Date        : 2013/9/23
    Author      : gonghuojin
    Modification: Created file

******************************************************************************/
#ifndef PCP_PARSER_H
#define PCP_PARSER_H

#ifdef __APPLE__
#include "bson.h"
#include "json_api.h"
#else
#include <bson/bson.h>
#include <json/json_api.h>
#endif

#include "pcp_cfg.h"

PCP_EXTERN_C_START

void null_parser();


enum msg_type_t
{
	PCP_MSG_CALL_TP 	= 0,
	PCP_MSG_VERSION_TP 	= 1,
	PCP_MSG_REGISTER_TP = 2
};

enum dtmf_int_value_e
{
    PCP_DTMF_0        = 0,
    PCP_DTMF_1        = 1,
    PCP_DTMF_2        = 2,
    PCP_DTMF_3        = 3,
    PCP_DTMF_4        = 4,
    PCP_DTMF_5        = 5,
    PCP_DTMF_6        = 6,
    PCP_DTMF_7        = 7,
    PCP_DTMF_8        = 8,
    PCP_DTMF_9        = 9,
    PCP_DTMF_STAR     = 10, /* (*) */
    PCP_DTMF_POUND    = 11, /* (#) */
    PCP_DTMF_A        = 12,
    PCP_DTMF_B        = 13,
    PCP_DTMF_C        = 14,
    PCP_DTMF_D        = 15,
    PCP_FLASH         = 16,
};

typedef struct str_tag
{
	int   slen;
	char* sbuf;
}str_t;

typedef struct pcp_route_tag
{
	int 	sn;		//serial number
	int 	type;	//msg type
	int 	op;		//msg option
	int 	ack;	//route ack
	int 	enc;	// 1:encrypt rc4, 2: zlib compress, 0: no encrypt and compress
	int   	dsid;   //vps id
	int 	fpv;    //from platform
	int     tpv;	//to  platform
	int     atype;  // account type	
	int64_t fuid;   //from uid 
	int64_t tuid;	//to uid
}pcp_route_t;

typedef struct pcp_msg_tag
{
	pcp_int32 	version;
	pcp_uint16	hlen;
	pcp_uint16 	blen;
	void*		hmsg;
	void*		bmsg;

}pcp_msg_t;


typedef struct pcp_version_rsp_tag
{
	int randcode;
}pcp_version_rsp_t;

typedef struct pcp_login_tag
{
	int  randcode;
	int  netmode;
	int  uin;
	int  compressEn;
	int  encryptEn;
	int  desEn;
	char uid[UGO_MAX_USERID_LEN];
	char ssid[512]; /*modify by luochengcong 20140917*/
	char version[64];
	char devId[64];
	char devType[64];
	char sessionKey[64];
}pcp_login_t;
	
typedef struct pcp_login_rsp_tag
{
	int result;
	char description[128];
}pcp_login_rsp_t;

typedef struct pcp_call_req_tag
{
    int  sflag;                             /* update support flag */
    int  meetingflag;                       /* add by wuzhaoyang20150106 */
    int  callmode;                          /* call mode */
    int  calltype;                          /*uxin call type :0 normal call 1 uxin liaoyiliao*/
    int  atype;                             /* account type */
    char call_id[UGO_MAX_CALLID_LEN];       /* callid */
    char fuid[UGO_MAX_USERID_LEN];          /* from client_number */
    char fuserid[UGO_MAX_USERID_LEN];       /* from userid */
    char fphone[UGO_MAX_PHONE_LEN];         /* from mobie phone */
    char tuid[UGO_MAX_USERID_LEN];          /* to client number */
    char tuserid[UGO_MAX_USERID_LEN];       /* to userid, for im3.0 */
    char tphone[UGO_MAX_PHONE_LEN];         /* to mobie phone */
    char ver[UGO_MAX_VERSION_LEN];          /* version */
    char fnickname[UGO_MAX_NICKNAME_LEN];   /* from nickname */
    char user_data[UGO_MAX_USER_DATA_LEN];  /* user data */
    pcp_sdp_info_t* sdp;                    /* sdp info */
}pcp_call_req_t;


typedef struct pcp_call_rsp_tag
{
	int  sflag;
	int  baudio_pt;
	int  bvideo_pt;
	int	 reason;
	char call_id[UGO_MAX_CALLID_LEN];
	char ver[UGO_MAX_VERSION_LEN];
    char roomid[UGO_MAX_ROOMID_LEN];
	pcp_sdp_info_t* sdp;
}pcp_call_rsp_t;

typedef struct pcp_push_rsp_tag
{
	int	 reason;
	char call_id[UGO_MAX_CALLID_LEN];
	pcp_sdp_info_t* sdp;
}pcp_push_rsp_t;

typedef struct pcp_call_ack_tag
{
	int  ice_type;
	int  ice_mode;
	char call_id[UGO_MAX_CALLID_LEN];
}pcp_call_ack_t;

typedef struct pcp_bye_req_tag
{
	int  reason;
	char call_id[UGO_MAX_CALLID_LEN];
}pcp_bye_req_t;

typedef struct pcp_bye_rsp_tag
{
	int  reason;
	char call_id[UGO_MAX_CALLID_LEN];
} pcp_bye_rsp_t;


typedef struct pcp_update_req_tag
{
	char call_id[UGO_MAX_CALLID_LEN];
	char mip[64];
	char apt[64];
	char vpt[64];
	int  aport;
	int  vport;
} pcp_update_req_t;

typedef struct pcp_update_rsp_tag
{
	int  reason;
	int  ice_mode;
	char call_id[UGO_MAX_CALLID_LEN];
	char apt[64];
	char vpt[64];
} pcp_update_rsp_t;

typedef struct pcp_ring_rsp_tag
{
    int  sflag;
	int  baudio_pt;
	int  bvideo_pt;
	char call_id[UGO_MAX_CALLID_LEN];
	pcp_sdp_info_t* sdp;
} pcp_ring_rsp_t;

typedef struct pcp_route_update_tag
{
	char call_id[UGO_MAX_CALLID_LEN];
	char userid[UGO_MAX_USERID_LEN];
} pcp_route_update_t;

typedef struct pcp_direct_notify_tag
{
	int  reason;
	char call_id[UGO_MAX_CALLID_LEN];
} pcp_direct_notify_t;

/* add by VintonLiu on 20150810 for recv dtmf info */
typedef struct pcp_dtmf_info_tag
{
    int  dtmf_int;          /* dtmf int value */
    char call_id[UGO_MAX_CALLID_LEN];       /* callid */
    char dtmf_str[8];       /* dtmf string value */
} pcp_dtmf_info_t;
/* VintonLiu add end */

/* add by Vinton.Liu on 2016-04-05 for add internal state passthrough signalling begin */
typedef struct pcp_in_trans_req_tag
{
    char call_id[UGO_MAX_CALLID_LEN];
    int  vie_angle;      /* video angle change */
    int  vie_state;      /* video state, preview/stop or other */
    int  p2p_state;      /* ice p2p state */
    int  mic_state;      /* audio mic device state */
    int  net_state;      /* network state, reserved */
} pcp_in_trans_req_t;

typedef struct pcp_in_trans_rsp_tag
{
    char call_id[UGO_MAX_CALLID_LEN];
    int  errcode;
} pcp_in_trans_rsp_t;
/* Vinton.Liu add end */

//add by wuzhaoyang20140605 start
typedef struct pcp_user_info_tag
{
	int  call_mode;
	char tuid[UGO_MAX_USERID_LEN];
	char tphone[UGO_MAX_PHONE_LEN];
} pcp_user_info_t;

#define PCP_MAX_MEET_NUM UGO_CONF_PARTICIPANT_MAX_NUM
/* conference participants list */
typedef struct pcp_conf_plist_tag
{
	int num;
	pcp_user_info_t user_attr[UGO_CONF_PARTICIPANT_MAX_NUM];
} pcp_conf_plist_t;

typedef struct pcp_conf_call_req_tag
{
	int  sflag;                             /*update support flag*/
	int  conftype;                          /* conference type */
	int  atype;                             /* account type, im3.0 */
	char call_id[UGO_MAX_CALLID_LEN];
	char fuid[UGO_MAX_USERID_LEN];          /* from client num */
	char fuserid[UGO_MAX_USERID_LEN];       /* from userid, im3.0 */
	char fphone[UGO_MAX_PHONE_LEN];          
	char ver[UGO_MAX_VERSION_LEN];
    char roomname[UGO_MAX_ROOMNAME_LEN];
    char roompwd[UGO_MAX_ROOMPWD_LEN];
    char remark[UGO_MAX_ROOMREMARK_LEN];
	pcp_conf_plist_t  plist;
	pcp_sdp_info_t* sdp;
}pcp_conf_call_req_t;

typedef struct pcp_conf_info_tag
{
	int pnum;
	pcp_participant_state_t pstate[UGO_CONF_PARTICIPANT_MAX_NUM];
} pcp_conf_info_t;

typedef struct pcp_conf_ntfy_msg_tag
{
    int  roomstate; // 0 idle, 1 create, 2 closed
	char callid[UGO_MAX_CALLID_LEN];
	char roomid[UGO_MAX_ROOMID_LEN];	
	pcp_conf_info_t parties;
} pcp_conf_ntfy_msg_t;

typedef struct pcp_conf_inv_req_tag
{
    char callid[UGO_MAX_CALLID_LEN];
    char roomid[UGO_MAX_ROOMID_LEN];
    char fphone[UGO_MAX_PHONE_LEN];
    pcp_conf_plist_t  plist;
} pcp_conf_inv_req_t;

typedef struct pcp_conf_del_req_tag
{
    char callid[UGO_MAX_CALLID_LEN];
    char roomid[UGO_MAX_ROOMID_LEN];
} pcp_conf_del_req_t;

/* struct for conference normal rsp format */
typedef struct pcp_conf_nml_rsp_tag
{
    int errCode;
    char callId[UGO_MAX_CALLID_LEN];
    char roomId[UGO_MAX_ROOMID_LEN];
} pcp_conf_nml_rsp_t;

typedef struct pcp_conf_convert_rsp_tag //add by wuzhaoyang20140504
{
	int  reason;
	char call_id[UGO_MAX_CALLID_LEN];
} pcp_conf_convert_rsp_t;
//add by wuzhaoyang20140605 end

#define MAX_HEAD_SIZE	250
#define MAX_BODY_SIZE	1500
#define MIN_MSG_SIZE	4
#define MAX_MSG_SIZE	2049
#define PCP_VERSION     0x20

int str_init(str_t* pstr, int slen);

void str_release(str_t* pstr);

pcp_bool check_media_available(IN pcp_media_option_t* pmedia, IN pcp_int32 baudio_pt, IN pcp_int32 bvideo_pt);


int build_msg(IN pcp_msg_t* pmsg, OUT str_t* pstr);

int parser_msg(IN str_t* msg, OUT str_t* hd, OUT str_t* bd);

/* ----------------------------------------------
   BUILDING
   --------------------------------------------- */

/*		build bson  header for message  		*/
int build_route_bson_header(IN const pcp_route_t* proute, OUT str_t *b_header);


/*	  	build json  body for message  	 	*/
int build_login_req_json_body(IN pcp_login_t* plogin, OUT str_t* j_body);

int build_call_req_json_body(IN pcp_call_req_t* pdialing, OUT str_t* j_body);

int build_call_rsp_json_body(IN pcp_call_rsp_t* pdialing, OUT str_t* j_body);

int build_push_rsp_json_body(IN pcp_push_rsp_t* ppush, OUT str_t* j_body);

int build_call_ack_json_body(IN pcp_call_ack_t* pack, OUT str_t* j_body);

int build_bye_req_json_body(IN pcp_bye_req_t* pbye, OUT str_t* j_body);

int build_bye_rsp_json_body(IN pcp_bye_rsp_t* pbye, OUT str_t* j_body);

int build_update_req_json_body(IN pcp_update_req_t* pupdate, OUT str_t* j_body);

int build_update_rsp_json_body(IN pcp_update_rsp_t* pupdate, OUT str_t* j_body);

int build_ringing_rsp_json_body(IN pcp_ring_rsp_t* pring, OUT str_t* j_body);

int build_route_update_json_body(IN pcp_route_update_t* proute, OUT str_t* j_body);

int build_m_call_req_json_body(IN pcp_conf_call_req_t* pdialing, OUT str_t* j_body);

int build_convert_conference_rsp_json_body(IN pcp_conf_convert_rsp_t* convert_rsp, OUT str_t* j_body);

int build_m_inv_req_json_body(IN pcp_conf_inv_req_t* pdialing, OUT str_t* j_body);

int build_m_del_req_json_body(IN pcp_conf_del_req_t*pdel, OUT str_t* j_body);

/* ----------------------------------------------
   PARSERING
   --------------------------------------------- */

/*		parser bson  header for message  		*/
int parser_route_bson_header(IN str_t *b_header, OUT pcp_route_t* proute);


/*	  	parser json  body for message  	 	*/
int parser_version_rsp_json_body(IN str_t* j_body, OUT pcp_version_rsp_t* pversion);

int parser_login_rsp_json_body(IN str_t* j_body, OUT pcp_login_rsp_t* plogin);

int parser_call_req_json_body(IN str_t* j_body, OUT pcp_call_req_t* pdialing);

int parser_call_rsp_json_body(IN str_t* j_body, OUT pcp_call_rsp_t* pdialing);

int parser_push_rsp_json_body(IN str_t* j_body, OUT pcp_push_rsp_t* ppush);

int parser_call_ack_json_body(IN str_t* j_body, OUT pcp_call_ack_t* pack);

int parser_bye_req_json_body(IN str_t* j_body, OUT pcp_bye_req_t* pbye);

int parser_bye_rsp_json_body(IN str_t* j_body, OUT pcp_bye_rsp_t* pbye);

int parser_update_req_json_body(IN str_t* j_body, OUT pcp_update_req_t* pupdate);

int parser_update_rsp_json_body(IN str_t* j_body, OUT pcp_update_rsp_t* pupdate);

int parser_ringing_rsp_json_body(IN str_t* j_body, OUT pcp_ring_rsp_t* pring);
	
int parser_direct_notify_json_body(IN str_t* j_body, OUT pcp_direct_notify_t* pnotify);

int parser_dtmf_info_json_body(IN str_t* j_body, OUT pcp_dtmf_info_t*pdtmf);

//add by wuzhaoyang20140605 start
int parser_parties_json_body(IN json_t* jroot, OUT pcp_conf_info_t *pconf);

int parser_m_notify_json_body(IN str_t* j_body, OUT pcp_conf_ntfy_msg_t* pdialing);

int parser_convert_conference_json_body(IN str_t* j_body, OUT char* callid);

int parser_m_nml_rsp_json_body(IN str_t* j_body, OUT pcp_conf_nml_rsp_t *pRsp);
//add by wuzhaoyang20140605 end
PCP_EXTERN_C_END

#endif


