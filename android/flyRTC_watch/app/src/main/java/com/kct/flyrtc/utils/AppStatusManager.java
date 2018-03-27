package com.kct.flyrtc.utils;

/**
 * Created by zhouwq on 2017/12/29/029.
 * App状态管理
 */

public class AppStatusManager {
    // 单例
    private static AppStatusManager mInstance = null;
    // App状态 0-被杀掉,1-正常
    private int appStatus = 0;

    public static AppStatusManager getInstance() {
        if (mInstance == null) {
            synchronized (AppStatusManager.class) {
                if (mInstance == null)
                    mInstance = new AppStatusManager();
            }
        }
        return mInstance;
    }

    public void setAppStatus(int appStatus) {
        this.appStatus = appStatus;
    }

    public int getAppStatus() {
        return appStatus;
    }
}
