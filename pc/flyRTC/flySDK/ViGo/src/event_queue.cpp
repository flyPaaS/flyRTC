#include "event_queue.h"
#include "MediaEngineInterface.h"

static evt_cb_f g_evtcb = NULL;
static ms_mutex_t	g_evlock;
static MSList* 	g_evlist = NULL;



/* BEGIN: Added by gonghuojin, 2014/4/2   PN:add event Iterate thread*/
static volatile bool g_ThreadExit = false;
static ms_thread_t 	g_ThreadId = NULL;

static void evt_thread_start(void);
static void evt_thread_stop(void);
static void *evt_iteration_thread(void *arg);

void evt_thread_start(void)
{
	g_ThreadExit = false;
	ms_thread_create(&g_ThreadId, NULL, evt_iteration_thread, NULL);
}

void evt_thread_stop(void)
{
	int waiting_time = 2000;
	int *ptime = &waiting_time;
	g_ThreadExit = true;
	ms_thread_join(g_ThreadId, (void**)&ptime);
	g_ThreadId = NULL;
}

void *evt_iteration_thread(void *arg)
{
	ME_event_node_t* evdata = NULL;
	
	while (!g_ThreadExit)
	{ 
    	ms_mutex_lock(&g_evlock);
		if (g_evlist)
		{
			evdata = (ME_event_node_t*)g_evlist->data;
			if (evdata != NULL)
			{
				if (g_evtcb)
					g_evtcb(evdata->ev_type, evdata->ev_reason, \
					        evdata->something, evdata->param, evdata->size);
						
				evt_remove((void*)evdata);
			}
		}
	    ms_mutex_unlock(&g_evlock);	
		ms_usleep(10*1000);
	}

	return NULL;
}


/* END:   Added by gonghuojin, 2014/4/2 */

int evt_init()
{
	ms_init();
	
	ms_mutex_init(&g_evlock,NULL);
	g_evlist = NULL;

	evt_thread_start();
	
	return 0;
}

void evt_destroy()
{
	evt_thread_stop();

	ms_mutex_lock(&g_evlock);
	ms_list_free_data(g_evlist);
	g_evlist = ms_list_free(g_evlist);
	ms_mutex_unlock(&g_evlock);
	ms_mutex_destroy(&g_evlock);

	g_evtcb = NULL;
}

void evt_appand(int ev_type, int ev_reason, const char* something, void* param, int size)
{
#ifdef ME_USE_EVENT_LIST
	ME_event_node_t* evdata = NULL;
    
	evdata = (ME_event_node_t*)ms_malloc0(ME_EVENT_NODE_T_SIZE + size);
	evdata->size = size;
	if (evdata)
	{
		evdata->ev_type = ev_type;
		evdata->ev_reason = ev_reason;
		if ( something != NULL )
        {
            strncpy(evdata->something, (char*)something, sizeof(evdata->something) -1 );
        }

        if ( param != NULL )
        {
            memcpy(&evdata->param, param, size);
        }

		ms_mutex_lock(&g_evlock);
		g_evlist = ms_list_append(g_evlist, evdata);
		ms_mutex_unlock(&g_evlock);
	}
#else
    if ( g_evtcb )
    {
		g_evtcb(ev_type, ev_reason, something, param, size);
    }
#endif
	return ;
}

void evt_remove(void* ev_data)
{
	if (NULL == ev_data || g_evlist == NULL)
		return ;

    g_evlist = ms_list_remove(g_evlist, ev_data);
    ms_free(ev_data);
	
	return ;
}

void evt_callback(evt_cb_f pevfunc)
{
	g_evtcb = pevfunc;
}



