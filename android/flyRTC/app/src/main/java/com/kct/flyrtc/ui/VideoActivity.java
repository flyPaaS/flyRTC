package com.kct.flyrtc.ui;

import android.annotation.SuppressLint;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.ImageView;
import android.widget.ListView;
import android.widget.RelativeLayout;
import android.widget.TextView;
import android.widget.Toast;

import com.gl.softphone.VideoEncParam;
import com.kct.flyrtc.R;
import com.kct.flyrtc.app.MainApplication;
import com.kct.flyrtc.call.VideoCallActivity;
import com.kct.flyrtc.utils.UIAction;
import com.kct.flyrtc.utils.UIData;
import com.kct.sdk.KCBase;
import com.kct.sdk.KCSdk;
import com.kct.sdk.util.KCLog;

import java.util.ArrayList;

import static com.kct.sdk.KCBase.ICE_RTPP;

/**
 * Created by zhouwq on 2017/3/8/008.
 * 视频拨号界面
 */

public class VideoActivity extends BaseActivity {
    public ListView video_list = null;
    public VideoAdapter adapter = null;
    public VideoAdapter adapterNew = null;
    // 对象
    public MainApplication mMainApplication = null;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_video);
        mMainApplication = (MainApplication) getApplication();
        mMainApplication.bReCon = true;
        // 控件
        adapter = new VideoAdapter(this, 0);
        adapterNew = new VideoAdapter(this, 1);
        video_list = findViewById(R.id.video_list);
        int nMode = getIntent().getIntExtra("mode", 0);
        if (nMode == 0) {
            video_list.setAdapter(adapter);
        } else {
            video_list.setAdapter(adapterNew);
        }
        // 返回
        RelativeLayout rl_back = findViewById(R.id.rl_back);
        rl_back.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                Intent mIntent = new Intent(UIAction.ACTION_LOGIN_RESPONSE);
                mIntent.putExtra(UIAction.RESULT_KEY, 2);
                sendBroadcast(mIntent);
                // 退出
                KCSdk.getInstance().LoginOut();
                finish();
            }
        });

        TextView codec = findViewById(R.id.rl_codec);
        codec.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                // 拨打电话
                Intent mIntent = new Intent(VideoActivity.this, CodecActivity.class);
                startActivity(mIntent);
            }
        });

        // 设置default视频参数
        VideoEncParam encParam = new VideoEncParam();
        encParam.bitrates = new int[3][3];
        encParam.usWidth = 360;   // 推荐使用240x320，或者360x480
        encParam.usHeight = 480;
        encParam.ucUseHwEnc = 0;   // 硬编码
        encParam.ucUseHwDec = 0;   // 硬解码
        encParam.usMinBitrate = 50;
        encParam.usMaxBitrate = 500;
        encParam.ucmaxFramerate = 20;
        encParam.usStartBitrate = 500;
        encParam.bitrates[1][0] = 250;  // the bitrate threshold for 240x320 to 360x480
        encParam.bitrates[1][1] = 550;  // the bitrate threshold for 360x480 to 480x640
        encParam.bitrates[1][2] = 1050;  // the bitrate threshold for 480x640 to 720x960
        KCSdk.getInstance().SetEncoderPara(encParam);

        // 注册广播
        IntentFilter mIntentFilter = new IntentFilter();
        mIntentFilter.addAction(UIAction.ACTION_LOGIN_RESPONSE);
        registerReceiver(mBroadcastReceiver, mIntentFilter);
    }

    @Override
    protected void onResume() {
        KCSdk.getInstance().SwitchVideo(1, KCBase.RotateType.RETATE_270);
        KCSdk.getInstance().StopVideo(31);
        if (!mMainApplication.bConnect) {
            // 网络断开
            KCSdk.getInstance().ChangeICE(ICE_RTPP); // 0:P2P 1:RTPP
            mMainApplication.mHandler.removeMessages(1000);
            mMainApplication.mHandler.sendEmptyMessageDelayed(1000, 0);
            KCLog.e("网络断开，启动重连");
        }
        super.onResume();
    }

    @Override
    protected void onPause() {
        super.onPause();
    }

    @Override
    protected void onDestroy() {
        unregisterReceiver(mBroadcastReceiver);
        mMainApplication.bReCon = false;
        super.onDestroy();
    }

    @Override
    public void onBackPressed() {
        Intent mIntent = new Intent(UIAction.ACTION_LOGIN_RESPONSE);
        mIntent.putExtra(UIAction.RESULT_KEY, 2);
        sendBroadcast(mIntent);
        // 退出
        KCSdk.getInstance().LoginOut();
        super.onBackPressed();
    }

    private BroadcastReceiver mBroadcastReceiver = new BroadcastReceiver() {

        @Override
        public void onReceive(Context context, Intent intent) {
            String strAction = intent.getAction();
            if (strAction != null) {
                if (strAction.equals(UIAction.ACTION_NETWORK_STATE)) {
                    if (intent.getIntExtra(UIAction.RESULT_KEY, 0) == 100) {
                        // 说明SSID已经过期，退出
                        Toast.makeText(VideoActivity.this, getString(R.string.login_out_1), Toast.LENGTH_SHORT).show();
                        KCSdk.getInstance().LoginOut();
                        finish();
                    }
                }
            }
        }
    };

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
                viewHolder.video_list_iv = convertView.findViewById(R.id.common_list_iv);
                viewHolder.video_list_bt = convertView.findViewById(R.id.common_list_rl);
                viewHolder.video_list_tv_client = convertView.findViewById(R.id.common_list_tv_client);
                viewHolder.video_list_tv_phone = convertView.findViewById(R.id.common_list_tv_phone);
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
                    if (((MainApplication)getApplication()).bConnect) {
                        Intent mIntent = new Intent(VideoActivity.this, VideoCallActivity.class);
                        mIntent.putExtra("phoneNumber", call_client);
                        mIntent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
                        startActivity(mIntent);
                    } else {
                        Toast.makeText(VideoActivity.this, getString(R.string.videe_net), Toast.LENGTH_SHORT).show();
                    }
                }
            });
            return convertView;
        }
    }
}
