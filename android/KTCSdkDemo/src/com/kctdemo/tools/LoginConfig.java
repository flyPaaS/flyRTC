package com.kctdemo.tools;


import android.content.Context;

public class LoginConfig {

	public static final String KC_DEMO = "kc_demo";
	
	public static String getCurrentClientId(Context mContext){
		return mContext.getSharedPreferences(KC_DEMO, 0).getString(KC_DEMO+"_CLIENT_ID", "");
	}
	
	public static void saveCurrentClientId(Context mContext,String clientid){
		mContext.getSharedPreferences(KC_DEMO, 0).edit().putString(KC_DEMO+"_CLIENT_ID", clientid).commit();
	}
	
	public static void getCurrentClientMobile(){
		
	}
	
	public static void saveClientId(Context mContext,String cleintId){
		mContext.getSharedPreferences(KC_DEMO, 0).edit().putString(KC_DEMO+"INPUT_CLIENT_ID", cleintId).commit();
	}
	
	public static void saveClientPwd(Context mContext,String cleintPwd){
		mContext.getSharedPreferences(KC_DEMO, 0).edit().putString(KC_DEMO+"INPUT_CLIENT_PWD", cleintPwd).commit();
	}
	
	public static String getClientId(Context mContext){
		return mContext.getSharedPreferences(KC_DEMO, 0).getString(KC_DEMO+"INPUT_CLIENT_ID", "");
	}
	
	public static String getClientPwd(Context mContext){
		return mContext.getSharedPreferences(KC_DEMO, 0).getString(KC_DEMO+"INPUT_CLIENT_PWD", "");
	}
	
	public static void saveCurrentCall(Context mContext,int isCall){
		mContext.getSharedPreferences(KC_DEMO, 0).edit().putInt(KC_DEMO+"_CURRNET_CALL_INDEX", isCall).commit();
	}
	
	public static int getCurrentCall(Context mContext){
		return mContext.getSharedPreferences(KC_DEMO, 0).getInt(KC_DEMO+"_CURRNET_CALL_INDEX", 0);
	}
	
	public static void saveCurrentSidAndToken(Context mContext,String sid,String token){
		mContext.getSharedPreferences(KC_DEMO, 0).edit().putString(KC_DEMO+"_CURRNET_SID", sid+"="+token).commit();
	}
	
	public static String[] getCurrentSidAndToken(Context mContext){
		String sid = mContext.getSharedPreferences(KC_DEMO, 0).getString(KC_DEMO+"_CURRNET_SID", "");
		if(sid.contains("=")){
			return sid.split("=");
		}else{
			return null;
		}
	}
	
	public static void saveCurrentSid(Context mContext,String sid){
		mContext.getSharedPreferences(KC_DEMO, 0).edit().putString(KC_DEMO+"_CURRNET_LOGIN_SID", sid).commit();
	}
	
	public static String getCurrentSid(Context mContext){
		return mContext.getSharedPreferences(KC_DEMO, 0).getString(KC_DEMO+"_CURRNET_LOGIN_SID", "");
	}
}
