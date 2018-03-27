package com.kct.flyrtc.call;

import android.content.Context;
import android.os.Bundle;
import android.support.annotation.Nullable;
import android.support.v4.app.Fragment;
import android.support.v4.app.FragmentActivity;
import android.support.v4.app.FragmentManager;
import android.support.v4.app.FragmentPagerAdapter;
import android.util.DisplayMetrics;
import android.view.Gravity;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.RelativeLayout;

import com.kct.flyrtc.R;
import com.kct.flyrtc.utils.TouchListener;
import com.kct.sdk.KCSdk;
import com.kct.sdk.util.KCLog;

import java.util.ArrayList;

/**
 * Created by zhouwq on 2017/12/19/019.
 * 通话页面
 */

public class CallFragmentActivity extends FragmentActivity implements CallViewPager.ChangeViewCallback {
    // 当前Index
    public int mIndex = 0;
    // 滑动控件
    private CallViewPager mViewPager = null;
    private ArrayList<Fragment> mFragmentList = null;
    private CallFragmentPagerAdapter mCallAdapter = null;
    // Fragment页面
    private VideoFragment mVideoFragment = null;
    private AudioFragment mAudioFragment = null;

    // 本地视频窗口
    public RelativeLayout locallayout = null;
    // 窗口对象
    public WindowManager mWindowManager = null;
    public WindowManager.LayoutParams mLayoutParams = null;

    // 来电标记
    public boolean bInCall = false;
    // 电话号码
    public String strPhone = "";
    // 电话状态
    public int nCallStatus = 0;

    // 页面切换标记
    private boolean bSwitch = false;

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_call);
        //changeAppBrightness(30);
        // 初始化控件
        InitView();
        // 判断是不是来电
        if (getIntent().hasExtra("phoneNumber")) {
            strPhone = getIntent().getStringExtra("phoneNumber");
        }
        // 来电
        if (getIntent().hasExtra("inCall")) {
            bInCall = getIntent().getBooleanExtra("inCall", false);
        }
    }

    @Override
    protected void onDestroy() {
        mWindowManager.removeViewImmediate(locallayout);
        super.onDestroy();
    }

    // 获取app亮度
    public void changeAppBrightness(int brightness) {
        Window window = getWindow();
        WindowManager.LayoutParams lp = window.getAttributes();
        lp.screenBrightness = (brightness <= 0 ? 1 : brightness) / 255f;
        window.setAttributes(lp);
    }

    public void InitView() {
        // 获取悬浮窗
        mWindowManager = (WindowManager) getSystemService(Context.WINDOW_SERVICE);
        mLayoutParams = new WindowManager.LayoutParams();
        locallayout = new RelativeLayout(this);
        mLayoutParams.gravity = Gravity.START | Gravity.TOP;
        mLayoutParams.flags = WindowManager.LayoutParams.FLAG_NOT_TOUCH_MODAL;
        DisplayMetrics mDisplayMetrics = getResources().getDisplayMetrics();
        int screenWidth = mDisplayMetrics.widthPixels;
        mLayoutParams.x = screenWidth - 60;
        mLayoutParams.y = 0;
        mLayoutParams.width = 60;
        mLayoutParams.height = 80;
        mLayoutParams.alpha = 1.0f;
        mWindowManager.addView(locallayout, mLayoutParams);
        locallayout.setOnTouchListener(new TouchListener(this, mWindowManager, mLayoutParams));
        // 把Fragment加入数组
        mVideoFragment = new VideoFragment();
        mAudioFragment = new AudioFragment();
        mFragmentList = new ArrayList<>();
        mFragmentList.add(mVideoFragment);
        mFragmentList.add(mAudioFragment);
        // 设置ViewPage
        mCallAdapter = new CallFragmentPagerAdapter(getSupportFragmentManager());
        mViewPager = findViewById(R.id.viewpage);
        mViewPager.setChangeViewCallback(this);
        mViewPager.setAdapter(mCallAdapter);
        mViewPager.setOffscreenPageLimit(2);
        mViewPager.setCurrentItem(0);

        // 本地视频
        locallayout.setOnClickListener(new View.OnClickListener() {

            @Override
            public void onClick(View v) {
                KCLog.e("交换大小窗口");
                if (nCallStatus == 3) {
                    if (!bSwitch) {
                        KCSdk.getInstance().SwitchLayout(true);
                        bSwitch = true;
                    } else {
                        KCSdk.getInstance().SwitchLayout(false);
                        bSwitch = false;
                    }
                }
            }
        });
    }

    public void SetPage(int nPage) {
        if (mIndex != nPage) {
            if (nPage == 1) {
                // 从视频切换到音频
                mIndex = 1;
                mViewPager.setCurrentItem(1);
                locallayout.setVisibility(View.GONE);
            }
            if (nPage == 0) {
                // 从音频切换到视频,退出
                finish();
            }
        }
    }

    public void UpdateAudioShow() {
        // 更新静音状态
        if (KCSdk.getInstance().GetMute()) {
            mAudioFragment.Audio_call_mute.setBackgroundResource(R.mipmap.converse_muted);
            mVideoFragment.video_call_mute.setBackgroundResource(R.mipmap.converse_muted);
        } else {
            mAudioFragment.Audio_call_mute.setBackgroundResource(R.mipmap.converse_mute);
            mVideoFragment.video_call_mute.setBackgroundResource(R.mipmap.converse_mute);
        }
        // 更新扬声器显示状态
        if (KCSdk.getInstance().GetSpeaker()) {
            mAudioFragment.Audio_call_speaker.setBackgroundResource(R.mipmap.converse_speakerd);
            mVideoFragment.video_call_speaker.setBackgroundResource(R.mipmap.converse_speakerd);
        } else {
            mAudioFragment.Audio_call_speaker.setBackgroundResource(R.mipmap.converse_speaker);
            mVideoFragment.video_call_speaker.setBackgroundResource(R.mipmap.converse_speaker);
        }
    }

    @Override
    public void changeView(boolean left, boolean right) {

    }

    @Override
    public void getCurrentPageIndex(int index) {
        if (index == 1 && mIndex == 0) {
            mIndex = 1;
            UpdateAudioShow();
            // 判断是否通话
            if (nCallStatus == 3) {
                KCSdk.getInstance().ChangeCallMode();
                locallayout.setVisibility(View.GONE);
            } else if (nCallStatus == 2) {
                KCSdk.getInstance().ChangeCallMode();
                locallayout.setVisibility(View.GONE);
                // 再接听
                KCSdk.getInstance().StopCallRinging();
                KCSdk.getInstance().StopRinging();
                KCSdk.getInstance().Answer();
            } else if (nCallStatus == 1) {
                finish();
            }
        }
        if (index == 0 && mIndex == 1) {
            finish();
        }
    }

    // 对象
    public class CallFragmentPagerAdapter extends FragmentPagerAdapter {

        public CallFragmentPagerAdapter(FragmentManager fm) {
            super(fm);
        }

        @Override
        public Fragment getItem(int position) {
            return mFragmentList.get(position);
        }

        @Override
        public int getCount() {
            return mFragmentList.size();
        }
    }
}
