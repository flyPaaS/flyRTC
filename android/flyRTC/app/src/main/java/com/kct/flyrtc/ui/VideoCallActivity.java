package com.kct.flyrtc.ui;

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
import android.view.View;
import android.view.WindowManager;
import android.widget.ImageButton;
import android.widget.LinearLayout;
import android.widget.TextView;

import com.gl.softphone.UGoAPIParam;
import com.kct.flyrtc.R;
import com.kct.flyrtc.utils.UIAction;
import com.kct.flyrtc.utils.UIData;
import com.yzx.api.CallType;
import com.yzx.api.RotateType;
import com.yzx.api.UCSCall;
import com.yzx.api.UCSCameraType;
import com.yzx.api.UCSFrameType;
import com.yzx.preference.UserData;
import com.yzx.tools.FileTools;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;

/**
 * Created by zhouwq on 2017/3/8/008.
 * 视频电话界面
 */

public class VideoCallActivity extends CallActivity {
    // 控件
    private LinearLayout remotelayout = null;
    private LinearLayout locallayout = null;
    private ImageButton video_call_answer = null;
    private ImageButton video_call_hangup = null;
    private ImageButton converse_call_mute = null;
    private ImageButton converse_call_video = null;
    private ImageButton converse_call_speaker = null;
    private ImageButton converse_call_switch = null;
    private TextView converse_information = null;
    private TextView converse_client = null;
    private TextView converse_network = null;
    private TextView converse_network_status = null;

    private int nCallStatus = 0;
    private boolean inCall = false;
    private String phoneNumber = "";
    private boolean closeVideo = false;
    private boolean open_headset = false;
    private boolean speakerPhoneState = false;

    // 用于打开网络信息的连击动作时间列表
    private boolean bShowNetMsg = false;
    ArrayList<Long> clickList = new ArrayList<>();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON, WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        setContentView(R.layout.activity_video_call);
        // 控件
        InitView();
        UIData.saveCallType(getApplicationContext(), "2");
        // 初始化视频
        UCSCall.initCameraConfig(this, remotelayout, locallayout);
        // 判断是不是来电
        if (getIntent().hasExtra("phoneNumber")) {
            phoneNumber = getIntent().getStringExtra("phoneNumber");
        }
        converse_client.setText(phoneNumber);
        // 来电
        if (getIntent().hasExtra("inCall")) {
            inCall = getIntent().getBooleanExtra("inCall", false);
        }
        if (inCall) {
            nCallStatus = 2;
            video_call_hangup.setVisibility(View.VISIBLE);
            video_call_answer.setVisibility(View.VISIBLE);
            converse_information.setText("视频电话来电");
            UCSCall.setSpeakerphone(true);
            UCSCall.startRinging(true);
        } else {
            nCallStatus = 1;
            video_call_answer.setVisibility(View.GONE);
            video_call_hangup.setVisibility(View.VISIBLE);
            converse_information.setText("正在呼叫");
            if (phoneNumber != null && phoneNumber.length() > 0) {
                UCSCall.setSpeakerphone(false);
                UCSCall.dial(this, CallType.VIDEO, phoneNumber);
            }
            locallayout.setVisibility(View.VISIBLE);
            remotelayout.setVisibility(View.VISIBLE);
        }
        // 默认一开始使用前摄像头 0：后摄像头,1:前摄像头
        UCSCall.switchCameraDevice(1, RotateType.DEFAULT);
        // 注册广播
        IntentFilter mIntentFilter = new IntentFilter();
        mIntentFilter.addAction(UIAction.ACTION_NETWORK_STATE);
        mIntentFilter.addAction(UIAction.ACTION_CALL_TIME);
        mIntentFilter.addAction(UIAction.ACTION_DIAL_STATE);
        mIntentFilter.addAction(UIAction.ACTION_ANSWER);
        registerReceiver(mBroadcastReceiver, mIntentFilter);
    }

    @Override
    protected void onDestroy() {
        unregisterReceiver(mBroadcastReceiver);
        UCSCall.closeCamera(UCSCameraType.ALL);
        UCSCall.stopRinging();
        UCSCall.stopCallRinging();
        UIData.saveCallType(getApplicationContext(), "0");
        super.onDestroy();
    }

    @Override
    protected void onResume() {
        if (nCallStatus > 1) {
            mHandler.sendEmptyMessage(0);
        }
        super.onResume();
    }

    @Override
    protected void onPause() {
        if (nCallStatus > 1) {
            mHandler.sendEmptyMessage(1);
        }
        super.onPause();
    }

    private BroadcastReceiver mBroadcastReceiver = new BroadcastReceiver() {

        @Override
        public void onReceive(Context context, Intent intent) {
            String strAction = intent.getAction();
            if (strAction.equals(UIAction.ACTION_NETWORK_STATE)) {
                // 显示网络参数
                switch (intent.getIntExtra(UIAction.NETWORK_STATE, 0)) {
                    case 0:
                        converse_network.setText("网络状态极好");
                        break;
                    case 1:
                        converse_network.setText("网络状态良好");
                        break;
                    case 2:
                        converse_network.setText("网络状态一般");
                        break;
                    case 3:
                        converse_network.setText("网络状态较差");
                        break;
                    case 4:
                        converse_network.setText("网络状态极差");
                        break;
                    default:
                        converse_network.setText("无法获取网络状态");
                        break;
                }
                // 显示视频参数
                try {
                    JSONObject obj = new JSONObject(UserData.UGo_VIDEO_NETWORK_EV_PARAM);
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

                    converse_network_status.setTextColor(Color.rgb(255, 0, 0));
                    converse_network_status.setText(strMsg);
                } catch (JSONException e) {
                    e.printStackTrace();
                }
            } else if (strAction.equals(UIAction.ACTION_CALL_TIME)) {
                String timer = intent.getStringExtra(UIAction.CALL_TIME_STRING);
                if (converse_information != null) {
                    converse_information.setText(timer);
                }
            } else if (strAction.equals(UIAction.ACTION_DIAL_STATE)) {
                int state = intent.getIntExtra(UIAction.DAIL_STATE, 0);
                if (UIAction.dialState.keySet().contains(state)) {
                    converse_information.setText(UIAction.dialState.get(state));
                }
                // 分析
                if (state == UCSCall.HUNGUP_REFUSAL
                        || state == UCSCall.HUNGUP_MYSELF
                        || state == UCSCall.HUNGUP_OTHER
                        || state == UCSCall.HUNGUP_MYSELF_REFUSAL) {
                    UCSCall.stopCallRinging();
                    nCallStatus = 0;
                    // 关闭页面
                    UIData.saveCallType(getApplicationContext(), "0");
                    new Handler().postDelayed(new Runnable() {
                        @Override
                        public void run() {
                            VideoCallActivity.this.finish();
                        }
                    }, 1000);
                } else {
                    if ((state >= 300210 && state <= 300249) &&
                            (state != 300221 && state != 300222 && state != 300247)
                            || state == UCSCall.HUNGUP_NOT_ANSWER) {
                        // 关闭页面
                        nCallStatus = 0;
                        UIData.saveCallType(getApplicationContext(), "0");
                        new Handler().postDelayed(new Runnable() {
                            @Override
                            public void run() {
                                VideoCallActivity.this.finish();
                            }
                        }, 1000);
                    }
                }
                // 对方正在响铃
                if (!inCall && state == UCSCall.CALL_VOIP_RINGING_180) {
                    //UCSCall.refreshCamera(UCSCameraType.ALL, UCSFrameType.ORIGINAL);
                }
            } else if (strAction.equals(UIAction.ACTION_ANSWER)) {
                UCSCall.setSpeakerphone(true);
                converse_call_speaker.setBackgroundResource(R.mipmap.converse_speakerd);
                UCSCall.stopCallRinging();
                UCSCall.stopRinging();
                locallayout.setVisibility(View.VISIBLE);
                remotelayout.setVisibility(View.VISIBLE);
                converse_call_mute.setVisibility(View.VISIBLE);
                converse_call_video.setVisibility(View.VISIBLE);
                converse_call_speaker.setVisibility(View.VISIBLE);
                converse_call_switch.setVisibility(View.VISIBLE);
                video_call_answer.setVisibility(View.GONE);
                // 刷新摄像头
                nCallStatus = 3;
                open_headset = true;
                if (!UCSCall.isCameraPreviewStatu(VideoCallActivity.this)) {
                    //刷新摄像头发送和接收，重要，一定要加这个
                    UCSCall.refreshCamera(UCSCameraType.ALL, UCSFrameType.ORIGINAL);
                }
            } else if (strAction.equals(UIAction.ACTION_DIAL_HEAD)) {
                if (intent.getIntExtra(UIAction.DAIL_STATE, 0) == 1 && open_headset) {
                    converse_call_speaker.setBackgroundResource(R.mipmap.converse_speaker);
                    speakerPhoneState = UCSCall.isSpeakerphoneOn();
                    UCSCall.setSpeakerphone(false);
                } else if (intent.getIntExtra(UIAction.DAIL_STATE, 0) == 0 && open_headset) {
                    if (speakerPhoneState) {
                        converse_call_speaker.setBackgroundResource(R.mipmap.converse_speakerd);
                        UCSCall.setSpeakerphone(true);
                    }
                }
            }
        }
    };

    @SuppressLint("HandlerLeak")
    private Handler mHandler = new Handler() {
        @Override
        public void dispatchMessage(Message msg) {
            switch (msg.what) {
                case 0:
                    converse_call_video.setBackgroundResource(R.mipmap.converse_video);
                    UCSCall.refreshCamera(UCSCameraType.ALL, UCSFrameType.ORIGINAL);
                    break;
                case 1:
                    UCSCall.closeCamera(UCSCameraType.ALL);
                    break;
            }
        }
    };

    private void InitView() {
        remotelayout = (LinearLayout) findViewById(R.id.remotelayout);
        locallayout = (LinearLayout) findViewById(R.id.locallayout);
        video_call_answer = (ImageButton) findViewById(R.id.video_call_answer);
        video_call_hangup = (ImageButton) findViewById(R.id.video_call_hangup);
        converse_call_mute = (ImageButton) findViewById(R.id.converse_call_mute);
        converse_call_video = (ImageButton) findViewById(R.id.converse_call_video);
        converse_call_speaker = (ImageButton) findViewById(R.id.converse_call_speaker);
        converse_call_switch = (ImageButton) findViewById(R.id.converse_call_switch);
        converse_client = (TextView) findViewById(R.id.converse_client);
        converse_information = (TextView) findViewById(R.id.converse_information);
        converse_network = (TextView) findViewById(R.id.converse_network);
        converse_network_status = (TextView) findViewById(R.id.converse_network_status);
        converse_network_status.setVisibility(View.GONE);

        // 静音
        converse_call_mute.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (UCSCall.isMicMute()) {
                    converse_call_mute.setBackgroundResource(R.mipmap.converse_mute);
                } else {
                    converse_call_mute.setBackgroundResource(R.mipmap.converse_muted);
                }
                UCSCall.setMicMute(!UCSCall.isMicMute());
            }
        });

        // 摄像头开关
        converse_call_video.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (closeVideo) {
                    // 开启自己的摄像头
                    closeVideo = false;
                    converse_call_video.setBackgroundResource(R.mipmap.converse_video);
                    UCSCall.openCamera(UCSCameraType.LOCALCAMERA);
                } else {
                    // 关闭自己的摄像头
                    closeVideo = true;
                    converse_call_video.setBackgroundResource(R.mipmap.converse_videod);
                    UCSCall.closeCamera(UCSCameraType.LOCALCAMERA);
                }
            }
        });

        // 扬声器
        converse_call_speaker.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (UCSCall.isSpeakerphoneOn()) {
                    converse_call_speaker.setBackgroundResource(R.mipmap.converse_speaker);
                } else {
                    converse_call_speaker.setBackgroundResource(R.mipmap.converse_speakerd);
                }
                UCSCall.setSpeakerphone(!UCSCall.isSpeakerphoneOn());
            }
        });

        converse_call_speaker.setOnLongClickListener(new View.OnLongClickListener() {
            @SuppressLint("SimpleDateFormat")
            @Override
            public boolean onLongClick(View v) {
                SimpleDateFormat format = new SimpleDateFormat("yyyyMMddHHmmss");
                String remoteName = "remote_" + phoneNumber + "_" + format.format(new Date()) + ".jpg";
                UCSCall.videoCapture(UCSCameraType.REMOTECAMERA, remoteName, FileTools.getSdCardFilePath());
                return false;
            }
        });

        //切换摄像头
        converse_call_switch.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (UCSCall.getCurrentCameraIndex() == 0) {
                    UCSCall.switchCameraDevice(1, RotateType.DEFAULT);
                } else {
                    UCSCall.switchCameraDevice(0, RotateType.DEFAULT);
                }
            }
        });

        //接通
        video_call_answer.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                UCSCall.stopRinging();
                UCSCall.answer("");
                UCSCall.setSpeakerphone(false);
            }
        });

        //挂断
        video_call_hangup.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                UIData.saveCallType(getApplicationContext(), "0");
                UCSCall.stopRinging();
                UCSCall.hangUp("");
                new Handler().postDelayed(new Runnable() {
                    @Override
                    public void run() {
                        VideoCallActivity.this.finish();
                    }
                }, 1000);
            }
        });

        // 本地视频
        locallayout.setOnClickListener(new View.OnClickListener() {

            @Override
            public void onClick(View v) {
                if (bShowNetMsg) {
                    // 当前为打开状态则单击关闭
                    bShowNetMsg = false;
                    converse_network_status.setText("");
                    converse_network_status.setVisibility(View.GONE);
                    return;
                }
                clickList.add(SystemClock.uptimeMillis());
                if (clickList.size() == 3) {
                    //3次连击打开
                    if (clickList.get(clickList.size() - 1) - clickList.get(0) < 2000) {
                        clickList.clear();
                        bShowNetMsg = true;
                        converse_network_status.setText("");
                        converse_network_status.setVisibility(View.VISIBLE);
                    } else {
                        Long tmp = clickList.get(clickList.size() - 1);
                        clickList.clear();
                        clickList.add(tmp);
                    }
                }
            }
        });
    }
}
