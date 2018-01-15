package com.kct.flyrtc.utils;

/**
 * Created by zhouwq on 2017/3/8/008.
 * APP广播定义
 */

public class UIAction {
    // 登录CS响应
    public static String ACTION_LOGIN_RESPONSE = "com.login.response";
    // 登录CS响应返回码参数
    public static final String RESULT_KEY = "result";

    // 网络状态
    public static String ACTION_NETWORK_STATE = "com.network.state";
    // 网络状态参数
    public static final String NETWORK_STATE = "state";
    public static final String NETWORK_STRING = "msg";

    // 通话计时
    public static String ACTION_CALL_TIME = "com.call_time";
    // 计时参数
    public static final String CALL_TIME_TOTAL = "call_time_int";
    public static final String CALL_TIME_STRING = "call_time_str";

    // 对方接听
    public static String ACTION_ANSWER = "com.dail.answer";
    // 更新通话状态
    public static String ACTION_DIAL_STATE = "com.dial.state";
    // 免提
    public static String ACTION_DIAL_HEAD = "android.intent.action.HEADSET_PLUG";

    // 通话状态参数
    public static final String DAIL_STATE = "state";
    public static final String DAIL_STATE_ALERT = "alert";

    // 切音频
    public static String ACTION_AUDIO_MODE = "com.audio.mode";
}
