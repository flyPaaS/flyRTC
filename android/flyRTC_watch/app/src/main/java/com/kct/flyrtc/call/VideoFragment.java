package com.kct.flyrtc.call;

import android.annotation.SuppressLint;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.graphics.Color;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.os.SystemClock;
import android.support.annotation.Nullable;
import android.support.v4.app.Fragment;
import android.text.method.ScrollingMovementMethod;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageButton;
import android.widget.RelativeLayout;
import android.widget.TextView;

import com.gl.softphone.UGoAPIParam;
import com.kct.flyrtc.R;
import com.kct.flyrtc.utils.UIAction;
import com.kct.sdk.KCSdk;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.util.ArrayList;

/**
 * Created by zhouwq on 2017/12/19/019.
 * 视频页面
 */

public class VideoFragment extends Fragment {
    // 控件
    private View mMainView = null;
    private CallFragmentActivity mActivity = null;
    // 控件
    private RelativeLayout remotelayout = null;
    // 显示控件
    private TextView video_call_client = null;
    private TextView video_call_status = null;
    private TextView video_call_status2 = null;
    // 显示按键
    public ImageButton video_call_mute = null;
    public ImageButton video_call_speaker = null;
    // 通话按键
    private ImageButton video_call_answer = null;
    private ImageButton video_call_hangup = null;
    private ImageButton video_call_audio = null;
    // Tips
    private TextView video_call_tips = null;

    // 用于打开网络信息的连击动作时间列表
    private boolean bShowNetMsg = false;
    ArrayList<Long> clickList = new ArrayList<>();

    // 显示控件
    private boolean bShowCtrl = true;

    @Override
    public void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        // 获取视图
        LayoutInflater inflater = getActivity().getLayoutInflater();
        mMainView = inflater.inflate(R.layout.activity_video_call, null, false);
        mActivity = (CallFragmentActivity) getActivity();
        // 初始化硬件
        InitCtrl();
        // 初始化视频
        KCSdk.getInstance().InitVideo(mActivity.locallayout, remotelayout, mActivity);
        // 全屏显示
        KCSdk.getInstance().SetCurBig(true);
        // 判断是否来电
        if (mActivity.bInCall) {
            mActivity.nCallStatus = 2;
            video_call_hangup.setVisibility(View.GONE);
            video_call_answer.setVisibility(View.GONE);
            video_call_audio.setVisibility(View.GONE);
            video_call_status.setText("视频电话来电");
            //
            KCSdk.getInstance().SetSpeaker(true);
            KCSdk.getInstance().StartRinging();
            //
            mHandler.sendEmptyMessageDelayed(2000, 200);
        } else {
            mActivity.nCallStatus = 1;
            video_call_answer.setVisibility(View.GONE);
            video_call_hangup.setVisibility(View.VISIBLE);
            video_call_audio.setVisibility(View.GONE);
            video_call_status.setText("正在呼叫");
            if (mActivity.strPhone != null && mActivity.strPhone.length() > 0) {
                KCSdk.getInstance().SetSpeaker(true);
                KCSdk.getInstance().Dialing(mActivity.strPhone, true);
            }
        }
        // 注册广播
        IntentFilter mIntentFilter = new IntentFilter();
        mIntentFilter.addAction(UIAction.ACTION_NETWORK_STATE);
        mIntentFilter.addAction(UIAction.ACTION_CALL_TIME);
        mIntentFilter.addAction(UIAction.ACTION_DIAL_STATE);
        mIntentFilter.addAction(UIAction.ACTION_ANSWER);
        mIntentFilter.addAction(UIAction.ACTION_AUDIO_MODE);
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
        if (mActivity.nCallStatus == 2) {
            // 来电
            mHandler.sendEmptyMessageDelayed(1001, 100);
        }
        if (mActivity.nCallStatus == 3) {
            // 通话
            mHandler.sendEmptyMessage(1002);
        }
        super.onResume();
    }

    @Override
    public void onPause() {
        if (mActivity.nCallStatus == 2) {
            // 来电
            mHandler.sendEmptyMessage(1101);
        }
        if (mActivity.nCallStatus == 3) {
            // 通话
            mHandler.sendEmptyMessage(1102);
        }
        super.onPause();
    }

    @Override
    public void onDestroy() {
        mActivity.unregisterReceiver(mBroadcastReceiver);
        KCSdk.getInstance().Hangup();
        KCSdk.getInstance().StopRinging();
        KCSdk.getInstance().StopCallRinging();
        KCSdk.getInstance().StopVideo(31);
        super.onDestroy();
    }

    @SuppressLint("HandlerLeak")
    private Handler mHandler = new Handler() {
        @Override
        public void dispatchMessage(Message msg) {
            switch (msg.what) {
                case 1001:
                    KCSdk.getInstance().StartVideo(9, true);
                    break;
                case 1002:
                    KCSdk.getInstance().StartVideo(31, false);
                    break;
                case 1100:
                    KCSdk.getInstance().StopVideo(31);
                    break;
                case 1101:
                    KCSdk.getInstance().StopVideo(31);
                    break;
                case 1102:
                    KCSdk.getInstance().StopVideo(24);
                    break;
                case 2000:
                    video_call_hangup.setVisibility(View.VISIBLE);
                    video_call_answer.setVisibility(View.VISIBLE);
                    video_call_audio.setVisibility(View.VISIBLE);
                    break;
            }
        }
    };

    private BroadcastReceiver mBroadcastReceiver = new BroadcastReceiver() {

        @Override
        public void onReceive(Context context, Intent intent) {
            String strAction = intent.getAction();
            if (strAction != null) {
                if (strAction.equals(UIAction.ACTION_NETWORK_STATE)) {
                    // 显示视频参数
                    try {
                        JSONObject obj = new JSONObject(intent.getStringExtra(UIAction.NETWORK_STRING));
                        String strNet = "unknown";
                        switch (obj.getInt("ns")) {
                            case UGoAPIParam.eUGo_NETWORK_NICE:
                                strNet = "nice";
                                break;
                            case UGoAPIParam.eUGo_NETWORK_WELL:
                                strNet = "well";
                                break;
                            case UGoAPIParam.eUGo_NETWORK_GENERAL:
                                strNet = "general";
                                break;
                            case UGoAPIParam.eUGo_NETWORK_POOR:
                                strNet = "poor";
                                break;
                            case UGoAPIParam.eUGo_NETWORK_BAD:
                                strNet = "bad";
                                break;
                            default:
                                break;
                        }

                        String strMsg = "vie state:" + strNet
                                + "\r\nice:" + obj.getInt("ice") + ",rtt:" + obj.getInt("rtt")
                                + "\r\nlost:" + obj.getInt("ul") + "(s) " + obj.getInt("dl") + "(r)"
                                + "\r\nrate:" + obj.getInt("sb") + "(s) " + obj.getInt("rb") + "(r)"
                                + "\r\nres: " + obj.getInt("sw") + "x" + obj.getInt("sh") + "(s) " + obj.getInt("dw") + "x" + obj.getInt("dh") + "(r)"
                                + "\r\nframe:" + obj.getInt("sf") + "(s) " + obj.getInt("df") + "(r)"
                                + "\r\npt:" + obj.getInt("ep") + "(s) " + obj.getInt("dp") + "(r)"
                                + "\r\ncodec: " + obj.getString("eCodec") + " (s)" + obj.getString("dCodec") + "(r)";

                        int rtPOT_nCount = obj.getInt("nCnt");
                        strMsg = strMsg + "\r\nRelayCnt: " + obj.getInt("nCnt");
                        if (rtPOT_nCount > 0) {
                            JSONArray jrtPOTarray = obj.getJSONArray("rtPOT");
                            for (int i = 0; i < jrtPOTarray.length(); i++) {
                                JSONObject jsonObj = jrtPOTarray.getJSONObject(i);
                                int sIP = jsonObj.getInt("sIP");
                                int rIP = jsonObj.getInt("rIP");
                                int sFlow_a = jsonObj.getInt("sFlow_a");
                                int rFlow_a = jsonObj.getInt("rFlow_a");
                                int sFlow_v = jsonObj.getInt("sFlow_v");
                                int rFlow_v = jsonObj.getInt("rFlow_v");

                                strMsg = strMsg + "\r\nRelay_" + i + ": " + sIP + " (s) " + rIP + "(r)" +
                                        "\nFlow_a_" + i + ": " + sFlow_a + " KB (s) " + rFlow_a + " KB (r)" +
                                        "\nFlow_v_" + i + ": " + sFlow_v + " KB (s) " + rFlow_v + " KB (r)";
                            }
                        }
                        video_call_tips.setTextColor(Color.rgb(255, 0, 0));
                        video_call_tips.setText(strMsg);
                    } catch (JSONException e) {
                        e.printStackTrace();
                    }
                } else if (strAction.equals(UIAction.ACTION_CALL_TIME)) {
                    String timer = intent.getStringExtra(UIAction.CALL_TIME_STRING);
                    if (video_call_status != null) {
                        video_call_status.setText(timer);
                    }
                    if (video_call_status2 != null) {
                        video_call_status2.setText(timer);
                    }
                } else if (strAction.equals(UIAction.ACTION_DIAL_STATE)) {
                    int state = intent.getIntExtra(UIAction.DAIL_STATE_ALERT, 0);
                    if (state == 1 && !mActivity.bInCall) {
                        // 对方正在响铃
                        KCSdk.getInstance().StartCallRinging("ring.pcm");
                        KCSdk.getInstance().StartVideo(9, true);
                        return;
                    }
                    String str = intent.getStringExtra(UIAction.DAIL_STATE);
                    if (str != null) {
                        video_call_status.setText(str);
                    }
                    // 关闭页面
                    mActivity.nCallStatus = 0;
                    new Handler().postDelayed(new Runnable() {
                        @Override
                        public void run() {
                            mActivity.finish();
                        }
                    }, 1000);
                } else if (strAction.equals(UIAction.ACTION_ANSWER)) {
                    KCSdk.getInstance().StopRinging();
                    KCSdk.getInstance().StopCallRinging();
                    // 显示隐藏控件
                    remotelayout.setVisibility(View.VISIBLE);
                    if (mActivity.mIndex == 0) {
                        mActivity.locallayout.setVisibility(View.VISIBLE);
                    } else {
                        mActivity.locallayout.setVisibility(View.GONE);
                    }
                    video_call_answer.setVisibility(View.GONE);
                    video_call_hangup.setVisibility(View.GONE);
                    video_call_audio.setVisibility(View.GONE);
                    video_call_client.setVisibility(View.GONE);
                    video_call_status.setVisibility(View.GONE);
                    video_call_status2.setVisibility(View.VISIBLE);
                    video_call_mute.setVisibility(View.VISIBLE);
                    video_call_speaker.setVisibility(View.VISIBLE);
                    // 刷新摄像头
                    mActivity.nCallStatus = 3;
                    // 启动视频
                    KCSdk.getInstance().SetCurBig(false);
                    KCSdk.getInstance().StartVideo(31, true);
                    KCSdk.getInstance().SetSpeaker(true);
                    mActivity.UpdateAudioShow();
                } else if (strAction.equals(UIAction.ACTION_AUDIO_MODE)) {
                    // 启动音频界面
                    mActivity.SetPage(1);
                    mActivity.UpdateAudioShow();
                }
            }
        }
    };

    public void InitCtrl() {
        remotelayout = mMainView.findViewById(R.id.remotelayout);
        // 显示控件
        video_call_client = mMainView.findViewById(R.id.video_call_client);
        video_call_status = mMainView.findViewById(R.id.video_call_status);
        video_call_client.setText(mActivity.strPhone);
        video_call_status2 = mMainView.findViewById(R.id.video_call_status2);
        video_call_status2.setVisibility(View.GONE);
        video_call_tips = mMainView.findViewById(R.id.video_call_tips);
        video_call_tips.setVisibility(View.GONE);
        video_call_tips.setMovementMethod(ScrollingMovementMethod.getInstance());
        video_call_mute = mMainView.findViewById(R.id.video_call_mute);
        video_call_mute.setVisibility(View.GONE);
        video_call_speaker = mMainView.findViewById(R.id.video_call_speaker);
        video_call_speaker.setVisibility(View.GONE);

        // 电话控制按键
        video_call_answer = mMainView.findViewById(R.id.video_call_answer);
        video_call_hangup = mMainView.findViewById(R.id.video_call_hangup);
        video_call_audio = mMainView.findViewById(R.id.video_call_audio);

        // 接通
        video_call_answer.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                KCSdk.getInstance().StopCallRinging();
                KCSdk.getInstance().StopRinging();
                KCSdk.getInstance().Answer();
            }
        });

        // 挂断
        video_call_hangup.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                KCSdk.getInstance().StopCallRinging();
                KCSdk.getInstance().StopRinging();
                KCSdk.getInstance().Hangup();
                new Handler().postDelayed(new Runnable() {
                    @Override
                    public void run() {
                        mActivity.finish();
                    }
                }, 1000);
            }
        });

        // 切音频
        video_call_audio.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                // 先切换音频
                KCSdk.getInstance().ChangeCallMode();
                mActivity.SetPage(1);
                // 再接听
                KCSdk.getInstance().StopCallRinging();
                KCSdk.getInstance().StopRinging();
                KCSdk.getInstance().Answer();
            }
        });

        // 静音
        video_call_mute.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (KCSdk.getInstance().GetMute()) {
                    video_call_mute.setBackgroundResource(R.mipmap.converse_mute);
                    KCSdk.getInstance().SetMute(false);
                } else {
                    video_call_mute.setBackgroundResource(R.mipmap.converse_muted);
                    KCSdk.getInstance().SetMute(true);
                }
            }
        });

        // 扬声器
        video_call_speaker.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (KCSdk.getInstance().GetSpeaker()) {
                    video_call_speaker.setBackgroundResource(R.mipmap.converse_speaker);
                    KCSdk.getInstance().SetSpeaker(false);
                } else {
                    video_call_speaker.setBackgroundResource(R.mipmap.converse_speakerd);
                    KCSdk.getInstance().SetSpeaker(true);
                }
            }
        });

        // 显示TIPS
        video_call_status2.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if (bShowNetMsg) {
                    // 当前为打开状态则单击关闭
                    bShowNetMsg = false;
                    video_call_tips.setText("");
                    video_call_tips.setVisibility(View.GONE);
                    return;
                }
                clickList.add(SystemClock.uptimeMillis());
                if (clickList.size() == 3) {
                    //3次连击打开
                    if (clickList.get(clickList.size() - 1) - clickList.get(0) < 2000) {
                        clickList.clear();
                        bShowNetMsg = true;
                        video_call_tips.setText("");
                        video_call_tips.setVisibility(View.VISIBLE);
                    } else {
                        Long tmp = clickList.get(clickList.size() - 1);
                        clickList.clear();
                        clickList.add(tmp);
                    }
                }
            }
        });

        // 远程视频
        remotelayout.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (mActivity.nCallStatus == 3) {
                    // 隐藏
                    if (!bShowCtrl) {
                        video_call_status2.setVisibility(View.GONE);
                        video_call_mute.setVisibility(View.GONE);
                        video_call_speaker.setVisibility(View.GONE);
                        mActivity.locallayout.setVisibility(View.GONE);
                        bShowCtrl = true;
                    } else {
                        video_call_status2.setVisibility(View.VISIBLE);
                        video_call_mute.setVisibility(View.VISIBLE);
                        video_call_speaker.setVisibility(View.VISIBLE);
                        if (mActivity.mIndex == 0) {
                            mActivity.locallayout.setVisibility(View.VISIBLE);
                        } else {
                            mActivity.locallayout.setVisibility(View.GONE);
                        }
                        bShowCtrl = false;
                    }
                }
            }
        });
    }
}
