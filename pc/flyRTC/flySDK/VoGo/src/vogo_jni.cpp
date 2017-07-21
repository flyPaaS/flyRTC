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
#include "VoGoEngine.h"

//#include "pcp_parser.h"
#include<bson/bson.h>
#include<json/json_api.h>
#include<json/json.h>

using namespace gl_media_engine;


#define TAG    "*VOGO*"
#define LOGI(...) \
    if ( g_debugflag ) \
        ((void)__android_log_print(ANDROID_LOG_INFO,  TAG, __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN,  TAG, __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__))

static JavaVM *jvm=0;
static gl_media_engine::MediaEngineInterface* g_vogo = NULL;
static int g_debugflag = 0;

JNIEXPORT jint JNICALL  JNI_OnLoad(JavaVM *ajvm, void *reserved)
{
#ifdef ANDROID
    ms_set_jvm(ajvm);
//          JNI_OnLoad_SetWebRTC_JVM(ajvm,reserved);
#endif /*ANDROID*/
    jvm=ajvm;
    return JNI_VERSION_1_4;
}

extern "C" jint Java_com_gl_softphone_VoGoManager_voeLoadMediaEngine(JNIEnv*  env
        ,jobject  thiz
        ,jint mode)
{

#ifdef VIGO_ENABLED
    g_vogo = new ViGoEngine();
#else
    g_vogo = new VoGoEngine();
#endif

    if (g_vogo == NULL)
    {
        LOGE("voeLoadMediaEngine: g_vogo is null, mode[%d]", mode);
        return -1;
    }

    return 0;
}

extern "C" void Java_com_gl_softphone_VoGoManager_voeSetAndroidContext(JNIEnv*  env
        ,jobject  thiz
        ,jobject context)
{
    media_jni_env_pm_t cfg;

    LOGI("voeSetAndroidContext start");
    if (g_vogo)
    {
        cfg.jvm     = jvm;
        cfg.env     = env;
        cfg.context = context;
        g_vogo->set_android_obj(&cfg);

        LOGI("voeSetAndroidContext Success!");
    }
}

/*
extern "C" jint Java_com_gl_softphone_VoGoManager_setAndroidAPILevel(JNIEnv* env
        ,jobject thiz
        ,jint apilevel)
{
    int ret = -1;
    if(g_vogo)
    {
        ret =g_vogo->set_android_api_level(apilevel);
    }

    if(g_debugflag)
    {
        __android_log_print(ANDROID_LOG_ERROR, TAG, "VOGOsetAndroidAPILevel ret=%d", ret);
    }
    return ret;
}
*/

extern "C" jint Java_com_gl_softphone_VoGoManager_voeInit(JNIEnv * env
        ,jobject thiz)
{
    int ret = -1;

    if(g_vogo)
    {
        ret = g_vogo->init(0);

        LOGI("voeInit  ret =%d", ret);
    }

    return ret;
}

extern "C" jint Java_com_gl_softphone_VoGoManager_voeDestroy(JNIEnv* env
        ,jobject thiz)
{
    int ret = -1;

    if(g_vogo)
    {
        ret = g_vogo->destroy();
    }

    LOGI("voeDestroy is ok. ret = %d ",ret);
    
    return ret;
}

extern "C" jint Java_com_gl_softphone_VoGoManager_voeCreateAudioStream(JNIEnv* env
        ,jobject thiz)
{
    int ret = -1;

    if(g_vogo)
    {
        ret = g_vogo->create_audio_stream();
    }
    
    LOGI("voeCreateAudioStream: ret =%d", ret);
    return ret;
}

extern "C" jint Java_com_gl_softphone_VoGoManager_voeDeleteAudioStream(JNIEnv*  env
        ,jobject  thiz)
{
    int ret = -1;

    if(g_vogo)
    {
        ret = g_vogo->delete_audio_stream();
    }
    
    LOGI("voeDeleteAudioStream: ret=%d", ret);
    return ret;
}

extern "C" jint Java_com_gl_softphone_VoGoManager_voeSetAudioStream(JNIEnv*  env
        ,jobject  thiz
        ,jobject para)
{
    int ret = -1;
    ME_audio_stream_info_t audioInfo;
    jclass audioInfoClass = NULL;
    int playload = -1;
    jstring remoteip_string = NULL;
    int remote_port = -1;
    int local_port = -1;
    bool exTransportEnable;

    LOGI("voeSetAudioStream: begin");

    if ( NULL == g_vogo || NULL == para )
    {
        LOGE("voeSetAudioStream: param is NULL.");
        return -1;
    }
    
    memset(&audioInfo, 0, sizeof(audioInfo));

    audioInfoClass = (jclass) env->NewGlobalRef(env->GetObjectClass(para));
    jfieldID playloadID = env->GetFieldID(audioInfoClass, "playload", "I");
    jfieldID remoteipID = env->GetFieldID(audioInfoClass, "remote_ip", "Ljava/lang/String;");
    jfieldID remoteportID = env->GetFieldID(audioInfoClass, "remote_port", "I");
    jfieldID localportID = env->GetFieldID(audioInfoClass, "local_port", "I");
    jfieldID ucExTransportEnableID = env->GetFieldID(audioInfoClass,"ucExTransportEnable","Z");

    playload = env->GetIntField(para, playloadID);
    remoteip_string = (jstring) env->GetObjectField(para, remoteipID);
    const char* remoteip_char = remoteip_string?env->GetStringUTFChars(remoteip_string, NULL):NULL;
    remote_port = env->GetIntField(para, remoteportID);
    local_port = env->GetIntField(para, localportID);
    exTransportEnable = env->GetBooleanField(para, ucExTransportEnableID);

    audioInfo.iPayLoadType = playload;
    audioInfo.iRemoteAudioPort = remote_port;
    audioInfo.iLocalAudioPort = local_port;
    audioInfo.ucExTransportEnable = exTransportEnable;
    if(remoteip_char)
    {
        strcpy(audioInfo.cRemoteAudioIp,remoteip_char);
        env->ReleaseStringUTFChars(remoteip_string, remoteip_char);
    }

    LOGI("voeSetAudioStream: audioInfo = { pt = %d, rp = %d lp = %d rip = %s }", \
		audioInfo.iPayLoadType, audioInfo.iRemoteAudioPort,\
        audioInfo.iLocalAudioPort, audioInfo.cRemoteAudioIp);

    ret = g_vogo->set_audio_stream(&audioInfo);

    if(audioInfoClass)
    {
        env->DeleteGlobalRef(audioInfoClass);
        audioInfoClass = NULL;
    }
    
    LOGI("voeSetAudioStream: ret =%d",ret);   
    
    return ret;
}


extern "C" jint Java_com_gl_softphone_VoGoManager_voeEnableAudioPlayout(JNIEnv* env
        ,jobject thiz
        ,jboolean enable)
{
    int ret = -1;

    if(g_vogo)
    {
        ret = g_vogo->enable_audio_playout(enable);
    }

    LOGI("voeEnableAudioPlayout: ret =%d",ret);
    return ret;
}
extern "C" jint Java_com_gl_softphone_VoGoManager_voeEnableAudioSend(JNIEnv* env
        ,jobject thiz
        ,jboolean enable)
{
    int ret = -1;

    if(g_vogo)
    {
        ret = g_vogo->enable_audio_send(enable);
    }

    LOGI("voeEnableAudioSend: ret =%d", ret);
    return ret;
}

extern "C" jint Java_com_gl_softphone_VoGoManager_voeEnableAudioReceive(JNIEnv* env
        ,jobject thiz
        ,jboolean enable)
{
    int ret = -1;

    if(g_vogo)
    {
        ret = g_vogo->enable_audio_receive(enable);
    }

    LOGI("voeEnableAudioReceive: ret =%d", ret);
    return ret;
}

extern "C" jint Java_com_gl_softphone_VoGoManager_voeGetState(JNIEnv*  env
        ,jobject  thiz)
{
    int ret =-1;

    if(g_vogo)
    {
        ret = g_vogo->get_state();
    }

    LOGI("voeGetState: ret =%d", ret);
    return ret;
}

extern "C" void Java_com_gl_softphone_VoGoManager_voeSetState(JNIEnv*  env
        ,jobject  thiz
        ,jint state)
{
    if(g_vogo)
    {
        g_vogo->set_state(state);
    }

    LOGI("voeSetState: state = %d", state);
}

extern "C" jint Java_com_gl_softphone_VoGoManager_voeSendDTMF(JNIEnv* env
        ,jobject thiz
        , jchar dtmf)
{
    int ret = -1;

    if(g_vogo)
    {
        ret = g_vogo->send_DTMF(dtmf);
    }

    LOGI("voeSendDTMF: dtmf =%c, ret=%d", dtmf, ret);
    return ret;
}

extern "C" jint Java_com_gl_softphone_VoGoManager_voeSetMicMute(JNIEnv* env
        ,jobject thiz, jboolean enable)
{
    int ret = -1;
    if(g_vogo)
    {
        ret = g_vogo->set_mic_mute(enable);
    }

    LOGI("voeSetMicMute: ret=%d", ret);

    return ret;
}

extern "C" jboolean Java_com_gl_softphone_VoGoManager_voeGetMicMute(JNIEnv* env
    , jobject thiz)
{
    int ret = 0;
    if ( g_vogo )
    {
        ret = g_vogo->get_mic_mute();
    }

    LOGI("voeGetMicMute: ret = %d", ret);

    return (jboolean)ret;
}

extern "C" jint Java_com_gl_softphone_VoGoManager_voeSetMicVoulme(JNIEnv* env
        ,jobject thiz
        ,jint volume)
{
    int ret = -1;

    if(g_vogo)
    {
        ret = g_vogo->set_mic_volume(volume);
    }

    LOGI("voeSetMicVoulme: ret=%d", ret);
    return ret;
}
extern "C" jint Java_com_gl_softphone_VoGoManager_voeSetLoudSpeakerStatus(JNIEnv* env
        ,jobject thiz, jboolean enable)
{
    int ret = -1;

    if(g_vogo)
    {
        ret = g_vogo->set_loudspeaker_status(enable);
    }

    LOGI("voeSetLoudSpeakerStatus: ret=%d", ret);
    return ret;
}

extern "C" jboolean Java_com_gl_softphone_VoGoManager_voeGetLoudSpeakerStatus(JNIEnv* env
    , jobject thiz)
{
    int ret = 0;
    if ( g_vogo )
    {
        ret = g_vogo->get_loudspeaker_status();
    }

    LOGI("voeGetLoudSpeakerStatus: ret = %d", ret);
	
    return (jboolean)ret;
}

extern "C" jint Java_com_gl_softphone_VoGoManager_voeSetSpeakerVolume(JNIEnv* env
        ,jobject thiz
        ,jint volume)
{
    int ret = -1;
    if(g_vogo)
    {
        ret = g_vogo->set_speaker_volume(volume);
    }

    LOGI("voeSetSpeakerVolume: ret=%d", ret);
    return ret;
}

extern "C" jstring Java_com_gl_softphone_VoGoManager_voeGetVersion(JNIEnv* env
        ,jobject thiz)
{
    int ret = -1;
    char version[64]= {0};
    g_vogo->get_version(version);
    jstring string_version = env->NewStringUTF(version);

    LOGI("voeGetVersion: version = %s", version);

    return string_version;

}
extern "C" jint Java_com_gl_softphone_VoGoManager_voeSetLogCfg(JNIEnv* env
        ,jobject thiz
        ,jobject param)
{
    int ret = -1;
    char filepath[256]= {0};

    if ( g_vogo == NULL || param == NULL )
    {
        LOGE("voeSetLogCfg: param is NULL. ");
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

    ret = g_vogo->set_debug_level(iLevel);
    ret = g_vogo->set_log_file_path(filepath);

    if(cfgClass)
    {
        env->DeleteGlobalRef(cfgClass);
        cfgClass = NULL;
    }

    LOGI("voeSetLogCfg: filepath=%s", filepath);

    return ret;
}

extern "C" jint Java_com_gl_softphone_VoGoManager_voePlayFile(JNIEnv* env
        ,jobject thiz
        ,jobject para)
{
    int ret = -1;
    int mode = 0;

    if ( g_vogo == NULL || para == NULL )
    {
        LOGE("voePlayFile: param is NULL. ");
        return -1;
    }
    
    jclass  envClass = (jclass)env->NewGlobalRef(env->GetObjectClass(para));
    jfieldID iFileFormat_id = env->GetFieldID(envClass, "iFileFormat", "I");
    jfieldID iDirect_id = env->GetFieldID(envClass, "iDirect", "I");
    jfieldID iLoop_id = env->GetFieldID(envClass, "iLoop", "I");
    jfieldID mode_id    = env->GetFieldID(envClass, "mode", "I");

    mode = env->GetIntField(para, mode_id);
    int iDirect = env->GetIntField(para, iDirect_id);
    int iFileFormat = env->GetIntField(para, iFileFormat_id);
    int iLoop = env->GetIntField(para, iLoop_id);

    media_file_play_pm_t pt;
    memset(&pt, 0, sizeof(pt));

    pt.iFileFormat  = iFileFormat;
    pt.iDirect      = iDirect;
    pt.iLoop        = iLoop;

    switch(mode)
    {
        case 2:
        {
            jfieldID filepath_id = env->GetFieldID(envClass, "filepath", "Ljava/lang/String;");
            jstring   jfilepath = (jstring)env->GetObjectField(para, filepath_id);
            const char* filePath = jfilepath?env->GetStringUTFChars(jfilepath, NULL):NULL;

            if (filePath)
            {
                strncpy(pt.filepath.filename, filePath, sizeof(pt.filepath.filename)-1);
            }

            ret = g_vogo->play_file(mode, &pt);
            env->ReleaseStringUTFChars(jfilepath, filePath);
        }
        break;

        case 1:
        {
            int data_size = 0;
            int i;
            jboolean *buf = NULL;
            jfieldID datasize_id = env->GetFieldID(envClass, "data_size", "I");
            jfieldID streamdata_id = env->GetFieldID(envClass, "audioData", "[B");

            data_size = env->GetIntField(para, datasize_id);
            jbyteArray  data_array = (jbyteArray)env->GetObjectField(para, streamdata_id);
            jsize dataArray_length = env->GetArrayLength(data_array);
            LOGI("voePlayFile: data_size = %d, dataArray_length = %d",\
                data_size, dataArray_length);
            
            jbyte* arraybody = env->GetByteArrayElements(data_array, NULL);

            /*create jboolean buf, jboolean in c++ is unsigned char */
            buf = (jboolean *)calloc(data_size , sizeof(jboolean));
            if ( buf == NULL )
            {
                LOGE("voePlayFile: calloc error.");
                break;
            }
            
            /*copy jcharArray data into buf*/
            for (i=0; i < data_size; i++)
            {
                *(buf + i) = (jboolean)(*(arraybody + i));
            }

            /*release char array*/
            env->ReleaseByteArrayElements(data_array, arraybody, 0);
            pt.filestream.data = (void *)buf;
            pt.filestream.size = dataArray_length;

            ret = g_vogo->play_file(mode, &pt);
            
            if(buf)
            {
                free(buf);
                buf = NULL;
            }
        }
        break;

        default:
            break;
    }
        
    if(envClass)
    {
        env->DeleteGlobalRef(envClass);
        envClass = NULL;
    }

    LOGI("voePlayFile ret = %d",ret);   

    return ret;
}

extern "C" jint Java_com_gl_softphone_VoGoManager_voeStopFile(JNIEnv* env
        ,jobject thiz)
{
    int ret = -1;

    if(g_vogo)
    {
        ret = g_vogo->stop_file();
    }

    return ret;
}

extern "C" jint Java_com_gl_softphone_VoGoManager_voeStartRecord(JNIEnv*  env
        ,jobject  thiz, jobject para)
{
    int ret = -1;

    if ( para == NULL )
    {
        LOGE("voeStartRecord: para is null.");
        return ret;
    }

    jclass  envClass = (jclass)env->NewGlobalRef(env->GetObjectClass(para));
    jfieldID filepath_id = env->GetFieldID(envClass, "filePath", "Ljava/lang/String;");
    jfieldID iFileFormat_id = env->GetFieldID(envClass, "iFileFormat", "I");
    jstring   jfilepath = (jstring)env->GetObjectField(para,filepath_id);
    int iFileFormat = env->GetIntField(para,iFileFormat_id);
    const char* filePath = jfilepath?env->GetStringUTFChars(jfilepath, NULL):NULL;

    if (filePath)
    {
        media_file_record_pm_t pt;
        memset(&pt, 0, sizeof(pt));
        strncpy(pt.filepath, filePath, sizeof(pt.filepath)-1);

        pt.iFileFormat = iFileFormat;
        ret = g_vogo->record_start(&pt);
    }

    env->ReleaseStringUTFChars(jfilepath, filePath);

    if ( envClass )
    {
        env->DeleteGlobalRef(envClass);
        envClass = NULL;
    }

    return ret;
}

extern "C" jint Java_com_gl_softphone_VoGoManager_voeStopRecord(JNIEnv*  env
        ,jobject  thiz)
{
    int ret = -1;
    ret = g_vogo->record_stop();
    return ret;
}


static int convert_emodel_value_to_string(ME_emodel_info_t* emodel)
{
    char tmp_buff[128] = {0};

    if (emodel == NULL )
        return -1;

    sprintf(tmp_buff, "M=\"%.2lf,%.2lf,%.2lf\"", \
        emodel->emodel_mos.min, emodel->emodel_mos.max, \
        emodel->emodel_mos.average);
    LOGI("EMODEL: %s", tmp_buff);

    memset(&tmp_buff, 0x00, sizeof(tmp_buff));
    sprintf(tmp_buff, "T=\"%.1lf,%.1lf,%.1lf\"", \
        emodel->emodel_tr.min, emodel->emodel_tr.max, \
        emodel->emodel_tr.average);
    LOGI("%s", tmp_buff);

    memset(&tmp_buff, 0x00, sizeof(tmp_buff));
    sprintf(tmp_buff, "P=\"%.2lf,%.2lf,%.2lf\"", \
        emodel->emodel_ppl.min, emodel->emodel_ppl.max, \
        emodel->emodel_ppl.average);
    LOGI("%s", tmp_buff);

    memset(&tmp_buff, 0x00, sizeof(tmp_buff));
    sprintf(tmp_buff, "B=\"%.2lf,%.2lf,%.2lf\"", \
        emodel->emodel_burstr.min, emodel->emodel_burstr.max, \
        emodel->emodel_burstr.average);
    LOGI("%s", tmp_buff);

    memset(&tmp_buff, 0x00, sizeof(tmp_buff));
    sprintf(tmp_buff, "I=\"%.1lf,%.1lf,%.1lf\"", \
        emodel->emodel_ie.min, emodel->emodel_ie.max, \
        emodel->emodel_ie.average);
    LOGI("%s", tmp_buff);

    memset(&tmp_buff, 0x00, sizeof(tmp_buff));
    sprintf(tmp_buff, "D=\"%.2lf, %.2lf, %.2lf\"", \
        emodel->delay.min, emodel->delay.max,
        emodel->delay.average);
    LOGI("%s", tmp_buff);

    memset(&tmp_buff, 0x00, sizeof(tmp_buff));
    sprintf(tmp_buff, "J=\"%.2lf, %.2lf, %.2lf\"", \
        emodel->jitbuf.min, emodel->jitbuf.max, \
        emodel->jitbuf.average);
    LOGI("%s", tmp_buff);
     
    return 0;
}

extern "C" jint Java_com_gl_softphone_VoGoManager_voeGetEmodelValue(JNIEnv*  env
        ,jobject  thiz
        ,jobject mos
        ,jobject tr
        ,jobject ppl
        ,jobject burstr
        ,jobject ie)
{
    ME_emodel_info_t ev;

    jclass mosClass = NULL;
    jclass trClass = NULL;
    jclass pplClass = NULL;
    jclass burstrClass = NULL;
    jclass ieClass = NULL;

    memset(&ev, 0x00, sizeof(ME_emodel_info_t));
    int ret = g_vogo->get_emodel_calc_value(&ev);
    if (ret != 0)
    {
        LOGE("voeGetEmodelValue: get emodel failed.");
        return -1;
    }

    convert_emodel_value_to_string(&ev);

    if (mos != NULL)
    {
        mosClass = (jclass)env->NewGlobalRef(env->GetObjectClass(mos));

        jfieldID mosFlag_id=env->GetFieldID(mosClass, "flag", "I");
        jfieldID mosCount_id=env->GetFieldID(mosClass, "count", "I");
        jfieldID mosTotal_id=env->GetFieldID(mosClass, "total", "D");
        jfieldID mosAverage_id=env->GetFieldID(mosClass, "average", "D");
        jfieldID mosMin_id=env->GetFieldID(mosClass, "min", "D");
        jfieldID mosMax_id=env->GetFieldID(mosClass, "max", "D");
        jfieldID mosCurrent_id=env->GetFieldID(mosClass, "current", "D");


        env->SetIntField(mos,mosFlag_id,(int)ev.emodel_mos.flag);
        env->SetIntField(mos,mosCount_id,(int)ev.emodel_mos.count);
        env->SetDoubleField(mos,mosTotal_id,(double)ev.emodel_mos.total);
        env->SetDoubleField(mos,mosAverage_id,(double)ev.emodel_mos.average);
        env->SetDoubleField(mos,mosMin_id,(double)ev.emodel_mos.min);
        env->SetDoubleField(mos,mosMax_id,(double)ev.emodel_mos.max);
        env->SetDoubleField(mos,mosCurrent_id,(double)ev.emodel_mos.current);
    }

    if (tr != NULL)
    {
        trClass = (jclass)env->NewGlobalRef(env->GetObjectClass(tr));

        jfieldID trFlag_id=env->GetFieldID(trClass, "flag", "I");
        jfieldID trCount_id=env->GetFieldID(trClass, "count", "I");
        jfieldID trTotal_id=env->GetFieldID(trClass, "total", "D");
        jfieldID trAverage_id=env->GetFieldID(trClass, "average", "D");
        jfieldID trMin_id=env->GetFieldID(trClass, "min", "D");
        jfieldID trMax_id=env->GetFieldID(trClass, "max", "D");
        jfieldID trCurrent_id=env->GetFieldID(trClass, "current", "D");

        env->SetIntField(tr,trFlag_id,(int)ev.emodel_tr.flag);
        env->SetIntField(tr,trCount_id,(int)ev.emodel_tr.count);
        env->SetDoubleField(tr,trTotal_id,(double)ev.emodel_tr.total);
        env->SetDoubleField(tr,trAverage_id,(double)ev.emodel_tr.average);
        env->SetDoubleField(tr,trMin_id,(double)ev.emodel_tr.min);
        env->SetDoubleField(tr,trMax_id,(double)ev.emodel_tr.max);
        env->SetDoubleField(tr,trCurrent_id,(double)ev.emodel_tr.current);
    }

    if (ppl != NULL)
    {
        pplClass = (jclass)env->NewGlobalRef(env->GetObjectClass(ppl));

        jfieldID pplFlag_id=env->GetFieldID(pplClass, "flag", "I");
        jfieldID pplCount_id=env->GetFieldID(pplClass, "count", "I");
        jfieldID pplTotal_id=env->GetFieldID(pplClass, "total", "D");
        jfieldID pplAverage_id=env->GetFieldID(pplClass, "average", "D");
        jfieldID pplMin_id=env->GetFieldID(pplClass, "min", "D");
        jfieldID pplMax_id=env->GetFieldID(pplClass, "max", "D");
        jfieldID pplCurrent_id=env->GetFieldID(pplClass, "current", "D");

        env->SetIntField(ppl,pplFlag_id,(int)ev.emodel_ppl.flag);
        env->SetIntField(ppl,pplCount_id,(int)ev.emodel_ppl.count);
        env->SetDoubleField(ppl,pplTotal_id,(double)ev.emodel_ppl.total);
        env->SetDoubleField(ppl,pplAverage_id,(double)ev.emodel_ppl.average);
        env->SetDoubleField(ppl,pplMin_id,(double)ev.emodel_ppl.min);
        env->SetDoubleField(ppl,pplMax_id,(double)ev.emodel_ppl.max);
        env->SetDoubleField(ppl,pplCurrent_id,(double)ev.emodel_ppl.current);
    }

    if (burstr != NULL)
    {
        burstrClass = (jclass)env->NewGlobalRef(env->GetObjectClass(burstr));

        jfieldID burstrFlag_id=env->GetFieldID(burstrClass, "flag", "I");
        jfieldID burstrCount_id=env->GetFieldID(burstrClass, "count", "I");
        jfieldID burstrTotal_id=env->GetFieldID(burstrClass, "total", "D");
        jfieldID burstrAverage_id=env->GetFieldID(burstrClass, "average", "D");
        jfieldID burstrMin_id=env->GetFieldID(burstrClass, "min", "D");
        jfieldID burstrMax_id=env->GetFieldID(burstrClass, "max", "D");
        jfieldID burstrCurrent_id=env->GetFieldID(burstrClass, "current", "D");

        env->SetIntField(burstr,burstrFlag_id,(int)ev.emodel_burstr.flag);
        env->SetIntField(burstr,burstrCount_id,(int)ev.emodel_burstr.count);
        env->SetDoubleField(burstr,burstrTotal_id,(double)ev.emodel_burstr.total);
        env->SetDoubleField(burstr,burstrAverage_id,(double)ev.emodel_burstr.average);
        env->SetDoubleField(burstr,burstrMin_id,(double)ev.emodel_burstr.min);
        env->SetDoubleField(burstr,burstrMax_id,(double)ev.emodel_burstr.max);
        env->SetDoubleField(burstr,burstrCurrent_id,(double)ev.emodel_burstr.current);
    }

    if (ie != NULL)
    {
        ieClass = (jclass)env->NewGlobalRef(env->GetObjectClass(ie));

        jfieldID ieFlag_id=env->GetFieldID(burstrClass, "flag", "I");
        jfieldID ieCount_id=env->GetFieldID(burstrClass, "count", "I");
        jfieldID ieTotal_id=env->GetFieldID(burstrClass, "total", "D");
        jfieldID ieAverage_id=env->GetFieldID(burstrClass, "average", "D");
        jfieldID ieMin_id=env->GetFieldID(burstrClass, "min", "D");
        jfieldID ieMax_id=env->GetFieldID(burstrClass, "max", "D");
        jfieldID ieCurrent_id=env->GetFieldID(burstrClass, "current", "D");

        env->SetIntField(ie,ieFlag_id,(int)ev.emodel_ie.flag);
        env->SetIntField(ie,ieCount_id,(int)ev.emodel_ie.count);
        env->SetDoubleField(ie,ieTotal_id,(double)ev.emodel_ie.total);
        env->SetDoubleField(ie,ieAverage_id,(double)ev.emodel_ie.average);
        env->SetDoubleField(ie,ieMin_id,(double)ev.emodel_ie.min);
        env->SetDoubleField(ie,ieMax_id,(double)ev.emodel_ie.max);
        env->SetDoubleField(ie,ieCurrent_id,(double)ev.emodel_ie.current);
    }

    if (mosClass)
    {
        env->DeleteGlobalRef(mosClass);
        mosClass = NULL;
    }
    if (trClass)
    {
        env->DeleteGlobalRef(trClass);
        trClass = NULL;
    }

    if (pplClass)
    {
        env->DeleteGlobalRef(pplClass);
        pplClass = NULL;
    }

    if (burstrClass)
    {
        env->DeleteGlobalRef(burstrClass);
        burstrClass = NULL;
    }

    if (ieClass)
    {
        env->DeleteGlobalRef(ieClass);
        ieClass = NULL;
    }

    return 0;
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
        default:
            break;
    }
}

extern "C" jint Java_com_gl_softphone_VoGoManager_voeSetConfig(JNIEnv* env
        ,jobject thiz
        ,jint MethodID
        ,jobject config
        ,jint version)
{
    int ret = -1;
    char moduleName[64] = {0};
    jclass envClass = NULL;

    if ( NULL == g_vogo || NULL == config )
    {
        LOGE("voeSetConfig: param is NULL. ");
        return -1;
    }

    getModuleName(MethodID, moduleName);
    LOGI("voeSetConfig: %s", moduleName);
    
    envClass = (jclass)env->NewGlobalRef(env->GetObjectClass(config));
    
    switch(MethodID)
    {
        case ME_CTRL_CFG_MODULE_ID:
        {
            ME_CTRL_cfg_t ctrl_config;
            unsigned int ucRealTimeType;    //Real time protocol type, 0: RTP 1: PRTP
            unsigned int ucVideoEnable;     //Video module, 0: disable 1: enable
            unsigned int ucEmodelEnable;    //Emodel module, 0: disable 1: enable
            unsigned int ucFecEnable;   //Emodel module, 0: disable 1: enable

            jfieldID ucRealTimeType_id =  env->GetFieldID(envClass, "ucRealTimeType", "I");
            jfieldID ucVideoEnable_id =  env->GetFieldID(envClass, "ucVideoEnable", "I");
            jfieldID ucEmodelEnable_id = env->GetFieldID(envClass, "ucEmodelEnable", "I");
            jfieldID ucFecEnable_id = env->GetFieldID(envClass, "ucFecEnable", "I");

            ucRealTimeType =  env->GetIntField(config,ucRealTimeType_id);
            ucVideoEnable = env->GetIntField(config,ucVideoEnable_id);
            ucEmodelEnable = env->GetIntField(config,ucEmodelEnable_id);
            ucFecEnable = env->GetIntField(config,ucFecEnable_id);

            ctrl_config.ucRealTimeType= ucRealTimeType;
            ctrl_config.ucVideoEnable = ucVideoEnable;
            ctrl_config.ucEmodelEnable = ucEmodelEnable;
            ctrl_config.ucFecEnable = ucFecEnable;

            LOGI("voeSetConfig: ctrl_config = { RT = %d, VE = %d, EE = %d FE = %d }",\
                ucRealTimeType, ucVideoEnable, ucEmodelEnable, ucFecEnable);

            g_vogo->enable_audio_fec(ucFecEnable);
            ret = g_vogo->set_config(ME_CTRL_CFG_MODULE_ID, &ctrl_config);
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
            
            LOGI("voeSetConfig: vqe_cfg={Ec = %d, Agc = %d, Ns = %d, RAgc = %d, RNs = %d}", \
                Ec_enable, Agc_enable, Ns_enable, Agc_Rx_enable, Ns_Rx_enable);

            ret = g_vogo->set_config(ME_VQE_CFG_MODULE_ID, &vqe_config);
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
            
            LOGI("voeSetConfig: rtpCfg = { timeout =%d, fixlowBitrate = %d }", \
                uiRTPTimeout, uiFixLowPayload);

            ret = g_vogo->set_config(ME_RTP_CFG_MODULE_ID, &rtp_config);
        }
        break;
        
        case ME_ENV_CFG_MODULE_ID:
        {
            ME_ENV_cfg_t envcfg;
            char status;
            char dialogScene;
            int networktype;
            jfieldID status_id = env->GetFieldID(envClass, "status", "Z");
            jfieldID dialogScene_id = env->GetFieldID(envClass, "dialogScene", "I");
            jfieldID networktype_id = env->GetFieldID(envClass, "networktype", "I");
            status = env->GetBooleanField(config,status_id);
            dialogScene = env->GetIntField(config,dialogScene_id);
            networktype = env->GetIntField(config,networktype_id);
            envcfg.networktype  = networktype;
            envcfg.status       = status;
            envcfg.dialogScene       = dialogScene;
            
            ret= g_vogo->set_config(ME_ENV_CFG_MODULE_ID, &envcfg);

             LOGI("voeSetConfig: env status = %c", status);
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

    LOGI("voeSetConfig: return ret = %d", ret);
    return ret;
}

extern "C" jint Java_com_gl_softphone_VoGoManager_voeGetConfig(JNIEnv* env
        ,jobject thiz
        ,jint MethodID
        ,jobject config
        ,jint version)
{
    int ret = -1;
    char moduleName[64] = {0};
    jclass envClass = NULL;

    if ( NULL == g_vogo || NULL == config )
    {
        LOGE("voeGetConfig: param is NULL. ");
        return -1;
    }

    getModuleName(MethodID, moduleName);
    LOGI("voeGetConfig: %s", moduleName);
    
    envClass = (jclass)env->NewGlobalRef(env->GetObjectClass(config));
    
    switch(MethodID)
    {
        case ME_CTRL_CFG_MODULE_ID:
        {
            ME_CTRL_cfg_t ctrl_config;

            memset(&ctrl_config, 0x00, sizeof(ctrl_config));
            ret = g_vogo->get_config(ME_CTRL_CFG_MODULE_ID, &ctrl_config);
            if (ret != 0)
            {
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
            
            LOGI("voeGetConfig: ctrl_cfg={%d, %d, %d, %d}", \
                ctrl_config.ucRealTimeType, ctrl_config.ucVideoEnable, \
                ctrl_config.ucEmodelEnable, ctrl_config.ucFecEnable);
        }
        break;
        
        case ME_VQE_CFG_MODULE_ID:
        {
            ME_VQE_cfg_t vqe_config;

            memset(&vqe_config, 0x00, sizeof(vqe_config));
            ret = g_vogo->get_config(ME_VQE_CFG_MODULE_ID, &vqe_config);
            if (ret != 0)
            {
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
            LOGI("voeGetConfig: vqe_cfg={%d, %d, %d, %d, %d}",\
                vqe_config.Ec_enable, vqe_config.Agc_enable,\
                vqe_config.Ns_enable, vqe_config.Agc_Rx_enable, \
                vqe_config.Ns_Rx_enable);
        }
        break;
        
        case ME_RTP_CFG_MODULE_ID:
        {
            ME_RTP_cfg_t rtp_config;

            memset(&rtp_config, 0x00, sizeof(rtp_config));
            ret = g_vogo->get_config(ME_RTP_CFG_MODULE_ID, &rtp_config);
            if (ret != 0)
            {
                break;
            }

            jfieldID uiRTPTimeout_id =  env->GetFieldID(envClass, "uiRTPTimeout", "I");
            jfieldID uiFixLowPayload_id =  env->GetFieldID(envClass, "uiFixLowPayload", "Z");
            env->SetIntField(config,uiRTPTimeout_id,rtp_config.uiRTPTimeout);
            env->SetBooleanField(config,uiFixLowPayload_id,rtp_config.uiFixLowPayload);
            LOGI("voeGetConfig: Rtp_cfg={%d, %d}", \
                rtp_config.uiRTPTimeout, rtp_config.uiFixLowPayload);
        }
        break;
        
        case ME_ENV_CFG_MODULE_ID:
        {
            ME_ENV_cfg_t env_st;

            memset(&env_st, 0x00, sizeof(ME_ENV_cfg_t));
            ret = g_vogo->get_config(ME_ENV_CFG_MODULE_ID, &env_st);

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

            LOGI("voeGetConfig: env_cfg={%d,%d,%d}", \
                env_st.status, env_st.dialogScene, env_st.networktype);
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

    LOGI("voeGetConfig: return ret = %d", ret);
    return ret;
}

extern void vogo_event_cb(int ev_type, int ev_reason, const char* something, void* param, int size);
extern void vogo_send_msg_cb(int media_type, int data_type, const void* data, int len);
extern void vogo_trace_log_cb(char* summary, char* detail, enum eME_TraceLevel level);

static ME_cb_vtable_t mCallbacks =
{
    vogo_event_cb,
    vogo_trace_log_cb,
    NULL,
};

jclass VoGoCBClass          = NULL;
jobject VoGoCBObject        = NULL;
jmethodID eventMethod       = NULL;
jmethodID sendMsgMethod     = NULL;
jmethodID traceLogMethod    = NULL;

extern "C" void Java_com_gl_softphone_VoGoManager_Callbacks(JNIEnv* env
        ,jobject thiz
        ,jobject cb_obj)
{
    LOGI("VoGoManager_Callbacks IN");

    if(cb_obj)
    {
        VoGoCBObject = env->NewGlobalRef(cb_obj);
        VoGoCBClass = (jclass)env->NewGlobalRef(env->GetObjectClass(cb_obj));
        eventMethod = env->GetMethodID(VoGoCBClass, "eventCallback", "(IILjava/lang/String;Ljava/lang/String;)V");
        traceLogMethod = env->GetMethodID(VoGoCBClass, "sendCallback", "(II[BI)V");
        sendMsgMethod = env->GetMethodID(VoGoCBClass, "traceCallback", "(Ljava/lang/String;Ljava/lang/String;I)V");

        if(g_vogo)
        {
            g_vogo->callback_vtable(&mCallbacks);
        }
    }
    LOGI("VoGoManager_Callbacks OUT");
}

void vogo_event_cb(int ev_type, int ev_reason, const char* something, void* param, int size)
{
    if(VoGoCBObject)
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
                    LOGE("vogo_event_cb: cannot attach VM");
                    return;
                }
                attached = true;
                break;
            case JNI_EVERSION:
                LOGE("vogo_event_cb: JNI_EVERSION error");
                return;
        }
        ME_event_node_t* pcallenv = NULL;
        pcallenv = (ME_event_node_t*)something;
        json_t *event_json  = NULL;
        json_t *jtemp       = NULL;
        char strcmp[64]     = {0};
        char* event_buf     = NULL;
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
        LOGI("vogo_event_cb: %s", constSomething);
        
        env->CallVoidMethod(VoGoCBObject
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
void vogo_trace_log_cb(char* summary,char* detail, enum eME_TraceLevel level)
{
    if (VoGoCBObject)
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
                    LOGE("vogo_trace_log_cb: cannot attach VM");
                    return;
                }
                attached = true;
                break;
            case JNI_EVERSION:
                LOGE("vogo_trace_log_cb: JNI_EVERSION error");
                return;
        }

        LOGI("vogo_trace_log_cb: %s", detail);
        env->CallVoidMethod(VoGoCBObject
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


void vogo_send_msg_cb(int media_type, int data_type, const void* data, int len)
{
    if (VoGoCBObject)
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
                    LOGE("vogo_send_msg_cb: cannot attach VM");
                    return;
                }
                attached = true;
                break;
            case JNI_EVERSION:
                LOGE("vogo_send_msg_cb: JNI_EVERSION error");
                return;
        }

        LOGI("vogo_send_msg_cb: in......");

        /*new char array*/
        const char* msg =(const char *)data;
        jbyteArray array = env->NewByteArray(len);
        jbyte *pArray;
        int i;

        if(array == NULL)
        {
            LOGE("vogo_send_msg_cb: NewCharArray error.");
            return;
        }

        /*create jbyte buf*/
        pArray = (jbyte*)calloc(len, sizeof(jbyte));
        if(pArray == NULL)
        {
            LOGE("vogo_send_msg_cb: calloc error.");
            return;
        }

        /*copy buffer to jbyte array*/
        for(i = 0; i < len; i++)
        {
            *(pArray + i) = *(msg + i);
            /*(pArray + i) = (signed char)(*(data + i));*/
            //   __android_log_print(ANDROID_LOG_INFO, "*WEBRTCN*", "jni msg %s",*(pArray + i));
        }

        //pArray[strlen(data)]= '\0';
        /*copy buffer to jcharArray*/
        env->SetByteArrayRegion(array,0,len,pArray);

        /*invoke java callback method */
        env->CallVoidMethod(VoGoCBObject,
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
        LOGI("vogo_send_msg_cb: out......");

        if (attached)
        {
            jvm->DetachCurrentThread();
        }
    }
}

#endif

