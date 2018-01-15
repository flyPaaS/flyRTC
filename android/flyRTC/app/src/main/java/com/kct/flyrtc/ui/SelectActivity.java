package com.kct.flyrtc.ui;

import android.annotation.SuppressLint;
import android.app.ProgressDialog;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.ImageView;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.Toast;

import com.kct.flyrtc.R;
import com.kct.flyrtc.rest.RestHttpClient;
import com.kct.flyrtc.utils.UIAction;
import com.kct.flyrtc.utils.UIData;
import com.kct.sdk.KCSdk;

import java.util.ArrayList;
import java.util.Timer;
import java.util.TimerTask;

/**
 * Created by zhouwq on 2017/3/8/008.
 * Client登录界面
 */

public class SelectActivity extends BaseActivity {
    // 登录超时定时器
    private Timer mTimer = null;
    // 控件
    private ProgressDialog mProgressDialog = null;
    // 选中标记
    private int nSelect = -1;
    // 是否显示
    private boolean bShow = true;

    // 选择模式
    private int nSelectMode = 0;
    public ListView login_list = null;
    //public LoginAdapter adapter = null;
    public LoginAdapter adapterNew = null;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_select);
        // 控件
        nSelectMode = 1;
        //final CheckBox oldbtn = findViewById(R.id.old_btn);
        //final CheckBox newbtn = findViewById(R.id.new_btn);
        //oldbtn.setChecked(true);
        //newbtn.setChecked(false);

        //adapter = new LoginAdapter(this, 0);
        adapterNew = new LoginAdapter(this, 1);
        login_list = findViewById(R.id.login_list);
        login_list.setAdapter(adapterNew);
        // 注册广播
        IntentFilter mIntentFilter = new IntentFilter();
        mIntentFilter.addAction(UIAction.ACTION_LOGIN_RESPONSE);
        registerReceiver(mBroadcastReceiver, mIntentFilter);

        /*
        oldbtn.setOnClickListener(new View.OnClickListener() {

            @Override
            public void onClick(View v) {
                nSelectMode = 0;
                adapter.notifyDataSetChanged();
                login_list.setAdapter(adapter);
                oldbtn.setChecked(true);
                newbtn.setChecked(false);
            }
        });
        newbtn.setOnClickListener(new View.OnClickListener() {

            @Override
            public void onClick(View v) {
                nSelectMode = 1;
                adapterNew.notifyDataSetChanged();
                login_list.setAdapter(adapterNew);
                oldbtn.setChecked(false);
                newbtn.setChecked(true);
            }
        });*/

        // 登入第二步：选中子账户进行登入
        findViewById(R.id.login_client_bt).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (nSelect >= 0) {
                    startCallTimer();
                    startProgDlg();
                    // 登陆
                    new Thread(new Runnable() {
                        @Override
                        public void run() {
                            RestHttpClient mRestHttpClient = new RestHttpClient();
                            if (nSelectMode == 0) {
                                if (mRestHttpClient.queryAccountLine(UIData.clientId.get(nSelect)) == 0) {
                                    // 可以登录
                                    UIData.nSelect = nSelect;
                                    KCSdk.getInstance().Login(UIData.accountSid, UIData.accountToken, UIData.clientId.get(nSelect), UIData.clientPwd.get(nSelect));
                                } else {
                                    handler.sendEmptyMessage(1002);
                                }
                            }
                            if (nSelectMode == 1) {
                                if (mRestHttpClient.queryAccountLine(UIData.clientIdNew.get(nSelect)) == 0) {
                                    // 可以登录
                                    UIData.nSelect = nSelect;
                                    KCSdk.getInstance().Login(UIData.accountSidNew, UIData.accountTokenNew, UIData.clientIdNew.get(nSelect), UIData.clientPwdNew.get(nSelect));
                                } else {
                                    handler.sendEmptyMessage(1002);
                                }
                            }
                        }
                    }).start();
                } else {
                    handler.sendEmptyMessage(1003);
                }
            }
        });
    }

    @Override
    protected void onResume() {
        super.onResume();
        bShow = true;
    }

    @Override
    protected void onPause() {
        super.onPause();
        bShow = false;
    }

    @Override
    protected void onDestroy() {
        stopProgDlg();
        stopCallTimer();
        unregisterReceiver(mBroadcastReceiver);
        super.onDestroy();
    }

    @SuppressLint("HandlerLeak")
    private Handler handler = new Handler () {
        @Override
        public void handleMessage(Message msg) {
            stopProgDlg();
            stopCallTimer();
            if (msg.what == 1000) {
                if (bShow) {
                    Toast.makeText(SelectActivity.this, getString(R.string.login_fail), Toast.LENGTH_SHORT).show();
                }
            }
            if (msg.what == 1001) {
                if (bShow) {
                    Toast.makeText(SelectActivity.this, getString(R.string.login_ok), Toast.LENGTH_SHORT).show();
                    // 启动视频界面
                    Intent mIntent = new Intent(SelectActivity.this, VideoActivity.class);
                    mIntent.putExtra("mode", nSelectMode);
                    mIntent.putExtra("select", nSelect);
                    startActivity(mIntent);
                }
            }
            if (msg.what == 1002) {
                Toast.makeText(SelectActivity.this, getString(R.string.login_client_line), Toast.LENGTH_SHORT).show();
            }
            if (msg.what == 1003) {
                Toast.makeText(SelectActivity.this, getString(R.string.login_select), Toast.LENGTH_SHORT).show();
            }
            if (msg.what == 1004) {
                Toast.makeText(SelectActivity.this, getString(R.string.login_out), Toast.LENGTH_SHORT).show();
            }
            super.handleMessage(msg);
        }
    };

    private BroadcastReceiver mBroadcastReceiver = new BroadcastReceiver() {

        @Override
        public void onReceive(Context context, Intent intent) {
            String str = intent.getAction();
            if (str != null) {
                if (str.equals(UIAction.ACTION_LOGIN_RESPONSE)) {
                    int nResult = intent.getIntExtra(UIAction.RESULT_KEY, 0);
                    if (nResult == 0) {
                        // 登录成功
                        handler.sendEmptyMessage(1001);
                    }
                    if (nResult == 1) {
                        // 登录失败
                        handler.sendEmptyMessage(1000);
                    }
                    if (nResult == 2) {
                        // 手动退出
                        handler.sendEmptyMessage(1004);
                    }
                }
            }
        }
    };

    // 客户端列表适配器
    class LoginAdapter extends BaseAdapter {
        // Item 数据结构
        private LayoutInflater mInflater = null;
        private ArrayList<ChildClient> clients = new ArrayList<>();

        // Item 控件结构
        class ViewHolder {
            private ImageView login_list_iv;
            private CheckBox login_list_cb;
            private TextView login_list_tv_client;
            private TextView login_list_tv_phone;
        }

        // Item数据类
        class ChildClient {
            // 客户端id
            private String clientid = "";
            // 电话名称
            private String phone = "";
            // Item是否选中
            private Boolean ischeck = false;

            // 获取客户端id
            private String getClientid() {
                return clientid;
            }

            // 设置客户端id
            private void setClientid(String clientid) {
                this.clientid = clientid;
            }

            // 获取电话名称
            private String getPhone() {
                return phone;
            }

            // 设置电话名称
            private void setPhone(String phone) {
                this.phone = phone;
            }

            // 获取是否选中
            private Boolean getIscheck() {
                return ischeck;
            }

            // 设置是否选中
            private void setIscheck(Boolean ischeck) {
                this.ischeck = ischeck;
            }
        }

        // 默认构造
        private LoginAdapter(Context mContext, int nMode) {
            clients.clear();
            mInflater = (LayoutInflater) mContext.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
            if (nMode == 0) {
                for (int i = 0; i < UIData.clientId.size(); i++) {
                    ChildClient child = new ChildClient();
                    child.setClientid(UIData.clientId.get(i));
                    child.setPhone(UIData.clientNum.get(i));
                    child.setIscheck(false);
                    clients.add(child);
                }
            }
            if (nMode == 1) {
                for (int i = 0; i < UIData.clientIdNew.size(); i++) {
                    ChildClient child = new ChildClient();
                    child.setClientid(UIData.clientIdNew.get(i));
                    child.setPhone(UIData.clientNumNew.get(i));
                    child.setIscheck(false);
                    clients.add(child);
                }
            }
        }

        @Override
        public int getCount() {
            return clients.size();
        }

        @Override
        public Object getItem(int position) {
            return clients.get(position);
        }

        @Override
        public long getItemId(int position) {
            return position;
        }

        @SuppressLint("InflateParams")
        @Override
        public View getView(final int position, View convertView, ViewGroup parent) {
            // 设置Item布局
            ViewHolder viewHolder;
            if (null == convertView) {
                viewHolder = new ViewHolder();
                convertView = mInflater.inflate(R.layout.list_login, null);
                viewHolder.login_list_iv = convertView.findViewById(R.id.login_list_iv);
                viewHolder.login_list_cb = convertView.findViewById(R.id.login_list_cb);
                viewHolder.login_list_tv_client = convertView.findViewById(R.id.login_list_tv_client);
                viewHolder.login_list_tv_phone = convertView.findViewById(R.id.login_list_tv_phone);
                convertView.setTag(viewHolder);
            } else {
                viewHolder = (ViewHolder) convertView.getTag();
            }

            switch (position % 6) {
                case 0:
                    viewHolder.login_list_iv.setBackgroundResource(R.mipmap.head1_small);
                    break;
                case 1:
                    viewHolder.login_list_iv.setBackgroundResource(R.mipmap.head2_small);
                    break;
                case 2:
                    viewHolder.login_list_iv.setBackgroundResource(R.mipmap.head3_small);
                    break;
                case 3:
                    viewHolder.login_list_iv.setBackgroundResource(R.mipmap.head4_small);
                    break;
                case 4:
                    viewHolder.login_list_iv.setBackgroundResource(R.mipmap.head5_small);
                    break;
                case 5:
                    viewHolder.login_list_iv.setBackgroundResource(R.mipmap.head6_small);
                    break;
                default:
                    viewHolder.login_list_iv.setBackgroundResource(R.mipmap.head1_small);
                    break;
            }

            viewHolder.login_list_tv_client.setText(clients.get(position).getClientid());
            viewHolder.login_list_tv_phone.setText(clients.get(position).getPhone());
            if (clients.size() > position) {
                viewHolder.login_list_cb.setChecked(clients.get(position).getIscheck());
            }

            // 处理点击Item事件
            viewHolder.login_list_cb.setOnClickListener(new View.OnClickListener() {

                @Override
                public void onClick(View v) {
                    for (int i = 0; i < clients.size(); i++) {
                        clients.get(i).setIscheck(false);
                    }
                    if (clients.size() > position) {
                        clients.get(position).setIscheck(((CompoundButton) v).isChecked());
                        nSelect = position;
                    }
                    notifyDataSetChanged();
                }
            });
            return convertView;
        }
    }

    // 弹出登录等待框
    private void startProgDlg() {
        stopProgDlg();
        if (mProgressDialog == null) {
            mProgressDialog = new ProgressDialog(this);
        }
        mProgressDialog.setIndeterminate(true);
        mProgressDialog.setCancelable(true);
        mProgressDialog.setMessage(getString(R.string.login_client_progress));
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
