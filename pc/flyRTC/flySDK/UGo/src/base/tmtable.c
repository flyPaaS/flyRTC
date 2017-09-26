#include "tmtable.h"
#include "mscommon.h"


static tm_vtable_t* g_tmvtable; /*timer table*/

static tm_vtable_t* create_tm_vtable(cb_time_func tm_cb);
static void free_tm_vtalble(tm_vtable_t* pvtable);
static tm_t* create_tm(tm_mode_t type, const char* tname, long m_millisecond, cb_time_func t_cb);
static int delete_tm(tm_t* ptimer);

tm_vtable_t* create_tm_vtable(cb_time_func tm_cb)
{
	tm_vtable_t* ptable = NULL;

	ptable = (tm_vtable_t*)ms_malloc0(sizeof(tm_vtable_t));

	if (ptable != NULL)
	{
		ptable->t0 = create_tm(TM_ONLYONCE_TP, 
		                        "ice detection overtime", 
		                        TM_P2P_DETECTION_T0_V, 
		                        tm_cb);
		ptable->t1 = create_tm(TM_PERIOD_TP, 
		                        "ice check line state overtime", 
		                        TM_P2P_CHECKLINE_T1_V, 
		                        tm_cb);
		ptable->t2 = create_tm(TM_PERIOD_TP, 
		                        "ice send ping msg overtime", 
		                        TM_P2P_SENDPING_T2_V, 
		                        tm_cb);
		ptable->t3 = create_tm(TM_PERIOD_TP, 
		                        "uc send in trans req period", 
		                        TM_SEND_IN_TRANS_REQ_T3_V, 
		                        tm_cb);
	}

	return ptable;
}

void free_tm_vtalble(tm_vtable_t* pvtable)
{
	if (pvtable != NULL)
	{
		/*release timer*/
		delete_tm(pvtable->t0);
		delete_tm(pvtable->t1);
		delete_tm(pvtable->t2);
		delete_tm(pvtable->t3);
	
		ms_free(pvtable);
		pvtable = NULL;
	}
}

tm_t* create_tm(tm_mode_t type, const char* tname, long m_millisecond, cb_time_func t_cb)
{
	tm_t* ptimer = NULL;

	ptimer = (tm_t*)ms_malloc0(sizeof(tm_t));
	if (ptimer != NULL)
	{
		ptimer->tm_id = tm_create_timer(type, m_millisecond, t_cb);
		ptimer->tm_cb = t_cb;
		strcpy(ptimer->tm_name, tname);
	}
	return ptimer;
}

int delete_tm(tm_t* ptimer)
{
	if (ptimer != NULL)
	{
		tm_delete_timer(ptimer->tm_id);
		ms_free(ptimer);
		ptimer = NULL;
	}
	
	return TM_OK;
}

int tmt_init(IN tm_cb_t tm_cb)
{
	/*init tm model*/
	tm_init();

	g_tmvtable = create_tm_vtable(tm_cb);
	if (g_tmvtable == NULL)
		return TM_ERR;

	return TM_OK;
}

void tmt_destroy(void)
{
	if (g_tmvtable)
	{
		free_tm_vtalble(g_tmvtable);
		g_tmvtable = NULL;
	}

	/*destroy time*/
	tm_destroy();
}

int tmt_start_timer(IN tm_type_t time_type)
{
	int res = TM_ERR;

	if (g_tmvtable == NULL)
		return TM_ERR;
	
	switch(time_type)
	{
		case TM_P2P_DETECTION_T0:
			if (g_tmvtable->t0)
				res = tm_start_timer(g_tmvtable->t0->tm_id);
			break;
		case TM_P2P_CHECKLINE_T1:
			if (g_tmvtable->t1)
				res = tm_start_timer(g_tmvtable->t1->tm_id);
			break;
		case TM_P2P_SENDPING_T2:
			if (g_tmvtable->t2)
				res = tm_start_timer(g_tmvtable->t2->tm_id);
			break;
			
	    case TM_SEND_IN_TRANS_REQ_T3:
	    {
	        if ( g_tmvtable->t3 )
	        {
	            res = tm_start_timer(g_tmvtable->t3->tm_id);
	        }
	    }
	    break;
	}

	return res;
}

int tmt_stop_timer(IN tm_type_t time_type)
{
	int res = TM_ERR;

	if (g_tmvtable == NULL)
		return TM_ERR;
	
	switch(time_type)
	{
		case TM_P2P_DETECTION_T0:
			if (g_tmvtable->t0)
				res = tm_stop_timer(g_tmvtable->t0->tm_id);
			break;
		case TM_P2P_CHECKLINE_T1:
			if (g_tmvtable->t1)
				res = tm_stop_timer(g_tmvtable->t1->tm_id);
			break;
		case TM_P2P_SENDPING_T2:
			if (g_tmvtable->t2)
				res = tm_stop_timer(g_tmvtable->t2->tm_id);
			break;

		case TM_SEND_IN_TRANS_REQ_T3:
		{
		    if ( g_tmvtable->t3 )
		    {
		        res = tm_stop_timer(g_tmvtable->t3->tm_id);
		    }
		}
		break;

		default:
		break;
	}

	return res;
}

int tmt_stop_all_timers(void)
{
	if (g_tmvtable == NULL)
		return TM_ERR;

	if (g_tmvtable->t0)
		tm_stop_timer(g_tmvtable->t0->tm_id);
	if (g_tmvtable->t1)
		tm_stop_timer(g_tmvtable->t1->tm_id);
	if (g_tmvtable->t2)
		tm_stop_timer(g_tmvtable->t2->tm_id);

	if (g_tmvtable->t3)
		tm_stop_timer(g_tmvtable->t3->tm_id);

	return TM_OK;
}

OUT int tmt_get_timer_type(IN int time_id)
{
	int t_type = -1;

	if (g_tmvtable == NULL)
		return TM_ERR;

	if (g_tmvtable->t0 && g_tmvtable->t0->tm_id == time_id)
	{
		t_type = TM_P2P_DETECTION_T0;
	}
	else if (g_tmvtable->t1 && g_tmvtable->t1->tm_id == time_id)
	{
		t_type = TM_P2P_CHECKLINE_T1;
	}
	else if (g_tmvtable->t2 && g_tmvtable->t2->tm_id == time_id)
	{
		t_type = TM_P2P_SENDPING_T2;
	}
	else if (g_tmvtable->t3 && g_tmvtable->t3->tm_id == time_id)
	{
		t_type = TM_SEND_IN_TRANS_REQ_T3;
	}

	return t_type;
}


int tmt_get_timer_name(IN int time_id, OUT char* time_name)
{
	if (g_tmvtable == NULL)
		return TM_ERR;

	if (g_tmvtable->t0 && g_tmvtable->t0->tm_id == time_id)
	{
		strcpy(time_name, g_tmvtable->t0->tm_name);
	}
	else if (g_tmvtable->t1 && g_tmvtable->t1->tm_id == time_id)
	{
		strcpy(time_name, g_tmvtable->t1->tm_name);
	}
	else if (g_tmvtable->t2 && g_tmvtable->t2->tm_id == time_id)
	{
		strcpy(time_name, g_tmvtable->t2->tm_name);
	}
	else if (g_tmvtable->t3 && g_tmvtable->t3->tm_id == time_id)
	{
		strcpy(time_name, g_tmvtable->t3->tm_name);
	}
	else
	{
		strcpy(time_name, "unknow timer");
	}
	
	return TM_OK;
}

