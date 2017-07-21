#ifndef _VIE_TEST_H_
#define _VIE_TEST_H_


/************************************************************************
	The reserved interface that only used to test or extend, don't call it
************************************************************************/
int ViE_set_audio_device(unsigned int wav_dev_in, unsigned int wav_dev_out);
int ViE_set_speaker_volume(unsigned int volume);
int ViE_set_mic_volume(unsigned int volume);
#ifdef _WIN32
int ViE_play_file(const char* filepath, int iFileFormat = 1);
int ViE_stop_file(const char* filepath);
#endif

#endif




