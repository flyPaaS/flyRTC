package com.kct.flyrtc.ui;

import android.content.Context;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.ListView;
import android.widget.RelativeLayout;
import android.widget.TextView;

import com.kct.flyrtc.R;
import com.kct.flyrtc.utils.UIData;
import com.kct.sdk.KCSdk;

import java.util.ArrayList;

/**
 * Created by zhouwq on 2017/6/2/002.
 * 编解码配置
 */

public class CodecActivity extends BaseActivity {
    // 控件
    public ListView video_list_1 = null;
    public ListView video_list_2 = null;
    public VideoAdapter videoAdapter = null;
    public AudioAdapter audioAdapter = null;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_setting);

        RelativeLayout rls_back = findViewById(R.id.rls_back);
        rls_back.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                finish();
            }
        });

        TextView codec = findViewById(R.id.rls_codec);
        codec.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                // 设置
                KCSdk.getInstance().SetCodec(UIData.nVideoCodec, UIData.nAudioCodec);
                finish();
            }
        });

        video_list_1 = findViewById(R.id.video_list_1);
        video_list_2 = findViewById(R.id.video_list_2);
        videoAdapter = new VideoAdapter(this);
        audioAdapter = new AudioAdapter(this);
        video_list_1.setAdapter(videoAdapter);
        video_list_2.setAdapter(audioAdapter);
    }

    private class VideoAdapter extends BaseAdapter {
        // Item数据类
        class VideoClient {
            // Codec Name
            private String codecname = "";
            // Item是否选中
            private Boolean ischeck = false;

            // 获取Codec Name
            private String getCodecName() {
                return codecname;
            }
            // 设置Codec Name
            private void setCodecName(String codec) {
                this.codecname = codec;
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

        // Item 数据结构
        private LayoutInflater mInflater = null;
        private ArrayList<VideoClient> clients = new ArrayList<>();
        // Item 控件结构
        private final class VideoHolder {
            private TextView videocodec;
            private CheckBox videocb;
        }

        // 默认构造
        private VideoAdapter(Context context) {
            mInflater = (LayoutInflater) context.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
            clients.clear();
            // 添加数据
            VideoClient st = new VideoClient();
            st.setCodecName("h264-sw");
            st.setIscheck(UIData.nVideoCodec == 0);
            clients.add(st);

            st = new VideoClient();
            st.setCodecName("h264-hw");
            st.setIscheck(UIData.nVideoCodec == 1);
            clients.add(st);

            st = new VideoClient();
            st.setCodecName("h265-sw");
            st.setIscheck(UIData.nVideoCodec == 2);
            clients.add(st);

            st = new VideoClient();
            st.setCodecName("h265-hw");
            st.setIscheck(UIData.nVideoCodec == 3);
            clients.add(st);
        }

        public int getCount() {
            return clients.size();
        }

        public Object getItem(int arg0) {
            return clients.get(arg0);
        }

        public long getItemId(int arg0) {
            return arg0;
        }

        public View getView(final int position, View convertView, ViewGroup parent) {
            VideoHolder holder;
            // 设置Item布局
            if (null == convertView) {
                holder = new VideoHolder();
                convertView = mInflater.inflate(R.layout.list_codec, null);
                holder.videocb = convertView.findViewById(R.id.login_list_cb);
                holder.videocodec = convertView.findViewById(R.id.login_list_tv_client);
                convertView.setTag(holder);
            } else {
                holder = (VideoHolder)convertView.getTag();
            }

            holder.videocodec.setText(clients.get(position).getCodecName());
            if (clients.size() > position) {
                holder.videocb.setChecked(clients.get(position).getIscheck());
            }

            // 处理点击Item事件
            holder.videocb.setOnClickListener(new View.OnClickListener() {

                @Override
                public void onClick(View v) {
                    for (int i = 0; i < clients.size(); i++) {
                        clients.get(i).setIscheck(false);
                    }
                    if (clients.size() > position) {
                        clients.get(position).setIscheck(((CompoundButton) v).isChecked());
                        UIData.nVideoCodec = position;
                    }
                    notifyDataSetChanged();
                }
            });
            return convertView;
        }
    }

    private class AudioAdapter extends BaseAdapter {
        // Item数据类
        class AudioClient {
            // Codec Name
            private String codecname = "";
            // Item是否选中
            private Boolean ischeck = false;

            // 获取Codec Name
            private String getCodecName() {
                return codecname;
            }
            // 设置Codec Name
            private void setCodecName(String codec) {
                this.codecname = codec;
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

        // Item 数据结构
        private LayoutInflater mInflater = null;
        private ArrayList<AudioClient> clients = new ArrayList<>();
        // Item 控件结构
        private final class AudioHolder {
            private TextView audiocodec;
            private CheckBox audiocb;
        }

        // 默认构造
        private AudioAdapter(Context context) {
            mInflater = (LayoutInflater) context.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
            clients.clear();
            // 添加数据
            AudioClient st = new AudioClient();
            st.setCodecName("OPUS");
            st.setIscheck(UIData.nAudioCodec == 0);
            clients.add(st);

            st = new AudioClient();
            st.setCodecName("AAC");
            st.setIscheck(UIData.nAudioCodec == 1);
            clients.add(st);
        }

        public int getCount() {
            return clients.size();
        }

        public Object getItem(int arg0) {
            return clients.get(arg0);
        }

        public long getItemId(int arg0) {
            return arg0;
        }

        public View getView(final int position, View convertView, ViewGroup parent) {
            AudioHolder holder;
            // 设置Item布局
            if (null == convertView) {
                holder = new AudioHolder();
                convertView = mInflater.inflate(R.layout.list_codec, null);
                holder.audiocb = convertView.findViewById(R.id.login_list_cb);
                holder.audiocodec = convertView.findViewById(R.id.login_list_tv_client);
                convertView.setTag(holder);
            } else {
                holder = (AudioHolder)convertView.getTag();
            }

            holder.audiocodec.setText(clients.get(position).getCodecName());
            if (clients.size() > position) {
                holder.audiocb.setChecked(clients.get(position).getIscheck());
            }

            // 处理点击Item事件
            holder.audiocb.setOnClickListener(new View.OnClickListener() {

                @Override
                public void onClick(View v) {
                    for (int i = 0; i < clients.size(); i++) {
                        clients.get(i).setIscheck(false);
                    }
                    if (clients.size() > position) {
                        clients.get(position).setIscheck(((CompoundButton) v).isChecked());
                        UIData.nAudioCodec = position;
                    }
                    notifyDataSetChanged();
                }
            });
            return convertView;
        }
    }
}
