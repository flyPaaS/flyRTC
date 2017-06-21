//
//  ChangeTheViewController.h
//  Top9
//
//  Created by admin on 14/12/5.
//  Copyright (c) 2014年 admin. All rights reserved.
//

#import <UIKit/UIKit.h>
typedef enum {
    GenerallyAnimationSliderFormLeft = 0 ,//从左边缘 直线 进入（相对其父类而言）
    GenerallyAnimationSliderFormRight ,//从右边缘 直线 进入（相对其父类而言）
    GenerallyAnimationSliderFormTop ,//从上边缘 直线 进入（相对其父类而言）
    GenerallyAnimationSliderFormBottom ,//从下边缘 直线 进入（相对其父类而言）
    
    GenerallyAnimationSliderToLeft,//直线动画到左侧（相对其父类而言）
    GenerallyAnimationSliderToRight,//直线动画到右侧 （相对其父类而言）
    GenerallyAnimationSliderToTop,//直线动画到上边缘 （相对其父类而言）
    GenerallyAnimationSliderToBottom,//直线动画到下边缘 （相对其父类而言）
    
    GenerallyAnimationFallIn,//从大到小，transform由1.5变到1
    GenerallyAnimationFallOut,//从小到大，transform由1变到1.5
    
    GenerallyAnimationPopIn,//从大到小，transform由1变到0.1 alpha由1变到0
    GenerallyAnimationPopOut,//由小到大，transform由0.1变到1 alpha由0变到1
    
    GenerallyAnimationFallSliderFormLeft,//从左侧侧滑进入，transform由0.1变到1
    GenerallyAnimationFallSliderFormRight,//从右侧侧滑进入，transform由0.1变到1
    GenerallyAnimationFallSliderFormTop,//从顶部侧滑进入，transform由0.1变到1
    GenerallyAnimationFallSliderFormBottom,//从下部侧滑进入，transform由0.1变到1
    
    GenerallyAnimationConverLayerFormLeft,//从左到右 遮罩
    GenerallyAnimationConverLayerFormRight,//从右到左 遮罩
    GenerallyAnimationConverLayerFormTop,//从上往下 遮罩
    GenerallyAnimationConverLayerFormBottom,//从下往上 遮罩
    GenerallyAnimationConverLayerFormCenter,//从中央扩散 遮罩
    
    GenerallyAnimationFadeIn,//淡隐淡出,出现
    GenerallyAnimationFadeOut,//消失
    
    
    GenerallyAnimationLineBack, //收缩成一条线，然后消失
    GenerallyAnimationLineOut, //由一条线慢慢扩张出现
    
    GenerallyAnimationBack, //从小到大 ,transform由1变到2 alpha从1到0
    GenerallyAnimationNormal, //正常

    
}GenerallyAnimationEnum;
@interface UIViewController(ChangeTheViewController)

- (void)generyallyAnimationWithView:(UIView *)animationView animationType:(GenerallyAnimationEnum)animationType duration:(float)animationTime delayTime:(float)delayTime finishedBlock: (void (^)(void))completion;


- (void)backTheViewAnimationWithView:(UIView *)animationview duration:(float)animationTime finishedBlock: (void (^)(void))completion;




//屏幕截图 像素压缩
-(UIImage*)beginImageContext:(CGRect)rect View:(UIView*)view;
//当前屏幕截图 像素不变
-(UIImage*)getScreenImageContext:(CGRect)rect View:(UIView*)view;

//压缩图片
- (UIImage *)imageWithImageSimple:(UIImage*)image scaledToSize:(CGSize)newSize;
- (UIImage *)imageByScalingToMaxSize:(UIImage *)sourceImage;


//常用视图动画
//旋转动画
- (void)startXuanzhuanView:(UIView *)view;
//弹跳视图
- (void)bounceTheView:(UIView *)bounceView;

/*
 左右滑动指定的视图
 */
+ (void)shakeAnimationInView : (UIView *)shakeView ;



@end
