#include "pcp_sdp.h"
#include "pcp_comm.h"
#include "getaddress.h"
#include "cfg.h"


/*------sdp  const params  define-------*/
const int kDefaultAudioPort = 10054;
const int kDefaultVideoPort = 10056;

const char kSessionVersion[]   = "0";		
const char kMediaTypeVideo[]   = "v";		//video
const char kMediaTypeAudio[]   = "a";		//audio

static pcp_codec_param_t codec_audio_order[]=
{ 
#if defined(UGO_BUILD_CSRV_ENABLED)
    { TRUE, 18,  "g729" },
    { TRUE, 113, "ilbc" },
#else
	{ TRUE, 111, "opus" },
	{ TRUE, 112, "opusnb" },
#ifdef _WIN32
	{ TRUE, 106, "silk" },
#endif
	{ TRUE, 107, "amr" },
	{ TRUE, 115, "silkwb" },
	{ TRUE, 18,  "g729" },
	{ TRUE, 113, "ilbc" },
	{ FALSE, 116, "opusfb" },
	{ FALSE, 0,   "pcmu"},
	{ TRUE, 124, "aac" },
#endif
};

static pcp_codec_param_t codec_video_order[]=
{
	{TRUE, 125, "H265" },
//	{TRUE, 122, "VP8"},
	{TRUE, 121, "H264"},
//	{TRUE, 97, "VIDEOFEC"},
//	{TRUE, 123, "VP9"}
};

static pcp_codec_list_t updata_codec;
static int videoPtFirst = 121;  // H.264->121 , H.265->125

static int check_media_consult(const pcp_media_info_t* media, const char* ptype);
static int set_sdp_string_param(char* param, const char* value);
static int set_sdp_int_param(int *param, const int value);
static int local_media_sdp(pcp_media_info_t* pmedia, pcp_bool video_enabled);
static int get_default_sdp_proxy(pcp_sdp_info_t* sdp, pcp_bool video_enabled);
static void st_ret(void* st, int size);
static void st_copy(void* der, const void* src, int size);
static void media_codecs_clean(void);
static int media_codecs_check(int pt);


void st_ret(void* st, int size)
{
	memset(st, 0, size);	
}

void st_copy(void* der, const void* src, int size)
{
	memcpy(der, src, size);
}

void media_codecs_clean(void)
{
	int audio_cnt = 0;
	int video_cnt = 0;
	int i = 0;

	audio_cnt = sizeof(codec_audio_order)/sizeof(pcp_codec_param_t);
	if (audio_cnt != 0)
	{
		for(i = 0; i < audio_cnt; i++)
		{
			codec_audio_order[i].enabled = FALSE;
		}
	}

	video_cnt = sizeof(codec_video_order)/sizeof(pcp_codec_param_t);
	if (video_cnt != 0)
	{
		for(i = 0; i < video_cnt; i++)
		{
			codec_video_order[i].enabled = FALSE;
		}
	}
	
}


int media_codecs_check(int pt)
{
	int audio_cnt = 0;
	//int video_cnt = 0;
	int i = 0;

	audio_cnt = sizeof(codec_audio_order)/sizeof(pcp_codec_param_t);
	if (audio_cnt != 0)
	{
		for(i = 0; i < audio_cnt; i++)
		{
			if (codec_audio_order[i].pt == pt)
			{
				return i;
			}
		}
	}

#if 0
	video_cnt = sizeof(codec_video_order)/sizeof(pcp_codec_param_t);
	if (video_cnt != 0)
	{
		for(i = 0; i < video_cnt; i++)
		{
			if (codec_video_order[i].pt == pt)
			{
				return PCP_OK;
			}
		}
	}
#endif

	return PCP_ERR;
}

int get_audio_codecs(OUT pcp_codec_list_t* pcodecslist)
{
	int audio_cnt = 0;
	int i = 0;
	int m_x = 0;
	
	if ( NULL == pcodecslist )
		return PCP_ERR;

	audio_cnt = sizeof(codec_audio_order)/sizeof(pcp_codec_param_t);
	if (audio_cnt != 0)
	{
		for(i = 0; i < audio_cnt; i++)
		{
			pcodecslist->codec_arr[m_x] = codec_audio_order[i];
			m_x++;
		}
	}

	pcodecslist->num = m_x;

	return PCP_OK;
}


int update_codecs_params(IN const pcp_codec_list_t* pcodecslist)
{
	int i = 0;
	int aindex = 0;
	
	if (NULL == pcodecslist)
		return PCP_ERR;

 	memset(&updata_codec, 0x00, sizeof(pcp_codec_list_t));

 	for (i = 0; i < pcodecslist->num; i++)
 	{
        if ( (aindex = media_codecs_check(pcodecslist->codec_arr[i].pt)) != PCP_ERR )
        {
            memcpy(&updata_codec.codec_arr[updata_codec.num], \
                   &pcodecslist->codec_arr[i], sizeof(pcp_codec_param_t));
            updata_codec.num++;

			codec_audio_order[aindex].enabled = pcodecslist->codec_arr[i].enabled;
        }
 	}
	
	return PCP_OK;
}

static int local_media_sdp(pcp_media_info_t* pmedia, pcp_bool video_enabled)
{
	int audio_cnt = 0;
	int video_cnt = 0;
	int i = 0, j = 0;
	int m_idx = 0;
    int isfind = 0;
    int nextpri = 0;
	
	if (NULL == pmedia)
		return PCP_ERR;

	if (updata_codec.num == 0)
	{
		audio_cnt = sizeof(codec_audio_order)/sizeof(pcp_codec_param_t);
		if (audio_cnt != 0)
		{
			for(i = 0; i < audio_cnt; i++)
			{
				if (codec_audio_order[i].enabled)
				{
					set_sdp_int_param(&pmedia->media_arr[m_idx].flag, FALSE);
					set_sdp_int_param(&pmedia->media_arr[m_idx].pr, i);
					set_sdp_int_param(&pmedia->media_arr[m_idx].port, kDefaultAudioPort);
					set_sdp_string_param(pmedia->media_arr[m_idx].type, kMediaTypeAudio);
					set_sdp_int_param(&pmedia->media_arr[m_idx].pt,codec_audio_order[i].pt);
					set_sdp_string_param(pmedia->media_arr[m_idx].mine, codec_audio_order[i].mine);
					m_idx++;
				}
			}
		}
	} 
	else
	{
		for (i = 0; i < updata_codec.num; i++)
		{
			if (updata_codec.codec_arr[i].enabled)
			{
				set_sdp_int_param(&pmedia->media_arr[m_idx].flag, FALSE);
				set_sdp_int_param(&pmedia->media_arr[m_idx].pr, nextpri);
				set_sdp_int_param(&pmedia->media_arr[m_idx].port, kDefaultAudioPort);
				set_sdp_string_param(pmedia->media_arr[m_idx].type, kMediaTypeAudio);
				set_sdp_int_param(&pmedia->media_arr[m_idx].pt,updata_codec.codec_arr[i].pt);
				set_sdp_string_param(pmedia->media_arr[m_idx].mine, updata_codec.codec_arr[i].mine);
				m_idx++;
				nextpri++;
			}
		}
		audio_cnt = sizeof(codec_audio_order)/sizeof(pcp_codec_param_t);
		if (audio_cnt != 0)
		{
			for(i = 0; i < audio_cnt; i++)
			{
				if (codec_audio_order[i].enabled)
				{
                    /* add by VintonLiu on 20150105 for filter the same codec */
                    isfind = 0;
                    for ( j = 0; j < updata_codec.num; j++ )
                    {
                        if ( updata_codec.codec_arr[j].enabled
                            && (codec_audio_order[i].pt == updata_codec.codec_arr[j].pt) )
                        {
                            isfind = 1;
                            break;
                        }
                    }
                    if ( isfind )
                    {
                        continue;
                    }
                    /* VintonLiu add end */
                    
					set_sdp_int_param(&pmedia->media_arr[m_idx].flag, FALSE);
					set_sdp_int_param(&pmedia->media_arr[m_idx].pr, nextpri);
					set_sdp_int_param(&pmedia->media_arr[m_idx].port, kDefaultAudioPort);
					set_sdp_string_param(pmedia->media_arr[m_idx].type, kMediaTypeAudio);
					set_sdp_int_param(&pmedia->media_arr[m_idx].pt,codec_audio_order[i].pt);
					set_sdp_string_param(pmedia->media_arr[m_idx].mine, codec_audio_order[i].mine);					
					m_idx++;
					nextpri++;
				}
			}
		}
	}
	//set audio media info 
// 	audio_cnt = sizeof(codec_audio_order)/sizeof(pcp_codec_param_t);
// 	if (audio_cnt != 0)
// 	{
// 		for(i = 0; i < audio_cnt; i++)
// 		{
// 			if (codec_audio_order[i].enabled)
// 			{
// 				set_sdp_int_param(&pmedia->media_arr[m_idx].flag, FALSE);
// 				set_sdp_int_param(&pmedia->media_arr[m_idx].pr, i);
// 				set_sdp_int_param(&pmedia->media_arr[m_idx].port, kDefaultAudioPort);
// 				set_sdp_string_param(pmedia->media_arr[m_idx].type, kMediaTypeAudio);
// 				set_sdp_int_param(&pmedia->media_arr[m_idx].pt,codec_audio_order[i].pt);
// 				set_sdp_string_param(pmedia->media_arr[m_idx].mine, codec_audio_order[i].mine);
// 				m_idx++;
// 			}
// 		}
// 	}
	
	//set video media info
	if (video_enabled)
	{
		video_cnt = sizeof(codec_video_order)/sizeof(pcp_codec_param_t);
		if (video_cnt != 0)
		{
			for(i = 0; i < video_cnt; i++)
			{
				if (codec_video_order[i].enabled)
				{
					set_sdp_int_param(&pmedia->media_arr[m_idx].flag, FALSE);
					set_sdp_int_param(&pmedia->media_arr[m_idx].pr, i);
					set_sdp_int_param(&pmedia->media_arr[m_idx].port, kDefaultVideoPort);
					set_sdp_string_param(pmedia->media_arr[m_idx].type, kMediaTypeVideo);
					set_sdp_int_param(&pmedia->media_arr[m_idx].pt,codec_video_order[i].pt);
					set_sdp_string_param(pmedia->media_arr[m_idx].mine, codec_video_order[i].mine);
					m_idx++;
				}
			}
		}
	}

	pmedia->media_num = m_idx;

	return PCP_OK;
}

int set_sdp_string_param(char* param, const char* value)
{
	strcpy(param, value);
	
	return PCP_OK;
}

int set_sdp_int_param(int *param, const int value)
{
	*param = value;
	return PCP_OK;
}

int get_default_sdp_proxy(pcp_sdp_info_t* sdp, pcp_bool video_enabled)
{
	int res = 0;
	int ipv6Enabled;
	char local_addr[64] = {0};

	if (sdp == NULL)
		return PCP_ERR;

	st_ret(sdp, sizeof(pcp_sdp_info_t));
	pcp_get_localipaddr(local_addr);
	if ( !strlen(local_addr) )
	{
        ipv6Enabled = pcp_is_ipv6_enabled();
#if defined (__APPLE__) 
        res = get_ip_address_by_ifname(ipv6Enabled, "en0", local_addr);	
#elif defined (ANDROID)
    	res = get_ip_address_by_ifname(ipv6Enabled, "wlan0", local_addr);	
#elif defined (WEBRTC_LINUX)
	    res = get_ip_address_by_ifname(ipv6Enabled, "eth0", local_addr);		
#elif defined (_WIN32)
	    res = get_local_address(local_addr);
#endif
    }
	if (res == 0)
	{
		set_sdp_string_param(sdp->c_ip, local_addr);
	}
	
	set_sdp_string_param(sdp->v_version, kSessionVersion);

	if ( video_enabled )
	{
	    set_sdp_int_param(&sdp->support_v, video_enabled);
	}
	
	local_media_sdp(&sdp->media, video_enabled);

	return PCP_OK;
}

int check_payload(IN const pcp_media_info_t* media, IN int pt)
{
	int i = 0;
	int res = PCP_ERR;
	
	if (NULL == media)
		return res;

	for (i =0; i < media->media_num; i++)
	{
		if (pt == media->media_arr[i].pt)
		{
			res = i;
			break;
		}
	}

	return res;
}

int check_sdp_availably(IN const pcp_sdp_info_t* psdp)
{
	if (NULL == psdp)
		return FALSE;
	
	return ((psdp->media.media_num != 0) ? TRUE : FALSE);
}


int check_media_consult(const pcp_media_info_t* media, const char* ptype)
{
	int i = 0;
	bool res = PCP_ERR;

	if (NULL == media)
		return res;


	for (i=0; i<media->media_num; i++)
	{
		if (strcmp(media->media_arr[i].type, ptype) == 0)
		{
			if (media->media_arr[i].flag)
			{
				res = PCP_OK;
				break;
			}	
		}
	}

	return res;
}


OUT pcp_sdp_info_t* create_sdp(IN pcp_bool f_def, IN pcp_bool video_enabled)
{
	pcp_sdp_info_t* psdp = pcp_malloc(sizeof(pcp_sdp_info_t));

	if (psdp == NULL)
	{
        ms_error("[create_sdp] malloc failed.");
		return NULL;
	}
    
	if (f_def)
		get_default_sdp_proxy(psdp, video_enabled);
	
	return psdp;
}

void free_sdp(IN pcp_sdp_info_t* sdp)
{
	if (sdp != NULL)
	{
		pcp_free(sdp);
		sdp = NULL;
	}
}

int media_sdp_consultation(IN const pcp_sdp_info_t* local_sdp, IN const pcp_sdp_info_t* remote_sdp)
{
	int i = 0;	
	int idx = 0;
	int res = PCP_BOTH_CONSULT_FAILD;
	int bFirstVideo = 0;

	pcp_media_info_t* lmedia = NULL;
	pcp_media_info_t* rmedia = NULL; 

	if (local_sdp == NULL || remote_sdp == NULL)
		return res;

	lmedia = (pcp_media_info_t*)&local_sdp->media;
	rmedia = (pcp_media_info_t*)&remote_sdp->media;

	for (i=0; i<rmedia->media_num; i++)
	{
		if (strcmp(rmedia->media_arr[i].type, "a") == 0 
			|| strcmp(rmedia->media_arr[i].type, "v") == 0)
		{
			idx = check_payload(lmedia, rmedia->media_arr[i].pt);

			if ( strcmp(rmedia->media_arr[i].type, "v") == 0)
			{				
				if(( idx >= 0) && (rmedia->media_arr[i].pt == videoPtFirst))
				{
					bFirstVideo = 1;
				}
			}
			if(idx < 0)
			{
				//if pt not find set this media flag to false
				rmedia->media_arr[i].flag = FALSE;
				lmedia->media_arr[idx].flag = FALSE;
			}
			else
			{	//set media consult flag to true;
				rmedia->media_arr[i].flag = TRUE;
				lmedia->media_arr[idx].flag = TRUE;
			}
		}
	}

    
	for (i=0; i<lmedia->media_num; i++)
	{
		if ( strcmp(lmedia->media_arr[i].type, "v") == 0)
		{
			if(bFirstVideo && (lmedia->media_arr[i].pt != videoPtFirst))
			{				
				lmedia->media_arr[i].flag = FALSE;
			}
		}
	}


	for (i=0; i<rmedia->media_num; i++)
	{
		if ( strcmp(rmedia->media_arr[i].type, "v") == 0)
		{
			if(bFirstVideo && (rmedia->media_arr[i].pt != videoPtFirst))
			{				
				rmedia->media_arr[i].flag = FALSE;
			}
		}
	}

	if (check_media_consult(lmedia, "a") == PCP_OK) res |= 0x02;
		
	if (check_media_consult(lmedia, "v") == PCP_OK) res |= 0x01;
		
	return res;
}

int update_sdp_ice_params(OUT pcp_sdp_info_t* psdp, IN const pcp_ice_info_t* pice)
{
	if (psdp == NULL || pice == NULL)
		return PCP_ERR;

	st_copy(&psdp->ice, pice, sizeof(pcp_ice_info_t));

	return PCP_OK;
}

int update_sdp_rtpp_params(OUT pcp_sdp_info_t* psdp, IN const pcp_rtpp_info_t* prtpp)
{
	if (psdp == NULL || prtpp == NULL)
		return PCP_ERR;

	st_copy(&psdp->rtpp, prtpp, sizeof(pcp_rtpp_info_t));

	return PCP_OK;
}

/*added by luochengcong 20141009*/
int update_sdp_through_params(OUT pcp_sdp_info_t* psdp, IN const pcp_through_info_t* pthrough)
{
    if (psdp == NULL)	
		return PCP_ERR;
	if(pthrough == NULL)
	{
		memset(&psdp->through, 0, sizeof(pcp_through_info_t));
		return PCP_OK;
	}
    
	st_copy(&psdp->through, pthrough, sizeof(pcp_through_info_t));
    
	return PCP_OK;
}
/*end by luochengcong 20141009*/

int best_media_option(IN int type, IN const pcp_media_info_t* pmedia, OUT pcp_media_option_t* poption)
{
	int i = 0;
	int isfirst = TRUE;
	pcp_media_option_t m_option;
	char stype[21] = {0};
	
	if (NULL == poption || pmedia == NULL)
		return PCP_ERR;

	if (type == VIDEO) 
		strcpy(stype, "v");
	else 
		strcpy(stype, "a");

	res_st(&m_option, sizeof(m_option));
	for(i = 0; i < pmedia->media_num; i++)
	{
		if (strcmp(pmedia->media_arr[i].type, stype) == 0
			&& pmedia->media_arr[i].flag)
		{
			if (isfirst)
			{
				st_copy(&m_option, &pmedia->media_arr[i], sizeof(m_option));
				isfirst = FALSE;
				continue;
			}

			if (m_option.pr > pmedia->media_arr[i].pr)
			{
				st_copy(&m_option, &pmedia->media_arr[i], sizeof(m_option));
			}
		}
	}

	st_copy(poption, &m_option, sizeof(m_option));
	
	return PCP_OK;
}


//add by wuzhaoyang20140605 start
OUT pcp_conf_state_t* create_conference()
{
	pcp_conf_state_t* pconf = pcp_malloc(PCP_CONF_STATE_SIZE);

	if (pconf == NULL)
		return NULL;

	return pconf;
}

void free_conference(IN pcp_conf_state_t* con_state)
{
	if (con_state != NULL)
	{
		pcp_free(con_state);
		con_state = NULL;
	}
}
//add by wuzhaoyang20140605 end

int get_video_codec(pcp_codec_param_t**p, int *num, int **firstPt)
{
	if(p)
		*p = codec_video_order;
	if(num)
		*num = sizeof(codec_video_order)/sizeof(pcp_codec_param_t);
	if(firstPt)
		*firstPt = &videoPtFirst;
	return 0;
}
//added by luochengcong 20150105
int pcp_get_updatereq_pt_info(char *media, pcp_codec_list_t *media_list)
{
    char optstr[64]={0};
	char *p = NULL;
	int i = 0;
	char *delim = ",";

	if (media!=NULL && strlen(media)!=0 && media_list!=NULL) {
		strcpy(optstr, media);
	}
	else
	{
		return PCP_ERR;
	}

    // eg: "18,113,112,116"
    p = strtok(optstr, delim);
    while( p )
    {
        media_list->codec_arr[i++].pt = atoi(p);
        p = strtok(NULL, delim);
    }
	media_list->num = i;

	return PCP_OK;
}

int pcp_update_media_consultion(pcp_update_req_t *updatereq, pcp_update_rsp_t *updatersp ,pcp_session_t* psession)
{
	int i=0;
	int j=0;
	int media_cnt=0;
	int errornum=0;

	char *p=NULL;
	char audiostring[64]={0};
	char videostring[64]={0};
	char buf[64]={0};
	pcp_codec_list_t audio_pt_info;
	pcp_codec_list_t video_pt_info;
	pcp_media_option_t audio_hight_pr_option;
	pcp_media_option_t video_hight_pr_option;

	if(updatereq==NULL || updatersp==NULL || psession ==NULL){
		return PCP_ERR;
	}
	
	memset(&audio_pt_info, 0, sizeof(pcp_codec_list_t));
	memset(&video_pt_info, 0, sizeof(pcp_codec_list_t));


	errornum = pcp_get_updatereq_pt_info(updatereq->apt, &audio_pt_info);
	errornum = pcp_get_updatereq_pt_info(updatereq->vpt, &video_pt_info);

	media_cnt = psession->lsdp->media.media_num;

	for(i=0;i<media_cnt;++i){
		psession->lsdp->media.media_arr[i].flag=FALSE;
	}

	for (i=0; i<media_cnt; ++i) {
		if(strcmp(psession->lsdp->media.media_arr[i].type, "a")==0){
			for (j=0; j<audio_pt_info.num; ++j) {
				if (psession->lsdp->media.media_arr[i].pt == audio_pt_info.codec_arr[j].pt) {
					audio_pt_info.codec_arr[j].enabled=TRUE;
					psession->lsdp->media.media_arr[i].flag = TRUE;
					break;
				}
			}
		}

		if(strcmp(psession->lsdp->media.media_arr[i].type, "v")==0){
			for (j=0; j<video_pt_info.num; ++j) {
				if (psession->lsdp->media.media_arr[i].pt == video_pt_info.codec_arr[j].pt) {
					video_pt_info.codec_arr[j].enabled=TRUE;
					psession->lsdp->media.media_arr[i].flag = TRUE;
					break;
				}
			}
		}
	}

	//get the pr hightest
//	get_best_mediastream(IN int role,IN int type,OUT pcp_session_t * psession);
	memset(&audio_hight_pr_option,0, sizeof(pcp_media_option_t));
	memset(&video_hight_pr_option,0, sizeof(pcp_media_option_t));
	errornum=best_media_option(AUDIO,&(psession->lsdp->media),&audio_hight_pr_option);
	if(errornum == PCP_OK)
		psession->audio_stream.session_pt=audio_hight_pr_option.pt;
	if(psession->video_flag){
		errornum=best_media_option(VIDEO,&(psession->lsdp->media),&video_hight_pr_option);
		if(errornum == PCP_OK)
			psession->video_stream.session_pt=video_hight_pr_option.pt;
	}
	
	ms_report("The psession->audio_stream.session_pt is %d",psession->audio_stream.session_pt);
//output the value to server
	for (i=0; i< media_cnt; ++i) {
		if(strcmp(psession->lsdp->media.media_arr[i].type, "a")==0){
			if (psession->lsdp->media.media_arr[i].flag) {
				sprintf(buf,"%d",psession->lsdp->media.media_arr[i].pt);
				strcat(audiostring,buf);
				strcat(audiostring, ",");
				memset(buf,0,sizeof(buf));
			}
		}

		if(strcmp(psession->lsdp->media.media_arr[i].type, "v")==0){
			if (psession->lsdp->media.media_arr[i].flag) {
				sprintf(buf,"%d",psession->lsdp->media.media_arr[i].pt);
				strcat(videostring, buf);
				strcat(videostring, ",");
				memset(buf,0,sizeof(buf));
			}
		}
	}

	p=audiostring;
	if(psession->audio_flag!=0 && strlen(audiostring)!=0)
		memset(p+strlen(audiostring)-1, 0, sizeof(char));
	p=NULL;

	p=videostring;
	if(psession->video_flag!=0 && strlen(videostring)!=0)
		memset(p+strlen(videostring)-1, 0, sizeof(char));
	p=NULL;

	strcpy(updatersp->apt, audiostring);
	strcpy(updatersp->vpt, videostring);

	ms_report("The updatersp.apt is %s",updatersp->apt);
	ms_report("The updatersp.vpt is %s", updatersp->vpt);
	return PCP_OK;
}

void pcp_getptypename(int pt, char *name)
{
	int pt_cnt = sizeof(codec_audio_order)/sizeof(pcp_codec_param_t);
	int i = 0;
	if ( NULL == name )
	{
		return;
	}
	
	for ( i = 0; i < pt_cnt; i++ )
	{
		if ( pt == codec_audio_order[i].pt )
		{
			strcpy(name, codec_audio_order[i].mine);
			break;
		}
	}
}
//added end by luochengcong 20150105