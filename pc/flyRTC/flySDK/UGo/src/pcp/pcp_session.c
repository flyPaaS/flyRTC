#include "comm.h"
#include "pcp_session.h"
#include "mslist.h"
#include "pcp_sdp.h"
#include "pcp_trace.h"

static mslist_handle_t g_session_list;/*session list*/
static ms_mutex_t  g_slock;/*session lock*/
static int check_session_func(const void* plist, const void* user_data);
static void free_data_func(void* data); 

void free_data_func(void* data)
{
	pcp_session_t* psession = (pcp_session_t*)data;

	if (psession != NULL)
	{
		free_sdp(psession->lsdp);
		free_sdp(psession->rsdp);
		psession->lsdp = NULL;
		psession->rsdp = NULL;
	}
}

int check_session_func(const void* plist, const void* user_data)
{
	pcp_session_t* psession = (pcp_session_t*)plist;
	char* callid = (char*)user_data;
	int res = 0;

	if (psession == NULL)
		return PCP_ERR;

	/*if not callid info, default get first one session*/
	if (callid == NULL || callid[0] == '\0')
		return PCP_OK;

	res = strcmp(psession->callid, callid);

	return res;
}


int init_session(void)
{
	/*init tace log*/
	trace_init();
	mslist_create(&g_session_list);

	/*init session mutex lock*/
	ms_mutex_init(&g_slock, NULL);
	
	return PCP_OK;
}

void destroy_session(void)
{
	mslist_for_each(&g_session_list, free_data_func);
	mslist_delete(&g_session_list);

	ms_mutex_destroy(&g_slock);

	/*destroy trace log*/
	trace_destroy();
}

OUT pcp_session_t* create_session(IN const char* callid)
{
	pcp_session_t* psession = pcp_malloc(sizeof(pcp_session_t));

	if (psession)
	{
		strcpy(psession->callid, callid);
		/*create call log proxy*/
		psession->call_log = create_trace_proxy();
		/*append to session list*/
		mslist_append(&g_session_list,(void*)psession);
	}

	return psession;
}

void delete_session(IN pcp_session_t* psession)
{
	if (psession != NULL)
	{
		/*free sdp*/
		free_sdp(psession->lsdp);
		free_sdp(psession->rsdp);
		if (psession->conf_state != NULL)
		{
			free_conference(psession->conf_state);//add by wuzhaoyang20140605
			psession->conf_state = NULL;
		}

		psession->lsdp = NULL;
		psession->rsdp = NULL;

		/*free call log proxy*/
		free_trace_proxy(psession->call_log);
		psession->call_log = NULL;
		
		mslist_remove(&g_session_list, psession);
	}
}

OUT pcp_session_t* get_session(IN const char* callid)
{
	MSList* ls = NULL;
	
	ls = mslist_find(&g_session_list, check_session_func, (void*)callid);

	if (ls == NULL)
		return NULL;

	return ls->data;
}

int create_callid(IN const char* uid, OUT char* callid)
{
	char alp[6] = {0};
	static unsigned nsceq = 0;
	
	if (callid == NULL)
		return PCP_ERR;

	if (nsceq++ > 999)
		nsceq = 0;

	cm_rand_alphabet5(alp);
	
	if (uid != NULL && strlen(uid) != 0)
	{
		sprintf(callid, "%s%s%d%d", uid, alp, cm_rand_time(), nsceq);
	}
	else
	{
		sprintf(callid, "null%s%d%d", alp, cm_rand_time(), nsceq);
	}
	
	return PCP_OK;
}


int update_session_state(IN const char* callid, IN pcp_state_t state)
{
	pcp_session_t* psession = NULL;

	lock_session();
	psession = get_session(callid);
	if (psession != NULL)
	{
		psession->call_state = state;
		/*callback state changed*/
		call_state_cb(psession->call_state);
	}
	ulock_session();
		
	return PCP_OK;
}

int session_state(IN const char* callid)
{	
	int state = PCP_STATE_WAIT;
	pcp_session_t* psession = NULL;

	lock_session();
	psession = get_session(callid);
	if (psession != NULL)
		state = psession->call_state;
	ulock_session();

	return state;
}

int get_best_mediastream(IN int role, IN int type, OUT pcp_session_t* psession)
{
	int i = 0;
	int rs_i = 0;
	pcp_media_option_t l_option;
	pcp_media_option_t r_option;
	pcp_media_info_t* lmedia = NULL;
	pcp_media_info_t* rmedia = NULL; 

	if (psession == NULL)
		return PCP_ERR;

	if (psession->lsdp == NULL || psession->rsdp == NULL)
		return PCP_ERR;
	
	lmedia = &psession->lsdp->media;
	rmedia = &psession->rsdp->media;

	res_st(&l_option, sizeof(l_option));
	res_st(&r_option, sizeof(r_option));
	if (role == PCP_OFFER)
	{
		best_media_option(type, lmedia, &l_option);
		if (l_option.flag)
		{
			rs_i = check_payload(rmedia, l_option.pt);
			if (rs_i != PCP_ERR)
				memcpy(&r_option, &rmedia->media_arr[rs_i], sizeof(r_option));
		}
	}
	else
	{
		best_media_option(type, rmedia, &r_option);
		if (r_option.flag)
		{
			rs_i = check_payload(lmedia, r_option.pt);
			if (rs_i != PCP_ERR)
				memcpy(&l_option, &lmedia->media_arr[rs_i], sizeof(l_option));
		}
	}

	if (!r_option.flag || !l_option.flag)
		return PCP_ERR;

	if (type == AUDIO)
	{
		psession->audio_stream.session_pt = l_option.pt;
		
		psession->audio_stream.local_port = l_option.port;
		strcpy(psession->audio_stream.local_addr, psession->lsdp->c_ip);

		psession->audio_stream.remote_port = r_option.port;
		strcpy(psession->audio_stream.remote_addr, psession->rsdp->c_ip);

		psession->relay.a_port= r_option.port;
		strcpy(psession->relay.m_ip, psession->rsdp->c_ip);
	}
	else
	{
		psession->video_stream.session_pt = l_option.pt;
		
		psession->video_stream.local_port = l_option.port;
		strcpy(psession->video_stream.local_addr, psession->lsdp->c_ip);

		psession->video_stream.remote_port = r_option.port;
		strcpy(psession->video_stream.remote_addr, psession->rsdp->c_ip);

		psession->relay.v_port= r_option.port;
		strcpy(psession->relay.m_ip, psession->rsdp->c_ip);
	}
	
	return PCP_OK;
}


void lock_session_d(const char *pfunc)
{
    //ms_error("[%s] get lock.", pfunc);
    
	ms_mutex_lock(&g_slock);
}

void ulock_session_d(const char *pfunc)
{
    //ms_error("[%s] release lock.", pfunc);
    
	ms_mutex_unlock(&g_slock);
}

int check_session(IN pcp_session_t* psession)
{
	if (psession == NULL)
		return PCP_ERR;

	if (psession->expired_flag)
		return PCP_ERR;

	return PCP_OK;
}



