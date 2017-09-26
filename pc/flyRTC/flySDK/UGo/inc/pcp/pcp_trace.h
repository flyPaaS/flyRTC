/******************************************************************************

  Copyright (C), 2001-2011, DCN Co., Ltd.

 ******************************************************************************
  File Name     : pcp_trace.h
  Version       : Initial Draft
  Author        : gonghuojin
  Created       : 2013/10/18
  Last Modified :
  Description   : pcp log trace file header
  Function List :
  History       :
  1.Date        : 2013/10/18
    Author      : gonghuojin
    Modification: Created file

******************************************************************************/
#ifndef PCP_TRACE_H
#define PCP_TRACE_H
	
#include "pcp_cfg.h"

PCP_EXTERN_C_START

void null_trace(void);

/*****debug level ***/
enum pcp_trace_level
{
    PCP_TRACE_NONE              = 0x0000,   // no trace
    PCP_TRACE_INFO          	= 0x0001,
    PCP_TRACE_WARNING           = 0x0002,
    PCP_TRACE_ERROR             = 0x0004,
    PCP_TRACE_DEBUG             = 0x0800,	// debug
    PCP_TRACE_REPORT			= 0x4000,   //calling report
};

int trace_init(void);

void trace_destroy(void);

OUT pcp_trace_t* create_trace_proxy(void);

void free_trace_proxy(IN pcp_trace_t* ptraceproxy);

int trace_log(IN pcp_session_t* psession, IN const char* describe);

int trace_invite_log(IN pcp_session_t* psession);

int trace_notify_log(IN pcp_session_t* psession);

int trace_callreq_ack_log(IN pcp_session_t* psession);

int trace_ringing_log(IN pcp_session_t* psession);

int trace_answer_log(IN pcp_session_t* psession, IN int reason);

int trace_ack_log(IN pcp_session_t* psession);

int trace_nack_log(IN pcp_session_t* psession, IN int nack);

int trace_hangup_log(IN pcp_session_t* psession, IN int reason);

int trace_route_update_log(IN pcp_session_t* psession);

int trace_stream_log(IN pcp_session_t* psession);

int trace_linechange_log(IN pcp_session_t* psession, IN int mode);

int trace_call_log(IN pcp_session_t* psession, OUT char* pcalllog);


int trace_m_invite_log(IN pcp_session_t* psession);//add by wuzhaoyang20140605

int trace_m_notify_log(IN pcp_session_t* psession);//add by wuzhaoyang20140605

int trace_convert_conference_log(IN pcp_session_t* psession);//add by wuzhaoyang20140605

PCP_EXTERN_C_END
	
#endif





