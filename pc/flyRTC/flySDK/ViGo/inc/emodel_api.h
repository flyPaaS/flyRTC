/******************************************************************************

  Copyright (C), 2001-2011, DCN Co., Ltd.

 ******************************************************************************
  File Name     : emodel_api.h
  Version       : Initial Draft
  Author        : gonghuojin
  Created       : 2013/11/6
  Last Modified :
  Description   : emodel api header filer
  Function List :
  History       :
  1.Date        : 2013/11/6
    Author      : gonghuojin
    Modification: Created file

******************************************************************************/   
#ifndef EMODE_API_H
#define EMODE_API_H
#include "cfg.h"


#if 0   // Cancel them, WeiYouquan, 2015.12.29

typedef enum tag_codec_bitrate_e
{
	eEMD_SILK_16KBITRATE,
	eEMD_SILK_14KBITRATE,
	eEMD_SILK_12KBITRATE,	
	eEMD_SILK_8KBITRATE,	
	eEMD_SILK_5KBITRATE,	
	eEMD_G729_8KBITRATE,	
	eEMD_AMR_4750BITRATE, 
	eEMD_AMR_5150BITRATE, 
	eEMD_AMR_5900BITRATE, 
	eEMD_AMR_6700BITRATE, 
	eEMD_AMR_7400BITRATE, 
	eEMD_AMR_7950BITRATE, 
	eEMD_AMR_10200BITRATE,
	eEMD_AMR_12200BITRATE,
    eEMD_SILKWB_30KBITRATE, 
    eEMD_SILKWB_28KBITRATE, 
    eEMD_SILKWB_26KBITRATE, 
    eEMD_SILKWB_24KBITRATE, 
    eEMD_SILKWB_22KBITRATE, 
    eEMD_SILKWB_20KBITRATE, 
    eEMD_SILKWB_18KBITRATE, 
    eEMD_SILKWB_16KBITRATE, 
    eEMD_SILKWB_14KBITRATE,
    //eEMD_Opus_30KBITRATE,
    //eEMD_Opus_28KBITRATE,
    //eEMD_Opus_26KBITRATE,
    eEMD_Opus_24KBITRATE,
    eEMD_Opus_22KBITRATE,
    eEMD_Opus_20KBITRATE,
    eEMD_Opus_18KBITRATE,
    eEMD_Opus_16KBITRATE,
    eEMD_Opus_14KBITRATE,
}emd_codecs_bitrate_e;

#else

typedef enum tag_codec_bitrate_e   
{
	eEMD_SILK_16KBITRATE,
	eEMD_SILK_14KBITRATE,
	eEMD_SILK_12KBITRATE,
	eEMD_SILK_8KBITRATE,
	eEMD_G729_8KBITRATE,
	eEMD_AAC_16KBITRATE,
	eEMD_AMR_12200BITRATE,
	eEMD_AMR_10200BITRATE,
	eEMD_AMR_7950BITRATE,
	eEMD_AMR_7400BITRATE,
	eEMD_AMR_6700BITRATE,
	eEMD_SILKWB_24KBITRATE,
	eEMD_SILKWB_22KBITRATE,
	eEMD_SILKWB_20KBITRATE,
	eEMD_SILKWB_18KBITRATE,
	eEMD_SILKWB_16KBITRATE,
	eEMD_Opus_24KBITRATE,
	eEMD_Opus_22KBITRATE,
	eEMD_Opus_20KBITRATE,
	eEMD_Opus_18KBITRATE,
	eEMD_Opus_16KBITRATE,
	eEMD_Opusnb_12KBITRATE,
	eEMD_Opusnb_10KBITRATE,
	eEMD_Opusnb_8KBITRATE,
	eEMD_Opusfb_48KBITRATE,
	eEMD_ILBC_13KBITRATE,
}emd_codecs_bitrate_e;
#endif 

#ifdef EMODEL_ENABLED
#include "chromium/src/third_party/e-model/inc/emodel.h" //emodel

typedef enum eEmodel_state
{
	eEMD_STOP_S	 = 0,
	eEMD_START_S = 1
}eEmodel_state;


int emd_init(void);

void emd_destroy(void);

void emd_update_state(eEmodel_state state);

bool emd_is_started(void);

bool emd_get_value(/*ME_emodel_info_t**/void* pvalue);

void emd_update_ie_value(emd_codecs_bitrate_e ebitrate);

void emd_calculate_value(int gq, int tr, int ppl);

void emd_calculate_clean(void);

#endif

#endif /* EMODE_API_H */

#if 0   // Added by WeiYouquan in 2015.12.29
char *p[] = {
	"eEMD_SILK_16KBITRATE",
	"eEMD_SILK_14KBITRATE",
	"eEMD_SILK_12KBITRATE",
	"eEMD_SILK_8KBITRATE",
	"eEMD_G729_8KBITRATE",
	"eEMD_AMR_12200BITRATE",
	"eEMD_AMR_10200BITRATE",
	"eEMD_AMR_7950BITRATE",
	"eEMD_AMR_7400BITRATE",
	"eEMD_AMR_6700BITRATE",
	"eEMD_SILKWB_24KBITRATE",
	"eEMD_SILKWB_22KBITRATE",
	"eEMD_SILKWB_20KBITRATE",
	"eEMD_SILKWB_18KBITRATE",
	"eEMD_SILKWB_16KBITRATE",
	"eEMD_Opus_24KBITRATE",
	"eEMD_Opus_22KBITRATE",
	"eEMD_Opus_20KBITRATE",
	"eEMD_Opus_18KBITRATE",
	"eEMD_Opus_16KBITRATE",
	"eEMD_Opusnb_12KBITRATE",
	"eEMD_Opusnb_10KBITRATE",
	"eEMD_Opusnb_8KBITRATE",
	"eEMD_Opusfb_48KBITRATE",
	"eEMD_ILBC_13KBITRATE",

};
#endif