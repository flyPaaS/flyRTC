package com.kctdemo.activity;

import java.util.Timer;
import java.util.TimerTask;

import com.kctdemo.R;
import com.yzx.api.UCSService;
import com.yzx.tools.CustomLog;
import com.kctdemo.action.UIDfineAction;
import com.kctdemo.restClient.JsonReqClient;
import com.kctdemo.tools.Config;
import com.kctdemo.tools.DataTools;
import com.kctdemo.tools.DfineAction;
import com.kctdemo.tools.LoginConfig;
import com.kctdemo.ui.LoginDialog;

import android.annotation.SuppressLint;
import android.app.ProgressDialog;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Handler;
import android.os.Message;
import android.text.Html;
import android.view.View;
import android.view.Window;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;

public class LoginActivity extends BaseActivity {
	// 控件
	private EditText login_admin = null;
	private EditText login_pwd = null;
	private ProgressDialog mProgressDialog = null;
	private LoginDialog mLoginDialog = null;
	
	private String login_admin_str = "";
	private Timer timer;// 登录超时定时器
	
	private BroadcastReceiver br = new BroadcastReceiver() {
		@Override
		public void onReceive(Context context, Intent intent) {
			if (mProgressDialog != null) {
				mProgressDialog.dismiss();
				mProgressDialog = null;
			}
			// 接收连接回调后发送的广播
			if (intent.getAction().equals(UIDfineAction.ACTION_TCP_LOGIN_RESPONSE)) {
				// 登入第一步后收到的回调广播，获得子庄户信息，即将在对话框中显示出来
				showSelectDialog(intent.getIntExtra(UIDfineAction.RESULT_KEY, 1));
			} else if (intent.getAction().equals(UIDfineAction.ACTION_TCP_LOGIN_CLIENT_RESPONSE)) {
				// 登入第二步后收到的回调广播，子账户登入成功，即将进入能力展示界面
				if (null != mLoginDialog && mLoginDialog.isShowing()) {
					if (intent.getIntExtra(UIDfineAction.RESULT_KEY, 1) == 0) {
						Toast.makeText(LoginActivity.this, "成功", Toast.LENGTH_SHORT).show();
						if (mLoginDialog != null) {
							mLoginDialog.dismiss();
							mLoginDialog = null;
						}
						handler.sendEmptyMessageDelayed(1, 1000);
					} else if (intent.getIntExtra(UIDfineAction.RESULT_KEY, 1) == 3) {
						Toast.makeText(LoginActivity.this, "失败:" + intent.getStringExtra(UIDfineAction.REASON_KEY), Toast.LENGTH_SHORT).show();
					}
					else {
						Toast.makeText(LoginActivity.this, "失败:" + intent.getIntExtra(	UIDfineAction.REASON_KEY, 1), Toast.LENGTH_SHORT).show();
					}
				}
			}
		}
	};

	public void showSelectDialog(int result) {
		if (mLoginDialog == null || !mLoginDialog.isShowing()) {
			if (result == 0 || result == 2) {
				CustomLog.i(DfineAction.TAG_TCP, "打开子账号对话框");
				
				if (result == 0) {
					Toast.makeText(LoginActivity.this, "登录成功", Toast.LENGTH_SHORT).show();
				}
				
				LoginConfig.saveCurrentSidAndToken(LoginActivity.this, login_admin_str, login_pwd.getText().toString());
				LoginConfig.saveCurrentSid(LoginActivity.this, login_admin_str);
				Config.initProperties(LoginActivity.this);
				if (null != mLoginDialog) {
					mLoginDialog = null;
				}
				mLoginDialog = new LoginDialog(LoginActivity.this);
				mLoginDialog.show();
			} else {
				String str = "";
				switch (result) {
				case 10:
					str = "JSON错误";
					break;
				case 11:
					str = "没有SD卡或内存不足";
					break;
				case 12:
					str = "IO错误";
					break;
				case 101111:
					str = "用户名和密码错误";
					break;
				default:
					str = result + "";
					break;
				}
				Toast.makeText(LoginActivity.this, "登录失败:" + str,
						Toast.LENGTH_SHORT).show();
			}
		}
	}

	protected void onCreate(android.os.Bundle savedInstanceState) {
		requestWindowFeature(Window.FEATURE_NO_TITLE);
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_login);

		IntentFilter ift = new IntentFilter();
		ift.addAction(UIDfineAction.ACTION_TCP_LOGIN_RESPONSE);
		ift.addAction(UIDfineAction.ACTION_TCP_LOGIN_CLIENT_RESPONSE);
		registerReceiver(br, ift);

		login_admin = (EditText) findViewById(R.id.login_admin);
		login_pwd = (EditText) findViewById(R.id.login_pwd);

		// 填入账户和密码
		final String token[] = LoginConfig.getCurrentSidAndToken(this);
		if (token != null && token.length >= 2) {
			login_admin.setText(token[0] != null ? token[0] : "");
			login_pwd.setText(token[1] != null ? token[1] : "");
		}

		// 登入第一步：获得子账号信息
		findViewById(R.id.login_btn).setOnClickListener(
				new View.OnClickListener() {
					@Override
					public void onClick(View v) {
						checkTest();
						if (login_admin.getText().length() > 0 && login_pwd.getText().length() > 0) {
							startCallTimer();
							showProgressDialog();
							// 登陆
							new Thread(new Runnable() {
								@Override
								public void run() {
									JsonReqClient client = new JsonReqClient();
									String json = client.login(login_admin_str, login_pwd.getText().toString());
									CustomLog.i(DfineAction.TAG_TCP, "RESULT:" + json);
									Config.parseConfig(json, LoginActivity.this);
								}
							}).start();
						}
					}
				});

		findViewById(R.id.login_register).setVisibility(View.GONE);
		String version = "SDK_VERSION:" + UCSService.getSDKVersion() + "\r\nDEMO_VERSION:" + DataTools.getSoftVersion(this);
		((TextView) findViewById(R.id.package_version)).setText(Html.fromHtml(version));
	}

	public void checkTest() {
		if (login_admin.getText().toString().startsWith("*#*")) {
			DataTools.istest = true;
			login_admin_str = login_admin.getText().toString().substring(3);
		} else {
			DataTools.istest = false;
			login_admin_str = login_admin.getText().toString();
		}
	}

	private void showProgressDialog() {
		if (mProgressDialog == null) {
			mProgressDialog = new ProgressDialog(LoginActivity.this);
		}
		mProgressDialog.setIndeterminate(true);
		mProgressDialog.setCancelable(true);
		mProgressDialog.setMessage("正在获取账号,请稍等...");
		mProgressDialog.show();
	}

	@Override
	protected void onDestroy() {
		if (mProgressDialog != null) {
			mProgressDialog.dismiss();
			mProgressDialog = null;
		}
		if (mLoginDialog != null) {
			mLoginDialog.dismiss();
			mLoginDialog = null;
		}
		stopCallTimer();
		unregisterReceiver(br);
		super.onDestroy();
	}

	/**
	 * 启动登录超时定时器
	 */
	private void startCallTimer() {
		stopCallTimer();
		if (timer == null) {
			timer = new Timer();
		}

		timer.schedule(new TimerTask() {
			@Override
			public void run() {
				handler.sendEmptyMessage(0);
			}
		}, 30000);
	}

	/**
	 * 停止登录超时定时器
	 */
	private void stopCallTimer() {
		if (timer != null) {
			timer.cancel();
			timer = null;
		}
	}

	@SuppressLint("HandlerLeak") 
	private Handler handler = new Handler() {
		@Override
		public void handleMessage(Message msg) {
			super.handleMessage(msg);
			if (msg.what == 0) {
				if (mProgressDialog != null) {
					mProgressDialog.dismiss();
					mProgressDialog = null;
					Toast.makeText(LoginActivity.this, "登录失败", Toast.LENGTH_SHORT).show();
				}
			}
			if (msg.what == 1) {
				startActivity(new Intent(LoginActivity.this, VideoActivity.class));
				//LoginActivity.this.finish();
			}
		}
	};
}
