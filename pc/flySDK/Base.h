#pragma once

// APP版本
#define VERSION		"1.0"

// 全局
extern CString CS_URL;
extern CString CS_PORT;
extern CString AMS_URL_ADDRESS;
extern CString GET_URL_ADDRESS;
extern CString CS_URL_ADDRESS;
extern CString RTPP_URL_ADDRESS;

// 回调函数
void TUGo_CallBack(int ev_type, int ev_reason, const char* something, const void* param);
void TUGo_LogCallBack(char* summary, char* detail, enum eME_TraceLevel level);
void TUGO_Send_Msg_CB(const ucpaas_int8* msg, ucpaas_int32 slen);
void TUGo_EncryptCb(unsigned char* in_data, unsigned char* out_data, int bytes_in, int* bytes_out);
void TUGo_decryptCb(unsigned char* in_data, unsigned char* out_data, int bytes_in, int* bytes_out);
void TUGO_Screen_ShotCB(const ucpaas_uint8* dst_argb, int dst_stride, int width, int height, int islocal, ScreenType screen_type);
void TUGO_ExlMedia_ProcCB(ucpaas_int16* audio_10ms, ucpaas_int32 samples, ucpaas_int32 samplingFreqHz, ucpaas_boolean isStereo, ucpaas_int16* audio_out, ucpaas_int32* samples_out);
void TUGo_InitPlayoutCb(unsigned int sample_rate, unsigned int bytes_per_sample, unsigned int num_of_channels);
void TUGo_InitRecordingCb(unsigned int sample_rate, unsigned int bytes_per_sample, unsigned int num_of_channels);
int TUGo_ReadRecordingDataCb(signed char *audio_data, unsigned int nSize);
int TUGo_WritePlayoutDataCb(const signed char *audio_data, unsigned int nSize);

// 写BMP文件
int BmpWrite(unsigned char *image, int xsize, int ysize, int islocal, ScreenType screen_type, char *filename);