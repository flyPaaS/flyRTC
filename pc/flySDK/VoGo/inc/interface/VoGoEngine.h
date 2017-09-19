#ifndef  _VOGO_MEDIA_ENGINE_H_
#define _VOGO_MEDIA_ENGINE_H_

#include "MediaEngineCommon.h"

namespace gl_media_engine {

class VoGoEngine : public MediaEngineInterface{

public:
	VoGoEngine(void);

public:
	///////////////// COMMON FUNCTIONS BEGIN /////////////////////
#ifdef ANDROID
	/* Set App context object for android platform */
	virtual void set_android_obj(const media_jni_env_pm_t* pm_obj);

	virtual void set_android_api_level(ucpaas_int32 level);
#endif

	/* Voe module init */	
	virtual int init(int iDataSource);
	
	/* Voe module destroy */
	virtual int destroy(void);
	
	/* App callbacks fuction register */
	virtual int callback_vtable(const ME_cb_vtable_t* pvtable);

	/* get config prms of VoGo */	
	virtual int get_config(ucpaas_int32 methodID, void* cfg_struct);

	/* Voe module config set */ 
	virtual int set_config(ucpaas_int32 methodID, void* cfg_struct);

	/* Set debug level */
	virtual int set_debug_level(ucpaas_int32 level);

	/* Set log file path */
	virtual int set_log_file_path(const ucpaas_int8* filepath);

	/* Get current voe version */
	virtual int get_version(ucpaas_int8* voe_version);
	
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
	virtual int enable_audio_fec(ucpaas_int8 enable);//add by wuzhaoyang20140607

	virtual int set_directcallfec_status(ucpaas_int8 enable);//add by wuzhaoyang20151225

	virtual int get_audio_fec(void);

	virtual int set_audio_fec(ucpaas_uint16 nMulriple, ucpaas_uint16 nOffset);//yangjj add
	
	virtual int set_net_analyze(bool isAnalyze);

	/* Start record */
	virtual int record_start(const media_file_record_pm_t* pm);
	
	/* Stop record */
	virtual int record_stop(void);

	virtual int play_file(ucpaas_int32 mode, const media_file_play_pm_t* pm);

	virtual int stop_file(void);

#ifdef IOS_SET_FILEPATH
    virtual int set_filepath(char *filepath);//add by wuzhaoyang20160407
#endif

	/* Whether music have been started */
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

    /* Recording device status */
    virtual int get_recording_device_status();
    
	///////////////// AUDIO FUNCTIONS END /////////////////////
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

public:
  virtual ~VoGoEngine();
};

}  // namespace gl_media_engine

#endif  //_MEDIASTREAMINTERFACE_H_


