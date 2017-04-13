package com.kct.flyrtc.ui;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;

import com.kct.flyrtc.utils.UIData;

/**
 * Created by zhouwq on 2017/3/10/010.
 * 页面基类
 */

public class BaseActivity extends Activity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        String type = UIData.getCallType(getApplicationContext());
        if (type.length() > 0) {
            if (type.equals("2")) {
                // 如果还在通话状态,启动通话页面
                Intent mIntent = new Intent(this, VideoCallActivity.class);
                mIntent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
                startActivity(mIntent);
            }
        }
    }

    @Override
    protected void onRestart() {
        super.onRestart();
        String type = UIData.getCallType(getApplicationContext());
        if (type.length() > 0) {
            if (type.equals("2")) {
                // 如果还在通话状态,启动通话页面
                Intent mIntent = new Intent(this, VideoCallActivity.class);
                mIntent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
                startActivity(mIntent);
            }
        }
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
    }
}
