#include "pcp_trace.h"
#include "pcp_sdp.h"
#include "pcp_errcode.h"
#include "logtrace.h"


static ms_mutex_t g_tracelock;
static void trace_mutex_init(void);
static void trace_mutex_destroy(void);
static void trace_lock(void);
static void trace_unlock(void);

static void reset_trace(pcp_trace_t* ptrace);
static void trace_appand(pcp_trace_t* ptrace, const char* tracelog);
static void trace_buff(pcp_trace_t* ptrace, char* trace_buf);
static int trace_media_sdp(pcp_sdp_info_t* psdp, char* pmedia);
static int trace_media_stream(pcp_session_media_t* pmedia, char* pstream);


void trace_mutex_init(void)
{
	ms_mutex_init(&g_tracelock, NULL);
}

void trace_mutex_destroy(void)
{
	ms_mutex_destroy(&g_tracelock);
}

void trace_lock(void)
{
	ms_mutex_lock(&g_tracelock);
}

void trace_unlock(void)
{
	ms_mutex_unlock(&g_tracelock);
}

int trace_media_sdp(pcp_sdp_info_t* psdp, char* pmedia)
{
	int i = 0; 	
	int a_port = 0;
	int v_port = 0;
	char a_pt[64] = {0};
	char v_pt[64] = {0};
	char tmp[64] = {0};
	pcp_bool v_flag = FALSE;
	
	if (psdp && pmedia)
	{
		for (i = 0; i < psdp->media.media_num; i++)
		{	
			if (strcmp(psdp->media.media_arr[i].type, "a") == 0)
			{
				sprintf(tmp, "%d ", psdp->media.media_arr[i].pt);
				strcat(a_pt, tmp);
				a_port = psdp->media.media_arr[i].port;
			}
		}

		for (i = 0; i < psdp->media.media_num; i++)
		{	
			if (strcmp(psdp->media.media_arr[i].type, "v") == 0)
			{
				sprintf(tmp, "%d ", psdp->media.media_arr[i].pt);
				strcat(a_pt, tmp);
				v_port = psdp->media.media_arr[i].port;
				v_flag = TRUE;
			}
		}

		if (v_flag)
			sprintf(pmedia, "c=%s a{%d,[%s]} v{%d,[%s]}", psdp->c_ip, a_port, a_pt, v_port, v_pt);
		else
			sprintf(pmedia, "c=%s a{%d,[%s]}", psdp->c_ip, a_port, a_pt);
	}

	return PCP_OK;
}


int trace_media_stream(pcp_session_media_t* pmedia, char* pstream)
{
	if (pmedia && pstream)
	{
		sprintf(pstream, "%s:%d <--> %s:%d %d", pmedia->local_addr, pmedia->local_port,
				pmedia->remote_addr, pmedia->remote_port, pmedia->session_pt);
	}

	return PCP_OK;
}

void reset_trace(pcp_trace_t* ptrace)
{
	if (ptrace != NULL)
	{
		memset(ptrace->trace_buf, 0, MAX_TRACE_BUF_SIZE);
		ptrace->left = MAX_TRACE_BUF_SIZE;
		ptrace->used = 0;
	}
}

void trace_appand(pcp_trace_t* ptrace, const char* tracelog) 
{
	int len = 0; 
	char* p = NULL;

	if (tracelog != NULL)
	{
		if (ptrace != NULL)
		{
			trace_lock();
			len = strlen(tracelog);
			if(ptrace->left < len + 5)
				reset_trace(ptrace);
			p = ptrace->trace_buf + ptrace->used;
			memcpy(p, tracelog,len);	
			ptrace->left -= len;
			ptrace->used += len;
			trace_unlock();
		}
	} 
}

void trace_buff(pcp_trace_t* ptrace, char* trace_buf)
{
	if (trace_buf != NULL && ptrace != NULL)
	{
		trace_lock();
		memcpy(trace_buf, ptrace->trace_buf, ptrace->used);
		trace_buf[ptrace->used] = '\0';
		trace_unlock();
	}
}


/*****************************************
	
		Trace Api function defined

******************************************/
int trace_init(void)
{
	ms_init();
	trace_mutex_init();
	return PCP_OK;
}

void trace_destroy(void)
{
	trace_mutex_destroy();
	ms_exit();
}

OUT pcp_trace_t* create_trace_proxy(void)
{
	pcp_trace_t* ptraceproxy = NULL;

	ptraceproxy = (pcp_trace_t*)ms_malloc0(PCP_TRACE_T_SIZE);
	if (ptraceproxy != NULL)
	{
		trace_lock();
		reset_trace(ptraceproxy);
		trace_unlock();
	}
	
	return ptraceproxy;
}


void free_trace_proxy(IN pcp_trace_t* ptraceproxy)
{
	if (ptraceproxy != NULL)
	{
		trace_lock();
		ms_free(ptraceproxy);
		ptraceproxy = NULL;
		trace_unlock();
	}
}

int trace_log(IN pcp_session_t* psession, IN const char* describe)
{
	if (psession != NULL)
	{
		trace_appand(psession->call_log, describe);		
	}

	return PCP_OK;
}


int trace_invite_log(IN pcp_session_t* psession)
{
	char media_str[128] = {0};
	char trace_log[256] = {0};
	
	if (psession != NULL)
	{
		if (psession->role == PCP_OFFER)
		{
			trace_media_sdp(psession->lsdp, media_str);
			sprintf(trace_log, "CALL_LOG:CALLID:%s CALLER:%s CALLEE:[%s %s] ROLE:OFFER\r\n", 
			psession->callid, psession->fuid, psession->tuid, psession->tphone);
			trace_appand(psession->call_log, trace_log);

			sprintf(trace_log, "(S)---call req(%s)-->(R)\r\n", media_str);
			trace_appand(psession->call_log, trace_log);
		}
		else
		{
			trace_media_sdp(psession->rsdp, media_str);
			sprintf(trace_log, "CALL_LOG:CALLID:%s CALLER:%s CALLEE:[%s %s] ROLE:ANSWER\r\n", 
			psession->callid, psession->fuid, psession->tuid, psession->tphone);
			trace_appand(psession->call_log, trace_log);

			sprintf(trace_log, "(S)<--call req(%s)---(R)\r\n", media_str);
			trace_appand(psession->call_log, trace_log);
		}
	}

	return PCP_OK;
}


int trace_notify_log(IN pcp_session_t* psession)
{
	char trace_log[256] = {0};

	if (psession != NULL)
	{
		sprintf(trace_log, " | <--call notify(direct)--- |\r\n");
		trace_appand(psession->call_log, trace_log);
	}
	
	return PCP_OK;
}

int trace_callreq_ack_log(IN pcp_session_t* psession)
{
	char trace_log[256] = {0};

	if (psession != NULL)
	{
		sprintf(trace_log, " | <--call req ack--- |\r\n");
		trace_appand(psession->call_log, trace_log);
	}
	
	return PCP_OK;
}

int trace_ringing_log(IN pcp_session_t* psession)
{
	//char media_str[128] = {0};
	char trace_log[256] = {0};

	if (psession != NULL)
	{
		if (psession->role == PCP_OFFER)
		{
			//trace_media_sdp(psession->rsdp, media_str);
			//sprintf(trace_log, " | <--call ring(%s)--- |\r\n", media_str);
			sprintf(trace_log, " | <---call ring(pt %d)--- |\r\n", psession->audio_stream.session_pt);
		}
		else
		{
			//trace_media_sdp(psession->lsdp, media_str);
			//sprintf(trace_log, " | ---call ring(%s)--> |\r\n", media_str);
			sprintf(trace_log, " | ---call ring(pt %d)--> |\r\n", psession->audio_stream.session_pt);
		}
		
		trace_appand(psession->call_log, trace_log);
	}
	
	return PCP_OK;
}

int trace_answer_log(IN pcp_session_t* psession, int reason)
{
	//char media_str[128] = {0};
	char trace_log[256] = {0};

	if (psession != NULL)
	{	
		if (psession->role == PCP_OFFER)
		{	
			if (reason == PCP_SUCCESS)
			{
				sprintf(trace_log,
				        " | <--call rsp(pt %d dfec %d afec %d mfec %d vfec %d vnack %d)--- |\r\n", 
				        psession->audio_stream.session_pt, 
				        psession->directfec_flag, 
				        psession->afec_flag, 
				        psession->directfec_mulriple_flag,
				        psession->vfec_flag,
				        psession->vnack_flag);
			}
			else
			{
				sprintf(trace_log, " | <--call rsp(%d)--- |\r\n(E)call terminate.", reason);
			}
		}
		else
		{	
			if (reason == PCP_SUCCESS)
			{
				sprintf(trace_log, " | <--call rsp(pt %d dfec %d ffec %d mfec %d vfec %d vnack %d)--- |\r\n", 
				    psession->audio_stream.session_pt, 
				    psession->directfec_flag, 
				    psession->afec_flag,
				    psession->directfec_mulriple_flag,
				    psession->vfec_flag,
    		        psession->vnack_flag);
			}
			else
			{
				sprintf(trace_log, " | ---call rsp(%d)--> |\r\n(E)call terminate.", reason);
			}	
		}	
		trace_appand(psession->call_log, trace_log);
	}
	
	return PCP_OK;
}

int trace_ack_log(IN pcp_session_t* psession)
{
	char trace_log[256] = {0};

	if (psession != NULL)
	{
		if (psession->role== PCP_OFFER)
		{
			sprintf(trace_log, " | ---call ack--> |\r\n");
		}
		else
		{
			sprintf(trace_log, " | <--call ack--- |\r\n");
		}
		
		trace_appand(psession->call_log, trace_log);
	}	
	
	return PCP_OK;
}

int trace_nack_log(IN pcp_session_t* psession, IN int nack)
{
	char trace_log[256] = {0};

	if (psession != NULL)
	{
		sprintf(trace_log, " | <--route nack[%d]--- |\r\n(E)call end.", nack);
		trace_appand(psession->call_log, trace_log);
	}	
	
	return PCP_OK;
}

int trace_hangup_log(IN pcp_session_t* psession, IN int reason)
{
	char trace_log[256] = {0};

	if (psession != NULL)
	{
		if ( reason != PCP_HUNGUPPEER)
		{
			sprintf(trace_log, " | ---call bye[%d]--> |\r\n(E)call end.", reason);
		}
		else
		{
			sprintf(trace_log, " | <--call bye[%d]--- |\r\n(E)call end.", reason);
		}
		
		trace_appand(psession->call_log, trace_log);
	}

	return PCP_OK;
}

int trace_route_update_log(IN pcp_session_t* psession)
{
	char trace_log[256] = {0};

	if (psession != NULL)
	{
		sprintf(trace_log, " | ---route update--> |\r\n");
		trace_appand(psession->call_log, trace_log);
	}

	return PCP_OK;
}


int trace_stream_log(IN pcp_session_t* psession)
{
	char media_audio[128] = {0};
	char media_video[128] = {0};
	char trace_log[256] = {0};

	if (psession != NULL)
	{
		trace_media_stream(&psession->audio_stream, media_audio);
		sprintf(trace_log, " | ---audio[%s]--- |\r\n", media_audio);
		trace_appand(psession->call_log, trace_log);
		if (psession->video_flag)
		{
			trace_media_stream(&psession->video_stream, media_video);
			sprintf(trace_log, " | ---video[%s]--- |\r\n", media_video);
			trace_appand(psession->call_log, trace_log);
		}
	}

	return PCP_OK;
}

int trace_linechange_log(IN pcp_session_t* psession, IN int mode)
{
	char trace_log[256] = {0};

	if (psession != NULL)
	{
		if (mode == PCP_LINE_P2P_MD)
			sprintf(trace_log, " | <<-change line to p2p->> |\r\n");
		else
			sprintf(trace_log, " | <<-change line to rtpp->> |\r\n");
		trace_appand(psession->call_log, trace_log);
	}

	return PCP_OK;
}

int trace_call_log(IN pcp_session_t* psession, OUT char* pcalllog)
{
	if (psession != NULL)
		trace_buff(psession->call_log, pcalllog);
	
	return PCP_OK;
}



//add by wuzhaoyang20140428
int trace_m_invite_log(IN pcp_session_t* psession)
{
	char media_str[128] = {0};
	char trace_log[256] = {0};

	if (psession != NULL)
	{
		if (psession->role == PCP_OFFER)
		{
			trace_media_sdp(psession->lsdp, media_str);
			sprintf(trace_log, "CALL_LOG:CALLID:%s CALLER:%s CALLEE:[%s %s] ROLE:OFFER\r\n", 
				psession->callid, psession->fuid, psession->tuid, psession->tphone);
			trace_appand(psession->call_log, trace_log);

			sprintf(trace_log, "(S)---call req(%s)-->(R)\r\n", media_str);
			trace_appand(psession->call_log, trace_log);
		}
		else
		{
			trace_media_sdp(psession->rsdp, media_str);
			sprintf(trace_log, "CALL_LOG:CALLID:%s CALLER:%s CALLEE:[%s %s] ROLE:ANSWER\r\n", 
				psession->callid, psession->fuid, psession->tuid, psession->tphone);
			trace_appand(psession->call_log, trace_log);

			sprintf(trace_log, "(S)<--call req(%s)---(R)\r\n", media_str);
			trace_appand(psession->call_log, trace_log);
		}
	}

	return PCP_OK;
}



int trace_convert_conference_log(IN pcp_session_t* psession)//add by wuzhaoyang20140504
{
	char trace_log[256] = {0};

	if (psession != NULL)
	{
		sprintf(trace_log, " The dial has converted to a conference\n");
		trace_appand(psession->call_log, trace_log);
	}	

	return PCP_OK;
}

//add by wuzhaoyang20140605
// int trace_m_notify_log(IN pcp_session_t* psession)
// {
// 	char trace_log[512] = {0};
// 
// 	if (psession != NULL)
// 	{
// 		sprintf(trace_log, "The conference state update\n", 
// 			psession->conference_state->states.participant_num);		
// 		trace_appand(psession->call_log, trace_log);
// 	}	
// 	return PCP_OK;
// }


