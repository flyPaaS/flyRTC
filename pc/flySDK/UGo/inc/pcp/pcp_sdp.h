/******************************************************************************

  Copyright (C), 2001-2011, DCN Co., Ltd.

 ******************************************************************************
  File Name     : pcp_sdp.h
  Version       : Initial Draft
  Author        : gonghuojin
  Created       : 2013/9/24
  Last Modified :
  Description   : sdp manager header
  Function List :
  History       :
  1.Date        : 2013/9/24
    Author      : gonghuojin
    Modification: Created file

******************************************************************************/
#ifndef PCP_SDP_H
#define PCP_SDP_H

#include "pcp_cfg.h"
#include "pcp_parser.h"

PCP_EXTERN_C_START

void null_sdp();

typedef enum pcp_consultation_error
{
	PCP_BOTH_CONSULT_FAILD = 0,	
	PCP_AUDIO_CONSULT_FAILD = 1,
	PCP_VIDEO_CONSULT_FAILD = 2,
	PCP_BOTH_CONSULT_SUCCESS = 3
	
}pcp_consult_err;


OUT pcp_sdp_info_t* create_sdp(IN pcp_bool f_def, IN pcp_bool video_enabled);

void free_sdp(IN pcp_sdp_info_t* sdp);

int media_sdp_consultation(IN const pcp_sdp_info_t* local_sdp, IN const pcp_sdp_info_t* remote_sdp);

int update_sdp_ice_params(OUT pcp_sdp_info_t* psdp, IN const pcp_ice_info_t* pice);

int update_sdp_rtpp_params(OUT pcp_sdp_info_t* psdp, IN const pcp_rtpp_info_t* pice);
//added by luochengcong 20141009
int update_sdp_through_params(OUT pcp_sdp_info_t* psdp, IN const pcp_through_info_t* pthrough);
//end by luochengcong 20141009
int get_audio_codecs(OUT pcp_codec_list_t* pcodecslist);

int update_codecs_params(IN const pcp_codec_list_t* pcodecslist);

int best_media_option(IN int type, IN const pcp_media_info_t* pmedia, OUT pcp_media_option_t* poption);

int check_payload(IN const pcp_media_info_t* media, IN int pt);

int check_sdp_availably(IN const pcp_sdp_info_t* psdp);

void pcp_getptypename(int pt, char *name);

//added by luochengcong 20150105
int pcp_get_updatereq_pt_info(char* media, pcp_codec_list_t* media_list);
int pcp_update_media_consultion(pcp_update_req_t* updatereq, pcp_update_rsp_t* updatersp, pcp_session_t* psession);
//added by luochengcong 20150105 ended
OUT pcp_conf_state_t* create_conference();//add by wuzhaoyang20140605
void free_conference(IN pcp_conf_state_t* conf_state);//add by wuzhaoyang20140605

PCP_EXTERN_C_END

#endif

