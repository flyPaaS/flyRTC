package com.kct.flyrtc.ui;

import android.app.Activity;
import android.app.KeyguardManager;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.media.AudioManager;
import android.os.Bundle;
import android.os.PowerManager;
import android.telephony.PhoneStateListener;
import android.telephony.TelephonyManager;
import android.view.KeyEvent;

import java.lang.reflect.Method;

/**
 * Created by zhouwq on 2017/3/9/009.
 * 通话页面基类
 */

public class CallActivity extends Activity {
    // 用于监听系统电话呼入
    private TelephonyManager mTelephonyManager = null;
    private MonitoringSystemCallListener mIncomingCallMonitor = null;

    // 开屏对象
    private PowerManager.WakeLock mWakeLock;
    private KeyguardManager mKeyguardManager = null;
    private KeyguardManager.KeyguardLock mKeyguardLock = null;

    public boolean isDownHome = false;
    private InnerRecevier mInnerRecevier = new InnerRecevier();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setVolumeControlStream(AudioManager.STREAM_RING);
        // 添加电话状态监听器
        addTelophonyManagerListener();
        // 初始化
        PowerManager mPowerManager = (PowerManager) getSystemService(Context.POWER_SERVICE);
        mWakeLock = mPowerManager.newWakeLock(PowerManager.ACQUIRE_CAUSES_WAKEUP | PowerManager.SCREEN_DIM_WAKE_LOCK, "CALL_ACTIVITY" + "#" + getClass().getName());
        mKeyguardManager = (KeyguardManager) getSystemService(Context.KEYGUARD_SERVICE);
        initWakeLock();
        // 注册广播
        IntentFilter mIntentFilter = new IntentFilter(Intent.ACTION_CLOSE_SYSTEM_DIALOGS);
        getApplicationContext().registerReceiver(mInnerRecevier, mIntentFilter);
    }

    @Override
    protected void onDestroy() {
        getApplicationContext().unregisterReceiver(mInnerRecevier);
        freeWakeLock();
        // 取消电话监听
        cancalTelophonyManagerListener();
        super.onDestroy();
    }

    // 添加电话状态监听器
    private void addTelophonyManagerListener() {
        if (mTelephonyManager == null) {
            mTelephonyManager = (TelephonyManager) getSystemService(Context.TELEPHONY_SERVICE);
            mIncomingCallMonitor = new MonitoringSystemCallListener();
            mTelephonyManager.listen(mIncomingCallMonitor, PhoneStateListener.LISTEN_CALL_STATE);
        }
    }

    // 取消电话监听
    private void cancalTelophonyManagerListener() {
        if (mTelephonyManager != null && mIncomingCallMonitor != null) {
            mTelephonyManager.listen(mIncomingCallMonitor, PhoneStateListener.LISTEN_NONE);
        }
    }

    class MonitoringSystemCallListener extends PhoneStateListener {
        @Override
        public void onCallStateChanged(int state, String incomingNumber) {
            switch (state) {
                case TelephonyManager.CALL_STATE_RINGING:
                    TelephonyManager tm = (TelephonyManager) getSystemService(Context.TELEPHONY_SERVICE);
                    try {
                        Class<?> c = Class.forName(tm.getClass().getName());
                        Method m = c.getDeclaredMethod("getITelephony");
                        m.setAccessible(true);
                        // 挂掉电话
                        //ITelephony telephonyService = (ITelephony) m.invoke(tm);
                        //telephonyService.endCall();
                    } catch (Exception e) {
                        e.printStackTrace();
                    }
                    break;
                case TelephonyManager.CALL_STATE_OFFHOOK:
                    break;
                case TelephonyManager.CALL_STATE_IDLE:
                    break;
            }
        }
    }

    // 来电时候开锁
    private void initWakeLock() {
        if (!mWakeLock.isHeld()) {
            mWakeLock.acquire();
        }
        // 初始化键盘锁，可以锁定或解开键盘锁
        mKeyguardLock = mKeyguardManager.newKeyguardLock("");
        // 禁用显示键盘锁定
        mKeyguardLock.disableKeyguard();
    }

    // 通话结速时重新上锁
    private void freeWakeLock() {
        try {
            if (mWakeLock.isHeld()) {
                if (mKeyguardLock != null) {
                    mKeyguardLock.reenableKeyguard();
                    mKeyguardLock = null;
                }
                mWakeLock.release();
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    @Override
    public boolean onKeyUp(int keyCode, KeyEvent event) {
        return false;
    }

    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event) {
        if (keyCode == KeyEvent.KEYCODE_BACK) {
            return false;
        } else {
            return super.onKeyDown(keyCode, event);
        }
    }

    class InnerRecevier extends BroadcastReceiver {
        final String SYSTEM_DIALOG_REASON_KEY = "reason";
        final String SYSTEM_DIALOG_REASON_RECENT_APPS = "recentapps";
        final String SYSTEM_DIALOG_REASON_HOME_KEY = "homekey";

        @Override
        public void onReceive(Context context, Intent intent) {
            String action = intent.getAction();
            if (action.equals(Intent.ACTION_CLOSE_SYSTEM_DIALOGS)) {
                String reason = intent.getStringExtra(SYSTEM_DIALOG_REASON_KEY);
                if (reason != null) {
                    if (reason.equals(SYSTEM_DIALOG_REASON_HOME_KEY)) {
                        // home键
                        isDownHome = true;
                    } else if (reason.equals(SYSTEM_DIALOG_REASON_RECENT_APPS)) {
                        // 长按home键
                    }
                }
            }
        }
    }
}
