package com.kctdemo.service;


import java.util.HashMap;
import java.util.Timer;
import java.util.TimerTask;

import com.reason.UcsReason;
import com.yzx.api.CallType;
import com.yzx.api.UCSCall;
import com.yzx.api.UCSService;
import com.yzx.listenerInterface.CallStateListener;
import com.yzx.listenerInterface.ConnectionListener;
import com.yzx.tcp.packet.UcsStatus;
import com.yzx.tools.CustomLog;
import com.kctdemo.action.UIDfineAction;
import com.kctdemo.activity.AudioConverseActivity;
import com.kctdemo.activity.TerminalLoginDialogActivity;
import com.kctdemo.activity.VideoConverseActivity;
import com.kctdemo.restClient.JsonReqClient;
import com.kctdemo.tools.DataTools;
import com.kctdemo.tools.DfineAction;
import com.kctdemo.tools.DialConfig;
import com.kctdemo.tools.LoginConfig;

import android.annotation.SuppressLint;
import android.app.Service;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.IBinder;

/**
 * 后台服务/连接控制器
 */
public class ConnectionService extends Service implements ConnectionListener,CallStateListener {

	public static HashMap<String ,UcsStatus> mapstatus = new HashMap<String ,UcsStatus>();
	
	private String cliend_id;
	private String cliend_pwd;
	private String sid;
	private String sid_pwd;
	
	@Override
	public IBinder onBind(Intent intent) {
		return null;
	}

	@SuppressLint("NewApi") 
	@Override
	public void onCreate() {
		super.onCreate();
		mapstatus.clear();
		CustomLog.i("ConnectionService onCreate ... ");
		//添加连接监听器
		UCSService.addConnectionListener(ConnectionService.this);
		//添加电话监听器
		UCSCall.addCallStateListener(ConnectionService.this);
		//启动未接听时视频预览
		UCSCall.setCameraPreViewStatu(ConnectionService.this,true);
		//初始化SDK
		UCSService.initAction(this);
		UCSService.init(this,true);
		//初始化action动作
		UIDfineAction.initAction(ConnectionService.this.getPackageName());

		IntentFilter ift = new IntentFilter();
        ift.addAction(UIDfineAction.ACTION_LOGIN);
        ift.addAction(UIDfineAction.ACTION_DIAL);
        ift.addAction(UIDfineAction.ACTION_START_TIME);
        ift.addAction(UCSService.ACTION_INIT_SUCCESS);
        registerReceiver(br, ift);
	}
	
	private BroadcastReceiver br = new BroadcastReceiver() {
		@Override
		public void onReceive(Context context, Intent intent) {
			if (intent.getAction().equals(UIDfineAction.ACTION_LOGIN)) {
				// 重新登录时，如果有通话侧挂断
				UCSCall.hangUp("");
				//进行Client账号登陆
				CustomLog.i(DfineAction.TAG_TCP, "开始进行子账号登入");
				if(intent.hasExtra("cliend_id") && intent.hasExtra("cliend_pwd")){
					cliend_id = intent.getStringExtra("cliend_id");
					cliend_pwd = intent.getStringExtra("cliend_pwd");
					sid = intent.getStringExtra("sid");
					sid_pwd = intent.getStringExtra("sid_pwd");

					if (cliend_id != null && cliend_id.length() > 0 && cliend_pwd != null && cliend_pwd.length() > 0) {
						if (DataTools.istest) {
							//测试用
							connectionService("http://113.31.89.144","8887",sid,sid_pwd,cliend_id,cliend_pwd);
						} else {
							connectionService(null,null,sid,sid_pwd,cliend_id,cliend_pwd);
						}
					}
				} else if (intent.hasExtra("sid_pwd")) {
					cliend_id = null;
					cliend_pwd = null;
					sid = null;
					sid_pwd = null;
					String token = intent.getStringExtra("sid_pwd");
					CustomLog.i(DfineAction.TAG_TCP, "TOKEN:"+token);
					connectionService(null,null,token);
				}
			} else if (intent.getAction().equals(UIDfineAction.ACTION_DIAL)) {
				int type = intent.getIntExtra("type", 1);
				String uid = intent.getStringExtra(UIDfineAction.CALL_UID);
				String phone = intent.getStringExtra(UIDfineAction.CALL_PHONE);
				// FROM_NUM_KEY（主叫显号）和TO_NUM_KEY（被叫显号），仅仅作为回拨的时候显示用：可以为空
				String fromSerNum = "";
				String toSerNum = "";
				if (intent.hasExtra(UIDfineAction.FROM_NUM_KEY)) {
					fromSerNum = intent.getStringExtra(UIDfineAction.FROM_NUM_KEY);
				}
				if (intent.hasExtra(UIDfineAction.TO_NUM_KEY)) {
					toSerNum = intent.getStringExtra(UIDfineAction.TO_NUM_KEY);
				}
				// type:
				// 0：直拨
				// 1：免费
				// 2:回拨
				// 3:视频点对点
				// 4:会议
				// 5:智能拨打
				switch (type) {
				case 0:
					// UCSCall.dial(ConnectionService.this,CallType.DIRECT,
					// phone);
					break;
				case 1:
					// 方法有进行重载，可以传入透传字段也可以不传入。
					// UCSCall.dial(ConnectionService.this,CallType.VOIP, uid);
					UCSCall.dial(ConnectionService.this, CallType.VOIP, uid);
					break;
				case 2:
					// UCSCall.callBack(ConnectionService.this, phone,
					// fromSerNum, toSerNum);
					break;
				case 3:
					UCSCall.dial(ConnectionService.this, CallType.VIDEO, uid);
					break;
				case 5:
					// UCSCall.dial(ConnectionService.this, CallType.CALL_AUTO,
					// phone);
					break;
				}
			} else if (intent.getAction().equals(UIDfineAction.ACTION_LOGOUT)) {
				//UCSService.uninit(ConnectionService.this);
			} else if (intent.getAction().equals(UCSService.ACTION_INIT_SUCCESS)) {
				CustomLog.i(DfineAction.TAG_TCP, "启动成功  ... ");
				UCSService.openSdkLog(true);
			} else if (intent.getAction().equals(UIDfineAction.ACTION_START_TIME)) {
				startCallTimer();
			}
		}
	};
	
	@Override
	public void onDestroy() {
		CustomLog.i(DfineAction.TAG_TCP,"onDestroy ... ");
		unregisterReceiver(br);
		//断开云联接
		UCSService.uninit();
		android.os.Process.killProcess(android.os.Process.myPid());
		super.onDestroy();
	}

	//连接失败或断线回调
	@Override
	public void onConnectionFailed(UcsReason reason) {
		CustomLog.i(DfineAction.TAG_TCP,"CONNECTION_FAILED:"+reason.getReason());
		if(reason.getMsg().length() > 0){
			CustomLog.i(DfineAction.TAG_TCP,"CONNECTION_FAILED:"+reason.getMsg());
		}
		sendBroadcast(new Intent(UIDfineAction.ACTION_TCP_LOGIN_CLIENT_RESPONSE).putExtra(UIDfineAction.RESULT_KEY, 1).putExtra(UIDfineAction.REASON_KEY, reason.getReason()));
		if(reason.getReason() == 300505 || reason.getReason() == 300207){
			sendBroadcast(new Intent(UIDfineAction.ACTION_LOGOUT).putExtra(UIDfineAction.REASON_KEY, reason.getReason()));
			
			Intent intent = new Intent();
			intent.setClass(ConnectionService.this,TerminalLoginDialogActivity.class);
			intent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK).putExtra("reason", reason.getReason());
			startActivity(intent);
		}
	}

	//连接成功回调
	@Override
	public void onConnectionSuccessful() {
		sendBroadcast(new Intent(UIDfineAction.ACTION_TCP_LOGIN_CLIENT_RESPONSE).putExtra(UIDfineAction.RESULT_KEY, 0));
		CustomLog.i("CONNECTION_SUCCESS ... ");
		if(cliend_id != null && cliend_id.length() > 0){
			LoginConfig.saveCurrentClientId(ConnectionService.this, cliend_id);
		}
	}
	
	public void connectionService(final String host,final String port,final String sid ,final String sidPwd ,final String cliend_id,final String cliend_pwd){
		new Thread(new Runnable() {
			@Override
			public void run() {
				if(host != null && host.length() > 0){
					UCSService.connect(host,port,sid,sidPwd,cliend_id,cliend_pwd);
				}else{
					UCSService.connect(sid,sidPwd,cliend_id,cliend_pwd);
				}
			}
		}).start();
	}
	
	public void connectionService(final String host,final String port,final String token){
		new Thread(new Runnable() {
			@Override
			public void run() {
				if(host != null && host.length() > 0){
					UCSService.connect(host,port,token);
				}else{
					UCSService.connect(token);
				}
			}
		}).start();
	}

	private int second = 0;
	private int minute = 0;
	private int hour = 0;
	private Timer timer = null;
	/**
	 * 通话走时
	 */
	public void startCallTimer(){
		stopCallTimer();
		if(timer == null){
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
				CustomLog.i(DfineAction.TAG_TCP,"timer:"+timer.toString());
				sendBroadcast(new Intent(UIDfineAction.ACTION_CALL_TIME).putExtra("callduration", hour * 3600 + minute * 60 + second).putExtra("timer", timer.toString()));
			}
		}, 0, 1000);
	}
	
	public void stopCallTimer(){
		if (timer != null){
			CustomLog.i(DfineAction.TAG_TCP,"cancel() timer");
			timer.cancel();
			timer=null;
		}
	}
	
	//对方正在响铃回调
	@Override
	public void onAlerting(String arg0) {
		CustomLog.i(DfineAction.TAG_TCP,"onAlerting CURRENT_ID:"+arg0);
		sendBroadcast(new Intent(UIDfineAction.ACTION_DIAL_STATE).putExtra("state", UCSCall.CALL_VOIP_RINGING_180));
	}

	//对方接通回调
	@Override
	public void onAnswer(String arg0) {
		CustomLog.i(DfineAction.TAG_TCP,"onAnswer CURRENT_ID:"+arg0);
		sendBroadcast(new Intent(UIDfineAction.ACTION_ANSWER));
		startCallTimer();
	}

	//拨打失败回调，请打印出错误码reason.getReason()，官网查询错误码含义
	@Override
	public void onDialFailed(String arg0, UcsReason reason) {
		DialConfig.saveCallType(ConnectionService.this, "");
		CustomLog.i("onDialFailed CURRENT_ID:"+arg0+"          SERVICE:"+reason.getReason()+"   MSG:"+reason.getMsg());
		voipSwitch(reason);
	}

	private void voipSwitch(UcsReason reason){
		stopCallTimer();
		switch(reason.getReason()){
		case 300210:
			sendBroadcast(new Intent(UIDfineAction.ACTION_DIAL_STATE).putExtra("state", UCSCall.CALL_VOIP_ERROR));
			break;
		case 300211:
			sendBroadcast(new Intent(UIDfineAction.ACTION_DIAL_STATE).putExtra("state", UCSCall.CALL_VOIP_NOT_ENOUGH_BALANCE));
			break;
		case 300212:
			sendBroadcast(new Intent(UIDfineAction.ACTION_DIAL_STATE).putExtra("state", UCSCall.CALL_VOIP_BUSY));
			break;
		case 300213:
			sendBroadcast(new Intent(UIDfineAction.ACTION_DIAL_STATE).putExtra("state", UCSCall.CALL_VOIP_REFUSAL));
			break;
		case 300214:
		case 300215:
			sendBroadcast(new Intent(UIDfineAction.ACTION_DIAL_STATE).putExtra("state", UCSCall.CALL_VOIP_NUMBER_ERROR));
			break;
		case 300216:
			sendBroadcast(new Intent(UIDfineAction.ACTION_DIAL_STATE).putExtra("state", UCSCall.CALL_VOIP_ACCOUNT_FROZEN));
			break;
		case 300217:
			sendBroadcast(new Intent(UIDfineAction.ACTION_DIAL_STATE).putExtra("state", UCSCall.CALL_VOIP_REJECT_ACCOUNT_FROZEN));
			break;
		case 300218:
			sendBroadcast(new Intent(UIDfineAction.ACTION_DIAL_STATE).putExtra("state", UCSCall.CALL_VOIP_ACCOUNT_EXPIRED));
			break;
		case 300219:
			sendBroadcast(new Intent(UIDfineAction.ACTION_DIAL_STATE).putExtra("state", UCSCall.CALL_VOIP_CALLYOURSELF));
			break;
		case 300220:
		case 300224:
			sendBroadcast(new Intent(UIDfineAction.ACTION_DIAL_STATE).putExtra("state", UCSCall.CALL_VOIP_NETWORK_TIMEOUT));
			break;
		case 300221:
			sendBroadcast(new Intent(UIDfineAction.ACTION_DIAL_STATE).putExtra("state", UCSCall.CALL_VOIP_NOT_ANSWER));
			break;
		case 300222:
			sendBroadcast(new Intent(UIDfineAction.ACTION_DIAL_STATE).putExtra("state", UCSCall.CALL_VOIP_TRYING_183));
			break;
		case 300223:
			sendBroadcast(new Intent(UIDfineAction.ACTION_DIAL_STATE).putExtra("state", UCSCall.CALL_VOIP_SESSION_EXPIRATION));
			break;
		case 300225:
			sendBroadcast(new Intent(UIDfineAction.ACTION_DIAL_STATE).putExtra("state", UCSCall.HUNGUP_MYSELF));
			break;
		case 300226:
			sendBroadcast(new Intent(UIDfineAction.ACTION_DIAL_STATE).putExtra("state", UCSCall.HUNGUP_OTHER));
			break;
		case 300267:
			sendBroadcast(new Intent(UIDfineAction.ACTION_DIAL_STATE).putExtra("state", UCSCall.HUNGUP_WHILE_2G));
			break;
		case 300248:
			sendBroadcast(new Intent(UIDfineAction.ACTION_DIAL_STATE).putExtra("state", UCSCall.HUNGUP_MYSELF_REFUSAL));
			break;
		case 300249:
			sendBroadcast(new Intent(UIDfineAction.ACTION_DIAL_STATE).putExtra("state", UCSCall.CALL_VIDEO_DOES_NOT_SUPPORT));
			break;
		default:
			if(reason.getReason() >= 10000 && reason.getReason() <= 20000){//透传错误码
				CustomLog.i(DfineAction.TAG_TCP,"KC_REASON:"+reason.getReason());
			}else if(reason.getReason() >= 300233 && reason.getReason() <= 300243){//回拨
				sendBroadcast(new Intent(UIDfineAction.ACTION_DIAL_STATE).putExtra("state", reason.getReason()));
			}
			break;
		}
	}
	
	@Override
	public void onHangUp(String arg0, UcsReason reason) {
		DialConfig.saveCallType(ConnectionService.this, "");
		UCSCall.stopCallRinging();
		sendBroadcast(new Intent(UIDfineAction.ACTION_DIAL_HANGUP).putExtra("state", reason.getReason()));
		CustomLog.i("onHangUp CURRENT_ID:"+arg0+"SERVICE:"+reason.getReason());
		voipSwitch(reason);
	}

	/**
	 * 接收新消息
	 * nickName 显示主叫昵称
	 * userdata 暂时没用到
	 */
	@Override
	public void onIncomingCall(String callId, String callType, String callerNumber ,String nickName, String userdata) {
		CustomLog.v("收到新的来电 callType="+callType);
		CustomLog.v("收到新的来电信息："+userdata);
		Intent intent = new Intent();
		if(callType.equals("0")){
			intent.setClass(ConnectionService.this,AudioConverseActivity.class);
		}else if(callType.equals("2")){
			//会议
		}else{
			//视频电话
			intent.setClass(ConnectionService.this,VideoConverseActivity.class);
		}
		intent.putExtra("phoneNumber", callerNumber).setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
		intent.putExtra("inCall", true);
		intent.putExtra("nickName", nickName);
		startActivity(intent);
	}

	/**
	 * @param 0 无法获取网络状态
	 * @param 1网络状态极好
	 * @param 2网络状态良好
	 * @param 3网络状态一般
	 * @param 4网络状态极差
	 */
	@Override
	public void onNetWorkState(int reason) {
		sendBroadcast(new Intent(UIDfineAction.ACTION_NETWORK_STATE).putExtra("state", reason));
	}

	@Override
	public void onDTMF(int dtmfCode) {
		CustomLog.d(DfineAction.TAG_TCP,"DTMF:"+dtmfCode);
	}

	@Override
	public void onCameraCapture(String filePaht) {
		CustomLog.d(DfineAction.TAG_TCP,"CAMERACAPTURE:"+filePaht);
	}
}
