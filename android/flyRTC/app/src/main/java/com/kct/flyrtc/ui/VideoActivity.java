package com.kct.flyrtc.ui;

import android.annotation.SuppressLint;
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
import android.widget.ImageView;
import android.widget.ListView;
import android.widget.RelativeLayout;
import android.widget.TextView;

import com.kct.flyrtc.R;
import com.kct.flyrtc.utils.UIAction;
import com.kct.flyrtc.utils.UIData;
import com.yzx.preference.UserData;
import com.yzx.tcp.TcpTools;

import java.util.ArrayList;

/**
 * Created by zhouwq on 2017/3/8/008.
 * 视频拨号界面
 */

public class VideoActivity extends BaseActivity {
    // 是否显示对象
    private boolean bShow = true;
    // 是否掉线过
    private boolean bLine = true;

    public ListView video_list = null;
    public VideoAdapter adapter = null;
    public VideoAdapter adapterNew = null;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_video);
        // 控件
        adapter = new VideoAdapter(this, 0);
        adapterNew = new VideoAdapter(this, 1);
        video_list = (ListView)findViewById(R.id.video_list);
        int nMode = getIntent().getIntExtra("mode", 0);
        if (nMode == 0) {
            video_list.setAdapter(adapter);
        } else {
            video_list.setAdapter(adapterNew);
        }
        // 返回
        RelativeLayout rl_back = (RelativeLayout) findViewById(R.id.rl_back);
        rl_back.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                Intent mIntent = new Intent(UIAction.ACTION_LOGIN_RESPONSE);
                mIntent.putExtra(UIAction.RESULT_KEY, 2);
                sendBroadcast(mIntent);
                TcpTools.tcpDisconnection();
                finish();
            }
        });

        TextView codec = (TextView)findViewById(R.id.rl_codec);
        codec.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                // 拨打电话
                Intent mIntent = new Intent(VideoActivity.this, CodecActivity.class);
                startActivity(mIntent);
            }
        });

        // 注册广播
        IntentFilter mIntentFilter = new IntentFilter();
        mIntentFilter.addAction(UIAction.ACTION_LOGIN_RESPONSE);
        registerReceiver(mBroadcastReceiver, mIntentFilter);
    }

    @SuppressLint("HandlerLeak")
    private Handler handler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            if (msg.what == 1000) {
                TcpTools.tcpConnection();
            }
            super.handleMessage(msg);
        }
    };

    @Override
    protected void onResume() {
        super.onResume();
        bShow = true;
        if (!bLine) {
            handler.sendEmptyMessageDelayed(1000, 3000);
            bLine = true;
        }
    }

    @Override
    protected void onPause() {
        super.onPause();
        bShow = false;
    }

    private BroadcastReceiver mBroadcastReceiver = new BroadcastReceiver() {

        @Override
        public void onReceive(Context context, Intent intent) {
            if (intent.getAction().equals(UIAction.ACTION_LOGIN_RESPONSE)) {
                int nResult = intent.getIntExtra(UIAction.RESULT_KEY, 0);
                if (nResult == 3) {
                    bLine = false;
                    // 读错误连接断开
                    if (UserData.UGO_CALL_STATUS == 0) {
                        if (bShow) {
                            handler.sendEmptyMessageDelayed(1000, 3000);
                        }
                    } else {
                        handler.sendEmptyMessageDelayed(1000, 3000);
                    }
                }
            }
        }
    };

    @Override
    protected void onDestroy() {
        unregisterReceiver(mBroadcastReceiver);
        super.onDestroy();
    }

    @Override
    public void onBackPressed() {
        Intent mIntent = new Intent(UIAction.ACTION_LOGIN_RESPONSE);
        mIntent.putExtra(UIAction.RESULT_KEY, 2);
        sendBroadcast(mIntent);
        TcpTools.tcpDisconnection();
        super.onBackPressed();
    }

    class VideoAdapter extends BaseAdapter {
        private LayoutInflater mInflater;
        private ArrayList<ChildClient> clients = new ArrayList<>();

        class ViewHolder {
            private ImageView video_list_iv;
            private RelativeLayout video_list_bt;
            private TextView video_list_tv_client;
            private TextView video_list_tv_phone;
        }

        class ChildClient {
            private String clientid = "";
            private String phone = "";

            private String getClientid() {
                return clientid;
            }

            private void setClientid(String clientid) {
                this.clientid = clientid;
            }

            private String getPhone() {
                return phone;
            }

            private void setPhone(String phone) {
                this.phone = phone;
            }
        }

        private VideoAdapter(Context context, int nMode) {
            clients.clear();
            mInflater = LayoutInflater.from(context);
            if (nMode == 0) {
                for (int i = 0; i < UIData.clientId.size(); i++) {
                    if (i != UIData.nSelect) {
                        ChildClient child = new ChildClient();
                        child.setClientid(UIData.clientId.get(i));
                        child.setPhone(UIData.clientNum.get(i));
                        clients.add(child);
                    }
                }
            }
            if (nMode == 1) {
                for (int i = 0; i < UIData.clientIdNew.size(); i++) {
                    if (i != UIData.nSelect) {
                        ChildClient child = new ChildClient();
                        child.setClientid(UIData.clientIdNew.get(i));
                        child.setPhone(UIData.clientNumNew.get(i));
                        clients.add(child);
                    }
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
                convertView = mInflater.inflate(R.layout.list_video, null);
                viewHolder.video_list_iv = (ImageView) convertView.findViewById(R.id.common_list_iv);
                viewHolder.video_list_bt = (RelativeLayout) convertView.findViewById(R.id.common_list_rl);
                viewHolder.video_list_tv_client = (TextView) convertView.findViewById(R.id.common_list_tv_client);
                viewHolder.video_list_tv_phone = (TextView) convertView.findViewById(R.id.common_list_tv_phone);
                convertView.setTag(viewHolder);
            } else {
                viewHolder = (ViewHolder) convertView.getTag();
            }

            switch (position % 6) {
                case 0:
                    viewHolder.video_list_iv.setBackgroundResource(R.mipmap.head1_small);
                    break;
                case 1:
                    viewHolder.video_list_iv.setBackgroundResource(R.mipmap.head2_small);
                    break;
                case 2:
                    viewHolder.video_list_iv.setBackgroundResource(R.mipmap.head3_small);
                    break;
                case 3:
                    viewHolder.video_list_iv.setBackgroundResource(R.mipmap.head4_small);
                    break;
                case 4:
                    viewHolder.video_list_iv.setBackgroundResource(R.mipmap.head5_small);
                    break;
                case 5:
                    viewHolder.video_list_iv.setBackgroundResource(R.mipmap.head6_small);
                    break;
                default:
                    viewHolder.video_list_iv.setBackgroundResource(R.mipmap.head1_small);
                    break;
            }
            viewHolder.video_list_tv_client.setText(clients.get(position).getClientid());
            viewHolder.video_list_tv_phone.setText(clients.get(position).getPhone());

            final String call_client = clients.get(position).getClientid();
            viewHolder.video_list_bt.setOnClickListener(new View.OnClickListener() {

                @Override
                public void onClick(View v) {
                    notifyDataSetChanged();
                    // 拨打电话
                    Intent mIntent = new Intent(VideoActivity.this, VideoCallActivity.class);
                    mIntent.putExtra("phoneNumber", call_client);
                    mIntent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
                    startActivity(mIntent);
                }
            });
            return convertView;
        }
    }
}
