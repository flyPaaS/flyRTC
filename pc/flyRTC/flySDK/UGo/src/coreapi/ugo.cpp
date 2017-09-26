#include "comm.h"
#include "mslist.h"
#include "uc.h"
#include "me.h"
#include "pcp.h"
#include "ugo.h"

/*******************************************************************
*                    self-check test define
*******************************************************************/

static bool g_isLoadFlag = false;/*UGo load flag*/
static ugo_cb_vtable_t g_cbtable; /*callback tables*/

static void z_st(void* st, int size);
static void copy_st( void*der, void* src, int size);
static void on_event_cb(int type, int reason, const char* describe, void* param);
static void on_send_msg_cb(const char* msg, int slen);
static void on_call_state_cb(int state);
static void on_trace_log_cb(int level, const char* describe);
static void on_rtp_encrypt_cb(unsigned char* in_data,
					        unsigned char* out_data,
					        int bytes_in,
					        int* bytes_out);
static void on_rtp_decrypt_cb(unsigned char* in_data,
    					        unsigned char* out_data,
    					        int bytes_in,
    					        int* bytes_out);
static void on_screen_shot_cb(const unsigned char* dst_argb,
                            int dst_stride, int width, int height, 
                            int islocate, ScreenType type);
static void on_media_proc_cb( ucpaas_int16* audio_10ms, 
                              ucpaas_int32 samples,
                              ucpaas_int32 samplingFreqHz,
                              ucpaas_boolean isStereo,
                              ucpaas_int16* audio_out,
                              ucpaas_int32* samples_out);

/* add by Vinton.Liu on 2016-03-21 for support external audio device push pcm to and pull pcm from engine begin */
static void on_init_playout_cb(ucpaas_uint32 sample_rate, 
                                     ucpaas_uint32 bytes_per_sample,
                                     ucpaas_uint32 num_of_channels);
static void on_init_recording_cb(ucpaas_uint32 sample_rate, 
                                     ucpaas_uint32 bytes_per_sample,
                                     ucpaas_uint32 num_of_channels);
static int on_pull_capture_data_cb(ucpaas_int16* audio_data, 
                                            ucpaas_uint32 nSamples);
static int on_push_render_data_cb(const ucpaas_int16* audio_data, 
                                            ucpaas_uint32 nSamples);
/* Vinton.Liu add end */

void z_st(void* st, int size)
{
	memset(st, 0, size);
}

void copy_st(void*der, void* src, int size)
{
	memcpy(der, src, size);
}


/**********************************
	module callback function
	
***********************************/
void on_event_cb(int type, int reason, const char* describe, void* param)
{
	if (g_cbtable.event_cb)
	{
		g_cbtable.event_cb(type, reason, describe, param);
	}
}

void on_send_msg_cb(const char* msg, int slen)
{
	if (g_cbtable.send_cb)
	{
		g_cbtable.send_cb(msg, slen);
	}
}

void on_call_state_cb(int state)
{
	ms_report("on_call_state_cb: call state changed!---%d.", state);
}

void on_trace_log_cb(int level, const char* describe)
{
	char detail[1024] = {0};
    char summary[1024] = {0};
    char *pstr = NULL;

	if (level == kME_TraceError || level == kME_TraceReport)
	{
#if 0
        pstr = strstr((char *)describe, ":");
        if(pstr != NULL)
        {
            strcpy(detail, pstr+1);
            *pstr = '\0';
			strcpy(summary, (char*)describe);
        }
		else
#endif
		{
			strcpy(summary, "TraceLog");
			strcpy(detail, describe);
		}

		if (g_cbtable.log_cb)
        {            
            g_cbtable.log_cb(summary, detail, (level == kME_TraceError)?kME_TraceError:kME_TraceReport);
        }
	}
}

#define RTP_HEADER_SIZE 12
void on_rtp_encrypt_cb(unsigned char* in_data,
					        unsigned char* out_data,
					        int bytes_in,
					        int* bytes_out)
{
	if ((g_cbtable.encrypt) && (bytes_in > RTP_HEADER_SIZE))
	{
        int iOldInCnt = bytes_in - RTP_HEADER_SIZE;  //old core data count
        int iNewInCnt = bytes_in - RTP_HEADER_SIZE;  //new core data count
        unsigned char ucOldPad;                      //old pad count
        unsigned char uchTmpBuf[1500] = {0};
        unsigned char* puchInBuf = in_data + RTP_HEADER_SIZE;  //point to core data

        /*to keep 16bit multiple using pading*/
        if (0 != (iOldInCnt&0x0f)) 
        {
            puchInBuf = uchTmpBuf;
            memcpy(puchInBuf, in_data+RTP_HEADER_SIZE, iOldInCnt);

            iNewInCnt = iOldInCnt & 0xfffffff0;
            iNewInCnt += 0x10;

            //get pad data count
            if (in_data[0]&0x20)
            {
                ucOldPad = in_data[bytes_in-1];      /* save old pad byte count */
            }
            else
            {
                in_data[0] |= 0x20;       /* Set the P bit */
                ucOldPad = 0;
            }

            memset(&puchInBuf[iOldInCnt], 0, iNewInCnt-iOldInCnt);
            puchInBuf[iNewInCnt-1] = iNewInCnt-iOldInCnt+ucOldPad;
        }
        memcpy(out_data, in_data, RTP_HEADER_SIZE);           //copy rtp header
		g_cbtable.encrypt(puchInBuf, out_data+RTP_HEADER_SIZE, iNewInCnt, bytes_out);
        if ( iNewInCnt != *bytes_out )
        {
            char tmp[256] = { 0 };
            
            *bytes_out = 0;
            
            sprintf(tmp, "on_rtp_encrypt_cb failed on inlen(%d) != outLen(%d)", 
                iNewInCnt, *bytes_out);
            on_trace_log_cb(kME_TraceError, tmp);
        }
        else
        {
            *bytes_out += RTP_HEADER_SIZE;
        }
	}
    else
    {
        *bytes_out = bytes_in;
        memcpy(out_data, in_data, bytes_in);
    }
}

void on_rtp_decrypt_cb(unsigned char* in_data,
					        unsigned char* out_data,
					        int bytes_in,
					        int* bytes_out)
{
	if ((g_cbtable.decrypt) && (bytes_in > RTP_HEADER_SIZE))
	{
        int iOldInCnt = bytes_in - RTP_HEADER_SIZE;  //old core data count
        int iNewInCnt;                               //new core data count
        unsigned char ucOldPad;                     //old pad count
        unsigned char uchTmpBuf[1500] = {0};
        unsigned char* puchInBuf = uchTmpBuf;

		g_cbtable.decrypt(in_data+RTP_HEADER_SIZE, puchInBuf, iOldInCnt, &iNewInCnt);

        if ( iNewInCnt > 0 && (iOldInCnt == iNewInCnt))
        {
            //get pad data count
            if ( in_data[0]&0x20)
            {
                ucOldPad = puchInBuf[iNewInCnt-1];      /* save old pad byte count */
                in_data[0] &= ~0x20;                 /* Clear the P bit */
                if (ucOldPad > iNewInCnt)
                {
                    return;
                }
            }
            else
            {
                ucOldPad = 0;
            }

            //copy rtp header
            memcpy(out_data, in_data, RTP_HEADER_SIZE);
            
            //copy core data
            memcpy(out_data+RTP_HEADER_SIZE, puchInBuf, iNewInCnt-ucOldPad);
            *bytes_out = RTP_HEADER_SIZE + iNewInCnt-ucOldPad;
        }
        else
        {
            char tmp[256] = { 0 };
            sprintf(tmp, "on_rtp_decrypt_cb failed on inlen(%d) != outLen(%d)", 
                iOldInCnt, iNewInCnt);
            on_trace_log_cb(kME_TraceError, tmp);
        }
	}
    else
    {
        *bytes_out = bytes_in;
        memcpy(out_data, in_data, bytes_in);
    }
}

void on_screen_shot_cb(const unsigned char* dst_argb,
                            int dst_stride, int width, int height, 
                            int islocate, ScreenType type)
{
	if (g_cbtable.screen_shot_cb)
	{
		g_cbtable.screen_shot_cb( dst_argb, dst_stride, 
		                            width, height, islocate, type);
	}
}

static void on_media_proc_cb( ucpaas_int16* audio_10ms, 
                              ucpaas_int32 samples,
                              ucpaas_int32 samplingFreqHz,
                              ucpaas_boolean isStereo,
                              ucpaas_int16* audio_out,
                              ucpaas_int32* samples_out)
{
    int minSamples = 0;

    if ( samplingFreqHz > 0 )
    {
        /* samples every 10ms */
        minSamples = (samplingFreqHz * 10 / 1000) * ( isStereo ? 2 : 1);
    }

    if ( samples < minSamples )
    {
        ms_error("on_media_proc_cb failed. samples(%d) freqHz(%d)", 
                samples, samplingFreqHz);
        return;
    }
    
    if ( g_cbtable.ext_mediaproc_cb ) 
    {
      g_cbtable.ext_mediaproc_cb(audio_10ms, samples, samplingFreqHz, isStereo, audio_out, samples_out);
    }
    else
    {
        
    }
}

/* add by Vinton.Liu on 2016-03-21 for support external audio device push pcm to and pull pcm from engine begin */
static void on_init_playout_cb(ucpaas_uint32 sample_rate, 
                                     ucpaas_uint32 bytes_per_sample,
                                     ucpaas_uint32 num_of_channels)
{
    if ( !sample_rate || !bytes_per_sample || !num_of_channels ) 
    {
        return;
    }
    
    if ( g_cbtable.initplayout_cb )
    {
        g_cbtable.initplayout_cb(sample_rate, 
                                bytes_per_sample, 
                                num_of_channels);
    }
}

static void on_init_recording_cb(ucpaas_uint32 sample_rate, 
                                     ucpaas_uint32 bytes_per_sample,
                                     ucpaas_uint32 num_of_channels)
{
    if ( !sample_rate || !bytes_per_sample || !num_of_channels ) 
    {
        return;
    }

    if ( g_cbtable.initrecording_cb )
    {
        g_cbtable.initrecording_cb(sample_rate, 
                                    bytes_per_sample, 
                                    num_of_channels);
    }
}

static int on_pull_capture_data_cb(ucpaas_sint8* audio_data, 
                                            ucpaas_uint32 size)
{
    if ( !audio_data || !size )
    {
        ms_error("on_pull_capture_data_cb failed on error param. ");
        return -1;
    }

    if ( g_cbtable.read_recording_cb )
    {
        return g_cbtable.read_recording_cb(audio_data, size);
    }

    return -1;
}

static int on_push_render_data_cb(const ucpaas_sint8* audio_data, 
                                            ucpaas_uint32 size)
{
    if ( !audio_data || !size )
    {
        ms_error("on_push_render_data_cb failed on error param. ");
        return -1;
    }

    if ( g_cbtable.write_playout_cb )
    {
        return g_cbtable.write_playout_cb(audio_data, size);
    }

    return -1;
}
/* Vinton.Liu add end */

/**********************************
		module API  function
	
***********************************/
int UGo_init(void)
{
	if (!g_isLoadFlag)
	{
		uc_cb_vtable_t cb_table;
		z_st(&cb_table, sizeof(cb_table));
		cb_table.event_cb = on_event_cb;
		cb_table.send_cb  = on_send_msg_cb;
		cb_table.state_cb = on_call_state_cb;
		cb_table.log_cb   = on_trace_log_cb;
		cb_table.encrypt  = on_rtp_encrypt_cb;
		cb_table.decrypt  = on_rtp_decrypt_cb;
		cb_table.screenshot_cb = on_screen_shot_cb;
		cb_table.extmedia_cb   = on_media_proc_cb;
        /* add by Vinton.Liu on 2016-03-21 for support external audio device push pcm to and pull pcm from engine begin */
        cb_table.initplayout_cb = on_init_playout_cb;
        cb_table.initrecording_cb = on_init_recording_cb;
        cb_table.pull_capture_cb = on_pull_capture_data_cb;
        cb_table.push_render_cb = on_push_render_data_cb;
        /* Vinton.Liu add end */
        
		if(uc_init(0, &cb_table) < 0)
		{
			return RES_ERR;
		}

		g_isLoadFlag = true;
	}

	return RES_OK;
}

int UGo_destroy(void)
{
	if (g_isLoadFlag)
	{
		uc_destroy();
		g_isLoadFlag = false;
	}

	return RES_OK;
}

int UGo_callback_vtable(ugo_cb_vtable_t* cb_vtable)
{
	if (cb_vtable == NULL)
		return RES_ERR;

	copy_st(&g_cbtable, cb_vtable, sizeof(ugo_cb_vtable_t));

	return RES_OK;
}

int UGo_update_system_callstate(int state)
{
	return uc_update_system_state(state);
}

int UGo_call_dialing(IN const ugo_call_dialing_para_t* pCallParam)
{
	if ( NULL == pCallParam )
	{
        ms_error("UGo_call_dialing: error on null dial param.");
		return RES_ERR;
	}

    if ( (pCallParam->tuid[0] == '\0') && (pCallParam->tphone[0] == '\0'))
    {
        ms_error("UGo_call_dialing: not any callee info.");
        return RES_ERR;
    }

    if ( (pCallParam->call_mode < eUGo_CM_DIRECT)
        || (pCallParam->call_mode > eUGo_CM_FREE))
    {
        ms_error("UGo_call_dialing: wrong call mode.");
        return RES_ERR;
    }
    
    return uc_call_dialing(pCallParam);
}

int UGo_ios_call_incoming_rsp(IN const ugo_ios_incoming_rsp_pm_t* callparam)
{
	if (NULL == callparam)
		return RES_ERR;

	/* BEGIN: Added by gonghuojin, 2014/4/18   
	PD:ios system call active, uxin incoming call need to hangup for busy */
	if (('\0' == callparam->callid[0] || callparam->vpsid < 0) && callparam->reason != eUGo_Reason_Success)
	{
		/*this maybe need to hangup for busy*/
		uc_call_hangup(callparam->reason);
	}	
	else
	{
		uc_ios_push_call_rsp(callparam->callid, callparam->vpsid, callparam->reason);
	}

	/* END:   Added by gonghuojin, 2014/4/18 */
	
	return RES_OK;
}

int UGo_call_answered(void)
{
	return uc_call_answer();
}

int UGo_call_hangup(int iReason)
{
	int ret = -1;
	
	ret = uc_call_hangup(iReason);

	return ret;
}

//add by wuzhaoyang201406055 start
int UGo_conference_call_dialing(IN ugo_conf_dialing_para_t *pDialParam)
{	
    if ( NULL == pDialParam )
	{
        ms_error("UGo_conference_call_dialing: error on null dial param.");
		return RES_ERR;
	}
    
	return uc_conference_call_dialing(pDialParam);
}

int UGo_conference_invite_parties(IN ugo_conf_dialing_para_t *pDialParam)
{
    if ( NULL == pDialParam )
	{
        ms_error("UGo_conference_call_dialing: error on null dial param.");
		return RES_ERR;
	}
    return uc_conference_invite_parties(pDialParam);
}

int UGo_delete_conference(int reason)
{
    return uc_delete_conference(reason);
}
//add by wuzhaoyang20140605 end

int UGo_get_state(void)
{
	return uc_get_state();
}

int UGo_tcp_update_state(int tcp_state)
{
	return uc_update_tcp_state(tcp_state);
}

int UGo_tcp_recv_msg(int recv_len,void*  recv_buf)
{
	if (recv_len == 0 || recv_buf == NULL)
		return RES_ERR;

	return uc_recv_message(recv_len, (char*)recv_buf);
}

int UGo_load_media_engine()
{
    /* mod by VintonLiu on 20151019 for code optimize */
	return me_load();
    /* VintonLiu mod end */
}

int UGo_unload_media_engine(void)
{
	return me_unload();
}

#if defined(ANDROID)
int UGo_set_android_obj(media_jni_env_pm_t* env)
{
    if ( NULL == env )
    {
        return RES_ERR;
    }
    
    return uc_set_android_obj(env);
}
#endif

int UGo_record_start(IN const media_file_record_pm_t* filerecord)
{
	if (NULL == filerecord)
		return RES_ERR;
	return me_record_start(filerecord);
}

int UGo_record_stop(void)
{
	return me_record_stop();
}

int UGo_screenshot_start(int islocal, ScreenType type)
{
	return me_screenshot_start(islocal, type);
}

int UGo_play_file(IN ucpaas_int32 mode, IN const media_file_play_pm_t* fileplay)
{
	if ( NULL == fileplay )
		return RES_ERR;

	return me_play_file(mode, fileplay);
}

int UGo_stop_file(void)
{
    return me_stop_file();
}

int UGo_send_DTMF(char c_dtmf)
{
	return me_send_dtmf(c_dtmf);
}

int UGo_set_mic_mute(char enabled)
{
	return me_set_mic_mute(enabled);
}

int UGo_get_mic_mute()
{
    return me_get_mic_mute();
}

int UGo_set_loudspeaker_status(char enabled)
{
	return me_set_loudspeaker_status(enabled);
}

int UGo_get_loudspeaker_status()
{
    return me_get_loudspeaker_status ();
}

/* add by Vinton.Liu on 2016-03-16 for Detect recording devices status begin */
int UGo_get_recording_device_status()
{
    return me_get_recording_device_status();
}
/* Vinton.Liu add end */

int UGo_set_log_file(IN const ugo_log_trace_pm_t* logtrace)
{
	if (NULL == logtrace)
		return RES_ERR;
	return me_set_log_file(logtrace->level, logtrace->filepath);
}

int UGo_enable_stack_log(const char* filepath)
{
	return uc_enable_logs(filepath);
}

int UGo_get_version(char * ugo_version)
{
	if ( NULL == ugo_version )
	{
	    return RES_ERR;
	}
	
	me_get_version(ugo_version);
	return RES_OK;
}

int  UGo_set_api(int apilevel)
{
	return me_set_api(apilevel);
}

int  UGo_video_preset( char*key, char* value)
{
    return me_video_preset(key, value);
}


int UGo_get_emodel_value(ugo_emodel_info_t* pemodel)
{
    uc_get_call_info_report(&(pemodel->callinfo));

    return me_get_emodel(&(pemodel->emodel));
}

int UGo_set_config(int methodID, void * cfg_struct, int version)
{
	int ret = RES_ERR;
	if ( NULL == cfg_struct )
	{
	    ms_error("UGo_set_config failed, moduleID(%d).", methodID);
	    return RES_ERR;
	}
	
	switch(methodID)
	{
		case UGO_CFG_PARAM_MODULE_ID:
        {
			ugo_cfg_t* pcfg;
			pcfg = (ugo_cfg_t*)cfg_struct;
            if ( pcfg->atype == 2 )
            {
                pcfg->tlv_enabled = 0;
                pcfg->compress_enabled = 0;
            }

#ifndef VIGO_ENABLED
            pcfg->video_enabled = 0;
#endif
			ret = uc_set_cfg(pcfg);
		}
			break;
            
		case UGO_CFG_TCP_MODULE_ID:
        {
			ugo_cfg_tcp_t* tcp;
			tcp = (ugo_cfg_tcp_t*)cfg_struct;
			if ( tcp->tcp_enabled && tcp->tcp_srvaddr[0] == '\0')
			{
			    return RES_ERR;
			}
			ret = uc_set_tcp_cfg(tcp);
		}
			break;
            
		case UGO_CFG_ICE_MODULE_ID:
        {
			ugo_cfg_ice_t* ice;
			ice = (ugo_cfg_ice_t*)cfg_struct;
			ret = uc_set_ice_cfg(ice);
		}
			break;
            
		case UGO_RTPP_CFG_MODULE_ID:
        {
			ugo_rtpp_cfg_t* rtpp;
			rtpp = (ugo_rtpp_cfg_t*)cfg_struct;
			ret  = uc_set_rtpp_cfg(rtpp);
		}
			break;
            
		case ME_VIDEO_ENC_CFG_MODULE_ID:
        {
            uc_set_video_codecs(cfg_struct);
			ret = me_set_cfg(methodID, cfg_struct);
		}
			break;

		case ME_CTRL_CFG_MODULE_ID:
        {
            ME_CTRL_cfg_t* pCtrlCfg = (ME_CTRL_cfg_t*)cfg_struct;
			ret = me_set_cfg(methodID, cfg_struct);
            uc_set_directcallfec_enable(pCtrlCfg->ucFecEnable);
            uc_set_video_fec_enabled(pCtrlCfg->vieFecEnabled);
            uc_set_video_nack_enabled(pCtrlCfg->vieNackEnabled);
		}
			break;
		case ME_VQE_CFG_MODULE_ID:
		case ME_RTP_CFG_MODULE_ID:
		case ME_ENV_CFG_MODULE_ID:
		case ME_VIDEO_DEC_CFG_MODULE_ID:
		case ME_VIDEO_RENDER_CFG_MODULE_ID:
		case ME_VIDEO_PROCES_CFG_MODULE_ID:
			ret = me_set_cfg(methodID, cfg_struct);
			break;
            
		case ME_CODECS_CFG_MODULE_ID:
			ret = uc_set_codecs(cfg_struct);
			break;
            
		case ME_VIDEO_EXTERN_CAPTURE_CFG_MODULE_ID:
			ret = me_set_cfg(methodID, cfg_struct);
			break;
            
		default:
			break;
	
	}
	return ret;
}

int UGo_get_config(int methodID,void *cfg_struct,int version)
{
	int ret = -1;
	switch(methodID)
	{
		case UGO_CFG_PARAM_MODULE_ID:
			ret = uc_get_cfg(cfg_struct);
			break;
		case UGO_CFG_TCP_MODULE_ID:
			ret = uc_get_tcp_cfg(cfg_struct);
			break;
		case UGO_CFG_ICE_MODULE_ID:
			ret = uc_get_ice_cfg(cfg_struct);
			break;
		case ME_CTRL_CFG_MODULE_ID:
		case ME_VQE_CFG_MODULE_ID:
		case ME_RTP_CFG_MODULE_ID:
		case ME_ENV_CFG_MODULE_ID:
		case ME_VIDEO_DEC_CFG_MODULE_ID:
		case ME_VIDEO_RENDER_CFG_MODULE_ID:
		case ME_VIDEO_PROCES_CFG_MODULE_ID:
			ret =  me_get_cfg(methodID, cfg_struct);
			break;
		case ME_VIDEO_ENC_CFG_MODULE_ID:
			ret =  me_get_cfg(methodID, cfg_struct);
			uc_get_video_codecs(cfg_struct);
			break;			            

		case ME_CODECS_CFG_MODULE_ID:
			ret = uc_get_codecs(cfg_struct);
			break;
		case ME_VIDEO_EXTERN_CAPTURE_CFG_MODULE_ID:
			ret = me_get_cfg(methodID, cfg_struct);
			break;
		default:
			break;
	}
	return ret;
}

int UGo_video_start(IN int send_receive)
{
	int ret = RES_ERR;

	return uc_start_video(send_receive);
}

int UGo_video_stop(IN int send_receive)
{
	int ret = RES_ERR;
	
	ret = uc_stop_video(send_receive);
	
	return ret;
}

int UGo_video_start_record(const ME_video_record_param_t* record_param)
{
    if ( NULL == record_param )
    {
        return RES_ERR;
    }
    
	return me_video_start_record(record_param);
}

int UGo_video_stop_record(const unsigned int stream_select)
{
	return me_video_stop_record(stream_select);
}

int UGo_video_set_camera(const ME_video_camera_param_t* param)
{   
    int ret = uc_set_video_cemara( param->usWidth, param->usHeight, 
                                   param->usRotateAngle, param->ucMaxFps,
                                   param->usCameraIdx);

    return ret;
}

int UGo_video_get_camera_count(void)
{
	return me_video_get_camera_count();
}

int UGo_video_get_camera_state(ME_video_camera_param_t* camera_state)
{
    if(uc_get_process_running(PCP_SET_CEMERA_EV))
        return -1;
	return me_video_get_camera_state(camera_state);
}

int UGo_video_get_camera_info(const ucpaas_uint32 camera_idx, ME_video_camera_info_t* camera_info)
{
	return me_video_get_camera_info(camera_idx, camera_info);
}

int UGo_video_get_camera_capability(const ucpaas_uint8 camera_idx, 
                                            const ucpaas_uint8 cap_idx, 
                                            ME_video_camera_capability_t* camera_capability)
{
	return me_video_get_camera_capability(camera_idx, cap_idx, camera_capability);
}

int UGo_video_set_send_recive_rotation(const ucpaas_uint32 send_rotation, const ucpaas_uint32 recive_rotation)
{
	return me_video_set_send_recive_rotation(send_rotation, recive_rotation);
}

ucpaas_int32 UGo_video_incoming_frame(const ucpaas_uint8* data,
    const ucpaas_uint32 len)
{
    return me_video_incoming_frame(data, len);
}

ucpaas_int32 UGo_video_set_filter(const int filter_type,
                                  const ucpaas_float32 filter_sensisty)
{
    return me_video_set_filter(filter_type, filter_sensisty);
}



#ifdef AUTO_TEST_SWITCH
int UGo_start_rtpdump(const char* fileName, int direction)
{
    return me_start_RTPDump(fileName, direction);
}

int UGo_stop_rtpdump(int direction)
{
   return me_stop_RTPDump(direction); 
}

int UGo_set_testmode(int mode)
{
    return me_set_testMode(mode);
}

int UGo_get_localIP(int& port, char *ipAddr)
{
    return me_get_localIP(port, ipAddr);
}

int UGo_get_destIP(int& port, char *ipAddr)
{
    return me_get_destIP(port, ipAddr);
}

int UGo_set_encrypt_cb(ugo_encrypt_cb_t encrypt, ugo_decrypt_cb_t decrypt)
{
    g_cbtable.encrypt = encrypt;
    g_cbtable.decrypt = decrypt;
	return RES_OK;
}
#endif /* AUTO_TEST_SWITCH */

ucpaas_int32 UGo_change_ice_mode() // yangjj add
{
#ifdef ICE_ENABLED
    return uc_change_ice_mode();
#else
    return RES_ERR;
#endif
}
#ifdef IOS_SET_FILEPATH
ucpaas_int32 UGo_set_filepath(char *filepath)//add by wuzhaoyang20160407
{
	return me_set_filepath(filepath);
}
#endif
#ifdef EXTERNAL_AUDIO_DEVICE
/************************************************************
Function	: UGo_send_pcm
Description : Input media data to send to replace internal recording
Input		: see struct exl_pcm_input_itf_t
Output		: None 
Return	    : Successful return 0 , Failure returns -1
Remark      : None
Modified    : 2014/11/10    v1.0.0  Rambo.Fu
************************************************************/
int UGo_send_pcm(ME_exl_pcm_input_itf_t* Input)
{
	return me_send_pcm(Input);
}

/************************************************************
Function	: UGo_rcv_pcm
Description : Output media data to external to replace internal playing
Input		: see struct exl_pcm_output_itf_t
Output		: None 
Return	    : Successful return 0 , Failure returns -1
Remark      : None
Modified    : 2014/11/10    v1.0.0  Rambo.Fu
************************************************************/
int UGo_rcv_pcm(ME_exl_pcm_output_itf_t* Output)
{
	return me_rcv_pcm(Output);
}
#endif

