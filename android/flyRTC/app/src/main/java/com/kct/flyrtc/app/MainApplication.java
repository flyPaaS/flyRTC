package com.kct.flyrtc.app;

import android.annotation.SuppressLint;
import android.app.Application;
import android.content.Intent;
import android.os.Handler;
import android.os.Message;

import com.kct.flyrtc.call.VideoCallActivity;
import com.kct.flyrtc.utils.UIAction;
import com.kct.sdk.KCSdk;
import com.kct.sdk.listen.KCEventListen;
import com.kct.sdk.util.KCLog;

import java.util.Timer;
import java.util.TimerTask;

import static com.kct.sdk.KCBase.CALL_ALERT;
import static com.kct.sdk.KCBase.CALL_ANETWORK;
import static com.kct.sdk.KCBase.CALL_ANSWER;
import static com.kct.sdk.KCBase.CALL_AUDIO_MODE;
import static com.kct.sdk.KCBase.CALL_HANDUP;
import static com.kct.sdk.KCBase.CALL_INCOME;
import static com.kct.sdk.KCBase.CALL_OUTFAIL;
import static com.kct.sdk.KCBase.CALL_VNETWORK;
import static com.kct.sdk.KCBase.ICE_RTPP;
import static com.kct.sdk.KCBase.TCP_CONNECT;
import static com.kct.sdk.KCBase.TCP_DISCONNECT;

/**
 * Created by zhouwq on 2017/3/8/008.
 * 应用程序类
 */

public class MainApplication extends Application implements KCEventListen {
    // 通话计时
    private int second = 0;
    private int minute = 0;
    private int hour = 0;
    private Timer timer = null;
    // TCP连接状态
    public boolean bConnect = false;
    // 是否重连标记
    public boolean bReCon = false;

    @Override
    public void onCreate() {
        super.onCreate();
        KCSdk.getInstance().Init(this);
        KCSdk.getInstance().AddCallBack(this);
    }

    @Override
    public void onTerminate() {
        KCSdk.getInstance().RemoveCallBack(this);
        KCSdk.getInstance().Free();
        super.onTerminate();
    }

    @SuppressLint("HandlerLeak")
    public Handler mHandler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            if (msg.what == 1000) {
                if (bReCon) {
                    new Thread(new Runnable() {
                        @Override
                        public void run() {
                            KCSdk.getInstance().TcpConnect();
                        }
                    }).start();
                }
            }
            if (msg.what == 1001) {
                if (bReCon) {
                    new Thread(new Runnable() {
                        @Override
                        public void run() {
                            KCSdk.getInstance().NextTcpConnect();
                        }
                    }).start();
                }
            }
            super.handleMessage(msg);
        }
    };

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

    @Override
    public void CallBackFunc(int i, int i1, int i2, String s) {
        if (i == TCP_CONNECT) {
            KCLog.e("TCP_CONNECT");
            Intent mIntent = new Intent(UIAction.ACTION_LOGIN_RESPONSE);
            mIntent.putExtra(UIAction.RESULT_KEY, 0);
            sendBroadcast(mIntent);
            bConnect = true;
        }
        if (i == TCP_DISCONNECT) {
            KCLog.e("TCP_DISCONNECT code = " + i1 + ", param = " + i2);
            bConnect = false;
            if (i1 == 0) {
                if (i2 == 0) {
                    // 正常退出
                }
                if (i2 == 1) {
                    // TCP连接失败,换一个服务器登录
                    mHandler.removeMessages(1001);
                    mHandler.sendEmptyMessageDelayed(1001, 500);
                }
                if (i2 == 2) {
                    // TCP心跳失败,重连
                    mHandler.removeMessages(1000);
                    mHandler.sendEmptyMessageDelayed(1000, 500);
                }
                if (i2 == 3) {
                    // 读失败，网络断开
                    KCSdk.getInstance().ChangeICE(ICE_RTPP); // 0:P2P 1:RTPP
                    mHandler.removeMessages(1000);
                    mHandler.sendEmptyMessageDelayed(1000, 500);
                }
                if (i2 == 4) {
                    // 写失败，网络断开
                    KCSdk.getInstance().ChangeICE(ICE_RTPP); // 0:P2P 1:RTPP
                    mHandler.removeMessages(1000);
                    mHandler.sendEmptyMessageDelayed(1000, 500);
                }
                if (i2 == 5) {
                    // SSID过期，需要退出从新登录
                    Intent mIntent = new Intent(UIAction.ACTION_LOGIN_RESPONSE);
                    mIntent.putExtra(UIAction.RESULT_KEY, 100);
                    sendBroadcast(mIntent);
                }
            } else if (i1 == 1) {
                // 账号认证失败
                Intent mIntent = new Intent(UIAction.ACTION_LOGIN_RESPONSE);
                mIntent.putExtra(UIAction.RESULT_KEY, 1);
                sendBroadcast(mIntent);
            } else if (i1 == 2) {
                // 获取服务器地址失败
                Intent mIntent = new Intent(UIAction.ACTION_LOGIN_RESPONSE);
                mIntent.putExtra(UIAction.RESULT_KEY, 1);
                sendBroadcast(mIntent);
            } else if (i1 == 3) {
                // 获取信令服务器地址失败
                Intent mIntent = new Intent(UIAction.ACTION_LOGIN_RESPONSE);
                mIntent.putExtra(UIAction.RESULT_KEY, 1);
                sendBroadcast(mIntent);
            }
        }
        if (i == CALL_OUTFAIL) {
            KCLog.e("CALL_OUTFAIL");
            // 停止计时
            stopCallTimer();
            // 发送报告
            Intent mIntent = new Intent(UIAction.ACTION_DIAL_STATE);
            mIntent.putExtra(UIAction.DAIL_STATE, s);
            sendBroadcast(mIntent);
        }
        if (i == CALL_INCOME) {
            KCLog.e("CALL_INCOME");
            Intent intent = new Intent();
            intent.setClass(this, VideoCallActivity.class);
            intent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
            intent.putExtra("phoneNumber", s);
            intent.putExtra("inCall", true);
            startActivity(intent);
        }
        if (i == CALL_ANSWER) {
            KCLog.e("CALL_ANSWER");
            Intent mIntent = new Intent(UIAction.ACTION_ANSWER);
            sendBroadcast(mIntent);
            // 启动计时
            startCallTimer();
        }
        if (i == CALL_ALERT) {
            KCLog.e("CALL_ALERT");
            Intent mIntent = new Intent(UIAction.ACTION_DIAL_STATE);
            mIntent.putExtra(UIAction.DAIL_STATE_ALERT, 1);
            sendBroadcast(mIntent);
        }
        if (i == CALL_HANDUP) {
            KCLog.e("CALL_HANDUP");
            // 停止计时
            stopCallTimer();
            // 发送报告
            Intent mIntent = new Intent(UIAction.ACTION_DIAL_STATE);
            mIntent.putExtra(UIAction.DAIL_STATE, s);
            sendBroadcast(mIntent);
        }
        if (i == CALL_ANETWORK) {
            //Intent mIntent = new Intent(UIAction.ACTION_NETWORK_STATE);
            //mIntent.putExtra(UIAction.NETWORK_STATE, i);
            //sendBroadcast(mIntent);
        }
        if (i == CALL_VNETWORK) {
            Intent mIntent = new Intent(UIAction.ACTION_NETWORK_STATE);
            mIntent.putExtra(UIAction.NETWORK_STATE, i);
            mIntent.putExtra(UIAction.NETWORK_STRING, s);
            sendBroadcast(mIntent);
        }
        if (i == CALL_AUDIO_MODE) {
            Intent mIntent = new Intent(UIAction.ACTION_AUDIO_MODE);
            sendBroadcast(mIntent);
        }
    }
}
