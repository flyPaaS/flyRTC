/******************************************************************************

  Copyright (C), 2001-2011, DCN Co., Ltd.

 ******************************************************************************
  File Name     : pcp_errcode.h
  Version       : Initial Draft
  Author        : gonghuojin
  Created       : 2013/9/27
  Last Modified :
  Description   : pcp errer code config file
  Function List :
  History       :
  1.Date        : 2013/9/27
    Author      : gonghuojin
    Modification: Created file

******************************************************************************/
#ifndef PCP_ERRCODE_H
#define PCP_ERRCODE_H
#include "pcp_comm.h"


PCP_EXTERN_C_START

void null_errcode();

/*temporary reason 40~49*/
#define PCP_TMP_REASON_MIN_CODE  80 
#define PCP_TMP_REASON_MAX_CODE  98

/*Pass-through reason 10000~20000*/
#define PCP_PSTH_REASON_MIN_CODE 10000
#define PCP_PSTH_REASON_MAX_CODE 20000

enum pcp_event_reason
{
    /**success reason**/
    PCP_SUCCESS					= 0,    //success

    /*public reason1~29*/
    PCP_NOTACCEPT				= 1, 	//not accept
    PCP_RTPPTIMEOUT				= 2, 	//RTPP RTP timeout
    PCP_NOBALANCE				= 3, 	//nobalance
    PCP_UPDATEMEDIAFAIL			= 4, 	//update media fail
    PCP_BUSY					= 5, 	//busy
    PCP_REJECT					= 6, 	//reject
    PCP_NOTFIND					= 7, 	//not find
    PCP_NUMBERTOOSHORT			= 8, 	//Forbidden(callee number too short)
    PCP_CALLEEFROZEN			= 9, 	//callee have been frozen
    PCP_CALLERFROZEN			= 10,	//caller have been frozen
    PCP_CALLEREXPIRED			= 11, 	//caller expired
    PCP_CALLERCANCEL			= 12, 	//Terminater for Cancel 
    PCP_NUMBERBIND				= 13, 	//Forbidden(caller binding number)
    PCP_NORESPONSE				= 14, 	//no response timeout
    PCP_NETWORKDISABLE			= 15,   //The network is not supported
    PCP_UNREACHABLE				= 16,   //Signaling inaccessible(NACK)	
    PCP_UNABLETOPUSH			= 17,   //ios unable to push
    PCP_CALLIDNOTEXIST			= 18,   //callid is not exist
    PCP_NOANSWER				= 19, 	//callee have no answer
    PCP_CONNECTFAILD			= 20,	//connect faild
    PCP_BLACKLISTFAILED         = 23,   // callee in blacklist

    /*client reason 30~49*/
    PCP_HUNGUPMYSELF			= 30,   //call hungup by myself
    PCP_HUNGUPPEER				= 31,   //call hungup by peer
    PCP_HUNGUPTCPDISCONNECTED	= 32,   //"tcp event:server connect failed!!!"
    PCP_HUNGUPRTPTIMEOUT		= 33,   //"medie engine: RTP time out!!!"
    PCP_STARTSENDFAILED         = 34,   // start send failed

    /**vps sever reason 50~79**/
    PCP_PROXYAUTH				= 50, 	//proxy auth 
    PCP_MSGHEADERR				= 51, 	//message head error
    PCP_MSGBODYERR				= 52, 	//message body error
    PCP_CALLIDEXIST				= 53, 	//callid exist
    PCP_MSGTIMEOUT				= 54, 	//message timeout
    PCP_USERIDNOTEXIST          = 55,   // user not exist

    PCP_CONF_NO_EXIST           = 60,   //find the conference fail
    PCP_CONF_STATE_ERR          = 61,   //conference state error
    PCP_CONF_FULL               = 62,   //conference full
    PCP_CONF_CREATE_ERR         = 63,   //create conference fail
    PCP_CONF_CALL_FAILED        = 64,   //call procedure fail
    PCP_CONF_MEDIA_FAILED       = 65,   //apply media resource fail
    PCP_CONF_TER_UNSUPPORT      = 66,   //the peer don't support	
    PCP_CONF_PARTICIPANT_OVER   = 67,   // too many participant
    PCP_CONF_FINISHED           = 68,   // conference finished

    //add by wuzhaoyang20140605 start
    PCP_GETSTATE                = 70,   //get the conference state
    PCP_CALLER_CHANGEMODE       = 71,   //caller convert the mode(from the P2P to RTPP)
    PCP_CALLEE_CHANGEMODE       = 72,   //callee convert the mode(from the P2P to RTPP)  
    //add by wuzhaoyang20140605 end

    /*temporary reason 80~98(notify:80~89,other:90~98)*/
    PCP_NOTIFYPEERNOTFIND    	= 80,   //call notify peer uid not find
    PCP_NOTIFYPEEROFFLINE       = 81,   //call notify peer offline
    PCP_NOTIFYTIMEOUT   		= 82,   //call notify free call timeout

    PCP_IN_TRANS_TIMEOUT        = 90,   // in trans rsp timeout
    PCP_CONNECTING				= 97,	//connecting between send invite and don't receive response
    PCP_RINGING					= 98,   //Ringing response

    /**unkown error**/
    PCP_UNKOWNERR				= 99,	//unkown error
};


int pcp_errcode_str(IN int* errcode, OUT char* perrstr);


PCP_EXTERN_C_END

#endif

