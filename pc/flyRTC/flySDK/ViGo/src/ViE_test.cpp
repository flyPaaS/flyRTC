#include "ViE_test.h"
#include "conductor.h"
#include "../../comm/inc/cfg.h"
/**********The interface to test or extend***************/
#ifdef VIGO_TEST_ON

int ViE_set_audio_device(unsigned int wav_dev_in, unsigned int wav_dev_out)
{
	int nRet = RES_ERR;
    
	if (NULL != g_pVideoEngine)
	{
		nRet = g_pVideoEngine->SetAudioDevice(wav_dev_in, wav_dev_out);
	}
    
	return nRet;
}

int ViE_set_speaker_volume(unsigned int volume)
{	
	int nRet = RES_ERR;
    
	if (NULL != g_pVideoEngine)
	{
		nRet = g_pVideoEngine->SetSpeakerVolume(volume);
	}
    
	return nRet;
}


int ViE_set_mic_volume(unsigned int volume)
{
	int nRet = RES_ERR;
    
	if (NULL != g_pVideoEngine)
	{
		nRet = g_pVideoEngine->SetMicVolume(volume);
	}
    
	return nRet;
}

#ifdef _WIN32
int ViE_play_file(const char* filepath, int iFileFormat = 1)
{
	int nRet = RES_ERR;
    
	if (NULL != g_pVideoEngine)
	{
	 	nRet = g_pVideoEngine->PlayFile(filepath, iFileFormat);
	}
    
	return nRet;
}

int ViE_stop_file(const char* filepath)
{
	int nRet = RES_ERR;

	if (NULL != g_pVideoEngine)
	{
	 	nRet = g_pVideoEngine->StopFile();
	}
		
	return nRet;
}


#endif

#endif

