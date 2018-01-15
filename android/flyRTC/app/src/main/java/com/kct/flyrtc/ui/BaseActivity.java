package com.kct.flyrtc.ui;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.os.PowerManager;

import com.kct.flyrtc.utils.AppStatusManager;

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
        checkAppStatus();
        // 保证后台TCP连接的稳定性
        PowerManager mPowerManager = (PowerManager) getSystemService(Context.POWER_SERVICE);
        if (mPowerManager != null) {
            mWakeLock = mPowerManager.newWakeLock(PowerManager.PARTIAL_WAKE_LOCK, "BaseActivity_" + getClass().getName());
            if (!mWakeLock.isHeld()) {
                mWakeLock.acquire(1000);
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

    // 检查APP是否被回收
    private void checkAppStatus() {
        if (AppStatusManager.getInstance().getAppStatus() == 0) {
            // 该应用已被回收
            Intent intent = new Intent(this, MainActivity.class);
            intent.setFlags(Intent.FLAG_ACTIVITY_CLEAR_TASK);
            startActivity(intent);
        }
    }
}
