#include "mq.h"
#include "ice.h"
#include "../../comm/inc/cfg.h"

typedef struct uc_mslists_t
{
	mslist_handle_t		tcp_list_hd; /*tcp event list*/
	mslist_handle_t		ice_list_hd; /*ice event list*/
	mslist_handle_t		pcp_list_hd; /*pcp event list*/
	mslist_handle_t		meg_list_hd; /*media engine event list*/
	mslist_handle_t		log_list_hd; /*log trace event list*/
	mslist_handle_t		tmt_list_hd; /*tmt event list*/
}uc_mslists_t;
#define UGO_MSLISTS_SIZE   (sizeof(uc_mslists_t))

static uc_mslists_t*  g_mslists = NULL;


void free_ice_mslist_param(void)
{
	if (g_mslists != NULL)
	{
		MSList* evlist = NULL;
		evlist = g_mslists->ice_list_hd.ev_list;
		while(evlist)
		{ 
			ice_event_node_t* evdata = NULL;
			evdata = (ice_event_node_t*)evlist->data;
			if (evdata && evdata->ev_param)
			{
				ms_free(evdata->ev_param);
				evdata->ev_param = NULL;
			}

			evlist = evlist->next;
		}
	}
}


int mq_init(void)
{
	/*init mslist*/
	mslist_init();

	if (g_mslists == NULL)
	{
		g_mslists = (uc_mslists_t*)ms_malloc(UGO_MSLISTS_SIZE);
		if (g_mslists == NULL)
			return RES_ERR;

		mslist_create(&g_mslists->tcp_list_hd);
		mslist_create(&g_mslists->ice_list_hd);
		mslist_create(&g_mslists->pcp_list_hd);
		mslist_create(&g_mslists->meg_list_hd);
		mslist_create(&g_mslists->log_list_hd);
		mslist_create(&g_mslists->tmt_list_hd);
	}

	return RES_OK;
}

int mq_destroy(void)
{
	if (g_mslists != NULL)
	{
#ifdef  ICE_ENABLED
		/*free ice evetn list */
		free_ice_mslist_param();
#endif
		mslist_delete(&g_mslists->ice_list_hd);
		/*free tcp event list*/
		mslist_delete(&g_mslists->tcp_list_hd);
		/*free pcp event list*/
		mslist_delete(&g_mslists->pcp_list_hd);
		/*free meg event list*/
		mslist_delete(&g_mslists->meg_list_hd);
		/*free log event list*/
		mslist_delete(&g_mslists->log_list_hd);
		/*free tmt event list*/
		mslist_delete(&g_mslists->tmt_list_hd);
		/*free list manager*/
		ms_free(g_mslists);
		g_mslists = NULL;
	}

	mslist_destroy();

	return RES_OK;
}


OUT mslist_handle_t* mq_get_mslist(IN mq_type_t mstype)
{
	mslist_handle_t* mshd = NULL;

	if (g_mslists == NULL)
		return NULL;

	switch(mstype)
	{
		case MQ_ICE_TP:
			mshd = &g_mslists->ice_list_hd;
			break;
		case MQ_TCP_TP:
			mshd = &g_mslists->tcp_list_hd;
			break;
		case MQ_MEG_TP:
			mshd = &g_mslists->meg_list_hd;
			break;
		case MQ_PCP_TP:
			mshd = &g_mslists->pcp_list_hd;
			break;
		case MQ_TMT_TP:
			mshd = &g_mslists->tmt_list_hd;
			break;
		case MQ_LOG_TP:
			mshd = &g_mslists->log_list_hd;
			break;
		default:
			break;
	}

	return mshd;
}

OUT void* mq_get_node(IN mq_type_t mstype)
{
	void* data_node = NULL;
	MSList* evlist = NULL;
	
	if (g_mslists == NULL)
		return NULL;

	switch(mstype)
	{
		case MQ_ICE_TP:
			evlist = g_mslists->ice_list_hd.ev_list;
			break;
		case MQ_TCP_TP:
			evlist = g_mslists->tcp_list_hd.ev_list;
			break;
		case MQ_MEG_TP:
			evlist = g_mslists->meg_list_hd.ev_list;
			break;
		case MQ_PCP_TP:
			evlist = g_mslists->pcp_list_hd.ev_list;
			break;
		case MQ_TMT_TP:
			evlist = g_mslists->tmt_list_hd.ev_list;
			break;
		case MQ_LOG_TP:
			evlist = g_mslists->log_list_hd.ev_list;
			break;
		default:
			break;
	}

	if (evlist)
		data_node = evlist->data;

	return data_node;
}

int mq_remove_node(IN mq_type_t mstype, IN void* node)
{
	mslist_handle_t* mshd = NULL;

	if (g_mslists == NULL)
		return RES_ERR;

	switch(mstype)
	{
		case MQ_ICE_TP:
			mshd = &g_mslists->ice_list_hd;
			break;
		case MQ_TCP_TP:
			mshd = &g_mslists->tcp_list_hd;
			break;
		case MQ_MEG_TP:
			mshd = &g_mslists->meg_list_hd;
			break;
		case MQ_PCP_TP:
			mshd = &g_mslists->pcp_list_hd;
			break;
		case MQ_TMT_TP:
			mshd = &g_mslists->tmt_list_hd;
			break;
		case MQ_LOG_TP:
			mshd = &g_mslists->log_list_hd;
			break;
		default:
			break;
	}

	if (mshd)
		mslist_remove(mshd, node);
	
	return RES_OK;
}


