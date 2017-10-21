package com.kct.flyrtc.app;

import android.annotation.SuppressLint;
import android.app.Application;
import android.content.Intent;
import android.os.Handler;
import android.os.Message;

import com.kct.flyrtc.ui.VideoCallActivity;
import com.kct.flyrtc.utils.UIAction;
import com.reason.UcsReason;
import com.yzx.api.UCSCall;
import com.yzx.api.UCSService;
import com.yzx.listenerInterface.CallStateListener;
import com.yzx.listenerInterface.ConnectionListener;
import com.yzx.tcp.TcpTools;
import com.yzx.tools.CustomLog;

import java.util.Timer;
import java.util.TimerTask;

/**
 * Created by zhouwq on 2017/3/8/008.
 * 应用程序类
 */

public class MainApplication extends Application implements ConnectionListener, CallStateListener {
    // 通话计时
    private int second = 0;
    private int minute = 0;
    private int hour = 0;
    private Timer timer = null;
    // TCP连接状态
    public boolean bConnect = false;

    @Override
    public void onCreate() {
        super.onCreate();
        // 添加连接监听器
        UCSService.addConnectionListener(this);
        // 添加电话监听器
        UCSCall.addCallStateListener(this);
        // 启动未接听时视频预览
        UCSCall.setCameraPreViewStatu(this, true);
        // 初始化SDK
        UCSService.initAction(this);
        UCSService.init(this, true);
    }

    @Override
    public void onTerminate() {
        UCSCall.removeCallStateListener(this);
        UCSService.removeConnectionListener(this);
        UCSService.uninit();
        super.onTerminate();
    }

    @SuppressLint("HandlerLeak")
    private Handler handler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            if (msg.what == 1000) {
                TcpTools.reTcpConnection();
            }
            super.handleMessage(msg);
        }
    };

    @Override
    public void onDialFailed(String s, com.reason.UcsReason ucsReason) {
        // 拨打失败回调
        CustomLog.e("onDialFailed = " + s);
        stopCallTimer();
        voipSwitch(ucsReason);
    }

    @Override
    public void onIncomingCall(String s, String s1, String s2, String s3, String s4) {
        CustomLog.e("onIncomingCall = " + s2);
        Intent intent = new Intent();
        intent.setClass(this, VideoCallActivity.class);
        intent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
        intent.putExtra("phoneNumber", s2);
        intent.putExtra("inCall", true);
        intent.putExtra("nickName", s3);
        startActivity(intent);
    }

    @Override
    public void onHangUp(String s, com.reason.UcsReason ucsReason) {
        CustomLog.e("onHangUp = " + s);
        // 分析挂断原因
        stopCallTimer();
        voipSwitch(ucsReason);
    }

    @Override
    public void onAlerting(String s) {
        // 对方正在响铃回调
        CustomLog.e("onAlerting = " + s);
        Intent mIntent = new Intent(UIAction.ACTION_DIAL_STATE);
        mIntent.putExtra(UIAction.DAIL_STATE_ALERT, 1);
        sendBroadcast(mIntent);
    }

    @Override
    public void onAnswer(String s) {
        // 对方接通电话
        CustomLog.e("onAnswer = " + s);
        Intent mIntent = new Intent(UIAction.ACTION_ANSWER);
        sendBroadcast(mIntent);
        // 启动计时
        startCallTimer();
    }

    @Override
    public void onNetWorkState(int i) {
        Intent mIntent = new Intent(UIAction.ACTION_NETWORK_STATE);
        mIntent.putExtra(UIAction.NETWORK_STATE, i);
        sendBroadcast(mIntent);
    }

    @Override
    public void onDTMF(int i) {
        CustomLog.i("onDTMF = " + i);
    }

    @Override
    public void onCameraCapture(String s) {
        CustomLog.i("onCameraCapture = "+ s);
    }

    @Override
    public void onConnectionSuccessful() {
        CustomLog.e("onConnectionSuccessful");
        Intent mIntent = new Intent(UIAction.ACTION_LOGIN_RESPONSE);
        mIntent.putExtra(UIAction.RESULT_KEY, 0);
        sendBroadcast(mIntent);
        bConnect = true;
    }

    @Override
    public void onConnectionFailed(com.reason.UcsReason ucsReason) {
        CustomLog.e("onConnectionFailed");
        switch (ucsReason.getReason()) {
            case 300501:
                // Socket 读失败
                Intent mIntent = new Intent(UIAction.ACTION_LOGIN_RESPONSE);
                mIntent.putExtra(UIAction.RESULT_KEY, 3);
                sendBroadcast(mIntent);
                break;
            case 300503:
                // Socket 写失败
                break;
            case 300506:
                // Socket 连接失败 主机IP地址不对
            case 300507:
                // Socket 连接失败 IO错误
            case 300508:
                // Socket 连接失败 其它错误
                handler.sendEmptyMessageDelayed(1000, 3000);
                break;
            default:
                // 发送失败广播
                mIntent = new Intent(UIAction.ACTION_LOGIN_RESPONSE);
                mIntent.putExtra(UIAction.RESULT_KEY, 1);
                sendBroadcast(mIntent);
                break;
        }
        // 通知TCP网络断开
        Intent mIntent = new Intent(UIAction.ACTION_NETWORK_STATE);
        mIntent.putExtra(UIAction.NETWORK_CONNECT, 1);
        sendBroadcast(mIntent);
        bConnect = false;
    }

    // 分析状态
    private void voipSwitch(UcsReason reason) {
        Intent mIntent = new Intent(UIAction.ACTION_DIAL_STATE);
        mIntent.putExtra(UIAction.DAIL_STATE, reason.getMsg());
        sendBroadcast(mIntent);
    }

    // 通话走时
    public void startCallTimer() {
        stopCallTimer();
        if (timer == null) {
            timer = new Timer();
        }
        second = 0; //秒
        minute = 0; //分
        hour = 0;   //时
        timer.scheduleAtFixedRate(new TimerTask() {
            @Override
            public void run() {
                StringBuffer timer = new StringBuffer();
                second++;
                if (second >= 60) {
                    minute++;
                    second = 0;
                }
                if (minute >= 60) {
                    hour++;
                    minute = 0;
                }
                if (hour != 0) {
                    if (hour < 10) {
                        timer.append(0);
                    }
                    timer.append(hour);
                    timer.append(":");
                }
                if (minute < 10) {
                    timer.append(0);
                }
                timer.append(minute);
                timer.append(":");
                if (second < 10) {
                    timer.append(0);
                }
                timer.append(second);
                // 发送给UI显示
                Intent mIntent = new Intent(UIAction.ACTION_CALL_TIME);
                mIntent.putExtra(UIAction.CALL_TIME_TOTAL, hour * 3600 + minute * 60 + second);
                mIntent.putExtra(UIAction.CALL_TIME_STRING, timer.toString());
                sendBroadcast(mIntent);
            }
        }, 0, 1000);
    }

    public void stopCallTimer() {
        if (timer != null) {
            timer.cancel();
            timer = null;
        }
    }
}
