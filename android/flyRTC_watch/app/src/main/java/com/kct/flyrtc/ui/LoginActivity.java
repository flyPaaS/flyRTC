package com.kct.flyrtc.ui;

import android.annotation.SuppressLint;
import android.app.ProgressDialog;
import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.text.Html;
import android.view.View;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;

import com.kct.flyrtc.R;
import com.kct.flyrtc.rest.RestHttpClient;
import com.kct.flyrtc.utils.UIData;
import com.kct.sdk.KCSdk;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.util.Timer;
import java.util.TimerTask;

/**
 * Created by zhouwq on 2017/3/7/007.
 * 登录界面
 */

public class LoginActivity extends BaseActivity {
    // 登录超时定时器
    private Timer mTimer = null;
    // 控件
    private EditText login_admin = null;
    private EditText login_pwd = null;
    private ProgressDialog mProgressDialog = null;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_login);
        // 控件
        login_admin = findViewById(R.id.login_admin);
        login_pwd = findViewById(R.id.login_pwd);
        // 显示版本
        String version = getString(R.string.sdk_version) + KCSdk.getInstance().getSDKVersion() + "\r\n" + getString(R.string.demo_version) + getSoftVersion(this);
        ((TextView) findViewById(R.id.package_version)).setText(Html.fromHtml(version));
        // 登入第一步：获得子账号信息
        findViewById(R.id.login_btn).setOnClickListener(
                new View.OnClickListener() {
                    @Override
                    public void onClick(View v) {
                        if (login_admin.getText().length() > 0 && login_pwd.getText().length() > 0) {
                            startCallTimer();
                            startProgDlg();
                            // 登陆rest
                            new Thread(new Runnable() {
                                @Override
                                public void run() {
                                    UIData.clearData();
                                    boolean bLogin = false;
                                    RestHttpClient mRestHttpClient = new RestHttpClient();
                                    String json = mRestHttpClient.loginRest(login_admin.getText().toString(), login_pwd.getText().toString());
                                    // 分析结果
                                    try {
                                        JSONObject jsonInput = new JSONObject(json);
                                        if (jsonInput.has("resp")) {
                                            JSONObject jsonObj = jsonInput.getJSONObject("resp");
                                            if (jsonObj.has("respCode") && jsonObj.getString("respCode").equals("000000")) {
                                                // 保存数据
                                                if (jsonObj.has("sid") && jsonObj.has("token")) {
                                                    UIData.accountSid = jsonObj.get("sid").toString();
                                                    UIData.accountToken = jsonObj.get("token").toString();
                                                    if (jsonObj.has("appId")) {
                                                        UIData.accountAppid = jsonObj.get("appId").toString();
                                                    }
                                                }
                                                // 解析客户端数据
                                                if (jsonObj.has("client")) {
                                                    JSONArray array = jsonObj.getJSONArray("client");
                                                    for (int i = 0; i < array.length(); i++) {
                                                        JSONObject jsonClient = (JSONObject) array.get(i);
                                                        if (jsonClient.has("client_number") && jsonClient.has("client_pwd") && jsonClient.has("mobile")) {
                                                            UIData.clientId.add(jsonClient.getString("client_number"));
                                                            UIData.clientPwd.add(jsonClient.getString("client_pwd"));
                                                            UIData.clientNum.add(jsonClient.getString("mobile"));
                                                        }
                                                    }
                                                }
                                                bLogin = true;
                                            }
                                        }
                                    } catch (JSONException e) {
                                        e.printStackTrace();
                                    }

                                    json = mRestHttpClient.loginNewRest(login_admin.getText().toString(), login_pwd.getText().toString());
                                    // 分析结果
                                    try {
                                        JSONObject jsonInput = new JSONObject(json);
                                        if (jsonInput.has("resp")) {
                                            JSONObject jsonObj = jsonInput.getJSONObject("resp");
                                            if (jsonObj.has("respCode") && jsonObj.getString("respCode").equals("000000")) {
                                                // 保存数据
                                                if (jsonObj.has("sid") && jsonObj.has("token")) {
                                                    UIData.accountSidNew = jsonObj.get("sid").toString();
                                                    UIData.accountTokenNew = jsonObj.get("token").toString();
                                                    if (jsonObj.has("appId")) {
                                                        UIData.accountAppidNew = jsonObj.get("appId").toString();
                                                    }
                                                }
                                                // 解析客户端数据
                                                if (jsonObj.has("client")) {
                                                    JSONArray array = jsonObj.getJSONArray("client");
                                                    for (int i = 0; i < array.length(); i++) {
                                                        JSONObject jsonClient = (JSONObject) array.get(i);
                                                        if (jsonClient.has("client_number") && jsonClient.has("client_pwd") && jsonClient.has("mobile")) {
                                                            UIData.clientIdNew.add(jsonClient.getString("client_number"));
                                                            UIData.clientPwdNew.add(jsonClient.getString("client_pwd"));
                                                            UIData.clientNumNew.add(jsonClient.getString("mobile"));
                                                        }
                                                    }
                                                }
                                                bLogin = true;
                                            }
                                        }
                                    } catch (JSONException e) {
                                        e.printStackTrace();
                                    }
                                    // 判断登录成功失败
                                    if (bLogin) {
                                        handler.sendEmptyMessage(1001);
                                    } else {
                                        handler.sendEmptyMessage(1000);
                                    }
                                }
                            }).start();
                        }
                    }
                });
    }

    @Override
    protected void onDestroy() {
        stopProgDlg();
        stopCallTimer();
        super.onDestroy();
    }

    @SuppressLint("HandlerLeak")
    private Handler handler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            stopProgDlg();
            stopCallTimer();
            if (msg.what == 1000) {
                Toast.makeText(LoginActivity.this, getString(R.string.login_fail), Toast.LENGTH_SHORT).show();
            }
            if (msg.what == 1001) {
                Toast.makeText(LoginActivity.this, getString(R.string.login_ok), Toast.LENGTH_SHORT).show();
                // 启动选择Client界面
                Intent mIntent = new Intent(LoginActivity.this, SelectActivity.class);
                startActivity(mIntent);
            }
            super.handleMessage(msg);
        }
    };

    // 获取软件版本
    private String getSoftVersion(Context context){
        String version = "";
        try {
            PackageManager mPackageManager = context.getPackageManager();
            PackageInfo mPackageInfo = mPackageManager.getPackageInfo(context.getPackageName(), 0);
            version = mPackageInfo.versionName;
        } catch (PackageManager.NameNotFoundException e) {
            e.printStackTrace();
        }
        return version;
    }

    // 弹出登录等待框
    private void startProgDlg() {
        stopProgDlg();
        if (mProgressDialog == null) {
            mProgressDialog = new ProgressDialog(this);
        }
        mProgressDialog.setIndeterminate(true);
        mProgressDialog.setCancelable(true);
        mProgressDialog.setMessage(getString(R.string.login_progress));
        mProgressDialog.show();
    }

    // 关闭登录等待框
    private void stopProgDlg() {
        if (mProgressDialog != null) {
            mProgressDialog.dismiss();
            mProgressDialog = null;
        }
    }

    // 启动登录超时定时器
    private void startCallTimer() {
        stopCallTimer();
        if (mTimer == null) {
            mTimer = new Timer();
        }

        mTimer.schedule(new TimerTask() {
            @Override
            public void run() {
                handler.sendEmptyMessage(1000);
            }
        }, 30000);
    }

    // 停止登录超时定时器
    private void stopCallTimer() {
        if (mTimer != null) {
            mTimer.cancel();
            mTimer = null;
        }
    }
}
