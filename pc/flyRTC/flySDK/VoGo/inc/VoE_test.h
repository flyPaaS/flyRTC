#ifndef _VOE_TEST_H_
#define _VOE_TEST_H_


/************************************************************************
	The reserved interface that only used to test or extend, don't call it
************************************************************************/
int VoE_set_audio_device(unsigned int wav_dev_in, unsigned int wav_dev_out);
int VoE_set_speaker_volume(unsigned int volume);
int VoE_set_mic_volume(unsigned int volume);
int VoE_play_file(const char* filepath, int iFileFormat = 1);
int VoE_stop_file(const char* filepath);

#endif




