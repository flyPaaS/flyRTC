package com.kct.flyrtc.call;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Bundle;
import android.support.annotation.Nullable;
import android.support.v4.app.Fragment;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageButton;
import android.widget.TextView;

import com.kct.flyrtc.R;
import com.kct.flyrtc.utils.UIAction;
import com.kct.sdk.KCSdk;

/**
 * Created by zhouwq on 2017/12/19/019.
 * 音频界面
 */

public class AudioFragment extends Fragment {
    // 控件
    private View mMainView = null;
    private CallFragmentActivity mActivity = null;

    // 控件
    private ImageButton Audio_call_hangup = null;
    public ImageButton Audio_call_mute = null;
    public ImageButton Audio_call_speaker = null;

    private TextView Audio_call_status = null;
    private TextView Audio_call_client = null;

    @Override
    public void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        // 获取视图
        LayoutInflater inflater = getActivity().getLayoutInflater();
        mMainView = inflater.inflate(R.layout.activity_audio_call, null, false);
        mActivity = (CallFragmentActivity) getActivity();
        // 获取控件
        InitCtrl();
        // 注册广播
        IntentFilter mIntentFilter = new IntentFilter();
        mIntentFilter.addAction(UIAction.ACTION_CALL_TIME);
        mActivity.registerReceiver(mBroadcastReceiver, mIntentFilter);
    }

    @Nullable
    @Override
    public View onCreateView(LayoutInflater inflater, @Nullable ViewGroup container, @Nullable Bundle savedInstanceState) {
        ViewGroup p = (ViewGroup) mMainView.getParent();
        if (p != null) {
            p.removeAllViewsInLayout();
        }
        return mMainView;
    }

    @Override
    public void onResume() {
        super.onResume();
    }

    @Override
    public void onPause() {
        super.onPause();
    }

    @Override
    public void onDestroy() {
        mActivity.unregisterReceiver(mBroadcastReceiver);
        super.onDestroy();
    }

    private BroadcastReceiver mBroadcastReceiver = new BroadcastReceiver() {

        @Override
        public void onReceive(Context context, Intent intent) {
            String strAction = intent.getAction();
            if (strAction != null) {
                if (strAction.equals(UIAction.ACTION_CALL_TIME)) {
                    String timer = intent.getStringExtra(UIAction.CALL_TIME_STRING);
                    if (Audio_call_status != null) {
                        Audio_call_status.setText(timer);
                    }
                }
            }
        }
    };

    public void InitCtrl() {
        // 按键
        Audio_call_mute = mMainView.findViewById(R.id.audio_call_mute);
        Audio_call_hangup = mMainView.findViewById(R.id.audio_call_hangup);
        Audio_call_speaker = mMainView.findViewById(R.id.audio_call_speaker);
        // 状态显示
        Audio_call_status = mMainView.findViewById(R.id.audio_call_status);
        Audio_call_client = mMainView.findViewById(R.id.audio_call_client);
        // 更新电话号码
        Audio_call_client.setText(mActivity.strPhone);
        // 更新静音状态
        if (KCSdk.getInstance().GetMute()) {
            Audio_call_mute.setBackgroundResource(R.mipmap.converse_muted);
        } else {
            Audio_call_mute.setBackgroundResource(R.mipmap.converse_mute);
        }
        // 更新扬声器显示状态
        if (KCSdk.getInstance().GetSpeaker()) {
            Audio_call_speaker.setBackgroundResource(R.mipmap.converse_speakerd);
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
                mActivity.finish();
            }
        });

        // 扬声器
        Audio_call_speaker.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (KCSdk.getInstance().GetSpeaker()) {
                    Audio_call_speaker.setBackgroundResource(R.mipmap.converse_speaker);
                    KCSdk.getInstance().SetSpeaker(false);
                } else {
                    Audio_call_speaker.setBackgroundResource(R.mipmap.converse_speakerd);
                    KCSdk.getInstance().SetSpeaker(true);
                }
            }
        });
    }
}
