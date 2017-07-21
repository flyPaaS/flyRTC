#include "pm.h"
#include "comm.h"
#include "p2p.h"

static uc_cfg_t* g_uccfg = NULL;

#define UGO_CFG_CHECK(cfg) if (cfg == NULL) pm_create();


int pm_create(void)
{
	if (g_uccfg == NULL)
	{
		/**create uc config object*/
		g_uccfg = (uc_cfg_t*)ms_malloc0(UC_CFG_SIZE);
		if (g_uccfg == NULL)
			return RES_ERR;

		/**set ugo config default value****/
		g_uccfg->ugo_cfg.rc4_enabled = FALSE;
		g_uccfg->ugo_cfg.video_enabled = FALSE;
        g_uccfg->ugo_cfg.tlv_enabled = FALSE;
        g_uccfg->ugo_cfg.ipv6_enabled = FALSE;
        g_uccfg->ugo_cfg.compress_enabled = FALSE;
        g_uccfg->ugo_cfg.csrv_enabled = FALSE;

		/**set tcp config default value****/
		g_uccfg->tcp_cfg.tcp_enabled = FALSE;

#ifdef ICE_ENABLED
		/**set ice config default value****/
		g_uccfg->ice_cfg.ice_enabled = FALSE;
        g_uccfg->ice_cfg.ipv6_enabled = FALSE;
#endif

		/**set rtpplist config default value***/
		g_uccfg->rtpp_cfg.rtpp_num = 0;
		
	}
	
	return RES_OK;
}

int pm_remove(void)
{
	if (g_uccfg != NULL)
	{
		ms_free(g_uccfg);
		g_uccfg = NULL;
	}
	
	return RES_OK;
}

int pm_is_local_rc4_enabled(void)
{
	if (g_uccfg == NULL)
		return FALSE;

	return g_uccfg->ugo_cfg.rc4_enabled;
}

int pm_is_local_ice_enabled(void)
{
	if (g_uccfg == NULL)
		return FALSE;

	return g_uccfg->ice_cfg.ice_enabled;
}

int pm_is_local_video_enabled(void)
{
	if (g_uccfg == NULL)
		return FALSE;

	return g_uccfg->ugo_cfg.video_enabled;
}

int pm_is_tcp_enabled(void)
{
	if (g_uccfg == NULL)
		return FALSE;

	return g_uccfg->tcp_cfg.tcp_enabled;
}

int pm_is_tlv_enabled()
{
    if ( g_uccfg == NULL )
    {
        return FALSE;
    }

    return g_uccfg->ugo_cfg.tlv_enabled;
}

int pm_is_compress_enabled()
{
    if ( g_uccfg == NULL )
    {
        return FALSE;
    }

    return g_uccfg->ugo_cfg.compress_enabled;
}

int pm_is_ipv6_enabled()
{
    if ( g_uccfg == NULL )
    {
        return FALSE;
    }

    return g_uccfg->ugo_cfg.ipv6_enabled;
}

/* add by Vinton.Liu on 2016-06-02 for custom service version begin */
OUT int pm_is_csrv_enabled()
{
    if ( g_uccfg == NULL )
    {
        return FALSE;
    }

    return g_uccfg->ugo_cfg.csrv_enabled;
}
/* Vinton.Liu add end */

/* add by VintonLiu on 20150709 for IM3.0 */
int pm_get_account_type(void)
{
    UGO_CFG_CHECK(g_uccfg);

    return g_uccfg->ugo_cfg.atype;
}
/* VintonLiu add end */

int pm_set_uc_cfg(IN const ugo_cfg_t* pcfg)
{
	if (NULL == pcfg)
		return RES_ERR;

	UGO_CFG_CHECK(g_uccfg);

	memcpy(&g_uccfg->ugo_cfg, pcfg, UGO_CFG_T_SIZE);

	return RES_OK;
}

int pm_set_tcp_cfg(IN const ugo_cfg_tcp_t* pcfg)
{
	if(NULL == pcfg)
		return RES_ERR;
	
	UGO_CFG_CHECK(g_uccfg);
	
	memcpy(&g_uccfg->tcp_cfg, pcfg, sizeof(ugo_cfg_tcp_t));
	
	return RES_OK;
}

int pm_set_ice_cfg(IN const ugo_cfg_ice_t* pcfg)
{
	if (NULL == pcfg)
		return RES_ERR;
	
	UGO_CFG_CHECK(g_uccfg);

	memcpy(&g_uccfg->ice_cfg, pcfg, sizeof(ugo_cfg_ice_t));
    if ( pcfg->stun_server[0] == '\0' )
    {
        g_uccfg->ice_cfg.ice_enabled = FALSE;
    }
	return RES_OK;
}

int pm_set_rtpp_cfg(IN const ugo_rtpp_cfg_t* pcfg)
{
	if (NULL == pcfg)
		return RES_ERR;
	
	UGO_CFG_CHECK(g_uccfg);

	memcpy(&g_uccfg->rtpp_cfg, pcfg, UGO_RTPP_CFG_T_SIZE);

	if (g_uccfg->rtpp_cfg.rtpp_num > UGO_RTPP_MAX_NUM)
		g_uccfg->rtpp_cfg.rtpp_num = UGO_RTPP_MAX_NUM;


	return RES_OK;
}

int pm_set_cb_vtable(IN const uc_cb_vtable_t* pvtable)
{
	if (NULL == pvtable)
		return RES_ERR;
    if (g_uccfg != NULL)
    {
      pm_remove();
    }
	UGO_CFG_CHECK(g_uccfg);

	memcpy(&g_uccfg->uc_table, pvtable, sizeof(uc_cb_vtable_t));

	return RES_OK;
}

int pm_get_uc_cfg(OUT ugo_cfg_t* pcfg)
{
	if (pcfg == NULL)
		return RES_ERR;

	UGO_CFG_CHECK(g_uccfg);

	memcpy(pcfg, &g_uccfg->ugo_cfg, sizeof(ugo_cfg_t));
	
	return RES_OK;
}

int pm_get_tcp_cfg(OUT ugo_cfg_tcp_t* pcfg)
{
	if(pcfg == NULL)
		return RES_ERR;

	UGO_CFG_CHECK(g_uccfg);

	memcpy(pcfg, &g_uccfg->tcp_cfg, sizeof(ugo_cfg_tcp_t));

	return RES_OK;
}

int pm_get_ice_cfg(OUT ugo_cfg_ice_t* pcfg)
{
	if(pcfg == NULL)
		return RES_ERR;

	UGO_CFG_CHECK(g_uccfg);

	memcpy(pcfg, &g_uccfg->ice_cfg, sizeof(ugo_cfg_ice_t));

	return RES_OK;
}

int pm_get_rtpp_cfg(OUT ugo_rtpp_cfg_t* pcfg)
{
	if(pcfg == NULL)
		return RES_ERR;

	UGO_CFG_CHECK(g_uccfg);

	memcpy(pcfg, &g_uccfg->rtpp_cfg, sizeof(ugo_rtpp_cfg_t));

	return RES_OK;
}

OUT int pm_get_platform(void)
{
	UGO_CFG_CHECK(g_uccfg);
	
	return g_uccfg->ugo_cfg.platform;
}

int pm_get_brand(OUT char* brand)
{
	UGO_CFG_CHECK(g_uccfg);

	strcpy(brand, g_uccfg->ugo_cfg.brand);

	return RES_OK;
}

int pm_get_uid(OUT char* uid)
{
    UGO_CFG_CHECK(g_uccfg);

	strcpy(uid, g_uccfg->ugo_cfg.uid);

	return RES_OK;
}

int pm_get_userid(OUT char* userid)
{
	UGO_CFG_CHECK(g_uccfg);

	strcpy(userid, g_uccfg->ugo_cfg.userid);

	return RES_OK;
}

int pm_get_userphone(OUT char* phone)
{
	UGO_CFG_CHECK(g_uccfg);

	strcpy(phone, g_uccfg->ugo_cfg.phone);

	return RES_OK;
}

int pm_get_nickname(OUT char* nickname)
{
    UGO_CFG_CHECK(g_uccfg);
    strcpy(nickname, g_uccfg->ugo_cfg.nickname);

    return RES_OK;
}

int pm_get_localipaddr(OUT char* ipaddr)
{
    UGO_CFG_CHECK(g_uccfg);
    strcpy(ipaddr, g_uccfg->ugo_cfg.localipaddr);

    return RES_OK;
}

int pm_get_tcp_serveraddr(OUT char* srvaddr)
{
	UGO_CFG_CHECK(g_uccfg);

	strcpy(srvaddr, g_uccfg->tcp_cfg.tcp_srvaddr);

	return RES_OK;
}

int pm_get_ice_stunaddr(OUT char* stunaddr)
{
	UGO_CFG_CHECK(g_uccfg);

	strcpy(stunaddr, g_uccfg->ice_cfg.stun_server);

	return RES_OK;
}

int pm_get_ice_ipv6_enabled()
{
    UGO_CFG_CHECK(g_uccfg);
    return g_uccfg->ice_cfg.ipv6_enabled;
}

void pm_uc_event_cb(IN int type, IN int reason, IN const char* describe, IN void* param)
{
	if (g_uccfg != NULL && g_uccfg->uc_table.event_cb)
	{
		g_uccfg->uc_table.event_cb(type, reason, describe, param);
	}
}

void pm_uc_send_msg_cb(IN const char* msg, IN int slen)
{
	if (g_uccfg != NULL && g_uccfg->uc_table.send_cb)
	{
		g_uccfg->uc_table.send_cb(msg, slen);
	}
}

void pm_uc_call_state_cb(IN int state)
{
	if (g_uccfg != NULL && g_uccfg->uc_table.state_cb)
	{
		g_uccfg->uc_table.state_cb(state);
	}
}

void pm_uc_trace_log_cb(IN int level, IN const char* describe)
{
	if (g_uccfg != NULL && g_uccfg->uc_table.log_cb)
	{
		g_uccfg->uc_table.log_cb(level, describe);
	}
}




