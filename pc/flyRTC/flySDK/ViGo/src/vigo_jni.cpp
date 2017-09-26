#ifdef ANDROID
#include <jni.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <android/log.h>
#include <msjava.h>

#include <sys/socket.h>
#include <netinet/in.h>

#ifdef VIGO_ENABLED
#include "ViGoEngine.h"
#endif

#include "webrtc/modules/video_coding/codecs/mediacodec/android/on_load.h"

#include<bson/bson.h>
#include<json/json_api.h>
#include<json/json.h>

using namespace gl_media_engine;

#define TAG    "*VIGO*"
#define LOGI(...) \
    if ( g_debugflag ) \
        ((void)__android_log_print(ANDROID_LOG_INFO,  TAG, __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN,  TAG, __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__))

static JavaVM *jvm=0;
static gl_media_engine::MediaEngineInterface* g_vigo = NULL;
static int g_debugflag = 1;

JNIEXPORT jint JNICALL  JNI_OnLoad(JavaVM *ajvm, void *reserved)
{
#ifdef ANDROID
    ms_set_jvm(ajvm);

#ifdef VIGO_ENABLED
	JNI_OnLoad_SetWebRTC_JVM(ajvm, reserved);
#endif

#endif /*ANDROID*/
    jvm=ajvm;
    return JNI_VERSION_1_4;
}

extern "C" jint Java_com_gl_softphone_ViGoManager_vieLoadMediaEngine(JNIEnv*  env
        ,jobject  thiz )
{

#ifdef VIGO_ENABLED
	g_vigo = new ViGoEngine();
#endif

	if ( g_vigo == NULL )
	{
		LOGE("vieLoadMediaEngine: g_vigo is null");
		return -1;
	}

	return 0;
}

extern "C" void Java_com_gl_softphone_ViGoManager_vieSetAndroidContext(JNIEnv*  env
        ,jobject  thiz
        ,jobject context)
{
    media_jni_env_pm_t cfg;
    LOGI("vieSetAndroidContext: start");
    if ( g_vigo )
    {
    	cfg.jvm		= jvm;
    	cfg.env 	= env;
    	cfg.context = context;
        g_vigo->set_android_obj(&cfg);
		
		LOGI("vieSetAndroidContext: Success!");
	}
}

extern "C" jstring Java_com_gl_softphone_ViGoManager_vieGetVersion(JNIEnv* env
	,jobject thiz)
{
	int ret = -1;
	char version[64]={0};
    
	g_vigo->get_version(version);
	jstring string_version = env->NewStringUTF(version);

    LOGI("vieGetVersion: version = %s", version);
	
	return string_version;	
}

extern "C" jint Java_com_gl_softphone_ViGoManager_vieInit(JNIEnv * env
		,jobject thiz)
{
	 int ret = -1;

	 if ( g_vigo )
	 {
	 	ret = g_vigo->init(0);

        LOGI("vieInit: ret = %d", ret);
	 }

	 return ret;
}

extern "C" jint Java_com_gl_softphone_ViGoManager_vieDestroy(JNIEnv* env
		,jobject thiz)
{
    int ret = -1;

    if ( g_vigo )
    {
        ret = g_vigo->destroy();
    }

    LOGI("vieDestroy: ret = %d ",ret);
    return ret;
}

extern "C" jint Java_com_gl_softphone_ViGoManager_vieGetState(JNIEnv*  env
        ,jobject  thiz)
{
	int ret =-1;
	
	if ( g_vigo )
	{
		ret = g_vigo->get_state();
	}

    LOGI("vieGetState: ret =%d", ret);
	return ret;
}

extern "C" void Java_com_gl_softphone_ViGoManager_vieSetState(JNIEnv*  env
        ,jobject  thiz
        ,jint state)
{
	if ( g_vigo )
	{
		g_vigo->set_state(state);
	}

	LOGI("vieSetState state = %d", state);
}

extern "C" jint Java_com_gl_softphone_ViGoManager_vieSetLogCfg(JNIEnv* env
        ,jobject thiz
        ,jobject param)
{
    int ret = -1;
    char filepath[256]= {0};

    if ( g_vigo == NULL || param == NULL )
    {
        LOGE("vieSetLogCfg: param is NULL. ");
        return -1;
    }
    
    jclass cfgClass = (jclass)env->NewGlobalRef(env->GetObjectClass(param));
    jfieldID filepath_id = env->GetFieldID(cfgClass, "path","Ljava/lang/String;");
    jfieldID loglevle_id = env->GetFieldID(cfgClass, "level", "I");

    jstring jfilepath = (jstring)env->GetObjectField(param, filepath_id);
    int iLevel = env->GetIntField(param, loglevle_id);
    const char* file_path = jfilepath?env->GetStringUTFChars(jfilepath, NULL):NULL;

    if (file_path)
    {
        strncpy(filepath, file_path, sizeof(filepath)-1 );
    }

    ret = g_vigo->set_debug_level(iLevel);
    ret = g_vigo->set_log_file_path(filepath);

    if (cfgClass)
    {
        env->DeleteGlobalRef(cfgClass);
        cfgClass = NULL;
    }

    LOGI("vieSetLogCfg: filepath = %s", filepath);

    return ret;
}

extern "C" jint Java_com_gl_softphone_ViGoManager_vieSetLoudSpeakerStatus(JNIEnv* env
	,jobject thiz, jboolean enable)
{
	int ret = -1;
	
	if ( g_vigo )
	{
		ret = g_vigo->set_loudspeaker_status(enable);
	}
	
	LOGI("vieSetLoudSpeakerStatus: ret=%d", ret);
	
	return ret;
}

extern "C" jboolean Java_com_gl_softphone_ViGoManager_vieGetLoudSpeakerStatus(JNIEnv* env
    , jobject thiz)
{
    int ret = 0;
    if ( g_vigo )
    {
        ret = g_vigo->get_loudspeaker_status();
    }

    LOGI("vieGetLoudSpeakerStatus: ret = %d", ret);

    return (jboolean)ret;
}

extern "C" jint Java_com_gl_softphone_ViGoManager_vieSetMicMute(JNIEnv* env
	,jobject thiz, jboolean enable)
{
	int ret = -1;
	
	if ( g_vigo )
	{
		ret = g_vigo->set_mic_mute(enable);
	}
	
	LOGI("vieSetMicMute: ret=%d", ret);
	
	return ret;
}

extern "C" jboolean Java_com_gl_softphone_ViGoManager_vieGetMicMute(JNIEnv* env
    , jobject thiz)
{
    int ret = 0;
    if ( g_vigo )
    {
        ret = g_vigo->get_mic_mute();
    }

    return (jboolean)ret;
}


extern "C" jint Java_com_gl_softphone_ViGoManager_vieCreateAudioStream(JNIEnv* env
	,jobject thiz)
{
	int ret = -1;

	if ( g_vigo )
	{
		ret = g_vigo->create_audio_stream();
		
	}
    
	LOGI("vieCreateAudioStream: ret =%d", ret);
	return ret;
}

extern "C" jint Java_com_gl_softphone_ViGoManager_vieDeleteAudioStream(JNIEnv*  env
        ,jobject  thiz)
{
  int ret = -1;
  
  if(g_vigo)
  {
	ret = g_vigo->delete_audio_stream();
	
  }
  
  LOGI("vieDeleteAudioStream: ret = %d", ret);
  return ret;
}

extern "C" jint Java_com_gl_softphone_ViGoManager_vieSetAudioStream(JNIEnv*  env
        ,jobject  thiz
        ,jobject para)
{
	int ret = -1;
	ME_audio_stream_info_t audioInfo;
	jclass envClass = NULL;
	int playload = -1;
	jstring remoteip_string = NULL;
	int remote_port = -1;
	int local_port = -1;
	bool exTransportEnable;

    LOGI("vieSetAudioStream: begin");

    if ( NULL == g_vigo || NULL == para )
    {
        LOGE("vieSetAudioStream: param is NULL. ");
        return -1;
    }

	envClass = (jclass) env->NewGlobalRef(env->GetObjectClass(para));
	jfieldID playloadID = env->GetFieldID(envClass, "playload", "I");
	jfieldID remoteipID = env->GetFieldID(envClass, "remote_ip", "Ljava/lang/String;");
	jfieldID remoteportID = env->GetFieldID(envClass, "remote_port", "I");
	jfieldID localportID = env->GetFieldID(envClass, "local_port", "I");
	jfieldID ucExTransportEnableID = env->GetFieldID(envClass,"ucExTransportEnable","Z");
    
	playload = env->GetIntField(para, playloadID);
	remoteip_string = (jstring) env->GetObjectField(para, remoteipID);
	const char* remoteip_char = remoteip_string?env->GetStringUTFChars(remoteip_string, NULL):NULL;
	remote_port = env->GetIntField(para, remoteportID);
	local_port = env->GetIntField(para, localportID);
	exTransportEnable = env->GetBooleanField(para, ucExTransportEnableID);

    memset(&audioInfo, 0, sizeof(audioInfo));		
	audioInfo.iPayLoadType = playload;
	audioInfo.iRemoteAudioPort = remote_port;
	audioInfo.iLocalAudioPort = local_port;
	audioInfo.ucExTransportEnable = exTransportEnable;
	if (remoteip_char)
	{
		strcpy(audioInfo.cRemoteAudioIp, remoteip_char);
		env->ReleaseStringUTFChars(remoteip_string, remoteip_char);
	}
    
	LOGI("vieSetAudioStream: audioInfo = { pt = %d, rp = %d lp = %d rip = %s }", \
		audioInfo.iPayLoadType, audioInfo.iRemoteAudioPort,\
		audioInfo.iLocalAudioPort, audioInfo.cRemoteAudioIp);
	
	ret = g_vigo->set_audio_stream(&audioInfo);

	if (envClass)
	{
		env->DeleteGlobalRef(envClass);
		envClass = NULL;
	}

    LOGI("vieSetAudioStream: ret = %d",ret);	
	return ret;
}

extern "C" jint Java_com_gl_softphone_ViGoManager_vieEnableAudioPlayout(JNIEnv* env
		,jobject thiz
		,jboolean enable)
{
	int ret = -1;

	if(g_vigo) {
		ret = g_vigo->enable_audio_playout(enable);
	}

    LOGI("vieEnableAudioPlayout: ret = %d",ret);	
	return ret;
}
extern "C" jint Java_com_gl_softphone_ViGoManager_vieEnableAudioSend(JNIEnv* env
		,jobject thiz
		,jboolean enable)
{
	int ret = -1;

	if(g_vigo)
	{
		ret = g_vigo->enable_audio_send(enable);
	}

    LOGI("vieEnableAudioSend: ret = %d", ret);
	return ret;
}

extern "C" jint Java_com_gl_softphone_ViGoManager_vieEnableAudioReceive(JNIEnv* env
		,jobject thiz
		,jboolean enable)
{
	int ret = -1;

	if(g_vigo)
	{
		ret = g_vigo->enable_audio_receive(enable);
	}

    LOGI("vieEnableAudioReceive: ret = %d", ret);
	
	return ret;
}

extern "C" jint Java_com_gl_softphone_ViGoManager_vieCreateVideoStream(JNIEnv* env
	,jobject thiz)
{
	int ret = -1;

	if(g_vigo)
	{
		ret = g_vigo->video_create_stream();
		
	}
    
    LOGI("vieCreateVideoStream: ret = %d", ret);
	return ret;
}

extern "C" jint Java_com_gl_softphone_ViGoManager_vieDeleteVideoStream(JNIEnv*  env
        ,jobject  thiz)
{
    int ret = -1;

    if ( g_vigo )
    {
        ret = g_vigo->video_delete_stream();
    }
    
    LOGI("vieDeleteVideoStream: ret=%d", ret);
    return ret;
}

extern "C" jint Java_com_gl_softphone_ViGoManager_vieSetVideoStream(JNIEnv* env
    ,jobject thiz
    , jobject para)
{
    int ret = -1;
    jclass envClass = NULL;
    int playload = -1;
	jstring remoteip_string = NULL;
	int remote_port = -1;
	int local_port = -1;
	bool exTransportEnable;
    ME_video_stream_info_t videoInfo;

    if ( NULL == g_vigo || NULL == para )
    {
        LOGE("vieSetVideoStream: param is NULL. ");
        return -1;
    }
    
    envClass = (jclass) env->NewGlobalRef(env->GetObjectClass(para));
    jfieldID playloadID = env->GetFieldID(envClass, "playload", "I");
	jfieldID remoteipID = env->GetFieldID(envClass, "remote_ip", "Ljava/lang/String;");
	jfieldID remoteportID = env->GetFieldID(envClass, "remote_port", "I");
	jfieldID localportID = env->GetFieldID(envClass, "local_port", "I");
	jfieldID ucExTransportEnableID = env->GetFieldID(envClass,"ucExTransportEnable", "Z");
	
	playload = env->GetIntField(para, playloadID);
	remoteip_string = (jstring) env->GetObjectField(para, remoteipID);
	const char* remoteip_char = remoteip_string ? env->GetStringUTFChars(remoteip_string, NULL) : NULL;
	remote_port = env->GetIntField(para, remoteportID);
	local_port = env->GetIntField(para, localportID);
	exTransportEnable = env->GetBooleanField(para, ucExTransportEnableID);

    memset(&videoInfo, 0x00, sizeof(videoInfo));
	videoInfo.iPayLoadType = playload;
	videoInfo.iRemoteVideoPort = remote_port;
	videoInfo.iLocalVideoPort = local_port;
	videoInfo.ucExTransportEnable = exTransportEnable;
	if ( remoteip_char )
	{
		strncpy(videoInfo.cRemoteVideoIp, remoteip_char, sizeof(videoInfo.cRemoteVideoIp)-1);
		env->ReleaseStringUTFChars(remoteip_string, remoteip_char);
	}
    
	LOGI("vieSetVideoStream: videoInfo = { pt = %d, rp = %d lp = %d rip = %s }",
		videoInfo.iPayLoadType, videoInfo.iRemoteVideoPort,\
		videoInfo.iLocalVideoPort, videoInfo.cRemoteVideoIp);
	
	ret = g_vigo->video_set_stream(&videoInfo);

	if (envClass)
	{
		env->DeleteGlobalRef(envClass);
		envClass = NULL;
	}

    LOGI("vieSetVideoStream: ret = %d", ret);
    
    return ret;
}

extern "C" jint Java_com_gl_softphone_ViGoManager_vieStartVideo(JNIEnv* env
    , jobject thiz, jint type)
{
    int ret = -1;
    if ( g_vigo )
    {
        LOGI("vieStartVideo: type = %d", type);
        ret = g_vigo->video_start(type);
    }
    
    LOGI("vieStartVideo: ret = %d", ret);
    return ret;
}

extern "C" jint Java_com_gl_softphone_ViGoManager_vieStopVideo(JNIEnv* env
    , jobject thiz, jint type)
{
    int ret = -1;
    if ( g_vigo )
    {
        ret = g_vigo->video_stop(type);
    }
    
    LOGI("vieStopVideo: ret=%d", ret);
    return ret;
}    

extern "C" jint Java_com_gl_softphone_ViGoManager_vieGetCameraCount(JNIEnv* env
    ,jobject thiz)
{
    int ret = -1;
    if ( g_vigo )
    {
        ret = g_vigo->video_get_camera_count();
    }

    LOGI("vieGetCameraCount. ret = %d", ret);
    
    return ret;
}

extern "C" jint Java_com_gl_softphone_ViGoManager_vieGetCameraParam(JNIEnv* env
    , jobject thiz, jobject para)
{
    int ret = -1;
    jclass envClass = NULL;
    ME_video_camera_param_t cameraParam;

    if ( NULL == g_vigo || NULL == para )
    {
        LOGE("vieGetCameraState: param is NULL. ");
        return -1;
    }

    memset(&cameraParam, 0x00, sizeof(cameraParam));
    ret = g_vigo->video_get_camera_state(&cameraParam);
    if ( ret < 0 )
    {
        LOGE("vieGetCameraState: failed. ret = %d", ret);
        return ret;
    }
    
    envClass = (jclass)env->NewGlobalRef(env->GetObjectClass(para));
    jfieldID usWidth = env->GetFieldID(envClass, "usWidth", "I");
    jfieldID usHeight = env->GetFieldID(envClass, "usHeight", "I");        
    jfieldID usCameraIdx = env->GetFieldID(envClass, "usCameraIdx", "I");
    jfieldID usRotateAngle = env->GetFieldID(envClass, "iRotateAngle", "I");
    jfieldID ucMaxFps = env->GetFieldID(envClass, "ucMaxFps", "I");

    env->SetIntField(para, usWidth, cameraParam.usWidth);
    env->SetIntField(para, usHeight, cameraParam.usHeight);
    env->SetIntField(para, usCameraIdx, cameraParam.usCameraIdx);
    env->SetIntField(para, usRotateAngle, cameraParam.usRotateAngle);
    env->SetIntField(para, ucMaxFps, cameraParam.ucMaxFps);

    if (envClass)
    {
        env->DeleteGlobalRef(envClass);
        envClass = NULL;
    }

    LOGI("vieGetCameraState: ret = %d", ret);
    return ret;
}

extern "C" jint Java_com_gl_softphone_ViGoManager_vieSetCameraParam(JNIEnv* env
    , jobject thiz, jobject para)
{
    int ret = -1;
    jclass envClass = NULL;
    ME_video_camera_param_t cameraParam;

    if ( NULL == g_vigo || NULL == para )
    {
        LOGE("vieSetCameraParam: param is NULL. ");
        return -1;
    }
    
    memset(&cameraParam, 0x00, sizeof(cameraParam));
    
    envClass = (jclass)env->NewGlobalRef(env->GetObjectClass(para));
    jfieldID usWidth = env->GetFieldID(envClass, "usWidth", "I");
    jfieldID usHeight = env->GetFieldID(envClass, "usHeight", "I");        
    jfieldID usCameraIdx = env->GetFieldID(envClass, "usCameraIdx", "I");
    jfieldID usRotateAngle = env->GetFieldID(envClass, "iRotateAngle", "I");
    jfieldID ucMaxFps = env->GetFieldID(envClass, "ucMaxFps", "I");

    cameraParam.usWidth = env->GetIntField(para, usWidth);
    cameraParam.usHeight = env->GetIntField(para, usHeight);
    cameraParam.usCameraIdx = env->GetIntField(para, usCameraIdx);
    cameraParam.usRotateAngle = env->GetIntField(para, usRotateAngle);
    cameraParam.ucMaxFps = env->GetIntField(para, ucMaxFps);

    LOGI("vieSetCameraParam: param = { idx: %d W x H: %d x %d RA: %d MFps: %d }", \
        cameraParam.usCameraIdx, cameraParam.usWidth, cameraParam.usHeight, \
        cameraParam.usRotateAngle, cameraParam.ucMaxFps);
    
    ret = g_vigo->video_set_camera( &cameraParam);
    
    if ( envClass )
    {
        env->DeleteGlobalRef(envClass);
        envClass = NULL;
    }
    
    LOGI("vieSetCameraParam: ret = %d", ret);
    
    return ret;
}

#define MODULE_NAME(moduleId) #moduleId
void getModuleName(int moduleId, char *moduleName)
{
    switch (moduleId )
    {
        case ME_CTRL_CFG_MODULE_ID:
            strcpy(moduleName, MODULE_NAME(ME_CTRL_CFG_MODULE_ID));
            break;
        case ME_VQE_CFG_MODULE_ID:
            strcpy(moduleName, MODULE_NAME(ME_VQE_CFG_MODULE_ID));
            break;
        case ME_RTP_CFG_MODULE_ID:
            strcpy(moduleName, MODULE_NAME(ME_RTP_CFG_MODULE_ID));
            break;
        case ME_ENV_CFG_MODULE_ID:
            strcpy(moduleName, MODULE_NAME(ME_ENV_CFG_MODULE_ID));
            break;
        case ME_VIDEO_ENC_CFG_MODULE_ID:
            strcpy(moduleName, MODULE_NAME(ME_VIDEO_ENC_CFG_MODULE_ID));
            break;
        case ME_VIDEO_DEC_CFG_MODULE_ID:
            strcpy(moduleName, MODULE_NAME(ME_VIDEO_DEC_CFG_MODULE_ID));
            break;
        case ME_VIDEO_RENDER_CFG_MODULE_ID:
            strcpy(moduleName, MODULE_NAME(ME_VIDEO_RENDER_CFG_MODULE_ID));
            break;
        case ME_VIDEO_PROCES_CFG_MODULE_ID:
            strcpy(moduleName, MODULE_NAME(ME_VIDEO_PROCES_CFG_MODULE_ID));
            break;
        default:
            break;
    }
}

extern "C" jint Java_com_gl_softphone_ViGoManager_vieSetConfig(JNIEnv*  env
        ,jobject  thiz, jint ModuleID, jobject config, jint version)
{
    int ret =-1;
    char moduleName[64] = {0};
    jclass envClass = NULL;

    if ( NULL == g_vigo || NULL == config )
    {
        LOGE("vieSetConfig: param is NULL. ");
        return -1;
    }

    getModuleName(ModuleID, moduleName);
    LOGI("vieSetConfig: %s", moduleName);
    
    envClass = (jclass)env->NewGlobalRef(env->GetObjectClass(config));
    
    switch ( ModuleID )
    {
        case ME_CTRL_CFG_MODULE_ID:
        {
            ME_CTRL_cfg_t ctrl_config;

            unsigned int ucRealTimeType;    //Real time protocol type, 0: RTP 1: PRTP
            //unsigned int ucPhoneProtocol; //Phone protocol, 0: disable 1: enable
            unsigned int ucVideoEnable;     //Video module, 0: disable 1: enable
            unsigned int ucEmodelEnable;    //Emodel module, 0: disable 1: enable
            unsigned int ucFecEnable;   //Emodel module, 0: disable 1: enable

            jfieldID ucRealTimeType_id =  env->GetFieldID(envClass, "ucRealTimeType", "I");
            //jfieldID ucPhoneProtocol_id = env->GetFieldID(envClass, "ucPhoneProtocol", "I");
            jfieldID ucVideoEnable_id =  env->GetFieldID(envClass, "ucVideoEnable", "I");
            jfieldID ucEmodelEnable_id = env->GetFieldID(envClass, "ucEmodelEnable", "I");
            jfieldID ucFecEnable_id = env->GetFieldID(envClass, "ucFecEnable", "I");
            ucRealTimeType =  env->GetIntField(config,ucRealTimeType_id);
            //ucPhoneProtocol = env->GetIntField(config,ucPhoneProtocol_id);
            ucVideoEnable = env->GetIntField(config,ucVideoEnable_id);
            ucEmodelEnable = env->GetIntField(config,ucEmodelEnable_id);
            ucFecEnable = env->GetIntField(config,ucFecEnable_id);

            ctrl_config.ucRealTimeType= ucRealTimeType;
            //ctrl_config.ucPhoneProtocol = ucPhoneProtocol;
            ctrl_config.ucVideoEnable = ucVideoEnable;
            ctrl_config.ucEmodelEnable = ucEmodelEnable;
            ctrl_config.ucFecEnable = ucFecEnable;

            LOGI("vieSetConfig: rtp protocal =%d, ucVideoEnable = %d ",\
                ucRealTimeType, ucVideoEnable);

            ret = g_vigo->set_config(ME_CTRL_CFG_MODULE_ID, &ctrl_config);
        }
        break;
        
        case ME_VIDEO_ENC_CFG_MODULE_ID:
        {
            ME_video_enc_param_t vEncCfg;

            jfieldID usWidth_id =  env->GetFieldID(envClass, "usWidth", "I");
            jfieldID usHeight_id =  env->GetFieldID(envClass, "usHeight", "I");
            jfieldID usStartBitrate = env->GetFieldID(envClass, "usStartBitrate", "I");
            jfieldID usMaxBitrate = env->GetFieldID(envClass, "usMaxBitrate", "I");
            jfieldID usMinBitrate = env->GetFieldID(envClass, "usMinBitrate", "I");
            jfieldID ucmaxFramerate = env->GetFieldID(envClass, "ucmaxFramerate", "I");
            jfieldID ucQpMax = env->GetFieldID(envClass, "ucQpMax", "I");
         
            memset(&vEncCfg, 0, sizeof(vEncCfg));

            vEncCfg.usWidth = env->GetIntField(config,usWidth_id);
            vEncCfg.usHeight = env->GetIntField(config,usHeight_id);          
            vEncCfg.usStartBitrate = env->GetIntField(config,usStartBitrate);
            vEncCfg.ucMaxFramerate = env->GetIntField(config,ucmaxFramerate);
            vEncCfg.ucQpMax = env->GetIntField(config,ucQpMax);
            vEncCfg.usMaxBitrate = env->GetIntField(config,usMaxBitrate);
            vEncCfg.usMinBitrate = env->GetIntField(config,usMinBitrate);

            LOGI("vieSetConfig: width =%d, height = %d, bitrate = %d, maxbitrate = %d",\
                vEncCfg.usWidth, vEncCfg.usHeight, \
                vEncCfg.usStartBitrate, vEncCfg.ucMaxFramerate);
     
            ret = g_vigo->set_config(ME_VIDEO_ENC_CFG_MODULE_ID, &vEncCfg);
        }
        break;

        case ME_VIDEO_RENDER_CFG_MODULE_ID:
        {
            ME_video_render_param_t para;
            memset(&para, 0, sizeof(para));
            g_vigo->get_config(ME_VIDEO_RENDER_CFG_MODULE_ID, &para);

            jfieldID pWindowRemote =  env->GetFieldID(envClass, "pWindowRemote", "Ljava/lang/Object;");
            jfieldID pWindowLocal =  env->GetFieldID(envClass, "pWindowLocal", "Ljava/lang/Object;");
            para.pWindowRemote = (void*)env->GetObjectField(config,pWindowRemote);
            para.pWindowLocal = (void*)env->GetObjectField(config,pWindowLocal);

            ret = g_vigo->set_config(ME_VIDEO_RENDER_CFG_MODULE_ID, &para);
        }
        break;

        case ME_VIDEO_PROCES_CFG_MODULE_ID:
        {
            ME_video_proces_module_t para;
            unsigned short usWidth;
            unsigned short usHeight;
            unsigned int   uistartBitrate;
            unsigned char  ucmaxFramerate;
            
            memset(&para, 0, sizeof(para));

            jfieldID deflickingEnable =env->GetFieldID(envClass, "deflickingEnable", "Z");
            jfieldID denoisingEnable =env->GetFieldID(envClass, "denoisingEnable", "Z");
            jfieldID enhancementEnable =env->GetFieldID(envClass, "enhancementEnable", "Z");

            para.deflickingEnable = env->GetBooleanField(config, deflickingEnable);
            para.denoisingEnable =  env->GetBooleanField(config, denoisingEnable);
            para.enhancementEnable=  env->GetBooleanField(config, enhancementEnable);

            ret = g_vigo->set_config(ME_VIDEO_PROCES_CFG_MODULE_ID, &para);
        }
        break;
        
        case ME_VQE_CFG_MODULE_ID:
        {
            ME_VQE_cfg_t vqe_config;
            bool Ec_enable;             //Enable EC function on send port if TRUE, else disable
            bool Agc_enable;            //Enable Agc function on send port if TRUE, else disable
            bool Ns_enable;             //Enable Ns function on send port if TRUE, else disable
            bool Agc_Rx_enable;         //Enable Agc function on receive port if TRUE, else disable
            bool Ns_Rx_enable;

            jfieldID Ec_enable_id =env->GetFieldID(envClass, "Ec_enable", "Z");
            jfieldID Agc_enable_id =env->GetFieldID(envClass, "Agc_enable", "Z");
            jfieldID Ns_enable_id =env->GetFieldID(envClass, "Ns_enable", "Z");
            jfieldID Agc_Rx_enable_id =env->GetFieldID(envClass, "Agc_Rx_enable", "Z");
            jfieldID Ns_Rx_enable_id =env->GetFieldID(envClass, "Ns_Rx_enable", "Z");

            Ec_enable = env->GetBooleanField(config, Ec_enable_id);
            Agc_enable = env->GetBooleanField(config, Agc_enable_id);
            Ns_enable = env->GetBooleanField(config, Ns_enable_id);
            Agc_Rx_enable = env->GetBooleanField(config, Agc_Rx_enable_id);
            Ns_Rx_enable = env->GetBooleanField(config, Ns_Rx_enable_id);

            vqe_config.Ec_enable = Ec_enable;
            vqe_config.Agc_enable = Agc_enable;
            vqe_config.Ns_enable = Ns_enable;
            vqe_config.Agc_Rx_enable = Agc_Rx_enable;
            vqe_config.Ns_Rx_enable = Ns_Rx_enable;
            
            LOGI("vieSetConfig: vqe_cfg={Ec = %d, Agc = %d, Ns = %d, RAgc = %d, RNs = %d}", \
                Ec_enable, Agc_enable, Ns_enable, Agc_Rx_enable, \
                Ns_Rx_enable);

            ret = g_vigo->set_config(ME_VQE_CFG_MODULE_ID, &vqe_config);
        }
        break;
        
        case ME_RTP_CFG_MODULE_ID:
        {
            ME_RTP_cfg_t rtp_config;
            unsigned int uiRTPTimeout;   //RTP time over, it is valid when set RTP as transport;
            bool uiFixLowPayload;        //when network is bad,we used this,only used in prtp protocal,if this enable,auto payload close

            jfieldID uiRTPTimeout_id =  env->GetFieldID(envClass, "uiRTPTimeout", "I");
            jfieldID uiFixLowPayload_id =  env->GetFieldID(envClass, "uiFixLowPayload", "Z");

            uiRTPTimeout = env->GetIntField(config,uiRTPTimeout_id);
            uiFixLowPayload = env->GetBooleanField(config,uiFixLowPayload_id);
            rtp_config.uiRTPTimeout = uiRTPTimeout;
            rtp_config.uiFixLowPayload = uiFixLowPayload;
            
            LOGI("vieSetConfig: timeout =%d, fixlowBitrate = %d", \
                uiRTPTimeout, uiFixLowPayload);
        
            ret = g_vigo->set_config(ME_RTP_CFG_MODULE_ID, &rtp_config);
        }
        break;
        
        case ME_ENV_CFG_MODULE_ID:
        {
            ME_ENV_cfg_t env_st;
            char status;
            char dialogScene;
            int networktype;

            jfieldID status_id = env->GetFieldID(envClass, "status", "Z");
            jfieldID dialogScene_id = env->GetFieldID(envClass, "dialogScene", "I");
            jfieldID networktype_id = env->GetFieldID(envClass, "networktype", "I");
            status = env->GetBooleanField(config,status_id);
            dialogScene = env->GetIntField(config,dialogScene_id);
            networktype = env->GetIntField(config,networktype_id);
            env_st.status = status;
            env_st.dialogScene = dialogScene;
            env_st.networktype = networktype;
            
            ret = g_vigo->set_config(ME_ENV_CFG_MODULE_ID, &env_st);
        }
        break;
        
        default:
            break;
    }

    if (envClass)
    {
        env->DeleteGlobalRef(envClass);
        envClass = NULL;
    }

    LOGI("vieSetConfig: ret = %d", ret);
    return ret;
}

extern "C" jint Java_com_gl_softphone_ViGoManager_vieGetConfig(JNIEnv*  env
        ,jobject  thiz, jint ModuleID, jobject config, jint version)
{
    int ret = -1;
    char moduleName[64] = {0};
    jclass envClass = NULL;

    if ( NULL == g_vigo || NULL == config )
    {
        LOGE("vieGetConfig: param is NULL. ");
        return -1;
    }

    getModuleName(ModuleID, moduleName);
    LOGI("vieGetConfig: %s", moduleName);
    
    envClass = (jclass)env->NewGlobalRef(env->GetObjectClass(config));
    
    switch(ModuleID)
    {
        case ME_CTRL_CFG_MODULE_ID:
        {   
            ME_CTRL_cfg_t ctrl_config;
            memset(&ctrl_config, 0x00, sizeof(ctrl_config));
            ret = g_vigo->get_config(ME_CTRL_CFG_MODULE_ID, &ctrl_config);

            if (ret != 0)
            {
                /* note: always should break for release global reference(envClass) */
                break;
            }

            jfieldID ucRealTimeType_id =env->GetFieldID(envClass, "ucRealTimeType", "I");
            jfieldID ucVideoEnable_id =env->GetFieldID(envClass, "ucVideoEnable", "I");
            jfieldID ucEmodelEnable_id =env->GetFieldID(envClass, "ucEmodelEnable", "I");
            jfieldID ucFecEnable_id =env->GetFieldID(envClass, "ucFecEnable", "I");
            env->SetIntField(config,ucRealTimeType_id,(int)ctrl_config.ucRealTimeType);
            env->SetIntField(config,ucVideoEnable_id,(int)ctrl_config.ucVideoEnable);
            env->SetIntField(config,ucEmodelEnable_id,(int)ctrl_config.ucEmodelEnable);
            env->SetIntField(config,ucFecEnable_id,(int)ctrl_config.ucFecEnable);
            
            LOGI("vieGetConfig: ctrl_cfg={%d,%d,%d,%d}", \
                ctrl_config.ucRealTimeType, ctrl_config.ucVideoEnable, \
                ctrl_config.ucEmodelEnable, ctrl_config.ucFecEnable);
        }
        break;
        
        case ME_VQE_CFG_MODULE_ID:
        {
            ME_VQE_cfg_t vqe_config;
            memset(&vqe_config, 0x00, sizeof(vqe_config));
            
            ret = g_vigo->get_config(ME_VQE_CFG_MODULE_ID, &vqe_config);

            if (ret != 0)
            {
                /* note: always should break for release global reference(envClass) */
                break;
            }

            jfieldID Ec_enable_id =env->GetFieldID(envClass, "Ec_enable", "Z");
            jfieldID Agc_enable_id =env->GetFieldID(envClass, "Agc_enable", "Z");
            jfieldID Ns_enable_id =env->GetFieldID(envClass, "Ns_enable", "Z");
            jfieldID Agc_Rx_enable_id =env->GetFieldID(envClass, "Agc_Rx_enable", "Z");
            jfieldID Ns_Rx_enable_id =env->GetFieldID(envClass, "Ns_Rx_enable", "Z");

            env->SetBooleanField(config,Ec_enable_id,vqe_config.Ec_enable);
            env->SetBooleanField(config,Agc_enable_id,vqe_config.Agc_enable);
            env->SetBooleanField(config,Ns_enable_id,vqe_config.Ns_enable);
            env->SetBooleanField(config,Agc_Rx_enable_id,vqe_config.Agc_Rx_enable);
            env->SetBooleanField(config,Ns_Rx_enable_id,vqe_config.Ns_Rx_enable);

            LOGI("vieGetConfig: vqe_cfg={%d,%d,%d,%d,%d}",\
                vqe_config.Ec_enable, vqe_config.Agc_enable, \
                vqe_config.Ns_enable, vqe_config.Agc_Rx_enable, \
                vqe_config.Ns_Rx_enable);
        }
        break;
        
        case ME_RTP_CFG_MODULE_ID:
        {
            ME_RTP_cfg_t rtp_config;
            memset(&rtp_config, 0x00, sizeof(rtp_config));
            ret = g_vigo->get_config(ME_RTP_CFG_MODULE_ID, &rtp_config);

            if (ret != 0)
            {
                /* note: always should break for release global reference(envClass) */
                break;
            }

            jfieldID uiRTPTimeout_id =  env->GetFieldID(envClass, "uiRTPTimeout", "I");
            jfieldID uiFixLowPayload_id =  env->GetFieldID(envClass, "uiFixLowPayload", "Z");
            env->SetIntField(config,uiRTPTimeout_id,rtp_config.uiRTPTimeout);
            env->SetBooleanField(config,uiFixLowPayload_id,rtp_config.uiFixLowPayload);
            
            LOGI("vieGetConfig: Rtp_cfg={%d,%d}", \
                rtp_config.uiRTPTimeout, \
                rtp_config.uiFixLowPayload);
        }
        break;

        case ME_ENV_CFG_MODULE_ID:
        {
            ME_ENV_cfg_t env_st;

            memset(&env_st, 0x00, sizeof(ME_ENV_cfg_t));
            ret = g_vigo->get_config(ME_ENV_CFG_MODULE_ID, &env_st);

            if (ret != 0)
            {
                break;
            }

            jfieldID status_id = env->GetFieldID(envClass, "status", "Z");
            jfieldID dialogScene_id = env->GetFieldID(envClass, "dialogScene", "I");
            jfieldID networktype_id = env->GetFieldID(envClass, "networktype", "I");
            env->SetBooleanField(config, status_id, env_st.status);
            env->SetIntField(config, dialogScene_id, env_st.dialogScene);
            env->SetIntField(config, networktype_id, env_st.networktype);

            LOGI("vieGetConfig: env_cfg={%d, %d, %d}", \
                env_st.status, env_st.dialogScene, env_st.networktype);
        }
        break;
                    
        case ME_VIDEO_ENC_CFG_MODULE_ID:
        {
            ME_video_enc_param_t vEncCfg;

            memset(&vEncCfg, 0, sizeof(vEncCfg));
            ret = g_vigo->get_config(ME_VIDEO_ENC_CFG_MODULE_ID, &vEncCfg);
            if (ret < 0)
            {
                /* note: always should break for release global reference(envClass) */
                break;
            }


            jfieldID usWidth_id =  env->GetFieldID(envClass, "usWidth", "I");
            jfieldID usHeight_id =  env->GetFieldID(envClass, "usHeight", "I");
      //      jfieldID ucPlName = env->GetFieldID(envClass, "ucPlName", "Ljava/lang/String;");
        //    jfieldID ucPlType =  env->GetFieldID(envClass, "ucPlType", "I");

            jfieldID usStartBitrate =  env->GetFieldID(envClass, "usStartBitrate", "I");
            jfieldID usMaxBitrate =  env->GetFieldID(envClass, "usMaxBitrate", "I");
            jfieldID usMinBitrate =  env->GetFieldID(envClass, "usMinBitrate", "I");
            jfieldID ucmaxFramerate =  env->GetFieldID(envClass, "ucmaxFramerate", "I");
            jfieldID ucQpMax =  env->GetFieldID(envClass, "ucQpMax", "I");

            env->SetIntField(config, usWidth_id,  vEncCfg.usWidth);
            env->SetIntField(config, usHeight_id, vEncCfg.usHeight);           
            

            env->SetIntField(config, usStartBitrate, vEncCfg.usStartBitrate);
            env->SetIntField(config, ucmaxFramerate, vEncCfg.ucMaxFramerate);
            env->SetIntField(config, ucQpMax, vEncCfg.ucQpMax);
            env->SetIntField(config, usMaxBitrate, vEncCfg.usMaxBitrate);
            env->SetIntField(config, usMinBitrate, vEncCfg.usMinBitrate);              

            LOGI("vieGetConfig: width =%d, height = %d, bitrate = %d, maxbitrate = %d", \
                vEncCfg.usWidth, vEncCfg.usHeight, \
                vEncCfg.usStartBitrate, vEncCfg.ucMaxFramerate);
        }
        break;

        case ME_VIDEO_DEC_CFG_MODULE_ID:
        {
            ME_video_dec_info_t  para;
            memset(&para, 0, sizeof(para));

            ret = g_vigo->get_config(ME_VIDEO_DEC_CFG_MODULE_ID, &para);
            if (ret < 0 )
            {
                /* note: always should break for release global reference(envClass) */
                break;
            }
            
            jfieldID uiMaxDefinition_id =  env->GetFieldID(envClass, "uiMaxDefinition", "I");         

            env->SetIntField(config, uiMaxDefinition_id,  para.uiMaxDefinition);
           
           
        }
        break;

        case ME_VIDEO_RENDER_CFG_MODULE_ID:
        {
            ME_video_render_param_t para;
            memset(&para, 0, sizeof(para));
            
            ret = g_vigo->get_config(ME_VIDEO_RENDER_CFG_MODULE_ID, &para);
            if (ret != 0)
            {
                /* note: always should break for release global reference(envClass) */
                break;
            }

            if ( NULL != para.pWindowRemote)
            {
                jfieldID pWindowRemote = env->GetFieldID(envClass, "pWindowRemote", "Ljava/lang/Object;");
                env->SetObjectField(config, pWindowRemote, (jobject)para.pWindowRemote);
            }

            if ( NULL != para.pWindowLocal )
            {
                jfieldID pWindowLocal =  env->GetFieldID(envClass, "pWindowLocal", "Ljava/lang/Object;");
                env->SetObjectField(config, pWindowLocal, (jobject)para.pWindowLocal);
            }
        }
        break;

        case ME_VIDEO_PROCES_CFG_MODULE_ID:
        {
            ME_video_proces_module_t para;
            memset(&para, 0, sizeof(para));
             
            ret = g_vigo->get_config(ME_VIDEO_RENDER_CFG_MODULE_ID, &para);
            if (ret != 0 )
            {
                /* note: always should break for release global reference(envClass) */
                break;
            }
            
            jfieldID deflickingEnable = env->GetFieldID(envClass, "deflickingEnable", "Z");
            jfieldID denoisingEnable = env->GetFieldID(envClass, "denoisingEnable", "Z");
            jfieldID enhancementEnable = env->GetFieldID(envClass, "enhancementEnable", "Z");

            env->SetBooleanField(config, deflickingEnable, para.deflickingEnable);
            env->SetBooleanField(config, denoisingEnable, para.denoisingEnable);
            env->SetBooleanField(config, enhancementEnable, para.enhancementEnable);               
        }

        default:
            break;
    }

    if (envClass)
    {
        env->DeleteGlobalRef(envClass);
        envClass = NULL;
    }

    LOGI("vieGetConfig: ret = %d", ret);
    return ret;
}


extern void vigo_event_cb(int ev_type, int ev_reason, const char* something, void* param, int size);
extern void vigo_send_msg_cb(int media_type, int data_type, const void* data, int len);
extern void vigo_trace_log_cb(char* summary, char* detail, enum eME_TraceLevel level);

static ME_cb_vtable_t mCallbacks = {
	vigo_event_cb,
	vigo_trace_log_cb,	
	NULL,
};

jclass ViGoCBClass 		 	= NULL;
jobject ViGoCBObject 		= NULL;
jmethodID eventMethod 	 	= NULL;
jmethodID sendMsgMethod  	= NULL;
jmethodID traceLogMethod 	= NULL;
extern "C" void Java_com_gl_softphone_ViGoManager_Callbacks(JNIEnv* env
		,jobject thiz
		,jobject cb_obj)
{
	LOGI("ViGoManager_Callbacks IN");

	if (cb_obj)
	{
		ViGoCBObject = env->NewGlobalRef(cb_obj);
		ViGoCBClass = (jclass)env->NewGlobalRef(env->GetObjectClass(cb_obj));
		eventMethod = env->GetMethodID(ViGoCBClass, "eventCallback", "(IILjava/lang/String;Ljava/lang/String;)V");
		traceLogMethod = env->GetMethodID(ViGoCBClass, "sendCallback", "(II[BI)V");
		sendMsgMethod = env->GetMethodID(ViGoCBClass, "traceCallback", "(Ljava/lang/String;Ljava/lang/String;I)V");

		if(g_vigo)
		{
			g_vigo->callback_vtable(&mCallbacks);
		}
	}
	LOGI("ViGoManager_Callbacks OUT");
}

void vigo_event_cb(int ev_type, int ev_reason, const char* something, void* param, int size)
{
	if(ViGoCBObject)
	{
		JNIEnv *env = 0;
		bool attached  = false;
		switch(jvm->GetEnv((void**)&env, JNI_VERSION_1_4))
		{
			case JNI_OK:
				break;
			case JNI_EDETACHED:
	            if (jvm->AttachCurrentThread(&env, NULL)!=0)
	            {
	                LOGE("vigo_event_cb: cannot attach VM");
	                return;
	            }
	            attached = true;
	            break;
	        case JNI_EVERSION:
	            LOGE("vigo_event_cb: JNI_EVERSION error");
	            return;
		}
		ME_event_node_t* pcallenv = NULL;
		pcallenv = (ME_event_node_t*)something;
		json_t *event_json	= NULL;
		json_t *jtemp		= NULL;
		char strcmp[64]		= {0};
		char* event_buf		= NULL;
		if(pcallenv != NULL)
		{
			event_json = jsonapi_new_value(JSON_OBJECT, NULL);
		
		
    		sprintf(strcmp, "%d", pcallenv->ev_type);
    		jtemp = jsonapi_new_value(JSON_NUMBER, strcmp);
    		jsonapi_append_value(event_json,"ev_type",jtemp);

    		sprintf(strcmp, "%d", pcallenv->ev_reason);
    		jtemp = jsonapi_new_value(JSON_NUMBER, strcmp);
    		jsonapi_append_value(event_json,"ev_reason",jtemp);

    		jtemp = jsonapi_new_value(JSON_STRING, pcallenv->something);
    		jsonapi_append_value(event_json,"something",jtemp);

    		jsonapi_value_to_string(event_json, &event_buf);
		}
        
    	const char* constSomething = (const char *)something;
    	LOGI("vigo_event_cb: %s", constSomething);
    	env->CallVoidMethod(ViGoCBObject
						,eventMethod
						,ev_type
						,ev_reason
						,constSomething ? env->NewStringUTF(constSomething) : NULL
						,event_buf ? env->NewStringUTF(event_buf) : NULL
						);
    	if(event_buf != NULL)	
    	{
    	    free(event_buf);
    	}
    	if (attached)
    	{
    		jvm->DetachCurrentThread();
    	}
    }
}

void vigo_trace_log_cb(char* summary,char* detail, enum eME_TraceLevel level)
{
	if (ViGoCBObject)
	{
		JNIEnv *env = 0;
		bool attached = false;
		switch (jvm->GetEnv((void**)&env, JNI_VERSION_1_4))
		{
			case JNI_OK:
				break;
			case JNI_EDETACHED:
				if (jvm->AttachCurrentThread(&env, NULL)!=0)
				{
					LOGE("vigo_trace_log_cb: cannot attach VM");
					return;
				}
				attached = true;
				break;
			case JNI_EVERSION:
				LOGE("vigo_trace_log_cb: JNI_EVERSION error");
				return;
		}

		LOGI("vigo_trace_log_cb: %s", detail);
		env->CallVoidMethod(ViGoCBObject
							,traceLogMethod
							,summary ? env->NewStringUTF(summary) : NULL
							,detail ? env->NewStringUTF(detail) : NULL
							,level
						   );
		if (attached)
		{
			jvm->DetachCurrentThread();
		}
	}
}


void vigo_send_msg_cb(int media_type, int data_type, const void* data, int len)
{
	if (ViGoCBObject)
	{
		JNIEnv *env = 0;
		bool attached = false;
		
		switch (jvm->GetEnv((void**)&env, JNI_VERSION_1_4))
		{
			case JNI_OK:
				break;
			case JNI_EDETACHED:
				if (jvm->AttachCurrentThread(&env, NULL)!=0)
				{
					LOGE("vigo_send_msg_cb: cannot attach VM");
					return;
				}
				attached = true;
				break;
			case JNI_EVERSION:
				LOGE("vigo_send_msg_cb: JNI_EVERSION error");
				return;
		}

		LOGI("vigo_send_msg_cb: in......");
		
		/*new char array*/
		const char* msg =(const char *)data;
		jbyteArray array = env->NewByteArray(len);
		jbyte *pArray;
		int i;
		  
		if(array == NULL)
		{  
			LOGE("vigo_send_msg_cb: NewCharArray error.");  
			return;   
		}  

		/*create jbyte buf*/
		pArray = (jbyte*)calloc(len, sizeof(jbyte));  
		if(pArray == NULL)
		{  
			LOGE("vigo_send_msg_cb: calloc error.");	
			return;   
		}  
	  
		/*copy buffer to jbyte array*/	
		for(i = 0; i < len; i++)  
		{  
			*(pArray + i) = *(msg + i);
			/*(pArray + i) = (signed char)(*(data + i));*/
			//	 __android_log_print(ANDROID_LOG_INFO, "*WEBRTCN*", "jni msg %s",*(pArray + i));
		}  
	
		//pArray[strlen(data)]= '\0';
		/*copy buffer to jcharArray*/  
		env->SetByteArrayRegion(array,0,len,pArray);

		/*invoke java callback method */ 
		env->CallVoidMethod(ViGoCBObject,
							sendMsgMethod,
							media_type,
							data_type,
							array,
							len);
		/*release resource */ 
		if(array)
		{
			env->DeleteLocalRef(array);
		}
		if(pArray)
		{
			free(pArray);  
			pArray = NULL;
		}
		LOGI("vigo_send_msg_cb: out......");
		
		if (attached)
		{
			jvm->DetachCurrentThread();
		}
	}
}

#endif

