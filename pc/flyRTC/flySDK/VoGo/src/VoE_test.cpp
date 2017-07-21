#include "../../comm/inc/cfg.h"
#include "VoE_test.h"
#include "conductor.h"

/**********The interface to test or extend***************/
#ifdef VOGO_TEST_ON

int VoE_set_audio_device(unsigned int wav_dev_in, unsigned int wav_dev_out)
{
	int nRet = RES_ERR;
    
	if (NULL != g_pVoiceEngine)
	{
		nRet = g_pVoiceEngine->SetAudioDevice(wav_dev_in, wav_dev_out);
	}
    
	return nRet;
}

int VoE_set_speaker_volume(unsigned int volume)
{	
	int nRet = RES_ERR;
    
	if (NULL != g_pVoiceEngine)
	{
		nRet = g_pVoiceEngine->SetSpeakerVolume(volume);
	}
    
	return nRet;
}


int VoE_set_mic_volume(unsigned int volume)
{
	int nRet = RES_ERR;
    
	if (NULL != g_pVoiceEngine)
	{
		nRet = g_pVoiceEngine->SetMicVolume(volume);
	}
    
	return nRet;
}

#ifdef _WIN32
int VoE_play_file(const char* filepath, int iFileFormat)
{
	int nRet = RES_ERR;
    
	if (NULL != g_pVoiceEngine)
	{
	 	nRet = g_pVoiceEngine->PlayFile(filepath, iFileFormat);
	}
    
	return nRet;
}

int VoE_stop_file(const char* filepath)
{
	int nRet = RES_ERR;

	if (NULL != g_pVoiceEngine)
	{
	 	nRet = g_pVoiceEngine->StopFile();
	}
		
	return nRet;
}
#endif

#endif


