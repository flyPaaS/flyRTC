package com.kct.flyrtc.utils;

import android.content.Context;
import android.util.DisplayMetrics;
import android.view.MotionEvent;
import android.view.View;
import android.view.WindowManager;

/**
 * Created by zhouwq on 2017/11/16/016.
 * View拖动事件监听
 */

public class TouchListener implements View.OnTouchListener {
    // 位置参数
    private int lastX = 0;
    private int lastY = 0;
    private int screenWidth = 0;
    private int screenHeight = 0;
    // 窗口布局参数
    private WindowManager mWindowManager = null;
    private WindowManager.LayoutParams mLayoutParams = null;
    // 单击判断
    private boolean isClick = false;
    private long startTime = 0;
    private long endTime = 0;

    // 构造参数
    public TouchListener(Context mContext, WindowManager windowManager, WindowManager.LayoutParams layoutParams) {
        mWindowManager = windowManager;
        mLayoutParams = layoutParams;
        DisplayMetrics mDisplayMetrics = mContext.getResources().getDisplayMetrics();
        screenWidth = mDisplayMetrics.widthPixels;
        screenHeight = mDisplayMetrics.heightPixels;
    }

    @Override
    public boolean onTouch(View v, MotionEvent event) {
        switch (event.getAction()) {
            case MotionEvent.ACTION_DOWN:
                lastX = (int) event.getRawX();
                lastY = (int) event.getRawY();
                isClick = false;
                startTime = System.currentTimeMillis();
                break;
            case MotionEvent.ACTION_MOVE:
                isClick = true;
                int dx = (int) event.getRawX() - lastX;
                int dy = (int) event.getRawY() - lastY;
                int left = v.getLeft() + dx;
                int top = v.getTop() + dy;
                int right = v.getRight() + dx;
                int bottom = v.getBottom() + dy;
                // 判断边界
                if (left < 0) {
                    left = 0;
                    right = left + v.getWidth();
                }
                if (right > screenWidth) {
                    right = screenWidth;
                    left = right - v.getWidth();
                }
                if (top < 0) {
                    top = 0;
                    bottom = top + v.getHeight();
                }
                if (bottom > screenHeight) {
                    bottom = screenHeight;
                    top = bottom - v.getHeight();
                }
                v.layout(left, top, right, bottom);
                // 更新新的位置
                lastX = (int) event.getRawX();
                lastY = (int) event.getRawY();
                mLayoutParams.x = lastX - (right - left) / 2;
                mLayoutParams.y = lastY - (bottom - top) / 2;
                mWindowManager.updateViewLayout(v, mLayoutParams);
                break;
            case MotionEvent.ACTION_UP:
                endTime = System.currentTimeMillis();
                if ((endTime - startTime) > 0.1 * 1000L) {
                    isClick = true;
                } else {
                    isClick = false;
                }
                break;
        }
        return isClick;
    }
}
