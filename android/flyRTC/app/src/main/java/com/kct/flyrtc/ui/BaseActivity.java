package com.kct.flyrtc.ui;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.os.PowerManager;

import com.kct.flyrtc.utils.UIData;

/**
 * Created by zhouwq on 2017/3/10/010.
 * 页面基类
 */

public class BaseActivity extends Activity {
    // 电源锁
    private PowerManager.WakeLock mWakeLock = null;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        // 保证后台TCP连接的稳定性
        PowerManager mPowerManager = (PowerManager) getSystemService(Context.POWER_SERVICE);
        mWakeLock = mPowerManager.newWakeLock(PowerManager.PARTIAL_WAKE_LOCK, "BaseActivity_" + getClass().getName());
        if (!mWakeLock.isHeld()) {
            mWakeLock.acquire();
        }
        // 检测有没有还在通话
        String type = UIData.getCallType(getApplicationContext());
        if (type.length() > 0) {
            if (type.equals("2")) {
                // 如果还在通话状态,启动通话页面
                Intent mIntent = new Intent(this, VideoCallActivity.class);
                mIntent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
                startActivity(mIntent);
            }
        }
    }

    @Override
    protected void onRestart() {
        super.onRestart();
        // 检测有没有还在通话
        String type = UIData.getCallType(getApplicationContext());
        if (type.length() > 0) {
            if (type.equals("2")) {
                // 如果还在通话状态,启动通话页面
                Intent mIntent = new Intent(this, VideoCallActivity.class);
                mIntent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
                startActivity(mIntent);
            }
        }
    }

    @Override
    protected void onDestroy() {
        // 释放锁
        if (mWakeLock.isHeld()) {
            mWakeLock.release();
        }
        super.onDestroy();
    }
}
