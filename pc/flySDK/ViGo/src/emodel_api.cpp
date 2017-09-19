#include <stdio.h>
#include <string.h>
#include "emodel_api.h"

#ifdef EMODEL_ENABLED
#include "MediaEngineInterface.h"

static eEmodel_state g_emodelflag = eEMD_STOP_S;
static st_emodel_calculate g_calcvalue;
static void convert_emodel_option(const st_emodel_calc_t* fix_info, ME_emodel_calc_info_t* flt_info, float qx);
static void convert_emodel_value(const st_emodel_calculate* fix_ecalc, ME_emodel_info_t* flt_ecalc);
static bool check_emodel_value(int type, const st_emodel_calc_t* pvalue);
static void clean_store_calcvalue(void);
static void update_store_calcvalue(const st_emodel_calculate* pcalcvalue);
static bool check_calcvalue_is_new(const st_emodel_calculate* pcalcvalue);
static bool compare_emodel_option(st_emodel_calc_t* psrc, st_emodel_calc_t* pdrc);
static void clean_emodel_value_flag(ME_emodel_info_t* pemodevalue);

/********************************************
		module pritive function 
		
**********************************************/
void clean_store_calcvalue(void)
{
	memset(&g_calcvalue, 0, sizeof(g_calcvalue));
}

void update_store_calcvalue(const st_emodel_calculate* pcalcvalue)
{
	if (pcalcvalue != NULL)
		memcpy(&g_calcvalue, pcalcvalue, sizeof(st_emodel_calculate));
}

bool compare_emodel_option(st_emodel_calc_t* psrc, const st_emodel_calc_t* pdrc)
{
	if (NULL == psrc || NULL == pdrc)
		return false;
	
	if (psrc->average != pdrc->average)
		return false;

	if (psrc->total != pdrc->total)
		return false;

	if (psrc->current != pdrc->current)
		return false;

	if (psrc->count != pdrc->count)
		return false;

	if (psrc->min != pdrc->min)
		return false;

	if (psrc->max != pdrc->max)
		return false;

	return true;
}

bool check_calcvalue_is_new(const st_emodel_calculate* pcalcvalue)
{
	if (pcalcvalue == NULL)
		return false;

	return (!compare_emodel_option(&g_calcvalue.emodel_mos, &pcalcvalue->emodel_mos));
}

void clean_emodel_value_flag(ME_emodel_info_t* pemodevalue)
{
	if (pemodevalue)
	{
		pemodevalue->emodel_burstr.flag = 0;
		pemodevalue->emodel_ie.flag = 0;
		pemodevalue->emodel_mos.flag = 0;
		pemodevalue->emodel_ppl.flag = 0;
		pemodevalue->emodel_tr.flag = 0;
	}
}

bool check_emodel_value(int type, const st_emodel_calc_t* pvalue)
{
	if (pvalue == NULL)
		return false;

	if (pvalue->count == 0)
		return false;

	if (type == EM_TR && pvalue->max == 0)
		return false;

	return true;
}

void convert_emodel_option(const st_emodel_calc_t* fix_info, ME_emodel_calc_info_t* flt_info, float qx)
{
	if (fix_info && flt_info)
	{
		flt_info->count = fix_info->count;
		flt_info->current = fix_info->current/qx;
		flt_info->average = fix_info->average/qx;
		flt_info->max = fix_info->max/qx;
		flt_info->min = fix_info->min/qx;
		flt_info->total = fix_info->total/qx;
	}
}

void convert_emodel_value(const st_emodel_calculate* fix_ecalc, ME_emodel_info_t* flt_ecalc)
{
	if (flt_ecalc != NULL && fix_ecalc != NULL)
	{
		flt_ecalc->emodel_burstr.flag = check_emodel_value(EM_BR, &fix_ecalc->emodel_burstr);
		convert_emodel_option(&fix_ecalc->emodel_burstr, &flt_ecalc->emodel_burstr, 256.0);

		flt_ecalc->emodel_ie.flag = check_emodel_value(EM_IE, &fix_ecalc->emodel_ie);
		convert_emodel_option(&fix_ecalc->emodel_ie, &flt_ecalc->emodel_ie, 256.0);

		flt_ecalc->emodel_mos.flag = check_emodel_value(EM_MOS, &fix_ecalc->emodel_mos);
		convert_emodel_option(&fix_ecalc->emodel_mos, &flt_ecalc->emodel_mos, 256.0);

		flt_ecalc->emodel_ppl.flag = check_emodel_value(EM_PPL, &fix_ecalc->emodel_ppl);
		convert_emodel_option(&fix_ecalc->emodel_ppl, &flt_ecalc->emodel_ppl, 256.0);

		flt_ecalc->emodel_tr.flag = check_emodel_value(EM_TR, &fix_ecalc->emodel_tr);
		convert_emodel_option(&fix_ecalc->emodel_tr, &flt_ecalc->emodel_tr, 1.0);
	}
}


/********************************************
		   module public api function 
		
**********************************************/
int emd_init(void)
{
	clean_store_calcvalue();
	return pe_emodel_init();
}
	
void emd_destroy(void)
{
	clean_store_calcvalue();
	pe_emodel_reset_statistical_value();
	pe_emodel_destroy();
}

void emd_update_state(eEmodel_state state)
{
	g_emodelflag = state;
}
	
bool emd_is_started(void)
{
	return ((g_emodelflag == eEMD_START_S) ? true : false);
}

bool emd_get_value(void* pvalue)
{
	st_emodel_calculate calc_value;
	
	if (NULL == pvalue)
		return false;
	
    memset(&calc_value, 0x00, sizeof(st_emodel_calculate));
	pe_emodel_get_statistical_value(&calc_value);
	
	/*check calc value if it is new value*/
	if (check_calcvalue_is_new(&calc_value))
	{
	    convert_emodel_value(&calc_value, (ME_emodel_info_t*)pvalue);
		update_store_calcvalue(&calc_value);
	}
	else
	{
		clean_emodel_value_flag((ME_emodel_info_t*)pvalue);
	}

	return true;
}

void emd_update_ie_value(emd_codecs_bitrate_e ebitrate)
{
	 pe_emodel_update_codec_ie((codecs_bitrate_e)ebitrate);
}

void emd_calculate_value(int gq, int tr, int ppl)
{
	pe_emodel_calc(gq, tr, ppl);
}

void emd_calculate_clean(void)
{
	pe_emodel_reset_statistical_value();
}


#endif

