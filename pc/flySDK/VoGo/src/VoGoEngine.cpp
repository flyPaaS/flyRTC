
#include "../inc/conductor.h"
#include "VoGoEngine.h"

static ME_cb_vtable_t g_VoEvTableCb; /*App callbacks vtable*/

namespace gl_media_engine {

#ifdef ANDROID
#define TraceLog(A,...)	__android_log_print(ANDROID_LOG_ERROR, "VoGoEngine", __VA_ARGS__);
#else
#define	TraceLog(A,...) WEBRTC_TRACE(A, kTraceVoice, 0, __VA_ARGS__);
#endif

///////////////// COMMON FUNCTIONS BEGIN /////////////////////
#ifdef ANDROID
void VoGoEngine::set_android_obj(const media_jni_env_pm_t* pm_obj)
{
	  Conductor::setAndroidObj(pm_obj);
}

void VoGoEngine::set_android_api_level(int level)
{
	Conductor::SetAndroidApi(level);
}
#endif

VoGoEngine::VoGoEngine(void)
{
	g_pVoiceEngine = NULL;
}

VoGoEngine::~VoGoEngine(void)
{
	destroy();
}

int VoGoEngine::init(int iDataSource)
{
	g_pVoiceEngine = Conductor::GetInstance(iDataSource);
	
	if(g_pVoiceEngine == NULL)
	{
            TraceLog(kTraceError, "init: g_pVoiceEngine is NULL !!\r\n");
            return RES_ERR;
	}	

	g_pVoiceEngine->CallBackVtable(&g_VoEvTableCb);
	
	return RES_OK;
}

int VoGoEngine::destroy(void)
{
	if (g_pVoiceEngine)
	{
		Conductor::FreeInstance();
	}

	return RES_OK;
}

int VoGoEngine::callback_vtable(const ME_cb_vtable_t* pvtable)
{
	if (NULL == pvtable)
		return RES_ERR;
	
	memcpy(&g_VoEvTableCb, pvtable, sizeof(ME_cb_vtable_t));

	return RES_OK;
}

int VoGoEngine::set_debug_level(ucpaas_int32 iLevel)
{
	int nRet = RES_ERR;

	if (NULL != g_pVoiceEngine)
	{
		nRet = g_pVoiceEngine->SetLogLevel((TraceLevel)iLevel);
	}
	return(nRet);
}

int VoGoEngine::get_version(ucpaas_int8 * voe_version)
{
	strcpy(voe_version, VOGO_VERSION);
	return RES_OK;
}

int VoGoEngine::get_state(void)
{
	int nRet = eME_done;
	
	if (NULL != g_pVoiceEngine)
	{
		nRet =  g_pVoiceEngine->GetCurState();
	}

	return nRet;
}

void VoGoEngine::set_state(ucpaas_int32 state)
{
	if (NULL != g_pVoiceEngine)
	{
		g_pVoiceEngine->SetCurState(state);
	}
}

int VoGoEngine::set_log_file_path(const ucpaas_int8* filepath)
{
	int nRet = RES_ERR;
	
	if (NULL != g_pVoiceEngine)
	{
	 	nRet = g_pVoiceEngine->SetLogFilePath(filepath);
	}

	return nRet;
}

/*get configuration prms of VOGO */
int VoGoEngine::get_config(ucpaas_int32 methodID, void* cfg_struct)
{
	if (cfg_struct == NULL)
		return RES_ERR;
	
	if (NULL != g_pVoiceEngine)
	{
		switch(methodID)
		{
			case ME_CTRL_CFG_MODULE_ID:
				memcpy(cfg_struct, &g_pVoiceEngine->_stCTRLConfig, sizeof(ME_CTRL_cfg_t));
				break;
			case ME_VQE_CFG_MODULE_ID:
				//TraceLog(kTraceError, "vqe get confit:dualMicNsEnalbe=%d\n", g_pVoiceEngine->_stVQEConfig.Dual_Mic_Ns_enable);
				memcpy(cfg_struct, &g_pVoiceEngine->_stVQEConfig, sizeof(ME_VQE_cfg_t));
				break;
			case ME_RTP_CFG_MODULE_ID:
				memcpy(cfg_struct, &g_pVoiceEngine->_stRTPConfig, sizeof(ME_RTP_cfg_t));
				break;
			case ME_ENV_CFG_MODULE_ID:
                memcpy(cfg_struct, &g_pVoiceEngine->_stEnvConfig, sizeof(ME_ENV_cfg_t)); //Get envionment struction
				break;
			default:
				break;
		}
	}
	
	return RES_OK;
}

/*Interface of configuration for VOGO */
int VoGoEngine::set_config(ucpaas_int32 methodID, void* cfg_struct)
{
    if ( NULL == cfg_struct )
    {
        return RES_ERR;
    }
    
	if (NULL != g_pVoiceEngine)
	{
		switch(methodID)
		{
			case ME_CTRL_CFG_MODULE_ID:
				memcpy(&g_pVoiceEngine->_stCTRLConfig, cfg_struct, sizeof(ME_CTRL_cfg_t));
				g_pVoiceEngine->_updateCfgFlag |= CFG_UPDATE_TGO;
				g_pVoiceEngine->_updateCfgFlag |= CFG_UPDATE_FEC;
				break;
                
			case ME_VQE_CFG_MODULE_ID:
			{
                ME_VQE_cfg_t* vqeConfig = (ME_VQE_cfg_t*)cfg_struct;
                int agcTargetDb = g_pVoiceEngine->_stVQEConfig.Agc_targetDbfs;
                int agcComDb = g_pVoiceEngine->_stVQEConfig.Agc_compressionGaindB;

                memcpy(&g_pVoiceEngine->_stVQEConfig, cfg_struct, sizeof(ME_VQE_cfg_t));
				//TraceLog(kTraceError, "vqe set confit:dualMicNsEnalbe=%d\n", g_pVoiceEngine->_stVQEConfig.Dual_Mic_Ns_enable);
                
                if ( (vqeConfig->Agc_targetDbfs < AGC_TARGET_DBOV_MIN) 
                    || (vqeConfig->Agc_targetDbfs > AGC_TARGET_DBOV_MAX))
                {
                    g_pVoiceEngine->_stVQEConfig.Agc_targetDbfs = agcTargetDb;
                }
                if ( (vqeConfig->Agc_compressionGaindB < AGC_COMPRESSION_DB_MIN)
                    || (vqeConfig->Agc_compressionGaindB > AGC_COMPRESSION_DB_MAX) )
                {
                    g_pVoiceEngine->_stVQEConfig.Agc_compressionGaindB = agcComDb;
                }
                
    			g_pVoiceEngine->_updateCfgFlag |= CFG_UPDATE_VOICE;
    		}
				break;
                
			case ME_RTP_CFG_MODULE_ID:
			{
				unsigned int timeout = ((ME_RTP_cfg_t*)cfg_struct)->uiRTPTimeout;
				if (timeout < RTP_TIMEOUT_MIN)
					timeout = RTP_TIMEOUT_MIN;
				else if (timeout > RTP_TIMEOUT_MAX)
					timeout = RTP_TIMEOUT_MAX;
			
				memcpy(&g_pVoiceEngine->_stRTPConfig, cfg_struct, sizeof(ME_RTP_cfg_t));
				g_pVoiceEngine->_stRTPConfig.uiRTPTimeout = timeout;
				g_pVoiceEngine->_updateCfgFlag |= CFG_UPDATE_RTP;
			}
				break;
                
			case ME_ENV_CFG_MODULE_ID:
                memcpy(&g_pVoiceEngine->_stEnvConfig, cfg_struct, sizeof(ME_ENV_cfg_t)); //Save envionment struction
				g_pVoiceEngine->_updateCfgFlag |= CFG_UPDATE_ENV;
				break;
			case ME_CODECS_CFG_MODULE_ID:
				memcpy(&g_pVoiceEngine->_stCodecConfig, cfg_struct, sizeof(ME_codec_list_t));
				g_pVoiceEngine->_updateCfgFlag |= CFG_UPDATE_CODEC;
				break;
			default:
				break;
		}
	}
	
	return RES_OK;
}

int VoGoEngine::update_ice_mode(ucpaas_int32 mode)
{
	int nRet = RES_ERR;
	
	if (NULL != g_pVoiceEngine)
	{
		nRet = g_pVoiceEngine->UpdateIceMode(mode);
	}

	return nRet;
}

int VoGoEngine::get_ice_mode()
{
    if ( NULL == g_pVoiceEngine )
    {
        return RES_ERR;
    }
    return g_pVoiceEngine->GetIceMode();
}

int VoGoEngine::recv_data_packet(const recv_pm_t* recv_pm)
{
	int nRet = RES_ERR;

#ifdef EXTERNAL_TRANSPORT
	if (NULL != g_pVoiceEngine)
	{
	 	nRet = g_pVoiceEngine->RecvDataPacket(recv_pm);
	}
#endif		
	return nRet;
}
///////////////// COMMON FUNCTIONS END /////////////////////
/////////////////////////////////////////////////////////////

///////////////// AUDIO FUNCTIONS BEGIN /////////////////////
/*set audio streams*/
int VoGoEngine::set_audio_stream(const ME_audio_stream_info_t *pAudioInfo)
{
	int nRet = RES_ERR;

#if defined(ANDROID) || defined (__APPLE__)
	if(get_state() == eME_done)
		init(0);// only the app has called create_voe,the webrtc can create success
#endif
	if(NULL != g_pVoiceEngine)
	{
		nRet = g_pVoiceEngine->SetAudioChannel(pAudioInfo);
	}

	return nRet;
}

/* create audio media streams */
int VoGoEngine::create_audio_stream(void)
{
    int nRet = RES_ERR;

    if ( NULL == g_pVoiceEngine )
    {
        TraceLog(kTraceError, "create_audio_stream: g_pVoiceEngine is NULL !!\r\n");
    }
    else
    {
        nRet = g_pVoiceEngine->CreateChannel();
    }
    
    return nRet;
}

/* delete audio media streams */
int VoGoEngine::delete_audio_stream(void)
{
	int nRet = RES_ERR;
	
	if(NULL != g_pVoiceEngine)
	{
		nRet = g_pVoiceEngine->DeleteChannel();
	}
	
	return nRet;
}

/*restart audio media stream*/
int VoGoEngine::restart_audio_media_stream(void)
{
	int nRet = RES_ERR;
	
	if (NULL != g_pVoiceEngine)
	{
		nRet = g_pVoiceEngine->RestartAudioStream();
	}

	return nRet;
}

/* Start /Stop audio playout */
int VoGoEngine::enable_audio_playout(ucpaas_int8 enable)
{
	int nRet = RES_ERR;
	
	if (NULL != g_pVoiceEngine)
	{
		if (enable)
			nRet = g_pVoiceEngine->StartPlayOut();
		else
			nRet = g_pVoiceEngine->StopPlayOut();
	}

	return nRet;
}

/* Start/Stop audio send */
int VoGoEngine::enable_audio_send(ucpaas_int8 enable)
{
	int nRet = RES_ERR;
	
	if (NULL != g_pVoiceEngine)
	{
		if (enable)
			nRet = g_pVoiceEngine->StartSend();
		else
			nRet = g_pVoiceEngine->StopSend();
	}

	return nRet;
}

/* Start/Stop audio receive */
int VoGoEngine::enable_audio_receive(ucpaas_int8 enable)
{
	int nRet = RES_ERR;
	
	if (NULL != g_pVoiceEngine)
	{
		if (enable)
			nRet = g_pVoiceEngine->StartReceive();
		else
			nRet = g_pVoiceEngine->StopReceive();
	}

	return RES_OK;
}

/* enable audio fec */
int VoGoEngine::enable_audio_fec(ucpaas_int8 enable)
{
	int nRet = RES_ERR;

	if (NULL != g_pVoiceEngine)
	{
		//if (enable)
		nRet = g_pVoiceEngine->EnableFec(enable);		
	}

	return nRet;
}

//add by wuzhaoyang20151225
int VoGoEngine::set_directcallfec_status(ucpaas_int8 enable)
{
	int nRet = RES_ERR;

	if (NULL != g_pVoiceEngine)
	{
		nRet = g_pVoiceEngine->SetDirectCallFecStatus(enable);
	}

	return nRet;
}

/* enable audio fec */
int VoGoEngine::get_audio_fec(void)
{
	int nRet = RES_ERR;

	if (NULL != g_pVoiceEngine)
	{
		nRet = g_pVoiceEngine->GetFecStatus();		
	}

	return nRet;
}

int VoGoEngine::set_audio_fec(ucpaas_uint16 nMulriple, ucpaas_uint16 nOffset) //yangjj add
{
	int nRet = RES_ERR;
	if (NULL != g_pVoiceEngine)
	{
		nRet = g_pVoiceEngine->SetAudioFec(nMulriple, nOffset);
	}

	return nRet;
}

int VoGoEngine::set_net_analyze(bool isAnalyze)
{
	int nRet = RES_ERR;
	if (NULL != g_pVoiceEngine)
	{
		nRet = g_pVoiceEngine->SetNetAnalyze(isAnalyze);
	}

	return nRet;
}

int VoGoEngine::record_start(const media_file_record_pm_t* pm)
{
	int nRet = RES_ERR;

	if (NULL != g_pVoiceEngine)
	{
	 	nRet = g_pVoiceEngine->StartRecord(pm);
	}
		
	return nRet;
}

int VoGoEngine::record_stop(void)
{
	int nRet = RES_ERR;

	if (NULL != g_pVoiceEngine)
	{
		nRet = g_pVoiceEngine->StopRecord();
	}

	return nRet;
}

int VoGoEngine::music_is_started(void)
{
	int nRet = RES_ERR;
	
	if (NULL != g_pVoiceEngine)
	{
		nRet = g_pVoiceEngine->SoundIsStarted();
	}

	return nRet;
}

int VoGoEngine::send_DTMF(ucpaas_int8 c_dtmf)
{
	int nRet = RES_ERR;
	
	if (NULL != g_pVoiceEngine)
	{
		nRet = g_pVoiceEngine->SendDTMF(c_dtmf);
	}
	return nRet;
}

/* Add by Vinton.Liu on 20151123 for play dtmf tone to local */
int VoGoEngine::play_DTMF(ucpaas_int8 c_dtmf)
{
    int nRet = RES_ERR;

	if (NULL != g_pVoiceEngine)
	{
		nRet = g_pVoiceEngine->PlayDTMF(c_dtmf);
	}
	return nRet;
}
/* Vinton.Liu add end */

int VoGoEngine::set_mic_mute(ucpaas_int8 enabled)
{
	int nRet = RES_ERR;

	if (NULL != g_pVoiceEngine)
	{
		nRet = g_pVoiceEngine->SetMicrophoneMute(enabled?1:0);
	}

	return nRet;
}

int VoGoEngine::get_mic_mute ()
{
    int nRet = 0;
    if ( NULL != g_pVoiceEngine )
    {
        nRet = g_pVoiceEngine->GetMicrophoneMute();
    }

    return nRet;
}

int VoGoEngine::set_loudspeaker_status(ucpaas_int8 enabled)
{
	int nRet = RES_ERR;

	if (NULL != g_pVoiceEngine)
	{
		nRet = g_pVoiceEngine->SetLoudSpeakerStatus(enabled?1:0);
	}

	return nRet;
}

int VoGoEngine::get_loudspeaker_status()
{
    int nRet = 0;
    if ( NULL != g_pVoiceEngine )
    {
        nRet = g_pVoiceEngine->GetLoudSpeakerStatus();
    }

    return nRet;
}

int VoGoEngine::get_emodel_calc_value(ME_emodel_info_t* evalue)
{
	int nRet = RES_ERR;
	
#ifdef EMODEL_ENABLED	
	if (NULL != g_pVoiceEngine)
	{	
		nRet = g_pVoiceEngine->GetEmodelValue(evalue);
	}
#endif

	return nRet;
}
int VoGoEngine::get_rtt(void)
{
	int nRet = RES_ERR;
	
	if (NULL != g_pVoiceEngine)
	{
		nRet = g_pVoiceEngine->GetRTT();
	}

	return nRet;
}

int VoGoEngine::codec_supported(const codec_support_pm_t* pm)
{
	int nRet = RES_ERR;
	int pt = 0;

	if (NULL == pm)
		return RES_ERR;

	eME_codecs_check_method e_method = (eME_codecs_check_method)pm->e_method;
	if (NULL != g_pVoiceEngine)
	{
		if (e_method == eME_codecs_mime_method)
		{
			 nRet = g_pVoiceEngine->webrtcCodecSupport(pm->codecs_str);
		}
		else
		{
			pt = atoi(pm->codecs_str);
			nRet =  g_pVoiceEngine->webrtcCodecSupport(pt);
		}
	}

	return nRet;
}

int VoGoEngine::get_codecs(ME_codec_list_t* codecs_list)
{
    if (NULL != g_pVoiceEngine)
    {
        return g_pVoiceEngine->getCodecs(codecs_list);
    }
	return -1;
}

int VoGoEngine::set_audio_device(const audio_device_pm_t* pm)
{
	int nRet = RES_ERR;
    
	if (NULL != g_pVoiceEngine)
	{
		nRet = g_pVoiceEngine->SetAudioDevice(pm);
	}
    
	return nRet;
}

int VoGoEngine::set_speaker_volume(ucpaas_uint32 volume)
{	
	int nRet = RES_ERR;
    
	if (NULL != g_pVoiceEngine)
	{
		nRet = g_pVoiceEngine->SetSpeakerVolume(volume);
	}
    
	return nRet;
}


int VoGoEngine::set_mic_volume(ucpaas_uint32 volume)
{
	int nRet = RES_ERR;
    
	if (NULL != g_pVoiceEngine)
	{
		nRet = g_pVoiceEngine->SetMicVolume(volume);
	}
    
	return nRet;
}

int VoGoEngine::play_file(ucpaas_int32 mode, const media_file_play_pm_t* pm)
{
	int nRet = RES_ERR;

	if (NULL == pm)
		return -1;

	if (NULL != g_pVoiceEngine)
	{
#ifdef EXTERNAL_PLAYFILE
		if (mode != kME_FILE_PATHNAME)
		{
			nRet = g_pVoiceEngine->ExternalPlayFile(mode, pm);
		}
		else
#endif		
		{	
			nRet = g_pVoiceEngine->PlayFile(pm);
		}
	}
    
	return nRet;
}

int VoGoEngine::stop_file()
{
	int nRet = RES_ERR;

	if (NULL != g_pVoiceEngine)
	{
	 	nRet = g_pVoiceEngine->StopFile();
	}
		
	return nRet;
}

#ifdef IOS_SET_FILEPATH
int VoGoEngine::set_filepath(char *filepath)//add by wuzhaoyang20160407
{
	int nRet = RES_ERR;

	
	if (NULL != g_pVoiceEngine)
	{
		nRet = g_pVoiceEngine->set_filepath(filepath);
	}

	return nRet;
}
#endif

#ifdef EXTERNAL_AUDIO_DEVICE
int VoGoEngine::exl_send_pcm(ME_exl_pcm_input_itf_t* stInput)
{
	int nRet = RES_ERR;

	if (NULL != g_pVoiceEngine)
	{
	 	nRet = g_pVoiceEngine->exl_send_pcm(stInput);
	}
		
	return nRet;
}

int VoGoEngine::exl_rcv_pcm(ME_exl_pcm_output_itf_t* stOutput)
{
	int nRet = RES_ERR;

	if (NULL != g_pVoiceEngine)
	{
	 	nRet = g_pVoiceEngine->exl_rcv_pcm(stOutput);
	}
		
	return nRet;
}
#endif

/* add by Vinton.Liu on 2016-03-16 for Detect recording devices status begin */
/* Recording device status */
int VoGoEngine::get_recording_device_status()
{
    int nRet = 0;
    if ( NULL != g_pVoiceEngine )
    {
        nRet = g_pVoiceEngine->GetRecordingDeviceStatus();
    }

    return nRet;
}
/* Vinton.Liu add end */
///////////////// AUDIO FUNCTIONS END /////////////////////
///////////////////////////////////////////////////////////

///////////////// AUTO TEST FUNCTIONS BEGIN /////////////////////
#ifdef AUTO_TEST_SWITCH
int VoGoEngine::set_test_mode(ucpaas_int32 iMode)
{
	int nRet = RES_ERR;

	if (NULL != g_pVoiceEngine)
	{
	 	nRet = g_pVoiceEngine->SetTestMode(iMode);
	}
		
	return nRet;
}

int VoGoEngine::start_RTPDump(const char fileNameUTF8[1024], int direction)
{
	int nRet = RES_ERR;

	if (NULL != g_pVoiceEngine)
	{
	 	nRet = g_pVoiceEngine->StartRTPDump(fileNameUTF8, direction);
	}
		
	return nRet;
}

int VoGoEngine::stop_RTPDump(int direction)
{
	int nRet = RES_ERR;

	if (NULL != g_pVoiceEngine)
	{
	 	nRet = g_pVoiceEngine->StopRTPDump(direction);
	}
		
	return nRet;
}

int VoGoEngine::get_localIP(int& port, char ipAddr[64])
{
	int nRet = RES_ERR;

	if (NULL != g_pVoiceEngine)
	{
	 	nRet = g_pVoiceEngine->getLocalIP(port, ipAddr);
	}
		
	return nRet;
}

int VoGoEngine::get_destIP(int& port, char ipAddr[64])
{
	int nRet = RES_ERR;

	if (NULL != g_pVoiceEngine)
	{
	 	nRet = g_pVoiceEngine->getDestIP(port, ipAddr);
	}
		
	return nRet;
}

#endif
///////////////// AUTO TEST FUNCTIONS END /////////////////
///////////////////////////////////////////////////////////
}

