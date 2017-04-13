package com.kct.flyrtc.utils;

import android.content.Context;

import java.util.ArrayList;

/**
 * Created by zhouwq on 2017/3/8/008.
 * 保存APP数据
 */

public class UIData {
    // 当前选择的id(老接口)
    public static int nSelect = -1;
    // 登录返回的数据(老接口)
    public static String accountSid = "";
    public static String accountToken = "";
    public static String accountAppid = "";
    public static ArrayList<String> clientId = new ArrayList<>();
    public static ArrayList<String> clientPwd = new ArrayList<>();
    public static ArrayList<String> clientNum = new ArrayList<>();

    // 当前选择的id(老接口)
    public static int nSelectNew = -1;
    // 登录返回的数据(新接口)
    public static String accountSidNew = "";
    public static String accountTokenNew = "";
    public static String accountAppidNew = "";
    public static ArrayList<String> clientIdNew = new ArrayList<>();
    public static ArrayList<String> clientPwdNew = new ArrayList<>();
    public static ArrayList<String> clientNumNew = new ArrayList<>();

    // 保存是否通话状态
    private static final String KC_DEMO = "flyRTC_demo";

    public static String getCallType(Context mContext) {
        return mContext.getSharedPreferences(KC_DEMO, 0).getString(KC_DEMO + "_CALL_TYPE", "");
    }

    public static void saveCallType(Context mContext, String type) {
        mContext.getSharedPreferences(KC_DEMO, 0).edit().putString(KC_DEMO + "_CALL_TYPE", type).apply();
    }

    // 清楚所有数据
    public static void clearData() {
        nSelect = -1;
        accountSid = "";
        accountToken = "";
        accountAppid = "";
        clientId.clear();
        clientPwd.clear();
        clientNum.clear();

        nSelectNew = -1;
        accountSidNew = "";
        accountTokenNew = "";
        accountAppidNew = "";
        clientIdNew.clear();
        clientPwdNew.clear();
        clientNumNew.clear();
    }
}
