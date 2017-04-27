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
import android.view.KeyEvent;

/**
 * Created by zhouwq on 2017/3/9/009.
 * 通话页面基类
 */

public class CallActivity extends Activity {
    // 开屏对象
    private PowerManager.WakeLock mWakeLock;
    private KeyguardManager.KeyguardLock mKeyguardLock = null;

    public boolean isDownHome = false;
    private InnerRecevier mInnerRecevier = new InnerRecevier();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setVolumeControlStream(AudioManager.STREAM_RING);
        // 初始化
        PowerManager mPowerManager = (PowerManager) getSystemService(Context.POWER_SERVICE);
        KeyguardManager mKeyguardManager = (KeyguardManager) getSystemService(Context.KEYGUARD_SERVICE);
        mWakeLock = mPowerManager.newWakeLock(PowerManager.ACQUIRE_CAUSES_WAKEUP | PowerManager.SCREEN_DIM_WAKE_LOCK, "CallActivity_" + getClass().getName());
        if (!mWakeLock.isHeld()) {
            mWakeLock.acquire();
        }
        // 初始化键盘锁，可以锁定或解开键盘锁
        mKeyguardLock = mKeyguardManager.newKeyguardLock("");
        // 禁用显示键盘锁定
        mKeyguardLock.disableKeyguard();
        // 注册广播
        IntentFilter mIntentFilter = new IntentFilter(Intent.ACTION_CLOSE_SYSTEM_DIALOGS);
        getApplicationContext().registerReceiver(mInnerRecevier, mIntentFilter);
    }

    @Override
    protected void onDestroy() {
        getApplicationContext().unregisterReceiver(mInnerRecevier);
        if (mWakeLock.isHeld()) {
            if (mKeyguardLock != null) {
                mKeyguardLock.reenableKeyguard();
                mKeyguardLock = null;
            }
            mWakeLock.release();
        }
        super.onDestroy();
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
                        // 口键
                    }
                }
            }
        }
    }
}
