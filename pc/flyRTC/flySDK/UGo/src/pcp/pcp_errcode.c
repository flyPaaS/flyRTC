#include "pcp_errcode.h"

static pcp_bool is_pass_through_reason(int reason);

pcp_bool is_pass_through_reason(int reason)
{
	pcp_bool is_psth = FALSE;

	if (reason >= PCP_PSTH_REASON_MIN_CODE && reason < PCP_PSTH_REASON_MAX_CODE)
		is_psth = TRUE;

	return is_psth;
}


int pcp_errcode_str(IN int* errcode, OUT char* perrstr)
{
	if (NULL == perrstr)
		return PCP_ERR;

	switch(*errcode)
	{
	case PCP_SUCCESS:
		sprintf(perrstr, "Success!");
		break;
	case PCP_RINGING:
		sprintf(perrstr, "Call Ringging...");
		break;
	case PCP_NOTACCEPT:
		sprintf(perrstr, "Not Accept,Maybe media not support!");
		break;
	case PCP_RTPPTIMEOUT:
		sprintf(perrstr, "Rtpp Timeout,Recv rtp timeout!");
		break;
	case PCP_NOBALANCE:
		sprintf(perrstr, "Nobalance!");
		break;
	case PCP_UPDATEMEDIAFAIL:
		sprintf(perrstr, "Update media fial!");
		break;
	case PCP_BUSY:
		sprintf(perrstr, "Busy Now!");
		break;
	case PCP_REJECT:
		sprintf(perrstr, "Reject by peer!");
		break;
	case PCP_NUMBERTOOSHORT:
		sprintf(perrstr, "Callee Frozen(not exist or not online)!");
		break;
	case PCP_NOTFIND:
		sprintf(perrstr, "NotFind, Maybe callee not exist or not online!");
		break;
	case PCP_CALLEEFROZEN:
		sprintf(perrstr, "Callee Frozen!");
		break;
	case PCP_CALLERFROZEN:
		sprintf(perrstr, "Caller Frozen!");
		break;
	case PCP_CALLEREXPIRED:
		sprintf(perrstr, "Caller Expired!");
		break;
	case PCP_CALLERCANCEL:
		sprintf(perrstr, "Call Cancel!");
		break;
	case PCP_NUMBERBIND:
		sprintf(perrstr, "The number have bond! can`t call myself bind phone.");
		break;
	case PCP_NORESPONSE:
		sprintf(perrstr, "No Response, Maybe network is error!");
		break;
	case PCP_NETWORKDISABLE:
		sprintf(perrstr, "The network is not supported!");
		break;
	case PCP_UNREACHABLE:
		sprintf(perrstr, "Signaling inaccessible(NACK)!");
		break;
	case PCP_UNABLETOPUSH:
		sprintf(perrstr, "Ios unable to push!");
		break;
	case PCP_PROXYAUTH:
		sprintf(perrstr, "VPS Proxy Auth!");
		break;
	case PCP_MSGHEADERR:
		sprintf(perrstr, "Vps Message Head Error!");
		break;
	case PCP_MSGBODYERR:
		sprintf(perrstr, "Vps Message Body Error!");
		break;
	case PCP_CALLIDEXIST:
		sprintf(perrstr, "Vps Callid Exist!");
		break;
	case PCP_MSGTIMEOUT:
		sprintf(perrstr, "Vps Message Timeout!");
		break;
    case PCP_USERIDNOTEXIST:
        sprintf(perrstr, "UserId Not Exist!");
        break;
	case PCP_NOTIFYPEERNOTFIND:
		sprintf(perrstr, "Vps direct notify for peer uid not find!");
		break;
	case PCP_NOTIFYPEEROFFLINE:
		sprintf(perrstr, "Vps direct notify for peer offline!");
		break;
	case PCP_NOTIFYTIMEOUT:
		sprintf(perrstr, "Vps direct notify for timeout!");
		break;
	case PCP_CALLIDNOTEXIST:
		sprintf(perrstr, "Session Not Exist!");
		break;
	case PCP_NOANSWER:
		sprintf(perrstr, "Callee Have No Answer!");
		break;
	case PCP_CONNECTFAILD:
		sprintf(perrstr, "Call Connect Faild!");
		break;
    case PCP_BLACKLISTFAILED:
        sprintf(perrstr, "Callee in blacklist!");
        break;
	case PCP_CONNECTING:
		sprintf(perrstr, "Call is Connecting!");
		break;
	case PCP_HUNGUPTCPDISCONNECTED:
		sprintf(perrstr, "Tcp event:Server connect failed!");
		break;
	case PCP_HUNGUPRTPTIMEOUT:
		sprintf(perrstr, "medie engine: RTP time out!!!");
		break;
	case PCP_HUNGUPPEER:
		sprintf(perrstr, "Call Hangup By Peer!");
		break;
	case PCP_HUNGUPMYSELF:
		sprintf(perrstr, "Call Hangup By Myself!");
		break;
    /* add by Vinton.Liu on 2016-03-02 for add send failed error code begin */
    case PCP_STARTSENDFAILED:
        sprintf(perrstr, "Start send failed!");
        break;
    /* Vinton.Liu add end */
    case PCP_IN_TRANS_TIMEOUT:
        sprintf(perrstr, "In trans rsp timeout");
        break;
		//add by wuzhaoyang20140621
	case PCP_CONF_NO_EXIST:
		sprintf(perrstr, "Find the Conference Fail!");
		break;
	case PCP_CONF_STATE_ERR:
		sprintf(perrstr, "Conference State Error!");
		break;
	case PCP_CONF_FULL:
		sprintf(perrstr, "Conference Full!");
		break;
	case PCP_CONF_CREATE_ERR:
		sprintf(perrstr, "Create Conference Fail!");
		break;
	case PCP_CONF_CALL_FAILED:
		sprintf(perrstr, "Call Procedure Fail!");
		break;
	case PCP_CONF_MEDIA_FAILED:
		sprintf(perrstr, "Apply Media Resource Fail!");
		break;
	case PCP_CONF_TER_UNSUPPORT:
		sprintf(perrstr, "The Peer Don't Support!");
		break;
    case PCP_CONF_PARTICIPANT_OVER:
        sprintf(perrstr, "Too many participants");
        break;
    case PCP_CONF_FINISHED:
        sprintf(perrstr, "Conference finished!");
        break;
		//add by wuzhaoyang20140605 start
	case PCP_GETSTATE:
		sprintf(perrstr, "Conference State Update!");
		break;
	case PCP_CALLER_CHANGEMODE:
		sprintf(perrstr, "Active change mode to RTPP!");
		break;
	case PCP_CALLEE_CHANGEMODE:
		sprintf(perrstr, "Passive change mode to RTPP!");
		break;
		//add by wuzhaoyang20140605 end
	default:
		/* BEGIN: Added by gonghuojin, 2014/7/24   add Pass-through code */
		if ( is_pass_through_reason(*errcode) )
		{
			sprintf(perrstr, "Pass-through error[%d]!", *errcode);
		}
		else
		{
			sprintf(perrstr, "Unkown Error[%d]!", *errcode);
			//*errcode = PCP_UNKOWNERR;
		}

		/* END:   Added by gonghuojin, 2014/7/24 */
		break;
	}

	return PCP_OK;
}


