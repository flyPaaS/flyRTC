package com.kctdemo.activity;

import com.kctdemo.R;

import android.os.Bundle;
import android.os.Handler;
import android.content.Intent;
import android.view.Window;

public class WelcomeActivity extends BaseActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
    	requestWindowFeature(Window.FEATURE_NO_TITLE);
        super.onCreate(savedInstanceState);     
        setContentView(R.layout.activity_welcome);

		new Handler().postDelayed(new Runnable() {
			@Override
			public void run() {
				startActivity(new Intent(WelcomeActivity.this, LoginActivity.class));
				WelcomeActivity.this.finish();
			}
		}, 1000);
    }
}
