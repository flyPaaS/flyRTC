package com.kctdemo;

import android.app.Application;
import android.content.Intent;

import com.kctdemo.service.ConnectionService;

public class MainApplication extends Application{
	@Override
	public void onCreate() {
		super.onCreate();
		startService(new Intent(this, ConnectionService.class));
	}
}
