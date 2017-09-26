/******************************************************************************

  Copyright (C), 2001-2011, DCN Co., Ltd.

 ******************************************************************************
  File Name     : pcp_skt_parser.cpp
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
#if defined(SKT_PROTOCOL_SUPPORT_ENABLED)
#include "pcp_skt_parser.h"
#include <string>
#include "cfg.h"
#include "comm.h"
#include "mmpkg.h"
#include "mmcompress.h"
#include "mmsyncdef.h"
#include "Crypt.h"
#include "pcp_errcode.h"

using namespace Comm;

static int pcp_skt_check_valid_msg(pcp_uint32 msgLen)
{
    if ( msgLen < (PCP_SKT_MSG_HEADER_SIZE + PCP_SKT_VOIP_BODY_HEADER_SIZE))
    {
        ms_error("pcp_skt_check_valid_msg invalid msg. len(%d).", msgLen);
        return PCP_ERR;
    }

    return PCP_OK;
}

static void PrintStatus(unsigned int iStatus)
{
	if (iStatus & IGG_STATUS_EMAILVERIFY)
	{
		ms_report("IGG_STATUS_EMAILVERIFY.");
	}
	if (iStatus & IGG_STATUS_MOBILEVERIFY)
	{
		ms_report("IGG_STATUS_MOBILEVERIFY.");
	}
	if (iStatus & IGG_STATUS_PUSH_OPEN)
	{
		ms_report("IGG_STATUS_PUSH_OPEN.");
	}
	if (iStatus & IGG_STATUS_USERNAME)
	{
		ms_report("IGG_STATUS_USERNAME.");
	}
	if (iStatus & IGG_STATUS_VERIFY_USER)
	{
		ms_report("IGG_STATUS_VERIFY_USER.");
	}
}

// common function
#define CHECK_LEN() \
    if ( offset > strLen ) \
    { \
        ms_error("[%s %d] offset(%d) > strLen(%d)", \
        __FUNCTION__, __LINE__, offset, strLen); \
        return PCP_ERR; \
    }
    
int pcp_skt_build_msg(IN PcpSktMsg_t* pMsg, OUT str_t* pStr)
{
    pcp_int32 offset = 0;
    pcp_int32 temp = 0;
    pcp_uint16 usTemp = 0;
    pcp_uint16 u16Size = 0;
    pcp_uint16 u32Size = 0;
    pcp_uint16 u8Size = 0;
    pcp_uint16 strLen = 0;
    pcp_int8* pBuf = NULL;
    
    if ( NULL == pMsg || NULL == pStr )
    {
        ms_error("pcp_skt_build_msg failed. param is NULL.");
        return PCP_ERR;
    }

    if ( NULL == pMsg->msgHeader 
        || NULL == pMsg->bodyHeader )
    {
        ms_error("pcp_skt_build_msg failed. Msg param is NULL.");
        return PCP_ERR;
    }

    if ( NULL == pStr->sbuf || !pStr->slen )
    {
        ms_error("pcp_skt_build_msg failed. Out param is NULL.");
        return PCP_ERR;
    }

    u8Size = sizeof(pcp_uint8);
    u16Size = sizeof(pcp_uint16);
    u32Size = sizeof(pcp_uint32);
    
    pBuf = pStr->sbuf;    
    strLen = pStr->slen;
    
    /* Step 1 , build msg header */
    // msg len
    temp = htonl(pMsg->msgHeader->msgLen);
    memcpy(pBuf + offset, (pcp_int8 *)&temp, u32Size);
    offset += u32Size;
    CHECK_LEN();

    // header len
    usTemp = htons(pMsg->msgHeader->headerLen);
    memcpy(pBuf + offset, (pcp_int8 *)&usTemp, u16Size);
    offset += u16Size;
    CHECK_LEN();

    // protocol ver
    usTemp = htons(pMsg->msgHeader->version);
    memcpy(pBuf + offset, (pcp_int8 *)&usTemp, u16Size);
    offset += u16Size;
    CHECK_LEN();

    // command 
    temp = htonl(pMsg->msgHeader->cmd);
    memcpy(pBuf + offset, (pcp_int8 *)&temp, u32Size);
    offset += u32Size;
    CHECK_LEN();

    // sequence NO.
    temp = htonl(pMsg->msgHeader->seq);
    memcpy(pBuf + offset, (pcp_int8 *)&temp, u32Size);
    offset += u32Size;
    CHECK_LEN();
    
    /* Step 2,  build msg body header */
    // body header length
    memcpy(pBuf + offset, (pcp_int8 *)&(pMsg->bodyHeader->headlen), u8Size);
    offset += u8Size;
    CHECK_LEN();
    
    // body header version
    memcpy(pBuf + offset, (pcp_int8 *)&(pMsg->bodyHeader->ver), u8Size);
    offset += u8Size;
    CHECK_LEN();

    // operation code
    usTemp = htons(pMsg->bodyHeader->op);
    memcpy(pBuf + offset, (pcp_int8 *)&usTemp, u16Size);
    offset += u16Size;
    CHECK_LEN();

    // server id
    temp = htonl(pMsg->bodyHeader->servid);
    memcpy(pBuf + offset, (pcp_int8 *)&temp, u32Size);
    offset += u32Size;
    CHECK_LEN();

    // client id
    temp = htonl(pMsg->bodyHeader->clientid);
    memcpy(pBuf + offset, (pcp_int8 *)&temp, u32Size);
    offset += u32Size;
    CHECK_LEN();

    // account type
    memcpy(pBuf + offset, (pcp_int8 *)&(pMsg->bodyHeader->atype), u8Size);
    offset += u8Size;
    CHECK_LEN();

    //ack
    memcpy(pBuf + offset, (pcp_int8 *)&(pMsg->bodyHeader->ack), u8Size);
    offset += u8Size;
    CHECK_LEN();
    
    // enc 
    memcpy(pBuf + offset, (pcp_int8 *)&(pMsg->bodyHeader->enc), u8Size);
    offset += u8Size;
    CHECK_LEN();

    // Step 3, build msg body data
    if ( (offset + pMsg->dataLen) > strLen )
    { 
        ms_error("[%s %d] offset(%d) > strLen(%d)", 
                __FUNCTION__, __LINE__, offset, strLen);
        return PCP_ERR;
    }
    if ( pMsg->dataLen > 0 )
    {
        memcpy(pBuf + offset, (pcp_int8 *)pMsg->bodyData, pMsg->dataLen);
        offset += pMsg->dataLen;
    }

    pStr->slen = offset;

    return PCP_OK;
}

int pcp_skt_parse_msg_header(IN str_t* pMsg, OUT PcpSktMsgHeader_t* pMsgHeader, OUT str_t* pBody)
{
    pcp_int32 offset = 0;
    pcp_int32 temp = 0;
    pcp_uint16 usTemp = 0;
    pcp_int32 u16Size = 0;
    pcp_int32 u32Size = 0;
    pcp_int32 u8Size = 0;
    pcp_uint16 strLen = 0;
    pcp_int8 *pBuf = NULL;
    
    if ( NULL == pMsg || NULL == pMsg->sbuf || 0 == pMsg->slen )
    {
        ms_error("pcp_skt_parse_msg_header msg param is NULL.");
        return PCP_ERR;
    }

    if ( NULL == pMsgHeader )
    {
        ms_error("pcp_skt_parse_msg_header header param is NULL.");
        return PCP_ERR;
    }
    
    u8Size = sizeof(pcp_uint8);
    u16Size = sizeof(pcp_uint16);
    u32Size = sizeof(pcp_uint32);
    
    pBuf = pMsg->sbuf;
    strLen = pMsg->slen;

    // msg total len
    memcpy((pcp_int8 *)&temp, pBuf, u32Size);
    pMsgHeader->msgLen = ntohl(temp);
    offset += u32Size;
    CHECK_LEN();

    // msg header len
    memcpy((pcp_int8 *)&usTemp, pBuf + offset, u16Size);
    pMsgHeader->headerLen = ntohs(usTemp);
    offset += u16Size;
    CHECK_LEN();

    // protocol version
    memcpy((pcp_int8 *)&usTemp, pBuf + offset, u16Size);
    pMsgHeader->version = ntohs(usTemp);
    offset += u16Size;
    CHECK_LEN();

    //command 
    memcpy((pcp_int8 *)&temp, pBuf + offset, u32Size);
    pMsgHeader->cmd = ntohl(temp);
    offset += u32Size;
    CHECK_LEN();

    // sequence NO.
    memcpy((pcp_int8 *)&temp, pBuf + offset, u32Size);
    pMsgHeader->seq = ntohl(temp);
    offset += u32Size;
    CHECK_LEN();

    ms_report("pcp_skt_parse_msg_header: msgLen[%d] headLen[%d] ver[%x] cmd[%d]", \
        pMsgHeader->msgLen, pMsgHeader->headerLen, \
        pMsgHeader->version, pMsgHeader->cmd);
    
    // msg body
    if ( pBody != NULL )
    {
        pBody->slen = pMsgHeader->msgLen - offset;
        pBody->sbuf = pBuf + offset;
    }
    return offset;
}

int pcp_skt_parse_voip_body(IN str_t* pMsg, OUT PcpSktVoipBodyHeader_t* pBodyHeader, OUT str_t* pData)
{
    pcp_int32 offset = 0;
    pcp_int32 temp = 0;
    pcp_uint16 usTemp = 0;
    pcp_uint16 u16Size;
    pcp_uint16 u32Size;
    pcp_uint16 u8Size;
    pcp_uint16 strLen = 0;
    pcp_int8 *pBuf = NULL;
    
    if ( NULL == pMsg || NULL == pMsg->sbuf || 0 == pMsg->slen )
    {
        return PCP_ERR;
    }

    if ( NULL == pBodyHeader )
    {
        return PCP_ERR;
    }
    
    u8Size = sizeof(pcp_uint8);
    u16Size = sizeof(pcp_uint16);
    u32Size = sizeof(pcp_uint32);
    
    pBuf = pMsg->sbuf;
    strLen = pMsg->slen;

    // body header length
    memcpy((pcp_int8 *)&usTemp, pBuf + offset, u8Size);
    pBodyHeader->headlen = usTemp;
    offset += u8Size;
    CHECK_LEN();
    
    // body header version
    memcpy((pcp_int8 *)&usTemp, pBuf + offset, u8Size);
    pBodyHeader->ver = usTemp;
    offset += u8Size;
    CHECK_LEN();

    // operation code
    memcpy((pcp_int8 *)&usTemp, pBuf + offset, u16Size);
    pBodyHeader->op = ntohs(usTemp);
    offset += u16Size;
    CHECK_LEN();

    //server id
    memcpy((pcp_int8 *)&temp, pBuf + offset, u32Size);
    pBodyHeader->servid = ntohl(temp);
    offset += u32Size;
    CHECK_LEN();

    // client id
    memcpy((pcp_int8 *)&temp, pBuf + offset, u32Size);
    pBodyHeader->clientid = ntohl(temp);
    offset += u32Size;
    CHECK_LEN();

    //account type
    memcpy((pcp_int8 *)&usTemp, pBuf + offset, u8Size);
    pBodyHeader->atype = usTemp;
    offset += u8Size;
    CHECK_LEN();

    // ack
    memcpy((pcp_int8 *)&usTemp, pBuf + offset, u8Size);
    pBodyHeader->ack = usTemp;
    offset += u8Size;
    CHECK_LEN();

    // encrypt
    memcpy((pcp_int8 *)&usTemp, pBuf + offset, u8Size);
    pBodyHeader->enc = usTemp;
    offset += u8Size;
    CHECK_LEN();

    // get body data
    if ( pData != NULL )
    {
        pData->slen = pMsg->slen - offset;
        pData->sbuf = pBuf + offset;
    }

    return PCP_OK;
}

int pcp_skt_build_msg_header(PcpSktMsgHeader_t *pHeader,
                                    unsigned int cmd, unsigned int dataLen)
{
    if ( NULL == pHeader )
    {
        return PCP_ERR;
    }

    pHeader->version = SKT_PROTO_VER;
    pHeader->seq = cm_sceq();
    pHeader->cmd = cmd;
    pHeader->headerLen = PCP_SKT_MSG_HEADER_SIZE;
    pHeader->msgLen = PCP_SKT_VOIP_MSG_HEADER_SIZE + dataLen;

    return PCP_OK;
}

/////////// AUTH FUNCTIONS ////////////////
int pcp_skt_build_auth_req_msg(pcp_login_t* pLogin, str_t* pMsg)
{
    IGGAuthRequest_t bodyData;
    IGGBodyHead_t bodyHeader;
    PcpSktMsgHeader_t msgHeader;
    MMPkg pack;
    SKBuffer packBuf;
    BodyHead_t* head = NULL;
	pcp_int32 iRet = PCP_OK;
	pcp_int32 offset = 0;
    pcp_int32 temp = 0;
    pcp_uint16 usTemp = 0;
    pcp_uint16 u16Size = 0;
    pcp_uint16 u32Size = 0;
    pcp_uint16 u8Size = 0;
    pcp_uint16 strLen = 0;
    pcp_int8* pBuf = NULL;

    if ( NULL == pLogin || NULL == pMsg->sbuf )
    {
        ms_error("pcp_skt_build_auth_req_msg failed, param is NULL.");
        return PCP_ERR;
    }
    
    u8Size = sizeof(pcp_uint8);
    u16Size = sizeof(pcp_uint16);
    u32Size = sizeof(pcp_uint32);
    
    pBuf = pMsg->sbuf;    
    strLen = pMsg->slen;
    
    memset((void *)&bodyHeader, 0x00, sizeof(bodyHeader));
    memset((void *)&bodyData, 0x00, sizeof(bodyData));
    memset((void *)&msgHeader, 0x00, sizeof(msgHeader));

    // body header
    bodyHeader.Uin = 0;
    memcpy(bodyHeader.DeviceId, pLogin->devId, \
            sizeof(bodyHeader.DeviceId) - 1);
    bodyHeader.CmdId = PCP_AUTH_OP;
    bodyHeader.ServerId = NULL;
    bodyHeader.CompressAlgorithm = 0;
    bodyHeader.CryptAlgorithm = 0;

    // body data
    bodyData.tBaseRequest.iSeq = 0;
	memcpy(bodyData.tBaseRequest.cDeviceID, pLogin->devId, \
	        sizeof(bodyData.tBaseRequest.cDeviceID) - 1);
	memcpy(bodyData.tBaseRequest.sDeviceType, pLogin->devType, \
	        sizeof(bodyData.tBaseRequest.sDeviceType) - 1 );
	bodyData.tRandomEncryKey.pcBuff = strdup("1234567890abcdef");
	bodyData.tBaseRequest.iScene = 1;
	bodyData.tBaseRequest.iClientVersion = 1;
	bodyData.tLoginToken.pcBuff = strdup(pLogin->ssid);

	SKTLVPickle tlvPickle(&g_tMetaSipexMsg);
    iRet = tlvPickle.Struct2Buffer(eTypeAuthRequest, (void *)&bodyData, sizeof(bodyData), pack.GetBody());
    if ( iRet != 0 )
    {
        ms_error("pcp_skt_build_auth_req_msg failed(%d).", iRet);
        return PCP_ERR;
    }

    if ( pack.GetBody()->GetLen() > PCP_SKT_BODY_DATA_MAX_SIZE )
    {
        ms_error("pcp_skt_build_auth_req_msg failed. auth req msg too long.");
        return PCP_ERR;
    }
    
    head = pack.GetHead();
    memcpy(head, (BodyHead_t*)&bodyHeader, sizeof(BodyHead_t));
    //memcpy(pack.GetBody(), &authBuf, sizeof(SKBuffer)); 
    
    // compress and encrypt
    pLogin->compressEn = 0;
    pLogin->desEn = 0; // only auth req use RSA encrypt
    if ( pcp_skt_msg_compress(pLogin, pack) < 0 )
    {
        ms_error("pcp_skt_build_auth_req_msg compress failed.");
        return PCP_ERR;
    }
	pack.Pack(&packBuf);

    /*  build msg header */
    msgHeader.cmd = PCP_SKT_MSG_CMD_AUTH_REQ;
    msgHeader.seq = cm_sceq();
    msgHeader.version = SKT_PROTO_VER;
    msgHeader.headerLen = PCP_SKT_MSG_HEADER_SIZE;
    msgHeader.msgLen = PCP_SKT_MSG_HEADER_SIZE + packBuf.GetLen();
        
    // msg len
    temp = htonl(msgHeader.msgLen);
    memcpy(pBuf + offset, (pcp_int8 *)&temp, u32Size);
    offset += u32Size;
    CHECK_LEN();

    // header len
    usTemp = htons(msgHeader.headerLen);
    memcpy(pBuf + offset, (pcp_int8 *)&usTemp, u16Size);
    offset += u16Size;
    CHECK_LEN();

    // protocol ver
    usTemp = htons(msgHeader.version);
    memcpy(pBuf + offset, (pcp_int8 *)&usTemp, u16Size);
    offset += u16Size;
    CHECK_LEN();

    // command 
    temp = htonl(msgHeader.cmd);
    memcpy(pBuf + offset, (pcp_int8 *)&temp, u32Size);
    offset += u32Size;
    CHECK_LEN();

    // sequence NO.
    temp = htonl(msgHeader.seq);
    memcpy(pBuf + offset, (pcp_int8 *)&temp, u32Size);
    offset += u32Size;
    CHECK_LEN();

    // msg body
    memcpy(pBuf + offset, (pcp_int8 *)packBuf.GetBuffer(), packBuf.GetLen());
    offset += packBuf.GetLen();
    CHECK_LEN();

    pMsg->slen = offset;
    return PCP_OK;
}

int pcp_skt_parse_auth_rsp_msg(str_t* pMsgBody, pcp_login_t* pLogin)
{
    IGGAuthResponse_t authResp;
    IGGBodyHead_t bodyHeader;
    SKBuffer respBuf;
    MMPkg pkg;
    BodyHead_t* head = NULL;
    pcp_int8 *pBuf = NULL;
    pcp_int32 iRet = PCP_OK;
    
    if ( NULL == pMsgBody || NULL == pMsgBody->sbuf 
        || 0 == pMsgBody->slen || NULL == pLogin )
    {
        return PCP_ERR;
    }
    
    pBuf = pMsgBody->sbuf;

    memset((void*)&authResp, 0x00, sizeof(IGGAuthResponse_t));
    
	respBuf.Write(pBuf, pMsgBody->slen);
	
	pkg.Unpack(&respBuf);
	// decompress and decrypt
	pLogin->compressEn = 0;
	pLogin->desEn = 1;
	if ( pcp_skt_msg_decompress(pLogin, pkg) < 0)
	{
	    ms_error("pcp_skt_parse_auth_rsp_msg failed at decompress.");
	    return PCP_ERR;
	}
	
	head = pkg.GetHead();
	memcpy(&bodyHeader, (IGGBodyHead_t *)head, sizeof(IGGBodyHead_t));

	SKTLVPickle tlvPickle(&g_tMetaSipexMsg);

    iRet = tlvPickle.Buffer2Struct(eTypeAuthResponse, pkg.GetBody(), &authResp, sizeof(authResp));
	if(iRet != 0)
	{
		ms_error("pcp_skt_parse_auth_rsp_msg failed at Buffer2Struct(%d).", iRet);
		return PCP_ERR;
	}

    ms_report("Auth response recv. iRet = %d .", authResp.tBaseResponse.iRet);
	if (authResp.tBaseResponse.iRet == 0)
	{
	    pLogin->uin = authResp.iUin;
	    memcpy(pLogin->sessionKey, authResp.sSessionKey, \
	            sizeof(pLogin->sessionKey) - 1);
	    
		ms_report("Uin: %u",authResp.iUin);		
		ms_report("Seq %u", authResp.tBaseResponse.iSeq);
		ms_report("Sessionkey %s", authResp.sSessionKey);		
		ms_report("UserName %s", authResp.tUserName.pcBuff);
		ms_report("SafeUserName %s", authResp.tSafeUserName.pcBuff);
		ms_report("NickName %s", authResp.tNickName.pcBuff);
		ms_report("FirstName %s", authResp.tFirstName.pcBuff);
		ms_report("LastName %s", authResp.tLastName.pcBuff);
		ms_report("BindEmail %s", authResp.tBindEmail.pcBuff);
		ms_report("BindMobile %s", authResp.tBindMobile.pcBuff);
		ms_report("RegType %u", authResp.iRegType);
		ms_report("IpCount %d", authResp.tWebProxyInfo.iIPCount);
		if ( NULL != authResp.tClientNumber.pcBuff )
		{
		    ms_report("ClientNumber %s", authResp.tClientNumber.pcBuff);
		    strncpy(pLogin->uid, authResp.tClientNumber.pcBuff, sizeof(pLogin->uid)-1);
		}
		for ( int i=0; i<authResp.tWebProxyInfo.iIPCount; i++ )
		{
			ms_report("IP[%d]=%s", i, authResp.tWebProxyInfo.ptIPList[i].pcBuff);
		}
	
		ms_report("BirthDay %04d-%02d-%02d", authResp.tBirthday.iYear, authResp.tBirthday.iMonth, authResp.tBirthday.iDay);
		ms_report("UserSex %d", authResp.iUserSex);
	
		ms_report("UserStatus %d", authResp.iUserStatus);
	
		//g_sekey = resp.sSessionKey;
	
		PrintStatus(authResp.iStatus);
	}

	SKAllocator Allocator(&g_tMetaSipexMsg);
	iRet = Allocator.FreeField(eTypeAuthResponse, &authResp, sizeof(authResp));
	if(iRet != 0)
	{
	    ms_error("pcp_skt_parse_auth_rsp_msg failed at memory free(%d).", iRet);
	}
    return PCP_OK;
}

int pcp_skt_build_reauth_req_msg(pcp_login_t* pLogin, str_t* pMsg)
{
    IGGAuthBySKRequest_t bodyData;
    IGGBodyHead_t bodyHeader;
    PcpSktMsgHeader_t msgHeader;
    MMPkg pack;
    BodyHead_t* head = NULL;
    SKBuffer packBuf;
	pcp_int32 iRet = PCP_OK;
	pcp_int32 offset = 0;
    pcp_int32 temp = 0;
    pcp_uint16 usTemp = 0;
    pcp_uint16 u16Size = 0;
    pcp_uint16 u32Size = 0;
    pcp_uint16 u8Size = 0;
    pcp_uint16 strLen = 0;
    pcp_int8* pBuf = NULL;

    if ( NULL == pLogin || NULL == pMsg->sbuf )
    {
        ms_error("pcp_skt_build_reauth_req_msg failed, param is NULL.");
        return PCP_ERR;
    }
    
    u8Size = sizeof(pcp_uint8);
    u16Size = sizeof(pcp_uint16);
    u32Size = sizeof(pcp_uint32);
    
    pBuf = pMsg->sbuf;    
    strLen = pMsg->slen;
    
    memset((void *)&bodyHeader, 0x00, sizeof(bodyHeader));
    memset((void *)&bodyData, 0x00, sizeof(bodyData));
    memset((void *)&msgHeader, 0x00, sizeof(msgHeader));

    // body header
    bodyHeader.Uin = 0;
    memcpy(bodyHeader.DeviceId, pLogin->devId, \
            sizeof(bodyHeader.DeviceId) - 1);
    bodyHeader.CmdId = PCP_REAUTH_OP;
    bodyHeader.ServerId = NULL;
    bodyHeader.CompressAlgorithm = 0;
    bodyHeader.CryptAlgorithm = 0;

    // body data
    bodyData.tBaseRequest.iSeq = 0;
	memcpy(bodyData.tBaseRequest.cDeviceID, pLogin->devId, \
	        sizeof(bodyData.tBaseRequest.cDeviceID) - 1);
	memcpy(bodyData.tBaseRequest.sDeviceType, pLogin->devType, \
	        sizeof(bodyData.tBaseRequest.sDeviceType) - 1 );
	memcpy(bodyData.tBaseRequest.sSessionKey, pLogin->sessionKey, \
	        sizeof(bodyData.tBaseRequest.sSessionKey) - 1);
	bodyData.tBaseRequest.iScene = 15; //MM_SCENE_REAUTH
	bodyData.tBaseRequest.iClientVersion = 1;

	SKTLVPickle tlvPickle(&g_tMetaSipexMsg);
    iRet = tlvPickle.Struct2Buffer(eTypeAuthBySKRequest, (void *)&bodyData, sizeof(bodyData), pack.GetBody());
    if ( iRet != 0 )
    {
        ms_error("pcp_skt_build_reauth_req_msg failed(%d).", iRet);
        return PCP_ERR;
    }

    if ( pack.GetBody()->GetLen() > PCP_SKT_BODY_DATA_MAX_SIZE )
    {
        ms_error("pcp_skt_build_reauth_req_msg failed. reauth req msg too long.");
        return PCP_ERR;
    }

    head = pack.GetHead();
    memcpy(head, (BodyHead_t*)&bodyHeader, sizeof(BodyHead_t));
    //memcpy(pack.GetBody(), &reAuthBuf, sizeof(reAuthBuf));
    
    // compress and encrypt
    pLogin->compressEn = 0;
    pLogin->desEn = 1; // only auth req use RSA encrypt
    if ( pcp_skt_msg_compress(pLogin, pack) < 0 )
    {
        ms_error("pcp_skt_build_reauth_req_msg compress failed.");
        return PCP_ERR;
    }
	pack.Pack(&packBuf);

    /*  build msg header */
    msgHeader.cmd = PCP_SKT_MSG_CMD_REAUTH_REQ;
    msgHeader.seq = cm_sceq();
    msgHeader.version = SKT_PROTO_VER;
    msgHeader.headerLen = PCP_SKT_MSG_HEADER_SIZE;
    msgHeader.msgLen = PCP_SKT_MSG_HEADER_SIZE + packBuf.GetLen();
        
    // msg len
    temp = htonl(msgHeader.msgLen);
    memcpy(pBuf + offset, (pcp_int8 *)&temp, u32Size);
    offset += u32Size;
    CHECK_LEN();

    // header len
    usTemp = htons(msgHeader.headerLen);
    memcpy(pBuf + offset, (pcp_int8 *)&usTemp, u16Size);
    offset += u16Size;
    CHECK_LEN();

    // protocol ver
    usTemp = htons(msgHeader.version);
    memcpy(pBuf + offset, (pcp_int8 *)&usTemp, u16Size);
    offset += u16Size;
    CHECK_LEN();

    // command 
    temp = htonl(msgHeader.cmd);
    memcpy(pBuf + offset, (pcp_int8 *)&temp, u32Size);
    offset += u32Size;
    CHECK_LEN();

    // sequence NO.
    temp = htonl(msgHeader.seq);
    memcpy(pBuf + offset, (pcp_int8 *)&temp, u32Size);
    offset += u32Size;
    CHECK_LEN();

    // msg body
    memcpy(pBuf + offset, (pcp_int8 *)packBuf.GetBuffer(), packBuf.GetLen());
    offset += packBuf.GetLen();
    CHECK_LEN();

    pMsg->slen = offset;
    return PCP_OK;
}

int pcp_skt_parse_reauth_rsp_msg(str_t* pMsgBody, pcp_login_t* pLogin)
{
    IGGAuthBySKResponse_t reAuthResp;
    IGGBodyHead_t bodyHeader;
    pcp_int8 *pBuf = NULL;
    pcp_int32 iRet = PCP_OK;
    
    if ( NULL == pMsgBody || NULL == pMsgBody->sbuf 
        || 0 == pMsgBody->slen || NULL == pLogin )
    {
        return PCP_ERR;
    }    
    
    pBuf = pMsgBody->sbuf;
    memset((void*)&reAuthResp, 0x00, sizeof(IGGAuthBySKResponse_t));
    SKBuffer respBuf;
	respBuf.Write(pBuf, pMsgBody->slen);

	MMPkg pkg;
	pkg.Unpack(&respBuf);
	// decompress and decrypt
	pLogin->compressEn = 0;
	pLogin->desEn = 1;
	if ( pcp_skt_msg_decompress(pLogin, pkg) < 0)
	{
	    ms_error("pcp_skt_parse_reauth_rsp_msg failed at decompress.");
	    return PCP_ERR;
	}
	
	BodyHead_t* head = pkg.GetHead();
	memcpy(&bodyHeader, (IGGBodyHead_t *)head, sizeof(IGGBodyHead_t));

	SKTLVPickle tlvPickle(&g_tMetaSipexMsg);

    iRet = tlvPickle.Buffer2Struct(eTypeAuthBySKResponse, pkg.GetBody(), &reAuthResp, sizeof(reAuthResp));
	if(iRet != 0)
	{
		ms_error("pcp_skt_parse_reauth_rsp_msg failed at Buffer2Struct(%d).", iRet);
		return PCP_ERR;
	}

	ms_report("RetCode %d", reAuthResp.tBaseResponse.iRet);
	ms_report("Uin: %u",reAuthResp.iUin);
	ms_report("RespMsg %s", reAuthResp.tBaseResponse.tErrMsg.pcBuff);

	SKAllocator Allocator(&g_tMetaSipexMsg);
	iRet = Allocator.FreeField(eTypeAuthBySKResponse, &reAuthResp, sizeof(reAuthResp));
	if(iRet != 0)
	{
	    ms_error("pcp_skt_parse_reauth_rsp_msg failed at memory free(%d).", iRet);
	}
    return PCP_OK;
}

int pcp_skt_build_noop_req_msg(OUT str_t* pMsg)
{
    PcpSktMsgHeader_t msgHeader;
	pcp_int32 offset = 0;
    pcp_int32 temp = 0;
    pcp_uint16 usTemp = 0;
    pcp_uint16 u16Size = 0;
    pcp_uint16 u32Size = 0;
    pcp_uint16 strLen = 0;
    pcp_int8* pBuf = NULL;
    
    if ( NULL == pMsg->sbuf )
    {
        ms_error("pcp_skt_build_noop_req_msg failed, param is NULL.");
        return PCP_ERR;
    }
    
    u16Size = sizeof(pcp_uint16);
    u32Size = sizeof(pcp_uint32);
    
    pBuf = pMsg->sbuf;    
    strLen = pMsg->slen;
    
    memset((void *)&msgHeader, 0x00, sizeof(msgHeader));

    /*  build msg header */
    msgHeader.cmd = PCP_SKT_MSG_CMD_NOOP_REQ;
    msgHeader.seq = cm_sceq();
    msgHeader.version = SKT_PROTO_VER;
    msgHeader.headerLen = PCP_SKT_MSG_HEADER_SIZE;
    msgHeader.msgLen = PCP_SKT_MSG_HEADER_SIZE;
        
    // msg len
    temp = htonl(msgHeader.msgLen);
    memcpy(pBuf + offset, (pcp_int8 *)&temp, u32Size);
    offset += u32Size;
    CHECK_LEN();

    // header len
    usTemp = htons(msgHeader.headerLen);
    memcpy(pBuf + offset, (pcp_int8 *)&usTemp, u16Size);
    offset += u16Size;
    CHECK_LEN();

    // protocol ver
    usTemp = htons(msgHeader.version);
    memcpy(pBuf + offset, (pcp_int8 *)&usTemp, u16Size);
    offset += u16Size;
    CHECK_LEN();

    // command 
    temp = htonl(msgHeader.cmd);
    memcpy(pBuf + offset, (pcp_int8 *)&temp, u32Size);
    offset += u32Size;
    CHECK_LEN();

    // sequence NO.
    temp = htonl(msgHeader.seq);
    memcpy(pBuf + offset, (pcp_int8 *)&temp, u32Size);
    offset += u32Size;
    CHECK_LEN();

    pMsg->slen = offset;

    return PCP_OK;
}

/**
* Ñ¹Ëõ²¢¼ÓÃÜ
*/
int pcp_skt_msg_compress(pcp_login_t* pLogin, MMPkg& pkg) 
{
	BodyHead_t* head = pkg.GetHead();
	SKBuffer* body = pkg.GetBody();
	pcp_int32 ret = 0;

	if ( NULL == pLogin )
	{
	    ms_report("pcp_skt_msg_compress: param NULL.");
	    return PCP_ERR;
	}
	
	if ( pLogin->compressEn )
	{
		//ÉèÖÃÑ¹ËõËã·¨
		head->CompressVersion = MMCOMPRESS_PACK_VERSION_1;
		head->CompressAlgorithm = MicroMsg::MMCPR_ZLIB_COMPRESS;
		head->CompressLen = body->GetLen();

		//Ñ¹Ëõ
		RBBuffer comBuf;
		ret = MicroMsg::Compress((unsigned char*)body->GetBuffer(), body->GetLen(),
		                    &comBuf, head->CompressAlgorithm, head->CompressVersion);
		if (ret != 0) {
			ms_error("pcp_skt_msg_compress: ERR: compress failed: ret(%d)", ret);
			return PCP_ERR;
		}
		
		head->CompressedLen = comBuf.GetLen();
		ms_report("pcp_skt_msg_compress: body compressed");

		body->Reset();
		body->Write(comBuf.GetReadPtr(), comBuf.GetLen());
	}

#if defined(TLV_ENC_ENABLED)	
	if ( pLogin->encryptEn )
	{
		//¼ÓÃÜ
		RBBuffer ecOut;
		if ( !pLogin->desEn)
		{
			head->CryptAlgorithm = RSA_ENCRYPT_WITH_PUBLICKEY;
			ret = RSAEncrypt((unsigned char*)body->GetBuffer(), body->GetLen(), ecOut,
				RSA_PUBLIC_KEYN, RSA_PUBLIC_KEYE);
			if (ret != 0) {
				ms_error("pcp_skt_msg_compress: ERR: RSA encrypt failed: %d", ret);
				return PCP_ERR;
			}

			body->Reset();
			body->Write(ecOut.GetReadPtr(), ecOut.GetLen());
			ms_report("pcp_skt_msg_compress: body encrypted");
		}
		else
		{
			head->CryptAlgorithm = DES_ENCRYPT_WITH_PUBLICKEY;
			ret = DESEncrypt((unsigned char*)body->GetBuffer(), body->GetLen(), ecOut,
				pLogin->sessionKey, strlen(pLogin->sessionKey));
			if (ret != 0) 
			{
				ms_error("pcp_skt_msg_compress: ERR: DES encrypt failed: %d", ret);
				return PCP_ERR;
			}

			body->Reset();
			body->Write(ecOut.GetReadPtr(), ecOut.GetLen());
			ms_report("pcp_skt_msg_compress: body encrypted");
		}
	}
#endif

	return PCP_OK;
}

/**
* ½âÃÜ²¢½âÑ¹Ëõ
*/
int pcp_skt_msg_decompress(pcp_login_t* pLogin, MMPkg& pkg) 
{
	BodyHead_t* head = pkg.GetHead();
	SKBuffer* body = pkg.GetBody();
	pcp_int8 desKey[64] = {0};
    int ret = 0;

    if ( NULL == pLogin )
	{
	    return PCP_ERR;
	}
	
	ms_report("HeadRet: %d", head->Ret);

	if (  head->Ret != 0 )
	{
		return PCP_ERR;
	}

#if defined(TLV_ENC_ENABLED)
	strcpy(desKey, "1234567890abcdef");
	if ( head->CryptAlgorithm != 0 )
	{
		RBBuffer dcOut;
		//½âÃÜ
		if ( !pLogin->desEn )
		{
			ret = DESDecrypt((unsigned char*)body->GetBuffer(), body->GetLen(),
				            dcOut, desKey, strlen(desKey));
			if (ret != 0) 
			{
				ms_error("DESDecrypt by random key [%s]. ret(%d)", desKey, ret);
				return PCP_ERR;
			}
			else
			{
				ms_error("DESDecrypt by random key [%s] success.", desKey);
			}
		}
		else
		{
			ret = DESDecrypt((unsigned char*)body->GetBuffer(), body->GetLen(),
				dcOut, pLogin->sessionKey, strlen(pLogin->sessionKey));
			if (ret != 0) 
			{
				ms_error("DESDecrypt by session key [%s]. ret(%d)", pLogin->sessionKey, ret);
				return PCP_ERR;
			}
		}

		body->Reset();
		body->Write(dcOut.GetReadPtr(), dcOut.GetLen());
		
		ms_report("DESDecrypt OK!!!");
	}
#endif

	if (MicroMsg::MMCPR_NO_COMPRESS == head->CompressAlgorithm)
	{
		return PCP_OK;
	}
	
	//½âÑ¹Ëõ
	RBBuffer decomBuf;
	ret = MicroMsg::Decompress((unsigned char*)body->GetBuffer(), body->GetLen(),&decomBuf,
            		head->CompressLen, head->CompressAlgorithm, head->CompressVersion);
	
	if (ret != 0)
	{
		ms_error("decompress failed: %d", ret);
		return PCP_ERR;
	}

	ms_report("Decompress OK!!!");

	body->Reset();
	body->Write(decomBuf.GetReadPtr(), decomBuf.GetLen());

	return PCP_OK;
}

///////////// CALL FUNCTIONS   ////////////////
//call req, operation code 1
int pcp_skt_build_call_req_msg(IN pcp_call_req_t* pCallReq, OUT str_t* pBody)
{
    SKBuffer callBuf;
    CallReq_t callReq;
    pcp_uint16 mediaNum = 0;
    pcp_uint16 rtppNum = 0;
    Media_t    aMedia[PCP_MAX_MEDIA_NUM];
    Rtpp_t     aRtpp[PCP_MAX_RTPP];
    SDP_t*     pSDP = NULL;
    pcp_sdp_info_t *pReqSdp = NULL;
    str_t    iceBuf;
    str_t    throughBuf;
    pcp_int32    iRet = 0;
    
    if ( NULL == pCallReq || NULL == pBody )
    {
        return PCP_ERR;
    }
    
    memset((void *)&callReq, 0x00, sizeof(CallReq_t));
    
    callReq.type = pCallReq->callmode;
    callReq.ver.pcBuff = pCallReq->ver;
    callReq.callid.pcBuff = pCallReq->call_id;
    callReq.fnickname.pcBuff = pCallReq->fnickname;
    callReq.fuserid.pcBuff = pCallReq->fuserid;
    callReq.fuid.pcBuff = pCallReq->fuid;
    callReq.fphone.pcBuff = pCallReq->fphone;
    callReq.touserid.pcBuff = pCallReq->tuserid;
    callReq.tophone.pcBuff = pCallReq->tphone;
    callReq.sflag = pCallReq->sflag;
    callReq.meeting = pCallReq->meetingflag;
    callReq.userdata.pcBuff = pCallReq->user_data;

    //sdp
    pSDP = ((SDP_t *)&callReq.callsvr_sdp);
    pReqSdp = pCallReq->sdp;
    if ( NULL != pReqSdp )
    {
        // connect ip
        pSDP->cConIp.pcBuff = pReqSdp->c_ip;
        // version
        pSDP->Ver.pcBuff = pReqSdp->v_version;
        // media list
        mediaNum = MIN(pReqSdp->media.media_num, PCP_MAX_MEDIA_NUM);
        if ( mediaNum > 0 )
        {
            for ( int i = 0; i < mediaNum; i++ )
            {
                aMedia[i].cType = pReqSdp->media.media_arr[i].type[0];
                aMedia[i].iPort = pReqSdp->media.media_arr[i].port;
                aMedia[i].iPr = pReqSdp->media.media_arr[i].pr;
                aMedia[i].iPt = pReqSdp->media.media_arr[i].pt;
#if 0
                ms_report("pcp_skt_build_call_req_msg: type[%c] pr[%d] pt[%d] port[%d]", \
                    aMedia[i].cType, aMedia[i].iPr, aMedia[i].iPt, aMedia[i].iPort);
#endif                
            }
        }
        pSDP->tMedialist.iCount = mediaNum;
        pSDP->tMedialist.ptList = aMedia;

        // rtpp list
        rtppNum = MIN(pReqSdp->rtpp.rtpp_num, PCP_MAX_RTPP);
        if ( rtppNum > 0 )
        {
            for ( int i = 0; i < rtppNum; i++ )
            {
                aRtpp[i].cIp.pcBuff = pReqSdp->rtpp.rtpp_arr[i].ip;
                aRtpp[i].iDelay = pReqSdp->rtpp.rtpp_arr[i].delay;
                aRtpp[i].iLost = pReqSdp->rtpp.rtpp_arr[i].lost;
            }
        }
        pSDP->tRtpplist.iCount = rtppNum;
        pSDP->tRtpplist.ptList = aRtpp;

        // ice info
        str_init(&iceBuf, MAX_JSON_BODY_SIZE);
        if (PCP_OK == pcp_skt_build_ice_info(&(pReqSdp->ice), &iceBuf))
        {
            pSDP->cIce.iLen = iceBuf.slen;
            pSDP->cIce.pcBuff = iceBuf.sbuf;
        }

        // support video
        pSDP->support_v = pReqSdp->support_v;
        
        // through
        str_init(&throughBuf, MAX_JSON_BODY_SIZE);
        if ( pReqSdp->through.video_enabled )
        {
            if (PCP_OK == pcp_skt_build_through_info(&(pReqSdp->through), &throughBuf))
            {
                pSDP->through.pcBuff = throughBuf.sbuf;
            }
        }
    }

    SKTLVPickle tlvPickle(&g_tMetaSipexMsg);
    iRet = tlvPickle.Struct2Buffer(eTypeCallReq, (void *)&callReq, sizeof(callReq), &callBuf);
    if ( iRet != 0 )
    {
        ms_error("pcp_skt_build_call_req_msg failed(%d).", iRet);
        str_release(&iceBuf);
        str_release(&throughBuf);
        return PCP_ERR;
    }

    if ( callBuf.GetLen() > PCP_SKT_BODY_DATA_MAX_SIZE )
    {
        ms_error("pcp_skt_build_call_req_msg failed. call req msg too long(bufLen[%d] > bodyLen[%d].", \
			callBuf.GetLen(), PCP_SKT_BODY_DATA_MAX_SIZE);
        str_release(&iceBuf);
        str_release(&throughBuf);
        return PCP_ERR;
    }
	ms_report("pcp_skt_build_call_req_msg: bodyBuf len[%d]", callBuf.GetLen());
    memcpy(pBody->sbuf, callBuf.GetBuffer(), callBuf.GetLen());
    pBody->slen = callBuf.GetLen();

    str_release(&iceBuf);
    str_release(&throughBuf);
    
    return PCP_OK;
}

int pcp_skt_parse_call_req_msg(IN str_t* pBody, OUT pcp_call_req_t* pCallReq)
{
    SKBuffer callBuf;
    CallReq_t callReq;
    pcp_int32 iRet = PCP_OK;
    
    if ( NULL == pCallReq || NULL == pBody || NULL == pBody->sbuf )
    {
        return PCP_ERR;
    }

    memset((void *)&callReq, 0x00, sizeof(CallReq_t));
    
    callBuf.Write(pBody->sbuf, pBody->slen);
    SKTLVPickle tlvPickle(&g_tMetaSipexMsg);

    iRet = tlvPickle.Buffer2Struct(eTypeCallReq, &callBuf, &callReq, sizeof(callReq));
	if(iRet != 0)
	{
		ms_error("pcp_skt_parse_call_req_msg failed at Buffer2Struct(%d).", iRet);
		return PCP_ERR;
	}

    pCallReq->callmode = callReq.type;    
    pCallReq->meetingflag = callReq.meeting;
    pCallReq->sflag = callReq.sflag;
    if ( NULL != callReq.callid.pcBuff )
    {
        strncpy(pCallReq->call_id, callReq.callid.pcBuff, sizeof(pCallReq->call_id) - 1);
    }
    if ( NULL != callReq.fnickname.pcBuff )
    {
        strncpy(pCallReq->fnickname, callReq.fnickname.pcBuff, sizeof(pCallReq->fnickname) - 1);
    }

    if ( NULL != callReq.fuid.pcBuff )
    {
        strncpy(pCallReq->fuid, callReq.fuid.pcBuff, sizeof(pCallReq->fuid) - 1);
    }
    if ( NULL != callReq.fuserid.pcBuff )
    {
        strncpy(pCallReq->fuserid, callReq.fuserid.pcBuff, sizeof(pCallReq->fuserid) - 1);
    }
    if ( NULL != callReq.fphone.pcBuff )
    {
        strncpy(pCallReq->fphone, callReq.fphone.pcBuff, sizeof(pCallReq->fphone) - 1);
    }
    if ( NULL != callReq.touserid.pcBuff )
    {
        strncpy(pCallReq->tuserid, callReq.touserid.pcBuff, sizeof(pCallReq->tuserid) - 1);
    }
    if ( NULL != callReq.touserid.pcBuff )
    {
        strncpy(pCallReq->tuid, callReq.touserid.pcBuff, sizeof(pCallReq->tuid) - 1);
    }
    if ( NULL != callReq.tophone.pcBuff )
    {
        strncpy(pCallReq->tphone, callReq.tophone.pcBuff, sizeof(pCallReq->tphone) - 1);
    }
    if ( NULL != callReq.ver.pcBuff )
    {
        strncpy(pCallReq->ver, callReq.ver.pcBuff, sizeof(pCallReq->ver) - 1);
    }
    if ( NULL != callReq.userdata.pcBuff )
    {
        strncpy(pCallReq->user_data, callReq.userdata.pcBuff, sizeof(pCallReq->user_data) - 1);
    }
    
    pcp_skt_tlvsdp2struct(&(callReq.callsvr_sdp), pCallReq->sdp);

    SKAllocator Allocator(&g_tMetaSipexMsg);
	iRet = Allocator.FreeField(eTypeCallReq, &callReq, sizeof(callReq));
	if(iRet != 0)
	{
	    ms_error("pcp_skt_parse_call_req_msg failed at memory free(%d).", iRet);
	}
	
    return PCP_OK;
}

// call rsp, operation code 2
int pcp_skt_build_call_rsp_msg(IN pcp_call_rsp_t* pCallRsp, OUT str_t* pBody)
{
    SKBuffer callBuf;
    CallRsp_t callRsp;
    pcp_uint16 mediaNum = 0;
    pcp_uint16 rtppNum = 0;
    Media_t    aMedia[PCP_MAX_MEDIA_NUM];
    Rtpp_t     aRtpp[PCP_MAX_RTPP];
    SDP_t*     pSDP = NULL;
    pcp_sdp_info_t *pRspSdp = NULL;
    str_t    iceBuf;
    str_t    throughBuf;
    pcp_int32    mediaIndex = 0;
    pcp_int32    iRet = 0;
    
    if ( NULL == pCallRsp || NULL == pBody )
    {
        return PCP_ERR;
    }

    memset(&callRsp, 0x00, sizeof(CallRsp_t));

    callRsp.sflag = pCallRsp->sflag;
    callRsp.errcode = pCallRsp->reason;
    callRsp.ver.pcBuff = pCallRsp->ver;
    callRsp.callid.pcBuff = pCallRsp->call_id;

    // sdp
    str_init(&iceBuf, MAX_JSON_BODY_SIZE);
    str_init(&throughBuf, MAX_JSON_BODY_SIZE);
    if ( callRsp.errcode == PCP_SUCCESS )
    {
        pSDP = ((SDP_t *)&callRsp.callsvr_sdp);
        pRspSdp = pCallRsp->sdp;
        if ( NULL != pRspSdp )
        {
            // connect ip
            pSDP->cConIp.pcBuff = pRspSdp->c_ip;
            // version
            pSDP->Ver.pcBuff = pRspSdp->v_version;
            
            // media list
            mediaNum = MIN(pRspSdp->media.media_num, PCP_MAX_MEDIA_NUM);
            if ( mediaNum > 0 )
            {
                // the 1st priority codec
                for ( int i = 0; i < mediaNum; i++ )
                {
                    if (check_media_available(&pRspSdp->media.media_arr[i], 
                                    pCallRsp->baudio_pt, pCallRsp->bvideo_pt))
                    {
                        aMedia[mediaIndex].cType = pRspSdp->media.media_arr[i].type[0];
                        aMedia[mediaIndex].iPort = pRspSdp->media.media_arr[i].port;
                        aMedia[mediaIndex].iPr = pRspSdp->media.media_arr[i].pr;
                        aMedia[mediaIndex].iPt = pRspSdp->media.media_arr[i].pt;
                        mediaIndex++;
                    }
                }

                // other priority codecs
                for ( int i = 0; i < mediaNum; i++ )
                {
                    if (!check_media_available(&pRspSdp->media.media_arr[i], 
                                    pCallRsp->baudio_pt, pCallRsp->bvideo_pt))
                    {
                        aMedia[mediaIndex].cType = pRspSdp->media.media_arr[i].type[0];
                        aMedia[mediaIndex].iPort = pRspSdp->media.media_arr[i].port;
                        aMedia[mediaIndex].iPr = pRspSdp->media.media_arr[i].pr;
                        aMedia[mediaIndex].iPt = pRspSdp->media.media_arr[i].pt;
                        mediaIndex++;
                    }
                }
            }
#if 0
            for ( int i = 0; i < mediaNum; i++ )
            {
                ms_report("pcp_skt_build_call_rsp_msg: type[%c] pr[%d] pt[%d] port[%d]", \
                    aMedia[i].cType, aMedia[i].iPr, aMedia[i].iPt, aMedia[i].iPort);
            }
#endif
            pSDP->tMedialist.iCount = mediaNum;
            pSDP->tMedialist.ptList = aMedia;

            // rtpp list
            rtppNum = MIN(pRspSdp->rtpp.rtpp_num, PCP_MAX_RTPP);
            if ( rtppNum > 0 )
            {
                for ( int i = 0; i < rtppNum; i++ )
                {
                    aRtpp[i].cIp.pcBuff = pRspSdp->rtpp.rtpp_arr[i].ip;
                    aRtpp[i].iDelay = pRspSdp->rtpp.rtpp_arr[i].delay;
                    aRtpp[i].iLost = pRspSdp->rtpp.rtpp_arr[i].lost;
                }
            }
            pSDP->tRtpplist.iCount = rtppNum;
            pSDP->tRtpplist.ptList = aRtpp;

            // ice info            
            if (PCP_OK == pcp_skt_build_ice_info(&(pRspSdp->ice), &iceBuf))
            {
                pSDP->cIce.iLen = iceBuf.slen;
                pSDP->cIce.pcBuff = iceBuf.sbuf;
            }

            // support video
            pSDP->support_v = pRspSdp->support_v;
        
            // through
            if ( pRspSdp->through.video_enabled )
            {
                if (PCP_OK == pcp_skt_build_through_info(&(pRspSdp->through), &throughBuf))
                {
                    pSDP->through.pcBuff = throughBuf.sbuf;
                }
            }
        }
    }
    
    SKTLVPickle tlvPickle(&g_tMetaSipexMsg);
    iRet = tlvPickle.Struct2Buffer(eTypeCallRsp, (void *)&callRsp, sizeof(callRsp), &callBuf);
    if ( iRet != 0 )
    {
        ms_error("pcp_skt_build_call_rsp_msg failed(%d).", iRet);
        str_release(&iceBuf);
        str_release(&throughBuf);
        return PCP_ERR;
    }

    if ( callBuf.GetLen() > PCP_SKT_BODY_DATA_MAX_SIZE )
    {
        ms_error("pcp_skt_build_call_rsp_msg failed. call rsp msg too long.");
        str_release(&iceBuf);
        str_release(&throughBuf);
        return PCP_ERR;
    }

    memcpy(pBody->sbuf, callBuf.GetBuffer(), callBuf.GetLen());
    pBody->slen = callBuf.GetLen();

    str_release(&iceBuf);
    str_release(&throughBuf);
    
    return PCP_OK;
}

int pcp_skt_parse_call_rsp_msg(IN str_t* pBody, OUT pcp_call_rsp_t* pCallRsp)
{
    SKBuffer callBuf;
    CallRsp_t callRsp;
    pcp_int32 iRet = PCP_SUCCESS;
    
    if ( NULL == pCallRsp || NULL == pBody || NULL == pBody->sbuf )
    {
        return PCP_ERR;
    }

    memset(&callRsp, 0x00, sizeof(CallRsp_t));
    
    callBuf.Write(pBody->sbuf, pBody->slen);
    SKTLVPickle tlvPickle(&g_tMetaSipexMsg);

    iRet = tlvPickle.Buffer2Struct(eTypeCallRsp, &callBuf, &callRsp, sizeof(callRsp));
	if(iRet != 0)
	{
		ms_error("pcp_skt_parse_call_rsp_msg failed at Buffer2Struct(%d).", iRet);
		return PCP_ERR;
	}
    
    pCallRsp->reason = callRsp.errcode;
    pCallRsp->sflag = callRsp.sflag;
    if ( NULL != callRsp.callid.pcBuff)
    {
        strncpy(pCallRsp->call_id, callRsp.callid.pcBuff, sizeof(pCallRsp->call_id) - 1);
    }
    ms_report("pcp_skt_parse_call_rsp_msg: callid[%s], reason[%d]", \
            pCallRsp->call_id, pCallRsp->reason);
    if ( pCallRsp->reason == PCP_SUCCESS )
    {
        if ( NULL != callRsp.ver.pcBuff )
        {
            strncpy(pCallRsp->ver, callRsp.ver.pcBuff, sizeof(pCallRsp->ver) - 1);
        }
        
        pcp_skt_tlvsdp2struct(&(callRsp.callsvr_sdp), pCallRsp->sdp);
    }
    
    SKAllocator Allocator(&g_tMetaSipexMsg);
	iRet = Allocator.FreeField(eTypeCallRsp, &callRsp, sizeof(callRsp));
	if(iRet != 0)
	{
	    ms_error("pcp_skt_parse_call_rsp_msg failed at memory free.");
	}
	
    return PCP_OK;
}

// call rsp ack, operation code 3
int pcp_skt_build_call_ack_req_msg(IN pcp_call_ack_t* pCallAck, OUT str_t* pBody)
{
    CallAck_t callAck;
    SKBuffer callBuf;
    pcp_int32 iRet = PCP_OK;

    if ( NULL == pCallAck || NULL == pBody )
    {
        return PCP_ERR;
    }

    memset((void *)&callAck, 0x00, sizeof(CallAck_t));
    callAck.callid.pcBuff = pCallAck->call_id;
    callAck.ice_mode = pCallAck->ice_mode;
    callAck.ice_type = pCallAck->ice_type;    

    // sdp ?
    
    SKTLVPickle tlvPickle(&g_tMetaSipexMsg);
    iRet = tlvPickle.Struct2Buffer(eTypeCallAck, (void *)&callAck, sizeof(callAck), &callBuf);
    if ( iRet != 0 )
    {
        ms_error("pcp_skt_build_call_rsp_msg failed(%d).", iRet);
        return PCP_ERR;
    }

    if ( callBuf.GetLen() > PCP_SKT_BODY_DATA_MAX_SIZE )
    {
        ms_error("pcp_skt_build_call_rsp_msg failed. call rsp msg too long.");
        return PCP_ERR;
    }

    memcpy(pBody->sbuf, callBuf.GetBuffer(), callBuf.GetLen());
    pBody->slen = callBuf.GetLen();
    return PCP_OK;
}

int pcp_skt_parse_call_ack_req_msg(IN str_t* pBody, OUT pcp_call_ack_t* pCallAck)
{
    SKBuffer callBuf;
    CallAck_t callAck;
    pcp_int32 iRet = PCP_OK;

    if ( NULL == pBody || NULL == pBody->sbuf
        || !pBody->slen || NULL == pCallAck )
    {
        return PCP_ERR;
    }

    memset((void *)&callAck, 0x00, sizeof(CallAck_t));
    callBuf.Write(pBody->sbuf, pBody->slen);
    SKTLVPickle tlvPickle(&g_tMetaSipexMsg);

    iRet = tlvPickle.Buffer2Struct(eTypeCallAck, &callBuf, &callAck, sizeof(callAck));
	if(iRet != 0)
	{
		ms_error("pcp_skt_parse_call_ack_req_msg failed at Buffer2Struct(%d).", iRet);
		return PCP_ERR;
	}
    
    pCallAck->ice_mode = callAck.ice_mode;
    pCallAck->ice_type = callAck.ice_type;
    strncpy(pCallAck->call_id, callAck.callid.pcBuff, sizeof(pCallAck->call_id) - 1);

    // sdp?
    
    SKAllocator Allocator(&g_tMetaSipexMsg);
	iRet = Allocator.FreeField(eTypeCallAck, &callAck, sizeof(callAck));
	if(iRet != 0)
	{
	    ms_error("pcp_skt_parse_call_ack_req_msg failed at memory free.");
	}
	
    return PCP_OK;
}

// call bye, operation code 4
int pcp_skt_build_bye_req_msg(IN pcp_bye_req_t* pBye, OUT str_t* pBody)
{
    ByeReq_t byeReq;
    SKBuffer callBuf;
    pcp_int32 iRet = PCP_OK;

    if ( NULL == pBye || NULL == pBody || NULL == pBody->sbuf )
    {
        return PCP_ERR;
    }

    memset((void *)&byeReq, 0x00, sizeof(ByeReq_t));
    byeReq.callid.pcBuff = pBye->call_id;
    byeReq.errcode = pBye->reason;
    
    SKTLVPickle tlvPickle(&g_tMetaSipexMsg);
    iRet = tlvPickle.Struct2Buffer(eTypeByeReq, (void *)&byeReq, sizeof(byeReq), &callBuf);
    if ( iRet != 0 )
    {
        ms_error("pcp_skt_build_bye_req_msg failed(%d).", iRet);
        return PCP_ERR;
    }

    if ( callBuf.GetLen() > PCP_SKT_BODY_DATA_MAX_SIZE )
    {
        ms_error("pcp_skt_build_bye_req_msg failed. bye req msg too long.");
        return PCP_ERR;
    }

    memcpy(pBody->sbuf, callBuf.GetBuffer(), callBuf.GetLen());
    pBody->slen = callBuf.GetLen();
    
    return PCP_OK;
}

int pcp_skt_parse_bye_req_msg(IN str_t* pBody, OUT pcp_bye_req_t* pBye)
{
    ByeReq_t byeReq;
    SKBuffer callBuf;
    pcp_int32 iRet = PCP_OK;

    if ( NULL == pBody || NULL == pBody->sbuf
        || !pBody->slen || NULL == pBye )
    {
        return PCP_ERR;
    }

    memset((void *)&byeReq, 0x00, sizeof(ByeReq_t));
    callBuf.Write(pBody->sbuf, pBody->slen);
    SKTLVPickle tlvPickle(&g_tMetaSipexMsg);

    iRet = tlvPickle.Buffer2Struct(eTypeByeReq, &callBuf, &byeReq, sizeof(byeReq));
	if(iRet != 0)
	{
		ms_error("pcp_skt_parse_bye_req_msg failed at Buffer2Struct(%d).", iRet);
		return PCP_ERR;
	}

    pBye->reason = byeReq.errcode;
    strncpy(pBye->call_id, byeReq.callid.pcBuff, sizeof(pBye->call_id) - 1);

    SKAllocator Allocator(&g_tMetaSipexMsg);
	iRet = Allocator.FreeField(eTypeByeReq, &byeReq, sizeof(byeReq));
	if(iRet != 0)
	{
	    ms_error("pcp_skt_parse_bye_req_msg failed at memory free.");
	}
	
    return PCP_OK;
}

// call media update req, operation code 5
int pcp_skt_build_update_req_msg(IN pcp_update_req_t* pUpdateReq, OUT str_t* pBody)
{
    return PCP_OK;
}

int pcp_skt_parse_update_req_msg(IN str_t* pBody, OUT pcp_update_req_t* pUpdateReq)
{
    CallUpdate_t callUpdate;
    SKBuffer callBuf;
    pcp_int32 iRet = PCP_OK;

    if ( NULL == pBody || NULL == pBody->sbuf
        || !pBody->slen || NULL == pUpdateReq )
    {
        return PCP_ERR;
    }

    memset((void *)&callUpdate, 0x00, sizeof(CallUpdate_t));
    callBuf.Write(pBody->sbuf, pBody->slen);
    SKTLVPickle tlvPickle(&g_tMetaSipexMsg);

    iRet = tlvPickle.Buffer2Struct(eTypeUpdate, &callBuf, &callUpdate, sizeof(callUpdate));
	if(iRet != 0)
	{
		ms_error("pcp_skt_parse_direct_ntfy_msg failed at Buffer2Struct(%d).", iRet);
		return PCP_ERR;
	}

    strncpy(pUpdateReq->call_id, callUpdate.callid.pcBuff, sizeof(pUpdateReq->call_id) - 1);
    strncpy(pUpdateReq->mip, callUpdate.mip.pcBuff, sizeof(pUpdateReq->mip) - 1);
    strncpy(pUpdateReq->apt, callUpdate.apt.pcBuff, sizeof(pUpdateReq->apt) - 1);
    strncpy(pUpdateReq->vpt, callUpdate.vpt.pcBuff, sizeof(pUpdateReq->vpt) - 1);
    pUpdateReq->aport = callUpdate.aport;
    pUpdateReq->vport = callUpdate.vport;

	SKAllocator Allocator(&g_tMetaSipexMsg);
	iRet = Allocator.FreeField(eTypeUpdate, &callUpdate, sizeof(callUpdate));
	if(iRet != 0)
	{
	    ms_error("pcp_skt_parse_direct_ntfy_msg failed at memory free.");
	}
    
    return PCP_OK;
}

// call media update rsp, operation code 6
int pcp_skt_build_update_rsp_msg(IN pcp_update_rsp_t* pUpdateRsp, OUT str_t* pBody)
{
    CallUpdateRsp_t updateRsp;
    SKBuffer callBuf;
    pcp_int32 iRet = PCP_OK;

    if ( NULL == pUpdateRsp || NULL == pBody )
    {
        return PCP_ERR;
    }

    memset((void *)&updateRsp, 0x00, sizeof(CallUpdateRsp_t));
    updateRsp.callid.pcBuff = pUpdateRsp->call_id;
    updateRsp.errcode = pUpdateRsp->reason;
    updateRsp.apt.pcBuff = pUpdateRsp->apt;
    updateRsp.vpt.pcBuff = pUpdateRsp->vpt;
    updateRsp.ice_mode = pUpdateRsp->ice_mode;
    
    SKTLVPickle tlvPickle(&g_tMetaSipexMsg);
    iRet = tlvPickle.Struct2Buffer(eTypeUpdateRsp, (void *)&updateRsp, sizeof(updateRsp), &callBuf);
    if ( iRet != 0 )
    {
        ms_error("pcp_skt_build_update_rsp_msg failed(%d).", iRet);
        return PCP_ERR;
    }
    
    if ( callBuf.GetLen() > PCP_SKT_BODY_DATA_MAX_SIZE )
    {
        ms_error("pcp_skt_build_update_rsp_msg failed. update rsp msg too long.");
        return PCP_ERR;
    }

    memcpy(pBody->sbuf, callBuf.GetBuffer(), callBuf.GetLen());
    pBody->slen = callBuf.GetLen();
    
    return PCP_OK;
}

int pcp_skt_parse_update_rsp_msg(IN str_t* pBody, OUT pcp_update_rsp_t* pUpdateRsp)
{
    return PCP_OK;
}

// call ring rsp, operation code 7
int pcp_skt_build_ring_rsp_msg(IN pcp_ring_rsp_t* pRingRsp, OUT str_t* pBody)
{
    CallRing_t callRing;
    SKBuffer callBuf;
    pcp_uint16 mediaNum = 0;
    pcp_uint16 rtppNum = 0;
    Media_t    aMedia[PCP_MAX_MEDIA_NUM];
    Rtpp_t     aRtpp[PCP_MAX_RTPP];
    SDP_t*     pSDP = NULL;
    pcp_sdp_info_t *pRspSdp = NULL;
    str_t    iceBuf;
    str_t    throughBuf;
    pcp_int32 mediaIndex = 0;
    pcp_int32 iRet = PCP_OK;

    if ( NULL == pRingRsp || NULL == pBody )
    {
        return PCP_ERR;
    }

    memset((void *)&callRing, 0x00, sizeof(CallRing_t));
    memset((void *)&aMedia, 0x00, sizeof(aMedia));
    callRing.callid.pcBuff = pRingRsp->call_id;

    // sdp
    str_init(&iceBuf, MAX_JSON_BODY_SIZE);
    str_init(&throughBuf, MAX_JSON_BODY_SIZE);
    
    pSDP = ((SDP_t *)&callRing.callsvr_sdp);
    pRspSdp = pRingRsp->sdp;
    if ( NULL != pRspSdp )
    {
        // connect ip
        pSDP->cConIp.pcBuff = pRspSdp->c_ip;
        // version
        pSDP->Ver.pcBuff = pRspSdp->v_version;
        
        // media list
        mediaNum = MIN(pRspSdp->media.media_num, PCP_MAX_MEDIA_NUM);
        if ( mediaNum > 0 )
        {
            // the 1st priority codec
            for ( int i = 0; i < mediaNum; i++ )
            {
                if (check_media_available(&pRspSdp->media.media_arr[i], 
                                pRingRsp->baudio_pt, pRingRsp->bvideo_pt))
                {
                    aMedia[mediaIndex].cType = pRspSdp->media.media_arr[i].type[0];
                    aMedia[mediaIndex].iPort = pRspSdp->media.media_arr[i].port;
                    aMedia[mediaIndex].iPr = pRspSdp->media.media_arr[i].pr;
                    aMedia[mediaIndex].iPt = pRspSdp->media.media_arr[i].pt;                    
                    mediaIndex++;
                }                
            }

            // other priority codecs
            for ( int i = 0; i < mediaNum; i++ )
            {
                if (!check_media_available(&pRspSdp->media.media_arr[i], 
                                pRingRsp->baudio_pt, pRingRsp->bvideo_pt))
                {
                    aMedia[mediaIndex].cType = pRspSdp->media.media_arr[i].type[0];
                    aMedia[mediaIndex].iPort = pRspSdp->media.media_arr[i].port;
                    aMedia[mediaIndex].iPr = pRspSdp->media.media_arr[i].pr;
                    aMedia[mediaIndex].iPt = pRspSdp->media.media_arr[i].pt;
                    mediaIndex++;
                }
            }
        }
#if 0
        for ( int i = 0; i < mediaNum; i++ )
        {
            ms_report("pcp_skt_build_ring_rsp_msg 3: [%d] type[%c] pr[%d] pt[%d] port[%d]", i, \
                aMedia[i].cType, aMedia[i].iPr, aMedia[i].iPt, aMedia[i].iPort);
        }
#endif
        pSDP->tMedialist.iCount = mediaNum;
        pSDP->tMedialist.ptList = aMedia;

        // rtpp list
        rtppNum = MIN(pRspSdp->rtpp.rtpp_num, PCP_MAX_RTPP);
        if ( rtppNum > 0 )
        {
            for ( int i = 0; i < rtppNum; i++ )
            {
                aRtpp[i].cIp.pcBuff = pRspSdp->rtpp.rtpp_arr[i].ip;
                aRtpp[i].iDelay = pRspSdp->rtpp.rtpp_arr[i].delay;
                aRtpp[i].iLost = pRspSdp->rtpp.rtpp_arr[i].lost;
#if 0
                ms_report("pcp_skt_tlvsdp2struct: rtpp { %s %d %d }",
                    pRspSdp->rtpp.rtpp_arr[i].ip, \
                    pRspSdp->rtpp.rtpp_arr[i].delay, \
                    pRspSdp->rtpp.rtpp_arr[i].lost);
#endif
            }
        }
        pSDP->tRtpplist.iCount = rtppNum;
        pSDP->tRtpplist.ptList = aRtpp;

        // ice info            
        if (PCP_OK == pcp_skt_build_ice_info(&(pRspSdp->ice), &iceBuf))
        {
            pSDP->cIce.iLen = iceBuf.slen;
            pSDP->cIce.pcBuff = iceBuf.sbuf;
        }

        // support video
        pSDP->support_v = pRspSdp->support_v;
        
        // through
        if ( pRspSdp->through.video_enabled )
        {
            if (PCP_OK == pcp_skt_build_through_info(&(pRspSdp->through), &throughBuf))
            {
                pSDP->through.pcBuff = throughBuf.sbuf;
            }
        }
    }
    
    SKTLVPickle tlvPickle(&g_tMetaSipexMsg);
    iRet = tlvPickle.Struct2Buffer(eTypeCallRing, (void *)&callRing, sizeof(callRing), &callBuf);
    if ( iRet != 0 )
    {
        ms_error("pcp_skt_build_update_rsp_msg failed(%d).", iRet);
        str_release(&iceBuf);
        str_release(&throughBuf);
        return PCP_ERR;
    }
    
    if ( callBuf.GetLen() > PCP_SKT_BODY_DATA_MAX_SIZE )
    {
        ms_error("pcp_skt_build_update_rsp_msg failed. update rsp msg too long.");
        str_release(&iceBuf);
        str_release(&throughBuf);
        return PCP_ERR;
    }

    memcpy(pBody->sbuf, callBuf.GetBuffer(), callBuf.GetLen());
    pBody->slen = callBuf.GetLen();

    str_release(&iceBuf);
    str_release(&throughBuf);
    return PCP_OK;
}

int pcp_skt_parse_ring_rsp_msg(IN str_t* pBody, OUT pcp_ring_rsp_t* pRingRsp)
{
    CallRing_t callRing;
    SKBuffer callBuf;
    pcp_int32 iRet = PCP_OK;

    if ( NULL == pBody || NULL == pBody->sbuf
        || !pBody->slen || NULL == pRingRsp )
    {
        return PCP_ERR;
    }

    memset((void *)&callRing, 0x00, sizeof(CallRing_t));
    callBuf.Write(pBody->sbuf, pBody->slen);
    SKTLVPickle tlvPickle(&g_tMetaSipexMsg);

    iRet = tlvPickle.Buffer2Struct(eTypeCallRing, &callBuf, &callRing, sizeof(callRing));
	if(iRet != 0)
	{
		ms_error("pcp_skt_parse_ring_rsp_msg failed at Buffer2Struct(%d).", iRet);
		return PCP_ERR;
	}

    pRingRsp->sflag = callRing.sflag;
    if ( callRing.callid.pcBuff )
    {
	    strncpy(pRingRsp->call_id, callRing.callid.pcBuff, sizeof(pRingRsp->call_id) - 1);
    }
    
	//sdp
	pcp_skt_tlvsdp2struct(&(callRing.callsvr_sdp), pRingRsp->sdp);
	
    SKAllocator Allocator(&g_tMetaSipexMsg);
	iRet = Allocator.FreeField(eTypeCallRing, &callRing, sizeof(callRing));
	if(iRet != 0)
	{
	    ms_error("pcp_skt_parse_ring_rsp_msg failed at memory free.");
	}
    return PCP_OK;
}

// call direct notify rsp, operation code 8
int pcp_skt_parse_direct_ntfy_msg(IN str_t* pBody, OUT pcp_direct_notify_t* pNotify)
{
    DirectNfy_t nfyRsp;
    SKBuffer callBuf;
    pcp_int32 iRet = PCP_OK;

    if ( NULL == pBody || NULL == pBody->sbuf
        || !pBody->slen || NULL == pNotify )
    {
        return PCP_ERR;
    }

    memset((void *)&nfyRsp, 0x00, sizeof(DirectNfy_t));
    callBuf.Write(pBody->sbuf, pBody->slen);
    SKTLVPickle tlvPickle(&g_tMetaSipexMsg);

    iRet = tlvPickle.Buffer2Struct(eTypeDirectNfy, &callBuf, &nfyRsp, sizeof(nfyRsp));
	if(iRet != 0)
	{
		ms_error("pcp_skt_parse_direct_ntfy_msg failed at Buffer2Struct(%d).", iRet);
		return PCP_ERR;
	}

	pNotify->reason = nfyRsp.errcode;
	strncpy(pNotify->call_id, nfyRsp.callid.pcBuff, sizeof(pNotify->call_id) - 1);

	SKAllocator Allocator(&g_tMetaSipexMsg);
	iRet = Allocator.FreeField(eTypeDirectNfy, &nfyRsp, sizeof(nfyRsp));
	if(iRet != 0)
	{
	    ms_error("pcp_skt_parse_direct_ntfy_msg failed at memory free.");
	}
    return PCP_OK;
}

// call route update req, operation code 15
int pcp_skt_build_route_update_msg(IN pcp_route_update_t* pRoute, OUT str_t* pBody)
{
    RouteUpdate_t routeReq;
    SKBuffer callBuf;
    pcp_int32 iRet = PCP_OK;

    if ( NULL == pRoute || NULL == pBody )
    {
        return PCP_ERR;
    }

    memset((void *)&routeReq, 0x00, sizeof(RouteUpdate_t));
    routeReq.callid.pcBuff = pRoute->call_id;
    routeReq.userid.pcBuff = pRoute->userid;

    SKTLVPickle tlvPickle(&g_tMetaSipexMsg);
    iRet = tlvPickle.Struct2Buffer(eTypeRouteUpdate, (void *)&routeReq, sizeof(routeReq), &callBuf);
    if ( iRet != 0 )
    {
        ms_error("pcp_skt_build_route_update_msg failed(%d).", iRet);
        return PCP_ERR;
    }

    if ( callBuf.GetLen() > PCP_SKT_BODY_DATA_MAX_SIZE )
    {
        ms_error("pcp_skt_build_route_update_msg failed. route update req msg too long.");
        return PCP_ERR;
    }

    memcpy(pBody->sbuf, callBuf.GetBuffer(), callBuf.GetLen());
    pBody->slen = callBuf.GetLen();
    return PCP_OK;
}

// call dtmf info req, operation code 16
int pcp_skt_parse_dtmf_info_msg(IN str_t* pBody, OUT pcp_dtmf_info_t*pDtmfInfo)
{
    DtmfInfo_t dtmfInfo;
    SKBuffer callBuf;
    pcp_int32 iRet = PCP_OK;

    if ( NULL == pBody || NULL == pBody->sbuf
        || !pBody->slen || NULL == pDtmfInfo )
    {
        return PCP_ERR;
    }

    memset((void *)&dtmfInfo, 0x00, sizeof(DtmfInfo_t));
    callBuf.Write(pBody->sbuf, pBody->slen);
    SKTLVPickle tlvPickle(&g_tMetaSipexMsg);

    iRet = tlvPickle.Buffer2Struct(eTypeDtmfInfo, &callBuf, &dtmfInfo, sizeof(dtmfInfo));
	if(iRet != 0)
	{
		ms_error("pcp_skt_parse_dtmf_info_msg failed at Buffer2Struct(%d).", iRet);
		return PCP_ERR;
	}

    strncpy(pDtmfInfo->dtmf_str, dtmfInfo.dtmf.pcBuff, sizeof(pDtmfInfo->dtmf_str));
    strncpy(pDtmfInfo->call_id, dtmfInfo.callid.pcBuff, sizeof(pDtmfInfo->call_id) - 1);
    if ( strlen(pDtmfInfo->dtmf_str) > 0 )
    {
        switch(pDtmfInfo->dtmf_str[0])
        {
            case '0':
        		pDtmfInfo->dtmf_int = PCP_DTMF_0;
        		break;
            case '1':
        		pDtmfInfo->dtmf_int = PCP_DTMF_1;
        		break;
        	case '2':
        		pDtmfInfo->dtmf_int = PCP_DTMF_2;
        		break;
        	case '3':
        		pDtmfInfo->dtmf_int = PCP_DTMF_3;
        		break;
        	case '4':
        		pDtmfInfo->dtmf_int = PCP_DTMF_4;
        		break;
        	case '5':
        		pDtmfInfo->dtmf_int = PCP_DTMF_5;
        		break;
        	case '6':
        		pDtmfInfo->dtmf_int = PCP_DTMF_6;
        		break;
        	case '7':
        		pDtmfInfo->dtmf_int = PCP_DTMF_7;
        		break;
        	case '8':
        		pDtmfInfo->dtmf_int = PCP_DTMF_8;
        		break;
        	case '9':
        		pDtmfInfo->dtmf_int = PCP_DTMF_9;
        		break;
        	case '*':
        		pDtmfInfo->dtmf_int = PCP_DTMF_STAR;
        		break;        	
        	case '#':
        		pDtmfInfo->dtmf_int = PCP_DTMF_POUND;
        		break;
        	case 'A':
        	case 'a':
        		pDtmfInfo->dtmf_int = PCP_DTMF_A;
        		break;	
        	case 'B':
        	case 'b':
        		pDtmfInfo->dtmf_int = PCP_DTMF_B;
        		break;
        	case 'C':
        	case 'c':
        		pDtmfInfo->dtmf_int = PCP_DTMF_C;
        		break;
        	case 'D':
        	case 'd':
        		pDtmfInfo->dtmf_int = PCP_DTMF_D;
        		break;
        	case '!':
        		pDtmfInfo->dtmf_int = PCP_FLASH;
        		break;
        	default:
        		return PCP_ERR;
        }
    }

    SKAllocator Allocator(&g_tMetaSipexMsg);
	iRet = Allocator.FreeField(eTypeDtmfInfo, &dtmfInfo, sizeof(dtmfInfo));
	if(iRet != 0)
	{
	    ms_error("pcp_skt_parse_dtmf_info_msg failed at memory free.");
	}
	
    return PCP_OK;
}

// call internal state passthrough req, operation code 19
int pcp_skt_build_in_trans_req_msg(IN pcp_in_trans_req_t* pTransReq, OUT str_t* pBody)
{
    InTransReq_t transReq;
    SKBuffer callBuf;
    pcp_int32 iRet = PCP_OK;
    pcp_int8 through[255] = { 0 };

    if ( NULL == pTransReq || NULL == pBody )
    {
        return PCP_ERR;
    }

    memset((void *)&transReq, 0x00, sizeof(InTransReq_t));
    transReq.callid.pcBuff = pTransReq->call_id;

    /* Combine passthrough data begin */
    transReq.through.pcBuff = through;
    sprintf(through, "va:%d,vs:%d,p2p:%d,mic:%d,net:%d", \
        pTransReq->vie_angle, pTransReq->vie_state,
        pTransReq->p2p_state, pTransReq->mic_state,
        pTransReq->net_state);
    /* end */

    SKTLVPickle tlvPickle(&g_tMetaSipexMsg);
    iRet = tlvPickle.Struct2Buffer(eTypeInTransReq, 
                                    (void *)&transReq, 
                                    sizeof(transReq), 
                                    &callBuf);
    if ( iRet != 0 )
    {
        ms_error("[%s %d] failed(%d).", __FUNCTION__, __LINE__, iRet);
        return PCP_ERR;
    }

    if ( callBuf.GetLen() > PCP_SKT_BODY_DATA_MAX_SIZE )
    {
        ms_error("[%s %d] failed on msg too big.", __FUNCTION__, __LINE__);
        return PCP_ERR;
    }

    memcpy(pBody->sbuf, callBuf.GetBuffer(), callBuf.GetLen());
    pBody->slen = callBuf.GetLen();
    return PCP_OK;
}

int pcp_skt_parse_in_trans_req_msg(IN str_t* pBody, OUT pcp_in_trans_req_t* pTransReq)
{
    InTransReq_t transReq;
    SKBuffer callBuf;
    pcp_int32 iRet = PCP_OK;
    pcp_int8 through[255] = { 0 };
    pcp_int8* p = NULL;
    pcp_int8* q = NULL;

    if ( NULL == pBody || NULL == pBody->sbuf
        || !pBody->slen || NULL == pTransReq )
    {
        return PCP_ERR;
    }

    memset((void *)&transReq, 0x00, sizeof(InTransReq_t));
    callBuf.Write(pBody->sbuf, pBody->slen);
    SKTLVPickle tlvPickle(&g_tMetaSipexMsg);

    iRet = tlvPickle.Buffer2Struct(eTypeInTransReq, 
                                    &callBuf, 
                                    &transReq, 
                                    sizeof(transReq));
	if ( iRet != 0 )
	{
		ms_error("[%s %d] failed on Buffer2Struct(%d).", __FUNCTION__, __LINE__, 
                iRet);
		return PCP_ERR;
	}

    if ( transReq.callid.pcBuff )
    {
	    strncpy(pTransReq->call_id, transReq.callid.pcBuff, 
                sizeof(pTransReq->call_id) - 1);
    }

    /* passthrough data */
    if ( transReq.through.pcBuff )
    {
        strncpy(through, transReq.through.pcBuff, sizeof(through)-1);
        p = strtok(through, ",");
    
        while( p != NULL )
        {
            q = strstr(p, ":");
            if (strncmp(p, "va", strlen("va")) ==0 ) {
                pTransReq->vie_angle = atoi(q+1);
            }
    		else if(strncmp(p, "vs", strlen("vs")) == 0){
    			pTransReq->vie_state = atoi(q+1);
            }
    		else if(strncmp(p, "p2p", strlen("p2p")) == 0){
    			pTransReq->p2p_state = atoi(q+1);
            }
    		else if(strncmp(p, "mic", strlen("mic")) == 0){
    			pTransReq->mic_state = atoi(q+1);
            }
    		else if(strncmp(p, "net", strlen("net")) == 0){
    			pTransReq->net_state = atoi(q+1);
            }
            p = strtok(NULL, ",");
        }
    }
    
	SKAllocator Allocator(&g_tMetaSipexMsg);
	iRet = Allocator.FreeField(eTypeInTransReq, &transReq, sizeof(transReq));
	if(iRet != 0)
	{
	    ms_error("[%s %d] memory free failed.", __FUNCTION__, __LINE__);
	}
    return PCP_OK;
}

// call internal state passthrough rsp, operation code 20
int pcp_skt_build_in_trans_rsp_msg(IN pcp_in_trans_rsp_t* pTransRsp, OUT str_t* pBody)
{
    InTransRsp_t transRsp;
    SKBuffer callBuf;
    pcp_int32 iRet = PCP_OK;

    if ( NULL == pTransRsp || NULL == pBody )
    {
        return PCP_ERR;
    }

    memset((void *)&transRsp, 0x00, sizeof(InTransRsp_t));
    transRsp.callid.pcBuff = pTransRsp->call_id;
    transRsp.errcode = pTransRsp->errcode;

    SKTLVPickle tlvPickle(&g_tMetaSipexMsg);
    iRet = tlvPickle.Struct2Buffer(eTypeInTransRsp, 
                                    (void *)&transRsp, 
                                    sizeof(transRsp), 
                                    &callBuf);
    if ( iRet != 0 )
    {
        ms_error("[%s %d] failed(%d).", __FUNCTION__, __LINE__, iRet);
        return PCP_ERR;
    }

    if ( callBuf.GetLen() > PCP_SKT_BODY_DATA_MAX_SIZE )
    {
        ms_error("[%s %d] failed on msg too big.", __FUNCTION__, __LINE__);
        return PCP_ERR;
    }

    memcpy(pBody->sbuf, callBuf.GetBuffer(), callBuf.GetLen());
    pBody->slen = callBuf.GetLen();
    
    return PCP_OK;
}

int pcp_skt_parse_in_trans_rsp_msg(IN str_t* pBody, OUT pcp_in_trans_rsp_t* pTransRsp)
{
    InTransRsp_t transRsp;
    SKBuffer callBuf;
    pcp_int32 iRet = PCP_OK;

    if ( NULL == pBody || NULL == pBody->sbuf
        || !pBody->slen || NULL == pTransRsp )
    {
        return PCP_ERR;
    }

    memset((void *)&transRsp, 0x00, sizeof(InTransRsp_t));
    callBuf.Write(pBody->sbuf, pBody->slen);
    SKTLVPickle tlvPickle(&g_tMetaSipexMsg);

    iRet = tlvPickle.Buffer2Struct(eTypeInTransRsp, 
                                    &callBuf, 
                                    &transRsp, 
                                    sizeof(transRsp));
	if(iRet != 0)
	{
		ms_error("[%s %d] failed on Buffer2Struct(%d).", __FUNCTION__, __LINE__, 
                iRet);
		return PCP_ERR;
	}

    if ( transRsp.callid.pcBuff )
    {
	    strncpy(pTransRsp->call_id, transRsp.callid.pcBuff, 
                sizeof(pTransRsp->call_id) - 1);
    }
    pTransRsp->errcode = transRsp.errcode;
    
	SKAllocator Allocator(&g_tMetaSipexMsg);
	iRet = Allocator.FreeField(eTypeInTransRsp, &transRsp, sizeof(transRsp));
	if(iRet != 0)
	{
	    ms_error("[%s %d] memory free failed.", __FUNCTION__, __LINE__);
	}
    return PCP_OK;
}

// build ice string
int pcp_skt_build_ice_info(IN pcp_ice_info_t* pIceInfo, OUT str_t* pIceBuf)
{
    json_t* jIce = NULL;
    json_t* jTmp = NULL;
    json_t* jObj = NULL;
    pcp_int8* pBuf = NULL;    

    if ( NULL == pIceBuf || NULL == pIceInfo )
    {
        return PCP_ERR;
    }
    
    if ( NULL == pIceBuf->sbuf || !pIceBuf->slen )
    {
        return PCP_ERR;
    }

    /* create ice object */
	if ( pIceInfo->cdd_num > 0 )
	{
        /*create ice json object*/
	    jIce = jsonapi_new_value(JSON_OBJECT, NULL);
        
		jTmp = jsonapi_new_value(JSON_STRING, pIceInfo->ufrag);	
		jsonapi_append_value(jIce, "ice-ufrag", jTmp);

		jTmp = jsonapi_new_value(JSON_STRING, pIceInfo->pwd);	
		jsonapi_append_value(jIce, "ice-pwd", jTmp);

		jObj = jsonapi_new_value(JSON_ARRAY, NULL);
		for (int i = 0; i < pIceInfo->cdd_num; i++)
		{
			jTmp = jsonapi_new_value(JSON_STRING, pIceInfo->cdd_arr[i].attr_desc);	
			jsonapi_append_value(jObj, NULL, jTmp);
		}
		jsonapi_append_value(jIce, "candidates", jObj);

        /*convert jIce to string */
    	jsonapi_value_to_string(jIce, &pBuf);
        if ( strlen(pBuf) > pIceBuf->slen )
        {
            free(pBuf);
        
        	/* delete jIce */
        	jsonapi_delete_value(&jIce);
            return PCP_ERR;
        }
        
        pIceBuf->slen = strlen(pBuf);
        memcpy(pIceBuf->sbuf, pBuf, pIceBuf->slen);

        //ms_report("pcp_skt_build_ice_info: iceBuf[ %s ]", pIceBuf->sbuf);
    	free(pBuf);
        
    	/*delete jIce*/
    	jsonapi_delete_value(&jIce);

        return PCP_OK;
	}

    pIceBuf->slen = 0;

    return PCP_OK;
}

int pcp_skt_parse_ice_info(IN str_t* pIceBuf, OUT pcp_ice_info_t* pIceInfo)
{
    json_t* jIce = NULL;
	json_t* jCandidate = NULL;
	json_t *ita, *itb;
	int i = 0;
    char iceBuf[1024] = { 0 };
    char *pEnd = NULL;
    
    if ( NULL == pIceBuf || !pIceBuf->slen 
        || NULL == pIceBuf->sbuf || NULL == pIceInfo )
    {
        ms_error("pcp_skt_parse_ice_info ice buffer is NULL.");
        return PCP_ERR;
    }

    /* convert string to jIce */
    //ms_report("pcp_skt_parse_ice_info: iceBuf[ %s ]", pIceBuf->sbuf);
    if ( NULL == (pEnd = strstr(pIceBuf->sbuf, "}")) )
    {
        ms_error("pcp_skt_parse_ice_info wrong ice json string[%s].", pIceBuf->sbuf);
        return PCP_ERR;
    }
    
    strncpy(iceBuf, pIceBuf->sbuf, pEnd - pIceBuf->sbuf + 1);
	jsonapi_string_to_value(&jIce, iceBuf);
    if ( NULL == jIce )
    {
        ms_error("pcp_skt_parse_ice_info failed to parse ice json string.");
        return PCP_ERR;
    }
    
	jsonapi_parser_string(jIce, "ice-ufrag", pIceInfo->ufrag);
	jsonapi_parser_string(jIce, "ice-pwd", pIceInfo->pwd);
	jsonapi_parser_value(jIce, "candidates", &jCandidate);

	if ( jCandidate != NULL)
	{
		/* iterate through children */	
		if (jCandidate->child != NULL)	
		{		
			ita = jCandidate->child;

			/*parser candidate option info*/
			while (ita != NULL) 	
			{			
				jsonapi_parser_string(ita, NULL, pIceInfo->cdd_arr[i].attr_desc);
		
				itb = ita->next;			
				ita = itb;	
				i++;	
			}	
			
			pIceInfo->cdd_num = i;
		}
	}

    /* release jIce */
	jsonapi_delete_value(&jIce);
    
	return PCP_OK;
}

// build through string
int pcp_skt_build_through_info(IN pcp_through_info_t* pThrough, OUT str_t* pStr)
{
#if 0
    json_t* jThrough = NULL;
    json_t* jTmp = NULL;
    pcp_int8* pBuf = NULL;
    pcp_int8  strtmp[16] = {0};
#endif

    if ( NULL == pStr || NULL == pThrough )
    {
        return PCP_ERR;
    }
    
    if ( NULL == pStr->sbuf || !pStr->slen )
    {
        return PCP_ERR;
    }

#if 0
    /*create ice json object*/
    jThrough = jsonapi_new_value(JSON_OBJECT, NULL);

    sprintf(strtmp, "%d", pThrough->resolution.uiMaxDecodeDefinition);
	jTmp = jsonapi_new_value(JSON_NUMBER, strtmp);	
	jsonapi_append_value(jThrough, "umdd", jTmp);

    sprintf(strtmp, "%d", pThrough->resolution.camera_width);
	jTmp = jsonapi_new_value(JSON_NUMBER, strtmp);	
	jsonapi_append_value(jThrough, "cwidth", jTmp);

    sprintf(strtmp, "%d", pThrough->resolution.camera_height);
	jTmp = jsonapi_new_value(JSON_NUMBER, strtmp);	
	jsonapi_append_value(jThrough, "cheight", jTmp);

    sprintf(strtmp, "%d", pThrough->resolution.rotate);
	jTmp = jsonapi_new_value(JSON_NUMBER, strtmp);	
	jsonapi_append_value(jThrough, "rotate", jTmp);

    sprintf(strtmp, "%d", pThrough->decode_video_fps);
	jTmp = jsonapi_new_value(JSON_NUMBER, strtmp);	
	jsonapi_append_value(jThrough, "fps", jTmp);

    sprintf(strtmp, "%d", pThrough->v0);
	jTmp = jsonapi_new_value(JSON_NUMBER, strtmp);	
	jsonapi_append_value(jThrough, "v0", jTmp);

    sprintf(strtmp, "%d", pThrough->start_bitrate);
	jTmp = jsonapi_new_value(JSON_NUMBER, strtmp);	
	jsonapi_append_value(jThrough, "sbr", jTmp);

    sprintf(strtmp, "%d", pThrough->min_bitrate);
	jTmp = jsonapi_new_value(JSON_NUMBER, strtmp);	
	jsonapi_append_value(jThrough, "mibr", jTmp);

    sprintf(strtmp, "%d", pThrough->max_bitrate);
	jTmp = jsonapi_new_value(JSON_NUMBER, strtmp);	
	jsonapi_append_value(jThrough, "mabr", jTmp);

    /*convert jdialing to string */
	jsonapi_value_to_string(jThrough, &pBuf);
    if ( strlen(pBuf) > pStr->slen )
    {
        free(pBuf);
    
    	/*delete jdialing*/
    	jsonapi_delete_value(&jThrough);
        return PCP_ERR;
    }
    
    pStr->slen = strlen(pBuf);
    memcpy(pStr->sbuf, pBuf, pStr->slen);

	free(pBuf);
    
	/*delete jdialing*/
	jsonapi_delete_value(&jThrough);
#else
    sprintf(pStr->sbuf, "d:%d,cw:%d,ch:%d,r:%d,f:%d,v:%d,b0:%d,b1:%d,b2:%d", \
        pThrough->resolution.uiMaxDecodeDefinition, \
        pThrough->resolution.camera_width, pThrough->resolution.camera_height, \
        pThrough->resolution.rotate, pThrough->decode_video_fps, pThrough->v0, \
        pThrough->start_bitrate, pThrough->min_bitrate, pThrough->max_bitrate);

    //ms_report("pcp_skt_build_through_info: throughBuf[ %s ]", pStr->sbuf);
#endif
    return PCP_OK;
}

int pcp_skt_parse_through_info(IN str_t* pStr, OUT pcp_through_info_t* pThrough)
{
    if ( NULL == pStr || !pStr->slen 
        || NULL == pStr->sbuf || NULL == pThrough )
    {
        return PCP_ERR;
    }

#if 0
    json_t* jThrough = NULL;
    
    /*convert string to jdialing*/
	jsonapi_string_to_value(&jThrough, pStr->sbuf);
    if ( NULL == jThrough )
    {
        return PCP_ERR;
    }

    jsonapi_parser_number(jThrough, "umdd", &(pThrough->resolution.uiMaxDecodeDefinition));
    jsonapi_parser_number(jThrough, "cwidth", &(pThrough->resolution.camera_width));
    jsonapi_parser_number(jThrough, "cheight", &(pThrough->resolution.camera_height));
    jsonapi_parser_number(jThrough, "rotate", &(pThrough->resolution.rotate));
    jsonapi_parser_number(jThrough, "fps", &(pThrough->decode_video_fps));
    jsonapi_parser_number(jThrough, "v0", (int *)&(pThrough->v0));
    jsonapi_parser_number(jThrough, "sbr", &(pThrough->start_bitrate));
    jsonapi_parser_number(jThrough, "mibr", &(pThrough->min_bitrate));
    jsonapi_parser_number(jThrough, "mabr", &(pThrough->max_bitrate));

    /*release jdialing*/
	jsonapi_delete_value(&jThrough);
#else
    char *p = NULL;
    char *q = NULL;

    p = strtok(pStr->sbuf, ",");
    
    while( p != NULL )
    {
        q = strstr(p, ":");
        if (strncmp(p, "d", strlen("d")) ==0 ) {
            pThrough->resolution.uiMaxDecodeDefinition = atoi(q+1);
        }
		else if(strncmp(p, "cw", strlen("cw")) == 0){
			pThrough->resolution.camera_width = atoi(q+1);
        }
		else if(strncmp(p, "ch", strlen("ch")) == 0){
			pThrough->resolution.camera_height = atoi(q+1);
        }
		else if(strncmp(p, "r", strlen("r")) == 0){
			pThrough->resolution.rotate = atoi(q+1);
        }
		else if(strncmp(p, "f", strlen("f")) == 0){
			pThrough->decode_video_fps = atoi(q+1);
        }
		else if(strncmp(p, "v", strlen("v")) == 0){
			pThrough->v0 = atoi(q+1);
        }
		else if(strncmp(p, "b0", strlen("b0")) == 0){
			pThrough->start_bitrate = atoi(q+1);
        }
		else if(strncmp(p, "b1", strlen("b1")) == 0){
			pThrough->min_bitrate = atoi(q+1);
        }
		else if(strncmp(p, "b2", strlen("b2")) == 0){
			pThrough->max_bitrate = atoi(q+1);
        }
        else{
            return PCP_ERR;
        }
        
        p = strtok(NULL, ",");
    }
#endif
    
    return PCP_OK;
}

int pcp_skt_tlvsdp2struct(SDP_t* pTlvSdp, pcp_sdp_info_t* pSdpInfo)
{
    str_t strIce;
    str_t strThrough;
    
    if ( NULL == pTlvSdp || NULL == pSdpInfo )
    {
        ms_error("pcp_skt_tlvsdp2struct error on NULL param.");
        return PCP_ERR;
    }
    
    str_init(&strIce, MAX_JSON_BODY_SIZE);
    str_init(&strThrough, MAX_JSON_BODY_SIZE);
    if ( NULL != pTlvSdp->cConIp.pcBuff )
    {
        strncpy(pSdpInfo->c_ip, pTlvSdp->cConIp.pcBuff, sizeof(pSdpInfo->c_ip) - 1);
    }
    if ( NULL != pTlvSdp->Ver.pcBuff )
    {
        strncpy(pSdpInfo->v_version, pTlvSdp->Ver.pcBuff, sizeof(pSdpInfo->v_version) - 1);
    }

    //media list
    pSdpInfo->media.media_num = MIN(PCP_MAX_MEDIA_NUM, pTlvSdp->tMedialist.iCount);
    for(int i = 0; i < pSdpInfo->media.media_num; i++)
    {
        pSdpInfo->media.media_arr[i].type[0] = pTlvSdp->tMedialist.ptList[i].cType;
        pSdpInfo->media.media_arr[i].port = pTlvSdp->tMedialist.ptList[i].iPort;
        pSdpInfo->media.media_arr[i].pr = pTlvSdp->tMedialist.ptList[i].iPr;
        pSdpInfo->media.media_arr[i].pt = pTlvSdp->tMedialist.ptList[i].iPt;
#if 0        
        ms_report("pcp_skt_tlvsdp2struct: type[%s] pr[%d] pt[%d] port[%d]", \
            pSdpInfo->media.media_arr[i].type, \
            pSdpInfo->media.media_arr[i].pr, \
            pSdpInfo->media.media_arr[i].pt, \
            pSdpInfo->media.media_arr[i].port);
#endif
    }

    // rtpp list
    pSdpInfo->rtpp.rtpp_num = MIN(PCP_MAX_RTPP, pTlvSdp->tRtpplist.iCount);
    for (int i = 0; i < pSdpInfo->rtpp.rtpp_num; i++)
    {
        pSdpInfo->rtpp.rtpp_arr[i].delay = pTlvSdp->tRtpplist.ptList[i].iDelay;
        pSdpInfo->rtpp.rtpp_arr[i].lost = pTlvSdp->tRtpplist.ptList[i].iLost;
        strncpy(pSdpInfo->rtpp.rtpp_arr[i].ip, pTlvSdp->tRtpplist.ptList[i].cIp.pcBuff, \
            sizeof(pSdpInfo->rtpp.rtpp_arr[i].ip) - 1);
    }
    
    // ice info
    memset(strIce.sbuf, 0x00, strIce.slen);
    strIce.slen = pTlvSdp->cIce.iLen;
    if ( strIce.slen > 0 && (NULL != pTlvSdp->cIce.pcBuff))
    {
        memcpy(strIce.sbuf, pTlvSdp->cIce.pcBuff, strIce.slen);
        pcp_skt_parse_ice_info(&strIce, &(pSdpInfo->ice));
    }

    // support video
    pSdpInfo->support_v = pTlvSdp->support_v;
            
    //through info
    memset(strThrough.sbuf, 0x00, strThrough.slen);    
    if ( NULL != pTlvSdp->through.pcBuff )
    {
        strThrough.slen = strlen(pTlvSdp->through.pcBuff);
        memcpy(strThrough.sbuf, pTlvSdp->through.pcBuff, strThrough.slen);
        pcp_skt_parse_through_info(&strThrough, &(pSdpInfo->through));
    }
    
    str_release(&strIce);
    str_release(&strThrough);

    return PCP_OK;
}

/* compress and decompress */
int pcp_skt_compress(IN PcpSktCompress_t* pComParam, OUT str_t* pData)
{
    SKBuffer dataBuf;
    RBBuffer comBuf;
    pcp_int32 iRet = PCP_OK;
    
    if ( NULL == pComParam || NULL == pData )
    {
        ms_error("pcp_skt_compress: NULL param.");
        return PCP_ERR;
    }

    if ( NULL == pData->sbuf || !pData->slen )
    {
        ms_error("pcp_skt_compress: NULL data.");
        return PCP_ERR;
    }

    if (MicroMsg::MMCPR_NO_COMPRESS == pComParam->CompressAlgorithm)
	{
		return PCP_OK;
	}
	
#if 0	
    dataBuf.Write(pData->sbuf, pData->slen);

    pComParam->CompressLen = dataBuf.GetLen();
    iRet = MicroMsg::Compress((unsigned char *)dataBuf.GetBuffer(), dataBuf.GetLen(),
        &comBuf, pComParam->CompressAlgorithm, pComParam->CompressVersion);
    if ( iRet != 0 ) {
		ms_error("pcp_skt_compress ERR: compress failed(%d).", iRet);
		return PCP_ERR;
	}

	dataBuf.Reset();
	dataBuf.Write(comBuf.GetReadPtr(), comBuf.GetLen());
	if ( dataBuf.GetLen() > pData->slen )
	{
	    ms_error("pcp_skt_compress ERR: compress len > data len.");
	    return PCP_ERR;
	}
	pData->slen = dataBuf.GetLen();
	memcpy(pData->sbuf, dataBuf.GetBuffer(), pData->slen);
#else
    pComParam->CompressLen = pData->slen;
    iRet = MicroMsg::Compress((unsigned char *)pData->sbuf, pData->slen,
        &comBuf, pComParam->CompressAlgorithm, pComParam->CompressVersion);
    if ( iRet != MicroMsg::MMCPR_OK && iRet != MicroMsg::MMCPR_NO_COMPRESS ) {
		ms_error("pcp_skt_compress ERR: compress failed(%d).", iRet);
		return PCP_ERR;
	}

	if ( comBuf.GetLen() > pData->slen )
	{
	    ms_error("pcp_skt_compress ERR: compress len > data len.");
	    return PCP_ERR;
	}
	
	pData->slen = comBuf.GetLen();
	memcpy(pData->sbuf, comBuf.GetReadPtr(), pData->slen);
#endif

	return PCP_OK;
}

int pcp_skt_decompress(IN PcpSktCompress_t* pComParam, 
                                IN str_t* pBody, OUT str_t* pOutData)
{
    SKBuffer dataBuf;
    RBBuffer decomBuf;
    pcp_int32 iRet = PCP_OK;
    
    if ( NULL == pComParam || NULL == pOutData || NULL == pBody )
    {
        ms_error("pcp_skt_decompress: NULL param.");
        return PCP_ERR;
    }

    if ( NULL == pOutData->sbuf || !pOutData->slen )
    {
        ms_error("pcp_skt_decompress: NULL output data.");
        return PCP_ERR;
    }

    if ( !pBody->slen || NULL == pBody->sbuf )
    {
        ms_error("pcp_skt_decompress: NULL input data.");
        return PCP_ERR;
    }

    if (MicroMsg::MMCPR_NO_COMPRESS == pComParam->CompressAlgorithm)
	{
		return PCP_OK;
	}

#if 0	
    dataBuf.Write(pBody->sbuf, pBody->slen);
    iRet = MicroMsg::Decompress((unsigned char*)dataBuf.GetBuffer(), dataBuf.GetLen(),&decomBuf,
            		pOutData->slen, pComParam->CompressAlgorithm, pComParam->CompressVersion);
	
	if ( iRet != 0)
	{
		ms_error("pcp_skt_decompress failed(%d)", iRet);
		return PCP_ERR;
	}
	
	dataBuf.Reset();
	dataBuf.Write(decomBuf.GetReadPtr(), decomBuf.GetLen());
	if ( pOutData->slen < decomBuf.GetLen())
	{
	    ms_error("pcp_skt_decompress not enough space for decompress data.");
	    return PCP_ERR;
	}
	pOutData->slen = dataBuf.GetLen();
	memcpy(pOutData->sbuf, dataBuf.GetBuffer(), pOutData->slen);
#else
    iRet = MicroMsg::Decompress((unsigned char*)pBody->sbuf, pBody->slen, &decomBuf,
            		pOutData->slen, pComParam->CompressAlgorithm, pComParam->CompressVersion);	
	if ( iRet != 0)
	{
		ms_error("pcp_skt_decompress failed(%d)", iRet);
		return PCP_ERR;
	}

	if ( pOutData->slen < decomBuf.GetLen())
	{
	    ms_error("pcp_skt_decompress not enough space for decompress data.");
	    return PCP_ERR;
	}
	pOutData->slen = decomBuf.GetLen();
	memcpy(pOutData->sbuf, decomBuf.GetReadPtr(), pOutData->slen);
#endif
	return PCP_OK;
}

#endif /* SKT_PROTOCOL_SUPPORT_ENABLED */

