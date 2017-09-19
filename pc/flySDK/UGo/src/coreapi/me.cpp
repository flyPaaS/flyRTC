#include "me.h"
#include "logtrace.h"
#ifdef VIGO_ENABLED
#include "ViGoEngine.h"
#else
#include "VoGoEngine.h"
#endif

using namespace gl_media_engine;
static MediaEngineInterface* g_pMediaEngine = NULL;

#define CHECK() \
    if ( NULL == g_pMediaEngine ) {\
        ms_error("(%s) ERROR: MediaEngine not exist.", __FUNCTION__); \
        return RES_ERR; \
    }

int me_load()
{
#ifdef VIGO_ENABLED 	
	g_pMediaEngine = new ViGoEngine;
#else
#ifdef VOGO_ENABLED
	g_pMediaEngine = new VoGoEngine;
#endif
#endif

    CHECK();

    return RES_OK;
}

int me_unload(void)
{
    if ( NULL != g_pMediaEngine )
    {
        delete g_pMediaEngine;        
        g_pMediaEngine = NULL;
    }

    return RES_OK;
}

int me_is_load(void)
{
    CHECK();

    return RES_OK;
}

#if defined(ANDROID)
int me_set_android_obj(media_jni_env_pm_t* env)
{
    if ( NULL == env )
    {
        ms_error("me_set_android_obj get NULL param.");
        return RES_ERR;
    }

    CHECK();

    g_pMediaEngine->set_android_obj(env);
    return RES_OK;
}
#endif

int me_callback_vtable(ME_cb_vtable_t* pvtable)
{
	CHECK();

    return g_pMediaEngine->callback_vtable(pvtable);
}

/*In the call me_init() before, 
 *you need to call me_callback_vtable(ME_cb_vtable_t* pvtable)
 *for set up the App callback function.
 */
int me_init(int iDateSource)
{    
	CHECK();

    return g_pMediaEngine->init(iDateSource);
}

int me_destroy(void)
{
	CHECK();
	
    return g_pMediaEngine->destroy();
}

int me_set_audiostream(ME_audio_stream_info_t* maudio)
{
	CHECK();

	return g_pMediaEngine->set_audio_stream(maudio);
}

int me_create_audiostream(void)
{
	CHECK();

	return g_pMediaEngine->create_audio_stream();
}

int me_delete_audiostream(void)
{
	CHECK();
	
	return g_pMediaEngine->delete_audio_stream();
}

int me_enable_audio_playout(char enable)
{
	CHECK();
	
	return g_pMediaEngine->enable_audio_playout(enable);
}

int me_enable_audio_receive(char enable)
{
	CHECK();
	
	return g_pMediaEngine->enable_audio_receive(enable);
}

int me_enable_audio_send(char enable)
{
	CHECK();
	
	return g_pMediaEngine->enable_audio_send(enable);
}

int me_set_api(int apilevel)
{
#ifdef  ANDROID      	
	CHECK();

	g_pMediaEngine->set_android_api_level(apilevel);
#endif

	return RES_OK;
}

int me_get_version(OUT char * uc_version)
{
    if ( NULL == uc_version )
    {
        ms_error("me_get_version: buffer is NULL.");
        return RES_ERR;
    }
    
	CHECK();

	return g_pMediaEngine->get_version(uc_version);
}

int me_get_state(void)
{
	CHECK();
	
	return g_pMediaEngine->get_state();
}

int me_set_state(int state)
{
	CHECK();

	g_pMediaEngine->set_state(state);
	
	return RES_OK;
}

int me_set_cfg(IN int moduleid, IN void* pcfg)
{
	CHECK();

	return g_pMediaEngine->set_config(moduleid, pcfg);
}


int me_get_cfg(OUT int moduleid, OUT void *pcfg)
{
	CHECK();

	return g_pMediaEngine->get_config(moduleid, pcfg);
}

int me_record_start(IN const media_file_record_pm_t* pm)
{
	CHECK();

	return g_pMediaEngine->record_start(pm);
}

int me_record_stop(void)
{
	CHECK();

	return g_pMediaEngine->record_stop();
}

int me_screenshot_start(int islocal, ScreenType type)
{
	CHECK();

	return g_pMediaEngine->screenshot_start(islocal, type);
}

int me_play_file(IN int mode, IN const media_file_play_pm_t* pm)
{
	CHECK();

	return g_pMediaEngine->play_file(mode, pm);
}
#ifdef IOS_SET_FILEPATH
int me_set_filepath(char *filepath)//add by wuzhaoyang20160407
{
	CHECK();

	return g_pMediaEngine->set_filepath(filepath);
}
#endif
int me_stop_file(void)
{
	CHECK();

	return g_pMediaEngine->stop_file();
}

int me_send_dtmf(IN char c_dtmf)
{
	CHECK();

	return g_pMediaEngine->send_DTMF(c_dtmf);
}

/* Add by Vinton.Liu on 20151123 for play dtmf tone to local */
int me_play_dtmf(IN char c_dtmf)
{
    CHECK();

    return g_pMediaEngine->play_DTMF(c_dtmf);
}
/* Vinton.Liu add end */

int me_set_mic_mute(IN char enabled)
{
	CHECK();

	return g_pMediaEngine->set_mic_mute(enabled);
}

int me_get_mic_mute()
{
    CHECK();

    return g_pMediaEngine->get_mic_mute();
}

int me_set_loudspeaker_status(IN char enabled)
{
	CHECK();

	return g_pMediaEngine->set_loudspeaker_status(enabled);
}

int me_get_loudspeaker_status()
{
    CHECK();

    return g_pMediaEngine->get_loudspeaker_status();
}

/* add by Vinton.Liu on 2016-03-16 for Detect recording devices status begin */
int me_get_recording_device_status()
{
    CHECK();
    
    return g_pMediaEngine->get_recording_device_status();
}
/* Vinton.Liu add end */

int me_set_log_file(IN int level, IN const char* filepath)
{
    CHECK();

	g_pMediaEngine->set_debug_level(level);

    if ( NULL != filepath && filepath[0] != '\0' )
    {
	    return g_pMediaEngine->set_log_file_path(filepath);
    }
	return RES_OK;
}

int me_get_codecs(OUT ME_codec_list_t* pcodecs)
{
	CHECK();

    return g_pMediaEngine->get_codecs(pcodecs);
}

int me_get_emodel(OUT ME_emodel_info_t* pemodel)
{
    if ( NULL == pemodel )
    {
        ms_error("me_get_emodel get NULL param.");
        return RES_ERR;
    }
    
	CHECK();

    return g_pMediaEngine->get_emodel_calc_value(pemodel);
}


OUT int me_get_rtt(void)
{
	CHECK();
	
    return g_pMediaEngine->get_rtt();
}

int me_update_ice_mode(IN int mode)
{
	CHECK();
	
    return g_pMediaEngine->update_ice_mode(mode);
}

int me_get_ice_mode()
{
    CHECK();

    return g_pMediaEngine->get_ice_mode();
}

int me_is_start(void)
{
	CHECK();

    return g_pMediaEngine->music_is_started();
}

int me_recv_data_packet(IN const recv_pm_t* recv_pm)
{
	CHECK();

    return g_pMediaEngine->recv_data_packet(recv_pm);
}


//add by wuzhaoyang20140607
int me_enable_audio_fec(char enable)
{
	CHECK();

	return g_pMediaEngine->enable_audio_fec(enable);
}

//add by wuzhaoyang20151225
int me_set_directcallfec_status(char enable)
{
	CHECK();

	return g_pMediaEngine->set_directcallfec_status(enable);
}

int me_get_audio_fec(void)
{
	CHECK();

	return g_pMediaEngine->get_audio_fec();
}

int me_set_audio_fec(unsigned short nMulriple, unsigned short  nOffset)//yangjj add
{
	CHECK();

	return g_pMediaEngine->set_audio_fec(nMulriple, nOffset);
}

int me_set_net_analyze(char isAnalyze)
{
	CHECK();

	return g_pMediaEngine->set_net_analyze(isAnalyze);
}
/* Set, return 0 if succeed, else -1 */
int me_video_set_stream(const ME_video_stream_info_t* pVideoInfo)
{
	CHECK();

	return g_pMediaEngine->video_set_stream(pVideoInfo);
}

/* Create video channel, return 0 if succeed, else -1 */
int me_video_create_stream(void)
{
	CHECK();

	return g_pMediaEngine->video_create_stream();
}

/* Delete video channel, return 0 if succeed, else -1 */
int me_video_delete_stream(void)
{
	CHECK();

	return g_pMediaEngine->video_delete_stream();
}

/* Start recording Video, parameter:0-output, 1-input, return 0 if succeed, else -1 */
int me_video_start_record(const ME_video_record_param_t* record_param)
{
	CHECK();

	return g_pMediaEngine->video_start_record(record_param);
}

/* Stop recording Video, parameter:0-output, 1-input, return 0 if succeed, else -1 */
int me_video_stop_record(const unsigned int stream_select)
{
	CHECK();

	return g_pMediaEngine->video_stop_record(stream_select);
}

/* Set Video remote send rotation */
int me_video_set_remote_sendrotation(const int angle)
{
    CHECK();
    return g_pMediaEngine->video_set_remote_send_rotation(angle);
}

/* Get Video Send Rotation and State */
int me_video_get_sendrotation_state(int& angle, int& state)
{
    CHECK();
    return g_pMediaEngine->video_get_sendrotation_state(angle, state);
}

/* Set video rotation angle via rtp channel or not */
int me_video_set_media_signal_status(int status)
{
    CHECK();
    return g_pMediaEngine->video_set_media_signal_status(status);
}

/* Send video angle via media channel */
int me_video_send_state_packets(int type)
{
    CHECK();
    return g_pMediaEngine->video_send_state_packets(type);
}

/* Start video, interface also for user, return 0 if succeed */
int me_video_start(const unsigned int type)
{
	CHECK();

	return g_pMediaEngine->video_start(type);
}

/* Stop video, interface also for user, return 0 if succeed */
int me_video_stop(const unsigned int type)
{
	CHECK();

	return g_pMediaEngine->video_stop(type);
}

/* Set video device, return 0 if succeed, else -1 */
int me_video_set_camera(IN const ME_video_camera_param_t* param)
{
	CHECK();

	return g_pMediaEngine->video_set_camera(param);
}

/* Get camera count, return value = camera count */
int me_video_get_camera_count(void)
{
	CHECK();

	return g_pMediaEngine->video_get_camera_count();
}

/* Get camera state which was used currently */
int me_video_get_camera_state(OUT ME_video_camera_param_t* camera_state)
{
	CHECK();

	return g_pMediaEngine->video_get_camera_state(camera_state);
}

/* Get camera info */
int me_video_get_camera_info(IN const ucpaas_uint32 camera_idx, OUT ME_video_camera_info_t* camera_info)
{
	CHECK();

	return g_pMediaEngine->video_get_camera_info(camera_idx, camera_info);
}

/* Get camera Capability */
int me_video_get_camera_capability(IN const ucpaas_uint8 camera_idx, 
                                           IN const ucpaas_uint8 cap_idx, 
                                           OUT ME_video_camera_capability_t* camera_capability)
{
	CHECK();

	return g_pMediaEngine->video_get_camera_capability(camera_idx, cap_idx, camera_capability);
}


int me_video_set_send_recive_rotation(IN ucpaas_uint32 send_rotation, IN ucpaas_uint32 recive_rotation)
{
	CHECK();

	return g_pMediaEngine->video_set_send_recive_rotation(send_rotation, recive_rotation);
}

int me_video_incoming_frame(const ucpaas_uint8* data,
    const ucpaas_uint32 len)
{
    CHECK();

    return g_pMediaEngine->video_incoming_frame(data, len);
}

int me_video_set_filter(const int filter_type,
                         const ucpaas_float32 filter_sensisty)
{
    CHECK();
    
    return g_pMediaEngine->video_set_filter(filter_type, filter_sensisty);
}


int me_video_preset(IN char*key, IN char*value)
{
	CHECK();

	return g_pMediaEngine->video_preset(key, value);
}


int me_video_set_remote_dec_resolution(ucpaas_uint32 res)
{
	CHECK();

	return g_pMediaEngine->video_set_remote_dec_resolution(res);
}

int me_video_set_remote_camera_resolution(ucpaas_uint32 width, ucpaas_uint32 height)
{
	CHECK();

	return g_pMediaEngine->video_set_remote_camera_resolution(width, height);
}

int me_video_set_remote_fps(ucpaas_uint8 fps)
{
	CHECK();

	return g_pMediaEngine->video_set_remote_fps(fps);
}

int me_video_set_remote_bitrate(ucpaas_uint32 start_bitrate, ucpaas_uint32 min_bitrate, ucpaas_uint32 max_bitrate)
{
	CHECK();

	return g_pMediaEngine->video_set_remote_bitrate( start_bitrate, min_bitrate, max_bitrate);
}

#ifdef EXTERNAL_AUDIO_DEVICE
int me_send_pcm(ME_exl_pcm_input_itf_t* stInput)
{
	CHECK();

    return g_pMediaEngine->exl_send_pcm(stInput);
}
int me_rcv_pcm(ME_exl_pcm_output_itf_t* stOutput)
{
	CHECK();

    return g_pMediaEngine->exl_rcv_pcm(stOutput);
}
#endif

#ifdef AUTO_TEST_SWITCH
int me_set_testMode(int iMode)
{
	CHECK();

	return g_pMediaEngine->set_test_mode(iMode);
}    

int me_start_RTPDump(const char fileNameUTF8[1024], int direction)
{
	CHECK();

	return g_pMediaEngine->start_RTPDump(fileNameUTF8, direction);
}    

int  me_stop_RTPDump(int direction)	
{
	CHECK();

	return g_pMediaEngine->stop_RTPDump(direction);
}    

int me_get_localIP(int& port, char ipAddr[64])
{
	CHECK();

	return g_pMediaEngine->get_localIP(port, ipAddr);
}    

int me_get_destIP(int& port, char ipAddr[64])
{
	CHECK();

	return g_pMediaEngine->get_destIP(port, ipAddr);
}    

#endif

