/******************************************************************************

  Copyright (C), 2001-2011, DCN Co., Ltd.

 ******************************************************************************
  File Name     : pcp_skt_parser.h
  Version       : Initial Draft
  Author        :  VintonLiu
  Created       : 2015/08/19
  Last Modified :
  Description   : pcp binary parser
  Function List :
  History       :
  1.Date        : 2015/08/19
    Author      : VintonLiu
    Modification: Created file

******************************************************************************/
#ifndef __PCP_SKT_PARSER_H__
#define __PCP_SKT_PARSER_H__

#if defined(SKT_PROTOCOL_SUPPORT_ENABLED)

#include "mmpkg.h"
#include "sktlvapp.h"
#include "mmcompress.h"
#include "json_api.h"
#include "pcp_cfg.h"
#include "pcp_parser.h"

#define MAX_JSON_BODY_SIZE 512
#define PCP_SKT_BODY_DATA_MAX_SIZE   2000
#define PCP_SKT_MSG_MAX_SIZE   2049
#define DEFAULT_VOIP_HEADER_VER   0x01

#define PCP_SKT_COMPRESS_VER MMCOMPRESS_PACK_VERSION_1

typedef enum PcpSktCompressAlgEn
{
    PCP_SKT_COMPRESS_ALG_NO = MicroMsg::MMCPR_NO_COMPRESS,
    PCP_SKT_COMPRESS_ALG_ZLIB = MicroMsg::MMCPR_ZLIB_COMPRESS
}PcpSktCompressAlg_E;

typedef enum PcpSktMsgCmdEn
{
    PCP_SKT_MSG_CMD_UNKNOWN         = -1,
    PCP_SKT_MSG_CMD_AUTH_REQ        = 1,            /* Auth */
    PCP_SKT_MSG_CMD_AUTH_RESP       = 30001,        /* Auth response */
    PCP_SKT_MSG_CMD_REAUTH_REQ      = 2,            /* ReAuth request */
    PCP_SKT_MSG_CMD_REAUTH_RSP      = 30002,        /* ReAuth response */
    PCP_SKT_MSG_CMD_VOIP_CALL       = 2100,         /* Voip normal call */
    PCP_SKT_MSG_CMD_VOIP_CONF       = 3000,         /* Voip conference call */
    PCP_SKT_MSG_CMD_VOIP_CSRV       = 6100,         /* Voip custom service version call */
    PCP_SKT_MSG_CMD_NOOP_REQ        = 600100,       /* Noop req, heartbeat */
    PCP_SKT_MSG_CMD_NOOP_RSP        = 10600100      /* Noop rsp */
}PcpSktMsgCmd_E;

typedef enum PcpAuthOpEn
{
    PCP_AUTH_OP                = 101,
    PCP_REAUTH_OP              = 246
}PcpAuthOp_E;

typedef enum PcpSktMsgDefaultEn
{
	SKT_PROTO_VER		= 1,	/* IM version */	
	MIN_HEADER_LEN		= 16, 	/* Min length of the fixed header */
	MAX_HEADER_LEN		= 16  	/* Max length of the fixed header */	
}PcpSktMsgDefault_E;

typedef struct tagPcpSktMsgHeader
{
    pcp_uint32      msgLen;      /* The total length of the message, include the fixed header */
    pcp_uint16      headerLen;   /* Fixed header length */
    pcp_uint16      version;     /* Protocol version, default {PROTO_VER} */
    pcp_uint32      cmd;         /* Command value */
    pcp_uint32      seq;         /* Sequence NO., increaseing */
}__attribute__ ((packed))PcpSktMsgHeader_t;
#define PCP_SKT_MSG_HEADER_SIZE     (sizeof(PcpSktMsgHeader_t))

typedef struct tagPcpSktVoipBodyHeader
{
    pcp_uint8        headlen;       /* voip fixed header length */
    pcp_uint8        ver;           /* voip business version */
    pcp_uint16       op;            /* voip call operation code */
    pcp_uint32       servid;        /* vps index */
    pcp_uint32       clientid;      /* for server, not used in client app */
    pcp_uint8        atype;         /* account type. 0: userid, 1: client_num */
    pcp_uint8        ack;           /* is ack msg,  default 0. */ 
                                    /* 0: request msg; 1: ACK msg; 
                                                                2: NACK for user offline; 
                                                                3: NACK for destination unreachable */
    pcp_uint8        enc;           /* msg is encrypt. 0: cleartext; 1: RC4 encrypt */
}__attribute__ ((packed))PcpSktVoipBodyHeader_t;
#define PCP_SKT_VOIP_BODY_HEADER_SIZE     (sizeof(PcpSktVoipBodyHeader_t))
#define PCP_SKT_VOIP_MSG_HEADER_SIZE \
    (PCP_SKT_MSG_HEADER_SIZE + PCP_SKT_VOIP_BODY_HEADER_SIZE)

typedef struct tagPcpSktMsg
{
    PcpSktMsgHeader_t*       msgHeader;            /* msg fixed header */
    PcpSktVoipBodyHeader_t*  bodyHeader;           /* msg body fixed header */
    pcp_uint32               dataLen;              /* msg body data len */
    pcp_int8*                bodyData;             /* msg body binary data */    
}PcpSktMsg_t;

/* msg compress param */
typedef struct tagPcpSktCompress
{
    pcp_int16               CompressVersion;      /* Compress version */
    pcp_int16               CompressAlgorithm;    /* Compress Algorithm */
    pcp_int32               CompressLen;          /* data len before compress */
    pcp_int32               CompressedLen;        /* data len after compress  */
}PcpSktCompress_t;


////////// COMMON FUNCTIONS ///////////
int pcp_skt_build_msg(IN PcpSktMsg_t* pMsg, OUT str_t* pStr);
int pcp_skt_parse_msg_header(IN str_t* pMsg, OUT PcpSktMsgHeader_t* pMsgHeader, OUT str_t* pBody);
int pcp_skt_parse_voip_body(IN str_t* pMsg, OUT PcpSktVoipBodyHeader_t* pBodyHeader, OUT str_t* pData);
int pcp_skt_build_msg_header(PcpSktMsgHeader_t *pHeader,
                                    IN unsigned int cmd, IN unsigned int dataLen);

///////// AUTH FUNCTIONS ////////////
int pcp_skt_build_auth_req_msg(IN pcp_login_t* pLogin, OUT str_t* pMsg);
int pcp_skt_parse_auth_rsp_msg(IN str_t* pMsgBody, OUT pcp_login_t* pLogin);
int pcp_skt_build_reauth_req_msg(IN pcp_login_t* pLogin, OUT str_t* pMsg);
int pcp_skt_parse_reauth_rsp_msg(IN str_t* pMsgBody, pcp_login_t* pLogin);
int pcp_skt_build_noop_req_msg(OUT str_t* pMsg);
int pcp_skt_msg_compress(pcp_login_t* pLogin, MMPkg& pkg);
int pcp_skt_msg_decompress(pcp_login_t* pLogin, MMPkg& pkg) ;


///////////// CALL FUNCTIONS ////////////////
//call req, operation code 1
int pcp_skt_build_call_req_msg(IN pcp_call_req_t* pCallReq, OUT str_t* pBody);
int pcp_skt_parse_call_req_msg(IN str_t* pBody, OUT pcp_call_req_t* pCallReq);

// call rsp, operation code 2
int pcp_skt_build_call_rsp_msg(IN pcp_call_rsp_t* pCallRsp, OUT str_t* pBody);
int pcp_skt_parse_call_rsp_msg(IN str_t* pBody, OUT pcp_call_rsp_t* pCallRsp);

// call rsp ack, operation code 3
int pcp_skt_build_call_ack_req_msg(IN pcp_call_ack_t* pCallAck, OUT str_t* pBody);
int pcp_skt_parse_call_ack_req_msg(IN str_t* pBody, OUT pcp_call_ack_t* pCallAck);

// call bye, operation code 4
int pcp_skt_build_bye_req_msg(IN pcp_bye_req_t* pBye, OUT str_t* pBody);
int pcp_skt_parse_bye_req_msg(IN str_t* pBody, OUT pcp_bye_req_t* pBye);

// call media update req, operation code 5
int pcp_skt_build_update_req_msg(IN pcp_update_req_t* pUpdateReq, OUT str_t* pBody);
int pcp_skt_parse_update_req_msg(IN str_t* pBody, OUT pcp_update_req_t* pUpdateReq);

// call media update rsp, operation code 6
int pcp_skt_build_update_rsp_msg(IN pcp_update_rsp_t* pUpdateRsp, OUT str_t* pBody);
int pcp_skt_parse_update_rsp_msg(IN str_t* pBody, OUT pcp_update_rsp_t* pUpdateRsp);

// call ring rsp, operation code 7
int pcp_skt_build_ring_rsp_msg(IN pcp_ring_rsp_t* pRingRsp, OUT str_t* pBody);
int pcp_skt_parse_ring_rsp_msg(IN str_t* pBody, OUT pcp_ring_rsp_t* pRingRsp);

// call direct notify rsp, operation code 8
int pcp_skt_parse_direct_ntfy_msg(IN str_t* pBody, OUT pcp_direct_notify_t* pNotify);

// call route update req, operation code 15
int pcp_skt_build_route_update_msg(IN pcp_route_update_t* pRoute, OUT str_t* pBody);

// call dtmf info req, operation code 16
int pcp_skt_parse_dtmf_info_msg(IN str_t* pBody, OUT pcp_dtmf_info_t*pDtmfInfo);

// call internal state passthrough req, operation code 19
int pcp_skt_build_in_trans_req_msg(IN pcp_in_trans_req_t* pTransReq, OUT str_t* pBody);
int pcp_skt_parse_in_trans_req_msg(IN str_t* pBody, OUT pcp_in_trans_req_t* pTransReq);

// call internal state passthrough rsp, operation code 20
int pcp_skt_build_in_trans_rsp_msg(IN pcp_in_trans_rsp_t* pTransRsp, OUT str_t* pBody);
int pcp_skt_parse_in_trans_rsp_msg(IN str_t* pBody, OUT pcp_in_trans_rsp_t* pTransRsp);


//////////// AUXILIARY FUNCTIONS //////////////////////
// build ice string
int pcp_skt_build_ice_info(IN pcp_ice_info_t* pIceInfo, OUT str_t* pIceBuf);
int pcp_skt_parse_ice_info(IN str_t* pIceBuf, OUT pcp_ice_info_t* pIceInfo);

// build through string
int pcp_skt_build_through_info(IN pcp_through_info_t* pThrough, OUT str_t* pStr);
int pcp_skt_parse_through_info(IN str_t* pStr, OUT pcp_through_info_t* pThrough);

int pcp_skt_tlvsdp2struct(SDP_t* pTlvSdp, pcp_sdp_info_t* pSdpInfo);

int pcp_skt_compress(IN PcpSktCompress_t* pComParam, OUT str_t* pBuffer);
int pcp_skt_decompress(IN PcpSktCompress_t* pComParam, IN str_t* pBody, OUT str_t* pOutData);
#endif /* SKT_PROTOCOL_SUPPORT_ENABLED */
#endif /* PCP_SKT_PARSER_H */

