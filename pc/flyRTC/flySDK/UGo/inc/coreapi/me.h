/******************************************************************************

  Copyright (C), 2001-2011, DCN Co., Ltd.

 ******************************************************************************
  File Name     : me.h
  Version       : Initial Draft
  Author        : gonghuojin
  Created       : 2013/10/9
  Last Modified :
  Description   : media engine api header file
  Function List :
  History       :
  1.Date        : 2013/10/9
    Author      : gonghuojin
    Modification: Created file

******************************************************************************/
#ifndef MEDIA_ENGINE_H
#define MEDIA_ENGINE_H
#include "cfg.h"
#include "MediaEngineInterface.h"

#ifndef IN
#define IN 
#endif

#ifndef OUT
#define OUT
#endif

int me_load();
int me_unload(void);
int me_is_load(void);

#if defined(ANDROID)
int me_set_android_obj(media_jni_env_pm_t* env);
#endif
int me_screenshot_start(int islocal, ScreenType type);
int me_callback_vtable(IN ME_cb_vtable_t* pvtable);

int me_init(int iDataSource);
int me_destroy(void);
int me_set_audiostream(IN ME_audio_stream_info_t* maudio);
int me_create_audiostream(void);
int me_delete_audiostream(void);
int me_enable_audio_playout(IN char enable);
int me_enable_audio_receive(IN char enable);
int me_enable_audio_send(IN char enable);

int me_get_version(OUT char * uc_version);
int me_get_state(void);
int me_set_state(int state);
int me_set_api(int apilevel);

int me_set_cfg(IN int moduleid, IN void* pcfg);
int me_get_cfg(OUT int moduleid, OUT void *pcfg);
int me_record_start(IN const media_file_record_pm_t* pm);
int me_record_stop(void);
int me_play_file(IN int mode, IN const media_file_play_pm_t* pm);
int me_stop_file(void);
int me_send_dtmf(IN char c_dtmf);
int me_play_dtmf(IN char c_dtmf);
int me_set_mic_mute(IN char enabled);
int me_get_mic_mute();
int me_set_loudspeaker_status(IN char enabled);
int me_get_loudspeaker_status();
/* add by Vinton.Liu on 2016-03-16 for Detect recording devices status begin */
int me_get_recording_device_status();
/* Vinton.Liu add end */
int me_set_log_file(IN int level, IN const char* filepath);
int me_get_codecs(OUT ME_codec_list_t* pcodecs);
int me_get_emodel(OUT ME_emodel_info_t* pemodel);
int me_is_start(void);
OUT int me_get_rtt(void);
int me_get_ice_mode();
int me_update_ice_mode(IN int mode);
int me_recv_data_packet(IN const recv_pm_t* recv_pm);

int me_enable_audio_fec(char enable);//add by wuzhaoyang20140607
int me_set_directcallfec_status(char enable);//add by wuzhaoyang20151225
int me_get_audio_fec(void);
int me_set_audio_fec(unsigned short nMulriple, unsigned short  nOffset);//yangjj add
int me_set_net_analyze(char isAnalyze);//yangjj add
int me_video_preset(IN char*key, IN char*value);



/* Set, return 0 if succeed, else -1 */
int me_video_set_stream(IN const ME_video_stream_info_t* pVideoInfo);
/* Create video stream, return 0 if succeed, else -1 */
int me_video_create_stream(void);
/* Delete video stream, return 0 if succeed, else -1 */
int me_video_delete_stream(void);
/* Start recording Video, parameter:0-output, 1-input, return 0 if succeed, else -1 */
int me_video_start_record(IN const ME_video_record_param_t* record_param);
/* Stop recording Video, parameter:0-output, 1-input, return 0 if succeed, else -1 */
int me_video_stop_record(IN const unsigned int stream_select);

/* Set Video remote send rotation */
int me_video_set_remote_sendrotation(const int angle);

/* Get Video Send Rotation and State */
int me_video_get_sendrotation_state(int& angle, int& state);

/* Set video rotation angle via rtp channel or not */
int me_video_set_media_signal_status(int status);

/* Send video angle via media channel */
int me_video_send_state_packets(int type);

/* Start video, interface also for user, return 0 if succeed */
int me_video_start(IN const unsigned int type);

/* Stop video, interface also for user, return 0 if succeed */
int me_video_stop(IN const unsigned int type);

/* Set video device, return 0 if succeed, else -1 */
int me_video_set_camera(IN const ME_video_camera_param_t* param);

/* Get camera count, return value = camera count */
int me_video_get_camera_count(void);

/* Get camera state which was used currently */
int me_video_get_camera_state(OUT ME_video_camera_param_t* camera_state);

/* Get camera info */
int me_video_get_camera_info(IN const ucpaas_uint32 camera_idx, 
                                    OUT ME_video_camera_info_t* camera_info);	

/* Get camera Capability */
int me_video_get_camera_capability(IN const ucpaas_uint8 camera_idx, 
                                        IN const ucpaas_uint8 cap_idx, 
                                        OUT ME_video_camera_capability_t* camera_capability);

int me_video_set_send_recive_rotation(IN ucpaas_uint32 send_rotation, 
                                            IN ucpaas_uint32 recive_rotation);

int me_video_incoming_frame(const ucpaas_uint8* data,
    const ucpaas_uint32 len);

int me_video_set_filter(const int filter_type,
                         const ucpaas_float32 filter_sensisty);

int me_video_set_remote_dec_resolution(ucpaas_uint32 res);

int me_video_set_remote_camera_resolution(ucpaas_uint32 width, ucpaas_uint32 height);

int me_video_set_remote_fps(ucpaas_uint8 fps);

int me_video_set_remote_bitrate( ucpaas_uint32 start_bitrate, 
                                       ucpaas_uint32 min_bitrate, 
                                       ucpaas_uint32 max_bitrate); 

#ifdef EXTERNAL_AUDIO_DEVICE
int me_send_pcm(ME_exl_pcm_input_itf_t* stInput);
int me_rcv_pcm(ME_exl_pcm_output_itf_t* stOutput);
#endif

#ifdef AUTO_TEST_SWITCH
int me_set_testMode(int iMode);
int me_start_RTPDump(const char fileNameUTF8[1024], int direction);
int  me_stop_RTPDump(int direction);	
int me_get_localIP(int& port, char ipAddr[64]);
int me_get_destIP(int& port, char ipAddr[64]);
#endif
#ifdef IOS_SET_FILEPATH
int me_set_filepath(char *filepath);//add by wuzhaoyang20160407
#endif
#endif /* MEDIA_ENGINE_H */

