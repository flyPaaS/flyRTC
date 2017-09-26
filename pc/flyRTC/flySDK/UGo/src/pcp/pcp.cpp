#include "pcp_skt_parser.h"
#include "cfg.h"
#include <stdlib.h>
#include "logtrace.h"
#include "encrypt.h"
#include "getaddress.h"
#include "pcp.h"
#include "pcp_session.h"
#include "pcp_sdp.h"
#include "pcp_errcode.h"
#include "pcp_timer.h"
#include "pcp_parser.h"
#include "pcp_trace.h"
#include "pcp_pm.h"
#include "comm.h"

static void cb_timeout(int timeid);
static int check_ipaddr(const char* ipaddr);

static pcp_bool isbusy(void);
static pcp_bool ice_enabled(int remote_ice);
static pcp_bool in_trans_enabled(int remoteflag);
static pcp_bool issuccess(int reason);
static pcp_bool media_enabled(pcp_media_type_t type, int code);
static pcp_bool fec_enabled(pcp_media_type_t type, int sflag);
static pcp_bool nack_enabled(pcp_media_type_t type, int sflag);
int pcp_check_in_trans_state(IN pcp_call_trans_state_t* old_state, 
                             IN const pcp_call_trans_state_t* new_state);
static int update_call_param(pcp_session_t* psession);
static int convert_notify_reason(int svr_reason);
static int on_nack(const pcp_route_t* prt);
static int on_version_rsp(const pcp_route_t* prt, const str_t* bd);
static int on_login_rsp(const pcp_route_t* prt, const str_t* bd);
static int on_kickout_notify(const pcp_route_t* prt, const str_t* bd);
static int on_call_req(const pcp_route_t* prt, const str_t* bd);
static int on_call_rsp(const pcp_route_t* prt, const str_t* bd);
static int on_call_ack(const pcp_route_t* prt, const str_t* bd);
static int on_call_bye(const pcp_route_t* prt, const str_t* bd);
static int on_update_req(const pcp_route_t* prt, const str_t* bd);
static int on_ring_rsp(const pcp_route_t* prt, const str_t* bd);
static int on_direct_notify(const pcp_route_t* prt, const str_t* bd);
static int on_recv_dtmf(const pcp_route_t* prt, const str_t* bd);
/* add by Vinton.Liu on 2016-04-06 for add internal state passthrough signalling begin */
static int on_in_trans_req(const pcp_route_t* prt, const str_t* bd);
static int on_in_trans_rsp(const pcp_route_t* prt, const str_t* bd);
/* Vinton.Liu add end */
static int on_m_call_rsp(const pcp_route_t* prt, const str_t* bd);//add by wuzhaoyang20140428
static int on_m_notify(const pcp_route_t* prt, const str_t* bd);//add by wuzhaoyang20140428
static int on_convert_conference(const pcp_route_t* prt, const str_t* bd);//add by wuzhaoyang20140504
static int on_m_inv_rsp(const pcp_route_t* prt, const str_t* bd);
static int on_m_del_rsp(const pcp_route_t* prt, const str_t* bd);
/* add by Vinton.Liu on 2016-05-04 for unsupport op rsp err begin */
static int on_unsupport_req(const pcp_route_t* prt);
static int build_unsupport_rsp_err(const pcp_route_t* prt);
/* Vinton.Liu add end */
static int build_call_rsp_err(int errcode, const char* callid, const pcp_route_t* proute);
static int build_call_update_rsp(pcp_update_rsp_t* rsp, const pcp_route_t* proute);

static int report_register_event(int ev_type, int ev_reason, const char* describe);
static int report_call_event(int ev_type, int ev_reason, void* callpm, int size);
static int report_call_log(pcp_session_t* psession);
static int send_msg(str_t* hmsg, str_t* bmsg, pcp_bool vflag);
#if defined(SKT_PROTOCOL_SUPPORT_ENABLED)
static int send_skt_msg( const pcp_route_t* prt, const void* pBodyParm);
#endif
static int pcp_send_msg(const pcp_route_t* prt, const void* pBodyParm);

pcp_call_info_t g_call_info_report;
static pcp_login_t g_login;
static unsigned char directcallfec_enable = 0;
static int normalcall_to_conference = FALSE;

void cb_timeout(int timeid)
{
	int time_type = 0;
	int reason = PCP_MSGTIMEOUT;
	char display[128] = {0};
	
	time_type= get_timer(timeid);

	switch(time_type)
	{
		case TIMEOUT_T0:
			sprintf(display, "pcp tm: ring rsp timeout.");
			break;
		case TIMEOUT_T1:
			sprintf(display, "pcp tm: answer timeout.");
			reason = PCP_NOANSWER;
			break;
		case TIMEOUT_T2:
			sprintf(display, "pcp tm: ack timeout.");
			break;
		case TIMEOUT_T3:
			sprintf(display, "pcp tm: push timeout.");
			break;
		case TIMEOUT_T4:
			sprintf(display, "pcp tm: conf call rsp timeout.");
			break;
        case TIMEOUT_T5:
            reason = PCP_IN_TRANS_TIMEOUT;
            sprintf(display, "pcp tm: in trans rsp timeout.");
            break;
		default:
			break;
	}

	call_event_cb(PCP_CALLTIMEOUT_EV, reason, display, NULL, 0);	
}

int check_ipaddr(const char* ipaddr)
{
    int a = 0, b = 0, c = 0, d = 0;
    char t;
    char str_1[15] = { 0 }; 
    char str_2[15] = { 0 };
    char str_3[15] = { 0 };
    char str_4[15] = { 0 };

    if ( NULL == ipaddr )
    {
        return PCP_ERR;
    }

    if ( strlen(ipaddr) < 7 || strlen(ipaddr) > 15 )
    {
        ms_error("[%s %d] wrong ip addr(%s).", __FUNCTION__, __LINE__, ipaddr);
        return PCP_ERR;
    }
    else if ( 4 == sscanf(ipaddr, "%[^.].%[^.].%[^.].%s", str_1, str_2, str_3, str_4) )
    {
        if (strlen(str_1)>3 || strlen(str_2)>3 || strlen(str_3)>3 || strlen(str_4)>3)
        {
            ms_error("[%s %d] wrong ip addr(%s).", __FUNCTION__, __LINE__, ipaddr);
            return PCP_ERR;
        }
        else if ( 4 == sscanf(ipaddr, "%d.%d.%d.%d%c", &a, &b, &c, &d, &t) )
        {
            if ( (0 <= a && a <= 255)
                && (0 <= b && b <= 255)
                && (0 <= c && c <= 255)
                && (0 <= d && d <= 255))
            {
                return PCP_OK;
            }
        }
    }

    ms_error("[%s %d] wrong ip addr(%s).", __FUNCTION__, __LINE__, ipaddr);
    return PCP_ERR;
}

int report_register_event(int ev_type, int ev_reason, const char* describe)
{
	register_event_cb(ev_type, ev_reason, describe);

	return PCP_OK;
}

int report_call_event(int ev_type, int ev_reason, void* callpm, int size)
{
	char str_tmp[128] = {0};

	if ( ev_type == PCP_DTMF_EV && ev_reason != -1 )
	{
	    snprintf(str_tmp, 128, "Have get the dtmf data, dtmfvalue = %d\n", ev_reason);
	}
	else
	{
	    pcp_errcode_str(&ev_reason, str_tmp);
	}
	call_event_cb(ev_type, ev_reason, str_tmp, (void*)callpm, size);
	
	return PCP_OK;
}

int report_call_log(pcp_session_t* psession)
{
	char log_str[MAX_TRACE_BUF_SIZE] = {0};
	
	if (psession != NULL)
	{
		trace_call_log(psession, log_str);
		call_log_cb(PCP_TRACE_REPORT, log_str);
		ms_report("%s", log_str);
	}

	return PCP_OK;
}

int send_msg(str_t* hmsg, str_t* bmsg, pcp_bool vflag)
{
	str_t sm;
	pcp_msg_t pmsg;
	char data[MAX_MSG_SIZE] = {0};

	sm.slen = MAX_MSG_SIZE;
	sm.sbuf = data;

	/*build msg data*/
	res_st(&pmsg, sizeof(pmsg));
	pmsg.version = vflag==PCP_MSG_VERSION_TP?PCP_VERSION:0;
	if (hmsg != NULL && hmsg->slen != 0)
	{
		pmsg.hlen = hmsg->slen;
		pmsg.hmsg = hmsg->sbuf;
	}
	
	if (bmsg != NULL && bmsg->slen != 0)
	{
		/*RC4 encrypt*/
		if (is_local_rc4_enabled() && !vflag)
		{	
			encrypt_packet(bmsg->sbuf, bmsg->slen, bmsg->sbuf, &bmsg->slen, ENCRYPT_UGO);
		}

		pmsg.blen = bmsg->slen;
		pmsg.bmsg = bmsg->sbuf;
	}
	
	build_msg(&pmsg, &sm);

	/*call send callback*/
	call_send_cb(sm.sbuf, sm.slen);
	
	return PCP_OK;
}

#if defined(SKT_PROTOCOL_SUPPORT_ENABLED)
int send_skt_msg(const pcp_route_t* prt, const void* pBodyParm)
{
    int iRet;
    PcpSktMsg_t sktMsg;
    PcpSktMsgHeader_t msgHeader;
    PcpSktVoipBodyHeader_t bodyHeader;
    str_t strBody;
    str_t strMsg;
    pcp_uint16 dataLen = 0;

    if ( NULL == prt || (NULL == pBodyParm && prt->op != PCP_UNSUPPORT_RSP_OP ))
    {
        return PCP_ERR;
    }

    ms_report("send_skt_msg: type[%d] op[%d].", prt->type, prt->op);
    
    memset((void *)&sktMsg, 0x00, sizeof(PcpSktMsg_t));
    memset((void *)&msgHeader, 0x00, PCP_SKT_MSG_HEADER_SIZE);
    memset((void *)&bodyHeader, 0x00, PCP_SKT_VOIP_BODY_HEADER_SIZE);

    str_init(&strBody, PCP_SKT_BODY_DATA_MAX_SIZE);
    switch( prt->op )
    {
        case PCP_CALL_REQ_OP:
        {   
            pcp_call_req_t callReq;
            memset(&callReq, 0x00, sizeof(pcp_call_req_t));
            iRet = pcp_skt_build_call_req_msg((pcp_call_req_t *)pBodyParm, &strBody);
        }
        break;

        case PCP_CALL_RSP_OP:
        {
            iRet = pcp_skt_build_call_rsp_msg((pcp_call_rsp_t *)pBodyParm, &strBody);
        }
        break;

        case PCP_CALL_ACK_OP:
        {
            iRet = pcp_skt_build_call_ack_req_msg((pcp_call_ack_t *)pBodyParm, &strBody);
        }
        break;

        case PCP_CALL_BYE_OP:
        {
            iRet = pcp_skt_build_bye_req_msg((pcp_bye_req_t *)pBodyParm, &strBody);
        }
        break;

        case PCP_UPDATE_RSP_OP:
        {
            iRet = pcp_skt_build_update_rsp_msg((pcp_update_rsp_t *)pBodyParm, &strBody);
        }
        break;

        case PCP_RING_RSP_OP:
        {
            iRet = pcp_skt_build_ring_rsp_msg((pcp_ring_rsp_t *)pBodyParm, &strBody);
        }
        break;

        case PCP_ROUTE_UPDATE_OP:
        {
            iRet = pcp_skt_build_route_update_msg((pcp_route_update_t *)pBodyParm, &strBody);
        }
        break;

        /* add by Vinton.Liu on 2016-04-06 for add internal state passthrough signalling begin */
        case PCP_IN_TRANS_REQ_OP:
        {
            iRet = pcp_skt_build_in_trans_req_msg((pcp_in_trans_req_t *)pBodyParm, &strBody);
        }
        break;

        case PCP_IN_TRANS_RSP_OP:
        {
            iRet = pcp_skt_build_in_trans_rsp_msg((pcp_in_trans_rsp_t *)pBodyParm, &strBody);
        }
        break;
        /* Vinton.Liu add end */
        
        /* add by Vinton.Liu on 2016-05-04 for unsupport op rsp err begin */
        case PCP_UNSUPPORT_RSP_OP:
            iRet = PCP_OK;
        break;
        /* Vinton.Liu add end */
        
        default:
        {
            ms_warning("send_skt_msg: not find match op code.");
            str_release(&strBody);            
            return PCP_OK;
        }
    }

    if ( (prt->op != PCP_CALLREQ_ACK_OP)
        && ( prt->op != PCP_UNSUPPORT_RSP_OP) )
    {
        if ( PCP_ERR == iRet )
        {
            ms_warning("send_skt_msg: build msg body data error.");
            str_release(&strBody);
            return PCP_ERR;
        }
        else
        {
            /* Compress */
            if ( prt->enc & PCP_ENC_TYPE_COMPRESS )
            {
                PcpSktCompress_t stComParam;
                memset(&stComParam, 0x00, sizeof(PcpSktCompress_t));
                stComParam.CompressAlgorithm = PCP_SKT_COMPRESS_ALG_ZLIB;
                stComParam.CompressVersion = PCP_SKT_COMPRESS_VER;
                
//#define TEST_COMPRESS             
#ifdef TEST_COMPRESS //test
                strcpy(strBody.sbuf, "12345678901234567890123456789012345678901234567890");
                strBody.slen = strlen("12345678901234567890123456789012345678901234567890");
#endif
                pcp_skt_compress(&stComParam, &strBody);
#ifdef TEST_COMPRESS
                ms_report("send_skt_msg: compressed[%d]", strBody.slen);
                for ( int i = 0; i < strBody.slen; i++)
            	{            		
            		ms_report("%d   ", strBody.sbuf[i]);
            	}

                str_t strTmp;
                char tmpBuf[256] = {0};
                strTmp.slen = 256;
                strTmp.sbuf = tmpBuf;
            	pcp_skt_decompress(&stComParam, &strBody, &strTmp);

            	ms_report("send_skt_msg: decompressed[%d][%s]", strTmp.slen, strTmp.sbuf);
                return PCP_OK;
#endif
            }
            
            /*RC4 encrypt*/
            if ( prt->enc & PCP_ENC_TYPE_RC4 )
            {	
                encrypt_packet(strBody.sbuf, strBody.slen, strBody.sbuf, &strBody.slen, ENCRYPT_UGO);
            }
            
            dataLen = strBody.slen;
            sktMsg.dataLen = dataLen;
            sktMsg.bodyData = strBody.sbuf;
        }
    }

    int msg_cmd = PCP_SKT_MSG_CMD_VOIP_CALL;
    /* add by Vinton.Liu on 2016-06-02 for customer services version begin */
    if ( prt->type == PCP_CONF_TP )
    {
        msg_cmd = PCP_SKT_MSG_CMD_VOIP_CONF;
    }
    else if ( prt->type == PCP_CSRV_TP )
    {
        msg_cmd = PCP_SKT_MSG_CMD_VOIP_CSRV;
    }
    /* Vinton.Liu add end */
    
    pcp_skt_build_msg_header(&msgHeader, msg_cmd, dataLen);

    bodyHeader.op = prt->op;
    bodyHeader.ver = DEFAULT_VOIP_HEADER_VER;
    bodyHeader.ack = prt->ack;
    bodyHeader.atype = prt->atype;
    bodyHeader.enc = prt->enc;
    bodyHeader.servid = prt->dsid;
    bodyHeader.headlen = PCP_SKT_VOIP_BODY_HEADER_SIZE;
    
    sktMsg.bodyHeader = (PcpSktVoipBodyHeader_t *)&bodyHeader;
    sktMsg.msgHeader = (PcpSktMsgHeader_t *)&msgHeader;

    str_init(&strMsg, PCP_SKT_MSG_MAX_SIZE);
    pcp_skt_build_msg(&sktMsg, &strMsg);

    /*call send callback*/
    call_send_cb(strMsg.sbuf, strMsg.slen);

    str_release(&strBody);
    str_release(&strMsg);
    return PCP_OK;
}
#endif

int pcp_send_msg(const pcp_route_t* prt, const void* pBodyParm)
{    
    int ret = PCP_OK;    
    
    if ( NULL == prt || (NULL == pBodyParm && prt->op != PCP_UNSUPPORT_RSP_OP))
    {
        return PCP_ERR;
    }

    if ( (prt->type != PCP_CALL_TP)
        && (prt->type != PCP_CONF_TP)
        && (prt->type != PCP_CSRV_TP) )
    {
        ms_error("[pcp_send_msg] msg type[%d] send failed.", prt->type);
        return PCP_ERR;
    }
    
    ms_report("[pcp_send_msg] msg type[%d] op[%d] enter.", prt->type, prt->op);
    if ( !pcp_is_tlv_enabled())
    {
        str_t hmsg, bmsg;
        int needSend = 1;
        
        res_st(&hmsg, sizeof(hmsg));
        res_st(&bmsg, sizeof(bmsg));
        
    	
        /*build message bson header*/
        str_init(&hmsg, MAX_HEAD_SIZE);
        build_route_bson_header(prt, &hmsg);

        /*build message json body*/
        str_init(&bmsg, MAX_BODY_SIZE);
        switch ( prt->op )
        {
            case PCP_CALL_REQ_OP:
                ret = build_call_req_json_body((pcp_call_req_t*)pBodyParm, &bmsg);
            break;

            case PCP_CALL_RSP_OP:
                ret = build_call_rsp_json_body((pcp_call_rsp_t*)pBodyParm, &bmsg);
            break;

            case PCP_CALL_ACK_OP:
                ret = build_call_ack_json_body((pcp_call_ack_t*)pBodyParm, &bmsg);
            break;

            case PCP_CALL_BYE_OP:
                ret = build_bye_req_json_body((pcp_bye_req_t*)pBodyParm, &bmsg);
            break;

            case PCP_UPDATE_RSP_OP:
                ret = build_update_rsp_json_body((pcp_update_rsp_t*)pBodyParm, &bmsg);
            break;

            case PCP_RING_RSP_OP:
                ret = build_ringing_rsp_json_body((pcp_ring_rsp_t*)pBodyParm, &bmsg);
            break;

            case PCP_PUSH_RSP_OP:
                ret = build_push_rsp_json_body((pcp_push_rsp_t*)pBodyParm, &bmsg);
            break;

            case PCP_ROUTE_UPDATE_OP:
                ret = build_route_update_json_body((pcp_route_update_t*)pBodyParm, &bmsg);
            break;

            case PCP_MEET_CALL_REQ_OP:
                ret = build_m_call_req_json_body((pcp_conf_call_req_t*)pBodyParm, &bmsg);
            break;

            case PCP_CONVERT_CONFERENCE_RSP_OP:
                ret = build_convert_conference_rsp_json_body((pcp_conf_convert_rsp_t*)pBodyParm, &bmsg);
            break;

            case PCP_MEET_INV_REQ_OP:
                ret = build_m_inv_req_json_body((pcp_conf_inv_req_t*)pBodyParm, &bmsg);
            break;

            case PCP_MEET_DEL_REQ_OP:
                ret = build_m_del_req_json_body((pcp_conf_del_req_t*)pBodyParm, &bmsg);
            break;
            
            /* add by Vinton.Liu on 2016-05-04 for unsupport op rsp err begin */
            case PCP_UNSUPPORT_RSP_OP:
                ret = PCP_OK;
            break;
            /* Vinton.Liu add end */

            default:
                needSend = 0;
            break;
        }

        /*send message */
        if ( needSend && ret == PCP_OK )
        {
            ret = send_msg(&hmsg, &bmsg, PCP_MSG_CALL_TP);
            //printf("******send op[%d]***************\n",prt->op);
            ms_report("#send op[%d] %.*s", prt->op, bmsg.slen, bmsg.sbuf);
        }

        /*release str buf*/
        str_release(&hmsg);
        str_release(&bmsg);
    }
#if defined(SKT_PROTOCOL_SUPPORT_ENABLED)
    else
    {
        ret = send_skt_msg(prt, pBodyParm);
    }
#endif

    ms_report("[pcp_send_msg] msg type[%d] op[%d] exit(%d).", prt->type, prt->op, ret);
    return ret;
}

pcp_bool isbusy(void)
{
	return (pcp_get_state() != PCP_STATE_WAIT ? TRUE : FALSE);
}
	
pcp_bool ice_enabled(int remote_ice)
{
	return (is_local_ice_enabled() && remote_ice ? TRUE : FALSE);
}

pcp_bool issuccess(int reason)
{
	return (reason != PCP_SUCCESS ? FALSE: TRUE);
}

pcp_bool media_enabled(pcp_media_type_t type, int code)
{
	pcp_bool res  = FALSE;

	if (code == PCP_BOTH_CONSULT_SUCCESS)
		return TRUE;
		
	if (type == AUDIO && code != PCP_AUDIO_CONSULT_FAILD)
	{
		res = TRUE;
	}
	else if(type == VIDEO && code != PCP_VIDEO_CONSULT_FAILD)
	{
		res = TRUE;
	}

	return res;
}

pcp_bool fec_enabled(pcp_media_type_t type, int remote_fec)
{
	pcp_bool enabled = FALSE;

	if (type == AUDIO)
	{
		enabled = (is_local_audio_fec_enabled() 
                    && (remote_fec & PCP_FEC_OPEN)) ? TRUE : FALSE;
	}
	else if (type == VIDEO)
	{
		enabled = (is_local_video_fec_enabled() 
                && (remote_fec & PCP_VIE_FEC_OPEN)) ? TRUE : FALSE;
	}
	
	return enabled;
}

static pcp_bool nack_enabled(pcp_media_type_t type, int sflag)
{
    pcp_bool enabled = FALSE;

	if (type == AUDIO)
	{
		
	}
	else if (type == VIDEO)
	{
		enabled = (is_local_video_nack_enabled() 
                && ( sflag & PCP_VIE_NACK_OPEN)) ? TRUE : FALSE;
	}
	
	return enabled;
}

pcp_bool direct_fec_enabled(pcp_media_type_t type, int remote_fec)
{
	pcp_bool enabled = FALSE;

	if (type == AUDIO)
	{
		enabled = (is_local_direct_fec_enabled() && (remote_fec & (PCP_DIRECTFEC_OPEN | PCP_DIRECTFEC_MULRIPLE_OPEN))) ? TRUE : FALSE;     //modified by king 20160120
	}
	else if (type == VIDEO)
	{
		enabled = FALSE;
	}

	return enabled;
}
//add by wuzhaoyang20151228
pcp_bool direct_mulriple_fec_enabled(pcp_media_type_t type, int remote_fec)
{
	pcp_bool enabled = FALSE;

	if (type == AUDIO)
	{
		enabled = (is_local_direct_mulriple_fec_enabled() && (remote_fec & PCP_DIRECTFEC_MULRIPLE_OPEN)) ? TRUE : FALSE;
	}
	else if (type == VIDEO)
	{
		enabled = FALSE;
	}

	return enabled;
}

pcp_bool in_trans_enabled(int remoteflag)
{
    int localflag = get_upate_flag();

    if ( (localflag & PCP_IN_TRANS_OPEN)
        && ( remoteflag & PCP_IN_TRANS_OPEN))
    {
        return TRUE;
    }

    return FALSE;
}

int convert_notify_reason(int svr_reason)
{
	return svr_reason+79;
}

int update_call_param(pcp_session_t* psession)
{
	pcp_call_param_t callpm;

	if(NULL == psession)
		return PCP_ERR;

	//update call info
    pcp_update_call_info_report(&g_call_info_report, psession);
    
	/*store call param*/
	res_st(&callpm, sizeof(callpm));

	if (psession->role == PCP_ANSWER || psession->mode != PCP_DIRECT_CM)
	{
		callpm.calltype = PCP_FREE_CALL_TP;
	}
	else 
	{
		callpm.calltype = PCP_DIRECT_CALL_TP;
	}

	callpm.videoflag = psession->video_flag;	
	callpm.audiofec = psession->afec_flag;
	callpm.directfec = psession->directfec_flag;
	callpm.directmulriplefec = psession->directfec_mulriple_flag;//add by wuzhaoyang20151228
	callpm.videofec = psession->vfec_flag;
    callpm.videonack = psession->vnack_flag;
	strcpy(callpm.callid, psession->callid);
	strcpy(callpm.fuid, psession->fuid);
	strcpy(callpm.fuserid, psession->fuserid);
	strcpy(callpm.fphone, psession->fphone);
   	/* add by VintonLiu on 20141219 for store nickname in callpm */
    strncpy(callpm.fnickname, psession->fnickname, sizeof(callpm.fnickname) -1);
	strcpy(callpm.tuid, psession->tuid);
	strcpy(callpm.tuserid, psession->tuserid);
	strcpy(callpm.tphone, psession->tphone);
	strncpy(callpm.user_data, psession->user_data, sizeof(psession->user_data) -1);

	return pm_update(&callpm);
}

int on_version_rsp(const pcp_route_t* prt, const str_t* bd)
{
	
	int res = 0;
	pcp_version_rsp_t version_rsp;
	
	if (prt == NULL || bd == NULL || bd->sbuf == NULL || bd->slen == 0)
	{
		ms_warning("on_version_rsp:msg error!!");
		return PCP_ERR;
	}

	res_st(&version_rsp, sizeof(version_rsp));
	res = parser_version_rsp_json_body((str_t*)bd, &version_rsp);
	if (res < 0)
	{
		ms_error("parser_version_rsp_json_body:maybe msg format error!!");
		return PCP_ERR;
	}

	set_randcode(version_rsp.randcode);

	return PCP_OK;
	
}

int on_login_rsp(const pcp_route_t* prt, const str_t* bd)
{
	int res = 0;
	pcp_login_rsp_t login_rsp;
	
	if (prt == NULL || bd == NULL || bd->sbuf == NULL || bd->slen == 0)
	{
		ms_warning("on_login_rsp:msg error!!");
		return PCP_ERR;
	}

	res_st(&login_rsp, sizeof(login_rsp));
	res = parser_login_rsp_json_body((str_t*)bd, &login_rsp);
	if (res < 0)
	{
		ms_error("parser_login_rsp_json_body:maybe msg format error!!");
		return PCP_ERR;
	}

	report_register_event(PCP_NONE_REGISTER_EV, login_rsp.result, login_rsp.description);
	
	return PCP_OK;
}

int on_kickout_notify(const pcp_route_t* prt, const str_t* bd)
{
	int res = 0;
	
	if (prt == NULL || bd == NULL || bd->sbuf == NULL || bd->slen == 0)
	{
		ms_warning("on_kickout_notify:msg error!!");
		return PCP_ERR;
	}

	report_register_event(PCP_NONE_REGISTER_EV, 9, "kick out notify.");
	
	return PCP_OK;
}

int on_nack(const pcp_route_t* prt)
{
	pcp_session_t* psession = NULL;
	pcp_call_param_t callpm;
	char callid[128] = {0};
	
	if (NULL == prt)
	{
		ms_warning("on_nack:prt is NULL!!");
		return PCP_ERR;
	}

	lock_session();
	psession = get_session(NULL);
	if (check_session(psession) < 0)
	{
		ms_error("Have No Usable Session !!!");
		ulock_session();
		return PCP_ERR;
	}
	
	/*update sesson to expired*/
	psession->expired_flag = TRUE;
	strcpy(callid, psession->callid);
	
	/*trace call log*/
	trace_nack_log(psession, prt->ack);
	report_call_log(psession);
	
	/*stop all timer*/
	stop_all_timers();

	/*upade call state to wait*/
	psession->call_state = PCP_STATE_WAIT;
	call_state_cb(psession->call_state);
	/*delete session */
	delete_session(psession);
	ulock_session();

	/*callback bye event*/
	res_st(&callpm, sizeof(callpm));
	strcpy(callpm.callid, callid);
#ifdef WEBRTC_LINUX
	sprintf(callpm.fuid, "%ld", prt->fuid);
#else
	sprintf(callpm.fuid, "%lld", prt->fuid);
#endif
	report_call_event(PCP_CALLHUNGUP_EV, PCP_UNREACHABLE, (void *)&callpm, PCP_CALL_PARAM_SIZE);

	return PCP_OK;
}

int on_call_req(const pcp_route_t* prt, const str_t* bd)
{
	char str_tmp[128] = {0};
	pcp_call_req_t callreq;
	pcp_call_param_t callpm;
	pcp_session_t* psession = NULL;
	pcp_sdp_info_t* psdp = NULL;
	int res = 0;
	int reason = PCP_SUCCESS;
	pcp_bool ibusy = FALSE;
	pcp_bool video_enabled = FALSE;
    
	if (prt == NULL || bd == NULL || bd->sbuf == NULL || bd->slen == 0)
	{
		ms_error("on_call_req:msg error!!");
		return PCP_ERR;
	}

	res_st(&callreq, sizeof(callreq));
	/*store remote sdp and parser call req*/
	callreq.sdp = create_sdp(0, 0);
    if ( callreq.sdp == NULL )
    {
        ms_error("on_call_req: create remote sdp error.");
		return PCP_ERR;
    }

	res_st(callreq.sdp, sizeof(pcp_sdp_info_t));
    if ( !pcp_is_tlv_enabled())
    {
	    res = parser_call_req_json_body((str_t*)bd, &callreq);
	}
#if defined(SKT_PROTOCOL_SUPPORT_ENABLED)
    else
    {
        res = pcp_skt_parse_call_req_msg((str_t*)bd, &callreq);
    }
#endif
	if (res < 0)
	{
		ms_error("on_call_req:maybe msg is err!!!");
		free_sdp(callreq.sdp);
		return PCP_ERR;
	}

	/*is busy ?*/
	ibusy = isbusy();
	lock_session();/*lock session*/	

    bool isConflict = false;
	if (ibusy)
	{
        //modify by wenqinglin
        //get_session(callreq.call_id);这个是获取ios推送响应的session，
        pcp_session_t* fPsession = NULL;//定义主叫session
        fPsession = get_session(pm_callid());//获取主叫的session
        char *tuid = fPsession->tuid;//获取主叫方的tuid
        char fuid[UGO_MAX_USERID_LEN] = {0};
        sprintf(fuid, "%lld", prt->fuid);//获取被叫方的fuid
        
		/*if it is busy, check session state if push active state*/
		psession = get_session(callreq.call_id);
		if (psession != NULL && psession->call_state == PCP_STATE_PUSHACTIVE)
		{
			stop_timer(TIMEOUT_T3);
		}
        else if (strcmp(tuid, fuid) == 0)//如果主叫方的tuid 跟  被叫方的fuid相等，说明是双方同时打电话
        {
            isConflict = true;
            char *fuided = fPsession->fuid;//获取主叫方的fuid
            //> 0 说明主叫方的fuid的值比较大，根据策略作为 此次通话的主叫方
            if (strcmp(fuided, fuid) > 0)
            {
                printf("~~~~~此次通话作为[主叫]~~~~~\n");
                //此次通话作为主叫
                free_sdp(callreq.sdp);
                ulock_session();/*unlock session*/
                return PCP_OK;
            }
            else
            {
                printf("~~~~~此次通话作为[被叫]~~~~~\n");
                //此次通话作为被叫
                delete_session(fPsession);
            }
        }
		else
		{
            /* for missed calls */
			build_call_rsp_err(PCP_BUSY, callreq.call_id, prt);
			free_sdp(callreq.sdp);
			reason = PCP_BUSY;
            goto drop_out;
		}
	}

	video_enabled = is_local_video_enabled();

	/*create local sdp and consultation*/
	psdp = create_sdp(1, video_enabled);
    if ( psdp == NULL )
    {
        ms_error("[on_call_req] create local sdp error.");
        ulock_session();
        free_sdp(callreq.sdp);
        return PCP_ERR;
    }
    
	res = media_sdp_consultation(psdp, callreq.sdp);
	if (res == PCP_BOTH_CONSULT_FAILD)
	{
		build_call_rsp_err(PCP_NOTACCEPT, callreq.call_id, prt);
		free_sdp(callreq.sdp);
		free_sdp(psdp);
		if (psession != NULL)
			delete_session(psession);
		reason = PCP_NOTACCEPT;
		video_enabled = FALSE;
		goto drop_out;
	}
	
	/*create session for incoming call and store session info*/
	if (psession == NULL)
		psession = create_session(callreq.call_id);
	
	if (psession != NULL)
	{
		psession->lsdp = psdp;
		psession->rsdp = callreq.sdp;
		psession->audio_flag = media_enabled(AUDIO, res);
		psession->video_flag = media_enabled(VIDEO, res);
		psession->afec_flag = fec_enabled(AUDIO, callreq.sflag);
        /* add by Vinton.Liu on 2016-05-30 for video fec anc nack config begin */
        psession->vfec_flag = fec_enabled(VIDEO, callreq.sflag);
        psession->vnack_flag = nack_enabled(VIDEO, callreq.sflag);
        /* Vinton.Liu add end */
		psession->directfec_flag = direct_fec_enabled(AUDIO, callreq.sflag);
		psession->directfec_mulriple_flag = direct_mulriple_fec_enabled(AUDIO, callreq.sflag);
		psession->ice_flag = ice_enabled(callreq.sdp->ice.cdd_num);
        psession->intrans_flag = in_trans_enabled(callreq.sflag);
        
		/* call req from vps not have "type" */
		if ( callreq.callmode < PCP_DIRECT_CM 
		    || callreq.callmode > PCP_FREE_CALL_TP )
		{
		    callreq.callmode = PCP_FREE_CALL_TP;
		}
		psession->mode = callreq.callmode;
		psession->rc4_flag = is_local_rc4_enabled();
		psession->call_state = PCP_STATE_INVITE;
		psession->role = PCP_ANSWER;
		psession->meetingflag = callreq.meetingflag;
		psession->dsid = prt->dsid;
		psession->msg_type = prt->type;
        psession->atype = prt->atype;

		/*save video flag for event callback*/
		video_enabled = psession->video_flag;

        if ( pcp_is_tlv_enabled() )
        {
            strcpy(psession->fuid, callreq.fuid);
        }
        else
        {
#ifdef WEBRTC_LINUX
	        sprintf(psession->fuid, "%ld", prt->fuid);
#else
	        sprintf(psession->fuid, "%lld", prt->fuid);
#endif            
        }
		strcpy(psession->fuserid, callreq.fuserid);
		strcpy(psession->fphone, callreq.fphone);
		strcpy(psession->tuid, callreq.tuid);
		strcpy(psession->tuserid, callreq.tuserid);
		strcpy(psession->tphone, callreq.tphone);
		strncpy(psession->user_data, callreq.user_data, sizeof(psession->user_data) -1);

		get_best_mediastream(psession->role, AUDIO, psession);
		if (video_enabled)
			get_best_mediastream(psession->role, VIDEO, psession);

		/*trace call log*/
		trace_invite_log(psession);

		/* reset call info report */
		pcp_reset_call_info_report();
		
		/*store call param*/
		update_call_param(psession);

		/*callback state changed*/
		call_state_cb(psession->call_state);
	}

drop_out:
	ulock_session();/*unlock session*/
	
	/*call back incoming call event*/
	res_st(&callpm, sizeof(callpm));
	callpm.calltype = PCP_FREE_CALL_TP;
	callpm.videoflag = video_enabled;

	callpm.audiofec = fec_enabled(AUDIO, callreq.sflag);
    /* add by Vinton.Liu on 2016-05-30 for video fec anc nack config begin */
    callpm.videofec = fec_enabled(VIDEO, callreq.sflag);
    callpm.videonack = nack_enabled(VIDEO, callreq.sflag);
    /* Vinton.Liu add end */
	callpm.directfec = direct_fec_enabled(AUDIO, callreq.sflag);
	callpm.directmulriplefec = direct_mulriple_fec_enabled(AUDIO, callreq.sflag);//add by wuzhaoyang20151228
	callpm.meetingflag = callreq.meetingflag;
	strcpy(callpm.callid, callreq.call_id);
	strcpy(callpm.fphone, callreq.fphone);

    if ( pcp_is_tlv_enabled() )
    {
        strcpy(callpm.fuid, callreq.fuid);
    }
    else
    {
#ifdef WEBRTC_LINUX
        sprintf(callpm.fuid, "%ld", prt->fuid);
#else
        sprintf(callpm.fuid, "%lld", prt->fuid);
#endif            
    }
	strcpy(callpm.fuserid, callreq.fuserid);
	strcpy(callpm.tuserid, callreq.tuserid);
	strcpy(callpm.tuid, callreq.tuid);
	strcpy(callpm.tphone, callreq.tphone);
	strcpy(callpm.fnickname, callreq.fnickname);/*added by luochengcong 20141110*/
	strcpy(callpm.user_data, callreq.user_data);/*added by wuzhaoyang 20150202*/
	callpm.ucalltype = callreq.calltype;/*added by luochengcong 20140916*/
    
    if (isConflict)
    {
        if ( (res = pcp_build_call_answer(NULL)) == RES_ERR )
        {
            ms_error("uc_call_answer failed.");
        }
    }
    else
    {
        report_call_event(PCP_CALLINCOMING_EV, reason, (void *)&callpm, PCP_CALL_PARAM_SIZE);
    }
	return PCP_OK;
}

int on_call_rsp(const pcp_route_t* prt, const str_t* bd)
{
	char tmp_str[128] = {0};
	pcp_call_rsp_t callrsp;
	pcp_session_t* psession = NULL;
	int res = 0;
	int errcode = 0;

	if (prt == NULL || bd == NULL || bd->sbuf == NULL || bd->slen == 0)
	{
		ms_warning("on_call_rsp:msg error!!");
		return PCP_ERR;
	}

	res_st(&callrsp, sizeof(callrsp));
	/*store remote sdp and parser call rsp*/
	callrsp.sdp = create_sdp(0, 0);
	res_st(callrsp.sdp, sizeof(pcp_sdp_info_t));

    if (!pcp_is_tlv_enabled())
    {
	    res = parser_call_rsp_json_body((str_t*)bd, &callrsp);
	}
#if defined(SKT_PROTOCOL_SUPPORT_ENABLED)
    else
    {
        res = pcp_skt_parse_call_rsp_msg((str_t*)bd, &callrsp);
    }
#endif
	if (res < 0)
	{
		ms_error("on_call_rsp: maybe msg is err!!!");
		free_sdp(callrsp.sdp);
		return PCP_ERR;
	}

	lock_session();

	/*check rsp info if match for current session info*/
	psession = get_session(callrsp.call_id);
	if (psession == NULL)
	{
		ms_error("call rsp msg is not match for callid[%s]!!!", callrsp.call_id);
		ulock_session();
		free_sdp(callrsp.sdp);
		return PCP_ERR;
	}

	if (psession->call_state != PCP_STATE_INVITE 
		&& psession->call_state != PCP_STATE_RINGING)
	{
		ms_error("call rsp msg is not match for state!!!");
		ulock_session();
		free_sdp(callrsp.sdp);
		return PCP_ERR;
	}

	if (psession->role != PCP_OFFER)
	{
		ms_error("call rsp msg is not match for role!!!");
		ulock_session();
		free_sdp(callrsp.sdp);
		return PCP_ERR;
	}
    
	/*if rsp success, check media have consultation*/
	errcode = callrsp.reason;
	if (issuccess(errcode))
	{
		/*check media sdp have consultation, if not, this we make consultation*/
		if (psession->lsdp != NULL && check_sdp_availably(callrsp.sdp))
		{
            /* free ring rsp sdp if exist */
            if ( psession->rsdp != NULL )
            {
                free_sdp(psession->rsdp);
            }
            
			res = media_sdp_consultation(psession->lsdp, callrsp.sdp);
			if (res == PCP_BOTH_CONSULT_FAILD)
			{
				errcode = PCP_NOTACCEPT;
				ms_error("media sdp consultation error!!");
			}
			else
			{
				/*store remote sdp info*/
				psession->rsdp = callrsp.sdp;
				psession->audio_flag = media_enabled(AUDIO, res);
				psession->video_flag = media_enabled(VIDEO, res);
				psession->afec_flag = fec_enabled(AUDIO, callrsp.sflag);
                /* add by Vinton.Liu on 2016-05-30 for video fec anc nack config begin */
                psession->vfec_flag = fec_enabled(VIDEO, callrsp.sflag);
                psession->vnack_flag = nack_enabled(VIDEO, callrsp.sflag);
                /* Vinton.Liu add end */
            	psession->directfec_flag = direct_fec_enabled(AUDIO, callrsp.sflag);
            	psession->directfec_mulriple_flag = direct_mulriple_fec_enabled(AUDIO, callrsp.sflag);//add by wuzhaoyang20151228
            	psession->intrans_flag = in_trans_enabled(callrsp.sflag);
            	psession->ice_flag = ice_enabled(callrsp.sdp->ice.cdd_num);
                psession->dsid = prt->dsid;
                
				get_best_mediastream(psession->role, AUDIO, psession);
				if (psession->video_flag)
					get_best_mediastream(psession->role, VIDEO, psession);

				/*store call param*/
				update_call_param(psession);

                /*update call state to connect*/
                psession->call_state = PCP_STATE_CONNECT;
			}
		}
		else
		{
		    /*update call state to connect*/
            psession->call_state = PCP_STATE_CONNECT;	
			free_sdp(callrsp.sdp);
		}
        
		/*callback state changed*/
		call_state_cb(psession->call_state);
	}
                
	/*trace call log*/
	trace_answer_log(psession, errcode);
	if (!issuccess(errcode))
		report_call_log(psession);
	
	/*if rsp error release session*/
	if (!issuccess(errcode))
	{
		free_sdp(callrsp.sdp);
		delete_session(psession);
	}

	ulock_session();

	/*stop all timer*/
	stop_all_timers();

	/*callback call rsp event*/
	report_call_event(PCP_CALLDIALING_EV, errcode, NULL, 0);

	return PCP_OK;

}

//add by wuzhaoyang20140605 start
int on_m_call_rsp(const pcp_route_t* prt, const str_t* bd)
{
	char tmp_str[128] = {0};
	pcp_call_rsp_t callrsp;
	pcp_session_t* psession = NULL;
	int res = 0;
	int errcode = 0;

	if (prt == NULL || bd == NULL || bd->sbuf == NULL || bd->slen == 0)
	{
		ms_warning("on_call_rsp:msg error!!");
		return PCP_ERR;
	}

	res_st(&callrsp, sizeof(callrsp));
	/*store remote sdp and parser call rsp*/
	callrsp.sdp = create_sdp(0, 0);
	res_st(callrsp.sdp, sizeof(pcp_sdp_info_t));
	res = parser_call_rsp_json_body((str_t*)bd, &callrsp);
	if (res < 0)
	{
		ms_error("on_m_call_rsp: maybe msg is err!!!");	
		free_sdp(callrsp.sdp);
		return PCP_ERR;
	}

	lock_session();

	/*check rsp info if match for current session info*/
	psession = get_session(callrsp.call_id);
	if (psession == NULL)
	{
		ms_error("call rsp msg is not match for callid[%s]!!!", callrsp.call_id);
		free_sdp(callrsp.sdp);
		ulock_session();
		return PCP_ERR;
	}

	if ( psession->conf_state == NULL )
	{
	    ms_error("on_m_call_rsp: not find conference for callid[%s].", callrsp.call_id);
	    free_sdp(callrsp.sdp);
	    pcp_release_call_session();
		ulock_session();
		return PCP_ERR;
	}

	/*store remote sdp and parser call rsp*/
	if ( psession->conf_state->conftype != E_PCP_CONF_TYPE_NORMAL )
	{
    	free_sdp(callrsp.sdp);
	}

    {
        strcpy(psession->conf_state->roomid, callrsp.roomid);
    }

	/*if rsp success, check media have consultation*/
	errcode = callrsp.reason;
	if (issuccess(errcode))
	{
	    if ( psession->conf_state->conftype == E_PCP_CONF_TYPE_NORMAL )
	    {
    		/*check media sdp have consultation, if not, this we make consultation*/
    		if (psession->rsdp == NULL && check_sdp_availably(callrsp.sdp))
    		{
    			res = media_sdp_consultation(psession->lsdp, callrsp.sdp);
    			if (res == PCP_BOTH_CONSULT_FAILD)
    			{
    				errcode = PCP_NOTACCEPT;
    				ms_error("media sdp consultation error!!");
    			}
    			else
    			{
    				/*store remote sdp info*/
    				psession->rsdp = callrsp.sdp;
    				psession->audio_flag = media_enabled(AUDIO, res);
    				psession->video_flag = media_enabled(VIDEO, res);
    				psession->ice_flag = ice_enabled(callrsp.sdp->ice.cdd_num);
                    //psession->call_state = PCP_STATE_CONNECT;
    				psession->conf_state->pstate[0].call_state = PCP_STATE_CONNECT;
    				psession->dsid = prt->dsid;

    				get_best_mediastream(psession->role, AUDIO, psession);
    				if (psession->video_flag)
    					get_best_mediastream(psession->role, VIDEO, psession);
    			}
    		}
    		else
    		{
    			/*update call state to connect*/
    			free_sdp(callrsp.sdp);
    			//psession->call_state = PCP_STATE_CONNECT;
    			psession->conf_state->pstate[0].call_state = PCP_STATE_CONNECT;
    		}
        }
        
        psession->call_state = PCP_STATE_CONNECT;
        
		/*store call param*/		
    	update_call_param(psession);
	}

	/*trace call log*/
	trace_answer_log(psession, errcode);
	if (!issuccess(errcode))
		report_call_log(psession);

	/*if rsp error release session*/
	if (!issuccess(errcode))
	{
		free_sdp(callrsp.sdp);
		if (psession->call_state != PCP_STATE_CONNECT || 
			psession->conf_state->pstate[0].call_state != PCP_STATE_CONNECT)
		{
			delete_session(psession);
		}
	}

	ulock_session();

	/*stop timer*/
	stop_timer(TIMEOUT_T4);

	/*callback call rsp event*/
	report_call_event(PCP_CONFERENCE_EV, errcode, NULL, 0);

	return PCP_OK;
}

int on_m_notify(const pcp_route_t* prt, const str_t* bd)
{
	pcp_conf_ntfy_msg_t mnotify;
	pcp_conf_ntfy_state_t  ntfy_state;
	pcp_session_t* psession = NULL;
	int res = 0;
	int reason = 0;
	int i;

	if (prt == NULL || bd == NULL || bd->sbuf == NULL || bd->slen == 0)
	{
		ms_warning("on_m_notify:msg error!!");
		return PCP_ERR;
	}

	/*parser call ack*/
	res_st(&mnotify, sizeof(mnotify));
	res = parser_m_notify_json_body((str_t*)bd, &mnotify);
	if (res < 0)
	{
		ms_error("on_m_notify: maybe msg is err!!!");
		return PCP_ERR;
	}

	lock_session();

	/*check rsp info if match for current session info*/
	psession = get_session(mnotify.callid);
	if (psession == NULL)
	{
		ms_error("on_m_notify: meet notify msg is not match for callid!!!");
		ulock_session();
		return PCP_ERR;
	}

	//update conference state 
	if (psession->conf_state == NULL)
	{
		psession->conf_state = create_conference();
		psession->conf_state->roomstate = E_PCP_CONF_STATE_CREATED;
	}
	psession->conf_state->roomstate = mnotify.roomstate;
	strcpy(psession->conf_state->conference_id, mnotify.callid);
	strcpy(psession->conf_state->roomid, mnotify.roomid);	
	psession->conf_state->pnum = mnotify.parties.pnum;
	for (i = 0; i < mnotify.parties.pnum; i++)
	{
        /* add by VintonLiu on 20150204 for filter offline participant */
        if ( (mnotify.parties.pstate[i].call_state < PCP_STATE_WAIT) 
            || (mnotify.parties.pstate[i].call_state > PCP_STATE_CONNECT) )
        {
            psession->conf_state->pnum--;
            continue;
        }
        /* VintonLiu add end */
        
		psession->conf_state->pstate[i].call_state = 
			mnotify.parties.pstate[i].call_state;
		psession->conf_state->pstate[i].reason = 
			mnotify.parties.pstate[i].reason;
		strcpy(psession->conf_state->pstate[i].uid,
			mnotify.parties.pstate[i].uid);
		strcpy(psession->conf_state->pstate[i].phone,
			mnotify.parties.pstate[i].phone);
	}

    res_st(&ntfy_state, UGO_CONF_NTFY_STATE_SIZE);
    ntfy_state.roomstate = mnotify.roomstate;
	ntfy_state.pnum = mnotify.parties.pnum;
	for (i = 0; i < ntfy_state.pnum; i++)
	{
		ntfy_state.pstate[i].call_state = mnotify.parties.pstate[i].call_state;
		ntfy_state.pstate[i].reason = mnotify.parties.pstate[i].reason;
		strcpy(ntfy_state.pstate[i].uid, mnotify.parties.pstate[i].uid);
		strcpy(ntfy_state.pstate[i].phone, mnotify.parties.pstate[i].phone);
	}

	/*trace call log*/
	//trace_m_notify_log(psession);
	update_call_param(psession);

	report_call_event(PCP_CONFERENCE_EV, PCP_GETSTATE, \
	                (void *)&ntfy_state, UGO_CONF_NTFY_STATE_SIZE);

   	ulock_session(); 
	return PCP_OK;
}

int on_convert_conference(const pcp_route_t* prt, const str_t* bd)
{
	char callid[64] = {0};
	pcp_session_t* psession = NULL;
	int reason = 0;
	int res = 0;
	int ice_mode = 0;

	if (prt == NULL || bd == NULL || bd->sbuf == NULL || bd->slen == 0)
	{
		ms_warning("on_convert_conference:msg error!!");
		return PCP_ERR;
	}

	/* parser convert conference */
	res = parser_convert_conference_json_body((str_t*)bd, callid);
	if (res < 0)
	{
		ms_error("on_convert_conference: maybe msg is err!!!");
		return PCP_ERR;
	}

	lock_session();

	/*check rsp info if match for current session info*/
	psession = get_session(callid);
	if (psession == NULL)
	{
		ms_error("on_convert_conference: not match for callid!!!");
		reason = 1;
		//build_convert_conference_rsp(reason, callid, prt);
		ulock_session();
		return PCP_ERR;
	}

	if (psession->call_state != PCP_STATE_CONNECT)
	{
		ms_error("on_convert_conference: is not match for state!!!");
		reason = 1;
		//build_convert_conference_rsp(reason, callid, prt);
		ulock_session();
		return PCP_ERR;
	}	
	
	if ( psession->conf_state == NULL )
	{
		psession->conf_state = create_conference();
		strcpy(psession->conf_state->conference_id, callid);
	    psession->conf_state->conftype = E_PCP_CONF_TYPE_NORMAL;
	}
	psession->meetingflag = 1;
	//build_convert_conference_rsp(reason, callid, prt);

	/*trace call log*/
	trace_convert_conference_log(psession);
	//psession->ice_flag = false;

	ulock_session();

	report_call_event(PCP_CONFERENCE_EV, PCP_CALLEE_CHANGEMODE, NULL, 0);

	return PCP_OK;
}

int on_m_inv_rsp(const pcp_route_t* prt, const str_t* bd)
{
	pcp_conf_nml_rsp_t invRsp;
	pcp_session_t* psession = NULL;
	int res = 0;
	int errcode = 0;

	if (prt == NULL || bd == NULL || bd->sbuf == NULL || bd->slen == 0)
	{
		ms_warning("on_m_inv_rsp:msg error!!");
		return PCP_ERR;
	}

	res_st(&invRsp, sizeof(invRsp));
	res = parser_m_nml_rsp_json_body((str_t*)bd, &invRsp);
	if (res < 0)
	{
		ms_error("on_m_inv_rsp:maybe msg is err!!!");
		return PCP_ERR;
	}

	lock_session();

	/*check rsp info if match for current session info*/
	psession = get_session(invRsp.callId);
	if (psession == NULL)
	{
		ms_error("on_m_inv_rsp: not match for callid[%s]!!!", invRsp.callId);
		ulock_session();
		return PCP_ERR;
	}

    strcpy(psession->conf_state->roomid, invRsp.roomId);
    update_call_param(psession);

    errcode = invRsp.errCode;

	ulock_session();

	/*stop timer*/
	stop_timer(TIMEOUT_T4);

	return PCP_OK;
}

int on_m_del_rsp(const pcp_route_t* prt, const str_t* bd)
{
    pcp_conf_nml_rsp_t delRsp;
	pcp_session_t* psession = NULL;
	int res = 0;
	int errcode = 0;

	if (prt == NULL || bd == NULL || bd->sbuf == NULL || bd->slen == 0)
	{
		ms_warning("on_m_del_rsp:msg error!!");
		return PCP_ERR;
	}

	res_st(&delRsp, sizeof(delRsp));
	res = parser_m_nml_rsp_json_body((str_t*)bd, &delRsp);
	if (res < 0)
	{
		ms_error("on_m_del_rsp:maybe msg is err!!!");
		return PCP_ERR;
	}

	lock_session();

	/*check rsp info if match for current session info*/
	psession = get_session(delRsp.callId);
	if (check_session(psession) < 0)
	{
		ms_error("on_m_del_rsp:Have No Usable Session !!!");
		ulock_session();
		return PCP_ERR;
	}

    strcpy(psession->conf_state->roomid, delRsp.roomId);

    /*update session to expired*/
	psession->expired_flag = TRUE;
	
	trace_hangup_log(psession, PCP_HUNGUPPEER);
	report_call_log(psession);

	/*upade call state to wait*/
	psession->call_state = PCP_STATE_WAIT;
	call_state_cb(psession->call_state);
	
	/*delete session */
	delete_session(psession);
	
	/*stop all timer*/
	stop_all_timers();

	ulock_session();

//	report_call_event(PCP_CALLHUNGUP_EV, delRsp.errCode, NULL, 0);

	return PCP_OK;
}
//add by wuzhaoyang20140605 end

int on_call_ack(const pcp_route_t* prt, const str_t* bd)
{
	pcp_call_ack_t callack;
	pcp_session_t* psession = NULL;
	int res = 0;
	
	if (prt == NULL || bd == NULL || bd->sbuf == NULL || bd->slen == 0)
	{
		ms_warning("on_call_ack:msg error!!");
		return PCP_ERR;
	}

	/*parser call ack*/
	res_st(&callack, sizeof(callack));

    if (!pcp_is_tlv_enabled())
    {
	    res = parser_call_ack_json_body((str_t*)bd, &callack);
	}
#if defined(SKT_PROTOCOL_SUPPORT_ENABLED)
    else
    {
        res = pcp_skt_parse_call_ack_req_msg((str_t *)bd, &callack);
    }
#endif
	if (res < 0)
	{
		ms_error("parser_call_ack_json_body:maybe msg is err!!!");
		return PCP_ERR;
	}

	lock_session();

	/*check rsp info if match for current session info*/
	psession = get_session(callack.call_id);
	if (psession == NULL)
	{
		ms_error("call rsp msg is not match for callid!!!");
		ulock_session();
		return PCP_ERR;
	}

	if (psession->call_state != PCP_STATE_CONNECT)
	{
		ms_error("call rsp msg is not match for state!!!");
		ulock_session();
		return PCP_ERR;
	}

	if (psession->role != PCP_ANSWER)
	{
		ms_error("call rsp msg is not match for role!!!");
		ulock_session();
		return PCP_ERR;
	}

	/*trace call log*/
	trace_ack_log(psession);
	
	ulock_session();
	
	/*stop all timers*/
	stop_all_timers();

	return PCP_OK;

}

int on_call_bye(const pcp_route_t* prt, const str_t* bd)
{
	pcp_bye_req_t byereq;
	pcp_session_t* psession = NULL;
	int res = 0;

	if (prt == NULL || bd == NULL || bd->sbuf == NULL || bd->slen == 0)
	{
		ms_warning("on_call_bye:msg error!!");
		return PCP_ERR;
	}

	/*parser call ack*/
	res_st(&byereq, sizeof(byereq));
	
    if (!pcp_is_tlv_enabled())
    {
	    res = parser_bye_req_json_body((str_t*)bd, &byereq);
	}
#if defined(SKT_PROTOCOL_SUPPORT_ENABLED)
    else
    {
        res = pcp_skt_parse_bye_req_msg((str_t*)bd, &byereq);
    }
#endif
	if (res < 0)
	{
		ms_error("parser_bye_req_json_body:maybe msg is err!!!");
		return PCP_ERR;
	}

	lock_session();

	/*check bye req info if match for current session info*/
	psession = get_session(byereq.call_id);
	if (check_session(psession) < 0)
	{
		ms_error("on_call_bye:Have No Usable Session !!!");
		ulock_session();
		return PCP_ERR;
	}
	
	trace_hangup_log(psession, PCP_HUNGUPPEER);
	
    /*stop all timer*/
    stop_all_timers();

	ulock_session();
	
	/*callback bye event*/
#if 0	
	if (PCP_HUNGUPMYSELF <= byereq.reason
		&& PCP_HUNGUPRTPTIMEOUT >= byereq.reason)
	{
		byereq.reason = PCP_HUNGUPPEER;
	}
#else
    byereq.reason = PCP_HUNGUPPEER;
#endif

	report_call_event(PCP_CALLHUNGUP_EV, byereq.reason, NULL, 0);

	return PCP_OK;
}

int on_update_req(const pcp_route_t* prt, const str_t* bd)
{
	pcp_bool update_flag = false;
	pcp_update_req_t updatereq;
	pcp_update_rsp_t updatersp;
	pcp_session_t* psession = NULL;
	char rsp_apt[64]={0};
	char rsp_vpt[64]={0};
	int res = 0;
	int errnum = PCP_OK;

	if (prt == NULL || bd == NULL || bd->sbuf == NULL || bd->slen == 0)
	{
		ms_warning("on_update_req:msg error!!");
		return PCP_ERR;
	}
	
	res_st(&updatereq, sizeof(updatereq));
	res_st(&updatersp, sizeof(updatersp));

    if ( !pcp_is_tlv_enabled() )
    {
	    res = parser_update_req_json_body((str_t*)bd, &updatereq);
	}
#if defined(SKT_PROTOCOL_SUPPORT_ENABLED)
    else
    {
        res = pcp_skt_parse_update_req_msg((str_t*)bd, &updatereq);
    }
#endif
	if (res < 0)
	{
		ms_error("parser_update_req_json_body:maybe msg is err!!!");
		return PCP_ERR;
	}

	lock_session();

	/*check bye req info if match for current session info*/
	psession = get_session(updatereq.call_id);
	
	//luochengcong 20141/12/30
	if (psession == NULL)
	{
		ms_error("call update msg is not match for callid!!!");
		ulock_session();
		updatersp.ice_mode = 0;
		updatersp.reason = PCP_UPDATEMEDIAFAIL;
		strcpy(updatersp.call_id ,updatereq.call_id);
		build_call_update_rsp(&updatersp, prt);
		return PCP_ERR;
	}
	
	if (psession->call_state != PCP_STATE_CONNECT)
	{
		ms_error("call update msg is not match for state!!!");
		ulock_session();
		updatersp.ice_mode = 0;
		updatersp.reason = PCP_UPDATEMEDIAFAIL;
		strcpy(updatersp.call_id ,updatereq.call_id);
		build_call_update_rsp(&updatersp, prt);
		return PCP_ERR;
	}
	if ( (psession->meetingflag == 1)
	    && ( ( psession->role == PCP_ANSWER)  
	      || (psession->conf_state->conftype == E_PCP_CONF_TYPE_NORMAL)) )
	{
        update_flag = true;
		errnum= pcp_update_media_consultion(&updatereq, &updatersp, psession);
	}
		
	/*update relay info*/
	if (updatereq.aport != 0 && updatereq.aport != psession->relay.a_port)
	{
		psession->relay.a_port = updatereq.aport;
		update_flag = true;
	}
	if (updatereq.vport != 0 && updatereq.vport != psession->relay.v_port)
	{
		psession->relay.v_port = updatereq.vport;
		update_flag = true;
	}
	if (strlen(updatereq.mip) != 0 && strcmp(psession->relay.m_ip, updatereq.mip) != 0)
	{
		if ( PCP_OK == check_ipaddr(updatereq.mip) )
        {
            update_flag = true;
    		strcpy(psession->relay.m_ip, updatereq.mip);
        }
        else
        {
            errnum = PCP_ERR;            
        }
	}

	if (errnum == PCP_ERR)
	{
		update_flag = false;
	}
	
	ulock_session();

	/*send update success*/	
	updatersp.ice_mode = 0;
	updatersp.reason = errnum == PCP_OK ? PCP_SUCCESS : PCP_UPDATEMEDIAFAIL;;
	strcpy(updatersp.call_id ,updatereq.call_id);
	build_call_update_rsp(&updatersp, prt);

	if (update_flag)
	{
		/*callback call update event*/
		report_call_event(PCP_CALLUPDATE_EV, PCP_SUCCESS, NULL, 0);
	}

	return PCP_OK;
}

int on_ring_rsp(const pcp_route_t* prt, const str_t* bd)
{
	pcp_ring_rsp_t ringrsp;
	pcp_session_t* psession = NULL;
	int res = 0;
	
	if (prt == NULL || bd == NULL || bd->sbuf == NULL || bd->slen == 0)
	{
		ms_warning("on_ring_rsp:msg error!!");
		return PCP_ERR;
	}

	res_st(&ringrsp, sizeof(ringrsp));
	/*store remote sdp and parser call rsp*/
	ringrsp.sdp = create_sdp(0, 0);
	res_st(ringrsp.sdp, sizeof(pcp_sdp_info_t));

    if (!pcp_is_tlv_enabled())
    {
	    res = parser_ringing_rsp_json_body((str_t*)bd, &ringrsp);
	}
#if defined(SKT_PROTOCOL_SUPPORT_ENABLED)
    else
    {
        res = pcp_skt_parse_ring_rsp_msg((str_t*)bd, &ringrsp);
    }
#endif
	if (res < 0)
	{
		ms_error("parser_ringing_rsp_json_body:maybe msg is err!!!");
		free_sdp(ringrsp.sdp);
		return PCP_ERR;
	}

	lock_session();

	/*check ring rsp info if match for current session info*/
	psession = get_session(ringrsp.call_id);
	if (psession == NULL)
	{
		ms_error("call rsp msg is not match for callid!!!");
		ulock_session();
		free_sdp(ringrsp.sdp);
		return PCP_ERR;
	}

	if (psession->call_state != PCP_STATE_INVITE)
	{
		ms_error("call rsp msg is not match for state!!!");
		ulock_session();
		free_sdp(ringrsp.sdp);
		return PCP_ERR;
	}
    //ms_report("wuzhaoyang on_ring_rsp ring rsp pt = %d", ringrsp.sdp->media.media_arr->pt);
	/* media consultation*/
	if (psession->lsdp != NULL && check_sdp_availably(ringrsp.sdp))
	{
		res = media_sdp_consultation(psession->lsdp, ringrsp.sdp);
		if (res == PCP_BOTH_CONSULT_FAILD)
		{
			free_sdp(ringrsp.sdp);
		}
		else
		{
			psession->rsdp = ringrsp.sdp;
			psession->audio_flag = media_enabled(AUDIO, res);
			psession->video_flag = media_enabled(VIDEO, res);
			psession->afec_flag = fec_enabled(AUDIO, 0);/*default fec is closed--call rsp we will open it*/
            /* add by Vinton.Liu on 2016-05-30 for video fec anc nack config begin */
            psession->vfec_flag = fec_enabled(VIDEO, ringrsp.sflag);
            psession->vnack_flag = nack_enabled(VIDEO, ringrsp.sflag);
            /* Vinton.Liu add end */
			psession->directfec_flag = direct_fec_enabled(AUDIO, 0);
			psession->directfec_mulriple_flag = direct_mulriple_fec_enabled(AUDIO, 0);//add by wuzhaoyang20151228
			psession->ice_flag = ice_enabled(ringrsp.sdp->ice.cdd_num);
            psession->intrans_flag = in_trans_enabled(ringrsp.sflag);
			psession->call_state = PCP_STATE_RINGING;
			psession->dsid = prt->dsid;

			get_best_mediastream(psession->role, AUDIO, psession);
			if (psession->video_flag)
				get_best_mediastream(psession->role, VIDEO, psession);

            pcp_update_ctime();
			/*store call param*/
			update_call_param(psession);

			/*callback state changed*/
			call_state_cb(psession->call_state);
		}
	}
	else
	{
		free_sdp(ringrsp.sdp);
	}

	/*trace call log*/
	trace_ringing_log(psession);

	ulock_session();

	/*stop timer t0 and start timer t1*/
	stop_timer(TIMEOUT_T0);
	start_timer(TIMEOUT_T1);
	
	/*callback call ringing event*/
	report_call_event(PCP_CALLDIALING_EV, PCP_RINGING, NULL, 0);

	return PCP_OK;
}

int on_direct_notify(const pcp_route_t* prt, const str_t* bd)
{
	int res = 0;
	pcp_session_t* psession = NULL;
	pcp_direct_notify_t dn;
	
	if (prt == NULL)
	{
		ms_warning("on_direct_notify:msg error!!");
		return PCP_ERR;
	}

	if (isbusy())
	{
		/*parser direct notify*/
		res_st(&dn, sizeof(dn));

        if (!pcp_is_tlv_enabled())
        {
		    res = parser_direct_notify_json_body((str_t*)bd, &dn);
		}
#if defined(SKT_PROTOCOL_SUPPORT_ENABLED)
        else
        {
            res = pcp_skt_parse_direct_ntfy_msg((str_t*)bd, &dn);
        }
#endif
		if (res < 0)
		{
			ms_error("parser_direct_notify_json_body:maybe msg is err!!!");
			return PCP_ERR;
		}
	
		lock_session();
		
		psession = get_session(dn.call_id);
		if (check_session(psession) < 0)
		{
			ms_error("on_direct_notify:Have No Usable Session !!!");
			ulock_session();
			return PCP_ERR;
		}

		/*trace call log*/
		trace_notify_log(psession);
		pm_update_call_type(PCP_DIRECT_CALL_TP);

		/*update call mode*/
		psession->mode = PCP_DIRECT_CM;
		
		ulock_session();

		/*callback direct notify event*/
		report_call_event(PCP_CALLDIALING_EV, convert_notify_reason(dn.reason), NULL, 0);
	}

	return PCP_OK;
}

int on_callreq_ack(const pcp_route_t* prt, const str_t* bd)
{
	int res = 0;
	pcp_session_t* psession = NULL;
	
	if (prt == NULL)
	{
		ms_warning("on_callreq_ack:msg error!!");
		return PCP_ERR;
	}

	if (isbusy())
	{
		lock_session();
		/*trace call log*/
		psession = get_session(NULL);
		if (psession != NULL)
		{
			psession->dsid = prt->dsid;
			trace_callreq_ack_log(psession);
		}
		ulock_session();
	}

	return PCP_OK;
}

int on_recv_dtmf(const pcp_route_t* prt, const str_t* bd)
{
    int res = PCP_OK;
    pcp_session_t* psession = NULL;
    pcp_dtmf_info_t dtmf_info;
    pcp_dtmf_event_node_t dtmf_ev = { 0 };

    if ( prt == NULL || bd == NULL || bd->sbuf == NULL || bd->slen == 0 )
    {
        ms_warning("on_recv_dtmf: msg error!!");
        return PCP_ERR;
    }

    //ms_report("on_recv_dtmf: %s", bd->sbuf);
    
    res_st(&dtmf_info, sizeof(pcp_dtmf_info_t));

    if (!pcp_is_tlv_enabled())
    {
        res = parser_dtmf_info_json_body((str_t*)bd, &dtmf_info);
    }
#if defined(SKT_PROTOCOL_SUPPORT_ENABLED)
    else
    {
        res = pcp_skt_parse_dtmf_info_msg((str_t*)bd, &dtmf_info);
    }
#endif
    if (res < 0)
	{
		ms_error("on_recv_dtmf: maybe msg is err!!!");
		
		return PCP_ERR;
	}

	lock_session();
	/*check ring rsp info if match for current session info*/
	psession = get_session(dtmf_info.call_id);
	if (psession == NULL)
	{
		ms_error("on_recv_dtmf: not find match session!!!");
		ulock_session();
		return PCP_ERR;
	}

	if ( psession->call_state != PCP_STATE_CONNECT )
	{
	    ms_error("on_recv_dtmf: get dtmf not in connect state!!!");
		ulock_session();
		return PCP_ERR;
	}

	dtmf_ev.dtmf_int = dtmf_info.dtmf_int;
	dtmf_ev.dtmf_str[0] = dtmf_info.dtmf_str[0];

    report_call_event(PCP_DTMF_EV, dtmf_ev.dtmf_int, &dtmf_ev, PCP_DTMF_EVENT_NODE_SIZE);
	ulock_session();

	return PCP_OK;
}

/* add by Vinton.Liu on 2016-04-06 for add internal state passthrough signalling begin */
int on_in_trans_req(const pcp_route_t* prt, const str_t* bd)
{
    int res = PCP_OK;
    pcp_session_t* psession = NULL;
    pcp_in_trans_req_t in_trans_req = { 0 };
    pcp_call_trans_state_t trans_state = { 0 };

    if ( prt == NULL || bd == NULL || bd->sbuf == NULL || bd->slen == 0 )
    {
        ms_warning("on_in_trans_req: msg error!!");
        return PCP_ERR;
    }

    if (!pcp_is_tlv_enabled())
    {
        // not support
        return PCP_OK;
    }
#if defined(SKT_PROTOCOL_SUPPORT_ENABLED)
    else
    {
        res = pcp_skt_parse_in_trans_req_msg((str_t*)bd, &in_trans_req);
    }
#endif
    if (res < 0)
	{
		ms_error("on_in_trans_req: maybe msg is err!!!");
		
		return PCP_ERR;
	}

	lock_session();
	/*check ring rsp info if match for current session info*/
	psession = get_session(in_trans_req.call_id);
	if (psession == NULL)
	{
		ms_error("on_in_trans_req: not find match session!!!");
		ulock_session();
		return PCP_ERR;
	}

    trans_state.vie_angle = in_trans_req.vie_angle;
    trans_state.vie_state = in_trans_req.vie_state;
    trans_state.p2p_state = in_trans_req.p2p_state;
    trans_state.mic_state = in_trans_req.mic_state;
    trans_state.net_state = in_trans_req.net_state;

    report_call_event(PCP_IN_TRANS_REQ_EV, PCP_SUCCESS, 
                        &trans_state, PCP_CALL_TRANS_STATE_T_SIZE);
    ulock_session();

    return PCP_OK;
}

int on_in_trans_rsp(const pcp_route_t* prt, const str_t* bd)
{
    int res = PCP_OK;
    pcp_session_t* psession = NULL;
    pcp_in_trans_rsp_t in_trans_rsp = { 0 };

    if ( prt == NULL || bd == NULL || bd->sbuf == NULL || bd->slen == 0 )
    {
        ms_warning("on_in_trans_rsp: msg error!!");
        return PCP_ERR;
    }

    if (!pcp_is_tlv_enabled())
    {
        // not support
        return PCP_OK;
    }
#if defined(SKT_PROTOCOL_SUPPORT_ENABLED)
    else
    {
        res = pcp_skt_parse_in_trans_rsp_msg((str_t*)bd, &in_trans_rsp);
    }
#endif
    if (res < 0)
	{
		ms_error("on_in_trans_rsp: maybe msg is err!!!");
		
		return PCP_ERR;
	}

	lock_session();
	/*check ring rsp info if match for current session info*/
	psession = get_session(in_trans_rsp.call_id);
	if (psession == NULL)
	{
		ms_error("on_in_trans_rsp: not find match session!!!");
		ulock_session();
		return PCP_ERR;
	}

    report_call_event(PCP_IN_TRANS_RSP_EV, in_trans_rsp.errcode, NULL, 0);
	ulock_session();
    stop_timer(TIMEOUT_T5);
    
	return PCP_OK;
}
/* Vinton.Liu add end */

/* add by Vinton.Liu on 2016-05-04 for unsupport op rsp err begin */
int on_unsupport_req(const pcp_route_t* prt)
{
    if ( NULL == prt )
    {
        ms_warning("on_unsupport_req: msg error!!");
    }

    return build_unsupport_rsp_err(prt);
}
/* Vinton.Liu add end */

int pcp_recv_message(const char* msg, int slen)
{
	char decrypted = 0;
	str_t sm, hd, bd;
	str_t deComBuf;
	pcp_route_t rt;
	
	if ( NULL == msg || slen == 0 )
	{
		return PCP_ERR;
	}
    
	res_st(&rt, sizeof(rt));
	res_st(&hd, sizeof(hd));
	res_st(&bd, sizeof(bd));
	res_st(&deComBuf, sizeof(deComBuf));

	sm.slen = slen;
	sm.sbuf = (char*)msg;

    //ms_report("pcp_recv_message: tlv_enabled[%d]", pcp_is_tlv_enabled());
    if (!pcp_is_tlv_enabled())
    {
    	if (parser_msg(&sm, &hd, &bd) < 0)
    	{
    		ms_error("parser_msg: msg is error!!");
    		return PCP_ERR;
    	}

    	if (hd.slen <= 0)
    	{
    		ms_error("parser_msg:hd.len=%d, maybe is keepalive msg!!", hd.slen);
    		return PCP_ERR;
    	}
    	
    	if (parser_route_bson_header(&hd, &rt) < 0)
    	{
    		ms_error("parser_route_bson_header: header msg is error!!");
    		return PCP_ERR;
    	}

        if ( (rt.type != PCP_CALL_TP)
            && (rt.type != PCP_CONF_TP)
            && (rt.type != PCP_CSRV_TP) )
    	{
    		if(bd.sbuf != NULL && bd.slen != 0)
    		{
    			if (rt.type == PCP_NON_TP)
    			{
    				ms_warning("recv non msg. op[%d] buff: [%.*s].", rt.op, bd.slen, bd.sbuf);
    				switch(rt.op)
    				{
    				case PCP_NON_VERSION_OP:
    					on_version_rsp(&rt, &bd);
    					break;
    				case PCP_NON_LOGIN_OP:
    					on_login_rsp(&rt, &bd);
    					break;
    				case PCP_NON_LOGOUT_OP:
    					on_kickout_notify(&rt, &bd);
    					break;
    				default:
    					ms_error("msg op[%d] is not match!!,drop it!!", rt.op);
    					break;
    				}	
    			}
    		}
    		else
    		{
    			ms_error("msg type[%d][%d] is not match!!,drop it!!", rt.type, rt.op);
    		}
    		return PCP_OK;
    	}
	}
#if defined(SKT_PROTOCOL_SUPPORT_ENABLED)
    else
    {
        PcpSktMsgHeader_t msgHeader;
        memset(&msgHeader, 0x00, PCP_SKT_MSG_HEADER_SIZE);
        if ( pcp_skt_parse_msg_header(&sm, &msgHeader, &hd) < 0 )
        {
            ms_error("pcp_recv_message: header msg is error!!");
    		return PCP_ERR;
        }

        if ( msgHeader.cmd == PCP_SKT_MSG_CMD_AUTH_RESP )
        {
            if ( pcp_skt_parse_auth_rsp_msg(&hd, &g_login) < 0)
            {
                ms_error("pcp_recv_message: parse auth rsp error.");
                return PCP_ERR;
            }

            rt.type = PCP_NON_TP;
            rt.op = PCP_NON_LOGIN_OP;
            if ( g_login.uid[0] != '\0' )
            {
                set_login_client_num(g_login.uid);
            }
            
            return PCP_OK;
        }
        else if ( msgHeader.cmd == PCP_SKT_MSG_CMD_REAUTH_RSP )
        {
            if ( pcp_skt_parse_reauth_rsp_msg(&hd, &g_login) < 0 )
            {
                ms_error("pcp_recv_message: parse reauth rsp error.");
                return PCP_ERR;
            }
            return PCP_OK;
        }
        else if ( msgHeader.cmd == PCP_SKT_MSG_CMD_NOOP_RSP )
        {
            ms_report("pcp_recv_message: recv heartbeat msg.");
            return PCP_OK;
        }
        else if ( (msgHeader.cmd == PCP_SKT_MSG_CMD_VOIP_CALL) 
                || (msgHeader.cmd == PCP_SKT_MSG_CMD_VOIP_CONF) 
                || (msgHeader.cmd == PCP_SKT_MSG_CMD_VOIP_CSRV))
        {
            PcpSktVoipBodyHeader_t bodyHeader;
            memset((void *)&bodyHeader, 0x00, PCP_SKT_VOIP_BODY_HEADER_SIZE);
            if ( pcp_skt_parse_voip_body(&hd, &bodyHeader, &bd) < 0 )
            {
                ms_error("pcp_recv_message: failed to parse voip body header.");
                return PCP_ERR;
            }
            if ( msgHeader.cmd == PCP_SKT_MSG_CMD_VOIP_CONF )
            {
                rt.type = PCP_CONF_TP;
            }
            else if (msgHeader.cmd == PCP_SKT_MSG_CMD_VOIP_CSRV)
            {
                rt.type = PCP_CSRV_TP;
            }
            else
            {
              rt.type = PCP_CALL_TP;
            }
            rt.ack = bodyHeader.ack;
            rt.atype = bodyHeader.atype;
            rt.dsid = bodyHeader.servid;
            rt.op = bodyHeader.op;
            rt.enc = bodyHeader.enc;

            ms_report("recv# type{%d} op{%d} atype{%d} enc{%d}", rt.type, rt.op, rt.atype, rt.enc);
        }
        else
        {
            return PCP_OK;
        }
    }
#endif

	if (rt.ack != PCP_ACK_NORMAL_RT)
	{
		on_nack(&rt);
		ms_warning("msg type[%d] nack[%d]!!!", rt.op, rt.ack);
		return PCP_OK;
	}
    
	/*RC4 decrypt*/
	if( (rt.enc & PCP_ENC_TYPE_RC4) && bd.slen != 0 && bd.sbuf != NULL )
	{
		decrypt_packet(bd.sbuf, &bd.slen, &decrypted, ENCRYPT_UGO);
	}

	if (!pcp_is_tlv_enabled())
	{
		ms_report("recv# type{%d} op{%d} %.*s", rt.type, rt.op, bd.slen, bd.sbuf);
        //printf("******recv op[%d]***************\n",rt.op);
	}
#if defined(SKT_PROTOCOL_SUPPORT_ENABLED)
    else if ( rt.op != PCP_CALLREQ_ACK_OP )
    {
        /* decompress */
        if ( rt.enc & PCP_ENC_TYPE_COMPRESS )
        {
            PcpSktCompress_t stComParam;
            memset(&stComParam, 0x00, sizeof(PcpSktCompress_t));
            stComParam.CompressAlgorithm = PCP_SKT_COMPRESS_ALG_ZLIB;
            stComParam.CompressVersion = PCP_SKT_COMPRESS_VER;

            str_init(&deComBuf, PCP_SKT_BODY_DATA_MAX_SIZE);
            if ( pcp_skt_decompress(&stComParam, &bd, &deComBuf) != PCP_OK )
            {
                ms_error("pcp_recv_message faild on decompress msg body.");
                str_release(&deComBuf);                
                return PCP_ERR;
            }
            
            ms_report("pcp_recv_message decompress successed.");
            bd.sbuf = deComBuf.sbuf;
            bd.slen = deComBuf.slen;
        }
    }
#endif	

	switch(rt.op)
	{
		case PCP_CALL_REQ_OP:
			on_call_req(&rt, &bd);
			break;
		case PCP_CALL_RSP_OP:
			on_call_rsp(&rt, &bd);
			break;
		case PCP_CALL_ACK_OP:
			on_call_ack(&rt, &bd);
			break;
		case PCP_CALL_BYE_OP:
			on_call_bye(&rt, &bd);
			break;
		case PCP_UPDATE_REQ_OP:
			on_update_req(&rt, &bd);
			break;
		case PCP_RING_RSP_OP:
			on_ring_rsp(&rt, &bd);
			break;
		case PCP_DIR_NOTIFY_OP:
			on_direct_notify(&rt, &bd);
			break;
		case PCP_CALLREQ_ACK_OP:
			on_callreq_ack(&rt, &bd);
			break;
		case PCP_DTMF_INFO_OP:
		    on_recv_dtmf(&rt, &bd);
		    break;
        /* add by Vinton.Liu on 2016-04-05 for add internal state passthrough signalling begin */
		case PCP_IN_TRANS_REQ_OP:
            on_in_trans_req(&rt, &bd);
		    break;
		case PCP_IN_TRANS_RSP_OP:
            on_in_trans_rsp(&rt, &bd);
		    break;
        /* Vinton.Liu add end */
		case PCP_MEET_CALL_RSP_OP://add by wuzhaoyang20140605 start
			on_m_call_rsp(&rt, &bd);
			break;
		case PCP_MEET_CALL_NOTIFY_OP:
			on_m_notify(&rt, &bd);
			break;
		case PCP_CONVERT_CONFERENCE_OP:
			on_convert_conference(&rt, &bd);
			break;//add by wuzhaoyang20140605 end
		case PCP_MEET_INV_RSP_OP:
		    on_m_inv_rsp(&rt, &bd);
		    break;
		case PCP_MEET_DEL_RSP_OP:
		    on_m_del_rsp(&rt, &bd);
		    break;
		default:
			ms_error("msg type[%d] not support!!!", rt.op);
            /* add by Vinton.Liu on 2016-05-04 for unsupport op rsp err begin */
            on_unsupport_req(&rt);
            /* Vinton.Liu add end */
			break;
	}

#if defined(SKT_PROTOCOL_SUPPORT_ENABLED)
    if ( rt.enc & PCP_ENC_TYPE_COMPRESS )
    {
        str_release(&deComBuf);
    }
#endif
	return PCP_OK;
}

int pcp_init()
{
#if defined (__APPLE__) || defined (ANDROID)
	init_addresses();
#endif
    
	/*init timer*/
	if (init_timer(cb_timeout) < 0)
		return PCP_ERR;

	/*init cfg*/
	create_cfg();

	/*init call pm*/
	pm_init();

	/*init session*/
	init_session();

    /* init call info report */
    pcp_reset_call_info_report();

	/*update sys state to wait*/
	pcp_update_sysstate(PCP_STATE_WAIT);

	return PCP_OK;
}

void pcp_destroy()
{
	/*destroy timers*/
	destroy_timer();

	/*destroy session*/
	destroy_session();
    
#if defined (__APPLE__) || defined(ANDROID)
	free_addresses();
#endif

	/*destroy call pm*/
	pm_destroy();

	/* free cfg */
	remove_cfg();

	/*update state to wait*/
	pcp_update_sysstate(PCP_STATE_WAIT);
	
}

int pcp_set_cb_vtable(pcp_cb_vtable_t* vtable)
{
    if ( NULL == vtable )
    {
        return PCP_ERR;
    }
    return set_cb_vtable(vtable);
}

int pcp_set_config(IN pcp_cfg_t* pcfg)
{
	if (NULL == pcfg)
		return PCP_ERR;

	set_proxy_cfg(&pcfg->proxy);
	
	return PCP_OK;
}

int pcp_update_rtpp(IN pcp_rtpp_info_t* prtpp)
{
	if (prtpp == NULL)
		return PCP_ERR;

	return set_rtpp_cfg(prtpp);
}

int pcp_update_sysstate(IN int sys_state)
{
	return set_system_state((pcp_state_t)sys_state);
}

OUT int pcp_get_state(void)
{
	/*this we only support one call, so state can get from first call*/
	//int call_state = session_state(callid);
	int call_state = session_state(NULL);

	if (call_state == PCP_STATE_WAIT)
		call_state = get_system_state();
	
	return call_state;
}


OUT int pcp_get_callmode(void)
{
	int call_mode = PCP_DIRECT_CM;
	pcp_session_t* psession = NULL;

	lock_session();
	psession = get_session(pm_callid());
	if (psession != NULL)
		call_mode = psession->mode;
	ulock_session();

	return call_mode;
}

OUT int pcp_get_role(void)
{
	int call_role = PCP_OFFER;
	pcp_session_t* psession = NULL;

	lock_session();
	psession = get_session(pm_callid());
	if (psession != NULL)
		call_role = psession->role;
	ulock_session();

	return call_role;
}

OUT void* pcp_call_pm(void)
{
	return pm_get();
}

int pcp_trace_best_mediastream(void)
{
	pcp_session_t* psession = NULL;

	lock_session();
	psession = get_session(pm_callid());
	if (psession != NULL)
    {
		trace_stream_log(psession);
    }
	ulock_session();
	return PCP_OK;
}

int pcp_trace_line_change(IN int mode)
{
	pcp_session_t* psession = NULL;

	lock_session();
	psession = get_session(pm_callid());
	if (psession != NULL)
		trace_linechange_log(psession, mode);
	ulock_session();

	return PCP_OK;
}

int pcp_audio_fec_enabled(void)
{
	int fec_enabled = 0;
	pcp_session_t* psession = NULL ;

	lock_session();
	psession = get_session(pm_callid());
	if (psession != NULL)
		fec_enabled = psession->afec_flag;
	ulock_session();

	return fec_enabled;
}

int pcp_direct_fec_enabled(void)
{
	int fec_enabled = 0;
	pcp_session_t* psession = NULL ;

	lock_session();
	psession = get_session(pm_callid());
	if (psession != NULL)
		fec_enabled = psession->directfec_flag;
	ulock_session();

	return fec_enabled;
}
//add by wuzhaoyang20151228 start
int pcp_direct_mulriple_fec_enabled(void)
{
	int mulriple_fec_enabled = 0;
	pcp_session_t* psession = NULL;

	lock_session();
	psession = get_session(pm_callid());
	if (psession != NULL)
		mulriple_fec_enabled = psession->directfec_mulriple_flag;
	ulock_session();

	return mulriple_fec_enabled;
}
//add by wuzhaoyang20151228 end

/* add by Vinton.Liu on 2016-05-30 for video fec and nack config begin */
OUT int pcp_video_fec_enabled(void)
{
    int fec_enabled = 0;
    pcp_session_t* psession = NULL;

	lock_session();
	psession = get_session(pm_callid());
	if (psession != NULL)
		fec_enabled = psession->vfec_flag;
	ulock_session();

    return fec_enabled;
}

OUT int pcp_video_nack_enabled(void)
{
    int nack_enabled = 0;
    pcp_session_t* psession = NULL;

	lock_session();
	psession = get_session(pm_callid());
	if (psession != NULL)
		nack_enabled = psession->vnack_flag;
	ulock_session();

    return nack_enabled;
}
/* Vinton.Liu add end */

int pcp_ice_enabled(void)
{
	int ice_enabled = 0;
	pcp_session_t* psession = NULL ;

	lock_session();
	psession = get_session(pm_callid());
	if (psession != NULL)
		ice_enabled = psession->ice_flag;
	ulock_session();

	return ice_enabled;
}

int pcp_in_trans_support_enabled(void)
{
    int enabled = 0;
	pcp_session_t* psession = NULL ;

	lock_session();
	psession = get_session(pm_callid());
	if (psession != NULL)
		enabled = psession->intrans_flag;
	ulock_session();

	return enabled;
}

int pcp_media_stream_enabled(IN int type)
{
	int enabled = 0;
	pcp_session_t* psession = NULL;

	lock_session();
	psession = get_session(pm_callid());
	if (psession != NULL)
	{
		if (type == AUDIO)
			enabled = psession->audio_flag;
		else
			enabled = psession->video_flag;
	}
	ulock_session();
	
	return enabled;
}

int pcp_media_relay_params(OUT pcp_relay_info_t* prelay)
{
	pcp_session_t* psession = NULL;

	if (prelay == NULL)
		return PCP_ERR;

	lock_session();
	psession = get_session(pm_callid());
	if (psession != NULL)
		memcpy(prelay, &psession->relay, sizeof(pcp_relay_info_t));
	ulock_session();

	return PCP_OK;
}

int pcp_remote_ice_params(OUT pcp_ice_info_t* pice)
{
	pcp_session_t* psession = NULL;

	if (pice == NULL)
		return PCP_ERR;

	lock_session();
	psession = get_session(pm_callid());
	if (psession != NULL && psession->rsdp != NULL)
		memcpy(pice, &psession->rsdp->ice, sizeof(pcp_ice_info_t));
	ulock_session();

	return PCP_OK;
}

int pcp_get_remote_through_params(OUT pcp_through_info_t *through)
{

	pcp_session_t* psession = NULL;

	if (through == NULL)
		return PCP_ERR;

	lock_session();
	psession = get_session(pm_callid());
	if (psession != NULL && psession->rsdp != NULL)
		memcpy(through, &psession->rsdp->through, sizeof(pcp_through_info_t));
	ulock_session();

	return PCP_OK;
}

int pcp_best_mediastream(IN int type, OUT pcp_session_media_t* pstream)
{
	pcp_session_t* psession = NULL;
	
	if (pstream == NULL)
		return PCP_ERR;

	lock_session();
	psession = get_session(pm_callid());
	if (psession != NULL)
	{
		if (type == AUDIO)
		{
			memcpy(pstream, &psession->audio_stream, sizeof(pcp_session_media_t));
		}
		else
		{
			memcpy(pstream, &psession->video_stream, sizeof(pcp_session_media_t));
		}
	}
	ulock_session();

	return PCP_OK;
}

int pcp_update_mediastream(IN int type, IN const pcp_session_media_t* pstream)
{
	pcp_session_t* psession = NULL;
	
	if (pstream == NULL)
		return PCP_ERR;

	lock_session();
	psession = get_session(pm_callid());
	if (psession != NULL)
	{
		if (type == AUDIO)
		{
			memcpy(&psession->audio_stream, pstream, sizeof(pcp_session_media_t));
		}
		else
		{
			memcpy(&psession->video_stream, pstream, sizeof(pcp_session_media_t));
		}
	}
	ulock_session();

	return PCP_OK;
}

int pcp_get_audio_codecs(OUT pcp_codec_list_t* pcodecslist)
{
	return get_audio_codecs(pcodecslist);
}

int pcp_update_codecs_enabled(IN void* pcodeclist)
{
	return update_codecs_params((pcp_codec_list_t*)pcodeclist);
}

int pcp_directcallfec_enabled(IN unsigned char fecenable)
{
	directcallfec_enable = fecenable;
	return 0;
}

/* add by Vinton.Liu on 2016-05-30 for video fec and nack config begin */
int pcp_update_video_fec_enabled(IN bool enabled)
{
    return pcp_update_vie_fec_flag(enabled);
}

int pcp_update_video_nack_enabled(IN bool enabled)
{
    return pcp_update_vie_nack_flag(enabled);
}
/* Vinton.Liu add end */

int pcp_build_version(void)
{
	str_t hmsg, bmsg;
	res_st(&hmsg, sizeof(hmsg));
	res_st(&bmsg, sizeof(bmsg));
	
	/*send message */
	send_msg(&hmsg, &bmsg, PCP_MSG_VERSION_TP);
	
	return PCP_OK;
}


int pcp_build_login(IN const char* ssid, IN int netmode)
{
    pcp_route_t rh;
	str_t hmsg, bmsg;
	
	if (ssid == NULL)
		return PCP_ERR;
		
    res_st(&g_login, sizeof(g_login));
	g_login.randcode = get_randcode();
	g_login.netmode = netmode;
	strncpy(g_login.ssid, ssid, sizeof(g_login.ssid) - 1);
	get_proxy_version(g_login.version);

#ifdef _WIN32
	strcpy(g_login.devId, "Windows 7");
	strcpy(g_login.devType, "Win32");
#elif defined(ANDROID)
	strcpy(g_login.devId, "Android Phone");
	strcpy(g_login.devType, "Android");
#else
    strcpy(g_login.devId, "IPhone");
    strcpy(g_login.devType, "IOS");
#endif
	
    if ( !pcp_is_tlv_enabled() )
    {
    	/*build message bson header*/
    	res_st(&rh, sizeof(rh));
    	rh.type = PCP_NON_TP;
    	rh.op = PCP_NON_LOGIN_OP;
    	str_init(&hmsg, MAX_HEAD_SIZE);
    	build_route_bson_header(&rh, &hmsg);

    	/*build message json body*/	
    	str_init(&bmsg, MAX_BODY_SIZE);
    	build_login_req_json_body(&g_login, &bmsg);

    	/*send message */
    	send_msg(&hmsg, &bmsg, PCP_MSG_REGISTER_TP);

    	/*release str buf*/
    	str_release(&hmsg);
    	str_release(&bmsg);
	}
#if defined(SKT_PROTOCOL_SUPPORT_ENABLED)
    else
    {
        str_t msg;
        char data[MAX_MSG_SIZE] = {0};
        
        msg.slen = MAX_MSG_SIZE;
        msg.sbuf = data;

        if (pcp_skt_build_auth_req_msg(&g_login, &msg) < 0 )
        {
            ms_error("pcp_build_login: build auth req msg error.");
    	    return PCP_ERR;
        }
        
        /*call send callback*/
        call_send_cb(msg.sbuf, msg.slen);
    }
#endif
    return PCP_OK;
}

#if defined(SKT_PROTOCOL_SUPPORT_ENABLED)
int pcp_build_relogin()
{
    str_t msg;
    char data[MAX_MSG_SIZE] = {0};
    
    msg.slen = MAX_MSG_SIZE;
    msg.sbuf = data;
    if ( pcp_skt_build_reauth_req_msg(&g_login, &msg) < 0 )
    {
        ms_error("pcp_build_relogin: build reauth req msg error.");
	    return PCP_ERR;
    }
    
    /*call send callback*/
    call_send_cb(msg.sbuf, msg.slen);
    return PCP_OK;
}

int pcp_build_heartbeat(void)
{
    str_t msg;
    char data[MAX_MSG_SIZE] = {0};
    
    msg.slen = MAX_MSG_SIZE;
    msg.sbuf = data;
    if ( pcp_skt_build_noop_req_msg(&msg) < 0 )
    {
        ms_error("pcp_build_heartbeat: build heartbeat req msg error.");
	    return PCP_ERR;
    }
    
    /*call send callback*/
    call_send_cb(msg.sbuf, msg.slen);
    return PCP_OK;
}
#endif

int pcp_build_logout(void)
{
    if ( !pcp_is_tlv_enabled())
    {
    	pcp_route_t rh;
    	str_t hmsg;
    	
    	/*build message bson header*/
    	res_st(&rh, sizeof(rh));
    	rh.type = PCP_NON_TP;
    	rh.op = PCP_NON_LOGOUT_OP;
    	str_init(&hmsg, MAX_HEAD_SIZE);
    	build_route_bson_header(&rh, &hmsg);

    	/*send message */
    	send_msg(&hmsg, NULL, PCP_MSG_REGISTER_TP);

    	/*release str buf*/
    	str_release(&hmsg);
	}

	return PCP_OK;
}


int pcp_build_push_call_rsp(IN const char* callid, IN int vpsid, IN int reason)
{
	pcp_session_t* psession = NULL;
	pcp_route_t rh;
	pcp_push_rsp_t pushrsp;

	res_st(&rh, sizeof(rh));
	rh.sn = cm_sceq();
    if ( pcp_is_csrv_enabled() )
    {
        rh.type = PCP_CSRV_TP;
    }
    else
    {
        rh.type = PCP_CALL_TP;
    }

	rh.op = PCP_PUSH_RSP_OP;
	rh.enc = is_local_rc4_enabled();
    //modiy by wenqinglin
    //rh.atype = psession->atype;
    rh.atype = 2;
#if defined(SKT_PROTOCOL_SUPPORT_ENABLED)
    if ( pcp_is_tlv_enabled() && is_local_compress_enabled())
    {
        rh.enc |= PCP_ENC_TYPE_COMPRESS;
    }
#endif
	rh.dsid = vpsid;

	res_st(&pushrsp, sizeof(pushrsp));
	pushrsp.reason = reason;
	strcpy(pushrsp.call_id, callid);

	if ( pcp_send_msg(&rh, (void *)&pushrsp) < 0 )
    {
        ms_error("[pcp_build_push_call_rsp] send msg failed.");
        return PCP_ERR;
    }

	if (reason == PCP_SUCCESS)
	{
		/*create session*/
		psession = create_session(callid);
		if (psession == NULL)
			return PCP_ERR;
		
		lock_session();
		psession->dsid = vpsid;
		psession->role = PCP_ANSWER;
		psession->rc4_flag = is_local_rc4_enabled();
		psession->call_state = PCP_STATE_PUSHACTIVE;
		psession->mode = PCP_FREE_CM;

		/*store call param*/
		update_call_param(psession);
		/*callback state changed*/
		call_state_cb(psession->call_state);
		
		ulock_session();
		
		/*start timer t3*/
		start_timer(TIMEOUT_T3);
	}
	
	ms_report("send push call rsp[%d] ok!!!", reason);

	return PCP_OK;
}

int pcp_build_call_dialing(IN const pcp_call_dialing_para_t* pCallParam, IN const pcp_ice_info_t* ice, IN pcp_through_info_t *through)
{
	char uid[64] = {0};
	char userid[64] = {0};
	char phone[64] = {0};
	char callid[64] = {0};
    char tuid[64] = {0};
    char tuserid[64] = {0};
    char tphone[64] = {0};
    char fnickname[64] = {0};
	pcp_session_t* psession = NULL;
	pcp_route_t rh;
	pcp_call_req_t callreq;
	pcp_rtpp_info_t rtpp_info;
	pcp_bool video_enabled = FALSE;
	
	if ( NULL == pCallParam )
	{
        ms_error("pcp_build_call_dialing: failed on null param!");
		return PCP_ERR;
	}

	lock_session();
	/*create callid and create session*/
	get_uid(uid);
	get_userid(userid);
	get_userphone(phone);
	get_nickname(fnickname);
	
	if ( uid[0] != '\0' )
	{
	    create_callid(uid, callid);
	}
	else
	{
	    create_callid(userid, callid);
	}
	
	psession = create_session(callid);
	if (psession == NULL)
	{
		ulock_session();
		return PCP_ERR;
	}

    if ( pCallParam->tuid[0] != '\0' )
    {
        strncpy(tuid, pCallParam->tuid, strlen(pCallParam->tuid));
        // for im3.0
        strncpy(tuserid, pCallParam->tuid, strlen(pCallParam->tuid));
    }

    if ( pCallParam->tphone[0] != '\0' )
    {
        strncpy(tphone, pCallParam->tphone, strlen(pCallParam->tphone));
    }

#if 0
    if ( pCallParam->name != NULL )
    {
        strncpy(fnickname, pCallParam->name, strlen(pCallParam->name));
    }
#endif

	psession->mode = pCallParam->call_mode;
	psession->rc4_flag = is_local_rc4_enabled();
	psession->afec_flag = is_local_audio_fec_enabled();
	if (directcallfec_enable == 0)
	{
		psession->directfec_flag = 0;
	} 
	else
	{
		psession->directfec_flag = is_local_direct_fec_enabled();
	}	
	psession->call_state = PCP_STATE_INVITE;
	psession->role = PCP_OFFER;
    psession->atype = get_account_type();
	strcpy(psession->fuid, uid);
	strcpy(psession->fuserid, userid);
	strcpy(psession->fphone, phone);
    /* add by VintonLiu on 20141219 for store nickname in callpm */
    strncpy(psession->fnickname, fnickname, sizeof(psession->fnickname)-1);
	strcpy(psession->tuid, tuid);
	strcpy(psession->tuserid, tuserid);
	strcpy(psession->tphone, tphone);
	if ( pCallParam->user_data[0] != '\0' )
    {
        strncpy(psession->user_data, pCallParam->user_data, sizeof(psession->user_data) -1 );
    }

	if (is_local_video_enabled())
    	video_enabled = pCallParam->video_enabled;
	
	/* Added By MNY@2014-08-22 */
	psession->video_flag = video_enabled;

	/*create sdp for this session*/
	psession->lsdp = create_sdp(1, video_enabled);

	/*update rtpp params for this session*/
	res_st(&rtpp_info, sizeof(rtpp_info));
	get_rtpp_cfg(&rtpp_info);
	if (rtpp_info.rtpp_num != 0)
	{
	    ms_report("pcp_build_call_dialing: rtpp_num[%d]", rtpp_info.rtpp_num);
		update_sdp_rtpp_params(psession->lsdp, &rtpp_info);
	}

	/*update ice params for this session*/
	if (ice != NULL && ice->cdd_num != 0)
		update_sdp_ice_params(psession->lsdp, ice);

    /*update through params for this session*/
    if ( video_enabled && through ) {
        update_sdp_through_params(psession->lsdp, through);
    }
	
	res_st(&callreq, sizeof(callreq));
	if (directcallfec_enable)
	{
		callreq.sflag = get_upate_flag();
	}
	else
	{
		callreq.sflag = get_upate_flag();
		callreq.sflag &= ~PCP_DIRECTFEC_OPEN;
		callreq.sflag &= ~PCP_DIRECTFEC_MULRIPLE_OPEN;    //added by king 20160120
	}
	callreq.callmode = psession->mode;
	callreq.atype = get_account_type();
	get_proxy_version(callreq.ver);
	strcpy(callreq.call_id, psession->callid);
	if ( uid[0] != '\0' )
	{
	    strcpy(callreq.fuid, uid);
	}
	if ( userid[0] != '\0' )
	{
	    strcpy(callreq.fuserid, userid);
	}
	strcpy(callreq.fphone, psession->fphone);
	strcpy(callreq.tuid, psession->tuid);
	strcpy(callreq.tuserid, psession->tuserid);
	strcpy(callreq.tphone, psession->tphone);
	strcpy(callreq.fnickname, psession->fnickname);/*added by luochengcong 20141110*/
	strcpy(callreq.user_data, pCallParam->user_data);
	callreq.calltype = pCallParam->calltype;		/*added by luochengcong 20140916*/
	callreq.sdp = psession->lsdp;
	
	res_st(&rh, sizeof(rh));
	rh.sn = cm_sceq();
    if ( pcp_is_csrv_enabled() )
    {
        rh.type = psession->msg_type = PCP_CSRV_TP;
    }
    else
    {
        rh.type = psession->msg_type = PCP_CALL_TP;
    }
	rh.op = PCP_CALL_REQ_OP;
	rh.enc = psession->rc4_flag;
#if defined(SKT_PROTOCOL_SUPPORT_ENABLED)
    if ( pcp_is_tlv_enabled() && is_local_compress_enabled())
    {
        rh.enc |= PCP_ENC_TYPE_COMPRESS;
    }
#endif
	rh.atype = get_account_type();
	if ( tuid[0] != '\0' )
		rh.tuid = cm_stoi64(tuid);

    if ( pcp_send_msg(&rh, (void *)&callreq) < 0 )
    {    
        ms_error("[pcp_build_call_dialing] send msg failed.");
        ulock_session();
        return PCP_ERR;
    }

	/*trace call log*/
	trace_invite_log(psession);

	/* reset call info report */
	pcp_reset_call_info_report();
	
	/*store call param*/
	update_call_param(psession);

	/*callback state changed*/
	call_state_cb(psession->call_state);
	
	ulock_session();

	/*start timer t0*/
	start_timer(TIMEOUT_T0);
	
	/*connecting event*/
	report_call_event(PCP_CALLDIALING_EV, PCP_CONNECTING, NULL, 0);

	return PCP_OK;
}


int pcp_build_call_answer(IN const pcp_ice_info_t* ice)
{
	pcp_session_t* psession = NULL;
	pcp_route_t rh;
	pcp_call_rsp_t callrsp;
    pcp_through_info_t through_info;
	pcp_rtpp_info_t rtpp_info;
	
	lock_session();
    /*if callid is  NULL,default get first session node */
	psession = get_session(pm_callid()); 
	if (psession == NULL)
	{
		ulock_session();
		return PCP_ERR;
	}

	psession->rc4_flag = is_local_rc4_enabled();
	psession->role = PCP_ANSWER;
	psession->call_state = PCP_STATE_CONNECT;
	psession->mode = PCP_FREE_CM;

	/*create sdp for this session*/
	if (psession->lsdp == NULL)
		psession->lsdp = create_sdp(1, psession->video_flag);

	/*update rtpp params for this session*/
	res_st(&rtpp_info, sizeof(rtpp_info));
    /* add by vinton on 20140829 for get rtpp cfg */
    get_rtpp_cfg(&rtpp_info);
    if (rtpp_info.rtpp_num != 0)
    	update_sdp_rtpp_params(psession->lsdp, &rtpp_info);

	/*update ice params for this session*/
	if (ice != NULL)
		update_sdp_ice_params(psession->lsdp, ice);

    /* del by Vinton.Liu on 20160530 for don't need here, ring rsp have saved */
#if 0
    /*update ice params for this session*/
    res_st(&through_info, sizeof(through_info));
    if (psession->video_flag) {
        through_info.video_enabled = psession->video_flag;        
        update_sdp_through_params(psession->lsdp, &through_info);
    }
#endif
    /* Vinton.Liu del end */
    	
	res_st(&rh, sizeof(rh));
	rh.sn = cm_sceq();
	rh.type = psession->msg_type;
	rh.op = PCP_CALL_RSP_OP;
	rh.enc = psession->rc4_flag;
    rh.atype = psession->atype;
#if defined(SKT_PROTOCOL_SUPPORT_ENABLED)
    if ( pcp_is_tlv_enabled() && is_local_compress_enabled())
    {
        rh.enc |= PCP_ENC_TYPE_COMPRESS;
    }
#endif
	rh.tuid = cm_stoi64(psession->fuid);
	rh.fuid = cm_stoi64(psession->tuid);
	rh.dsid = psession->dsid;
	
	
	res_st(&callrsp, sizeof(callrsp));
	callrsp.reason = PCP_SUCCESS;
	/*add by Rookie for youxin old version20131206*/
	callrsp.baudio_pt = psession->audio_stream.session_pt;
	callrsp.bvideo_pt = psession->video_stream.session_pt;
	strcpy(callrsp.call_id, psession->callid);
	get_proxy_version(callrsp.ver);
	callrsp.sflag = get_upate_flag();
	callrsp.sdp = psession->lsdp;

    if ( pcp_send_msg(&rh, (void *)&callrsp) < 0 )
    {
        ulock_session();
        ms_error("[pcp_build_call_answer] send msg failed.");
        return PCP_ERR;
    }

	/*trace call log*/
	trace_answer_log(psession, PCP_SUCCESS);    
    
	/*callback state changed*/
	call_state_cb(psession->call_state);
	
	ulock_session();

	/*stop timer t1,start timer t2*/
	stop_timer(TIMEOUT_T1);
	start_timer(TIMEOUT_T2);

    report_call_event(PCP_CALLANSWER_EV, PCP_SUCCESS, NULL, 0);
    
	return PCP_OK;
}

int build_call_rsp_err(int errcode, const char* callid, const pcp_route_t* proute)
{
	pcp_route_t rh;
	pcp_call_rsp_t callrsp;
	
	res_st(&rh, sizeof(rh));
	rh.sn = cm_sceq();
	rh.type = proute->type;
	rh.op = PCP_CALL_RSP_OP;
	rh.enc = is_local_rc4_enabled();
    rh.atype = proute->atype;
#if defined(SKT_PROTOCOL_SUPPORT_ENABLED)
    if ( pcp_is_tlv_enabled() && is_local_compress_enabled())
    {
        rh.enc |= PCP_ENC_TYPE_COMPRESS;
    }
#endif
	rh.tuid = proute->fuid;
	rh.fuid = proute->tuid;
	rh.dsid = proute->dsid;
	
	res_st(&callrsp, sizeof(callrsp));
	callrsp.reason = errcode;
	strcpy(callrsp.call_id, callid);

    if ( pcp_send_msg(&rh, (void *)&callrsp) < 0 )
    {
        ms_error("[build_call_rsp_err] send msg failed.");
        return PCP_ERR;
    }
	return PCP_OK;
}

int pcp_build_call_ringing(IN const pcp_ice_info_t* ice, pcp_through_info_t *through)
{
	pcp_session_t* psession = NULL;
	pcp_route_t rh;
	pcp_ring_rsp_t ringrsp;
	pcp_rtpp_info_t rtpp_info;

	lock_session();
	/*if callid is NULL,default get first session node */
	psession = get_session(pm_callid()); 
	if (psession == NULL)
	{
		ulock_session();
		return PCP_ERR;
	}

	psession->rc4_flag = is_local_rc4_enabled();
	psession->role = PCP_ANSWER;
	psession->call_state = PCP_STATE_RINGING;
	psession->mode = PCP_FREE_CM;

	/*create sdp for this session*/
	if (psession->lsdp == NULL)
	{
		psession->lsdp = create_sdp(1, psession->video_flag);
        if ( psession->lsdp == NULL )
        {
            ms_error("[pcp_build_call_ringing] create local sdp failed.");  
            ulock_session();
            pcp_build_call_hangup(30);
    		return PCP_ERR;
        }
	}
    
	/*update rtpp params for this session*/
	res_st(&rtpp_info, sizeof(rtpp_info));
    /* add by vinton on 20140829 for get rtpp cfg */
    get_rtpp_cfg(&rtpp_info);    
	if (rtpp_info.rtpp_num != 0)
		update_sdp_rtpp_params(psession->lsdp, &rtpp_info);

	/*update ice params for this session*/
	if (ice != NULL && ice->cdd_num != 0 && psession->ice_flag)
		update_sdp_ice_params(psession->lsdp, ice);
    
    /*update ice params for this session*/ //added by luochengcong 20141009
    
    if (psession->video_flag) {
        through->video_enabled = psession->video_flag;
        update_sdp_through_params(psession->lsdp, through);
    }
    	
	res_st(&rh, sizeof(rh));
	rh.sn = cm_sceq();
	rh.type = psession->msg_type;
	rh.op = PCP_RING_RSP_OP;
	rh.enc = psession->rc4_flag;
    rh.atype = psession->atype;
#if defined(SKT_PROTOCOL_SUPPORT_ENABLED)
    if ( pcp_is_tlv_enabled() && is_local_compress_enabled())
    {
        rh.enc |= PCP_ENC_TYPE_COMPRESS;
    }
#endif
	rh.tuid = cm_stoi64(psession->fuid);
	rh.fuid = cm_stoi64(psession->tuid);
	rh.dsid = psession->dsid;
	
	res_st(&ringrsp, sizeof(ringrsp));
	strcpy(ringrsp.call_id, psession->callid);
	ringrsp.sdp = psession->lsdp;
	/*add by Rookie for youxin old version20131206*/
	ringrsp.baudio_pt = psession->audio_stream.session_pt;
	ringrsp.bvideo_pt = psession->video_stream.session_pt; 

    if ( pcp_send_msg(&rh, (void *)&ringrsp) < 0 )
    {
        ulock_session();
        ms_error("[pcp_build_call_ringing] send msg failed.");
        return PCP_ERR;
    }

	/*trace call log*/
	trace_ringing_log(psession);

	/*callback state changed*/
	call_state_cb(psession->call_state);

	ulock_session();

	/*start timer t1*/
	start_timer(TIMEOUT_T1);
	
	return PCP_OK;
}

int build_call_update_rsp(pcp_update_rsp_t* rsp, const pcp_route_t* proute)
{
	pcp_route_t rh;
	pcp_update_rsp_t updatersp;
		
	res_st(&rh, sizeof(rh));
	rh.sn = cm_sceq();
	rh.type = proute->type;
	rh.op = PCP_UPDATE_RSP_OP;
	rh.enc = is_local_rc4_enabled();
    rh.atype = proute->atype;
#if defined(SKT_PROTOCOL_SUPPORT_ENABLED)
    if ( pcp_is_tlv_enabled() && is_local_compress_enabled())
    {
        rh.enc |= PCP_ENC_TYPE_COMPRESS;
    }
#endif

	rh.tuid = proute->fuid;
	rh.fuid = proute->tuid;
	rh.dsid = proute->dsid;
	 
	res_st(&updatersp, sizeof(updatersp));
	updatersp.reason = rsp->reason;
	updatersp.ice_mode = rsp->ice_mode;
	strcpy(updatersp.call_id, rsp->call_id);
	strcpy(updatersp.apt, rsp->apt);
	strcpy(updatersp.vpt, rsp->vpt);

    if ( pcp_send_msg(&rh, (void *)&updatersp) < 0 )
    {
        ms_error("[build_call_rsp_err] send msg failed.");
        return PCP_ERR;
    }
	return PCP_OK;
}

int pcp_build_call_ack(IN int ice_mode)
{
	pcp_session_t* psession = NULL;
	pcp_route_t rh;
	pcp_call_ack_t callack;
	
	lock_session();
	/*if NULL,default get first session node */
	psession = get_session(pm_callid()); 
	if (psession == NULL)
	{
		ulock_session();
		return PCP_ERR;
	}
		
	res_st(&rh, sizeof(rh));
	rh.sn = cm_sceq();
	rh.type = psession->msg_type;
	rh.op = PCP_CALL_ACK_OP;
	rh.enc = psession->rc4_flag;
#if defined(SKT_PROTOCOL_SUPPORT_ENABLED)
    if ( pcp_is_tlv_enabled() && is_local_compress_enabled())
    {
        rh.enc |= PCP_ENC_TYPE_COMPRESS;
    }
#endif
	rh.tuid = cm_stoi64(psession->tuid);
	rh.fuid = cm_stoi64(psession->fuid);
	rh.dsid = psession->dsid;	
	
	res_st(&callack, sizeof(callack));
	callack.ice_type = get_ice_type();
	callack.ice_mode = ice_mode;
	strcpy(callack.call_id, psession->callid);

    if ( pcp_send_msg(&rh, (void *)&callack) < 0 )
    {        
        ms_error("[pcp_build_call_ack] send msg failed.");
        ulock_session();
        return PCP_ERR;
    }

	/*trace call log*/
	trace_ack_log(psession);
	
	ulock_session();

	/*stop all timers*/
	stop_all_timers();

	return PCP_OK;
}


int pcp_build_call_hangup(int reason)
{
	pcp_session_t* psession = NULL;
	pcp_route_t rh;
	pcp_bye_req_t byereq;

	lock_session();
	/*if callid is NULL,default get first session node */
	psession = get_session(pm_callid()); 
	if (check_session(psession) < 0)
	{
		ms_error("Have No Usable Session !!!");
		ulock_session();
		return PCP_ERR;
	}
		
	res_st(&rh, sizeof(rh));
	rh.sn = cm_sceq();
	rh.type = psession->msg_type;
	rh.op = PCP_CALL_BYE_OP;
	rh.enc = psession->rc4_flag;
#if defined(SKT_PROTOCOL_SUPPORT_ENABLED)
    if ( pcp_is_tlv_enabled() && is_local_compress_enabled())
    {
        rh.enc |= PCP_ENC_TYPE_COMPRESS;
    }
#endif
	/* BEGIN: Added by gonghuojin, 2014/2/11   PN:can't bye */
	if (psession->role == PCP_OFFER)
	{
		rh.tuid = cm_stoi64(psession->tuid);
		rh.fuid = cm_stoi64(psession->fuid);
	}
	else
	{
		rh.tuid = cm_stoi64(psession->fuid);
		rh.fuid = cm_stoi64(psession->tuid);
	}
	/* END:   Added by gonghuojin, 2014/2/11 */
	rh.dsid = psession->dsid;
		
	res_st(&byereq, sizeof(byereq));
	byereq.reason = reason;
	strcpy(byereq.call_id, psession->callid);

    if ( pcp_send_msg(&rh, (void *)&byereq) < 0 )
    {        
        ms_error("[pcp_build_call_hangup] send msg failed.");
        ulock_session();
        return PCP_ERR;
    }

	/*trace call log*/
	trace_hangup_log(psession, reason);

	/*stop  all timers*/
	stop_all_timers();
	
	ulock_session();

    report_call_event(PCP_CALLHUNGUP_EV, reason, NULL, 0);
	return PCP_OK;
}


int pcp_build_call_declined(IN int reason)
{
	pcp_session_t* psession = NULL;
	pcp_route_t rh;
	pcp_call_rsp_t callrsp;
	
	lock_session();
	/*if callid is NULL,default get first session node */
	psession = get_session(pm_callid());  
	if (check_session(psession) < 0)
	{
		ms_error("Have No Usable Session !!!");
		ulock_session();
		return PCP_ERR;
	}

	res_st(&rh, sizeof(rh));
	rh.sn = cm_sceq();
	rh.type = psession->msg_type;
	rh.op = PCP_CALL_RSP_OP;
	rh.enc = psession->rc4_flag;
    rh.atype = psession->atype;
#if defined(SKT_PROTOCOL_SUPPORT_ENABLED)
    if ( pcp_is_tlv_enabled() && is_local_compress_enabled())
    {
        rh.enc |= PCP_ENC_TYPE_COMPRESS;
    }
#endif

	/* BEGIN: Added by gonghuojin, 2014/2/11   PN:can't bye */
	if (psession->role == PCP_OFFER)
	{
		rh.tuid = cm_stoi64(psession->tuid);
		rh.fuid = cm_stoi64(psession->fuid);
	}
	else
	{
		rh.tuid = cm_stoi64(psession->fuid);
		rh.fuid = cm_stoi64(psession->tuid);
	}
	/* END:   Added by gonghuojin, 2014/2/11 */
	rh.dsid = psession->dsid;	
	
	res_st(&callrsp, sizeof(callrsp));
	callrsp.reason = reason;
	strcpy(callrsp.call_id, psession->callid);

    if ( pcp_send_msg(&rh, (void *)&callrsp) < 0 )
    {        
        ms_error("[pcp_build_call_declined] send msg failed.");
        ulock_session();
        return PCP_ERR;
    }
    
	/*trace call log*/
	trace_hangup_log(psession, callrsp.reason);

	/*stop  all timers*/
	stop_all_timers();

	ulock_session();

    /* declined by myself */
    reason = PCP_HUNGUPMYSELF;
    
    report_call_event(PCP_CALLHUNGUP_EV, reason, NULL, 0);
    
	return PCP_OK;
}

int pcp_build_route_update(void)
{
	pcp_session_t* psession = NULL;
	pcp_route_t rh;
	pcp_route_update_t rt_update;
	
	lock_session();
	/*if callid is NULL,default get first session node */
	psession = get_session(pm_callid()); 
	if (check_session(psession) < 0)
	{
		ms_error("Have No Useable Session !!!");
		ulock_session();
		return PCP_ERR;
	}

	
	res_st(&rh, sizeof(rh));
	rh.sn = cm_sceq();
	rh.type = psession->msg_type;
	rh.op = PCP_ROUTE_UPDATE_OP;
	rh.enc = psession->rc4_flag;
    rh.atype = psession->atype;
#if defined(SKT_PROTOCOL_SUPPORT_ENABLED)
    if ( pcp_is_tlv_enabled() && is_local_compress_enabled())
    {
        rh.enc |= PCP_ENC_TYPE_COMPRESS;
    }
#endif

	if (psession->role == PCP_OFFER)
	{
		rh.tuid = cm_stoi64(psession->tuid);
		rh.fuid = cm_stoi64(psession->fuid);
	}
	else
	{
		rh.tuid = cm_stoi64(psession->fuid);
		rh.fuid = cm_stoi64(psession->tuid);
	}
	
	rh.dsid = psession->dsid;	

	res_st(&rt_update, sizeof(rt_update));
	strcpy(rt_update.call_id, psession->callid);
    get_userid(rt_update.userid);
    
    if ( pcp_send_msg(&rh, (void *)&rt_update) < 0 )
    {        
        ms_error("[pcp_build_route_update] send msg failed.");
        ulock_session();
        return PCP_ERR;
    }

	/*trace call log*/
	trace_route_update_log(psession);

	ulock_session();

	return PCP_OK;
}

/* add by Vinton.Liu on 2016-05-04 for unsupport op rsp err begin */
int build_unsupport_rsp_err(const pcp_route_t* prt)
{
    pcp_route_t rh;
    
    res_st(&rh, sizeof(rh));
    rh.sn = cm_sceq();
	rh.type = prt->type;
	rh.op = PCP_UNSUPPORT_RSP_OP;
	rh.enc = is_local_rc4_enabled();
    rh.atype = prt->atype;
#if defined(SKT_PROTOCOL_SUPPORT_ENABLED)
    if ( pcp_is_tlv_enabled() && is_local_compress_enabled())
    {
        rh.enc |= PCP_ENC_TYPE_COMPRESS;
    }
#endif
	rh.tuid = prt->fuid;
	rh.fuid = prt->tuid;
	rh.dsid = prt->dsid;

    if ( pcp_send_msg(&rh, NULL) < 0 )
    {
        ms_error("[build_unsupport_rsp_err] send msg failed.");
        return PCP_ERR;
    }
	return PCP_OK;
}
/* Vinton.Liu add end */

/* add by Vinton.Liu on 2016-04-06 for add internal state passthrough signalling begin */
int pcp_check_in_trans_state(IN pcp_call_trans_state_t* old_state, 
                                    IN const pcp_call_trans_state_t* new_state)
{
    if ( NULL == old_state || NULL == new_state )
    {
        return PCP_ERR;
    }

    if ( (old_state->vie_angle != new_state->vie_angle)
        || (old_state->vie_state != new_state->vie_state)
        || (old_state->p2p_state != new_state->p2p_state)
        || (old_state->mic_state !=  new_state->mic_state)
        || (old_state->net_state != new_state->net_state))
    {
        return PCP_OK;
    }

    return PCP_ERR;    
}

int pcp_build_in_trans_req(IN const pcp_call_trans_state_t* trans_state)
{
    pcp_route_t rh;
    pcp_in_trans_req_t in_trans_req = { 0 };
    pcp_session_t* psession = NULL;
    
    if ( NULL == trans_state )
    {
        return PCP_ERR;
    }

    lock_session();
	/*if NULL,default get first session node */
	psession = get_session(pm_callid()); 
	if (psession == NULL)
	{
		ulock_session();
		return PCP_ERR;
	}
    
    if ( pcp_check_in_trans_state(&psession->trans_state, trans_state) != PCP_OK )
    {
        ulock_session();
		return PCP_OK;
    }
    
    res_st(&rh, sizeof(rh));
	rh.sn = cm_sceq();
	rh.type = psession->msg_type;
	rh.op = PCP_IN_TRANS_REQ_OP;
	rh.enc = psession->rc4_flag;
    rh.atype = psession->atype;
#if defined(SKT_PROTOCOL_SUPPORT_ENABLED)
    if ( pcp_is_tlv_enabled() && is_local_compress_enabled())
    {
        rh.enc |= PCP_ENC_TYPE_COMPRESS;
    }
#endif
	rh.tuid = cm_stoi64(psession->tuid);
	rh.fuid = cm_stoi64(psession->fuid);
	rh.dsid = psession->dsid;

    strncpy(in_trans_req.call_id, psession->callid, 
            sizeof(in_trans_req.call_id)-1);
    in_trans_req.vie_angle = trans_state->vie_angle;
    in_trans_req.vie_state = trans_state->vie_state;
    in_trans_req.p2p_state = trans_state->p2p_state;
    in_trans_req.mic_state = trans_state->mic_state;
    in_trans_req.net_state = trans_state->net_state;

    // save new state
    memcpy(&psession->trans_state, trans_state, sizeof(pcp_call_trans_state_t));

    ms_report("[%s %d] angle[%d] state[%d] p2p[%d] mic[%d]",
            __FUNCTION__, __LINE__,
            trans_state->vie_angle, trans_state->vie_state,
            trans_state->p2p_state, trans_state->mic_state);
    
    if ( pcp_send_msg(&rh, (void *)&in_trans_req) < 0 )
    {
        ms_error("[pcp_build_in_trans_req] send msg failed.");
        ulock_session();
        return PCP_ERR;
    }
	
	ulock_session();
    start_timer(TIMEOUT_T5);
    
	return PCP_OK;
}

int pcp_build_in_trans_rsp(IN int reason)
{
    pcp_route_t rh;
    pcp_in_trans_rsp_t in_trans_rsp = { 0 };
    pcp_session_t* psession = NULL;

    lock_session();
	/*if NULL,default get first session node */
	psession = get_session(pm_callid()); 
	if (psession == NULL)
	{
		ulock_session();
		return PCP_ERR;
	}
    
    res_st(&rh, sizeof(rh));
	rh.sn = cm_sceq();
	rh.type = psession->msg_type;
	rh.op = PCP_IN_TRANS_RSP_OP;
	rh.enc = psession->rc4_flag;
    rh.atype = psession->atype;
#if defined(SKT_PROTOCOL_SUPPORT_ENABLED)
    if ( pcp_is_tlv_enabled() && is_local_compress_enabled())
    {
        rh.enc |= PCP_ENC_TYPE_COMPRESS;
    }
#endif
	rh.tuid = cm_stoi64(psession->tuid);
	rh.fuid = cm_stoi64(psession->fuid);
	rh.dsid = psession->dsid;

    strncpy(in_trans_rsp.call_id, psession->callid, 
            sizeof(in_trans_rsp.call_id)-1);
    in_trans_rsp.errcode = reason;

    if ( pcp_send_msg(&rh, (void *)&in_trans_rsp) < 0 )
    {        
        ms_error("[pcp_build_in_trans_rsp] send msg failed.");
        ulock_session();
        return PCP_ERR;
    }
	
	ulock_session();
	return PCP_OK;
}
/* Vinton.Liu add end */

//add by wuzhaoyang20140605 start
int pcp_build_conf_call_req(IN const pcp_conf_dialing_para_t *dial_para)
{
	char fuid[64] = {0};
	char fuserid[64] = {0};
	char fphone[64] = {0};
	char callid[64] = {0};
	char *pCallId = callid;
	pcp_session_t* psession = NULL;
	pcp_route_t rh;
	pcp_conf_call_req_t callreq;
	pcp_rtpp_info_t rtpp_info;
	int i = 0;
	int is_busy = 0;
	int reason = 0;
	int video_enabled = 0;
	int temp_participant_num = 0;
	int added_num = 0;
	int needAdd = 0;

	if ( dial_para == NULL )
	{
		return PCP_ERR;
	}

	temp_participant_num = dial_para->user_num;
	if ( temp_participant_num == 0 )
	{	    
		return PCP_ERR;
	}

	is_busy = isbusy();
	lock_session();
	get_uid(fuid);
	get_userid(fuserid);
	get_userphone(fphone);

    /* if busy, change p2p call to normal conference */
    if ( is_busy )
    {	
        pCallId = pm_callid();
        if ( pCallId == NULL )
        {
            ms_error("pcp_build_conf_call_req: failed to find callid.");
            ulock_session();
            return PCP_ERR;
        }

        psession = get_session(pCallId);
        if ( psession == NULL )
		{
			ulock_session();
            ms_error("pcp_build_conf_call_req: failed to find session.");
			return PCP_ERR;
		}

		if ( psession->conf_state != NULL )
		{
		    ulock_session();
            ms_error("pcp_build_conf_call_req: conference already exist.");
			return PCP_ERR;
		}
		
        psession->rc4_flag = is_local_rc4_enabled();
    	psession->call_state = PCP_STATE_INVITE;
    	psession->meetingflag = 1;
    	//psession->ice_flag = false;
    	if ( fuid[0] != '\0' )
    	{
    	    strcpy(psession->fuid, fuid);
    	}

    	if ( fuserid[0] != '\0' )
    	{
    	    strcpy(psession->fuserid, fuserid);
    	}
    	strcpy(psession->fphone, fphone);

    	/*create sdp for this session*/
		if (psession->lsdp == NULL)
		{
			psession->lsdp = create_sdp(1, video_enabled);
		}

	    psession->conf_state = create_conference();
	    if ( psession->conf_state == NULL )
	    {
	        ulock_session();	        
            ms_error("pcp_build_conf_call_req: conference create failed.");
			return PCP_ERR;
	    }
	    
       	psession->conf_state->conftype = E_PCP_CONF_TYPE_NORMAL;
       	psession->conf_state->roomstate = E_PCP_CONF_STATE_CREATED;
    	strcpy(psession->conf_state->conference_id, pCallId);
    	if ( dial_para->roomname[0] != '\0' )
    	{
            strcpy(psession->conf_state->roomname, dial_para->roomname);
        }
        if ( dial_para->roompwd[0] != '\0' )
        {
            strcpy(psession->conf_state->roompwd, dial_para->roompwd);
        }
        if ( dial_para->remark[0] != '\0' )
        {
            strcpy(psession->conf_state->remark, dial_para->remark);
        }
        
        res_st(&callreq, sizeof(callreq));
		callreq.sflag = get_upate_flag() ^ 0x8 ^ 0x4;
		callreq.conftype = psession->conf_state->conftype;
		callreq.atype = psession->atype;
		get_proxy_version(callreq.ver);
		strcpy(callreq.call_id, psession->callid);
		strcpy(callreq.fuid, psession->fuid);
		strcpy(callreq.fuserid, psession->fuserid);
		strcpy(callreq.fphone, psession->fphone);
        strcpy(callreq.roomname, psession->conf_state->roomname);
        strcpy(callreq.roompwd, psession->conf_state->roompwd);
        strcpy(callreq.remark, psession->conf_state->remark);

		for (i = 0; i < dial_para->user_num; i++)
		{
		    needAdd = 0;
			if ( dial_para->user_attr[i].uid[0] != '\0' )
			{
			    needAdd = 1;
				strcpy(callreq.plist.user_attr[i].tuid, \
				        dial_para->user_attr[i].uid);
			}
			if ( dial_para->user_attr[i].phone[0] != '\0' )
			{
			    needAdd = 1;
				strcpy(callreq.plist.user_attr[i].tphone, \
				        dial_para->user_attr[i].phone);
			}
			if ( needAdd )
			{
    			callreq.plist.user_attr[i].call_mode = dial_para->user_attr[i].call_mode;
    			callreq.plist.num++;
			}
		}

		temp_participant_num = callreq.plist.num + 1;
        if ( temp_participant_num > MAX_PARTICIPANT_ATTR_NUM )
    	{
    	    ulock_session();
    		report_call_event(PCP_CONFERENCE_EV, \
    		                  PCP_CONF_PARTICIPANT_OVER, NULL, 0);
    		ms_error("pcp_build_conf_call_req: participant_num(%d) > %d", \
    		    temp_participant_num, MAX_PARTICIPANT_ATTR_NUM);
    		return PCP_ERR;
    	}

		callreq.sdp = psession->lsdp;

		res_st(&rh, sizeof(rh));
		rh.sn = cm_sceq();
		rh.type = psession->msg_type;
		rh.op = PCP_MEET_CALL_REQ_OP;
		rh.enc = psession->rc4_flag;
#if defined(SKT_PROTOCOL_SUPPORT_ENABLED)
        if ( pcp_is_tlv_enabled() && is_local_compress_enabled())
        {
            rh.enc |= PCP_ENC_TYPE_COMPRESS;
        }
#endif
		rh.atype = psession->atype;
		if ( psession->role == PCP_OFFER )
		{
			if (psession->tuid != NULL)
				rh.tuid = cm_stoi64(psession->tuid);	
		}
		else
		{
			if (psession->fuid != NULL)
				rh.tuid = cm_stoi64(psession->fuid);
		}

        if ( pcp_send_msg(&rh, (void *)&callreq) < 0 )
        {        
            ms_error("[pcp_build_conf_call_req] send msg failed.");
            ulock_session();
            return PCP_ERR;
        }
        
		/*trace call log*/
		trace_m_invite_log(psession);//������

		/*store call param*/
		update_call_param(psession);

        ulock_session();
        
		/*start timer t0*/
		start_timer(TIMEOUT_T4);
		
        report_call_event(PCP_CONFERENCE_EV, \
                          PCP_CALLER_CHANGEMODE, NULL, 0);

		return PCP_OK;
    	
    }    
    else
    {
    	/*create callid and create session*/
    	if ( fuid[0] != '\0' )
    	{
    	    create_callid(fuid, callid);
    	}
    	else
    	{
    	    create_callid(fuserid, callid);
    	}
    	
    	psession = create_session(callid);
    	if (psession == NULL)
    	{
    		ulock_session();
    		return PCP_ERR;
    	}

    	psession->rc4_flag = is_local_rc4_enabled();
    	psession->call_state = PCP_STATE_INVITE;
    	psession->role = PCP_OFFER;
    	psession->meetingflag = 1;
    	psession->ice_flag = false;
    	psession->atype = get_account_type();
    	
    	if ( fuid[0] != '\0' )
    	{
    	    strcpy(psession->fuid, fuid);
    	}

    	if ( fuserid[0] != '\0' )
    	{
    	    strcpy(psession->fuserid, fuserid);
    	}
    	strcpy(psession->fphone, fphone);

    	psession->conf_state = create_conference();
    	if ( psession->conf_state == NULL )
	    {
	        delete_session(psession);
	        ulock_session();	        
            ms_error("pcp_build_conf_call_req: conference create failed.");
			return PCP_ERR;
	    }
	    
    	psession->conf_state->conftype = dial_para->conftype;
    	psession->conf_state->roomstate = E_PCP_CONF_STATE_CREATED;
    	strcpy(psession->conf_state->conference_id, callid);

    	/*create sdp for this session*/
        if ( dial_para->conftype == E_PCP_CONF_TYPE_NORMAL )
        {
        	psession->lsdp = create_sdp(1, video_enabled);
        }
        
    	if ( dial_para->roomname[0] != '\0' )
    	{
            strcpy(psession->conf_state->roomname, dial_para->roomname);
        }
        if ( dial_para->roompwd[0] != '\0' )
        {
            strcpy(psession->conf_state->roompwd, dial_para->roompwd);
        }
        if ( dial_para->remark[0] != '\0' )
        {
            strcpy(psession->conf_state->remark, dial_para->remark);
        }        

    	res_st(&callreq, sizeof(callreq));
    	callreq.sflag = get_upate_flag() ^ 0x8 ^ 0x4;
    	callreq.atype = psession->atype;
    	callreq.conftype = psession->conf_state->conftype;
    	get_proxy_version(callreq.ver);
    	strcpy(callreq.call_id, psession->callid);
    	strcpy(callreq.fuid, psession->fuid);
		strcpy(callreq.fuserid, psession->fuserid);
    	strcpy(callreq.fphone, fphone);
        strcpy(callreq.roomname, psession->conf_state->roomname);
        strcpy(callreq.roompwd, psession->conf_state->roompwd);
        strcpy(callreq.remark, psession->conf_state->remark);

        for ( i = 0; i < temp_participant_num; i++ )
        {
            needAdd = 0;
            if ( dial_para->user_attr[i].uid[0] != '\0' )
            {
                needAdd = 1;
                strcpy(callreq.plist.user_attr[i].tuid, 
                        dial_para->user_attr[i].uid);                    
            }
            if ( dial_para->user_attr[i].phone[0] != '\0' )
            {
                needAdd = 1;
                strcpy(callreq.plist.user_attr[i].tphone,
                        dial_para->user_attr[i].phone);
            }
            if ( needAdd )
            {
                callreq.plist.user_attr[i].call_mode = dial_para->user_attr[i].call_mode;
                callreq.plist.num++;
            }
        }

        if ( callreq.plist.num > MAX_PARTICIPANT_ATTR_NUM )
    	{
    	    delete_session(psession);
    		ulock_session();
    		report_call_event(PCP_CONFERENCE_EV, \
    		                  PCP_CONF_PARTICIPANT_OVER, NULL, 0);
    		                             
    		ms_error("pcp_build_conf_call_req: participant_num(%d) > %d", \
    		         temp_participant_num, MAX_PARTICIPANT_ATTR_NUM);    		
    		return PCP_ERR;
    	}
    	
        if ( callreq.conftype == E_PCP_CONF_TYPE_NORMAL )
        {
    	    callreq.sdp = psession->lsdp;
    	    
    	    /*update rtpp params for this session*/
    	    res_st(&rtpp_info, sizeof(rtpp_info));
        	get_rtpp_cfg(&rtpp_info);
        	if (rtpp_info.rtpp_num != 0)
        		update_sdp_rtpp_params(psession->lsdp, &rtpp_info);
    	}
        
    	res_st(&rh, sizeof(rh));
    	rh.sn = cm_sceq();
    	rh.type = psession->msg_type = PCP_CONF_TP;
    	rh.op = PCP_MEET_CALL_REQ_OP;
    	rh.enc = psession->rc4_flag;
#if defined(SKT_PROTOCOL_SUPPORT_ENABLED)
        if ( pcp_is_tlv_enabled() && is_local_compress_enabled())
        {
            rh.enc |= PCP_ENC_TYPE_COMPRESS;
        }
#endif
    	rh.atype = psession->atype;
    	if ( callreq.plist.user_attr[1].tuid != NULL )
    		rh.tuid = cm_stoi64(callreq.plist.user_attr[1].tuid);	


        if ( pcp_send_msg(&rh, (void *)&callreq) < 0 )
        {        
            ms_error("[pcp_build_conf_call_req] send msg failed.");
            ulock_session();
            return PCP_ERR;
        }
        
    	/*trace call log*/
    	trace_m_invite_log(psession);//������

    	/*store call param*/
    	update_call_param(psession);

    	ulock_session();

    	/*start timer t0*/
    	start_timer(TIMEOUT_T4);

    	return PCP_OK;
	}
}

int pcp_build_conf_inv_req(IN const pcp_conf_dialing_para_t *dial_para)
{
    char uid[64] = {0};
	char phone[64] = {0};
    pcp_session_t *psession = NULL;
    pcp_conf_inv_req_t invreq;
    pcp_route_t rh;
	int temp_participant_num = 0;
	int needAdd = 0;
	int i = 0;
    
    if ( NULL == dial_para )
    {
        ms_error("pcp_build_conf_inv_req: dial_para is NULL.");
        return PCP_ERR;
    }

    lock_session();
    get_userid(uid);
	get_userphone(phone);
    psession = get_session(pm_callid());
    if ( NULL == psession )
	{
		ulock_session();
        ms_error("pcp_build_conf_inv_req failed to find session.");
		return PCP_ERR;
	}

	if ( NULL == psession->conf_state )
	{
	    ulock_session();
        ms_error("pcp_build_conf_inv_req failed to find conference.");
		return PCP_ERR;
	}

	if ( psession->conf_state->roomid[0] == '\0' )
	{
	    ulock_session();
	    ms_error("pcp_build_conf_inv_req failed to find roomid.");
	    return PCP_ERR;
	}

	res_st(&invreq, sizeof(invreq));
   	strcpy(invreq.callid, pm_callid());
   	strcpy(invreq.roomid, psession->conf_state->roomid);
   	strcpy(invreq.fphone, phone);
    
	for ( i = 0; i < dial_para->user_num; i++ )
	{
	    needAdd = 0;
        if ( dial_para->user_attr[i].uid[0] != '\0' )
        {
            needAdd = 1;
            strcpy(invreq.plist.user_attr[i].tuid, 
                    dial_para->user_attr[i].uid);                    
        }
        
        if ( dial_para->user_attr[i].phone[0] != '\0' )
        {
            needAdd = 1;
            strcpy(invreq.plist.user_attr[i].tphone,
                    dial_para->user_attr[i].phone);
        }
        
        if ( needAdd )
        {
            invreq.plist.user_attr[i].call_mode = dial_para->user_attr[i].call_mode;
            invreq.plist.num++;
        }
	}

	temp_participant_num = invreq.plist.num + \
	                       psession->conf_state->pnum;
    if ( temp_participant_num > MAX_PARTICIPANT_ATTR_NUM )
    {
        ulock_session();
        report_call_event(PCP_CONFERENCE_EV, PCP_CONF_PARTICIPANT_OVER, NULL, 0);
        ms_error("pcp_build_conf_inv_req: participant_num(%d) > %d", \
    		         temp_participant_num, MAX_PARTICIPANT_ATTR_NUM); 
        return PCP_ERR;
    }

	res_st(&rh, sizeof(rh));
	rh.sn = cm_sceq();
	rh.type = psession->msg_type;
	rh.op = PCP_MEET_INV_REQ_OP;
	rh.enc = psession->rc4_flag;
    rh.atype = psession->atype;
#if defined(SKT_PROTOCOL_SUPPORT_ENABLED)
    if ( pcp_is_tlv_enabled() && is_local_compress_enabled())
    {
        rh.enc |= PCP_ENC_TYPE_COMPRESS;
    }
#endif
	rh.dsid = psession->dsid;
	if (psession->role == PCP_OFFER)
	{
		if (psession->tuid != NULL)
			rh.tuid = cm_stoi64(psession->tuid);	
	}
	else
	{
		if (psession->fuid != NULL)
			rh.tuid = cm_stoi64(psession->fuid);
	}
    
    if ( pcp_send_msg(&rh, (void *)&invreq) < 0 )
    {        
        ms_error("[pcp_build_conf_inv_req] send msg failed.");
        ulock_session();
        return PCP_ERR;
    }
    
    ulock_session();
    
    /*start timer t0*/
    start_timer(TIMEOUT_T4);
    
    return PCP_SUCCESS;
}

int pcp_build_conf_del_req(int reason)
{
    pcp_session_t *psession = NULL;
    pcp_conf_del_req_t delReq;
    pcp_route_t rh;
	
    lock_session();
    psession = get_session(pm_callid());
    if (check_session(psession) < 0)
	{
		ms_error("pcp_build_conf_del_req: Have No Usable Session !!!");
		ulock_session();
		return PCP_ERR;
	}

	if ( NULL == psession->conf_state )
	{
	    ulock_session();
        ms_error("pcp_build_conf_del_req: failed to find conference.");
		return PCP_ERR;
	}

	if ( psession->conf_state->roomid[0] == '\0' )
	{
	    ulock_session();
	    ms_error("pcp_build_conf_del_req: failed to find roomid.");
	    return PCP_ERR;
	}

	res_st(&rh, sizeof(rh));
	rh.sn = cm_sceq();
	rh.type = psession->msg_type;
	rh.op = PCP_MEET_DEL_REQ_OP;
	rh.enc = psession->rc4_flag;
    rh.atype = psession->atype;
#if defined(SKT_PROTOCOL_SUPPORT_ENABLED)
    if ( pcp_is_tlv_enabled() && is_local_compress_enabled())
    {
        rh.enc |= PCP_ENC_TYPE_COMPRESS;
    }
#endif
	rh.dsid = psession->dsid;

   	res_st(&delReq, sizeof(delReq));
   	strcpy(delReq.callid, pm_callid());
   	strcpy(delReq.roomid, psession->conf_state->roomid);

    if ( pcp_send_msg(&rh, (void *)&delReq) < 0 )
    {        
        ms_error("[pcp_build_conf_del_req] send msg failed.");
        ulock_session();
        return PCP_ERR;
    }
    
	/*upade call state to wait*/
	psession->call_state = PCP_STATE_WAIT;
	psession->conf_state->roomstate = E_PCP_CONF_STATE_CLOSED;
	call_state_cb(psession->call_state);

	ulock_session();

	/*start timer t0*/
    start_timer(TIMEOUT_T4);
    
    report_call_event(PCP_CONFERENCE_EV, PCP_CONF_FINISHED, NULL, 0);
    return PCP_SUCCESS;
}

//add by wuzhaoyang20140605 start
int build_convert_conference_rsp(void)
{
	pcp_route_t rh;
	pcp_conf_convert_rsp_t convertrsp;
	pcp_session_t* psession = NULL;

	lock_session();
	/*if callid is NULL,default get first session node */
	psession = get_session(pm_callid()); 
	if (check_session(psession) < 0)
	{
		ms_error("Have No Usable Session !!!");
		ulock_session();
		return PCP_ERR;
	}

	/*build message bson header*/
	res_st(&rh, sizeof(rh));
	rh.sn = cm_sceq();
	rh.type = psession->msg_type;
	rh.op = PCP_CONVERT_CONFERENCE_RSP_OP;
	rh.enc = psession->rc4_flag;
#if defined(SKT_PROTOCOL_SUPPORT_ENABLED)
    if ( pcp_is_tlv_enabled() && is_local_compress_enabled())
    {
        rh.enc |= PCP_ENC_TYPE_COMPRESS;
    }
#endif

	if (psession->role == PCP_OFFER)
	{
		rh.tuid = cm_stoi64(psession->tuid);
		rh.fuid = cm_stoi64(psession->fuid);
	}
	else
	{
		rh.tuid = cm_stoi64(psession->fuid);
		rh.fuid = cm_stoi64(psession->tuid);
	}

	res_st(&convertrsp, sizeof(pcp_conf_convert_rsp_t));
	convertrsp.reason = 0;
	strcpy(convertrsp.call_id, psession->callid);
    
    if ( pcp_send_msg(&rh, (void *)&convertrsp) < 0 )
    {
        ms_error("[build_convert_conference_rsp] send msg failed.");
        ulock_session();
        return PCP_ERR;
    }

	ulock_session();
	

	return PCP_OK;
}
//add by wuzhaoyang20140605 end

int pcp_get_call_trace(char *log)
{

	pcp_session_t* psession = NULL;
	int res = 0;
	int reason = 0;	

	if ( NULL == log )
	{
	    return PCP_ERR;
	}
	
    lock_session();

	/*check rsp info if match for current session info*/
	psession = get_session(pm_callid());
	if (psession)
	    trace_call_log(psession, log);

	ulock_session();
	return PCP_OK;
}

int pcp_release_call_session(void)
{
    pcp_session_t* psession = NULL;
	int res = 0;
	int reason = 0;	
	
    lock_session();

	/*check rsp info if match for current session info*/
	psession = get_session(pm_callid());
	if (psession == NULL)
	{
		ms_error("pcp_release_call_session: not match session!!!");
		ulock_session();
		return PCP_ERR;
	}

    /*update session to expired*/
	psession->expired_flag = TRUE;	
	//report_call_log(psession);

	/*upade call state to wait*/
	psession->call_state = PCP_STATE_WAIT;
	call_state_cb(psession->call_state);
	
	/*delete session */
	delete_session(psession);
	
	/*stop all timer*/
	stop_all_timers();
	pm_clean();
	ulock_session();
	
	return PCP_SUCCESS;
}

char* pcp_get_event_name(int evType, char *name)
{
    switch(evType)
    {
        case PCP_NONE_REGISTER_EV:
        strcpy(name, "REGISTER");
        break;

        case PCP_CALLDIALING_EV:
        strcpy(name, "CALL DIALING");
        break;
        	
        case PCP_CALLINCOMING_EV:
        strcpy(name, "CALL INCOMING");
        break;
         
        case PCP_CALLANSWER_EV:
        strcpy(name, "CALL ANSWER");
        break;
        	
        case PCP_CALLHUNGUP_EV:
        strcpy(name, "CALL HUNGUP");
        break;
        	
        case PCP_CALLUPDATE_EV:
        strcpy(name, "CALL UPDATE");
        break;
        	
        case PCP_CALLTIMEOUT_EV:
        strcpy(name, "CALL TIMEOUT");
        break;
        	
        case PCP_CONFERENCE_EV:
        strcpy(name, "CONFERENCE");
        break;
           
        case PCP_DTMF_EV:
        strcpy(name, "GET DTMF");
        break;

        case PCP_START_VIDEO_EV:
        strcpy(name, "START_VIDEO");
        break;

        case PCP_STOP_VIDEO_EV:
        strcpy(name, "STOP_VIDEO");
        break;

        case PCP_SET_CEMERA_EV:
        strcpy(name, "SET_CEMERA");
        break;

        case PCP_IN_TRANS_REQ_EV:
        strcpy(name, "IN_TRANS_REQ");    
        break;

        case PCP_IN_TRANS_RSP_EV:
        strcpy(name, "IN_TRANS_RSP");    
        break;

        default:
        strcpy(name, "*UNKNOWN*");
        break;
    }

    return name;
}

int pcp_get_conftype(void)
{
    int conftype = E_PCP_CONF_TYPE_NORMAL;
	pcp_session_t* psession = NULL;

	lock_session();
	psession = get_session(pm_callid());
	if ( psession == NULL )
	{
	    ms_error("pcp_get_conftype: not find match session. callid[%s]", pm_callid());
	    ulock_session();
	    return conftype;
	}
	if ( psession->conf_state == NULL )
	{
	    ms_error("pcp_get_conftype: not find conference.");
	    ulock_session();
	    return conftype;
	}

	conftype = psession->conf_state->conftype;

	ulock_session();
	return conftype;
}

int pcp_get_confstate(void)
{
    int confState = E_PCP_CONF_STATE_CLOSED;
    pcp_session_t* psession = NULL;

	lock_session();
	psession = get_session(pm_callid());
	if ( psession == NULL )
	{
	    ms_error("pcp_get_confstate: not find match session. callid[%s]", pm_callid());
	    ulock_session();
	    return confState;
	}
	if ( psession->conf_state == NULL )
	{
	    ms_error("pcp_get_confstate: not find conference.");
	    ulock_session();
	    return confState;
	}

	confState = psession->conf_state->roomstate;
	
	ulock_session();

	return confState;
}

int pcp_get_meetflag(void)
{
    int inmeeting = 0;
    pcp_session_t* psession = NULL;

	lock_session();
	psession = get_session(pm_callid());
	if ( psession == NULL )
	{
	    ms_error("pcp_get_meetflag: not find match session. callid[%s]", pm_callid());
	    ulock_session();
	    return inmeeting;
	}

	inmeeting = psession->meetingflag;
	ulock_session();

	return inmeeting;
}

//add by wuzhaoyang20140605 end

//add by luochengcong20141022
int pcp_get_call_info_report(OUT pcp_call_info_t *pCallInfo )
{
    if ( NULL != pCallInfo ) {
        memcpy(pCallInfo, &g_call_info_report, sizeof(pcp_call_info_t));
    }
    return PCP_OK;
}

int pcp_reset_call_info_report()
{
    memset(&g_call_info_report, 0x00, sizeof(pcp_call_info_t));
	
    return PCP_OK;
}

int pcp_update_call_info_report(OUT pcp_call_info_t* pCallInfo, IN pcp_session_t* psession)
{
    char codec[64]={0};
    if ( NULL == psession || NULL == pCallInfo ) {
        return PCP_ERR;
    }
    
    if (psession->role == PCP_ANSWER || psession->mode != PCP_DIRECT_CM)
    {
    	pCallInfo->cmode = PCP_FREE_CALL_TP;
    }
    else
    {    
    	pCallInfo->cmode = PCP_DIRECT_CALL_TP;
    }
    pCallInfo->role = psession->role;
	pCallInfo->ctime = psession->ctime;
	strcpy(pCallInfo->callid, psession->callid);

	
	/* del for no need for video */
#if 0
    if (psession->video_flag) {
        pm->mmode = psession->video_stream.trans_mode;
        sprintf(codec, "%d",psession->video_stream.session_pt);
        strcpy(pm->mcodec, codec);
        strcpy(pm->mgw, psession->video_stream.remote_addr);
    } else 
#endif
    if ( (psession->meetingflag == 0)
        || ( (psession->meetingflag == 1) 
            && (( psession->role == PCP_ANSWER)
                || (psession->conf_state->conftype == E_PCP_CONF_TYPE_NORMAL))))
    {
		pcp_getptypename(psession->audio_stream.session_pt, codec);
        strcpy(pCallInfo->mcodec, codec);
        strcpy(pCallInfo->mgw, psession->audio_stream.remote_addr);

		//add by yujun
	pCallInfo->mgwport = psession->audio_stream.remote_port;
    }
    return PCP_OK;
}

int pcp_update_callinfo_mmode(int iceMode)
{
	g_call_info_report.mmode = iceMode == 0 ? 1 : 0;
	return PCP_OK;
}

int pcp_update_callinfo_sgw(IN const char* tcpAddr)
{
	if ( NULL == tcpAddr )
	{
		return PCP_ERR;
	}
	strncpy(g_call_info_report.sgw, tcpAddr, sizeof(g_call_info_report.sgw)-1);
	return PCP_OK;
}

int pcp_update_callinfo_cstate(IN pcp_cstate_e callstate)
{
	int preState = g_call_info_report.cstate;
	if ( (preState == E_PCP_CSTATE_UP_SINGLEPASS_FAILED 
			&& callstate == E_PCP_CSTATE_DOWN_SINGLEPASS_FAILED)
		|| (preState == E_PCP_CSTATE_DOWN_SINGLEPASS_FAILED 
			&& callstate == E_PCP_CSTATE_UP_SINGLEPASS_FAILED) )
	{
		g_call_info_report.cstate = E_PCP_CSTATE_BOTHWAY_BLOCK_FAILED;
	}
	else
	{
		g_call_info_report.cstate = callstate;
	}

	return PCP_OK;
}

int pcp_update_callinfo_mic_ismute(IN int ismute)
{
	g_call_info_report.ismute = ismute;
	return PCP_OK;
}

int pcp_update_callinfo_spreason(IN int spreason)
{
	g_call_info_report.spreason = spreason;
	return PCP_OK;
}
//add by luochengcong20141022 end

int pcp_set_normalcall_to_conference(IN int ntoc)
{
	normalcall_to_conference=ntoc;
	return PCP_OK;
}

int pcp_get_normalcall_to_conference(void)
{
	return normalcall_to_conference;
}

unsigned int getTimeInMs()
{
#if defined(_WIN32) || defined(WIN32)
    return timeGetTime();
#else
    struct timeval tv;
    struct timezone tz;
    unsigned long val;

    gettimeofday(&tv, &tz);
    val = tv.tv_sec * 1000 + tv.tv_usec / 1000;
    return val;
#endif
}


void pcp_update_start_time(void)
{
	pcp_session_t* psession = NULL;

	lock_session();
	psession = get_session(pm_callid());
	if (psession != NULL)
	{
		psession->start_time = getTimeInMs();
	}
	ulock_session();
}

void pcp_update_ctime(void)
{
	const unsigned int MAX_UWORD32 = 0xffffffff;

	pcp_session_t* psession = NULL;
	unsigned int offset  = -1;

	//lock_session();
	psession = get_session(pm_callid());
	if ( psession != NULL )
	{
		offset = getTimeInMs();

		if(offset< psession->start_time)
		{
			 offset += MAX_UWORD32 - psession->start_time + 1;
		}
		else
		offset -= psession->start_time;	    
		psession->ctime = offset;
	}
	//ulock_session();
}
