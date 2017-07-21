#include "pcp_timer.h"

static pcp_timer_vtable_t* g_tmtable = NULL;

static pcp_timer_vtable_t* create_timer_vtable(cb_time_func tm_cb);

static void free_timer_vtalble(pcp_timer_vtable_t* pvtable);

static pcp_timer_t* create_timer(timer_mode_t type, const char* tname, long m_millisecond, cb_time_func t_cb);

static int delete_timer(pcp_timer_t* ptimer);


pcp_timer_vtable_t* create_timer_vtable(cb_time_func tm_cb)
{
	pcp_timer_vtable_t* ptable = NULL;

	ptable = pcp_malloc(sizeof(pcp_timer_vtable_t));

	if (ptable != NULL)
	{
		ptable->t0 = create_timer(TP_ONLYONCE, "call overtime", 
		                          TIMEOUT_T0_V, tm_cb);
		
		ptable->t1 = create_timer(TP_ONLYONCE, "answer overtime", 
		                          TIMEOUT_T1_V, tm_cb);
		
		ptable->t2 = create_timer(TP_ONLYONCE, "ack overtime",
		                          TIMEOUT_T2_V, tm_cb);
		
		ptable->t3 = create_timer(TP_ONLYONCE, "push overtime", 
		                          TIMEOUT_T3_V, tm_cb);
		                          
		ptable->t4 = create_timer(TP_ONLYONCE, "conf call rsp overtime", 
		                          TIMEOUT_T4_V, tm_cb);
		                          
		ptable->t5 = create_timer(TP_ONLYONCE, "in trans rsp timeout",
		                          TIMEOUT_T5_V, tm_cb);
	}

	return ptable;
}

void free_timer_vtalble(pcp_timer_vtable_t* pvtable)
{
	if (pvtable != NULL)
	{
		/*release timer*/
		delete_timer(pvtable->t0);
		delete_timer(pvtable->t1);
		delete_timer(pvtable->t2);
		delete_timer(pvtable->t3);
		delete_timer(pvtable->t4);
		delete_timer(pvtable->t5);
	
		pcp_free(pvtable);
		pvtable = NULL;
	}
}


int init_timer(IN cb_time_func tm_cb)
{
	g_tmtable = create_timer_vtable(tm_cb);
	if (g_tmtable == NULL)
		return PCP_ERR;

	return PCP_OK;
}

void destroy_timer()
{
	/*destroy timer vtable*/
	if (g_tmtable)
	{
		free_timer_vtalble(g_tmtable);
		g_tmtable = NULL;
	}
}

pcp_timer_t* create_timer(timer_mode_t type, const char* tname, 
                            long m_millisecond, cb_time_func t_cb)
{
	pcp_timer_t* ptimer = NULL;

	ptimer = pcp_malloc(sizeof(pcp_timer_t));
	if (ptimer != NULL)
	{
		ptimer->time_id = tm_create_timer(type, m_millisecond, t_cb);
		ptimer->timer_cb = t_cb;
		strcpy(ptimer->timer_name, tname);
	}
	return ptimer;
}

int delete_timer(pcp_timer_t* ptimer)
{
	if (ptimer != NULL)
	{
		tm_delete_timer(ptimer->time_id);
		pcp_free(ptimer);
		ptimer = NULL;
	}
	
	return PCP_OK;
}

int start_timer(IN timer_type_t time_type)
{
	int res = PCP_ERR;

	if (g_tmtable == NULL)
		return PCP_ERR;
	
	switch(time_type)
	{
		case TIMEOUT_T0:
			if (g_tmtable->t0)
				res = tm_start_timer(g_tmtable->t0->time_id);
			break;
		case TIMEOUT_T1:
			if (g_tmtable->t1)
				res = tm_start_timer(g_tmtable->t1->time_id);
			break;
		case TIMEOUT_T2:
			if (g_tmtable->t2)
				res = tm_start_timer(g_tmtable->t2->time_id);
			break;
		case TIMEOUT_T3:
			if (g_tmtable->t3)
				res = tm_start_timer(g_tmtable->t3->time_id);
			break;
		case TIMEOUT_T4:
			if (g_tmtable->t4)
				res = tm_start_timer(g_tmtable->t4->time_id);
			break;
		case TIMEOUT_T5:
		    if ( g_tmtable->t5 )
		    {
		        res = tm_start_timer(g_tmtable->t5->time_id);
		    }
		    break;
	}

	return res;
}

int stop_timer(IN timer_type_t time_type)
{
	int res = PCP_ERR;

	if (g_tmtable == NULL)
		return PCP_ERR;
	
	switch(time_type)
	{
		case TIMEOUT_T0:
			if (g_tmtable->t0)
				res = tm_stop_timer(g_tmtable->t0->time_id);
			break;
		case TIMEOUT_T1:
			if (g_tmtable->t1)
				res = tm_stop_timer(g_tmtable->t1->time_id);
			break;
		case TIMEOUT_T2:
			if (g_tmtable->t2)
				res = tm_stop_timer(g_tmtable->t2->time_id);
			break;
		case TIMEOUT_T3:
			if (g_tmtable->t3)
				res = tm_stop_timer(g_tmtable->t3->time_id);
			break;
		case TIMEOUT_T4:
			if (g_tmtable->t4)
				res = tm_stop_timer(g_tmtable->t4->time_id);
			break;
		case TIMEOUT_T5:
		    if ( g_tmtable->t5)
		    {
		        res = tm_stop_timer(g_tmtable->t5->time_id);
		    }
		break;
	}

	return res;
}

int stop_all_timers(void)
{
	if (g_tmtable == NULL)
		return PCP_ERR;

	if (g_tmtable->t0)
		tm_stop_timer(g_tmtable->t0->time_id);
		
	if (g_tmtable->t1)
		tm_stop_timer(g_tmtable->t1->time_id);
		
	if (g_tmtable->t2)
		tm_stop_timer(g_tmtable->t2->time_id);
		
	if (g_tmtable->t3)
		tm_stop_timer(g_tmtable->t3->time_id);
		
	if (g_tmtable->t4)
		tm_stop_timer(g_tmtable->t4->time_id);
		
    if ( g_tmtable->t5 )
    {
        tm_stop_timer(g_tmtable->t5->time_id);
    }
	return PCP_OK;
}

OUT int get_timer(IN int time_id)
{
	int t_type = -1;

	if (g_tmtable == NULL)
		return PCP_ERR;

	if (g_tmtable->t0 && g_tmtable->t0->time_id == time_id)
		t_type = TIMEOUT_T0;
	else if (g_tmtable->t1 && g_tmtable->t1->time_id == time_id)
		t_type = TIMEOUT_T1;
	else if (g_tmtable->t2 && g_tmtable->t2->time_id == time_id)
		t_type = TIMEOUT_T2;
	else if (g_tmtable->t3 && g_tmtable->t3->time_id == time_id)
		t_type = TIMEOUT_T3;
	else if (g_tmtable->t4 && g_tmtable->t4->time_id == time_id)
		t_type = TIMEOUT_T4;
	else if ( g_tmtable->t5 && g_tmtable->t5->time_id == time_id)
	    t_type = TIMEOUT_T5;

	return t_type;
}



