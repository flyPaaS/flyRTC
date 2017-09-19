#include "pcp_cfg.h"

static pcp_cfg_t* g_cfg = NULL;
#define PCP_CFG_CHECK(cfg) if (cfg == NULL) create_cfg();

static pcp_state_t g_sysstate; /*system call state*/
static int g_randcode = 0;

int create_cfg(void)
{
	if (g_cfg == NULL)
	{
		g_cfg = pcp_malloc(sizeof(pcp_cfg_t));

		if (g_cfg == NULL)
			return PCP_ERR;
	}

	return PCP_OK;
}

int remove_cfg(void)
{
	if (g_cfg != NULL)
	{
		pcp_free(g_cfg);
		g_cfg = NULL;
	}

	return PCP_OK;
}

OUT int is_local_rc4_enabled(void)
{
	PCP_CFG_CHECK(g_cfg);

	return g_cfg->proxy.rc4_enabled;
}

OUT int is_local_compress_enabled(void)
{
    PCP_CFG_CHECK(g_cfg);
    return g_cfg->proxy.compress_enabled;
}

OUT int is_local_ice_enabled(void)
{
	PCP_CFG_CHECK(g_cfg);
	return g_cfg->proxy.ice_enabled;
}

OUT int is_local_video_enabled(void)
{
	PCP_CFG_CHECK(g_cfg);
	return g_cfg->proxy.video_enabled;
}

OUT int is_local_audio_fec_enabled(void)
{
	PCP_CFG_CHECK(g_cfg);
	return (g_cfg->proxy.update_flag & PCP_FEC_OPEN) ? 1 : 0;	
}

OUT int is_local_direct_fec_enabled(void)
{
	PCP_CFG_CHECK(g_cfg);
	return (g_cfg->proxy.update_flag & PCP_DIRECTFEC_OPEN) ? 1 : 0;	
}
//add by wuzhaoyang20151228
OUT int is_local_direct_mulriple_fec_enabled(void)
{
	PCP_CFG_CHECK(g_cfg);
	return (g_cfg->proxy.update_flag & PCP_DIRECTFEC_MULRIPLE_OPEN) ? 1 : 0;
}

/* add by Vinton.Liu on 2016-05-30 for video fec and nack config begin */
OUT int is_local_video_fec_enabled(void)
{
    PCP_CFG_CHECK(g_cfg);
    return (g_cfg->proxy.update_flag & PCP_VIE_FEC_OPEN) ? 1 : 0;
}

OUT int is_local_video_nack_enabled(void)
{
    PCP_CFG_CHECK(g_cfg);
    return (g_cfg->proxy.update_flag & PCP_VIE_NACK_OPEN) ? 1 : 0;
}
/* Vinton.Liu add end */

int pcp_is_tlv_enabled(void)
{
    PCP_CFG_CHECK(g_cfg);
    return g_cfg->proxy.tlv_enabled;
}

int pcp_is_ipv6_enabled(void)
{
    PCP_CFG_CHECK(g_cfg);
    return g_cfg->proxy.ipv6_enabled;
}

int pcp_is_csrv_enabled(void)
{
    PCP_CFG_CHECK(g_cfg);
    return g_cfg->proxy.csrv_enabled;
}

int pcp_get_localipaddr(OUT char* ipaddr)
{
    PCP_CFG_CHECK(g_cfg);
    strcpy(ipaddr, g_cfg->proxy.localipaddr);

    return PCP_OK;
}

int set_proxy_cfg(IN const pcp_proxy_t* pproxy)
{
	int updateflg = 0;
	if (pproxy == NULL)
		return PCP_ERR;

	PCP_CFG_CHECK(g_cfg);
	updateflg = g_cfg->proxy.update_flag;
	updateflg |= pproxy->update_flag;
	memcpy(&g_cfg->proxy, pproxy, sizeof(pcp_proxy_t));
	g_cfg->proxy.update_flag = updateflg;

	return PCP_OK;
}

int set_rtpp_cfg(IN const pcp_rtpp_info_t* pcfg)
{
	if (pcfg == NULL)
		return PCP_ERR;

	PCP_CFG_CHECK(g_cfg);

	memcpy(&g_cfg->rtpp, pcfg, sizeof(pcp_rtpp_info_t));

	return PCP_OK;
}

int set_cb_vtable(IN const pcp_cb_vtable_t* pvtable)
{
	if (pvtable == NULL)
		return PCP_ERR;

	PCP_CFG_CHECK(g_cfg);

	memcpy(&g_cfg->vtable, pvtable, sizeof(pcp_cb_vtable_t));

	return PCP_OK;
}

int set_system_state(IN const pcp_state_t state)
{
	g_sysstate = state;

	return PCP_OK;
}

int set_randcode(IN int randcode)
{
	g_randcode = randcode;

	return PCP_OK;
}

int set_login_client_num(char * uid)
{
    if ( NULL != uid )
    {
        PCP_CFG_CHECK(g_cfg);
        strncpy(g_cfg->proxy.uid, uid, sizeof(g_cfg->proxy.uid)-1);
        return PCP_OK;
    }

    return PCP_ERR;
}

/* add by Vinton.Liu on 2016-05-30 for video fec and nack config begin */
int pcp_update_vie_fec_flag(IN const bool enabled)
{
    PCP_CFG_CHECK(g_cfg);

    if ( enabled )
    {
        g_cfg->proxy.update_flag |= PCP_VIE_FEC_OPEN;
    }
    else
    {
        g_cfg->proxy.update_flag &= ~PCP_VIE_FEC_OPEN;
    }

    return PCP_OK;
}

int pcp_update_vie_nack_flag(IN const bool enabled)
{
    PCP_CFG_CHECK(g_cfg);
    if ( enabled )
    {
        g_cfg->proxy.update_flag |= PCP_VIE_NACK_OPEN;
    }
    else
    {
        g_cfg->proxy.update_flag &= ~PCP_VIE_NACK_OPEN;
    }
    ms_report("pcp_update_vie_nack_flag: enable[%d] flag[0x%2x]", 
                enabled, g_cfg->proxy.update_flag);

    return PCP_OK;
}
/* Vinton.Liu add end */

OUT int get_system_state(void)
{
	return g_sysstate;
}

OUT int get_randcode(void)
{
	return g_randcode;
}

OUT int get_ice_type(void)
{
	PCP_CFG_CHECK(g_cfg);
	
	return g_cfg->proxy.ice_type;
}

OUT int get_upate_flag(void)
{
	PCP_CFG_CHECK(g_cfg);

	return g_cfg->proxy.update_flag;
}

OUT int get_platform(void)
{
	PCP_CFG_CHECK(g_cfg);
	
	return g_cfg->proxy.platform;
}

/* add by VintonLiu on 20150709 for IM3.0 */
OUT int get_account_type(void)
{
    PCP_CFG_CHECK(g_cfg);
    return g_cfg->proxy.atype;
}
/* VintonLiu add end */

int get_brand(OUT char* brand)
{
	if (brand == NULL)
		return PCP_ERR;

	PCP_CFG_CHECK(g_cfg);

	strcpy(brand, g_cfg->proxy.brand);

	return PCP_OK;
}

int get_uid(OUT char* uid)
{
    if (NULL == uid)
		return PCP_ERR;
	
	PCP_CFG_CHECK(g_cfg);

	strcpy(uid, g_cfg->proxy.uid);

	return PCP_OK;
}

int get_userid(OUT char* userid)
{
	if ( NULL == userid )
		return PCP_ERR;
	
	PCP_CFG_CHECK(g_cfg);

	strcpy(userid, g_cfg->proxy.userid);

	return PCP_OK;
}

int get_userphone(OUT char* phone)
{
	if (NULL == phone)
		return PCP_ERR;
	
	PCP_CFG_CHECK(g_cfg);

	strcpy(phone, g_cfg->proxy.phone);

	return PCP_OK;
}

int get_nickname(OUT char* nickname)
{
    if (NULL == nickname)
		return PCP_ERR;
	
	PCP_CFG_CHECK(g_cfg);

	strcpy(nickname, g_cfg->proxy.nickname);

	return PCP_OK;
}

int get_proxy_version(OUT char* version)
{
	if (NULL == version)
		return PCP_ERR;
	
	PCP_CFG_CHECK(g_cfg);

	strcpy(version, g_cfg->proxy.version);

	return PCP_OK;
}

int get_rtpp_cfg(OUT pcp_rtpp_info_t* pcfg)
{
	if (NULL == pcfg)
		return PCP_ERR;

	PCP_CFG_CHECK(g_cfg);

	memcpy(pcfg, &g_cfg->rtpp, sizeof(pcp_rtpp_info_t));

	return PCP_OK;
}

void register_event_cb(IN int type, IN int reason, IN const char* describe)
{
	if (g_cfg != NULL && g_cfg->vtable.register_event_cb)
		g_cfg->vtable.register_event_cb(type, reason, describe);
}

void call_event_cb(IN int type, IN int reason, IN const char* describe, IN void* param, IN int size)
{
	if (g_cfg != NULL && g_cfg->vtable.call_event_cb)
		g_cfg->vtable.call_event_cb(type, reason, describe, param, size);
}

void call_send_cb(IN const char* msg, IN int slen)
{
	if (g_cfg != NULL && g_cfg->vtable.call_send_cb)
		g_cfg->vtable.call_send_cb(msg, slen);
}

void call_state_cb(IN int state)
{
	if (g_cfg != NULL && g_cfg->vtable.call_state_cb)
		g_cfg->vtable.call_state_cb(state);
}

void call_log_cb(IN int level, IN const char* describe)
{
	if (g_cfg != NULL && g_cfg->vtable.call_log_cb)
		g_cfg->vtable.call_log_cb(level, describe);
}

