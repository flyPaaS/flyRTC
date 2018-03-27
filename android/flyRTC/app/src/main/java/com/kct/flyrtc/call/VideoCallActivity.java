package com.kct.flyrtc.call;

import android.annotation.SuppressLint;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.graphics.Color;
import android.media.AudioManager;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.os.SystemClock;
import android.text.method.ScrollingMovementMethod;
import android.util.DisplayMetrics;
import android.view.Gravity;
import android.view.View;
import android.view.WindowManager;
import android.widget.ImageButton;
import android.widget.RelativeLayout;
import android.widget.TextView;

import com.gl.softphone.UGoAPIParam;
import com.kct.flyrtc.R;
import com.kct.flyrtc.ui.CallActivity;
import com.kct.flyrtc.utils.TouchListener;
import com.kct.flyrtc.utils.UIAction;
import com.kct.sdk.KCBase;
import com.kct.sdk.KCSdk;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.util.ArrayList;

/**
 * Created by zhouwq on 2017/3/8/008.
 * 视频电话界面
 */

public class VideoCallActivity extends CallActivity {
    // 窗口对象
    public WindowManager mWindowManager = null;
    public WindowManager.LayoutParams mLayoutParams = null;
    // 控件
    private RelativeLayout remotelayout = null;
    private RelativeLayout locallayout = null;
    // 显示控件
    private TextView video_call_client = null;
    private TextView video_call_status = null;
    private TextView video_call_status2 = null;
    // 按键
    private ImageButton converse_call_video2 = null;
    private TextView video_video_text2 = null;
    // 按键
    private ImageButton converse_call_mute = null;
    private ImageButton converse_call_video = null;
    private ImageButton converse_call_speaker = null;
    private ImageButton converse_call_switch = null;
    private TextView video_mute_text = null;
    private TextView video_video_text = null;
    private TextView video_speaker_text = null;
    private TextView video_switch_text = null;
    // 通话按键
    private ImageButton video_call_answer = null;
    private ImageButton video_call_hangup = null;
    private ImageButton video_call_hangup2 = null;
    // Tips
    private TextView converse_call_tips = null;

    // 电话状态
    private int nCallStatus = 0;
    // 来电标记
    private boolean inCall = false;
    // 电话号码
    private String phoneNumber = "";

    // 手机耳机状态
    private boolean bHeadset = false;
    private boolean bCurSpeak = false;

    // 用于打开网络信息的连击动作时间列表
    private boolean bShowNetMsg = false;
    ArrayList<Long> clickList = new ArrayList<>();

    // 显示控件
    private boolean bShowCtrl = true;
    // 大小屏幕切换标记
    private boolean bSwitch = false;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON, WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        setContentView(R.layout.activity_video_call);
        CallAdapter.getInstance().mVideoCallActivity = this;
        // 初始化悬浮窗口
        mWindowManager = (WindowManager) getSystemService(Context.WINDOW_SERVICE);
        mLayoutParams = new WindowManager.LayoutParams();
        // 初始化控件
        ViewCtrl();
        // 初始化视频
        KCSdk.getInstance().InitVideo(locallayout, remotelayout, this);
        // 全屏显示
        KCSdk.getInstance().SetCurBig(true);
        // 判断是不是来电
        if (getIntent().hasExtra("phoneNumber")) {
            phoneNumber = getIntent().getStringExtra("phoneNumber");
        }
        video_call_client.setText(phoneNumber);
        // 来电
        if (getIntent().hasExtra("inCall")) {
            inCall = getIntent().getBooleanExtra("inCall", false);
        }
        // 判断来电
        if (inCall) {
            nCallStatus = 2;
            video_call_hangup.setVisibility(View.GONE);
            video_call_answer.setVisibility(View.GONE);
            video_call_hangup2.setVisibility(View.GONE);
            converse_call_video2.setVisibility(View.GONE);
            video_video_text2.setVisibility(View.GONE);
            video_call_status.setText("视频电话来电");
            video_call_status2.setVisibility(View.GONE);
            // 播放来电音乐
            KCSdk.getInstance().SetSpeaker(true);
            KCSdk.getInstance().StartRinging();
            // 延时显示界面，防止快速点击接听
            mHandler.sendEmptyMessageDelayed(2000, 200);
        } else {
            nCallStatus = 1;
            video_call_answer.setVisibility(View.GONE);
            video_call_hangup.setVisibility(View.VISIBLE);
            video_call_hangup2.setVisibility(View.GONE);
            converse_call_video2.setVisibility(View.GONE);
            video_video_text2.setVisibility(View.GONE);
            video_call_status.setText("正在呼叫");
            video_call_status2.setVisibility(View.GONE);
            // 主叫
            if (phoneNumber != null && phoneNumber.length() > 0) {
                KCSdk.getInstance().SetSpeaker(true);
                KCSdk.getInstance().Dialing(phoneNumber, true);
            }
        }
        // 注册广播
        IntentFilter mIntentFilter = new IntentFilter();
        mIntentFilter.addAction(UIAction.ACTION_NETWORK_STATE);
        mIntentFilter.addAction(UIAction.ACTION_CALL_TIME);
        mIntentFilter.addAction(UIAction.ACTION_DIAL_STATE);
        mIntentFilter.addAction(UIAction.ACTION_ANSWER);
        mIntentFilter.addAction(UIAction.ACTION_AUDIO_MODE);
        mIntentFilter.addAction(UIAction.ACTION_DIAL_HEAD);
        registerReceiver(mBroadcastReceiver, mIntentFilter);
    }

    @Override
    protected void onDestroy() {
        unregisterReceiver(mBroadcastReceiver);
        KCSdk.getInstance().Hangup();
        KCSdk.getInstance().StopRinging();
        KCSdk.getInstance().StopCallRinging();
        KCSdk.getInstance().StopVideo(31);
        mWindowManager.removeViewImmediate(locallayout);
        //mWindowManager.removeViewImmediate(remotelayout);
        super.onDestroy();
    }

    @Override
    protected void onResume() {
        if (nCallStatus == 2) {
            // 来电
            mHandler.sendEmptyMessageDelayed(1001, 100);
        }
        if (nCallStatus == 3) {
            // 通话
            mHandler.sendEmptyMessage(1002);
        }
        super.onResume();
    }

    @Override
    protected void onPause() {
        if (nCallStatus == 2) {
            // 来电
            mHandler.sendEmptyMessage(1101);
        }
        if (nCallStatus == 3) {
            // 通话
            mHandler.sendEmptyMessage(1102);
        }
        super.onPause();
    }

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
                                //video_call_network.setText("网络状态极好");
                                strNet = "nice";
                                break;
                            case UGoAPIParam.eUGo_NETWORK_WELL:
                                //video_call_network.setText("网络状态良好");
                                strNet = "well";
                                break;
                            case UGoAPIParam.eUGo_NETWORK_GENERAL:
                                //video_call_network.setText("网络状态一般");
                                strNet = "general";
                                break;
                            case UGoAPIParam.eUGo_NETWORK_POOR:
                                //video_call_network.setText("网络状态较差");
                                strNet = "poor";
                                break;
                            case UGoAPIParam.eUGo_NETWORK_BAD:
                                //video_call_network.setText("网络状态极差");
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

                        converse_call_tips.setTextColor(Color.rgb(255, 0, 0));
                        converse_call_tips.setText(strMsg);
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
                    if (state == 1 && !inCall) {
                        // 对方正在响铃
                        KCSdk.getInstance().StartCallRinging("ring.pcm");
                        KCSdk.getInstance().StartVideo(9, true);
                        return;
                    }
                    String str = intent.getStringExtra(UIAction.DAIL_STATE);
                    if (str != null) {
                        video_call_status.setText(str);
                        video_call_status2.setText(str);
                    }
                    // 关闭页面
                    nCallStatus = 0;
                    new Handler().postDelayed(new Runnable() {
                        @Override
                        public void run() {
                            CallAdapter.getInstance().FinishVideo();
                            CallAdapter.getInstance().FinishAudio();
                        }
                    }, 1000);
                } else if (strAction.equals(UIAction.ACTION_ANSWER)) {
                    KCSdk.getInstance().StopRinging();
                    KCSdk.getInstance().StopCallRinging();
                    // 显示隐藏控件
                    converse_call_speaker.setBackgroundResource(R.mipmap.converse_speakerd);
                    remotelayout.setVisibility(View.VISIBLE);
                    locallayout.setVisibility(View.VISIBLE);
                    converse_call_mute.setVisibility(View.VISIBLE);
                    converse_call_video.setVisibility(View.VISIBLE);
                    converse_call_speaker.setVisibility(View.VISIBLE);
                    converse_call_switch.setVisibility(View.VISIBLE);
                    converse_call_video2.setVisibility(View.GONE);
                    video_mute_text.setVisibility(View.VISIBLE);
                    video_video_text.setVisibility(View.VISIBLE);
                    video_speaker_text.setVisibility(View.VISIBLE);
                    video_switch_text.setVisibility(View.VISIBLE);
                    video_video_text2.setVisibility(View.GONE);
                    video_call_answer.setVisibility(View.GONE);
                    video_call_hangup.setVisibility(View.GONE);
                    video_call_hangup2.setVisibility(View.VISIBLE);
                    video_call_client.setVisibility(View.GONE);
                    video_call_status.setVisibility(View.GONE);
                    video_call_status2.setVisibility(View.VISIBLE);
                    // 刷新摄像头
                    nCallStatus = 3;
                    // 获取耳机状态
                    AudioManager mAudioManager = (AudioManager) getSystemService(AUDIO_SERVICE);
                    bHeadset = mAudioManager.isWiredHeadsetOn();
                    // 更新外放状态
                    if (bHeadset) {
                        converse_call_speaker.setBackgroundResource(R.mipmap.converse_speaker);
                        KCSdk.getInstance().SetSpeaker(false);
                    }
                    // 启动视频
                    KCSdk.getInstance().SetCurBig(false);
                    KCSdk.getInstance().StartVideo(31, true);
                } else if (strAction.equals(UIAction.ACTION_DIAL_HEAD)) {
                    if (intent.getIntExtra(UIAction.DAIL_STATE, 0) == 1) {
                        // 耳机插上
                        bHeadset = true;
                        // 判断通话状态
                        if (nCallStatus == 3) {
                            // 更新显示
                            converse_call_speaker.setBackgroundResource(R.mipmap.converse_speaker);
                            bCurSpeak = KCSdk.getInstance().GetSpeaker();
                            KCSdk.getInstance().SetSpeaker(false);
                        }
                    } else if (intent.getIntExtra(UIAction.DAIL_STATE, 0) == 0) {
                        // 耳机拔出
                        bHeadset = false;
                        // 判断通话状态
                        if (nCallStatus == 3) {
                            // 更新显示
                            if (bCurSpeak) {
                                converse_call_speaker.setBackgroundResource(R.mipmap.converse_speakerd);
                                KCSdk.getInstance().SetSpeaker(true);
                            }
                        }
                    }
                } else if (strAction.equals(UIAction.ACTION_AUDIO_MODE)) {
                    // 启动音频界面
                    Intent mIntent = new Intent(VideoCallActivity.this, AudioCallActivity.class);
                    mIntent.putExtra("phoneNumber", phoneNumber);
                    startActivity(mIntent);
                }
            }
        }
    };

    @SuppressLint("HandlerLeak")
    private Handler mHandler = new Handler() {
        @Override
        public void dispatchMessage(Message msg) {
            switch (msg.what) {
                case 1001:
                    converse_call_video.setBackgroundResource(R.mipmap.converse_video);
                    KCSdk.getInstance().StartVideo(9, true);
                    break;
                case 1002:
                    converse_call_video.setBackgroundResource(R.mipmap.converse_video);
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
                    video_call_hangup2.setVisibility(View.GONE);
                    converse_call_video2.setVisibility(View.VISIBLE);
                    video_video_text2.setVisibility(View.VISIBLE);
                    break;
                case 3000:
                    KCSdk.getInstance().StartVideo(31, true);
                    break;
            }
        }
    };

    private void ViewCtrl() {
        // 计算长宽
        DisplayMetrics mDisplayMetrics = getResources().getDisplayMetrics();
        int screenWidth = mDisplayMetrics.widthPixels;
        // 远端视频
        remotelayout = findViewById(R.id.remotelayout);
        //remotelayout = new RelativeLayout(this);
        //mLayoutParams.gravity = Gravity.START | Gravity.TOP;
        //mLayoutParams.flags = WindowManager.LayoutParams.FLAG_NOT_TOUCH_MODAL;
        //DisplayMetrics mDisplayMetrics = getResources().getDisplayMetrics();
        //int screenWidth = mDisplayMetrics.widthPixels;
        //mLayoutParams.x = 0;
        //mLayoutParams.y = 0;
        //mLayoutParams.width = screenWidth / 2;
        //mLayoutParams.height = mLayoutParams.width * 4 / 3;
        //mLayoutParams.alpha = 1.0f;
        //mWindowManager.addView(remotelayout, mLayoutParams);
        // 本地视频
        locallayout = new RelativeLayout(this);
        mLayoutParams.gravity = Gravity.START | Gravity.TOP;
        mLayoutParams.flags = WindowManager.LayoutParams.FLAG_NOT_TOUCH_MODAL;
        mLayoutParams.x = screenWidth - (screenWidth / 3);
        mLayoutParams.y = 0;
        mLayoutParams.width = screenWidth / 3;
        mLayoutParams.height = mLayoutParams.width * 4 / 3;
        mLayoutParams.alpha = 1.0f;
        mWindowManager.addView(locallayout, mLayoutParams);
        locallayout.setOnTouchListener(new TouchListener(this, mWindowManager, mLayoutParams));
        // 显示控件
        video_call_client = findViewById(R.id.video_call_client);
        video_call_status = findViewById(R.id.video_call_status);
        video_call_status2 = findViewById(R.id.video_call_status2);
        // 4个控制按键
        converse_call_mute = findViewById(R.id.video_call_mute);
        converse_call_video = findViewById(R.id.video_call_video);
        converse_call_speaker = findViewById(R.id.video_call_speaker);
        converse_call_switch = findViewById(R.id.video_camera_switch);
        converse_call_mute.setVisibility(View.GONE);
        converse_call_video.setVisibility(View.GONE);
        converse_call_speaker.setVisibility(View.GONE);
        converse_call_switch.setVisibility(View.GONE);
        video_mute_text = findViewById(R.id.video_call_mute_text);
        video_video_text = findViewById(R.id.video_call_video_text);
        video_speaker_text = findViewById(R.id.video_call_speaker_text);
        video_switch_text = findViewById(R.id.video_camera_switch_text);
        video_mute_text.setVisibility(View.GONE);
        video_video_text.setVisibility(View.GONE);
        video_speaker_text.setVisibility(View.GONE);
        video_switch_text.setVisibility(View.GONE);

        converse_call_video2 = findViewById(R.id.video_call_video2);
        video_video_text2 = findViewById(R.id.video_call_video_text2);

        // 电话控制按键
        video_call_answer = findViewById(R.id.video_call_answer);
        video_call_hangup = findViewById(R.id.video_call_hangup);
        video_call_hangup2 = findViewById(R.id.video_call_hangup2);
        // Tips显示
        converse_call_tips = findViewById(R.id.video_call_tips);
        converse_call_tips.setVisibility(View.GONE);
        converse_call_tips.setMovementMethod(ScrollingMovementMethod.getInstance());

        // 静音
        converse_call_mute.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (KCSdk.getInstance().GetMute()) {
                    converse_call_mute.setBackgroundResource(R.mipmap.converse_mute);
                    KCSdk.getInstance().SetMute(false);
                } else {
                    converse_call_mute.setBackgroundResource(R.mipmap.converse_muted);
                    KCSdk.getInstance().SetMute(true);
                }
            }
        });

        // 切语音通话
        converse_call_video.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                // 切换音频通话
                KCSdk.getInstance().ChangeCallMode();
                // 启动音频界面
                Intent mIntent = new Intent(VideoCallActivity.this, AudioCallActivity.class);
                mIntent.putExtra("phoneNumber", phoneNumber);
                startActivity(mIntent);
            }
        });

        // 扬声器
        converse_call_speaker.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (!bHeadset) {
                    if (KCSdk.getInstance().GetSpeaker()) {
                        converse_call_speaker.setBackgroundResource(R.mipmap.converse_speaker);
                        KCSdk.getInstance().SetSpeaker(false);
                    } else {
                        converse_call_speaker.setBackgroundResource(R.mipmap.converse_speakerd);
                        KCSdk.getInstance().SetSpeaker(true);
                    }
                }
            }
        });

        // 切换摄像头
        converse_call_switch.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (KCSdk.getInstance().GetVideoIndex() == 0) {
                    KCSdk.getInstance().SwitchVideo(1, KCBase.RotateType.RETATE_270);
                } else {
                    KCSdk.getInstance().SwitchVideo(0, KCBase.RotateType.RETATE_90);
                }
                mHandler.sendEmptyMessageDelayed(3000,100);
            }
        });

        // 切语音通话
        converse_call_video2.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                // 切换音频通话
                KCSdk.getInstance().ChangeCallMode();
                // 启动音频界面
                Intent mIntent = new Intent(VideoCallActivity.this, AudioCallActivity.class);
                mIntent.putExtra("phoneNumber", phoneNumber);
                startActivity(mIntent);
                // 接通
                KCSdk.getInstance().StopCallRinging();
                KCSdk.getInstance().StopRinging();
                KCSdk.getInstance().Answer();
            }
        });

        // 显示TIPS
        video_call_status2.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if (bShowNetMsg) {
                    // 当前为打开状态则单击关闭
                    bShowNetMsg = false;
                    converse_call_tips.setText("");
                    converse_call_tips.setVisibility(View.GONE);
                    return;
                }
                clickList.add(SystemClock.uptimeMillis());
                if (clickList.size() == 3) {
                    //3次连击打开
                    if (clickList.get(clickList.size() - 1) - clickList.get(0) < 2000) {
                        clickList.clear();
                        bShowNetMsg = true;
                        converse_call_tips.setText("");
                        converse_call_tips.setVisibility(View.VISIBLE);
                    } else {
                        Long tmp = clickList.get(clickList.size() - 1);
                        clickList.clear();
                        clickList.add(tmp);
                    }
                }
            }
        });

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
                        CallAdapter.getInstance().FinishVideo();
                        CallAdapter.getInstance().FinishAudio();
                    }
                }, 1000);
            }
        });

        // 挂断
        video_call_hangup2.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                KCSdk.getInstance().StopCallRinging();
                KCSdk.getInstance().StopRinging();
                KCSdk.getInstance().Hangup();
                new Handler().postDelayed(new Runnable() {
                    @Override
                    public void run() {
                        CallAdapter.getInstance().FinishVideo();
                        CallAdapter.getInstance().FinishAudio();
                    }
                }, 1000);
            }
        });

        // 本地视频
        locallayout.setOnClickListener(new View.OnClickListener() {

            @Override
            public void onClick(View v) {
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

        // 远程视频
        remotelayout.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (nCallStatus == 3) {
                    // 隐藏
                    if (!bShowCtrl) {
                        video_call_hangup2.setVisibility(View.GONE);
                        converse_call_mute.setVisibility(View.GONE);
                        converse_call_video.setVisibility(View.GONE);
                        converse_call_speaker.setVisibility(View.GONE);
                        converse_call_switch.setVisibility(View.GONE);
                        video_mute_text.setVisibility(View.GONE);
                        video_video_text.setVisibility(View.GONE);
                        video_speaker_text.setVisibility(View.GONE);
                        video_switch_text.setVisibility(View.GONE);
                        video_call_status2.setVisibility(View.GONE);
                        locallayout.setVisibility(View.GONE);
                        bShowCtrl = true;
                    } else {
                        video_call_hangup2.setVisibility(View.VISIBLE);
                        converse_call_mute.setVisibility(View.VISIBLE);
                        converse_call_video.setVisibility(View.VISIBLE);
                        converse_call_speaker.setVisibility(View.VISIBLE);
                        converse_call_switch.setVisibility(View.VISIBLE);
                        video_mute_text.setVisibility(View.VISIBLE);
                        video_video_text.setVisibility(View.VISIBLE);
                        video_speaker_text.setVisibility(View.VISIBLE);
                        video_switch_text.setVisibility(View.VISIBLE);
                        video_call_status2.setVisibility(View.VISIBLE);
                        locallayout.setVisibility(View.VISIBLE);
                        bShowCtrl = false;
                    }
                }
            }
        });
    }
}
