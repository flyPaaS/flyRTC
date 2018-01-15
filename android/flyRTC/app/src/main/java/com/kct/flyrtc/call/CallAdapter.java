package com.kct.flyrtc.call;

/**
 * Created by zhouwq on 2017/12/15/015.
 * 电话显示控制
 */

public class CallAdapter {
    // 单例
    private static CallAdapter mCallAdapter = null;
    public static CallAdapter getInstance() {
        if (mCallAdapter == null) {
            mCallAdapter = new CallAdapter();
        }
        return mCallAdapter;
    }

    // UI对象
    public VideoCallActivity mVideoCallActivity = null;
    public AudioCallActivity mAudioCallActivity = null;

    // 关闭视频UI
    public void FinishVideo() {
        if (mVideoCallActivity != null) {
            mVideoCallActivity.finish();
            mVideoCallActivity = null;
        }
    }

    // 关闭音频UI
    public void FinishAudio() {
        if (mAudioCallActivity != null) {
            mAudioCallActivity.finish();
            mAudioCallActivity = null;
        }
    }
}
