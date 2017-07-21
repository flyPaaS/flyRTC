#ifdef ANDROID
#include <jni.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <android/log.h>
#include "msjava.h"


#include <sys/socket.h>
#include <netinet/in.h>
#include "ugo.h"
#include "ugo_test.h"

#ifdef AUTO_TEST_SWITCH
#include "ugo_auto_test.h"
#endif

#include "webrtc/modules/video_coding/codecs/mediacodec/android/on_load.h"

#include<bson/bson.h>
#include<json/json_api.h>
#include<json/json.h>

static JavaVM *jvm=0;
static int g_debugflag = 0;

#define TAG       "*UGO*"

#define LOGI(...) \
    if ( g_debugflag ) \
        ((void)__android_log_print(ANDROID_LOG_INFO,  TAG, __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN,  TAG, __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__))

//#define LOGT(...) __android_log_print(ANDROID_LOG_ERROR, "testcodec", __VA_ARGS__)

JNIEXPORT jint JNICALL  JNI_OnLoad(JavaVM *ajvm, void *reserved)
{    
    ms_set_jvm(ajvm);

#ifdef VIGO_ENABLED
	JNI_OnLoad_SetWebRTC_JVM(ajvm, reserved);
#endif

    jvm = ajvm;
    return JNI_VERSION_1_4;
}

static int convert_emodel_value_to_string(ugo_emodel_info_t* pEmodelInfo)
{
    ME_emodel_info_t* emodel = NULL;
    ugo_call_info_t* pCallInfo = NULL;
    char tmp_buff[128] = {0};
    
    if (emodel == NULL )
        return -1;

    emodel = &(pEmodelInfo->emodel);
    pCallInfo = &(pEmodelInfo->callinfo);

    sprintf(tmp_buff, "M=\"%.2lf,%.2lf,%.2lf\"", \
        emodel->emodel_mos.min, emodel->emodel_mos.max, \
        emodel->emodel_mos.average);
    LOGI("EMODEL: %s", tmp_buff);
    
    memset(&tmp_buff, 0x00, sizeof(tmp_buff));
    sprintf(tmp_buff, "T=\"%.2lf,%.2lf,%.2lf\"", \
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
    sprintf(tmp_buff, "I=\"%.2lf,%.2lf,%.2lf\"", \
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
 
    memset(&tmp_buff, 0x00, sizeof(tmp_buff));
    sprintf(tmp_buff, "C=\"%s %s %s %s %d %d %d %d %d %d %d %d %d %d\"", \
        pCallInfo->callid, \
        pCallInfo->mgw, pCallInfo->sgw, \
        pCallInfo->mcodec, pCallInfo->cmode,\
        pCallInfo->mmode, pCallInfo->ctime, \
        pCallInfo->cstate, pCallInfo->role, \
        pCallInfo->mgwport, pCallInfo->spreason, pCallInfo->ismute, \
        emodel->pkt_stat.pktRecv, emodel->pkt_stat.pktSnd );
    LOGI("%s", tmp_buff);
 
    return 0;
}

extern "C" void Java_com_gl_softphone_UGoManager_setAndroidContext(JNIEnv*  env
        ,jobject  thiz
        ,jobject context)
{
    media_jni_env_pm_t pm;
    pm.jvm      = jvm;
    pm.env      = env;
    pm.context  = context;
 
    UGo_set_android_obj(&pm);
}


extern "C" jint Java_com_gl_softphone_UGoManager_UGoDebugEnabled(JNIEnv*  env
        ,jobject  thiz
        ,jboolean enable
        ,jstring filepath)
{
    int res = 0;
    const char* pFilePath = filepath ? env->GetStringUTFChars(filepath, NULL):NULL;
    
    if ( enable )
    {
        if ( pFilePath != NULL )
        {
            res = UGo_enable_stack_log(pFilePath);
        }
        else
        {
            res = UGo_enable_stack_log("/mnt/sdcard/ugodebug.txt");
        }
        g_debugflag = 1;
    }
    else
    {
        g_debugflag = 0;
    }

    return res;
}

extern "C" jint Java_com_gl_softphone_UGoManager_UGoLoadMediaEngine(JNIEnv*  env
        ,jobject  thiz)
{
    return UGo_load_media_engine();
}

extern "C" jint Java_com_gl_softphone_UGoManager_UGoUnloadMediaEngine(JNIEnv*  env
        ,jobject  thiz)
{
    return UGo_unload_media_engine();
}
extern "C" jint Java_com_gl_softphone_UGoManager_UGoInit(JNIEnv*  env
        ,jobject  thiz)
{
    return UGo_init();
}

extern void releaseJniObject();
extern "C" jint Java_com_gl_softphone_UGoManager_UGoDestroy(JNIEnv*  env
        ,jobject  thiz)
{
    LOGI("UGoDestroy");
    
    int ret = UGo_destroy();
    Java_com_gl_softphone_UGoManager_UGoUnloadMediaEngine(env,thiz);
    releaseJniObject();
    
    return ret;
}

extern "C" jint Java_com_gl_softphone_UGoManager_UGoRegister(JNIEnv*  env
        ,jobject  thiz
        ,jstring uid
        ,jint mode)
{
    int ret = 0;
    const char* char_uid = uid?env->GetStringUTFChars(uid, NULL):NULL;

    LOGI("UGoRigister: uid = %s", char_uid);
    
    ret = ugo_test_login(char_uid, mode);
    if (char_uid) env->ReleaseStringUTFChars(uid, char_uid);
    return ret;
}

extern "C" jint Java_com_gl_softphone_UGoManager_UGoPresetVideo(JNIEnv*  env
        ,jobject  thiz
        ,jstring key
        ,jstring value
        )
{
    int ret = 0;
    if(key == NULL || value == NULL)
	return -1;
    const char* char_key= env->GetStringUTFChars(key, NULL);
    const char* char_value= env->GetStringUTFChars(value, NULL);

    LOGI("UGoPresetVideo: key = %s  value = %s", char_key, char_value);
    
    ret = UGo_video_preset((char*)char_key, (char*)char_value);
    env->ReleaseStringUTFChars(key, char_key);
    env->ReleaseStringUTFChars(value, char_value);
    return ret;
}

extern "C" jint Java_com_gl_softphone_UGoManager_UGoUnRegister(JNIEnv*  env
        ,jobject  thiz)
{
    return ugo_test_logout();
}

extern "C" jint Java_com_gl_softphone_UGoManager_UGoCallPush(JNIEnv *env
    , jobject thiz, IN jobject obj)
{
    ugo_ios_incoming_rsp_pm_t callparam;
    int ret = -1;
    jclass callClass = NULL;    

    if (obj == NULL)
    {
        LOGE("UGoCallPush: obj is null");
        return ret;
    }

    callClass = (jclass)env->NewGlobalRef(env->GetObjectClass(obj));
    jfieldID callid_id = env->GetFieldID(callClass, "callid", "Ljava/lang/String;");
    jfieldID vpsid_id = env->GetFieldID(callClass, "vpsid", "I");
    jfieldID reason_id = env->GetFieldID(callClass, "reason", "I");

    jstring callid_str = (jstring)env->GetObjectField(obj, callid_id);
    const char* callid = callid_str ? env->GetStringUTFChars(callid_str, NULL) : NULL;
    int vpsid = env->GetIntField(obj, vpsid_id);
    int reason = env->GetIntField(obj, reason_id);

    memset(&callparam, 0x00, UGO_IOS_INCOMING_RSP_PM_SIZE);
    callparam.vpsid = vpsid;
    callparam.reason = reason;
    if ( callid )
    {
        strncpy(callparam.callid, callid, sizeof(callparam.callid)-1);
        env->ReleaseStringUTFChars(callid_str, callid);
    }

    ret = UGo_ios_call_incoming_rsp(&callparam);
    if ( callClass )
    {
        env->DeleteGlobalRef(callClass);
        callClass = NULL;
    }

    LOGI("UGoCallPush: callparam = { callid = %s, vpsid = %d, reason = %d }. ret = %d",
        callparam.callid, callparam.vpsid, callparam.reason, ret);
    
    return ret;
}

extern "C" jint Java_com_gl_softphone_UGoManager_UGoDial(JNIEnv*  env
        ,jobject  thiz,IN jobject obj)
{
    ugo_call_dialing_para_t callparam;

    jclass callClass =NULL;
    //jstring fnickname_str;
    jstring uid_string;
    jstring phone_string;
    jstring userdata_str;
    int mode_flag;
    int video_flag;
    int call_type_flag;
    int ret =-1;

    if (obj == NULL)
    {
        LOGE("UGoDial: obj is null");
        return ret;
    }

    callClass = (jclass)env->NewGlobalRef(env->GetObjectClass(obj));
    //jfieldID fnickname_id = env->GetFieldID(callClass, "fnickname", "Ljava/lang/String;");
    jfieldID use_id = env->GetFieldID(callClass, "uid","Ljava/lang/String;");
    jfieldID phone_number = env->GetFieldID(callClass, "phone", "Ljava/lang/String;");
    jfieldID userdata_id = env->GetFieldID(callClass, "userdata", "Ljava/lang/String;");
    jfieldID mode_type = env->GetFieldID(callClass, "mode", "I");
    jfieldID video_flage = env->GetFieldID(callClass, "video_enable", "I");
    /* add by Vinton.Liu on 20140917 for uxin liaoyiliao */
    jfieldID call_type = env->GetFieldID(callClass, "ucalltype", "I");

    //fnickname_str = (jstring)env->GetObjectField(obj, fnickname_id);
    uid_string   = (jstring)env->GetObjectField(obj, use_id);
    phone_string = (jstring)env->GetObjectField(obj, phone_number);
    userdata_str = (jstring)env->GetObjectField(obj, userdata_id);
    mode_flag    = env->GetIntField(obj,mode_type);
    video_flag   = env->GetIntField(obj, video_flage);
    /* add by Vinton.Liu on 20140917 for uxin liaoyiliao */
    call_type_flag = env->GetIntField(obj, call_type);

    //const char* fnickname = fnickname_str ? env->GetStringUTFChars(fnickname_str, NULL) : NULL;
    const char* uid_char = uid_string ? env->GetStringUTFChars(uid_string, NULL) : NULL;
    const char* phone_char = phone_string ? env->GetStringUTFChars(phone_string, NULL) : NULL;
    const char* userdata = userdata_str ? env->GetStringUTFChars(userdata_str, NULL) : NULL;

    memset(&callparam, 0, sizeof(ugo_call_dialing_para_t));
    callparam.call_mode = mode_flag;
    callparam.video_enabled = video_flag;
    /* add by Vinton.Liu on 20140917 for uxin liaoyiliao */
    callparam.calltype = call_type_flag;

#if 0
    if ( fnickname )
    {
        strncpy(callparam.name, fnickname, sizeof(callparam.name)-1);
        env->ReleaseStringUTFChars(fnickname_str, fnickname);
    }
#endif

    if(uid_char)
    {
        strncpy(callparam.tuid, uid_char, sizeof(callparam.tuid) - 1);
        env->ReleaseStringUTFChars(uid_string,uid_char);
    }
    if(phone_char)
    {
        strncpy(callparam.tphone, phone_char, sizeof(callparam.tphone) - 1);
        env->ReleaseStringUTFChars(phone_string,phone_char);
    }

    if ( userdata )
    {
        strncpy(callparam.user_data, userdata, sizeof(callparam.user_data) -1 );
        env->ReleaseStringUTFChars(userdata_str, userdata);
    }

    ret = UGo_call_dialing(&callparam);

    if ( callClass )
    {
        env->DeleteGlobalRef(callClass);
        callClass = NULL;
    }

#if 0
    LOGI("UGoDial: ret = %d. callparam = {%s, %s, %s, %s, %d, %d, %d}", ret, \
        callparam.tuid, callparam.tphone, callparam.name, callparam.user_data, \
        callparam.call_mode, callparam.calltype, callparam.video_enabled);
#else
    LOGI("UGoDial: ret = %d. callparam = {%s, %s, %s, %d, %d, %d}", ret, \
        callparam.tuid, callparam.tphone, callparam.user_data, \
        callparam.call_mode, callparam.calltype, callparam.video_enabled);
#endif
    return ret;
}

extern "C" jint Java_com_gl_softphone_UGoManager_UGoAnswer(JNIEnv*  env
        ,jobject  thiz)
{
    return UGo_call_answered();
}

extern "C" jint Java_com_gl_softphone_UGoManager_UGoHangup(JNIEnv*  env
        ,jobject  thiz, jint reason)
{
    return UGo_call_hangup(reason);
}

extern "C" jstring Java_com_gl_softphone_UGoManager_UGoGetVersion(JNIEnv*  env
        ,jobject  thiz)
{
    char version[64]= {0};
    UGo_get_version(version);
    jstring string_version = env->NewStringUTF(version);
    return string_version;
}

extern "C" jint Java_com_gl_softphone_UGoManager_UGoSetApi(JNIEnv*  env
        ,jobject  thiz
        ,jint  level)
{
    LOGI("UGoSetApi: apilevel = %d",level);
    return UGo_set_api(level);

}
extern "C" jint Java_com_gl_softphone_UGoManager_UGoTcpRecvMsg(JNIEnv*  env
        ,jobject  thiz
        ,jint recvlen
        ,jbyteArray recvbuf)
{
    jbyte *array;
    jboolean *buf;
    int i;
    int res;

    LOGI("UGoTcpRecvMsg msglen = %d", recvlen);
    
    /*get byte array buf */
    array = env->GetByteArrayElements(recvbuf, NULL);
    if(array == NULL)
    {
        LOGE("UGoTcpRecvMsg: GetCharArrayElements error.");
        return -1;
    }

    /*create jboolean buf, jboolean in c++ is unsigned char */
    buf = (jboolean *)calloc(recvlen , sizeof(jboolean));
    if(buf == NULL)
    {
        LOGE("UGoTcpRecvMsg: calloc error.");
        return -1;
    }

    /*copy jcharArray data into buf*/
    for(i=0; i < recvlen; i++)
    {
        *(buf + i) = (jboolean)(*(array + i));
    }

    /*release char array*/
    env->ReleaseByteArrayElements(recvbuf, array, 0);

    res = UGo_tcp_recv_msg(recvlen, (void*)buf);

    /*free buf */
    free(buf);
    buf = NULL;

    return res;
}

extern "C" jint Java_com_gl_softphone_UGoManager_UGoVideoIncomingFrame(JNIEnv*  env
        ,jobject  thiz        
        ,jbyteArray data,jint len)
{
    jbyte *array;
    jboolean *buf;
    int i;
    int res;

    //LOGI("UGoVideoIncomingFrame len = %d", len);
    
    /*get byte array buf */
    array = env->GetByteArrayElements(data, NULL);
    if(array == NULL)
    {
        LOGE("UGoTcpRecvMsg: GetCharArrayElements error.");
        return -1;
    }

   

    res = UGo_video_incoming_frame((const unsigned char*)array, len);    
    

    /*release char array*/
    env->ReleaseByteArrayElements(data, array, 0);

  

    return res;
}
extern "C" jint Java_com_gl_softphone_UGoManager_UGoTcpUpdateState(JNIEnv*  env
        ,jobject  thiz
        ,jint state)
{
    return UGo_tcp_update_state(state);
}

extern "C" jint Java_com_gl_softphone_UGoManager_UGoSetMicMute(JNIEnv*  env
        ,jobject  thiz
        ,jboolean enable)
{
    return UGo_set_mic_mute(enable);
}

extern "C" jboolean Java_com_gl_softphone_UGoManager_UGoGetMicMute(JNIEnv*  env
        ,jobject  thiz)
{
    return (jboolean)UGo_get_mic_mute();
}


extern "C" jint Java_com_gl_softphone_UGoManager_UGoSetLoudSpeakerStatus(JNIEnv*  env
        ,jobject  thiz
        ,jboolean enable)
{
    return UGo_set_loudspeaker_status(enable);
}

extern "C" jboolean Java_com_gl_softphone_UGoManager_UGoGetLoudSpeakerStatus(JNIEnv*  env
        ,jobject  thiz)
{
    return (jboolean)UGo_get_loudspeaker_status();
}

/* add by Vinton.Liu on 2016-03-16 for Detect recording devices status begin */
extern "C" jboolean Java_com_gl_softphone_UGoManager_UGoGetRecordingDeviceStatus(JNIEnv* env
        , jobject thiz)
{
    return (jboolean)UGo_get_recording_device_status();
}
/* Vinton.Liu add end */

extern "C" jint Java_com_gl_softphone_UGoManager_UGoSendDTMF(JNIEnv*  env
        ,jobject  thiz
        ,jchar dtmf)
{
    return UGo_send_DTMF(dtmf);
}

extern "C" jint Java_com_gl_softphone_UGoManager_UGoSetLogFile(JNIEnv*  env
        ,jobject  thiz, IN jobject para)
{
    int ret = -1;

    if ( para == NULL )
    {
        LOGE("UGoSetLogFile: para is null");
        return ret;
    }

    jclass  envClass = (jclass)env->NewGlobalRef(env->GetObjectClass(para));
    jfieldID filepath_id = env->GetFieldID(envClass, "path", "Ljava/lang/String;");
    jfieldID level_id = env->GetFieldID(envClass, "level", "I");
    jstring   jfilepath = (jstring)env->GetObjectField(para,filepath_id);
    int iLevel = env->GetIntField(para,level_id);
    const char* filePath = jfilepath?env->GetStringUTFChars(jfilepath, NULL):NULL;

    ugo_log_trace_pm_t pt;
    memset(&pt, 0, sizeof(pt));
    pt.level = iLevel;
    if (filePath)
    {
        strncpy(pt.filepath, filePath, sizeof(pt.filepath) - 1);
        env->ReleaseStringUTFChars(jfilepath, filePath);
    }
    ret = UGo_set_log_file(&pt);
    
    if ( envClass )
    {
        env->DeleteGlobalRef(envClass);
        envClass = NULL;
    }

    LOGI("UGoSetLogFile: ret = %d", ret);
    return ret;
}

extern "C" jint Java_com_gl_softphone_UGoManager_UGoGetState(JNIEnv*  env
        ,jobject  thiz)
{
    return UGo_get_state();
}


extern "C" jstring Java_com_gl_softphone_UGoManager_UGoGetEmodelValue(JNIEnv*  env
        ,jobject  thiz )
{
	ugo_emodel_info_t emodelInfo;
	ME_emodel_info_t* pEmodel = NULL;
    ugo_call_info_t* pCallInfo = NULL;
	json_t *jEJson = NULL;
	json_t *jTemp  = NULL;
	json_t* jArray = NULL;	  
	char *pBuf = NULL;
	char strtmp[16] = {0};
	char jsonBuf[1024] = {0};

	memset(&emodelInfo, 0x00, sizeof(ugo_emodel_info_t));
	int ret = UGo_get_emodel_value(&emodelInfo);
	if (ret != 0)
	{
		LOGE("UGoGetEmodelValue: get emodel failed. ret = %d", ret);
		return NULL;
	}
    
	convert_emodel_value_to_string(&emodelInfo);

	pEmodel = &(emodelInfo.emodel);
    pCallInfo = &(emodelInfo.callinfo);
	
	jEJson = jsonapi_new_value(JSON_OBJECT, NULL);

	// mos
	{
		json_t* jObj   = jsonapi_new_value(JSON_OBJECT, NULL);
		
		sprintf(strtmp, "%.2lf", pEmodel->emodel_mos.min);
		jTemp = jsonapi_new_value(JSON_NUMBER, strtmp);
		jsonapi_append_value(jObj, "min", jTemp);

		sprintf(strtmp, "%.2lf", pEmodel->emodel_mos.max);
		jTemp = jsonapi_new_value(JSON_NUMBER, strtmp);
		jsonapi_append_value(jObj, "max", jTemp);

		sprintf(strtmp, "%.2lf", pEmodel->emodel_mos.average);
		jTemp = jsonapi_new_value(JSON_NUMBER, strtmp);
		jsonapi_append_value(jObj, "average", jTemp);

		jsonapi_append_value(jEJson, "mos", jObj);
	}

	// rtt
	{
		json_t* jObj   = jsonapi_new_value(JSON_OBJECT, NULL);
		
		sprintf(strtmp, "%.2lf", pEmodel->emodel_tr.min);
		jTemp = jsonapi_new_value(JSON_NUMBER, strtmp);
		jsonapi_append_value(jObj, "min", jTemp);

		sprintf(strtmp, "%.2lf", pEmodel->emodel_tr.max);
		jTemp = jsonapi_new_value(JSON_NUMBER, strtmp);
		jsonapi_append_value(jObj, "max", jTemp);

		sprintf(strtmp, "%.2lf", pEmodel->emodel_tr.average);
		jTemp = jsonapi_new_value(JSON_NUMBER, strtmp);
		jsonapi_append_value(jObj, "average", jTemp);

		jsonapi_append_value(jEJson, "tr", jObj);
	}

	// loss rate
	{
		json_t* jObj   = jsonapi_new_value(JSON_OBJECT, NULL);
		
		sprintf(strtmp, "%.2lf", pEmodel->emodel_ppl.min);
		jTemp = jsonapi_new_value(JSON_NUMBER, strtmp);
		jsonapi_append_value(jObj, "min", jTemp);

		sprintf(strtmp, "%.2lf", pEmodel->emodel_ppl.max);
		jTemp = jsonapi_new_value(JSON_NUMBER, strtmp);
		jsonapi_append_value(jObj, "max", jTemp);

		sprintf(strtmp, "%.2lf", pEmodel->emodel_ppl.average);
		jTemp = jsonapi_new_value(JSON_NUMBER, strtmp);
		jsonapi_append_value(jObj, "average", jTemp);

		jsonapi_append_value(jEJson, "ppl", jObj);
	}

	// burst ratio
	{
		json_t* jObj   = jsonapi_new_value(JSON_OBJECT, NULL);
		
		sprintf(strtmp, "%.2lf", pEmodel->emodel_burstr.min);
		jTemp = jsonapi_new_value(JSON_NUMBER, strtmp);
		jsonapi_append_value(jObj, "min", jTemp);

		sprintf(strtmp, "%.2lf", pEmodel->emodel_burstr.max);
		jTemp = jsonapi_new_value(JSON_NUMBER, strtmp);
		jsonapi_append_value(jObj, "max", jTemp);

		sprintf(strtmp, "%.2lf", pEmodel->emodel_burstr.average);
		jTemp = jsonapi_new_value(JSON_NUMBER, strtmp);
		jsonapi_append_value(jObj, "average", jTemp);

		jsonapi_append_value(jEJson, "burstr", jObj);
	}

	// ie rate
	{
		json_t* jObj   = jsonapi_new_value(JSON_OBJECT, NULL);
		
		sprintf(strtmp, "%.2lf", pEmodel->emodel_ie.min);
		jTemp = jsonapi_new_value(JSON_NUMBER, strtmp);
		jsonapi_append_value(jObj, "min", jTemp);

		sprintf(strtmp, "%.2lf", pEmodel->emodel_ie.max);
		jTemp = jsonapi_new_value(JSON_NUMBER, strtmp);
		jsonapi_append_value(jObj, "max", jTemp);

		sprintf(strtmp, "%.2lf", pEmodel->emodel_ie.average);
		jTemp = jsonapi_new_value(JSON_NUMBER, strtmp);
		jsonapi_append_value(jObj, "average", jTemp);

		jsonapi_append_value(jEJson, "ie", jObj);
	}

    // jitter buffer
    {
        json_t* jObj   = jsonapi_new_value(JSON_OBJECT, NULL);
        
        sprintf(strtmp, "%.2lf", pEmodel->jitbuf.min);
        jTemp = jsonapi_new_value(JSON_NUMBER, strtmp);
        jsonapi_append_value(jObj, "min", jTemp);

        sprintf(strtmp, "%.2lf", pEmodel->jitbuf.max);
        jTemp = jsonapi_new_value(JSON_NUMBER, strtmp);
        jsonapi_append_value(jObj, "max", jTemp);

        sprintf(strtmp, "%.2lf", pEmodel->jitbuf.average);
        jTemp = jsonapi_new_value(JSON_NUMBER, strtmp);
        jsonapi_append_value(jObj, "average", jTemp);

		jsonapi_append_value(jEJson, "jittbuf", jObj);
	}

    // delay
    {
        json_t* jObj   = jsonapi_new_value(JSON_OBJECT, NULL);
        
        sprintf(strtmp, "%.2lf", pEmodel->delay.min);
        jTemp = jsonapi_new_value(JSON_NUMBER, strtmp);
        jsonapi_append_value(jObj, "min", jTemp);

        sprintf(strtmp, "%.2lf", pEmodel->delay.max);
        jTemp = jsonapi_new_value(JSON_NUMBER, strtmp);
        jsonapi_append_value(jObj, "max", jTemp);

        sprintf(strtmp, "%.2lf", pEmodel->delay.average);
        jTemp = jsonapi_new_value(JSON_NUMBER, strtmp);
        jsonapi_append_value(jObj, "average", jTemp);

		jsonapi_append_value(jEJson, "delay", jObj);
	}

	// call info
	{
		json_t* jObj   = jsonapi_new_value(JSON_OBJECT, NULL);

		jTemp = jsonapi_new_value(JSON_STRING, pCallInfo->callid);
		jsonapi_append_value(jObj, "callid", jTemp);
		
		jTemp = jsonapi_new_value(JSON_STRING, pCallInfo->mgw);
		jsonapi_append_value(jObj, "mgw", jTemp);

		jTemp = jsonapi_new_value(JSON_STRING, pCallInfo->sgw);
		jsonapi_append_value(jObj, "sgw", jTemp);

		jTemp = jsonapi_new_value(JSON_STRING, pCallInfo->mcodec);
		jsonapi_append_value(jObj, "mcodec", jTemp);

		sprintf(strtmp, "%d", pCallInfo->cmode);
		jTemp = jsonapi_new_value(JSON_NUMBER, strtmp);
		jsonapi_append_value(jObj, "cmode", jTemp);

		sprintf(strtmp, "%d", pCallInfo->mmode);
		jTemp = jsonapi_new_value(JSON_NUMBER, strtmp);
		jsonapi_append_value(jObj, "mmode", jTemp);

		sprintf(strtmp, "%d", pCallInfo->ctime);
		jTemp = jsonapi_new_value(JSON_NUMBER, strtmp);
		jsonapi_append_value(jObj, "ctime", jTemp);

		sprintf(strtmp, "%d", pCallInfo->cstate);
		jTemp = jsonapi_new_value(JSON_NUMBER, strtmp);
		jsonapi_append_value(jObj, "cstate", jTemp);

		sprintf(strtmp, "%d", pCallInfo->role);
		jTemp = jsonapi_new_value(JSON_NUMBER, strtmp);
		jsonapi_append_value(jObj, "role", jTemp);

		/////////////////////// add by yujun below ////////////////////////////////
		sprintf(strtmp, "%d", pCallInfo->mgwport);
		jTemp = jsonapi_new_value(JSON_NUMBER, strtmp);
		jsonapi_append_value(jObj, "mport", jTemp);

		sprintf(strtmp, "%d", pEmodel->pkt_stat.pktSnd);
		jTemp = jsonapi_new_value(JSON_NUMBER, strtmp);
		jsonapi_append_value(jObj, "pktsnd", jTemp);

		sprintf(strtmp, "%d", pEmodel->pkt_stat.pktRecv);
		jTemp = jsonapi_new_value(JSON_NUMBER, strtmp);
		jsonapi_append_value(jObj, "pktrcv", jTemp);

		sprintf(strtmp, "%d", pCallInfo->spreason);
		jTemp = jsonapi_new_value(JSON_NUMBER, strtmp);
		jsonapi_append_value(jObj, "sprsn", jTemp);

        sprintf(strtmp, "%d", pCallInfo->ismute);
		jTemp = jsonapi_new_value(JSON_NUMBER, strtmp);
		jsonapi_append_value(jObj, "ismute", jTemp);
		//////////////////////////// add by yujun  //////////////////////////////////

		jsonapi_append_value(jEJson, "cinfo", jObj);
	}

	jsonapi_value_to_string(jEJson, &pBuf);
	if ( pBuf == NULL )
	{
		return NULL;
	}
	int len = strlen(pBuf);
	LOGI("UGoGetEmodelValueStr: len = %d jsonBuf = \r\n%s", len, pBuf);
	memcpy(jsonBuf, pBuf, len);

	if ( pBuf != NULL )
	{
		free(pBuf);
	}
	return env->NewStringUTF(jsonBuf);
}

/* END:   Added by gonghuojin, 2013/8/8 */

extern "C" jint Java_com_gl_softphone_UGoManager_UGoUpdateSystemState(JNIEnv*  env
        ,jobject  thiz
        ,jint state)
{
    return UGo_update_system_callstate(state);
}

static void ugo_event_cb( int ev_type, int ev_reason, 
                                const char* something, const void* param);
static void ugo_send_msg_cb( const char* msg, int slen);
static void ugo_trace_log_cb( char* summary, char* detail, 
                                    enum eME_TraceLevel level);
static void ugo_decrypt_msg_cb( unsigned char* in_data, 
                                        unsigned char* out_data, 
                                        int bytes_in, 
                                        int* bytes_out);
static void ugo_encrypt_msg_cb( unsigned char* in_data, 
								unsigned char* out_data, 
                                int bytes_in, 
                                int* bytes_out);

static void ugo_screen_shot_cb( const ucpaas_uint8* dst_argb, 
                                int dst_stride, int width, int height, 
                                int islocal, ScreenType screen_type);

static void ugo_media_proc_cb( ucpaas_int16* audio_10ms, 
								ucpaas_int32 samples,
                                ucpaas_int32 samplingFreqHz,
                                ucpaas_boolean isStereo,
								ucpaas_int16* audio_out,
								ucpaas_int32* samples_out);

/* add by Vinton.Liu on 2016-03-22 for support external audio device push pcm to and pull pcm from engine begin */
static void ugo_init_playout_cb(ucpaas_uint32 sample_rate, 
                                     ucpaas_uint32 bytes_per_sample,
                                     ucpaas_uint32 num_of_channels);

static void ugo_init_recording_cb(ucpaas_uint32 sample_rate, 
                                     ucpaas_uint32 bytes_per_sample,
                                     ucpaas_uint32 num_of_channels);

static int ugo_pull_capture_data_cb(ucpaas_sint8* audio_data, 
                                          ucpaas_uint32 size);

static int ugo_push_render_data_cb(const ucpaas_sint8* audio_data, 
                                            ucpaas_uint32 size);
/* Vinton.Liu add end */

static ugo_cb_vtable_t mCallbacks =
{
    ugo_event_cb,
    ugo_send_msg_cb,
    ugo_trace_log_cb,
    ugo_encrypt_msg_cb,
    ugo_decrypt_msg_cb,
    ugo_screen_shot_cb,
    ugo_media_proc_cb,
    /* add by Vinton.Liu on 2016-03-22 for support external audio device push pcm to and pull pcm from engine begin */
    ugo_init_playout_cb,
    ugo_init_recording_cb,
    ugo_pull_capture_data_cb,
    ugo_push_render_data_cb
    /* Vinton.Liu add end */
};

jclass UGoCBClass = NULL;
jobject UGoCBObject= NULL;
jmethodID eventMethod = NULL;
jmethodID sendMsgMethod = NULL;
jmethodID traceLogMethod = NULL;
jmethodID encryptMethod = NULL;
jmethodID decryptMethod = NULL;
jmethodID screenshotMethod = NULL;
jmethodID mediaprocMethod = NULL;
/* add by Vinton.Liu on 2016-03-22 for support external audio device push pcm to and pull pcm from engine begin */
jmethodID initPlayoutMethod = NULL;
jmethodID initRecordingMethod = NULL;
jmethodID readRecordingMethod = NULL;
jmethodID writePlayoutMethod = NULL;
/* Vinton.Liu add end */

extern "C" void Java_com_gl_softphone_UGoManager_Callbacks(JNIEnv*  env
        ,jobject  thiz
        ,jobject cb_obj)
{
    LOGI("UGoManager_Callbacks IN");
    
    if (cb_obj)
    {
        UGoCBObject = env->NewGlobalRef(cb_obj);
        UGoCBClass = (jclass)env->NewGlobalRef(env->GetObjectClass(cb_obj));

        eventMethod = env->GetMethodID(UGoCBClass,"eventCallback", "(IILjava/lang/String;Ljava/lang/String;)V");
        sendMsgMethod = env->GetMethodID(UGoCBClass,"sendCallback", "([BI)V");
        traceLogMethod = env->GetMethodID(UGoCBClass,"traceCallback", "(Ljava/lang/String;Ljava/lang/String;I)V");
        encryptMethod = env->GetMethodID(UGoCBClass, "encryptCallback", "([B[BI[I)V");
        decryptMethod = env->GetMethodID(UGoCBClass, "decryptCallback", "([B[BI[I)V");
        screenshotMethod = env->GetMethodID(UGoCBClass, "screenshotCallback", "([BIIIII)V");
		mediaprocMethod = env->GetMethodID(UGoCBClass, "mediaProcCallback", "([S[SIIZ)I");
        initPlayoutMethod = env->GetMethodID(UGoCBClass, "initPlayout", "(III)V");
        initRecordingMethod = env->GetMethodID(UGoCBClass, "initRecording", "(III)V");
        readRecordingMethod = env->GetMethodID(UGoCBClass, "readRecordingData", "([BI)I");
        writePlayoutMethod = env->GetMethodID(UGoCBClass, "writePlayoutData", "([BI)I");
        
        UGo_callback_vtable(&mCallbacks);
    }

    LOGI("UGoManager_Callbacks OUT");    
}


static void build_network_json_body(IN ME_network_state_t *net, json_t *event_json)
{
	json_t* jnet = NULL;
	json_t* jtmp = NULL;
	char strtmp[128] = { 0 };

	if ( event_json == NULL )
	{
	    return;
	}

	jnet = event_json;

	sprintf(strtmp, "%d", net->net_state);
	jtmp = jsonapi_new_value(JSON_NUMBER, strtmp);
	jsonapi_append_value(jnet, "ns", jtmp);

	sprintf(strtmp, "%d", net->ice);
	jtmp = jsonapi_new_value(JSON_NUMBER, strtmp);
	jsonapi_append_value(jnet, "ice", jtmp);

	sprintf(strtmp, "%d", net->downlinkLost);
	jtmp = jsonapi_new_value(JSON_NUMBER, strtmp);
	jsonapi_append_value(jnet, "dl", jtmp);

	sprintf(strtmp, "%d", net->uplinkLost);
	jtmp = jsonapi_new_value(JSON_NUMBER, strtmp);
	jsonapi_append_value(jnet, "ul", jtmp);

	sprintf(strtmp, "%d", net->rttMs);
	jtmp = jsonapi_new_value(JSON_NUMBER, strtmp);
	jsonapi_append_value(jnet, "rtt", jtmp);

	sprintf(strtmp, "%d", net->upJitter);
	jtmp = jsonapi_new_value(JSON_NUMBER, strtmp);
	jsonapi_append_value(jnet, "upjt", jtmp);

   	sprintf(strtmp, "%d", net->downJitter);
	jtmp = jsonapi_new_value(JSON_NUMBER, strtmp);
	jsonapi_append_value(jnet, "dnjt", jtmp);

	sprintf(strtmp, "%d", net->sequenceLost);
	jtmp = jsonapi_new_value(JSON_NUMBER, strtmp);
	jsonapi_append_value(jnet, "sl", jtmp);
}

static void build_video_network_json_body(IN ME_video_network_state_t *net, json_t *event_json)
{
	json_t* jnet = NULL;
	json_t* jtmp = NULL;
	char strtmp[128] = { 0 };

	if ( event_json == NULL )
	{
	    return;
	}

	jnet = event_json;

	sprintf(strtmp, "%d", net->net_state);
	jtmp = jsonapi_new_value(JSON_NUMBER, strtmp);
	jsonapi_append_value(jnet, "ns", jtmp);

	sprintf(strtmp, "%d", net->ice);
	jtmp = jsonapi_new_value(JSON_NUMBER, strtmp);
	jsonapi_append_value(jnet, "ice", jtmp);

	sprintf(strtmp, "%d", net->uplinkLost);
	jtmp = jsonapi_new_value(JSON_NUMBER, strtmp);
	jsonapi_append_value(jnet, "ul", jtmp);

	sprintf(strtmp, "%d", net->downlinkLost);
	jtmp = jsonapi_new_value(JSON_NUMBER, strtmp);
	jsonapi_append_value(jnet, "dl", jtmp);

	sprintf(strtmp, "%d", net->bitrate_bps);
	jtmp = jsonapi_new_value(JSON_NUMBER, strtmp);
	jsonapi_append_value(jnet, "sb", jtmp);

	sprintf(strtmp, "%d", net->rttMs);
	jtmp = jsonapi_new_value(JSON_NUMBER, strtmp);
	jsonapi_append_value(jnet, "rtt", jtmp);

	sprintf(strtmp, "%d", net->upJitter);
	jtmp = jsonapi_new_value(JSON_NUMBER, strtmp);
	jsonapi_append_value(jnet, "upjt", jtmp);

    sprintf(strtmp, "%d", net->downJitter);
	jtmp = jsonapi_new_value(JSON_NUMBER, strtmp);
	jsonapi_append_value(jnet, "dnjt", jtmp);

	sprintf(strtmp, "%d", net->target_fps);
	jtmp = jsonapi_new_value(JSON_NUMBER, strtmp);
	jsonapi_append_value(jnet, "sf", jtmp);

	sprintf(strtmp, "%d", net->rec_bitrate);
	jtmp = jsonapi_new_value(JSON_NUMBER, strtmp);
	jsonapi_append_value(jnet, "rb", jtmp);

	sprintf(strtmp, "%d", net->width);
	jtmp = jsonapi_new_value(JSON_NUMBER, strtmp);
	jsonapi_append_value(jnet, "sw", jtmp);

	sprintf(strtmp, "%d", net->height);
	jtmp = jsonapi_new_value(JSON_NUMBER, strtmp);
	jsonapi_append_value(jnet, "sh", jtmp);

	sprintf(strtmp, "%d", net->decode_width);
	jtmp = jsonapi_new_value(JSON_NUMBER, strtmp);
	jsonapi_append_value(jnet, "dw", jtmp);

	sprintf(strtmp, "%d", net->decode_height);
	jtmp = jsonapi_new_value(JSON_NUMBER, strtmp);
	jsonapi_append_value(jnet, "dh", jtmp);

	sprintf(strtmp, "%d", net->decode_fps);
	jtmp = jsonapi_new_value(JSON_NUMBER, strtmp);
	jsonapi_append_value(jnet, "df", jtmp);

	sprintf(strtmp, "%d", net->EncoderPt);
	jtmp = jsonapi_new_value(JSON_NUMBER, strtmp);
	jsonapi_append_value(jnet, "ep", jtmp);

	sprintf(strtmp, "%d", net->DecoderPt);
	jtmp = jsonapi_new_value(JSON_NUMBER, strtmp);
	jsonapi_append_value(jnet, "dp", jtmp);	

	sprintf(strtmp, "%s", net->encCodec);
	jtmp = jsonapi_new_value(JSON_STRING, strtmp);
	jsonapi_append_value(jnet, "eCodec", jtmp);

	sprintf(strtmp, "%s", net->decCodec);
	jtmp = jsonapi_new_value(JSON_STRING, strtmp);
	jsonapi_append_value(jnet, "dCodec", jtmp);

	sprintf(strtmp, "%d", net->rtPOT_nCount);
	jtmp = jsonapi_new_value(JSON_NUMBER, strtmp);
	jsonapi_append_value(jnet, "nCnt", jtmp);

	json_t* jobj = NULL;
	json_t* jrtPOTarray = NULL;
	if (net->rtPOT_nCount != 0)
	{
		jrtPOTarray = jsonapi_new_value(JSON_ARRAY, NULL);
		for (int i = 0; i < net->rtPOT_nCount; i++)
		{
			jobj = jsonapi_new_value(JSON_OBJECT, NULL);

			sprintf(strtmp, "%d", net->rtPOT_SendIP[i]);
			jtmp = jsonapi_new_value(JSON_NUMBER, strtmp);
			jsonapi_append_value(jobj, "sIP", jtmp);

			sprintf(strtmp, "%d", net->rtPOT_SendValue_a[i]);
			jtmp = jsonapi_new_value(JSON_NUMBER, strtmp);
			jsonapi_append_value(jobj, "sFlow_a", jtmp);

			sprintf(strtmp, "%d", net->rtPOT_SendValue_v[i]);
			jtmp = jsonapi_new_value(JSON_NUMBER, strtmp);
			jsonapi_append_value(jobj, "sFlow_v", jtmp);

			sprintf(strtmp, "%d", net->rtPOT_RecvIP[i]);
			jtmp = jsonapi_new_value(JSON_NUMBER, strtmp);
			jsonapi_append_value(jobj, "rIP", jtmp);

			sprintf(strtmp, "%d", net->rtPOT_RecvValue_a[i]);
			jtmp = jsonapi_new_value(JSON_NUMBER, strtmp);
			jsonapi_append_value(jobj, "rFlow_a", jtmp);

			sprintf(strtmp, "%d", net->rtPOT_RecvValue_v[i]);
			jtmp = jsonapi_new_value(JSON_NUMBER, strtmp);
			jsonapi_append_value(jobj, "rFlow_v", jtmp);

			jsonapi_append_value(jrtPOTarray, NULL, jobj);
		}

		jsonapi_append_value(jnet, "rtPOT", jrtPOTarray);
	}

}

void ugo_event_cb( int ev_type, int ev_reason, 
                        const char* something, const void* param)
{
    if (UGoCBObject)
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
                    LOGE("ugo_event_cb: cannot attach VM");
                    return;
                }
                attached = true;
                break;
            case JNI_EVERSION:
                LOGE("ugo_event_cb: JNI_EVERSION error");
                return;
        }

        json_t *event_json = NULL;
        json_t *jtemp      = NULL;
        char strtmp[64]    = {0};
        char* event_buf    = NULL;
        if ( param != NULL )
        {
            event_json = jsonapi_new_value(JSON_OBJECT, NULL);

            if ( ev_type == eUGo_CONFERENCE_EV )
            {
                if ( ev_reason == eUGo_Reason_StateNotify )
                {
                    ugo_conf_ntfy_state_t *pconfev = (ugo_conf_ntfy_state_t *)param;
                    json_t* jmeetarray = NULL;
                    json_t* jobj = NULL;
                    sprintf(strtmp, "%d", pconfev->pnum);
                    jtemp     = jsonapi_new_value (JSON_NUMBER, strtmp);
                    jsonapi_append_value(event_json, "conference_num", jtemp);

                    if ( pconfev->pnum != 0 )
                    {
                        jmeetarray = jsonapi_new_value(JSON_ARRAY, NULL);
                        for (int i = 0; i < pconfev->pnum; i++)
                        {
                            jobj = jsonapi_new_value(JSON_OBJECT, NULL);

                            sprintf(strtmp, "%d", pconfev->pstate[i].call_state);
                            jtemp = jsonapi_new_value(JSON_NUMBER, strtmp);
                            jsonapi_append_value(jobj, "state", jtemp);

                            sprintf(strtmp, "%d", pconfev->pstate[i].reason);
                            jtemp = jsonapi_new_value(JSON_NUMBER, strtmp);
                            jsonapi_append_value(jobj, "reason", jtemp);

                            jtemp = jsonapi_new_value(JSON_STRING, pconfev->pstate[i].uid);
                            jsonapi_append_value(jobj, "uid", jtemp);

                            jtemp = jsonapi_new_value(JSON_STRING, pconfev->pstate[i].phone);
                            jsonapi_append_value(jobj, "phone", jtemp);

                            jsonapi_append_value(jmeetarray, NULL, jobj);
                        }
                        jsonapi_append_value(event_json, "parties", jmeetarray);
                    }
                }
            }
            else if ( ev_type == eUGo_CALL_TRANS_STATE_EV )
            {
                ugo_call_trans_state_t *trans_state = (ugo_call_trans_state_t *)param;

                sprintf(strtmp, "%d", trans_state->vie_state);
                jtemp = jsonapi_new_value(JSON_NUMBER, strtmp);
                jsonapi_append_value(event_json, "vstate", jtemp);

                sprintf(strtmp, "%d", trans_state->mic_state);
                jtemp = jsonapi_new_value(JSON_NUMBER, strtmp);
                jsonapi_append_value(event_json, "mstate", jtemp);

                sprintf(strtmp, "%d", trans_state->net_state);
                jtemp = jsonapi_new_value(JSON_NUMBER, strtmp);
                jsonapi_append_value(event_json, "nstate", jtemp);
            }
            else if ( (ev_type == eUGo_NETWORK_EV)
                || (ev_type == eUGo_VIDEO_NETWORK_EV))
            {
                if ( ev_type == eUGo_NETWORK_EV )
                {
                	build_network_json_body((ME_network_state_t*)(param), event_json);
                }
                else
                {
                	build_video_network_json_body((ME_video_network_state_t*)(param), event_json);
                }
            }
            else
            {
                ugo_callev_pm_t* pcallev = (ugo_callev_pm_t*)param;

                sprintf(strtmp, "%d", pcallev->videoflag);
                jtemp     = jsonapi_new_value(JSON_NUMBER, strtmp);
                jsonapi_append_value(event_json, "videoflag",jtemp);

                sprintf(strtmp, "%d", pcallev->audiofec);
                jtemp     = jsonapi_new_value(JSON_NUMBER, strtmp);
                jsonapi_append_value(event_json, "audiofec",jtemp);
				
				sprintf(strtmp, "%d", pcallev->directfec);
                jtemp     = jsonapi_new_value(JSON_NUMBER, strtmp);
                jsonapi_append_value(event_json, "directfec",jtemp);

                sprintf(strtmp, "%d", pcallev->calltype);
                jtemp     = jsonapi_new_value(JSON_NUMBER, strtmp);
                jsonapi_append_value(event_json, "calltype",jtemp);
				
				/* add by wuzhaoyang for meeting dialing*/
				sprintf(strtmp, "%d", pcallev->meetingflag);
                jtemp     = jsonapi_new_value(JSON_NUMBER, strtmp);
                jsonapi_append_value(event_json, "meetingflag",jtemp);

                /* add by Vinton.Liu on 20140918 for uxin liaoyiliao */
                sprintf(strtmp, "%d", pcallev->ucalltype);
                jtemp     = jsonapi_new_value (JSON_NUMBER, strtmp);
                jsonapi_append_value (event_json, "ucalltype", jtemp);
                /* Vinton.Liu add end */
                
                jtemp	  = jsonapi_new_value(JSON_STRING, pcallev->callid);
                jsonapi_append_value(event_json, "callid", jtemp);

                jtemp	  = jsonapi_new_value(JSON_STRING, pcallev->fuid);
                jsonapi_append_value(event_json, "fuid", jtemp);

                jtemp     = jsonapi_new_value(JSON_STRING, pcallev->fuserid);
                jsonapi_append_value(event_json, "fuserid", jtemp);
                
                jtemp	  = jsonapi_new_value(JSON_STRING, pcallev->fphone);
                jsonapi_append_value(event_json, "fphone", jtemp);
				
                jtemp     = jsonapi_new_value(JSON_STRING, pcallev->fnickname);
                jsonapi_append_value(event_json, "fnickname", jtemp);
                
                jtemp	  = jsonapi_new_value(JSON_STRING, pcallev->tuid);
                jsonapi_append_value(event_json, "tuid", jtemp);

                jtemp	  = jsonapi_new_value(JSON_STRING, pcallev->tuserid);
                jsonapi_append_value(event_json, "tuserid", jtemp);
                
                jtemp	  = jsonapi_new_value(JSON_STRING, pcallev->tphone);
                jsonapi_append_value(event_json, "tphone", jtemp);

                jtemp     = jsonapi_new_value(JSON_STRING, pcallev->user_data);
                jsonapi_append_value(event_json, "user_data", jtemp);
            }
            jsonapi_value_to_string(event_json, &event_buf);

            LOGI("ugo_event_cb: ev_type: %d buf: %s", ev_type, event_buf);
        }        

        const char* constSomething = (const char*)something;
		jstring jconstSomething = constSomething ? env->NewStringUTF(constSomething) : NULL;
		jstring jevent_buf = event_buf ? env->NewStringUTF(event_buf) : NULL;

		env->CallVoidMethod(UGoCBObject
				    ,eventMethod
				    ,ev_type
				    ,ev_reason
					,jconstSomething
					,jevent_buf
				   );
	   if (jconstSomething != NULL)
	  {
		  env->DeleteLocalRef(jconstSomething);
		  jconstSomething = NULL;
	  }
	   if (jevent_buf != NULL)
	  {
		  env->DeleteLocalRef(jevent_buf);
		  jconstSomething = NULL;
	  }
	  if(event_json)
	  {
	    jsonapi_delete_value(&event_json);
	  }
      
	  if (attached)
	     jvm->DetachCurrentThread();
	}
}

void ugo_send_msg_cb(const char* msg, int slen)
{
    if (UGoCBObject)
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
                    LOGE("ugo_send_msg_cb: cannot attach VM");
                    return;
                }
                attached = true;
                break;
            case JNI_EVERSION:
                LOGE("ugo_send_msg_cb: JNI_EVERSION error");
                return;
        }

        LOGI("ugo_send_msg_cb: in......");
        
        /*new char array*/
        jbyteArray array = env->NewByteArray(slen);
        jbyte *pArray;
        int i;

        if(array == NULL)
        {
            LOGE("ugo_send_msg_cb: NewCharArray error.");
            return;
        }

        /*create jbyte buf*/
        pArray = (jbyte*)calloc(slen, sizeof(jbyte));
        if(pArray == NULL)
        {
            LOGE("ugo_send_msg_cb: calloc error.");
            return;
        }

        /*copy buffer to jbyte array*/
        for(i = 0; i < slen; i++)
        {
            *(pArray + i) = *(msg + i);
            //   __android_log_print(ANDROID_LOG_INFO, TAG, "jni msg %s",*(pArray + i));
        }

        /*copy buffer to jcharArray*/
        env->SetByteArrayRegion(array,0,slen,pArray);

        /*invoke java callback method */
        env->CallVoidMethod(UGoCBObject,
                            sendMsgMethod,
                            array,
                            slen);
        /*release resource */
        env->DeleteLocalRef(array);
        free(pArray);
        pArray = NULL;

        LOGI("ugo_send_msg_cb: out......");
        
        if (attached)
            jvm->DetachCurrentThread();
    }
}

void ugo_trace_log_cb(char* summary, char* detail, enum eME_TraceLevel level)
{
    if (UGoCBObject)
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
                    LOGE("ugo_trace_log_cb: cannot attach VM");
                    return;
                }
                attached = true;
                break;
            case JNI_EVERSION:
                LOGE("ugo_trace_log_cb: JNI_EVERSION error");
                return;
        }

        //LOGI("ugo_trace_log_cb: %s", detail);
		jstring jsummary = summary ? env->NewStringUTF(summary) : NULL;
		jstring jdetail = detail ? env->NewStringUTF(detail) : NULL;
        env->CallVoidMethod(UGoCBObject
                            ,traceLogMethod
							,jsummary
							,jdetail
                            ,level
                           );

		if (jsummary != NULL)
		{
			env->DeleteLocalRef(jsummary);
			jsummary = NULL;
		}
		if (jdetail != NULL)
		{
			env->DeleteLocalRef(jdetail);
			jdetail = NULL;
		}
        if (attached)
            jvm->DetachCurrentThread();
    }
}

void ugo_encrypt_msg_cb( unsigned char* in_data, 
                                unsigned char* out_data,
							    int bytes_in, 
							    int* bytes_out)
{
    if ( NULL == in_data || NULL == out_data || !bytes_in )
    {
        LOGE("ugo_encrypt_msg_cb: param error!!");
        return;
    }

    // begin for debug
#if 0
    {
        *bytes_out = bytes_in;
        memcpy(out_data, in_data, bytes_in);
    }
    return;
#endif
    //end for debug
    
    if (UGoCBObject && encryptMethod)
    {
        JNIEnv *env = 0;
        bool attached = false;
        int uOutLen = 1500;
        switch (jvm->GetEnv((void**)&env, JNI_VERSION_1_4))
        {
            case JNI_OK:
                break;
            case JNI_EDETACHED:
                if (jvm->AttachCurrentThread(&env, NULL)!=0)
                {
                    LOGE("ugo_encrypt_msg_cb: cannot attach VM.");
                    return;
                }
                attached = true;
                break;
            case JNI_EVERSION:
                LOGE("ugo_encrypt_msg_cb: JNI_EVERSION error.");
                return;
        }

        //LOGI("ugo_encrypt_msg_cb: enter.");

        /*new char array*/
        jbyteArray jbInArray = env->NewByteArray(bytes_in);
        jbyteArray jbOutArray = env->NewByteArray(uOutLen);
        jintArray jbOutLenArray = env->NewIntArray(1);
        jbyte *pInArray = NULL;
        jbyte *pOutArray = NULL;
        jint outLenArray[1] = {0};
        int i;
        static int cnt = 5;

        //LOGI("ugo_encrypt_msg_cb: bytes_in = %d.", bytes_in);
        
        if( jbInArray == NULL || jbOutArray == NULL )
        {
            LOGE("ugo_encrypt_msg_cb:  NewCharArray error.");
            return;
        }

        /*create jbyte buf*/
        pInArray = (jbyte*)calloc(bytes_in, sizeof(jbyte));
        if(pInArray == NULL)
        {
            LOGE("ugo_encrypt_msg_cb:  pInArray calloc error.");
            return;
        }

        pOutArray = (jbyte*)calloc(uOutLen, sizeof(jbyte));
        if(pOutArray == NULL)
        {
            LOGE("ugo_encrypt_msg_cb:  pOutArray calloc error.");
            return;
        }

        /*copy buffer to jbyte array*/
        for(i = 0; i < bytes_in; i++)
        {
            *(pInArray + i) = *(in_data + i);
            //LOGI("ugo_encrypt_msg_cb: in_data[%d] = %d", i, in_data[i]);
        }

        /*copy buffer to jcharArray*/
        env->SetByteArrayRegion(jbInArray, 0, bytes_in, pInArray);
        env->SetByteArrayRegion(jbOutArray, 0, uOutLen, pOutArray);
        env->SetIntArrayRegion(jbOutLenArray, 0, 1, outLenArray);

        env->CallVoidMethod(UGoCBObject,
                            encryptMethod,
                            jbInArray,
                            jbOutArray,
                            bytes_in,
                            jbOutLenArray);
        
        jbyte *ptrOutArray = env->GetByteArrayElements(jbOutArray, 0);
        jint *pOutLenArray = env->GetIntArrayElements(jbOutLenArray, 0);
        //LOGI("ugo_encrypt_msg_cb: pOutLenArray = %d .", pOutLenArray[0]);
        *bytes_out = pOutLenArray[0];
        //LOGI("ugo_encrypt_msg_cb: bytes_out[%d].", *bytes_out);
        
        for (i = 0; i < pOutLenArray[0]; i++ )
        {            
            *(out_data + i) = *(ptrOutArray + i);
            //LOGI("ugo_encrypt_msg_cb: out_data[%d] = %d", i, out_data[i]);
        }

        /*release resource */
        env->ReleaseByteArrayElements(jbOutArray, ptrOutArray, 0);
        env->ReleaseIntArrayElements(jbOutLenArray, pOutLenArray, 0);
        env->DeleteLocalRef(jbInArray);
        env->DeleteLocalRef(jbOutArray);
        env->DeleteLocalRef(jbOutLenArray);
        if ( pInArray )
        {
            free(pInArray);
            pInArray = NULL;
        }
        
        if ( pOutArray )
        {
            free(pOutArray);
            pOutArray = NULL;
        }
        //LOGI("ugo_encrypt_msg_cb: out......");

        if (attached)
            jvm->DetachCurrentThread();
    }
}

void ugo_decrypt_msg_cb( unsigned char* in_data, 
						 unsigned char* out_data,
						 int bytes_in,
						 int* bytes_out)
{
    if ( NULL == in_data || NULL == out_data || !bytes_in )
    {
        LOGE("ugo_decrypt_msg_cb: param error!!");
        return;
    }
    // begin for debug
#if 0
    {
        *bytes_out = bytes_in;
        memcpy(out_data, in_data, bytes_in);
    }
    return;
#endif
    //end for debug    
    
    if (UGoCBObject && decryptMethod)
    {
        JNIEnv *env = 0;
        bool attached = false;
        int uOutLen = 1500;
        switch (jvm->GetEnv((void**)&env, JNI_VERSION_1_4))
        {
            case JNI_OK:
                break;
            case JNI_EDETACHED:
                if (jvm->AttachCurrentThread(&env, NULL)!=0)
                {
                    LOGE("ugo_decrypt_msg_cb: cannot attach VM.");
                    return;
                }
                attached = true;
                break;
            case JNI_EVERSION:
                LOGE("ugo_decrypt_msg_cb: JNI_EVERSION error.");
                return;
        }

        /*new char array*/
        jbyteArray jbInArray = env->NewByteArray(bytes_in);
        jbyteArray jbOutArray = env->NewByteArray(uOutLen);
        jintArray jbOutLenArray = env->NewIntArray(1);
        jbyte *pInArray = NULL;
        jbyte *pOutArray = NULL;
        jint outLenArray[1] = {0};
        int i;
        
        if( jbInArray == NULL || jbOutArray == NULL )
        {
            LOGE("ugo_decrypt_msg_cb: NewCharArray error.");
            return;
        }

        /*create jbyte buf*/
        pInArray = (jbyte*)calloc(bytes_in, sizeof(jbyte));
        if(pInArray == NULL)
        {
            LOGE("ugo_decrypt_msg_cb: pInArray calloc error.");
            return;
        }

        pOutArray = (jbyte*)calloc(uOutLen, sizeof(jbyte));
        if(pOutArray == NULL)
        {
            LOGE("ugo_decrypt_msg_cb: pOutArray calloc error.");
            return;
        }

        /*copy buffer to jbyte array*/
        for(i = 0; i < bytes_in; i++)
        {
            *(pInArray + i) = *(in_data + i);
        }

        /*copy buffer to jcharArray*/
        env->SetByteArrayRegion(jbInArray, 0, bytes_in, pInArray);
        env->SetByteArrayRegion(jbOutArray, 0, uOutLen, pOutArray);
        env->SetIntArrayRegion(jbOutLenArray, 0, 1, outLenArray);

        env->CallVoidMethod(UGoCBObject,
                            decryptMethod,
                            jbInArray,
                            jbOutArray,
                            bytes_in,
                            jbOutLenArray);
        
        
        jbyte *ptrOutArray = env->GetByteArrayElements(jbOutArray, 0);
        jint *pOutLenArray = env->GetIntArrayElements(jbOutLenArray, 0);
        *bytes_out = pOutLenArray[0];
        
        for (i = 0; i < pOutLenArray[0]; i++ )
        {
            *(out_data + i) = *(ptrOutArray + i);
        }

        /*release resource */
        env->ReleaseByteArrayElements(jbOutArray, ptrOutArray, 0);
        env->ReleaseIntArrayElements(jbOutLenArray, pOutLenArray, 0);
        env->DeleteLocalRef(jbInArray);
        env->DeleteLocalRef(jbOutArray);
        env->DeleteLocalRef(jbOutLenArray);
        
        if ( pInArray )
        {
            free(pInArray);
            pInArray = NULL;
        }
        
        if ( pOutArray )
        {
            free(pOutArray);
            pOutArray = NULL;
        }

        if (attached)
            jvm->DetachCurrentThread();
    }
}

static  void ugo_screen_shot_cb( const ucpaas_uint8* dst_argb, 
								 int dst_stride, int width, int height, 
                                 int islocal, ScreenType screen_type)
{

   if ( NULL == dst_argb  )
    {
        LOGE("screen_shot_cb: param error!!");
        return;
    }
   
    
    if (UGoCBObject && screenshotMethod)
    {
        JNIEnv *env = 0;
        bool attached = false;
        int uOutLen = width * height * ((screen_type == ARGB || screen_type == ABGR) ? 4 : 3);
       
        switch (jvm->GetEnv((void**)&env, JNI_VERSION_1_4))
        {
            case JNI_OK:
                break;
            case JNI_EDETACHED:
                if (jvm->AttachCurrentThread(&env, NULL)!=0)
                {
                    LOGE("ugo_decrypt_msg_cb: cannot attach VM.");
                    return;
                }
                attached = true;
                break;
            case JNI_EVERSION:
                LOGE("ugo_decrypt_msg_cb: JNI_EVERSION error.");
                return;
        }

        /*new char array*/
       
        jbyteArray jbOutArray = env->NewByteArray(uOutLen);      
       
        jbyte *pOutArray = NULL;
       
        int i;
        
        if( jbOutArray == NULL )
        {
            LOGE("screen_shot_cb: NewCharArray error.");
            return;
        }


        pOutArray = (jbyte*)calloc(uOutLen, sizeof(jbyte));
        if(pOutArray == NULL)
        {
            LOGE("ugo_decrypt_msg_cb: pOutArray calloc error.");
            return;
        }

        /*copy buffer to jbyte array*/
        memcpy(pOutArray, dst_argb, uOutLen);

        /*copy buffer to jcharArray*/     
        env->SetByteArrayRegion(jbOutArray, 0, uOutLen, pOutArray);

        env->CallVoidMethod( UGoCBObject,
                             screenshotMethod,                           
                             jbOutArray,
                             dst_stride, width, height,
                             islocal, (int)screen_type);

        /*release resource */      
        env->DeleteLocalRef(jbOutArray);
        
        if ( pOutArray )
        {
            free(pOutArray);
            pOutArray = NULL;
        }

        if (attached)
            jvm->DetachCurrentThread();
    }
}

static void ugo_media_proc_cb( ucpaas_int16* audio_10ms, 
							   ucpaas_int32 samples,
                               ucpaas_int32 samplingFreqHz,
                               ucpaas_boolean isStereo,
							   ucpaas_int16* audio_out,
							   ucpaas_int32* samples_out)
{
    if ( NULL == audio_10ms || samples <= 0 )
    {
        LOGE("ugo_media_proc_cb param null.");
        return;
    }
    
    if ( UGoCBObject )
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
                    LOGE("ugo_media_proc_cb: cannot attach VM.");
                    return;
                }
                attached = true;
                break;
            case JNI_EVERSION:
                LOGE("ugo_media_proc_cb: JNI_EVERSION error.");
                return;
        }

        /* create a short array */
        jshortArray jsInAudioArr = env->NewShortArray(samples);
        jshortArray jsOutAudioArr = env->NewShortArray(samples);
        if ( NULL == jsInAudioArr || NULL == jsOutAudioArr )
        {
            LOGE("ugo_media_proc_cb: malloc short array error.");
            return;
        }
        env->SetShortArrayRegion(jsInAudioArr, 0, samples, audio_10ms);

        *samples_out = env->CallIntMethod(UGoCBObject,
				                            mediaprocMethod,
				                            jsInAudioArr,
				                            jsOutAudioArr,
				                            samples,
				                            samplingFreqHz,
				                            isStereo);
        jshort *outSamples = env->GetShortArrayElements(jsOutAudioArr, 0);
        memcpy(samples_out, outSamples, *samples_out);
        /* release resource */
        env->ReleaseShortArrayElements(jsOutAudioArr, outSamples, 0);
        env->DeleteLocalRef(jsInAudioArr);
        env->DeleteLocalRef(jsOutAudioArr);
        
        if (attached)
            jvm->DetachCurrentThread();
    }
}

/* add by Vinton.Liu on 2016-03-22 for support external audio device push pcm to and pull pcm from engine begin */
static void ugo_init_playout_cb(ucpaas_uint32 sample_rate, 
                                     ucpaas_uint32 bytes_per_sample,
                                     ucpaas_uint32 num_of_channels)
{
    if ( UGoCBObject )
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
                    LOGE("ugo_event_cb: cannot attach VM");
                    return;
                }
                attached = true;
                break;
            case JNI_EVERSION:
                LOGE("ugo_event_cb: JNI_EVERSION error");
                return;
        }

        env->CallVoidMethod(UGoCBObject
                            , initPlayoutMethod
                            , sample_rate
                            , bytes_per_sample
                            , num_of_channels
                            );
        if (attached)
	        jvm->DetachCurrentThread();
    }
}

static void ugo_init_recording_cb(ucpaas_uint32 sample_rate, 
                                     ucpaas_uint32 bytes_per_sample,
                                     ucpaas_uint32 num_of_channels)
{
    if ( UGoCBObject )
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
                    LOGE("ugo_event_cb: cannot attach VM");
                    return;
                }
                attached = true;
                break;
            case JNI_EVERSION:
                LOGE("ugo_event_cb: JNI_EVERSION error");
                return;
        }

        env->CallVoidMethod(UGoCBObject
                            , initRecordingMethod
                            , sample_rate
                            , bytes_per_sample
                            , num_of_channels
                            );

        if (attached)
	        jvm->DetachCurrentThread();
    }
}

static int ugo_pull_capture_data_cb(ucpaas_sint8* audio_data, 
                                          ucpaas_uint32 size)
{
    int res = -1;
    
    if ( UGoCBObject && (audio_data && size) )
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
                    LOGE("ugo_pull_capture_data_cb: cannot attach VM");
                    return res;
                }
                attached = true;
                break;
            case JNI_EVERSION:
                LOGE("ugo_pull_capture_data_cb: JNI_EVERSION error");
                return res;
        }
        
        int uOutLen = 1500;
        jbyteArray jbOutArray = env->NewByteArray(uOutLen);
        jbyte *pOutArray = NULL;

        if( jbOutArray == NULL )
        {
            LOGE("ugo_pull_capture_data_cb:  NewCharArray error.");
            return res;
        }
        pOutArray = (jbyte*)calloc(uOutLen, sizeof(jbyte));
        if(pOutArray == NULL)
        {
            LOGE("ugo_pull_capture_data_cb:  Array calloc error.");
            return res;
        }
        env->SetByteArrayRegion(jbOutArray, 0, uOutLen, pOutArray);

        res = env->CallIntMethod(UGoCBObject,
                            readRecordingMethod,
                            jbOutArray,
                            size);
        jbyte *ptrOutArray = env->GetByteArrayElements(jbOutArray, 0);
        if ( res == 0 )
        {
            memcpy(audio_data, ptrOutArray, size);
        }
        
        /*release resource */
        env->ReleaseByteArrayElements(jbOutArray, ptrOutArray, 0);
        env->DeleteLocalRef(jbOutArray);
        if ( pOutArray )
        {
            free(pOutArray);
            pOutArray = NULL;
        }

        if (attached)
	        jvm->DetachCurrentThread();

        return res;
    }
    else
    {
        return res;
    }
}

static int ugo_push_render_data_cb(const ucpaas_sint8* audio_data, 
                                            ucpaas_uint32 size)
{
    int res = -1;
    
    if ( UGoCBObject && (audio_data && size) )
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
                    LOGE("ugo_push_render_data_cb: cannot attach VM");
                    return res;
                }
                attached = true;
                break;
            case JNI_EVERSION:
                LOGE("ugo_push_render_data_cb: JNI_EVERSION error");
                return res;
        }

        jbyteArray jbOutArray = env->NewByteArray(size);
        jbyte *pOutArray = NULL;

        if( jbOutArray == NULL )
        {
            LOGE("ugo_push_render_data_cb: NewCharArray error.");
            return res;
        }

        /*create jbyte buf*/
        pOutArray = (jbyte*)calloc(size, sizeof(jbyte));
        if(pOutArray == NULL)
        {
            LOGE("ugo_push_render_data_cb:  Array calloc error.");
            return res;
        }

        /*copy buffer to jbyte array*/
        memcpy(pOutArray, audio_data, size);

        /*copy buffer to jcharArray*/
        env->SetByteArrayRegion(jbOutArray, 0, size, pOutArray);

        res = env->CallIntMethod(UGoCBObject,
                            writePlayoutMethod,
                            jbOutArray,
                            size);
        
        /*release resource */
        env->DeleteLocalRef(jbOutArray);        
        if ( pOutArray )
        {
            free(pOutArray);
            pOutArray = NULL;
        }
        
        if (attached)
	        jvm->DetachCurrentThread();

        return res;
    }
    else
    {
        return res;
    }
}
/* Vinton.Liu add end */

void releaseJniObject()
{
    JNIEnv *env = 0;
    jvm->AttachCurrentThread(&env,NULL);

    if (UGoCBClass)
    {
        env->DeleteGlobalRef(UGoCBClass);
        UGoCBClass = NULL;
    }

    if (UGoCBObject)
    {
        env->DeleteGlobalRef(UGoCBObject);
        UGoCBObject = NULL;
    }
}


extern "C" jint Java_com_gl_softphone_UGoManager_UGoStartRecord(JNIEnv*  env
        ,jobject  thiz, IN jobject para)
{
    int ret = -1;

    if ( para == NULL )
    {
        LOGE("UGoStartRecord: para is null.");
        return ret;
    }

    jclass  envClass = (jclass)env->NewGlobalRef(env->GetObjectClass(para));
    jfieldID filepath_id = env->GetFieldID(envClass, "filePath", "Ljava/lang/String;");
    jfieldID iFileFormat_id = env->GetFieldID(envClass, "iFileFormat", "I");
    jfieldID iRecordMode_id = env->GetFieldID(envClass, "iRecordMode", "I");
    jstring   jfilepath = (jstring)env->GetObjectField(para,filepath_id);
    int iFileFormat = env->GetIntField(para,iFileFormat_id);
    int iRecordRecord = env->GetIntField(para, iRecordMode_id);
    const char* filePath = jfilepath?env->GetStringUTFChars(jfilepath, NULL):NULL;

    if (filePath)
    {
        media_file_record_pm_t pt;
        memset(&pt, 0, sizeof(pt));
        strncpy(pt.filepath, filePath, sizeof(pt.filepath)-1);

        pt.iFileFormat = iFileFormat;
        pt.iMode = iRecordRecord;
        ret = UGo_record_start(&pt);
        env->ReleaseStringUTFChars(jfilepath, filePath);
    }

    if ( envClass )
    {
        env->DeleteGlobalRef(envClass);
        envClass = NULL;
    }

    return ret;
}

extern "C" jint Java_com_gl_softphone_UGoManager_UGoStopRecord(JNIEnv*  env
        ,jobject  thiz)
{
    int ret = -1;
    ret = UGo_record_stop();
    return ret;
}
extern "C" jint Java_com_gl_softphone_UGoManager_UGoPlayFile(JNIEnv*  env
        ,jobject  thiz
        ,IN jobject para)
{
    int ret = -1;
    int mode = 0;

    if ( para == NULL )
    {
        LOGE("UGoPlayFile: para is null.");
        return ret;
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
            const char* filePath = jfilepath ? env->GetStringUTFChars(jfilepath, NULL) : NULL;
            if (filePath)
            {
                strncpy(pt.filepath.filename, filePath, sizeof(pt.filepath.filename) -1);
                ret = UGo_play_file(mode, &pt);
                env->ReleaseStringUTFChars(jfilepath, filePath);
            }
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
            
            LOGI("UGoPlayFile: data_size =%d, dataArray_length=%d",\
                data_size, dataArray_length);
            
            jbyte* arraybody = env->GetByteArrayElements(data_array, NULL);
            /*create jboolean buf, jboolean in c++ is unsigned char */
            buf = (jboolean *)calloc(data_size , sizeof(jboolean));
            if(buf == NULL)
            {
                LOGE("UGoPlayFile: calloc error.");
                break;
            }

            /*copy jcharArray data into buf*/
            for(i=0; i < data_size; i++)
            {
                *(buf + i) = (jboolean)(*(arraybody + i));
            }

            /*release char array*/
            env->ReleaseByteArrayElements(data_array, arraybody, 0);
            pt.filestream.data = (void *)buf;
            pt.filestream.size = dataArray_length;
            ret = UGo_play_file(mode, &pt);

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

    return ret;
}
extern "C" jint Java_com_gl_softphone_UGoManager_UGoStopFile(JNIEnv*  env
        ,jobject  thiz)
{
    int ret = -1;
    ret = UGo_stop_file();
    return ret;
}

#define MODULE_NAME(moduleId) #moduleId
char* getModuleName(int moduleId, char *moduleName)
{
    switch (moduleId )
    {
        case UGO_CFG_PARAM_MODULE_ID:
            strcpy(moduleName, MODULE_NAME(UGO_CFG_PARAM_MODULE_ID));
            break;
        case UGO_CFG_TCP_MODULE_ID:
            strcpy(moduleName, MODULE_NAME(UGO_CFG_TCP_MODULE_ID));
            break;
        case UGO_CFG_ICE_MODULE_ID:
            strcpy(moduleName, MODULE_NAME(UGO_CFG_ICE_MODULE_ID));
            break;
        case UGO_RTPP_CFG_MODULE_ID:
            strcpy(moduleName, MODULE_NAME(UGO_RTPP_CFG_MODULE_ID));
            break;
        case UGO_EMODEL_MODULE_ID:
            strcpy(moduleName, MODULE_NAME(UGO_EMODEL_MODULE_ID));
            break;
        case ME_CODECS_CFG_MODULE_ID:
            strcpy(moduleName, MODULE_NAME(ME_CODECS_CFG_MODULE_ID));
            break;
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
        case 0xabcd:
            strcpy(moduleName, "0xabcd");
            break;
        case 0xdcba:
            strcpy(moduleName, "0xdcba");
            break;
        default:
            strcpy(moduleName, "*UNKNOWN*");
            break;
    }

    return moduleName;
}

extern "C" jint Java_com_gl_softphone_UGoManager_UGoSetConfig(JNIEnv*  env
        ,jobject  thiz,IN jint ModuleID, IN jobject config, IN jint version)
{
    int ret = -1;
    char moduleName[64] = {0};
    jclass envClass = NULL;

    if ( NULL == config )
    {
        LOGE("UGoSetConfig: null config!");
        return ret;
    }

    LOGI("UGoSetConfig: %s", getModuleName(ModuleID, moduleName));

    envClass = (jclass)env->NewGlobalRef(env->GetObjectClass(config));

    /* Note: don't direct return in switch statement, need to delete global reference. */
    switch (ModuleID)
    {
        case UGO_CFG_PARAM_MODULE_ID:
        {
            ugo_cfg_t ugoConfig;
            bool rc4_enabled;
            int video_enabled;
            int  platform;
            int  atype;
            bool  tlv_enabled;
            bool  compress_enabled;
            
            //char brand[21] = {0};
            //char cliendip[64] = {0};
            jstring brand_string;
            jstring uid_string;
            jstring userid_str;
            jstring phone_string;
            jstring nickname_string;
            jstring ipaddr_string;

            jfieldID rc4_enabled_id = env->GetFieldID(envClass, "rc4_enabled", "Z");
            jfieldID com_enabled_id = env->GetFieldID(envClass, "compress_enabled", "Z");
            jfieldID tlv_enabled_id = env->GetFieldID(envClass, "tlv_enabled", "Z");
            jfieldID ipv6_enabled_id = env->GetFieldID(envClass, "ipv6_enabled", "Z");
            jfieldID csrv_enabled_id = env->GetFieldID(envClass, "csrv_enabled", "Z");
            jfieldID video_enabled_id = env->GetFieldID(envClass, "video_enabled", "I");
            jfieldID platform_id = env->GetFieldID(envClass, "platform", "I");
            jfieldID atype_id = env->GetFieldID(envClass, "atype", "I");            
            jfieldID uid_id = env->GetFieldID(envClass, "uid", "Ljava/lang/String;");
            jfieldID userid_id = env->GetFieldID(envClass, "userid", "Ljava/lang/String;");
            jfieldID brand_id = env->GetFieldID(envClass, "brand", "Ljava/lang/String;");
            jfieldID phone_id = env->GetFieldID(envClass, "phone", "Ljava/lang/String;");
            jfieldID nickname_id = env->GetFieldID(envClass, "nickname", "Ljava/lang/String;");
            jfieldID ipaddr_id = env->GetFieldID(envClass, "localipaddr", "Ljava/lang/String;");

            rc4_enabled = env->GetBooleanField(config, rc4_enabled_id);
            compress_enabled = env->GetBooleanField(config, com_enabled_id);
            video_enabled = env->GetIntField(config, video_enabled_id);
            platform = env->GetIntField(config, platform_id);
            tlv_enabled = env->GetBooleanField(config, tlv_enabled_id);
            atype = env->GetIntField(config, atype_id);
            uid_string = (jstring) env->GetObjectField(config, uid_id);
            userid_str = (jstring) env->GetObjectField(config, userid_id);
            brand_string = (jstring) env->GetObjectField(config, brand_id);
            phone_string = (jstring) env->GetObjectField(config, phone_id);
            nickname_string = (jstring) env->GetObjectField(config, nickname_id);
            ipaddr_string = (jstring) env->GetObjectField(config, ipaddr_id);

            const char* brand_char = brand_string ? env->GetStringUTFChars(brand_string, NULL) : NULL;
            const char* uid_char = uid_string ? env->GetStringUTFChars(uid_string, NULL) : NULL;
            const char* userid_char = userid_str ? env->GetStringUTFChars(userid_str, NULL) : NULL;
            const char* phone_char = phone_string ? env->GetStringUTFChars(phone_string, NULL) : NULL;
            const char* nickname_char = nickname_string ? env->GetStringUTFChars(nickname_string, NULL) : NULL;
            const char* ipaddr_char = ipaddr_string ? env->GetStringUTFChars(ipaddr_string, NULL) : NULL;
            
            memset(&ugoConfig, 0x00, UGO_CFG_T_SIZE);
            ugoConfig.rc4_enabled = rc4_enabled;
            ugoConfig.compress_enabled = compress_enabled;
            ugoConfig.video_enabled = video_enabled;
            ugoConfig.platform = platform;
            ugoConfig.atype = atype;
            ugoConfig.tlv_enabled = tlv_enabled;
            ugoConfig.ipv6_enabled = env->GetBooleanField(config, ipv6_enabled_id);
			ugoConfig.csrv_enabled = env->GetBooleanField(config, csrv_enabled_id);
            
            if (brand_char)
            {
                strncpy(ugoConfig.brand, brand_char, sizeof(ugoConfig.brand) -1);
                env->ReleaseStringUTFChars(brand_string, brand_char);
            }

            if (uid_char)
            {
                strncpy(ugoConfig.uid, uid_char, sizeof(ugoConfig.uid) -1);
                env->ReleaseStringUTFChars(uid_string, uid_char);
            }

            if ( userid_char )
            {
                strncpy(ugoConfig.userid, userid_char, sizeof(ugoConfig.userid) -1);
                env->ReleaseStringUTFChars(userid_str, userid_char);
            }

            if (phone_char)
            {
                strncpy(ugoConfig.phone, phone_char, sizeof(ugoConfig.phone) -1);
                env->ReleaseStringUTFChars(phone_string, phone_char);
            }

            if ( nickname_char )
            {
                strncpy(ugoConfig.nickname, nickname_char, sizeof(ugoConfig.nickname) -1);
                env->ReleaseStringUTFChars(nickname_string, nickname_char);
            }

            if ( ipaddr_char )
            {
                strncpy(ugoConfig.localipaddr, ipaddr_char, sizeof(ugoConfig.localipaddr) -1);
                env->ReleaseStringUTFChars(ipaddr_string, ipaddr_char);
            }

            ret = UGo_set_config(ModuleID, &ugoConfig, 0);
        }
        break;

        case UGO_CFG_TCP_MODULE_ID:
        {
            ugo_cfg_tcp_t tcpConfig;
            bool tcp_enabled;
            jstring tcp_srvaddr_string;

            jfieldID tcp_enabled_id = env->GetFieldID(envClass, "tcp_enabled", "Z");
            jfieldID tcp_srvaddr_id = env->GetFieldID(envClass, "tcp_srvaddr", "Ljava/lang/String;");
            tcp_enabled = env->GetBooleanField(config,tcp_enabled_id);
            tcp_srvaddr_string = (jstring)env->GetObjectField(config,tcp_srvaddr_id);
            const char* tcp_srvaddr_char = tcp_srvaddr_string?env->GetStringUTFChars(tcp_srvaddr_string, NULL):NULL;

            memset(&tcpConfig, 0x00, UGO_TCP_CFG_SIZE);
            tcpConfig.tcp_enabled = tcp_enabled;

            if (tcp_srvaddr_char)
            {
                strncpy(tcpConfig.tcp_srvaddr, tcp_srvaddr_char, sizeof(tcpConfig.tcp_srvaddr) -1);
                env->ReleaseStringUTFChars(tcp_srvaddr_string, tcp_srvaddr_char);
            }
            ret = UGo_set_config(ModuleID, &tcpConfig, 0);

            LOGI("UGoSetConfig: tcp{%d, %s}", tcp_enabled, tcpConfig.tcp_srvaddr);
        }
        break;

        case UGO_CFG_ICE_MODULE_ID:
        {
            ugo_cfg_ice_t iceConfig;
            bool ice_enabled;
            jstring stun_server_string;

            jfieldID ice_enabled_id = env->GetFieldID(envClass, "ice_enabled", "Z");
            jfieldID ipv6_enabled_id = env->GetFieldID(envClass, "ipv6_enabled", "Z"); 
            jfieldID stun_server_id = env->GetFieldID(envClass, "stun_server", "Ljava/lang/String;");
            ice_enabled = env->GetBooleanField(config,ice_enabled_id);
            stun_server_string=(jstring) env->GetObjectField(config,stun_server_id);
            const char* stun_server_char = stun_server_string?env->GetStringUTFChars(stun_server_string, NULL):NULL;

            memset(&iceConfig, 0x00, UGO_ICE_CFG_SIZE);
            iceConfig.ice_enabled = ice_enabled;
            iceConfig.ipv6_enabled = env->GetBooleanField(config, ipv6_enabled_id);
            if (stun_server_char)
            {
                strncpy(iceConfig.stun_server, stun_server_char, sizeof(iceConfig.stun_server) -1);
                env->ReleaseStringUTFChars(stun_server_string, stun_server_char);
            }
            ret = UGo_set_config(ModuleID, &iceConfig, 0);
            LOGI("UGoSetConfig: ice{En[%d], V6[%d], Srv[%s]}",
                iceConfig.ice_enabled, iceConfig.ipv6_enabled,
                iceConfig.stun_server);
        }
        break;

        case UGO_RTPP_CFG_MODULE_ID:
        {
            ugo_rtpp_cfg_t rtpp_cfg;
            json_t * jsonArray = NULL;
            json_t *cursor = NULL;
            int rtpp_list_len = 0;
            int i = 0;
            
            memset(&rtpp_cfg, 0, UGO_RTPP_CFG_T_SIZE);

            jfieldID rtpp_list_len_id= env->GetFieldID(envClass, "rtp_list_length", "I");
            jfieldID rtppArray_id = env->GetFieldID(envClass, "rtppcfg", "Ljava/lang/String;");

            rtpp_list_len = env->GetIntField(config,rtpp_list_len_id);
            jstring rtpp = (jstring)env->GetObjectField(config,rtppArray_id);

            const char* myrtpp = rtpp?env->GetStringUTFChars(rtpp,NULL):NULL;

            if (NULL == myrtpp)
            {
                LOGE("UGoSetConfig: RTPP LIST is NULL!!!!");
                break;
            }

            LOGI("UGoSetConfig: myrtpp = %s, rtpp_list_len=%d, myrtpp_len=%d", \
                myrtpp, rtpp_list_len, (int)strlen(myrtpp));
            
            ret = json_parse_document(&jsonArray,(char*)myrtpp);

            if( ret != 1 )
            {
                LOGE("UGoSetConfig: parse rtppjon string failed code =%d", ret);
            }
            else
            {
                cursor = jsonArray->child;

                if(cursor != NULL)
                {
                    while(cursor != NULL)
                    {
                        jsonapi_parser_string(cursor, "ip", rtpp_cfg.rtpps[i].ip);
                        jsonapi_parser_number(cursor, "lost", &rtpp_cfg.rtpps[i].lost);
                        jsonapi_parser_number(cursor, "delay", &rtpp_cfg.rtpps[i].delay);
                        cursor = cursor->next;
                        LOGI("UGoSetConfig: rtpp = { ip= %s lost= %d, delay= %d }", \
                            rtpp_cfg.rtpps[i].ip, rtpp_cfg.rtpps[i].lost, \
                            rtpp_cfg.rtpps[i].delay);
                        i++;
                    }
                    rtpp_cfg.rtpp_num = i;
                }
            }
            if ( myrtpp )
            {
                env->ReleaseStringUTFChars(rtpp, myrtpp);
            }

            if (rtpp_cfg.rtpp_num != 0)
            {
                ret = UGo_set_config(ModuleID, &rtpp_cfg, 0);
            }
        }
        break;

        /* add by VintonLiu on 20141128 for codec config */
        case ME_CODECS_CFG_MODULE_ID:
        {
            ME_codec_list_t setcodec;
            int codec_index = 0;
            
            memset(&setcodec, 0x00, sizeof(ME_codec_list_t));

            jfieldID pltype_id = env->GetFieldID(envClass, "pltype", "I");
            jfieldID plname_id = env->GetFieldID(envClass, "plname", "Ljava/lang/String;");
            /* del by Vinton.Liu on 2016-01-29 for not used begin */
#if 0
            jfieldID plfreq_id = env->GetFieldID(envClass, "plfreq", "I");
            jfieldID pacsize_id = env->GetFieldID(envClass, "pacsize", "I");
            jfieldID channels_id = env->GetFieldID(envClass, "channels", "I");
            jfieldID rate_id = env->GetFieldID(envClass, "rate", "I");
#endif
            /* Vinton.Liu del end */
            jfieldID enabled_id = env->GetFieldID(envClass, "enabled", "I");

            jstring plname = (jstring)env->GetObjectField(config, plname_id);
            const char* pPlname = plname ? env->GetStringUTFChars(plname, NULL) : NULL;
            
            setcodec.num = 1;
            if ( pPlname == NULL )
            {
                strcpy(setcodec.codecs[codec_index].plname, "");    
            }
            else
            {
                strncpy(setcodec.codecs[codec_index].plname, pPlname, \
                        sizeof(setcodec.codecs[codec_index].plname) -1);
            }

            setcodec.codecs[codec_index].pltype = env->GetIntField(config, pltype_id);
            /* del by Vinton.Liu on 2016-01-29 for not used begin */
#if 0
            setcodec.codecs[codec_index].plfreq = env->GetIntField(config, plfreq_id);
            setcodec.codecs[codec_index].pacsize = env->GetIntField(config, pacsize_id);
            setcodec.codecs[codec_index].channels = env->GetIntField(config, channels_id);
            setcodec.codecs[codec_index].rate = env->GetIntField(config, rate_id);
#endif
            /* Vinton.Liu del end */
            setcodec.codecs[codec_index].enabled = env->GetIntField(config, enabled_id);

            /* mod by Vinton.Liu on 2016-01-29 for not used begin */
#if 0
            LOGI( \
                "UGoSetConfig: codec = { t = %d, n = %s, f = %d, s = %d, c = %d, r = %d, e = %d}",\
                setcodec.codecs[codec_index].pltype, \
                setcodec.codecs[codec_index].plname, \
                setcodec.codecs[codec_index].plfreq, \
                setcodec.codecs[codec_index].pacsize, \
                setcodec.codecs[codec_index].channels, \
                setcodec.codecs[codec_index].rate, \
                setcodec.codecs[codec_index].enabled
                );
#else
            LOGI( \
                "UGoSetConfig: codec = { pt = %d, pname = %s, en = %d}",\
                setcodec.codecs[codec_index].pltype, \
                setcodec.codecs[codec_index].plname, \
                setcodec.codecs[codec_index].enabled
                );
#endif
            /* Vinton.Liu mod end */
            ret = UGo_set_config(ModuleID, &setcodec, 0);            
        }
        break;
        /* VintonLiu add end */

        case ME_CTRL_CFG_MODULE_ID:
        {
            ME_CTRL_cfg_t ctrl_config;
            unsigned int ucRealTimeType = 0;    //Real time protocol type, 0: RTP 1: PRTP
            unsigned int ucVideoEnable = 0;     //Video module, 0: disable 1: enable
            unsigned int ucEmodelEnable = 0;    //Emodel module, 0: disable 1: enable
            unsigned int ucFecEnable = 0;       //Emodel module, 0: disable 1: enable
            unsigned int ucRtpEncEnable = 0;    // Rtp encrypt, 0: disabled, 1: enabled
            unsigned int ucExtMediaProcEn = 0;  // External pcm media processing, 0: disabled, 1: enabled
            unsigned int ucAudioTransEnable;
            
            jfieldID ucRealTimeType_id =  env->GetFieldID(envClass, "ucRealTimeType", "I");
            jfieldID ucVideoEnable_id =  env->GetFieldID(envClass, "ucVideoEnable", "I");
            jfieldID ucEmodelEnable_id = env->GetFieldID(envClass, "ucEmodelEnable", "I");
            jfieldID ucFecEnable_id = env->GetFieldID(envClass, "ucFecEnable", "I");
            jfieldID ucRtpEncEnable_id = env->GetFieldID(envClass, "ucRtpEncEnable", "I");
            jfieldID ucExtMediaProcEn_id = env->GetFieldID(envClass, "ucExtMediaProcEn", "I");
            jfieldID ucTransport_id = env->GetFieldID(envClass, "ucExtAudioTransEnable", "I");
            jfieldID ipv6Enalbed_id = env->GetFieldID(envClass, "ucIPv6Enabled", "I");
            jfieldID vieFecEn_id = env->GetFieldID(envClass, "vieFecEnabled", "Z");
            jfieldID vieNackEn_id = env->GetFieldID(envClass, "vieNackEnabled", "Z");
            
            ucRealTimeType =  env->GetIntField(config,ucRealTimeType_id);
            ucVideoEnable = env->GetIntField(config,ucVideoEnable_id);
            ucEmodelEnable = env->GetIntField(config,ucEmodelEnable_id);
            ucFecEnable = env->GetIntField(config,ucFecEnable_id);
            ucRtpEncEnable = env->GetIntField(config, ucRtpEncEnable_id);
            ucExtMediaProcEn = env->GetIntField(config, ucExtMediaProcEn_id);
            ucAudioTransEnable = env->GetIntField(config, ucTransport_id);
            
            memset(&ctrl_config, 0x00, sizeof(ctrl_config));
            ctrl_config.ucRealTimeType= ucRealTimeType;
            ctrl_config.ucVideoEnable = ucVideoEnable;
            ctrl_config.ucEmodelEnable = ucEmodelEnable;
            ctrl_config.ucFecEnable = ucFecEnable;
            ctrl_config.ucRtpEncEnable = ucRtpEncEnable;
            ctrl_config.ucExtMediaProcEn = ucExtMediaProcEn;
            ctrl_config.ucExtAudioTransEn = ucAudioTransEnable;
            ctrl_config.ucIPv6Enable = env->GetIntField(config, ipv6Enalbed_id);
            ctrl_config.vieFecEnabled = env->GetBooleanField(config, vieFecEn_id);
            ctrl_config.vieNackEnabled = env->GetBooleanField(config, vieNackEn_id);

            LOGI("UGoSetConfig: ctrlCfg = { RT[%d], VE[%d], EE[%d] FE[%d]" \
                " RE[%d] EM[%d] ATE[%d] V6[%d] VFE[%d] VNE[%d]}",\
                ucRealTimeType, ucVideoEnable, ucEmodelEnable, ucFecEnable, \
                ucRtpEncEnable, ucExtMediaProcEn, ctrl_config.ucExtAudioTransEn, \
                ctrl_config.ucIPv6Enable, ctrl_config.vieFecEnabled,
                ctrl_config.vieNackEnabled);

            ret = UGo_set_config(ModuleID, &ctrl_config, 0);
        }
        break;

#ifdef VIGO_ENABLED
        case ME_VIDEO_ENC_CFG_MODULE_ID:
        {
            ME_video_enc_param_t vEncCfg;
            memset(&vEncCfg, 0, sizeof(vEncCfg));

            jfieldID ucPlName = env->GetFieldID(envClass, "ucPlName", "[Ljava/lang/String;");
            jfieldID ucPlType =  env->GetFieldID(envClass, "ucPlType", "[I");
            jfieldID enable =  env->GetFieldID(envClass, "enable", "[I");
            jfieldID usNum =  env->GetFieldID(envClass, "usNum", "I");
            jfieldID usWidth_id =  env->GetFieldID(envClass, "usWidth", "I");
            jfieldID usHeight_id =  env->GetFieldID(envClass, "usHeight", "I");
            jfieldID usStartBitrate =  env->GetFieldID(envClass, "usStartBitrate", "I");
            jfieldID usMaxBitrate =  env->GetFieldID(envClass, "usMaxBitrate", "I");
            jfieldID usMinBitrate =  env->GetFieldID(envClass, "usMinBitrate", "I");
            jfieldID ucmaxFramerate =  env->GetFieldID(envClass, "ucmaxFramerate", "I");
            jfieldID ucQpMax =  env->GetFieldID(envClass, "ucQpMax", "I");
            jfieldID ucComplexity =  env->GetFieldID(envClass, "ucComplexity", "I");
            jfieldID ucFirstSelectedPt =  env->GetFieldID(envClass, "ucFirstSelectedPt", "I");
            jfieldID ucUseHwEnc =  env->GetFieldID(envClass, "ucUseHwEnc", "I");
			jfieldID ucUseHwDec = env->GetFieldID(envClass, "ucUseHwDec", "I");
			
            jfieldID bitrates =  env->GetFieldID(envClass, "bitrates", "[[I");
            jfieldID presets =  env->GetFieldID(envClass, "presets", "[[I");
            jfieldID fps =  env->GetFieldID(envClass, "fps", "[[I");

		   {
				jint i,j;
				jobjectArray	bitratesArray = (jobjectArray)env->GetObjectField(config, bitrates);
				int row = (env)->GetArrayLength( bitratesArray);//获得行数    
				jarray myarray = (jarray)env->GetObjectArrayElement(bitratesArray , 0);  
		
				int col = (env)->GetArrayLength( myarray);//获得列数    
				//LOGT("row=%d,col=%d\n",row,col);
		
				for(i=0;i<row;i++)
				{
					myarray = (jarray)(env->GetObjectArrayElement(bitratesArray , i));  
					jint *data = env->GetIntArrayElements((jintArray)myarray, NULL );  
					for (j=0; j<col; j++) {  
						vEncCfg.enc_param_t.bitrates[i][j]= data[j];
						//LOGT("row=%d,col=%d,data=%d\n",i,j,data[j]);
					}  
					env->ReleaseIntArrayElements((jintArray)myarray, data,0);
				}
		   }


		   {
				jint i,j;
				jobjectArray	presetsArray = (jobjectArray)env->GetObjectField(config, presets);
				int row = (env)->GetArrayLength( presetsArray);//获得行数    
				jarray myarray = (jarray)env->GetObjectArrayElement(presetsArray , 0);  
		
				int col = (env)->GetArrayLength( myarray);//获得列数    
				//LOGT("row=%d,col=%d\n",row,col);
		
				for(i=0;i<row;i++)
				{
					myarray = (jarray)(env->GetObjectArrayElement(presetsArray , i));  
					jint *data = env->GetIntArrayElements((jintArray)myarray, NULL );  
					for (j=0; j<col; j++) {  
						vEncCfg.enc_param_t.presets[i][j]= data[j];
						//LOGT("row=%d,col=%d,data=%d\n",i,j,data[j]);
					}  
					env->ReleaseIntArrayElements((jintArray)myarray, data,0);
				}
		   }


		   {
				jint i,j;
				jobjectArray	fpsArray = (jobjectArray)env->GetObjectField(config, fps);
				int row = (env)->GetArrayLength( fpsArray);//获得行数    
				jarray myarray = (jarray)env->GetObjectArrayElement(fpsArray , 0);  
		
				int col = (env)->GetArrayLength( myarray);//获得列数    
				//LOGT("row=%d,col=%d\n",row,col);
		
				for(i=0;i<row;i++)
				{
					myarray = (jarray)(env->GetObjectArrayElement(fpsArray , i));  
					jint *data = env->GetIntArrayElements((jintArray)myarray, NULL );  
					for (j=0; j<col; j++) {  
						vEncCfg.enc_param_t.fps[i][j]= data[j];
						//LOGT("row=%d,col=%d,data=%d\n",i,j,data[j]);
					}  
					env->ReleaseIntArrayElements((jintArray)myarray, data,0);
				}
		   }


            jobjectArray nameArray = (jobjectArray) (env)->GetObjectField(config, ucPlName);
            jsize theNameLen = env->GetArrayLength(nameArray);
            for( int i = 0; i < theNameLen; i++)
            {
                 jstring string = (jstring)env->GetObjectArrayElement(nameArray, i);                 
                 const char * chars = env->GetStringUTFChars( string, 0);
                 strcpy((char*)vEncCfg.baseCodec[i].ucPlName, (char*)chars);
                 env->ReleaseStringUTFChars( string, chars);
            }

            jintArray plTypeArray = (jintArray) (env)->GetObjectField(config, ucPlType);
            jsize plTypeLen = env->GetArrayLength(plTypeArray);
            int *pl = env->GetIntArrayElements (plTypeArray, NULL);   
            for(int i=0; i<plTypeLen; i++)
            {                              
                 vEncCfg.baseCodec[i].ucPlType = pl[i];                 
            }
            
            jintArray enableArray = (jintArray) (env)->GetObjectField(config, enable);
            jsize enableLen = env->GetArrayLength(enableArray);
            int *en = env->GetIntArrayElements(enableArray, NULL);  
            for(int i=0; i<enableLen; i++)
            {                              
                 vEncCfg.baseCodec[i].enable = en[i];    
            }

            vEncCfg.usNum = env->GetIntField(config, usNum);
            vEncCfg.usWidth = env->GetIntField(config,usWidth_id);
            vEncCfg.usHeight = env->GetIntField(config,usHeight_id);          
            vEncCfg.usStartBitrate = env->GetIntField(config,usStartBitrate);
            vEncCfg.ucMaxFramerate = env->GetIntField(config,ucmaxFramerate);
            vEncCfg.ucQpMax = env->GetIntField(config,ucQpMax);
            vEncCfg.usMaxBitrate = env->GetIntField(config,usMaxBitrate);
            vEncCfg.usMinBitrate = env->GetIntField(config,usMinBitrate);
            vEncCfg.ucComplexity = env->GetIntField(config,ucComplexity);
            vEncCfg.ucFirstSelectedPt = env->GetIntField(config,ucFirstSelectedPt);
            vEncCfg.ucUseHwEnc = env->GetIntField(config,ucUseHwEnc);
			vEncCfg.ucUseHwDec = env->GetIntField(config, ucUseHwDec);
			
            LOGI("UGoSetConfig: width = %d, height = %d, bitrate = %d, maxbitrate = %d, FirstSelectedPt = %d, UseHwEnc = %d, UseHwDec = %d",\
                vEncCfg.usWidth, vEncCfg.usHeight, \
                vEncCfg.usStartBitrate, vEncCfg.usMaxBitrate, \
				vEncCfg.ucFirstSelectedPt, vEncCfg.ucUseHwEnc, vEncCfg.ucUseHwDec);
				
            ret = UGo_set_config(ModuleID, &vEncCfg,0);
        }
        break;

        case ME_VIDEO_DEC_CFG_MODULE_ID:
        {
            ME_video_dec_info_t vDecCfg;
            memset(&vDecCfg, 0, sizeof(vDecCfg));

            jfieldID uiMaxDefinition_id =  env->GetFieldID(envClass, "uiMaxDefinition", "I");    
            jfieldID ucmaxFramerate =  env->GetFieldID(envClass, "ucmaxFramerate", "I");

            vDecCfg.uiMaxDefinition = env->GetIntField(config,uiMaxDefinition_id);
            vDecCfg.ucMaxFramerate = env->GetIntField(config,ucmaxFramerate);

            ret = UGo_set_config(ModuleID, &vDecCfg,0);
        }
        break;
        
        case ME_VIDEO_EXTERN_CAPTURE_CFG_MODULE_ID:
        {
            ME_video_extern_capture_param_t vCaptureCfg;
            memset(&vCaptureCfg, 0, sizeof(vCaptureCfg));

            jfieldID useExternCapture =  env->GetFieldID(envClass, "useExternCapture", "Z");    
            jfieldID ucExternformate =  env->GetFieldID(envClass, "ucExternformate", "I");

            vCaptureCfg.useExternCapture = env->GetBooleanField(config,useExternCapture);
            vCaptureCfg.ucExternformate = env->GetIntField(config,ucExternformate);

            ret = UGo_set_config(ModuleID, &vCaptureCfg,0);
        }
        break;

        case ME_VIDEO_RENDER_CFG_MODULE_ID:
        {
            ME_video_render_param_t para;
            
            memset(&para, 0, sizeof(para));
            UGo_get_config(ModuleID, &para, 0);

			jfieldID remoteWidth_id = env->GetFieldID(envClass, "remoteWidth", "I");
			jfieldID remoteHeight_id = env->GetFieldID(envClass, "remoteHeight", "I");
			jfieldID renderMode_id = env->GetFieldID(envClass, "renderMode", "I");
            jfieldID pWindowRemote =  env->GetFieldID(envClass, "pWindowRemote", "Ljava/lang/Object;");
            jfieldID pWindowLocal =  env->GetFieldID(envClass, "pWindowLocal", "Ljava/lang/Object;");

			para.remoteHeight = env->GetIntField(config, remoteHeight_id);
			para.remoteWidth = env->GetIntField(config, remoteWidth_id);
			para.renderMode=env->GetIntField(config, renderMode_id);
            para.pWindowRemote = (void*)env->GetObjectField(config,pWindowRemote);
            para.pWindowLocal = (void*)env->GetObjectField(config,pWindowLocal);

            ret = UGo_set_config(ModuleID, &para,0);
        }
        break;

        case ME_VIDEO_PROCES_CFG_MODULE_ID:
        {
            ME_video_proces_module_t para;
            unsigned short usWidth;
            unsigned short usHeight;
            unsigned int   uistartBitrate;
            unsigned char  ucmaxFramerate;

            jfieldID deflickingEnable =env->GetFieldID(envClass, "deflickingEnable", "Z");
            jfieldID denoisingEnable =env->GetFieldID(envClass, "denoisingEnable", "Z");
            jfieldID enhancementEnable =env->GetFieldID(envClass, "enhancementEnable", "Z");

            memset(&para, 0, sizeof(para));
            para.deflickingEnable = env->GetBooleanField(config, deflickingEnable);
            para.denoisingEnable =  env->GetBooleanField(config, denoisingEnable);
            para.enhancementEnable=  env->GetBooleanField(config, enhancementEnable);

            ret = UGo_set_config(ModuleID, &para,0);
        }
        break;
#endif

        case ME_VQE_CFG_MODULE_ID:
        {
            ME_VQE_cfg_t vqe_config;
            bool Ec_enable;             //Enable EC function on send port if TRUE, else disable
            bool Agc_enable;            //Enable Agc function on send port if TRUE, else disable
            bool Ns_enable;             //Enable Ns function on send port if TRUE, else disable
            bool Agc_Rx_enable;         //Enable Agc function on receive port if TRUE, else disable
            bool Ns_Rx_enable;
			bool Dual_Mic_Ns_enable;    //Enable dual microphone noise suppression  added by chgx 20160505

            jfieldID Ec_enable_id =env->GetFieldID(envClass, "Ec_enable", "Z");
            jfieldID Agc_enable_id =env->GetFieldID(envClass, "Agc_enable", "Z");
            jfieldID Ns_enable_id =env->GetFieldID(envClass, "Ns_enable", "Z");
            jfieldID Agc_Rx_enable_id =env->GetFieldID(envClass, "Agc_Rx_enable", "Z");
            jfieldID Ns_Rx_enable_id =env->GetFieldID(envClass, "Ns_Rx_enable", "Z");
            jfieldID Agc_targetDbfs_id = env->GetFieldID(envClass, "Agc_targetDbfs", "I");
            jfieldID Agc_compressionGaindB_id = env->GetFieldID(envClass, "Agc_compressionGaindB", "I");
			jfieldID Dual_Mic_Ns_enable_id =env->GetFieldID(envClass, "Dual_Mic_Ns_enable", "Z");// added by chgx 20160505
            
            Ec_enable = env->GetBooleanField(config, Ec_enable_id);
            Agc_enable = env->GetBooleanField(config, Agc_enable_id);
            Ns_enable = env->GetBooleanField(config, Ns_enable_id);
            Agc_Rx_enable = env->GetBooleanField(config, Agc_Rx_enable_id);
            Ns_Rx_enable = env->GetBooleanField(config, Ns_Rx_enable_id);    
			Dual_Mic_Ns_enable = env->GetBooleanField(config, Dual_Mic_Ns_enable_id);    // added by chgx 20160505			
            
            memset(&vqe_config, 0x00, sizeof(vqe_config));
            vqe_config.Ec_enable = Ec_enable;
            vqe_config.Agc_enable = Agc_enable;
            vqe_config.Ns_enable = Ns_enable;
            vqe_config.Agc_Rx_enable = Agc_Rx_enable;
            vqe_config.Ns_Rx_enable = Ns_Rx_enable;
            vqe_config.Agc_targetDbfs = env->GetIntField(config, Agc_targetDbfs_id);
            vqe_config.Agc_compressionGaindB = env->GetIntField(config, Agc_compressionGaindB_id);
			vqe_config.Dual_Mic_Ns_enable = Dual_Mic_Ns_enable;// added by chgx 20160505

            LOGI("UGoSetConfig: vqe_cfg={Ec = %d, Agc = %d, Ns = %d, RAgc = %d," \
                 " RNs = %d, AgcT = %d, AgcM = %d,  dualMic=%d}", \
                 Ec_enable, Agc_enable, Ns_enable, \
                 Agc_Rx_enable, Ns_Rx_enable, vqe_config.Agc_targetDbfs, \
                 vqe_config.Agc_compressionGaindB,vqe_config.Dual_Mic_Ns_enable);

            ret = UGo_set_config(ModuleID, &vqe_config, 0);
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

            memset(&rtp_config, 0x00, sizeof(rtp_config));
            rtp_config.uiRTPTimeout = uiRTPTimeout;
            rtp_config.uiFixLowPayload = uiFixLowPayload;
            LOGI("UGoSetConfig: rtpCfg = { timeout =%d, fixlowBitrate = %d }", \
                 uiRTPTimeout, uiFixLowPayload);

            ret = UGo_set_config(ModuleID, &rtp_config, 0);
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
            status = env->GetBooleanField(config, status_id);
            dialogScene = env->GetIntField(config, dialogScene_id);
            networktype = env->GetIntField(config, networktype_id);

            memset(&env_st, 0x00, sizeof(env_st));
            env_st.status = status;
            env_st.dialogScene = dialogScene;
            env_st.networktype = networktype;

            ret =  UGo_set_config(ModuleID, &env_st, 0);
        }
        break;

        /* add by VintonLiu on 20141114 for conference auto test */
#ifdef AUTO_TEST_SWITCH
        case 0xabcd:
        {
            auto_test_crtl_t stAutoTestCtrl; 
            int isConfCall = 0;
            int callRole = 1;            
            int callCnt = 5;
            int callTime = 62;
            int callInterval = 5;
            memset(&stAutoTestCtrl, 0x00, sizeof(stAutoTestCtrl));            

            jfieldID isConfCall_id = env->GetFieldID(envClass, "isConfCall", "I");
            jfieldID callRole_id = env->GetFieldID(envClass, "callRole", "I");            
            jfieldID callCnt_id = env->GetFieldID(envClass, "callCnt", "I");
            jfieldID callTime_id = env->GetFieldID(envClass, "callTime", "I");
            jfieldID callInterval_id = env->GetFieldID(envClass, "callInterval", "I");
            isConfCall = env->GetIntField(config, isConfCall_id);
            callRole = env->GetIntField(config, callRole_id);            
            callCnt = env->GetIntField(config, callCnt_id);
            callTime = env->GetIntField(config, callTime_id);
            callInterval = env->GetIntField(config, callInterval_id);

            if ( callRole == 1 && isConfCall == 0 )
            {
                call_dialing_pm_t callparam;
                
                jstring uid_string;
                jstring phone_string;
                int mode_flag;
                int video_flag;
                int call_type_flag;
    
                jfieldID use_id = env->GetFieldID(envClass, "uid","Ljava/lang/String;");
                jfieldID phone_number = env->GetFieldID(envClass, "phone", "Ljava/lang/String;");
                jfieldID mode_type = env->GetFieldID(envClass, "mode", "I");
                jfieldID video_flage = env->GetFieldID(envClass, "video_enable", "I");
                jfieldID call_type = env->GetFieldID(envClass, "ucalltype", "I");

                uid_string   = (jstring)env->GetObjectField(config, use_id);
                phone_string = (jstring)env->GetObjectField(config, phone_number);
                mode_flag    = env->GetIntField(config,mode_type);
                video_flag   = env->GetIntField(config, video_flage);
                call_type_flag = env->GetIntField(config, call_type);

                const char* uid_char = uid_string?env->GetStringUTFChars(uid_string, NULL):NULL;
                const char* phone_char = phone_string?env->GetStringUTFChars(phone_string, NULL):NULL;

                memset(&callparam, 0, sizeof(call_dialing_pm_t));
                callparam.call_mode = mode_flag;
                callparam.video_enabled = video_flag;
                callparam.calltype = call_type_flag;

                if(uid_char)
                {
                    strncpy(callparam.uid, uid_char, sizeof(callparam.uid) -1);
                    env->ReleaseStringUTFChars(uid_string,uid_char);
                }
                if(phone_char)
                {
                    strncpy(callparam.uphone, phone_char, sizeof(callparam.uphone) - 1);
                    env->ReleaseStringUTFChars(phone_string,phone_char);
                }
                memcpy(&stAutoTestCtrl.dialCfg.dialParam, &callparam, sizeof(callparam));
            } 
            else if ( isConfCall == 1 )
            {
                /* caller, need dial param */
                conference_dialing_param_t conference_param;
                memset(&conference_param, 0x00, sizeof(conference_param));
                json_t * jsonArray = NULL;
                json_t *cursor = NULL;
                int participant_num = 0;
                int refFileIdx = 0;
                int testFileIdx = 0;
                int i = 0;

                jfieldID testFileIdx_id = env->GetFieldID(envClass, "testFileIdx", "I");
                jfieldID refFileIdx_id = env->GetFieldID(envClass, "refFileIdx", "I");
                refFileIdx = env->GetIntField(config, refFileIdx_id);
                testFileIdx = env->GetIntField(config, testFileIdx_id);

                if ( callRole == 1 )
                {
                    jfieldID participant_len_id= env->GetFieldID(envClass,"user_num","I");
                    jfieldID conferenceArray_id = env->GetFieldID(envClass,"user_attr","Ljava/lang/String;");
                    jfieldID roomName_id = env->GetFieldID(envClass, "roomname", "Ljava/lang/String;");
                    jfieldID roomPwd_id = env->GetFieldID(envClass, "roompwd", "Ljava/lang/String;");
                    jfieldID remark_id = env->GetFieldID(envClass, "remark", "Ljava/lang/String;");

                    jstring participant_array = (jstring)env->GetObjectField(config,conferenceArray_id);
                    jstring jsRoomName = (jstring)env->GetObjectField(config, roomName_id);
                    jstring jsRoomPwd = (jstring)env->GetObjectField(config, roomPwd_id);
                    jstring jsRemark = (jstring)env->GetObjectField(config, remark_id);

                    const char* myparticipant_array = participant_array?env->GetStringUTFChars(participant_array, NULL):NULL;
                    const char* pRoomName = jsRoomName ? env->GetStringUTFChars(jsRoomName, NULL) : NULL;
                    const char* pRoomPwd = jsRoomPwd ? env->GetStringUTFChars(jsRoomPwd, NULL) : NULL;
                    const char* pRemark = jsRemark ? env->GetStringUTFChars(jsRemark, NULL) : NULL;
                    participant_num = env->GetIntField(config, participant_len_id);

                    if (NULL == myparticipant_array)
                    {
                        LOGE("UGoSetConfig: my participant array is NULL!!!!" );
                        break;
                    }

                    if ( pRoomName )
                    {
                        strncpy(conference_param.roomname, pRoomName, sizeof(conference_param.roomname) -1 );
                        env->ReleaseStringUTFChars(jsRoomName, pRoomName);
                    }

                    if ( pRoomPwd )
                    {
                        strncpy(conference_param.roompwd, pRoomPwd, sizeof(conference_param.roompwd) -1 );
                        env->ReleaseStringUTFChars(jsRoomPwd, pRoomPwd);
                    }

                    if ( pRemark )
                    {
                        strncpy(conference_param.remark, pRemark, sizeof(conference_param.remark) -1 );
                        env->ReleaseStringUTFChars(jsRemark, pRemark);
                    }

                    LOGI("UGoSetConfig: roomName = %s", conference_param.roomname);
                    LOGI("UGoSetConfig: roomPwd = %s", conference_param.roompwd);
                    LOGI("UGoSetConfig: remark = %s", conference_param.remark);
                    LOGI("UGoSetConfig: myparticipant_array = %s",\
                        myparticipant_array);

                    ret = json_parse_document(&jsonArray,(char*)myparticipant_array);

                    if ( ret != 1)
                    {
                        LOGE("UGoSetConfig: myparticipant_array=%s", myparticipant_array);
                        LOGE("UGoSetConfig: parse participant json string failed code = %d", ret);
                    }
                    else
                    {
                        cursor = jsonArray->child;

                        if(cursor != NULL)
                        {
                            while(cursor != NULL)
                            {
                                jsonapi_parser_number(cursor, "mode", &conference_param.user_attr[i].mode);
                                jsonapi_parser_string(cursor, "uid", conference_param.user_attr[i].uid);
                                jsonapi_parser_string(cursor, "phone", conference_param.user_attr[i].phone);
                                cursor = cursor->next;

                                LOGI("UGoSetConfig: mode = %d uid= %s, phone= %s",\
                                    conference_param.user_attr[i].mode, \
                                    conference_param.user_attr[i].uid, \
                                    conference_param.user_attr[i].phone);

                                i++;
                            }

                            conference_param.user_num = i;
                            LOGI("UGoSetConfig: conference_param.user_num = %d", \
                                conference_param.user_num);
                        }

                    }
                    if ( myparticipant_array )
                    {
                        env->ReleaseStringUTFChars(participant_array,myparticipant_array);
                    }
                }
                
                stAutoTestCtrl.confRefFileIdx = refFileIdx;
                stAutoTestCtrl.confFileIndex = testFileIdx;
                memcpy(&stAutoTestCtrl.dialCfg.confDialCfg, &conference_param, sizeof(conference_param));
            }

            /***test option***/
        	stAutoTestCtrl.fecCtrl = 1; 		//fec function test switch.
        	stAutoTestCtrl.p2pCtrl = 1;		//P2P function test switch.
        	stAutoTestCtrl.prtpCtrl = 1;		//PRTP function test switch.
            stAutoTestCtrl.emodelCtr = 1;    //emodel function test switch.
        	stAutoTestCtrl.playCtrl = 1;		//play function test switch.
        	stAutoTestCtrl.recCtrl = 1;		//recording function test switch.
        	stAutoTestCtrl.logCtrl = 1;		//log function test switch.
        	stAutoTestCtrl.p862Ctrl = 1;	    //p862 function test switch. 
            stAutoTestCtrl.encryptCtrl = 0;    //encryption of rtp function test switch. 
            stAutoTestCtrl.exlDevCtrl = 0;    //external playing device function test switch. 
        	stAutoTestCtrl.videoCtrl = 0;    //video function test switch. 

            /***call option***/
            stAutoTestCtrl.isConfCall = isConfCall;            
            stAutoTestCtrl.callRole = callRole;
            stAutoTestCtrl.callCnt = callCnt;      //call count
            stAutoTestCtrl.callTime = callTime;     //call time every time, unit:s    
            stAutoTestCtrl.callInterval = callInterval; //call interval every time, unit:s

            LOGI("UGoSetConfig: test_param = { %d, %d, %d, %d, %d, %d }", \
                stAutoTestCtrl.confFileIndex, \
                stAutoTestCtrl.confRefFileIdx, \
                stAutoTestCtrl.callRole, \
                stAutoTestCtrl.callCnt, \
                stAutoTestCtrl.callTime, \
                stAutoTestCtrl.callInterval);

            Start_auto_test(&stAutoTestCtrl);
        }
            break;

        case 0xdcba:
            Stop_auto_test();
            break;
#endif
        /* VintonLiu add end */

        default:
            break;
    }

    /* delete global reference */
    if ( envClass )
    {
        env->DeleteGlobalRef(envClass);
        envClass = NULL;
    }

    LOGI("UGoSetConfig: return ret = %d", ret);

    return ret;
}

extern "C" jint Java_com_gl_softphone_UGoManager_UGoGetConfig(JNIEnv*  env
        ,jobject  thiz,IN jint ModuleID,IN jobject config,IN jint version)
{
    int ret = -1;
    char moduleName[64] = {0};
    jclass envClass = NULL;

    if ( NULL == config )
    {
        LOGE("UGoGetConfig: null config!");
        return ret;
    }

    LOGI("UGoGetConfig: %s", getModuleName(ModuleID, moduleName));

    envClass = (jclass)env->NewGlobalRef(env->GetObjectClass(config));

    /* Note: don't direct return in switch statement, need to delete global reference. */
    switch (ModuleID)
    {
        case UGO_CFG_PARAM_MODULE_ID:
        {
            ugo_cfg_t ugo_config;

            memset(&ugo_config, 0x00, sizeof(ugo_config));
            ret = UGo_get_config(ModuleID, &ugo_config, 0);
            if ( ret != 0 )
            {
                break;
            }

            jfieldID rc4_enabled_id = env->GetFieldID(envClass, "rc4_enabled", "Z");
			jfieldID compress_enabled_id = env->GetFieldID(envClass, "compress_enabled", "Z");
            jfieldID video_enabled_id = env->GetFieldID(envClass, "video_enabled", "I");
            jfieldID platform_id = env->GetFieldID(envClass, "platform", "I");
            jfieldID tlv_enabled_id = env->GetFieldID(envClass, "tlv_enabled", "Z");
            jfieldID ipv6Enalbed_id = env->GetFieldID(envClass, "ipv6_enabled", "Z");
            jfieldID csrv_enabled_id = env->GetFieldID(envClass, "csrv_enabled", "Z");
            jfieldID atype_id = env->GetFieldID(envClass, "atype", "I");
            jfieldID brand_id = env->GetFieldID(envClass, "brand", "Ljava/lang/String;");
            jfieldID uid_id = env->GetFieldID(envClass, "uid", "Ljava/lang/String;");
			jfieldID userid_id = env->GetFieldID(envClass, "userid", "Ljava/lang/String;");
            jfieldID phone_id = env->GetFieldID(envClass, "phone", "Ljava/lang/String;");
			jfieldID nickname_id = env->GetFieldID(envClass, "nickname", "Ljava/lang/String;");
            jfieldID ipaddr_id = env->GetFieldID(envClass, "localipaddr", "Ljava/lang/String;");

            env->SetBooleanField(config, rc4_enabled_id, ugo_config.rc4_enabled);
			env->SetBooleanField(config, compress_enabled_id, ugo_config.compress_enabled);
            env->SetIntField(config, video_enabled_id, ugo_config.video_enabled);
            env->SetIntField(config, platform_id, ugo_config.platform);
            env->SetBooleanField(config, tlv_enabled_id, ugo_config.tlv_enabled);
            env->SetBooleanField(config, ipv6Enalbed_id, ugo_config.ipv6_enabled);
            env->SetBooleanField(config, csrv_enabled_id, ugo_config.csrv_enabled);
            env->SetIntField(config, atype_id, ugo_config.atype);
            env->SetObjectField(config,brand_id,env->NewStringUTF(ugo_config.brand));
            env->SetObjectField(config,uid_id,env->NewStringUTF(ugo_config.uid));
			env->SetObjectField(config,userid_id,env->NewStringUTF(ugo_config.userid));
            env->SetObjectField(config,phone_id,env->NewStringUTF(ugo_config.phone));
			env->SetObjectField(config,nickname_id,env->NewStringUTF(ugo_config.nickname));
            env->SetObjectField(config, ipaddr_id, env->NewStringUTF(ugo_config.localipaddr));
        }
        break;

        case UGO_CFG_TCP_MODULE_ID:
        {
            ugo_cfg_tcp_t tcp_config;

            memset(&tcp_config, 0x00, sizeof(tcp_config));
            ret = UGo_get_config(ModuleID, &tcp_config, 0);
            if ( ret != 0 )
            {
                break;
            }

            jfieldID tcp_enabled_id =env->GetFieldID(envClass, "tcp_enabled", "Z");
            jfieldID tcp_srvaddr_id = env->GetFieldID(envClass, "tcp_srvaddr", "Ljava/lang/String;");
            env->SetBooleanField(config,tcp_enabled_id,tcp_config.tcp_enabled);
            env->SetObjectField(config,tcp_srvaddr_id,env->NewStringUTF(tcp_config.tcp_srvaddr));
        }
        break;

        case UGO_CFG_ICE_MODULE_ID:
        {
            ugo_cfg_ice_t ice_config;

            memset(&ice_config, 0x00, sizeof(ice_config));
            ret = UGo_get_config(ModuleID, &ice_config, 0);
            if ( ret != 0 )
            {
                break;
            }

            jfieldID ice_enabled_id =env->GetFieldID(envClass, "ice_enabled", "Z");
            jfieldID ipv6_enabled_id = env->GetFieldID(envClass, "ipv6_enabled", "Z"); 
            jfieldID stun_server_id =env->GetFieldID(envClass, "stun_server", "Ljava/lang/String;");

            env->SetBooleanField(config, ice_enabled_id, ice_config.ice_enabled);
            env->SetBooleanField(config, ipv6_enabled_id, ice_config.ipv6_enabled);            
            env->SetObjectField(config, stun_server_id, env->NewStringUTF(ice_config.stun_server));

            LOGI("UGoGetConfig: ice{En[%d], V6[%d], Srv[%s]}",
                ice_config.ice_enabled, ice_config.ipv6_enabled,
                ice_config.stun_server);
        }
        break;

        case ME_CTRL_CFG_MODULE_ID:
        {
            ME_CTRL_cfg_t ctrl_config;

            memset(&ctrl_config, 0x00, sizeof(ctrl_config));
            ret = UGo_get_config(ModuleID, &ctrl_config, 0);
            if (ret != 0)
            {
                break;
            }

            jfieldID ucRealTimeType_id =env->GetFieldID(envClass, "ucRealTimeType", "I");
            jfieldID ucVideoEnable_id =env->GetFieldID(envClass, "ucVideoEnable", "I");
            jfieldID ucEmodelEnable_id =env->GetFieldID(envClass, "ucEmodelEnable", "I");
            jfieldID ucFecEnable_id =env->GetFieldID(envClass, "ucFecEnable", "I");
            jfieldID ucRtpEncEnable_id = env->GetFieldID(envClass, "ucRtpEncEnable", "I");
            jfieldID ucExtMediaProcEn_id = env->GetFieldID(envClass, "ucExtMediaProcEn", "I");
            jfieldID ucTransport_id = env->GetFieldID(envClass, "ucExtAudioTransEnable", "I");
            jfieldID ipv6Enalbed_id = env->GetFieldID(envClass, "ucIPv6Enabled", "I");
            jfieldID vieFecEn_id = env->GetFieldID(envClass, "vieFecEnabled", "Z");
            jfieldID vieNackEn_id = env->GetFieldID(envClass, "vieNackEnabled", "Z");
            
            env->SetIntField(config, ucRealTimeType_id, (int)ctrl_config.ucRealTimeType);
            env->SetIntField(config, ucVideoEnable_id, (int)ctrl_config.ucVideoEnable);
            env->SetIntField(config, ucEmodelEnable_id, (int)ctrl_config.ucEmodelEnable);
            env->SetIntField(config, ucFecEnable_id, (int)ctrl_config.ucFecEnable);
            env->SetIntField(config, ucRtpEncEnable_id, (int)ctrl_config.ucRtpEncEnable);
            env->SetIntField(config, ucExtMediaProcEn_id, (int)ctrl_config.ucExtMediaProcEn);
            env->SetIntField(config, ucTransport_id, (int)ctrl_config.ucExtAudioTransEn);
            env->SetIntField(config, ipv6Enalbed_id, (int)ctrl_config.ucIPv6Enable);
            env->SetBooleanField(config, vieFecEn_id, ctrl_config.vieFecEnabled);
            env->SetBooleanField(config, vieNackEn_id, ctrl_config.vieNackEnabled);
            
            LOGI("UGoGetConfig: ctrl_cfg = { RT[%d], VE[%d], EE[%d] FE[%d]"
                " RE[%d] EM[%d] ATE[%d] V6[%d] VFE[%d] VNE[%d]}", \
                ctrl_config.ucRealTimeType, ctrl_config.ucVideoEnable, \
                ctrl_config.ucEmodelEnable, ctrl_config.ucFecEnable, \
                ctrl_config.ucRtpEncEnable, ctrl_config.ucExtMediaProcEn, \
                ctrl_config.ucExtAudioTransEn, ctrl_config.ucIPv6Enable, \
                ctrl_config.vieFecEnabled, ctrl_config.vieNackEnabled);
        }
        break;

        case ME_VQE_CFG_MODULE_ID:
        {
            ME_VQE_cfg_t vqe_config;

            memset(&vqe_config, 0x00, sizeof(vqe_config));
            ret = UGo_get_config(ModuleID, &vqe_config, 0);
            if (ret != 0)
            {
                break;
            }

            jfieldID Ec_enable_id =env->GetFieldID(envClass, "Ec_enable", "Z");
            jfieldID Agc_enable_id =env->GetFieldID(envClass, "Agc_enable", "Z");
            jfieldID Ns_enable_id =env->GetFieldID(envClass, "Ns_enable", "Z");
            jfieldID Agc_Rx_enable_id =env->GetFieldID(envClass, "Agc_Rx_enable", "Z");
            jfieldID Ns_Rx_enable_id =env->GetFieldID(envClass, "Ns_Rx_enable", "Z");
            jfieldID Agc_targetDbfs_id = env->GetFieldID(envClass, "Agc_targetDbfs", "I");
            jfieldID Agc_compressionGaindB_id = env->GetFieldID(envClass, "Agc_compressionGaindB", "I");
            jfieldID Dual_Mic_Ns_enable_id =env->GetFieldID(envClass, "Dual_Mic_Ns_enable", "Z");
				
            env->SetBooleanField(config,Ec_enable_id,vqe_config.Ec_enable);
            env->SetBooleanField(config,Agc_enable_id,vqe_config.Agc_enable);
            env->SetBooleanField(config,Ns_enable_id,vqe_config.Ns_enable);
            env->SetBooleanField(config,Agc_Rx_enable_id,vqe_config.Agc_Rx_enable);
            env->SetBooleanField(config,Ns_Rx_enable_id,vqe_config.Ns_Rx_enable);
            env->SetIntField(config, Agc_targetDbfs_id, vqe_config.Agc_targetDbfs);
            env->SetIntField(config, Agc_compressionGaindB_id, vqe_config.Agc_compressionGaindB);
			env->SetBooleanField(config, Dual_Mic_Ns_enable_id, vqe_config.Dual_Mic_Ns_enable);
    
            LOGI("UGoGetConfig: vqe_cfg={Ec = %d, Agc = %d, Ns = %d, RAgc = %d," \
                " RNs = %d , AgcT = %d, AgcM = %d, Dual_Mic_Ns = %d}",\
                vqe_config.Ec_enable, vqe_config.Agc_enable,\
                vqe_config.Ns_enable, vqe_config.Agc_Rx_enable, \
                vqe_config.Ns_Rx_enable, vqe_config.Agc_targetDbfs, \
                vqe_config.Agc_compressionGaindB, vqe_config.Dual_Mic_Ns_enable);
        }
        break;

        case ME_RTP_CFG_MODULE_ID:
        {
            ME_RTP_cfg_t rtp_config;

            memset(&rtp_config, 0x00, sizeof(rtp_config));
            ret = UGo_get_config(ModuleID, &rtp_config, 0);
            if (ret != 0)
            {
                break;
            }

            jfieldID uiRTPTimeout_id =  env->GetFieldID(envClass, "uiRTPTimeout", "I");
            jfieldID uiFixLowPayload_id =  env->GetFieldID(envClass, "uiFixLowPayload", "Z");
            env->SetIntField(config,uiRTPTimeout_id,rtp_config.uiRTPTimeout);
            env->SetBooleanField(config,uiFixLowPayload_id,rtp_config.uiFixLowPayload);

            LOGI("UGoGetConfig: Rtp_cfg={%d, %d}", \
                rtp_config.uiRTPTimeout, rtp_config.uiFixLowPayload);
        }
        break;

        case ME_ENV_CFG_MODULE_ID:
        {
            ME_ENV_cfg_t env_st;

            memset(&env_st, 0x00, sizeof(ME_ENV_cfg_t));
            ret = UGo_get_config(ModuleID, &env_st, 0);

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

            LOGI("UGoGetConfig: env_cfg={%d, %d, %d}", \
                env_st.status, env_st.dialogScene, env_st.networktype);
        }
        break;

        case ME_CODECS_CFG_MODULE_ID:
        {
            ME_codec_list_t codecs_list;

            memset(&codecs_list, 0x00, sizeof(codecs_list));
            ret= UGo_get_config(ModuleID, &codecs_list, 0);
            if (ret != 0)
            {
                break;
            }
            
            json_t *codecs_json = NULL;
            json_t *jtemp      = NULL;
            json_t* jcodecarray = NULL;
            json_t* jobj = NULL;
            char strtmp[64]    = {0};
            char* codecs_buf    = NULL;
            int codecs_num = 0;

            codecs_json = jsonapi_new_value(JSON_OBJECT, NULL);
            
            if ( codecs_list.num != 0 )
            {
                jcodecarray = jsonapi_new_value(JSON_ARRAY, NULL);
                for (int i = 0; i < codecs_list.num; i++)
                {   
                    jobj = jsonapi_new_value(JSON_OBJECT, NULL);

                    sprintf(strtmp, "%d", codecs_list.codecs[i].pltype);
                    jtemp = jsonapi_new_value(JSON_NUMBER, strtmp);
                    jsonapi_append_value(jobj, "pltype", jtemp);

                    jtemp = jsonapi_new_value(JSON_STRING, codecs_list.codecs[i].plname);
                    jsonapi_append_value(jobj, "plname", jtemp);

                    /* del by Vinton.Liu on 2016-01-29 for not used begin */
#if 0
                    sprintf(strtmp, "%d", codecs_list.codecs[i].plfreq);
                    jtemp = jsonapi_new_value(JSON_NUMBER, strtmp);
                    jsonapi_append_value(jobj, "plfreq", jtemp);

                    sprintf(strtmp, "%d", codecs_list.codecs[i].pacsize);
                    jtemp = jsonapi_new_value(JSON_NUMBER, strtmp);
                    jsonapi_append_value(jobj, "pacsize", jtemp);

                    sprintf(strtmp, "%d", codecs_list.codecs[i].channels);
                    jtemp = jsonapi_new_value(JSON_NUMBER, strtmp);
                    jsonapi_append_value(jobj, "channels", jtemp);

                    sprintf(strtmp, "%d", codecs_list.codecs[i].rate);
                    jtemp = jsonapi_new_value(JSON_NUMBER, strtmp);
                    jsonapi_append_value(jobj, "rate", jtemp);
#endif
                    /* Vinton.Liu del end */
                    
                    sprintf(strtmp, "%d", codecs_list.codecs[i].enabled);
                    jtemp = jsonapi_new_value(JSON_NUMBER, strtmp);
                    jsonapi_append_value(jobj, "enabled", jtemp);

                    jsonapi_append_value(jcodecarray, NULL, jobj);
                    codecs_num++;
                }

                sprintf(strtmp, "%d", codecs_num);
                jtemp     = jsonapi_new_value (JSON_NUMBER, strtmp);
                jsonapi_append_value(codecs_json, "codec_nums", jtemp);
                
                jsonapi_append_value(codecs_json, "codecs", jcodecarray);                
                jsonapi_value_to_string(codecs_json, &codecs_buf);
            }

            LOGI("UGoGetConfig: codec_num = %d, codecs types= %s", \
                codecs_num, codecs_buf);

            jfieldID codec_nums = env->GetFieldID(envClass, "codec_nums", "I");
            jfieldID codec_type = env->GetFieldID(envClass, "codec_types", "Ljava/lang/String;");
            env->SetIntField(config, codec_nums, (int)codecs_num);
            env->SetObjectField(config, codec_type, env->NewStringUTF(codecs_buf));
        }
        break;

#ifdef VIGO_ENABLED
        case ME_VIDEO_ENC_CFG_MODULE_ID:
        {
            ME_video_enc_param_t vEncCfg;

            memset(&vEncCfg, 0, sizeof(vEncCfg));
            ret = UGo_get_config(ModuleID, &vEncCfg, 0);
            if ( ret != 0 )
            {
                break;
            }

            jfieldID ucPlName = env->GetFieldID(envClass, "ucPlName", "[Ljava/lang/String;");
            jfieldID ucPlType =  env->GetFieldID(envClass, "ucPlType", "[I");
            jfieldID enable =  env->GetFieldID(envClass, "enable", "[I");
            jfieldID usNum =  env->GetFieldID(envClass, "usNum", "I");
            jfieldID usWidth_id =  env->GetFieldID(envClass, "usWidth", "I");
            jfieldID usHeight_id =  env->GetFieldID(envClass, "usHeight", "I");
            jfieldID usStartBitrate =  env->GetFieldID(envClass, "usStartBitrate", "I");
            jfieldID usMaxBitrate =  env->GetFieldID(envClass, "usMaxBitrate", "I");
            jfieldID usMinBitrate =  env->GetFieldID(envClass, "usMinBitrate", "I");
            jfieldID ucmaxFramerate =  env->GetFieldID(envClass, "ucmaxFramerate", "I");
            jfieldID ucQpMax =  env->GetFieldID(envClass, "ucQpMax", "I");
            jfieldID ucComplexity =  env->GetFieldID(envClass, "ucComplexity", "I");
            jfieldID ucFirstSelectedPt =  env->GetFieldID(envClass, "ucFirstSelectedPt", "I");
            jfieldID ucUseHwEnc =  env->GetFieldID(envClass, "ucUseHwEnc", "I");
			jfieldID ucUseHwDec = env->GetFieldID(envClass, "ucUseHwDec", "I");

            jobjectArray nameArray = (jobjectArray) (env)->GetObjectField(config, ucPlName);
            jsize theNameLen = env->GetArrayLength(nameArray);
            for(int i=0; i<theNameLen; i++)
            {
                 jstring string = env->NewStringUTF((const char*)vEncCfg.baseCodec[i].ucPlName);
                 env->SetObjectArrayElement(nameArray, i, string);                 
            }

            jintArray plTypeArray = (jintArray) (env)->GetObjectField(config, ucPlType);
            jsize plTypeLen = env->GetArrayLength(plTypeArray);
            for(int i=0; i<plTypeLen; i++)
            {                              
                 int pl = vEncCfg.baseCodec[i].ucPlType; 
                 env->SetIntArrayRegion(plTypeArray, i,1,&pl);     
            }
            
            jintArray enableArray = (jintArray) (env)->GetObjectField(config, enable);
            jsize enableLen = env->GetArrayLength(enableArray);
            for(int i=0; i<enableLen; i++)
            {                              
            	 int en = vEncCfg.baseCodec[i].enable;  
                 env->SetIntArrayRegion(enableArray, i,1, &en);          
            }
            env->SetIntField(config, usNum,  vEncCfg.usNum);
            env->SetIntField(config, usWidth_id,  vEncCfg.usWidth);
            env->SetIntField(config, usHeight_id, vEncCfg.usHeight);          
            env->SetIntField(config, usStartBitrate, vEncCfg.usStartBitrate);
            env->SetIntField(config, ucmaxFramerate, vEncCfg.ucMaxFramerate);
            env->SetIntField(config, ucQpMax, vEncCfg.ucQpMax);
            env->SetIntField(config, usMaxBitrate, vEncCfg.usMaxBitrate);
            env->SetIntField(config, usMinBitrate, vEncCfg.usMinBitrate);
            env->SetIntField(config, ucComplexity, vEncCfg.ucComplexity);
            env->SetIntField(config, ucFirstSelectedPt, vEncCfg.ucFirstSelectedPt);
            env->SetIntField(config, ucUseHwEnc, vEncCfg.ucUseHwEnc);
			env->SetIntField(config, ucUseHwDec, vEncCfg.ucUseHwDec);
            
            LOGI("UGoGetConfig: codec type %s, width = %d, height = %d, bitrate = %d, maxbitrate = %d, FirstSelectedPt = %d, UseHwEnc = %d, UseHwDec = %d",\
            	vEncCfg.baseCodec[0].ucPlName, vEncCfg.usWidth, vEncCfg.usHeight, \
                vEncCfg.usStartBitrate, vEncCfg.usMaxBitrate, \
				vEncCfg.ucFirstSelectedPt, vEncCfg.ucUseHwEnc, vEncCfg.ucUseHwDec);
        }
        break;

        case ME_VIDEO_DEC_CFG_MODULE_ID:
        {
            ME_video_dec_info_t  para;

            memset(&para, 0, sizeof(para));
            ret = UGo_get_config(ModuleID, &para,0);
            if ( ret != 0 )
            {
                break;
            }

            jfieldID uiMaxDefinition_id =  env->GetFieldID(envClass, "uiMaxDefinition", "I");           
            jfieldID ucmaxFramerate =  env->GetFieldID(envClass, "ucmaxFramerate", "I");

            env->SetIntField(config, uiMaxDefinition_id,  para.uiMaxDefinition);          
            env->SetIntField(config, ucmaxFramerate, para.ucMaxFramerate);
        }
        break;
        
          case ME_VIDEO_EXTERN_CAPTURE_CFG_MODULE_ID:
        {
            ME_video_extern_capture_param_t para;
            memset(&para, 0, sizeof(para));
            ret = UGo_get_config(ModuleID, &para,0);
            if ( ret != 0 )
            {
                break;
            }

            jfieldID useExternCapture =  env->GetFieldID(envClass, "useExternCapture", "Z");    
            jfieldID ucExternformate =  env->GetFieldID(envClass, "ucExternformate", "I");
          

            env->SetBooleanField(config, useExternCapture,  para.useExternCapture);          
            env->SetIntField(config, ucExternformate, para.ucExternformate);
        }
        break;

        case ME_VIDEO_RENDER_CFG_MODULE_ID:
        {
            ME_video_render_param_t para;

            memset(&para, 0, sizeof(para));
            ret = UGo_get_config(ModuleID, &para,0);
            if (ret <0)
            {
                break;
            }

            jfieldID pWindowRemote =  env->GetFieldID(envClass, "pWindowRemote", "Ljava/lang/Object;");
            jfieldID pWindowLocal =  env->GetFieldID(envClass, "pWindowLocal", "Ljava/lang/Object;");

            env->SetObjectField(config, pWindowRemote, (jobject)para.pWindowRemote);
            env->SetObjectField(config, pWindowLocal, (jobject)para.pWindowLocal);
        }
        break;

        case ME_VIDEO_PROCES_CFG_MODULE_ID:
        {
            ME_video_proces_module_t para;

            memset(&para, 0, sizeof(para));
            ret = UGo_get_config(ModuleID, &para,0);
            if (ret < 0)
            {
                break;
            }

            jfieldID deflickingEnable =env->GetFieldID(envClass, "deflickingEnable", "Z");
            jfieldID denoisingEnable =env->GetFieldID(envClass, "denoisingEnable", "Z");
            jfieldID enhancementEnable =env->GetFieldID(envClass, "enhancementEnable", "Z");

            env->SetBooleanField(config, deflickingEnable, para.deflickingEnable);
            env->SetBooleanField(config, denoisingEnable, para.denoisingEnable);
            env->SetBooleanField(config, enhancementEnable, para.enhancementEnable);
        }
        break;
#endif

        default:
            break;
    }

    /* delete global reference */
    if ( envClass )
    {
        env->DeleteGlobalRef(envClass);
        envClass = NULL;
    }

    LOGI("UGoGetConfig: successed. ret = %d", ret);
    return ret;
}

extern "C" jint Java_com_gl_softphone_UGoManager_UGoVideoSetCamera(JNIEnv*  env
        ,jobject  thiz,
        IN jobject para)
{

    int res = -1;
    //unsigned int  web_camera_idx = 0;
    //int rotate = -1;
    jclass envClass = NULL;
    ME_video_camera_param_t pm;

    if ( para == NULL )
    {
        LOGE("UGoVideoSetCamera: para is NULL!" );
        return res;
    }

    envClass = (jclass)env->NewGlobalRef(env->GetObjectClass(para));
    jfieldID idx = env->GetFieldID(envClass, "usCameraIdx", "I");
    jfieldID iRotate = env->GetFieldID(envClass, "iRotateAngle", "I");
    jfieldID iHeignt = env->GetFieldID(envClass, "usHeight", "I");
    jfieldID iWidth = env->GetFieldID(envClass, "usWidth", "I");
    jfieldID iFps = env->GetFieldID(envClass, "ucMaxFps", "I");
      

    memset(&pm, 0x00, sizeof(pm));
    pm.usCameraIdx = env->GetIntField(para, idx);
    pm.usRotateAngle = env->GetIntField(para, iRotate);
    pm.usHeight = env->GetIntField(para, iHeignt);
    pm.usWidth = env->GetIntField(para, iWidth);
    pm.ucMaxFps = env->GetIntField(para, iFps);
   

    if(envClass)
    {
        env->DeleteGlobalRef(envClass);
        envClass = NULL;
    }

    res = UGo_video_set_camera(&pm);

    return res;
}

extern "C" jint Java_com_gl_softphone_UGoManager_UGoStartVideo(JNIEnv*  env
        ,jobject  thiz,
        IN jint sendReceive)
{
    int res = -1;

    res = UGo_video_start(sendReceive);

    return res;
}

extern "C" jint Java_com_gl_softphone_UGoManager_UGoVideoSetSendReciveRotation(JNIEnv*  env
        ,jobject  thiz,
        IN jint send_rotation, IN jint recive_rotation)
{
    int res = -1;

    res = UGo_video_set_send_recive_rotation(send_rotation, recive_rotation);

    return res;
}


extern "C" jint Java_com_gl_softphone_UGoManager_UGoStopVideo(JNIEnv*  env
        ,jobject  thiz,
        IN jint sendReceive)
{
    int res = -1;

    res = UGo_video_stop(sendReceive);

    return res;
}

extern "C" jint Java_com_gl_softphone_UGoManager_UGoScreenshotStart(JNIEnv*  env
        ,jobject  thiz,
        IN jint islocal, IN jint type)
{
    int res = -1;
    static ScreenType t[] = {ABGR, ARGB, RGB24};
    if( type > 2 || type < 0)
	return res;	

    res = UGo_screenshot_start(islocal, t[type]);

    return res;
}

extern "C" jint Java_com_gl_softphone_UGoManager_UGoVideoGetCameraState(JNIEnv*  env
        ,jobject  thiz,
        IN jobject para)
{
    int res = -1;
    ME_video_camera_param_t info;
    jclass envClass = NULL;

    if ( para == NULL )
    {
        LOGE("UGoVideoGetCameraState: para is NULL!" );
        return res;
    }

    memset(&info, 0x00, sizeof(info));
    res =  UGo_video_get_camera_state(&info);

    if (res < 0)
    {
        LOGE("UGoVideoGetCameraState: get camera state failed." );
        return res;
    }

    envClass = (jclass)env->NewGlobalRef(env->GetObjectClass(para));
    jfieldID idx = env->GetFieldID(envClass, "usCameraIdx", "I");
    jfieldID iRotate = env->GetFieldID(envClass, "iRotateAngle", "I");
    jfieldID iHeignt = env->GetFieldID(envClass, "usHeight", "I");
    jfieldID iWidth = env->GetFieldID(envClass, "usWidth", "I");
    jfieldID iFps = env->GetFieldID(envClass, "ucMaxFps", "I");
    env->SetIntField(para, idx, info.usCameraIdx);
    env->SetIntField(para, iRotate, info.usRotateAngle);
    env->SetIntField(para, iHeignt, info.usHeight);
    env->SetIntField(para, iWidth, info.usWidth);
    env->SetIntField(para, iFps, info.ucMaxFps);

    if(envClass)
    {
        env->DeleteGlobalRef(envClass);
        envClass = NULL;
    }

    return res;
}

extern "C" jint Java_com_gl_softphone_UGoManager_UGoConferenceDial(JNIEnv*  env
        ,jobject  thiz,IN jobject obj)
{
    ugo_conf_dialing_para_t conference_param;
    jclass envClass = NULL;
    json_t * jsonArray = NULL;
    json_t *cursor = NULL;
    int participant_num = 0;
    int i = 0;
    int ret = -1;

    if ( obj == NULL )
    {
        LOGE("UGoConferenceDial: obj is NULL!" );
        return ret;
    }

    memset(&conference_param, 0, UGO_CONF_DIALING_PARA_SIZE);
    envClass = (jclass)env->NewGlobalRef(env->GetObjectClass(obj));

    jfieldID participant_len_id= env->GetFieldID(envClass,"user_num","I");
    jfieldID conftype_id = env->GetFieldID(envClass, "conftype", "I");
    jfieldID participantArray_id = env->GetFieldID(envClass,"user_attr","Ljava/lang/String;");
    jfieldID roomName_id = env->GetFieldID(envClass, "roomname", "Ljava/lang/String;");
    jfieldID roomPwd_id = env->GetFieldID(envClass, "roompwd", "Ljava/lang/String;");
    jfieldID remark_id = env->GetFieldID(envClass, "remark", "Ljava/lang/String;");

    jstring participant_array = (jstring)env->GetObjectField(obj, participantArray_id);
    jstring jsRoomName = (jstring)env->GetObjectField(obj, roomName_id);
    jstring jsRoomPwd = (jstring)env->GetObjectField(obj, roomPwd_id);
    jstring jsRemark = (jstring)env->GetObjectField(obj, remark_id);

    const char* myparticipant_array = participant_array?env->GetStringUTFChars(participant_array,NULL):NULL;
    const char* pRoomName = jsRoomName ? env->GetStringUTFChars(jsRoomName, NULL) : NULL;
    const char* pRoomPwd = jsRoomPwd ? env->GetStringUTFChars(jsRoomPwd, NULL) : NULL;
    const char* pRemark = jsRemark ? env->GetStringUTFChars(jsRemark, NULL) : NULL;
    participant_num = env->GetIntField(obj,participant_len_id);
    conference_param.conftype = env->GetIntField(obj, conftype_id);

    if ( NULL == myparticipant_array )
    {
        LOGE("UGoConferenceDial: participant array is NULL!!!!" );

        if ( envClass )
        {
            env->DeleteGlobalRef(envClass);
            envClass = NULL;
        }
        return -1;
    }

    /* add by Vinton.Liu on 20141020 for conference extend */
    if ( pRoomName )
    {
        strncpy(conference_param.roomname, pRoomName, sizeof(conference_param.roomname) -1 );
        env->ReleaseStringUTFChars(jsRoomName, pRoomName);
    }

    if ( pRoomPwd )
    {
        strncpy(conference_param.roompwd, pRoomPwd, sizeof(conference_param.roompwd) -1 );
        env->ReleaseStringUTFChars(jsRoomPwd, pRoomPwd);
    }

    if ( pRemark )
    {
        strncpy(conference_param.remark, pRemark, sizeof(conference_param.remark) -1 );
        env->ReleaseStringUTFChars(jsRemark, pRemark);
    }
    /* Vinton.Liu add end */

    LOGI("UGoConferenceDial: roomName = %s roomPwd = %s remark = %s confType = %d", \
            conference_param.roomname, conference_param.roompwd, \
            conference_param.remark, conference_param.conftype);

    ret = json_parse_document(&jsonArray, (char*)myparticipant_array);
    if ( ret != 1)
    {
        LOGE("UGoConferenceDial: myparticipant_array = %s", myparticipant_array);
        LOGE("UGoConferenceDial: parse participant json string failed code = %d", ret);
    }
    else
    {
        cursor = jsonArray->child;

        if(cursor != NULL)
        {
            while(cursor != NULL && i < UGO_CONF_PARTICIPANT_MAX_NUM)
            {
                jsonapi_parser_number(cursor, "mode", &conference_param.user_attr[i].call_mode);
                jsonapi_parser_string(cursor, "uid", conference_param.user_attr[i].uid);
                jsonapi_parser_string(cursor, "phone", conference_param.user_attr[i].phone);
                cursor = cursor->next;

                LOGI("UGoConferenceDial: mode = %d uid= %s, phone= %s",\
                    conference_param.user_attr[i].call_mode, \
                    conference_param.user_attr[i].uid, \
                    conference_param.user_attr[i].phone);

                i++;
            }

            conference_param.user_num = i;
            LOGI("UGoConferenceDial: conference_param.user_num = %d", \
                conference_param.user_num);
        }

    }
    if ( myparticipant_array )
    {
        env->ReleaseStringUTFChars(participant_array,myparticipant_array);
    }

    ret = UGo_conference_call_dialing(&conference_param);
    
    if ( envClass )
    {
        env->DeleteGlobalRef(envClass);
        envClass = NULL;
    }

    LOGI("UGoConferenceDial: ret = %d",ret);
    return ret;
}

extern "C" jint Java_com_gl_softphone_UGoManager_UGoConferenceInv(JNIEnv*  env
        ,jobject  thiz,IN jobject obj)
{
    ugo_conf_dialing_para_t conference_param;
    jclass envClass = NULL;
    json_t * jsonArray = NULL;
    json_t *cursor = NULL;
    int participant_num = 0;
    int conftype = 0;
    int i = 0;
    int ret = -1;

    if ( obj == NULL )
    {
        LOGE("UGoConferenceInv: obj is NULL!" );
        return ret;
    }

    memset(&conference_param, 0, UGO_CONF_DIALING_PARA_SIZE);
    envClass = (jclass)env->NewGlobalRef(env->GetObjectClass(obj));

    jfieldID participant_len_id= env->GetFieldID(envClass,"user_num","I");
    jfieldID conftype_id = env->GetFieldID(envClass, "conftype", "I");
    jfieldID participantArray_id = env->GetFieldID(envClass,"user_attr","Ljava/lang/String;");
    jfieldID roomName_id = env->GetFieldID(envClass, "roomname", "Ljava/lang/String;");
    jfieldID roomPwd_id = env->GetFieldID(envClass, "roompwd", "Ljava/lang/String;");
    jfieldID remark_id = env->GetFieldID(envClass, "remark", "Ljava/lang/String;");

    jstring participant_array = (jstring)env->GetObjectField(obj, participantArray_id);
    jstring jsRoomName = (jstring)env->GetObjectField(obj, roomName_id);
    jstring jsRoomPwd = (jstring)env->GetObjectField(obj, roomPwd_id);
    jstring jsRemark = (jstring)env->GetObjectField(obj, remark_id);

    const char* myparticipant_array = participant_array?env->GetStringUTFChars(participant_array,NULL):NULL;
    const char* pRoomName = jsRoomName ? env->GetStringUTFChars(jsRoomName, NULL) : NULL;
    const char* pRoomPwd = jsRoomPwd ? env->GetStringUTFChars(jsRoomPwd, NULL) : NULL;
    const char* pRemark = jsRemark ? env->GetStringUTFChars(jsRemark, NULL) : NULL;
    participant_num = env->GetIntField(obj,participant_len_id);
    conftype = env->GetIntField(obj, conftype_id);

    if ( NULL == myparticipant_array )
    {
        LOGE("UGoConferenceInv: participant array is NULL!!!!" );

        if ( envClass )
        {
            env->DeleteGlobalRef(envClass);
            envClass = NULL;
        }
        return -1;
    }

    if ( pRoomName )
    {
        strncpy(conference_param.roomname, pRoomName, sizeof(conference_param.roomname) -1 );
        env->ReleaseStringUTFChars(jsRoomName, pRoomName);
    }

    if ( pRoomPwd )
    {
        strncpy(conference_param.roompwd, pRoomPwd, sizeof(conference_param.roompwd) -1 );
        env->ReleaseStringUTFChars(jsRoomPwd, pRoomPwd);
    }

    if ( pRemark )
    {
        strncpy(conference_param.remark, pRemark, sizeof(conference_param.remark) -1 );
        env->ReleaseStringUTFChars(jsRemark, pRemark);
    }

    LOGI("UGoConferenceInv: roomName = %s roomPwd = %s remark = %s confType = %d", \
            conference_param.roomname, conference_param.roompwd, \
            conference_param.remark, conference_param.conftype);

    ret = json_parse_document(&jsonArray, (char*)myparticipant_array);
    if ( ret != 1)
    {
        LOGE("UGoConferenceInv: myparticipant_array = %s", myparticipant_array);
        LOGE("UGoConferenceInv: parse participant json string failed code = %d", ret);
    }
    else
    {
        cursor = jsonArray->child;

        if(cursor != NULL)
        {
            while(cursor != NULL && i < UGO_CONF_PARTICIPANT_MAX_NUM )
            {
                jsonapi_parser_number(cursor, "mode", &conference_param.user_attr[i].call_mode);
                jsonapi_parser_string(cursor, "uid", conference_param.user_attr[i].uid);
                jsonapi_parser_string(cursor, "phone", conference_param.user_attr[i].phone);
                cursor = cursor->next;

                LOGI("UGoConferenceInv: mode = %d uid= %s, phone= %s",\
                    conference_param.user_attr[i].call_mode, \
                    conference_param.user_attr[i].uid, \
                    conference_param.user_attr[i].phone);

                i++;
            }

            conference_param.user_num = i;
            LOGI("UGoConferenceInv: conference_param.user_num = %d", \
                conference_param.user_num);
        }

    }
    if ( myparticipant_array )
    {
        env->ReleaseStringUTFChars(participant_array,myparticipant_array);
    }

    ret = UGo_conference_invite_parties(&conference_param);
    
    if ( envClass )
    {
        env->DeleteGlobalRef(envClass);
        envClass = NULL;
    }

    LOGI("UGoConferenceInv: ret = %d",ret);
    return ret;
}

extern "C" jint Java_com_gl_softphone_UGoManager_UGoConferenceDel(JNIEnv*  env
        ,jobject  thiz, IN jint reason)
{
    return UGo_delete_conference(reason);
}
#endif

