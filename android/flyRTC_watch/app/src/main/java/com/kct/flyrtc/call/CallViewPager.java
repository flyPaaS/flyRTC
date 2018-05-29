package com.kct.flyrtc.call;

import android.content.Context;
import android.support.v4.view.ViewPager;
import android.util.AttributeSet;

/**
 * Created by zhouwq on 2017/12/19/019.
 * 滑动页面
 */

public class CallViewPager extends ViewPager {

    private boolean left = false;
    private boolean right = false;
    private boolean isScrolling = false;

    private int lastValue = -1;
    private ChangeViewCallback changeViewCallback = null;

    // 构造函数
    public CallViewPager(Context context) {
        super(context);
        init();
    }

    // 构造函数
    public CallViewPager(Context context, AttributeSet attrs) {
        super(context, attrs);
        init();
    }

    /**
     * init method .
     */
    private void init() {
        setOnPageChangeListener(listener);
    }

    public OnPageChangeListener listener = new OnPageChangeListener() {

        @Override
        public void onPageScrolled(int position, float positionOffset, int positionOffsetPixels) {
            if (isScrolling) {
                if (lastValue > positionOffsetPixels) {
                    // 递减，向右侧滑动
                    right = true;
                    left = false;
                } else if (lastValue < positionOffsetPixels) {
                    // 递减，向右侧滑动
                    right = false;
                    left = true;
                } else if (lastValue == positionOffsetPixels) {
                    right = left = false;
                }
            }
            //KCLog.e("onPageScrolled  last :arg2  ," + lastValue + ":" + positionOffsetPixels);
            lastValue = positionOffsetPixels;
        }

        @Override
        public void onPageSelected(int position) {
            //KCLog.e("onPageSelected : position = " + position);
            if (changeViewCallback != null) {
                changeViewCallback.getCurrentPageIndex(position);
            }
        }

        @Override
        public void onPageScrollStateChanged(int state) {
            if (state == 1) {
                isScrolling = true;
            } else {
                isScrolling = false;
            }

            //KCLog.e("onPageScrollStateChanged : arg0 = " + state);

            if (state == 2) {
                //KCLog.e("onPageScrollStateChanged  direction left = " + left);
                //KCLog.e("onPageScrollStateChanged  direction right = " + right);
                // 回调上层
                if (changeViewCallback != null) {
                    changeViewCallback.changeView(left, right);
                }
                right = left = false;
            }
        }
    };

    /**
     * 得到是否向右侧滑动
     *
     * @return true 为右滑动
     */
    public boolean getMoveRight() {
        return right;
    }

    /**
     * 得到是否向左侧滑动
     *
     * @return true 为左做滑动
     */
    public boolean getMoveLeft() {
        return left;
    }

    /**
     * 滑动状态改变回调
     *
     * @author zxy
     */
    public interface ChangeViewCallback {
        /**
         * 切换视图 ？决定于left和right 。
         *
         * @param left
         * @param right
         */
        void changeView(boolean left, boolean right);
        void getCurrentPageIndex(int index);
    }

    /**
     * set 设置回调
     *
     * @param callback
     */
    public void setChangeViewCallback(ChangeViewCallback callback) {
        changeViewCallback = callback;
    }
}
