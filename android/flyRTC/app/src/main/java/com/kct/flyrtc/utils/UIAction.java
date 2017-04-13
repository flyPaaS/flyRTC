package com.kct.flyrtc.utils;

import com.yzx.api.UCSCall;

import java.util.HashMap;

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

    // 通话状态集合
    public static final HashMap<Integer, String> dialState = new HashMap<>();

    static {
        dialState.put(UCSCall.CALL_VOIP_NOT_ENOUGH_BALANCE, "余额不足");
        dialState.put(UCSCall.CALL_VOIP_BUSY, "对方正忙");
        dialState.put(UCSCall.CALL_VOIP_REFUSAL, "对方拒绝");
        dialState.put(UCSCall.CALL_VOIP_NUMBER_ERROR, "被叫号码不在线");
        dialState.put(UCSCall.CALL_VOIP_REJECT_ACCOUNT_FROZEN, "被号账户被冻结");
        dialState.put(UCSCall.CALL_VOIP_ACCOUNT_FROZEN, "账户被冻结");
        dialState.put(UCSCall.CALL_VOIP_ACCOUNT_EXPIRED, "主号账户过期");
        dialState.put(UCSCall.CALL_VOIP_CALLYOURSELF, "不能拨打自己绑定号码");
        dialState.put(UCSCall.CALL_VOIP_NETWORK_TIMEOUT, "呼叫请求超时");
        dialState.put(UCSCall.CALL_VOIP_NOT_ANSWER, "对方无人应答");
        dialState.put(UCSCall.CALL_VOIP_TRYING_183, "被叫不在线,转直拨");
        dialState.put(UCSCall.CALL_VOIP_RINGING_180, "对方正在响铃");
        dialState.put(UCSCall.CALL_VOIP_SESSION_EXPIRATION, "鉴权失败(TCP未认证)");
        dialState.put(UCSCall.CALL_VOIP_ERROR, "服务器错误");
        dialState.put(UCSCall.HUNGUP_NOT_ANSWER, "被叫方没有应答");
        dialState.put(UCSCall.HUNGUP_MYSELF, "自己挂断电话");
        dialState.put(UCSCall.HUNGUP_OTHER, "对方挂断电话");
        dialState.put(UCSCall.HUNGUP_WHILE_2G, "2G不能拨打免费、直拨、视频和智能通话");
        dialState.put(UCSCall.HUNGUP_REFUSAL, "对方拒绝接听");
        dialState.put(UCSCall.HUNGUP_NOT_ENOUGH_BALANCE, "余额不足");
        dialState.put(UCSCall.HUNGUP_MYSELF_REFUSAL, "自己拒绝接听");
        dialState.put(UCSCall.CALL_VIDEO_DOES_NOT_SUPPORT, "该设备不支持视频通话");
    }
}
