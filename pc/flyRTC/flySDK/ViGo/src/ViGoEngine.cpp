#include "cfg.h"
#include "ViGoEngine.h"
#include "conductor.h"

#define	TraceLog(A,...) WEBRTC_TRACE(A, kTraceVideo, 0, __VA_ARGS__);

static ME_cb_vtable_t g_ViEvTableCb; /*App callback function*/

namespace gl_media_engine {

#ifdef ANDROID
void ViGoEngine::set_android_obj(const media_jni_env_pm_t* pm_obj)
{
	Conductor::setAndroidObj(pm_obj);
}

void ViGoEngine::set_android_api_level(ucpaas_int32 level)
{
	Conductor::SetAndroidApi(level);
}
#endif

ViGoEngine::ViGoEngine(void)
{
	g_pVideoEngine = NULL;
}

ViGoEngine::~ViGoEngine(void)
{
	destroy();
}

///////////////// COMMON FUNCTIONS BEGIN /////////////////////
int ViGoEngine::init(int iDataSource)
{
	g_pVideoEngine = Conductor::GetInstance(iDataSource);

	if (g_pVideoEngine == NULL)
	{
		return RES_ERR;
	}

	g_pVideoEngine->CallBackVtable(&g_ViEvTableCb);

	return RES_OK;
}

int ViGoEngine::destroy(void)
{
	if (g_pVideoEngine)
	{
		Conductor::FreeInstance();
	}

	return RES_OK;
}

int ViGoEngine::callback_vtable(const ME_cb_vtable_t* pvtable)
{
	if (NULL == pvtable)
		return RES_ERR;

	memcpy(&g_ViEvTableCb, pvtable, sizeof(ME_cb_vtable_t));

	return RES_OK;
}

int ViGoEngine::set_debug_level(ucpaas_int32 iLevel)
{
	int nRet = RES_ERR;

	if (NULL != g_pVideoEngine)
	{
		nRet = g_pVideoEngine->SetLogLevel((TraceLevel)iLevel);
	}
	return(nRet);
}

int ViGoEngine::set_log_file_path(const ucpaas_int8* filepath)
{
	int nRet = RES_ERR;

	if (NULL != g_pVideoEngine)
	{
		nRet = g_pVideoEngine->SetLogFilePath(filepath);
	}

	return nRet;
}

int ViGoEngine::get_version(ucpaas_int8* vie_version)
{
	strcpy(vie_version, VIGO_VERSION);
	return RES_OK;
}

int ViGoEngine::get_state(void)
{
	if (NULL != g_pVideoEngine)
	{
		return g_pVideoEngine->GetCurState();
	}

	return eME_done;
}

void ViGoEngine::set_state(ucpaas_int32 state)
{
	if (NULL != g_pVideoEngine)
	{
		g_pVideoEngine->SetCurState(state);
	}
}

/*get configuration prms of VIGO */
int ViGoEngine::get_config(ucpaas_int32 methodID, void* cfg_struct)
{
	if (cfg_struct == NULL)
		return RES_ERR;

	if (NULL != g_pVideoEngine)
	{
		switch (methodID)
		{
		case ME_CTRL_CFG_MODULE_ID:
			memcpy(cfg_struct, &g_pVideoEngine->_stCTRLConfig, sizeof(ME_CTRL_cfg_t));
			break;
		case ME_VQE_CFG_MODULE_ID:
			memcpy(cfg_struct, &g_pVideoEngine->_stVQEConfig, sizeof(ME_VQE_cfg_t));
			break;
		case ME_RTP_CFG_MODULE_ID:
			memcpy(cfg_struct, &g_pVideoEngine->_stRTPConfig, sizeof(ME_RTP_cfg_t));
			break;
		case ME_ENV_CFG_MODULE_ID:
			memcpy(cfg_struct, &g_pVideoEngine->_stEnvConfig, sizeof(ME_ENV_cfg_t)); //Get envionment struction
			break;
		case ME_VIDEO_ENC_CFG_MODULE_ID:
			g_pVideoEngine->VideoGetParam(methodID, cfg_struct);
			break;
		case ME_VIDEO_DEC_CFG_MODULE_ID:
			g_pVideoEngine->VideoGetParam(methodID, cfg_struct);
			break;
		case ME_VIDEO_RENDER_CFG_MODULE_ID:
			g_pVideoEngine->VideoGetParam(methodID, cfg_struct);
			memcpy(cfg_struct, &g_pVideoEngine->_stVideoRender, sizeof(ME_video_render_param_t));
			break;
		case ME_VIDEO_PROCES_CFG_MODULE_ID:
			g_pVideoEngine->VideoGetParam(methodID, cfg_struct);
			memcpy(cfg_struct, &g_pVideoEngine->_stVideoProces, sizeof(ME_video_proces_module_t));
			break;
		case ME_VIDEO_EXTERN_CAPTURE_CFG_MODULE_ID:
			memcpy(cfg_struct, &g_pVideoEngine->_stVideoExtCapture, sizeof(ME_video_extern_capture_param_t));
			break;
		default:
			break;
		}
	}

	return RES_OK;
}

/*Interface of configuration for VIGO */
int ViGoEngine::set_config(ucpaas_int32 methodID, void* cfg_struct)
{
	if (cfg_struct == NULL)
	{
		return RES_ERR;
	}

	if (NULL != g_pVideoEngine)
	{
		switch (methodID)
		{
		case ME_CTRL_CFG_MODULE_ID:
			memcpy(&g_pVideoEngine->_stCTRLConfig, cfg_struct, sizeof(ME_CTRL_cfg_t));
			g_pVideoEngine->_updateCfgFlag |= CFG_UPDATE_TGO;
			g_pVideoEngine->_updateCfgFlag |= CFG_UPDATE_FEC;
			break;
		case ME_VQE_CFG_MODULE_ID:
		{
            ME_VQE_cfg_t* vqeConfig = (ME_VQE_cfg_t*)cfg_struct;
            int agcTargetDb = g_pVideoEngine->_stVQEConfig.Agc_targetDbfs;
            int agcComDb = g_pVideoEngine->_stVQEConfig.Agc_compressionGaindB;

            memcpy(&g_pVideoEngine->_stVQEConfig, cfg_struct, sizeof(ME_VQE_cfg_t));
            
            if ( (vqeConfig->Agc_targetDbfs < AGC_TARGET_DBOV_MIN) 
                || (vqeConfig->Agc_targetDbfs > AGC_TARGET_DBOV_MAX))
            {
                g_pVideoEngine->_stVQEConfig.Agc_targetDbfs = agcTargetDb;
            }
            if ( (vqeConfig->Agc_compressionGaindB < AGC_COMPRESSION_DB_MIN)
                || (vqeConfig->Agc_compressionGaindB > AGC_COMPRESSION_DB_MAX) )
            {
                g_pVideoEngine->_stVQEConfig.Agc_compressionGaindB = agcComDb;
            }
            
			g_pVideoEngine->_updateCfgFlag |= CFG_UPDATE_VOICE;
		}
			break;
		case ME_RTP_CFG_MODULE_ID:
		{
			unsigned int timeout = ((ME_RTP_cfg_t*)cfg_struct)->uiRTPTimeout;
			if (timeout < RTP_TIMEOUT_MIN)
				timeout = RTP_TIMEOUT_MIN;
			else if (timeout > RTP_TIMEOUT_MAX)
				timeout = RTP_TIMEOUT_MAX;

			memcpy(&g_pVideoEngine->_stRTPConfig, cfg_struct, sizeof(ME_RTP_cfg_t));
			g_pVideoEngine->_stRTPConfig.uiRTPTimeout = timeout;
			g_pVideoEngine->_updateCfgFlag |= CFG_UPDATE_RTP;
		}
		break;
		case ME_ENV_CFG_MODULE_ID:
			memcpy(&g_pVideoEngine->_stEnvConfig, cfg_struct, sizeof(ME_ENV_cfg_t)); //Save envionment struction
			g_pVideoEngine->_updateCfgFlag |= CFG_UPDATE_ENV;
			break;
		case ME_VIDEO_ENC_CFG_MODULE_ID:
			memcpy(&g_pVideoEngine->_stVideoEnc, cfg_struct, sizeof(ME_video_enc_param_t));
			g_pVideoEngine->VideoSetParam(methodID);
			break;
		case ME_VIDEO_DEC_CFG_MODULE_ID:
			memcpy(&g_pVideoEngine->_stVideoDec, cfg_struct, sizeof(ME_video_dec_info_t));
			g_pVideoEngine->VideoSetParam(methodID);
			break;
		case ME_VIDEO_RENDER_CFG_MODULE_ID:
		{
			ME_video_render_param_t *para = (ME_video_render_param_t*)cfg_struct;
			//memcpy(&g_pVideoEngine->_stVideoEnc, cfg_struct, sizeof(ME_video_enc_param_t));
			g_pVideoEngine->_stVideoRender.pWindowLocal = para->pWindowLocal;
			g_pVideoEngine->_stVideoRender.pWindowRemote = para->pWindowRemote;
			g_pVideoEngine->_stVideoRender.remoteHeight = para->remoteHeight;
			g_pVideoEngine->_stVideoRender.remoteWidth = para->remoteWidth;
			g_pVideoEngine->_stVideoRender.renderMode = para->renderMode;
			TraceLog(kTraceReport, "Video set render Para width  %d  height %d mode %d",
				g_pVideoEngine->_stVideoRender.remoteWidth, g_pVideoEngine->_stVideoRender.remoteHeight, para->renderMode);
#ifndef __APPLE__
			g_pVideoEngine->VideoSetParam(methodID);
#endif
		}
		break;
		case ME_VIDEO_PROCES_CFG_MODULE_ID:
			memcpy(&g_pVideoEngine->_stVideoProces, cfg_struct, sizeof(ME_video_proces_module_t));
			g_pVideoEngine->VideoSetParam(methodID);
			break;
		case ME_CODECS_CFG_MODULE_ID:
			memcpy(&g_pVideoEngine->_stCodecConfig, cfg_struct, sizeof(ME_codec_list_t));
			g_pVideoEngine->_updateCfgFlag |= CFG_UPDATE_CODEC;
			break;
		case ME_VIDEO_EXTERN_CAPTURE_CFG_MODULE_ID:
			memcpy(&g_pVideoEngine->_stVideoExtCapture, cfg_struct, sizeof(ME_video_extern_capture_param_t));
			break;
		default:
			break;
		}
	}

	return RES_OK;
}

int ViGoEngine::update_ice_mode(ucpaas_int32 mode)
{
	int nRet = RES_ERR;

	if (NULL != g_pVideoEngine)
	{
		nRet = g_pVideoEngine->UpdateIceMode(mode);
	}

	return nRet;
}

int ViGoEngine::get_ice_mode()
{
	if (NULL == g_pVideoEngine)
	{
		return RES_ERR;
	}
	return g_pVideoEngine->GetIceMode();
}

int ViGoEngine::recv_data_packet(const recv_pm_t* recv_pm)
{
	int nRet = RES_ERR;
#ifdef EXTERNAL_TRANSPORT
	if (NULL != g_pVideoEngine)
	{
		nRet = g_pVideoEngine->RecvDataPacket(recv_pm);
	}
#endif		
	return nRet;
}
///////////////// COMMON FUNCTIONS END /////////////////////
/////////////////////////////////////////////////////////////

///////////////// AUDIO FUNCTIONS BEGIN /////////////////////
/* create audio media streams */
int ViGoEngine::create_audio_stream(void)
{
	int nRet = RES_ERR;
	if (NULL != g_pVideoEngine)
	{
		nRet = g_pVideoEngine->CreateChannel();
	}

	return nRet;
}

/* delete audio media streams */
int ViGoEngine::delete_audio_stream(void)
{
	int nRet = RES_ERR;
	if (NULL != g_pVideoEngine)
	{
		nRet = g_pVideoEngine->DeleteChannel();
	}

	return nRet;
}

/*restart audio media stream*/
int ViGoEngine::restart_audio_media_stream(void)
{
	int nRet = RES_ERR;

	if (NULL != g_pVideoEngine)
	{
		nRet = g_pVideoEngine->RestartAudioStream();
	}

	return nRet;
}

/*set audio streams*/
int ViGoEngine::set_audio_stream(const ME_audio_stream_info_t *pAudioInfo)
{
	int nRet = RES_ERR;

#if defined(ANDROID) || defined (__APPLE__)
	if(get_state() == eME_done)
		init(0);// only the app has called create_voe,the webrtc can create success
#endif
	if (NULL != g_pVideoEngine)
	{
		nRet = g_pVideoEngine->SetAudioChannel(pAudioInfo);
	}

	return nRet;
}

/* Start /Stop audio playout */
int ViGoEngine::enable_audio_playout(ucpaas_int8 enable)
{
	int nRet = RES_ERR;
	if (NULL != g_pVideoEngine)
	{
		if (enable)
			nRet = g_pVideoEngine->StartPlayOut();
		else
			nRet = g_pVideoEngine->StopPlayOut();
	}
	return nRet;
}

/* Start/Stop audio send */
int ViGoEngine::enable_audio_send(ucpaas_int8 enable)
{
	int nRet = RES_ERR;
	if (NULL != g_pVideoEngine)
	{
		if (enable)
			nRet = g_pVideoEngine->StartSend();
		else
			nRet = g_pVideoEngine->StopSend();
	}
	return nRet;
}

/* Start/Stop audio receive */
int ViGoEngine::enable_audio_receive(ucpaas_int8 enable)
{
	int nRet = RES_ERR;
	if (NULL != g_pVideoEngine)
	{
		if (enable)
			nRet = g_pVideoEngine->StartReceive();
		else
			nRet = g_pVideoEngine->StopReceive();
	}
	return nRet;
}

/* enable audio fec */
int ViGoEngine::enable_audio_fec(char enable)
{
	int nRet = RES_ERR;

	if (NULL != g_pVideoEngine)
	{
		nRet = g_pVideoEngine->EnableFec(enable);
	}

	return nRet;
}

//add by wuzhaoyang20151225
int ViGoEngine::set_directcallfec_status(ucpaas_int8 enable)
{
	int nRet = RES_ERR;

	if (NULL != g_pVideoEngine)
	{
		nRet = g_pVideoEngine->SetDirectCallFecStatus(enable);
	}

	return nRet;
}
/* enable audio fec */

int ViGoEngine::get_audio_fec(void)
{
	int nRet = RES_ERR;

	if (NULL != g_pVideoEngine)
	{
		nRet = g_pVideoEngine->GetFecStatus();
	}

	return nRet;
}

int ViGoEngine::set_audio_fec(ucpaas_uint16 nMulriple, ucpaas_uint16 nOffset) //yangjj add
{
	int nRet = RES_ERR;
	if (NULL != g_pVideoEngine)
	{
		nRet = g_pVideoEngine->SetAudioFec(nMulriple, nOffset);
	}

	return nRet;
}

int ViGoEngine::set_net_analyze(bool isAnalyze)
{
	int nRet = RES_ERR;
	if (NULL != g_pVideoEngine )
	{
		nRet = g_pVideoEngine->SetNetAnalyze(isAnalyze);
	}

	return nRet;
}

int ViGoEngine::record_start(const media_file_record_pm_t* pm)
{
	int nRet = RES_ERR;

	if (NULL != g_pVideoEngine)
	{
		nRet = g_pVideoEngine->StartRecord(pm);
	}

	return nRet;
}

int ViGoEngine::record_stop(void)
{
	int nRet = RES_ERR;

	if (NULL != g_pVideoEngine)
	{
		nRet = g_pVideoEngine->StopRecord();
	}

	return nRet;
}

int ViGoEngine::music_is_started(void)
{
	int nRet = 0;

	if (NULL != g_pVideoEngine)
	{
		nRet = g_pVideoEngine->SoundIsStarted();
	}

	return nRet;
}

int ViGoEngine::send_DTMF(ucpaas_int8 c_dtmf)
{
	int nRet = RES_ERR;

	if (NULL != g_pVideoEngine)
	{
		nRet = g_pVideoEngine->SendDTMF(c_dtmf);
	}
	return nRet;
}

/* Add by Vinton.Liu on 20151123 for play dtmf tone to local */
int ViGoEngine::play_DTMF(ucpaas_int8 c_dtmf)
{
    int nRet = RES_ERR;

	if (NULL != g_pVideoEngine)
	{
		nRet = g_pVideoEngine->PlayDTMF(c_dtmf);
	}
	return nRet;
}
/* Vinton.Liu add end */

int ViGoEngine::set_mic_mute(ucpaas_int8 enabled)
{
	int nRet = RES_ERR;

	if (NULL != g_pVideoEngine)
	{
		nRet = g_pVideoEngine->SetMicrophoneMute(enabled ? 1 : 0);
	}

	return nRet;
}

int ViGoEngine::get_mic_mute()
{
	int nRet = 0;
	if (NULL != g_pVideoEngine)
	{
		nRet = g_pVideoEngine->GetMicrophoneMute();
	}

	return nRet;
}

int ViGoEngine::set_loudspeaker_status(ucpaas_int8 enabled)
{
	int nRet = RES_ERR;

	if (NULL != g_pVideoEngine)
	{
		nRet = g_pVideoEngine->SetLoudSpeakerStatus(enabled ? 1 : 0);
	}

	return nRet;
}

int ViGoEngine::get_loudspeaker_status()
{
	int nRet = 0;
	if (NULL != g_pVideoEngine)
	{
		nRet = g_pVideoEngine->GetLoudSpeakerStatus();
	}

	return nRet;
}

int ViGoEngine::codec_supported(const codec_support_pm_t* pm)
{
	int nRet = RES_ERR;
	int pt = 0;

	if (NULL == pm)
		return RES_ERR;

	eME_codecs_check_method e_method = (eME_codecs_check_method)pm->e_method;
	if (NULL != g_pVideoEngine)
	{
		if (e_method == eME_codecs_mime_method)
		{
			nRet = g_pVideoEngine->webrtcCodecSupport(pm->codecs_str);
		}
		else
		{
			pt = atoi(pm->codecs_str);
			nRet = g_pVideoEngine->webrtcCodecSupport(pt);
		}
	}

	return nRet;
}

int ViGoEngine::get_codecs(ME_codec_list_t* codecs_list)
{
	if (NULL != g_pVideoEngine)
	{
		return g_pVideoEngine->getCodecs(codecs_list);
	}
	return -1;
}

int ViGoEngine::get_emodel_calc_value(ME_emodel_info_t* evalue)
{
	int nRet = RES_ERR;

#ifdef EMODEL_ENABLED
	if (NULL != g_pVideoEngine)
	{
		nRet = g_pVideoEngine->GetEmodelValue(evalue);
	}
#endif

	return nRet;
}

int ViGoEngine::get_rtt(void)
{
	int nRet = RES_ERR;

	if (NULL != g_pVideoEngine)
	{
		nRet = g_pVideoEngine->GetRTT();
	}

	return nRet;
}

int ViGoEngine::set_audio_device(const audio_device_pm_t* pm)
{
	int nRet = RES_ERR;

	if (NULL != g_pVideoEngine)
	{
		nRet = g_pVideoEngine->SetAudioDevice(pm);
	}

	return nRet;
}

int ViGoEngine::set_speaker_volume(ucpaas_uint32 volume)
{
	int nRet = RES_ERR;

	if (NULL != g_pVideoEngine)
	{
		nRet = g_pVideoEngine->SetSpeakerVolume(volume);
	}

	return nRet;
}

int ViGoEngine::set_mic_volume(ucpaas_uint32 volume)
{
	int nRet = RES_ERR;

	if (NULL != g_pVideoEngine)
	{
		nRet = g_pVideoEngine->SetMicVolume(volume);
	}

	return nRet;
}

int ViGoEngine::play_file(ucpaas_int32 mode, const media_file_play_pm_t* pm)
{
	int nRet = RES_ERR;

	if (NULL == pm)
		return -1;

	if (NULL != g_pVideoEngine)
	{
		nRet = g_pVideoEngine->PlayFile(mode, pm);
	}

	return nRet;
}

int ViGoEngine::stop_file(void)
{
	int nRet = RES_ERR;

	if (NULL != g_pVideoEngine)
	{
		nRet = g_pVideoEngine->StopFile();
	}

	return nRet;
}

#ifdef IOS_SET_FILEPATH
int ViGoEngine::set_filepath(char *filepath)//add by wuzhaoyang20160407
{
	int nRet = RES_ERR;


	if (NULL != g_pVideoEngine)
	{
		nRet = g_pVideoEngine->set_filepath(filepath);
	}

	return nRet;
}
#endif

#ifdef EXTERNAL_AUDIO_DEVICE
int ViGoEngine::exl_send_pcm(ME_exl_pcm_input_itf_t* stInput)
{
	int nRet = RES_ERR;

	if (NULL != g_pVideoEngine)
	{
		nRet = g_pVideoEngine->exl_send_pcm(stInput);
	}

	return nRet;
}

int ViGoEngine::exl_rcv_pcm(ME_exl_pcm_output_itf_t* stOutput)
{
	int nRet = RES_ERR;

	if (NULL != g_pVideoEngine )
	{
		nRet = g_pVideoEngine->exl_rcv_pcm(stOutput);
	}

	return nRet;
} 
#endif

/* add by Vinton.Liu on 2016-03-16 for Detect recording devices status begin */
/* Recording device status */
int ViGoEngine::get_recording_device_status()
{
    int nRet = 0;
    if ( NULL != g_pVideoEngine )
    {
        nRet = g_pVideoEngine->GetRecordingDeviceStatus();
    }

    return nRet;
}
/* Vinton.Liu add end */
///////////////// AUDIO FUNCTIONS END /////////////////////
///////////////////////////////////////////////////////////

///////////////// VIDEO FUNCTIONS BEGIN /////////////////////
int ViGoEngine::screenshot_start(int islocal, ScreenType type)
{
	int nRet = RES_ERR;

	if (NULL != g_pVideoEngine)
	{
		nRet = g_pVideoEngine->StartScreenShot(islocal, type);
	}

	return nRet;
}

int ViGoEngine::video_set_send_recive_rotation(ucpaas_uint32 send_rotation, ucpaas_uint32 recive_rotation)
{
	int nRet = RES_ERR;

	if (NULL != g_pVideoEngine)
	{
		nRet = g_pVideoEngine->VideoSetSendReciveRotation(send_rotation, recive_rotation);
	}

	return nRet;
}

int ViGoEngine::video_incoming_frame(const ucpaas_uint8* data, const ucpaas_uint32 len)
{
    int nRet = RES_ERR;

    if (NULL != g_pVideoEngine)
    {
        nRet = g_pVideoEngine->VideoIncomingFrame(data, len);
    }

    return nRet;
}
    
int ViGoEngine::video_set_filter(const int filter_type, const ucpaas_float32 filter_sensity)
{
    int nRet = RES_ERR;
        
    if (NULL != g_pVideoEngine)
    {
        nRet = g_pVideoEngine->VideoSetFilter(filter_type, filter_sensity);
    }
        
    return nRet;
}
    

int ViGoEngine::video_preset(ucpaas_int8* key, ucpaas_int8 * value)
{
	int nRet = RES_ERR;

	if (NULL != g_pVideoEngine)
	{
		nRet = g_pVideoEngine->VideoPreset(key, value);
	}

	return nRet;
}

/* Set, return 0 if succeed, else -1 */
int ViGoEngine::video_set_stream(const ME_video_stream_info_t* pVideoInfo)
{
	int nRet = RES_ERR;
	if (NULL != g_pVideoEngine)
	{
		nRet = g_pVideoEngine->VideoSetStream(pVideoInfo);
	}

	return nRet;
}

/* Create video stream, return 0 if succeed, else -1 */
int ViGoEngine::video_create_stream(void)
{
	int nRet = RES_ERR;
	if (NULL != g_pVideoEngine)
	{
		nRet = g_pVideoEngine->VideoCreateStream();
	}

	return nRet;
}

/* Delete video stream, return 0 if succeed, else -1 */
int ViGoEngine::video_delete_stream(void)
{
	int nRet = RES_ERR;
	if (NULL != g_pVideoEngine)
	{
		nRet = g_pVideoEngine->VideoDeleteStream();
	}

	return nRet;
}

int ViGoEngine::video_start_record(const ME_video_record_param_t* record_param)
{
	int nRet = RES_ERR;
	if (NULL != g_pVideoEngine)
	{
		nRet = g_pVideoEngine->VideoStartRecord(record_param);
	}

	return nRet;
}

int ViGoEngine::video_stop_record(const ucpaas_uint32 stream_select)
{
	int nRet = RES_ERR;
	if (NULL != g_pVideoEngine)
	{
		nRet = g_pVideoEngine->VideoStopRecord(stream_select);
	}

	return nRet;
}

/* Start video, interface also for user, return 0 if succeed */
int ViGoEngine::video_start(const ucpaas_uint32 type)
{
	int nRet = RES_ERR;
	if (NULL != g_pVideoEngine)
	{
		nRet = g_pVideoEngine->VideoStart(type);
	}

	return nRet;
}

/* Stop video, interface also for user, return 0 if succeed */
int ViGoEngine::video_stop(const ucpaas_uint32 type)
{
	int nRet = RES_ERR;
	if (NULL != g_pVideoEngine)
	{
		nRet = g_pVideoEngine->VideoStop(type);
	}

	return nRet;
}

/* Set video device, return 0 if succeed, else -1 */
int ViGoEngine::video_set_camera(const ME_video_camera_param_t* param)
{
	int nRet = RES_ERR;
	if (NULL != g_pVideoEngine)
	{
		nRet = g_pVideoEngine->VideoSetCamera(param);
	}

	return nRet;
}

/* Get camera count, return value = camera count */
int ViGoEngine::video_get_camera_count(void)
{
	int nRet = RES_ERR;
	if (NULL != g_pVideoEngine)
	{
		nRet = g_pVideoEngine->VideoGetCameraCount();
	}

	return nRet;
}

/* Get camera state which was used currently */
int ViGoEngine::video_get_camera_state(ME_video_camera_param_t* camera_state)
{
	int nRet = RES_ERR;
	if (NULL != g_pVideoEngine)
	{
		nRet = g_pVideoEngine->VideoGetCameraState(camera_state);
	}

	return nRet;
}

/* Get camera info */
int ViGoEngine::video_get_camera_info(const ucpaas_uint8 camera_idx, ME_video_camera_info_t* camera_info)
{
	int nRet = RES_ERR;
	if (NULL != g_pVideoEngine)
	{
		nRet = g_pVideoEngine->VideoGetCameraInfo(camera_idx, camera_info);
	}

	return nRet;
}

/* Get camera Capability */
int ViGoEngine::video_get_camera_capability(const ucpaas_uint8 camera_idx, const ucpaas_uint8 cap_idx, ME_video_camera_capability_t* camera_capability)
{
	int nRet = RES_ERR;
	if (NULL != g_pVideoEngine)
	{
		nRet = g_pVideoEngine->VideoGetCameraCapability(camera_idx, cap_idx, camera_capability);
	}

	return nRet;
}

int ViGoEngine::video_set_remote_dec_resolution(ucpaas_uint32 res)
{
	int nRet = RES_ERR;
	if (NULL != g_pVideoEngine)
	{
		nRet = g_pVideoEngine->VideoSetRemoteReslution((int)res);
	}
	return nRet;
}

int ViGoEngine::video_set_remote_camera_resolution(ucpaas_uint32 width, ucpaas_uint32 height)
{
	int nRet = RES_ERR;
	if (NULL != g_pVideoEngine)
	{
		nRet = g_pVideoEngine->VideoSetRemoteCameraReslution(width, height);
	}
	return nRet;
}

int ViGoEngine::video_set_remote_fps(ucpaas_uint8 fps)
{
	int nRet = RES_ERR;
	if (NULL != g_pVideoEngine)
	{
		nRet = g_pVideoEngine->VideoSetRemoteFps(fps);
	}
	return nRet;
}

int ViGoEngine::video_set_remote_bitrate(ucpaas_uint32 start_bitrate, ucpaas_uint32 min_bitrate, ucpaas_uint32 max_bitrate)
{
	int nRet = RES_ERR;
	if (NULL != g_pVideoEngine)
	{
		nRet = g_pVideoEngine->VideoSetRemoteBitRate(start_bitrate, min_bitrate, max_bitrate);
	}
	return nRet;
}

/* Set video rotation angle via rtp channel or not */
int ViGoEngine::video_set_media_signal_status(int status)
{
    int nRet = RES_ERR;

	if (NULL != g_pVideoEngine)
	{
		nRet = g_pVideoEngine->VideoSetMediaSignalStatus(status);
	}

	return nRet;
}

int ViGoEngine::video_send_state_packets(int type)
{
    int nRet = RES_ERR;

	if (NULL != g_pVideoEngine)
	{
		nRet = g_pVideoEngine->VideoSendStatePackets(type);
	}

	return nRet;
}

/* Set Video remote send rotation */
int ViGoEngine::video_set_remote_send_rotation(const int angle)
{
    int nRet = RES_ERR;
	if (NULL != g_pVideoEngine)
	{
		nRet = g_pVideoEngine->VideoSetRemoteSendRotation(angle);
	}    
	return nRet;
}

/* Get Video Send Rotation and State */
int ViGoEngine::video_get_sendrotation_state(int& angle, int& state)
{
    int nRet = RES_ERR;
	if (NULL != g_pVideoEngine)
	{
		nRet = g_pVideoEngine->VideoGetSendRotationAndState(angle, state);
	}    
	return nRet;
}
///////////////// VIDEO FUNCTIONS END /////////////////////
///////////////////////////////////////////////////////////

///////////////// AUTO TEST FUNCTIONS BEGIN ///////////////

#ifdef AUTO_TEST_SWITCH
int ViGoEngine::set_test_mode(ucpaas_int32 iMode)
{
	int nRet = RES_ERR;

	if (NULL != g_pVideoEngine)
	{
		nRet = g_pVideoEngine->SetTestMode(iMode);
	}

	return nRet;
}

int ViGoEngine::start_RTPDump(const char fileNameUTF8[1024], int direction)
{
	int nRet = RES_ERR;

	if (NULL != g_pVideoEngine)
	{
		nRet = g_pVideoEngine->StartRTPDump(fileNameUTF8, direction);
	}

	return nRet;
}

int ViGoEngine::stop_RTPDump(int direction)
{
	int nRet = RES_ERR;

	if (NULL != g_pVideoEngine)
	{
		nRet = g_pVideoEngine->StopRTPDump(direction);
	}

	return nRet;
}

int ViGoEngine::get_localIP(int& port, char ipAddr[64])
{
	int nRet = RES_ERR;

	if (NULL != g_pVideoEngine)
	{
		nRet = g_pVideoEngine->getLocalIP(port, ipAddr);
	}

	return nRet;
}

int ViGoEngine::get_destIP(int& port, char ipAddr[64])
{
	int nRet = RES_ERR;

	if (NULL != g_pVideoEngine)
	{
		nRet = g_pVideoEngine->getDestIP(port, ipAddr);
	}

	return nRet;
}
#endif
///////////////// AUTO TEST FUNCTIONS END /////////////////
///////////////////////////////////////////////////////////
}

