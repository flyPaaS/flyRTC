package com.kct.flyrtc.ui;

import android.Manifest;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.os.Build;
import android.os.Bundle;
import android.os.Handler;
import android.support.annotation.NonNull;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;
import android.support.v7.app.AppCompatActivity;
import android.widget.Toast;

import com.kct.flyrtc.R;
import com.kct.flyrtc.utils.AppStatusManager;

public class MainActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        AppStatusManager.getInstance().setAppStatus(1);
        // 定义权限
        String[] str = new String[4];
        str[0] = Manifest.permission.CAMERA;
        str[1] = Manifest.permission.RECORD_AUDIO;
        str[2] = Manifest.permission.WRITE_EXTERNAL_STORAGE;
        str[3] = Manifest.permission.CALL_PHONE;
        // 版本判断。当手机系统大于 23 时，才有必要去判断权限是否获取
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            // 检查该权限是否已经获取
            int nResult1 = ContextCompat.checkSelfPermission(this, Manifest.permission.CAMERA);
            int nResult2 = ContextCompat.checkSelfPermission(this, Manifest.permission.RECORD_AUDIO);
            int nResult3 = ContextCompat.checkSelfPermission(this, Manifest.permission.WRITE_EXTERNAL_STORAGE);
            int nResult4 = ContextCompat.checkSelfPermission(this, Manifest.permission.CALL_PHONE);
            // 权限是否已经授权
            if (nResult1 != PackageManager.PERMISSION_GRANTED || nResult2 != PackageManager.PERMISSION_GRANTED ||
                    nResult3 != PackageManager.PERMISSION_GRANTED || nResult4 != PackageManager.PERMISSION_GRANTED ) {
                // 如果没有授予该权限，就去提示用户请求
                ActivityCompat.requestPermissions(this, str, 0);
                return;
            }
        }

        // 启动
        new Handler().postDelayed(new Runnable() {
            @Override
            public void run() {
                Intent mIntent = new Intent(MainActivity.this, LoginActivity.class);
                startActivity(mIntent);
                finish();
            }
        }, 1000);
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        // 分析结果
        if (requestCode == 0) {
            if (grantResults[0] == PackageManager.PERMISSION_GRANTED && grantResults[1] == PackageManager.PERMISSION_GRANTED &&
                    grantResults[2] == PackageManager.PERMISSION_GRANTED && grantResults[3] == PackageManager.PERMISSION_GRANTED) {
                // 权限都申请成功，跳入界面
                new Handler().postDelayed(new Runnable() {
                    @Override
                    public void run() {
                        Intent mIntent = new Intent(MainActivity.this, LoginActivity.class);
                        startActivity(mIntent);
                        finish();
                    }
                }, 1000);
            } else {
                Toast.makeText(this, "you denied the permission", Toast.LENGTH_SHORT).show();
                finish();
            }
        }
    }
}
