#include "pcp_pm.h"

static pcp_call_param_t g_callpm;

static void call_param_clean(void);

void call_param_clean(void)
{
	res_st(&g_callpm, sizeof(g_callpm));
}

void pm_param_copy(void* pm)
{
	if (pm != NULL)
		memcpy(pm, &g_callpm, sizeof(g_callpm));
}

int pm_init(void)
{
	call_param_clean();
	return PCP_OK;
}

void pm_destroy(void)
{
	call_param_clean();
}

int pm_update(IN void* pm)
{
	if (pm == NULL)
		return PCP_ERR;

	memcpy(&g_callpm, pm, sizeof(g_callpm));

	return PCP_OK;	
}

OUT void* pm_get(void)
{
	return (void*)&g_callpm;
}

int pm_clean(void)
{
	call_param_clean();

	return PCP_OK;
}

int pm_update_call_type(IN int call_type)
{
	g_callpm.calltype = call_type;

	return PCP_OK;
}


OUT char* pm_callid(void)
{
	return g_callpm.callid;
}

OUT int pm_calltype(void)
{
	return g_callpm.calltype;
}

