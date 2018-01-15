package com.kct.flyrtc.call;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.media.AudioManager;
import android.os.Bundle;
import android.support.annotation.Nullable;
import android.view.View;
import android.widget.ImageButton;
import android.widget.TextView;

import com.kct.flyrtc.R;
import com.kct.flyrtc.ui.CallActivity;
import com.kct.flyrtc.utils.UIAction;
import com.kct.sdk.KCSdk;

/**
 * Created by zhouwq on 2017/12/14/014.
 * 音频通话界面
 */

public class AudioCallActivity extends CallActivity {
    // 控件
    private ImageButton Audio_call_hangup = null;
    private ImageButton Audio_call_mute = null;
    private ImageButton Audio_call_speaker = null;

    private TextView Audio_call_status = null;
    private TextView Audio_call_client = null;

    // 手机耳机状态
    private boolean bHeadset = false;
    private boolean bCurSpeak = false;

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_audio_call);
        CallAdapter.getInstance().mAudioCallActivity = this;
        // 获取耳机状态
        AudioManager mAudioManager = (AudioManager) getSystemService(AUDIO_SERVICE);
        bHeadset = mAudioManager.isWiredHeadsetOn();
        // 更新外放状态
        if (bHeadset) {
            KCSdk.getInstance().SetSpeaker(false);
        }
        // 控件
        InitCtrl();
        // 判断是不是来电
        if (getIntent().hasExtra("phoneNumber")) {
            String strPhone = getIntent().getStringExtra("phoneNumber");
            Audio_call_client.setText(strPhone);
        }

        // 注册广播
        IntentFilter mIntentFilter = new IntentFilter();
        mIntentFilter.addAction(UIAction.ACTION_CALL_TIME);
        mIntentFilter.addAction(UIAction.ACTION_DIAL_HEAD);
        registerReceiver(mBroadcastReceiver, mIntentFilter);
    }

    @Override
    protected void onDestroy() {
        unregisterReceiver(mBroadcastReceiver);
        super.onDestroy();
    }

    private BroadcastReceiver mBroadcastReceiver = new BroadcastReceiver() {

        @Override
        public void onReceive(Context context, Intent intent) {
            String strAction = intent.getAction();
            if (strAction != null) {
                if (strAction.equals(UIAction.ACTION_CALL_TIME)) {
                    String strTime = intent.getStringExtra(UIAction.CALL_TIME_STRING);
                    if (Audio_call_status != null) {
                        Audio_call_status.setText(strTime);
                    }
                } else if (strAction.equals(UIAction.ACTION_DIAL_HEAD)) {
                    if (intent.getIntExtra(UIAction.DAIL_STATE, 0) == 1) {
                        // 耳机插上
                        bHeadset = true;
                        Audio_call_speaker.setBackgroundResource(R.mipmap.converse_speaker);
                        bCurSpeak = KCSdk.getInstance().GetSpeaker();
                        KCSdk.getInstance().SetSpeaker(false);
                    } else if (intent.getIntExtra(UIAction.DAIL_STATE, 0) == 0) {
                        // 耳机拔出
                        bHeadset = false;
                        if (bCurSpeak) {
                            Audio_call_speaker.setBackgroundResource(R.mipmap.converse_speakerd);
                            KCSdk.getInstance().SetSpeaker(true);
                        }
                    }
                }
            }
        }
    };

    // 控件初始化
    private void InitCtrl() {
        // 按键
        Audio_call_mute = findViewById(R.id.audio_call_mute);
        Audio_call_hangup = findViewById(R.id.audio_call_hangup);
        Audio_call_speaker = findViewById(R.id.audio_call_speaker);
        //
        Audio_call_status = findViewById(R.id.audio_call_status);
        Audio_call_client = findViewById(R.id.audio_call_client);

        // 更新静音状态
        if (KCSdk.getInstance().GetMute()) {
            Audio_call_mute.setBackgroundResource(R.mipmap.converse_muted);
        } else {
            Audio_call_mute.setBackgroundResource(R.mipmap.converse_mute);
        }
        // 更新扬声器显示状态
        if (!bHeadset) {
            if (KCSdk.getInstance().GetSpeaker()) {
                Audio_call_speaker.setBackgroundResource(R.mipmap.converse_speakerd);
            } else {
                Audio_call_speaker.setBackgroundResource(R.mipmap.converse_speaker);
            }
        } else {
            Audio_call_speaker.setBackgroundResource(R.mipmap.converse_speaker);
        }

        // 静音
        Audio_call_mute.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (KCSdk.getInstance().GetMute()) {
                    Audio_call_mute.setBackgroundResource(R.mipmap.converse_mute);
                    KCSdk.getInstance().SetMute(false);
                } else {
                    Audio_call_mute.setBackgroundResource(R.mipmap.converse_muted);
                    KCSdk.getInstance().SetMute(true);
                }
            }
        });

        // 挂机
        Audio_call_hangup.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                CallAdapter.getInstance().FinishVideo();
                CallAdapter.getInstance().FinishAudio();
            }
        });

        // 扬声器
        Audio_call_speaker.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (!bHeadset) {
                    if (KCSdk.getInstance().GetSpeaker()) {
                        Audio_call_speaker.setBackgroundResource(R.mipmap.converse_speaker);
                        KCSdk.getInstance().SetSpeaker(false);
                    } else {
                        Audio_call_speaker.setBackgroundResource(R.mipmap.converse_speakerd);
                        KCSdk.getInstance().SetSpeaker(true);
                    }
                }
            }
        });
    }
}
