/***************************************************************************
** 版权所有:  Copyright (c) 2014-2019
** 文件名称:  MediaEngineCommon.h
** 文件标识: 
** 内容摘要:  common define for media engine
** 当前版本:  v1.0
** Author  :  Vinton.Liu
** 完成日期: 2016年4月08日
** 修改记录: 
** 修改日期:  
** 版本号: 
** 修改人:    
** 修改内容:  
***************************************************************************/
#ifndef  __MEDIA_ENGINE_COMMON_H__
#define __MEDIA_ENGINE_COMMON_H__

#include "MediaEngineInterface.h"

namespace gl_media_engine {

/*************************************************************************************
**
**								interface defintion
**   audio interface must always implement, but video not have to.
*************************************************************************************/

class MediaEngineInterface{
 public:

    ///////////////// COMMON FUNCTIONS BEGIN /////////////////////
#ifdef ANDROID
    /* Set App context object for android platform */
    virtual void set_android_obj(const media_jni_env_pm_t* pm_obj) = 0;
    virtual void set_android_api_level(ucpaas_int32 level) = 0;
#endif
    
	/* MediaEngine module init */	
    virtual int init(int iDataSource) = 0;
    
	/* MediaEngine module destroy */
    virtual int destroy(void) = 0;

	/* Get config params of MediaEngine */	
	virtual int get_config(ucpaas_int32 methodID, void* cfg_struct) = 0;

	/* Set config to MediaEngine */	
	virtual int set_config(ucpaas_int32 methodID, void* cfg_struct) = 0;

	/* App callbacks fuction register */
    virtual int callback_vtable(const ME_cb_vtable_t* pvtable) = 0;

    /* Set debug level */
    virtual int set_debug_level(ucpaas_int32 level) = 0;

	/* Set log file path */
	virtual int set_log_file_path(const ucpaas_int8* filepath) = 0;

	/* Get current voe version */
    virtual int get_version(ucpaas_int8* voe_version) = 0;
    
	/* Get current voe state */
    virtual int get_state(void) = 0;

  	/* Get current voe state */
    virtual void set_state(ucpaas_int32 state) = 0;

    /*Update ice mode*/
	virtual int update_ice_mode(ucpaas_int32 mode) = 0;

	/* Get ice mode */
	virtual int get_ice_mode() = 0;

    /* External transport  audio/video packet receiver */
    virtual int recv_data_packet(const recv_pm_t* recv_pm) = 0;
    ///////////////// COMMON FUNCTIONS END /////////////////////
    /////////////////////////////////////////////////////////////

    ///////////////// AUDIO FUNCTIONS BEGIN /////////////////////
	/*set audio streams*/
	virtual	int set_audio_stream(const ME_audio_stream_info_t *pAudioPayloadInfo) = 0;
	
	/* create audio media streams */
    virtual int create_audio_stream(void) = 0;
    
  	/* delete audio media streams */
    virtual int delete_audio_stream(void) = 0;

    /*restart audio media stream*/
	virtual int restart_audio_media_stream(void) = 0;

	/* Start /Stop audio playout */
    virtual int enable_audio_playout(ucpaas_int8 enable) = 0;

	/* Start/Stop audio send */
    virtual int enable_audio_send(ucpaas_int8 enable) = 0;

	/* Open audio fec */
	virtual int enable_audio_fec(char enable) = 0;//add by wuzhaoyang20140607
	
	virtual int set_directcallfec_status(ucpaas_int8 enable) = 0;//add by wuzhaoyang20151225

	/* Get audio fec status */
	virtual int get_audio_fec(void) = 0;//add by wuzhaoyang

	virtual int set_audio_fec(ucpaas_uint16 nMulriple, ucpaas_uint16 nOffset) = 0;//yangjj add
	
	virtual int set_net_analyze(bool isAnalyze) = 0;
	
	/* Start/Stop audio receive */
    virtual int enable_audio_receive(ucpaas_int8 enable) = 0;	
    
	/* Start record */
    virtual int record_start(const media_file_record_pm_t* pm) = 0;
    
 	/* Stop record */
    virtual int record_stop(void) = 0;

	virtual int play_file(ucpaas_int32 mode, const media_file_play_pm_t* pm) = 0;

	virtual int stop_file(void) = 0;

#ifdef IOS_SET_FILEPATH
	virtual int set_filepath(char *filepath) = 0;//add by wuzhaoyang20160407
#endif

	/* Whether music have been started */
    virtual int music_is_started(void) = 0;

    /* send DTMF */
    virtual int send_DTMF(ucpaas_int8 c_dtmf) = 0;

    /* play DTMF to local */
    virtual int play_DTMF(ucpaas_int8 c_dtmf) = 0;

	/* Mute set */
    virtual int set_mic_mute(ucpaas_int8 enabled) = 0;

    /* Get Mute */
    virtual int get_mic_mute() = 0;

    /* Speaker set */
    virtual int set_loudspeaker_status(ucpaas_int8 enabled) = 0;

    /* Speaker get */
    virtual int get_loudspeaker_status() = 0;

    /* Recording device status */
    virtual int get_recording_device_status() = 0;
	
	/*Set audio device*/
	virtual int set_audio_device(const audio_device_pm_t* pm) = 0;

	/*Set speaker volume*/
	virtual int set_speaker_volume(ucpaas_uint32 volume) = 0;

	/*Set mic volume*/
	virtual int set_mic_volume(ucpaas_uint32 volume) = 0;
  
	/* Set codec supported by check methods */
    virtual int codec_supported(const codec_support_pm_t* pm) = 0;

	/* Get current SP codecs */
    virtual int get_codecs(ME_codec_list_t* codecs_list) = 0;

	/* Get Emodel Calc values*/
	virtual int get_emodel_calc_value(ME_emodel_info_t* pemodel) = 0;

	/*Get RTT value*/
	virtual int get_rtt(void) = 0;

#ifdef EXTERNAL_AUDIO_DEVICE
    virtual int exl_send_pcm(ME_exl_pcm_input_itf_t* stInput) = 0;
    
    virtual int exl_rcv_pcm(ME_exl_pcm_output_itf_t* stOutput) = 0;
#endif

    ///////////////// AUDIO FUNCTIONS END /////////////////////
    ///////////////////////////////////////////////////////////

    ///////////////// VIDEO FUNCTIONS BEGIN /////////////////////
	/* Set video streams */
	virtual	int video_set_stream(const ME_video_stream_info_t* pVideoInfo){ return RES_OK; }
	
	/* Create video stream, return 0 if succeed, else -1 */
	virtual int video_create_stream(void){ return RES_OK; }
	
	/* Delete video stream, return 0 if succeed, else -1 */
	virtual int video_delete_stream(void){ return RES_OK; }
	
	/* Start video, return 0 if succeed */
	virtual int video_start(const ucpaas_uint32 type){ return RES_OK; }
	
	/* Stop video, return 0 if succeed */
	virtual int video_stop(const ucpaas_uint32 type){ return RES_OK; }
	
	/* Set camera device */
	virtual int video_set_camera(const ME_video_camera_param_t* camera_param){ return RES_OK; }
	
	/* Get camera count, return value = camera count */
	virtual int video_get_camera_count(void){ return RES_OK; }
	
	/* Get camera state which was used currently */
	virtual int video_get_camera_state(ME_video_camera_param_t* camera_state){ return RES_OK; }
	
	/* Get camera info */
	virtual int video_get_camera_info(const ucpaas_uint8 camera_idx, 
	                                    ME_video_camera_info_t* camera_info){ return RES_OK; }
	
	/* Get camera Capability */
	virtual int video_get_camera_capability(const ucpaas_uint8 camera_idx,
	                                            const ucpaas_uint8 cap_idx, 
	                                            ME_video_camera_capability_t* camera_capability){ return RES_OK; }
	                                            
	/* Start recording Video, parameter:0-output, 1-input, return 0 if succeed, else -1 */
	virtual int video_start_record(const ME_video_record_param_t* record_param){ return RES_OK; }
	
	/* Stop recording Video, parameter:0-output, 1-input, return 0 if succeed, else -1 */
	virtual int video_stop_record(const ucpaas_uint32 stream_select){ return RES_OK; }

	virtual int video_set_remote_dec_resolution(ucpaas_uint32 res){ return RES_OK; }
	
	virtual int video_set_remote_camera_resolution(ucpaas_uint32 width, 
	                                                        ucpaas_uint32 height){ return RES_OK; }

	/* set remote dec fps for fps negotiation*/
	virtual int video_set_remote_fps(ucpaas_uint8  fps){ return RES_OK; }

	virtual int video_set_remote_bitrate(ucpaas_uint32 start_bitrate, 
	                                        ucpaas_uint32 min_bitrate,
	                                        ucpaas_uint32 max_bitrate){ return RES_OK; }

	virtual int video_set_send_recive_rotation(ucpaas_uint32 send_rotation,
	                                                ucpaas_uint32 recive_rotation){ return RES_OK; }
	
	virtual int video_incoming_frame(const ucpaas_uint8* data, 
	                                        const ucpaas_uint32 len){ return RES_OK; }
    
    virtual int video_set_filter(const int filter_type, const ucpaas_float32 filter_sensisty){ return RES_OK; }

	virtual int screenshot_start(int islocal, ScreenType type){ return RES_OK; }

	virtual int  video_preset(ucpaas_int8*key, ucpaas_int8*value){ return RES_OK; }

	/* Set Video remote send rotation */
    virtual int video_set_remote_send_rotation(const int angle){ return RES_OK; }
    
    /* Get Video Send Rotation and State */
    virtual int video_get_sendrotation_state(int& angle, int& state){ return RES_OK; }

    /* Set video rotation angle via rtp channel or not */
	virtual int video_set_media_signal_status(int status){ return RES_OK; }

	virtual int video_send_state_packets(int type) { return RES_OK; }
    ///////////////// VIDEO FUNCTIONS END /////////////////////
    ///////////////////////////////////////////////////////////

    ///////////////// AUTO TEST FUNCTIONS BEGIN /////////////////////
#ifdef AUTO_TEST_SWITCH
	virtual int set_test_mode(ucpaas_int32 iMode){ return RES_OK; }
	virtual int start_RTPDump(const char fileNameUTF8[1024], int direction){ return RES_OK; }
	virtual int  stop_RTPDump(int direction){ return RES_OK; }
	virtual int get_localIP(int& port, char ipAddr[64]){ return RES_OK; }
	virtual int get_destIP(int& port, char ipAddr[64]){ return RES_OK; }
#endif
    ///////////////// AUTO TEST FUNCTIONS END /////////////////
    ///////////////////////////////////////////////////////////

public:	
	MediaEngineInterface(){}
    virtual ~MediaEngineInterface(){}/*this must define to virtual---add by rookie 20140625*/	

};

}  // namespace gl_media_engine
#endif // __MEDIA_ENGINE_COMMON_H__
