#ifndef  _ViGO_MEDIA_ENGINE_H_
#define _ViGO_MEDIA_ENGINE_H_

#include "MediaEngineCommon.h"

namespace gl_media_engine {

class ViGoEngine : public MediaEngineInterface{

public:
	ViGoEngine(void);
    ~ViGoEngine();

public:
    ///////////////// COMMON FUNCTIONS BEGIN /////////////////////
#ifdef ANDROID
	/* Set App context object for android platform */
	virtual void set_android_obj(const media_jni_env_pm_t* pm_obj);

	virtual void set_android_api_level(ucpaas_int32 level);
#endif

	/* MediaEngine module init */	
	virtual int init(int iDataSource);
	
	/* MediaEngine module destroy */
	virtual int destroy(void);
	
	/* App callbacks fuction register */
	virtual int callback_vtable(const ME_cb_vtable_t* pvtable);

	/* Set debug level */
	virtual int set_debug_level(ucpaas_int32 level);

	/* Set log file path */
	virtual int set_log_file_path(const ucpaas_int8* filepath);

	/* Get current voe version */
	virtual int get_version(ucpaas_int8* voe_version);

	/* get config prms of VoGo */	
	virtual int get_config(ucpaas_int32 methodID, void* cfg_struct);

	/* Voe module config set */ 
	virtual int set_config(ucpaas_int32 methodID, void* cfg_struct);
    
	/* Get current voe state */
	virtual int get_state(void);

	/* Get current voe state */
	virtual void set_state(ucpaas_int32 state);

    /*Update ice mode*/
	virtual int update_ice_mode(ucpaas_int32 mode);

	/* Get ice mode */
	virtual int get_ice_mode();
	
	virtual int recv_data_packet(const recv_pm_t* recv_pm);
    ///////////////// COMMON FUNCTIONS END /////////////////////
    /////////////////////////////////////////////////////////////

    ///////////////// AUDIO FUNCTIONS BEGIN /////////////////////
	/*set audio streams*/
	virtual int set_audio_stream(const ME_audio_stream_info_t *pAudioInfo);	

	/* create audio media streams */
	virtual int create_audio_stream(void);
	
	/* delete audio media streams */
	virtual int delete_audio_stream(void);

    /*restart audio media stream*/
	virtual int restart_audio_media_stream(void);
	
    /* Start /Stop audio playout */
	virtual int enable_audio_playout(ucpaas_int8 enable);

	/* Start/Stop audio send */
	virtual int enable_audio_send(ucpaas_int8 enable);

	/* Start/Stop audio receive */
	virtual int enable_audio_receive(ucpaas_int8 enable);
	
	/* enable audio fec */
	virtual int enable_audio_fec(char enable);//add by wuzhaoyang20140607

	virtual int set_directcallfec_status(ucpaas_int8 enable);//add by wuzhaoyang20151225

    virtual int set_audio_fec(ucpaas_uint16 nMulriple, ucpaas_uint16 nOffset);//yangjj add
    
	virtual int get_audio_fec(void);

	/* Start record */
	virtual int record_start(const media_file_record_pm_t* pm);
	
	/* Stop record */
	virtual int record_stop(void);

	virtual int play_file(ucpaas_int32 mode, const media_file_play_pm_t* pm);

	virtual int stop_file(void);

#ifdef IOS_SET_FILEPATH
    virtual int set_filepath(char *filepath);//add by wuzhaoyang20160407
#endif 


	/* Whether sound have been started */
	virtual int music_is_started(void);

	/* send DTMF */
	virtual int send_DTMF(ucpaas_int8 c_dtmf);

    /* play DTMF to local */
    virtual int play_DTMF(ucpaas_int8 c_dtmf);

	/* Mute set */
	virtual int set_mic_mute(ucpaas_int8 enabled);

    /* Get Mute */
    virtual int get_mic_mute();

	/* Speaker status set */
	virtual int set_loudspeaker_status(ucpaas_int8 enabled);

    /* Speaker status get */
    virtual int get_loudspeaker_status();

    /* Recording device status */
    virtual int get_recording_device_status();

	virtual int set_net_analyze(bool isAnalyze);

	/*Set audio device*/
	virtual int set_audio_device(const audio_device_pm_t* pm);

	/*Set speaker volume*/
	virtual int set_speaker_volume(ucpaas_uint32 volume);

	/*Set mic volume*/
	virtual int set_mic_volume(ucpaas_uint32 volume);
  
	/* Set codec supported by check methods */
	virtual int codec_supported(const codec_support_pm_t* pm);

	/* Get current SP codecs */
	virtual int get_codecs(ME_codec_list_t* codecs_list);

	/* Get Emodel Calc values*/
	virtual int get_emodel_calc_value(ME_emodel_info_t* pemodel);

	/*Get RTT value*/
	virtual int get_rtt(void);

#ifdef EXTERNAL_AUDIO_DEVICE
 	virtual int exl_send_pcm(ME_exl_pcm_input_itf_t* stInput);
 	
	virtual int exl_rcv_pcm(ME_exl_pcm_output_itf_t* stOutput);
#endif	
    ///////////////// AUDIO FUNCTIONS END /////////////////////
    ///////////////////////////////////////////////////////////

    ///////////////// VIDEO FUNCTIONS BEGIN /////////////////////
	/* Set, return 0 if succeed, else -1 */
	virtual int video_set_stream(const ME_video_stream_info_t* pVideoInfo);
	
	/* Create video stream, return 0 if succeed, else -1 */
	virtual int video_create_stream(void);

	/* Delete video stream, return 0 if succeed, else -1 */
	virtual int video_delete_stream(void);

	/* Start recording Video, parameter:0-output, 1-input, return 0 if succeed, else -1 */
	virtual int video_start_record(const ME_video_record_param_t* record_param);

	/* Stop recording Video, parameter:0-output, 1-input, return 0 if succeed, else -1 */
	virtual int video_stop_record(const ucpaas_uint32 stream_select);

	/* Start video, interface also for user, return 0 if succeed */
	virtual int video_start(const ucpaas_uint32 type);

	/* Stop video, interface also for user, return 0 if succeed */
	virtual int video_stop(const ucpaas_uint32 type);

	/* Set video device, return 0 if succeed, else -1 */
	virtual int video_set_camera(const ME_video_camera_param_t* param);

	/* Get camera count, return value = camera count */
	virtual int video_get_camera_count(void);

	/* Get camera state which was used currently */
	virtual int video_get_camera_state(ME_video_camera_param_t* camera_state);

	/* Get camera info */
	virtual int video_get_camera_info(const ucpaas_uint8 camera_idx,
	                                    ME_video_camera_info_t* camera_info);	

	/* Get camera Capability */
	virtual int video_get_camera_capability(const ucpaas_uint8 camera_idx, 
	                                            const ucpaas_uint8 cap_idx, 
	                                            ME_video_camera_capability_t* camera_capability);

	/* set remote dec resolution for resolution negotiation*/
	virtual int video_set_remote_dec_resolution(ucpaas_uint32 res);

	virtual int video_set_remote_camera_resolution(ucpaas_uint32 width, 
	                                                    ucpaas_uint32 height);

	/* set remote dec fps for fps negotiation*/
	virtual int video_set_remote_fps(ucpaas_uint8  fps);

	virtual int video_set_remote_bitrate(ucpaas_uint32 start_bitrate, 
	                                            ucpaas_uint32 min_bitrate,
	                                            ucpaas_uint32 max_bitrate);

	virtual int video_set_send_recive_rotation(ucpaas_uint32 send_rotation, 
	                                            ucpaas_uint32 recive_rotation);	

	virtual int video_incoming_frame(const ucpaas_uint8* data, 
	                                        const ucpaas_uint32 len);
    virtual int video_set_filter(const int filter_type,const ucpaas_float32 filter_sensity);

	virtual int screenshot_start(int islocal, ScreenType type);

	virtual int  video_preset(ucpaas_int8*key, ucpaas_int8*value);

	/* Set Video remote send rotation */
    virtual int video_set_remote_send_rotation(const int angle);
    
	/* Get Video Send Rotation and State */
    virtual int video_get_sendrotation_state(int& angle, int& state);

    /* Set video rotation angle via rtp channel or not */
	virtual int video_set_media_signal_status(int status);

	virtual int video_send_state_packets(int type);
    ///////////////// VIDEO FUNCTIONS END /////////////////////
    ///////////////////////////////////////////////////////////

    ///////////////// AUTO TEST FUNCTIONS BEGIN /////////////////////
#ifdef AUTO_TEST_SWITCH
	virtual int set_test_mode(ucpaas_int32 iMode);
	
	virtual int start_RTPDump(const char fileNameUTF8[1024], int direction);
	
	virtual int  stop_RTPDump(int direction);	
	
	virtual int get_localIP(int& port, char ipAddr[64]);
	
	virtual int get_destIP(int& port, char ipAddr[64]);	
#endif
	///////////////// AUTO TEST FUNCTIONS END /////////////////
    ///////////////////////////////////////////////////////////
};

}  // namespace gl_media_engine

#endif  //_MEDIASTREAMINTERFACE_H_


