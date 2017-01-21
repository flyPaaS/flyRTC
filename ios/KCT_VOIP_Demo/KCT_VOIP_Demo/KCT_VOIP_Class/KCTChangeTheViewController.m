//
//  ChangeTheViewController.m
//  Top9
//
//  Created by admin on 14/12/5.
//  Copyright (c) 2014年 admin. All rights reserved.
//

#import "KCTChangeTheViewController.h"
#define ORIGINAL_MAX_WIDTH 640
#define ChangeViewTime 0.3 //视图切换时间

//#import "MBProgressHUD.h"

@implementation UIViewController(KCTChangeTheViewController)


#pragma mark - 视图切换
- (void)generyallyAnimationWithView:(UIView *)animationView animationType:(GenerallyAnimationEnum)animationType duration:(float)animationTime delayTime:(float)delayTime finishedBlock: (void (^)(void))completion{
    
    
    switch (animationType) {
        case GenerallyAnimationLineBack:
        {
            
            [self generallyLineBack:animationView duration:animationTime delayTime:delayTime finishedBlock:completion];
            return;
        }
            break;
        case GenerallyAnimationLineOut:
        {
            
            [self generallyLineOut:animationView duration:animationTime delayTime:delayTime finishedBlock:completion];
            return;
        }
            break;
        case GenerallyAnimationPopIn:
        {
            
            [self generallyPopIn:animationView duration:animationTime delayTime:delayTime finishedBlock:completion];
            return;
        }
            break;
        default:
            break;
    }
    
    
    CGRect oriFrame = animationView.frame;
    CGRect lastFrame = oriFrame;
    UIView *fatherView = animationView.superview;
    CGRect fatherFrame = fatherView.frame;
    float fallValue = 1.0 ;
    float viewAlphaValue = 1.0;
    UIImageView *converView = [[UIImageView alloc]initWithFrame:animationView.bounds];
    converView.backgroundColor = [UIColor redColor];
    CGRect converFrame = converView.bounds ;
    CGRect converBounds = converFrame;
    switch (animationType) {
        case GenerallyAnimationSliderFormTop:
            animationView.alpha = 0 ;
            lastFrame.origin.y = -1 * (oriFrame.size.height);
            break;
        case GenerallyAnimationSliderToTop:
            oriFrame.origin.y = -1 *(oriFrame.size.height);
            break;
        case GenerallyAnimationSliderFormBottom:
            animationView.alpha = 1 ;
            lastFrame.origin.y = fatherFrame.size.height ;
            break;
        case GenerallyAnimationSliderToBottom:
            oriFrame.origin.y = fatherFrame.size.height ;
            break;
        case GenerallyAnimationSliderFormLeft:
            animationView.alpha = 0 ;
            lastFrame.origin.x = -1 * (oriFrame.size.width) ;
            break;
        case GenerallyAnimationSliderToLeft:
            oriFrame.origin.x = -1 *(oriFrame.size.width);
            break;
        case GenerallyAnimationSliderFormRight:
            animationView.alpha = 1 ;
            lastFrame.origin.x = fatherFrame.size.width;
            break;
        case GenerallyAnimationSliderToRight:
            oriFrame.origin.x = oriFrame.size.width ;
            break;
        case GenerallyAnimationFallIn:
            animationView.alpha = 0 ;
            animationView.transform = CGAffineTransformScale(CGAffineTransformMakeRotation(0.0), 1.5, 1.5);
            break;
        case GenerallyAnimationFallOut:
            animationView.alpha = 1 ;
            fallValue = 2.0 ;
            viewAlphaValue = 0.0 ;
            break;
        case GenerallyAnimationPopIn:
            viewAlphaValue = 0.0 ;
            fallValue = 0.1 ;
            oriFrame.origin.x = self.view.center.x ;
            oriFrame.origin.y = self.view.center.y ;
            oriFrame.size = CGSizeMake(100, 100);
            break;
        case GenerallyAnimationPopOut:
            animationView.alpha = 0 ;
            animationView.transform = CGAffineTransformScale(CGAffineTransformMakeRotation(0.0), 0.1, 0.1);
            //            lastFrame = CGRectMake(oriFrame.origin.x + oriFrame.size.width/2 - 10, oriFrame.origin.y + oriFrame.size.height/2 - 10, 20, 20);
            viewAlphaValue = 1.0 ;
            fallValue = 1.0 ;
            break;
        case GenerallyAnimationFallSliderFormLeft:
            animationView.alpha = 0 ;
            lastFrame.size.width = 1 ;
            lastFrame.origin.y -= 10;
            animationView.transform = CGAffineTransformScale(CGAffineTransformMakeRotation(0.0), 1.5, 1.5);
            break;
        case GenerallyAnimationFallSliderFormRight:
            animationView.alpha = 0 ;
            lastFrame.size.width = 1 ;
            lastFrame.origin.x += oriFrame.size.width - 1 ;
            lastFrame.origin.y -= 15;
            animationView.transform = CGAffineTransformScale(CGAffineTransformMakeRotation(0.0), 1.5, 1.5);
            break;
        case GenerallyAnimationFallSliderFormTop:
            animationView.alpha = 0 ;
            lastFrame.size.height = 1 ;
            lastFrame.size.width = 1 ;
            lastFrame.origin.x -= 10 ;
            lastFrame.origin.y -= 10 ;
            animationView.transform = CGAffineTransformScale(CGAffineTransformMakeRotation(0.0), 1.5, 1.5);
            break;
        case GenerallyAnimationFallSliderFormBottom:
            animationView.alpha = 0 ;
            lastFrame.size.width = 1 ;
            lastFrame.size.height = 1 ;
            lastFrame.origin.x -= 10 ;
            lastFrame.origin.y += oriFrame.size.height - 1 ;
            lastFrame.origin.y -= 10 ;
            animationView.transform = CGAffineTransformScale(CGAffineTransformMakeRotation(0.0), 1.5, 1.5);
            break;
        case GenerallyAnimationConverLayerFormLeft:
            converFrame.origin.x -= converFrame.size.width ;
            animationView.alpha = 1 ;
            converView.frame = converFrame ;
            [animationView.layer setMask:converView.layer];
            break;
        case GenerallyAnimationConverLayerFormRight:
            converFrame.origin.x = converFrame.size.width ;
            animationView.alpha = 1 ;
            converView.frame = converFrame ;
            [animationView.layer setMask:converView.layer];
            break;
        case GenerallyAnimationConverLayerFormTop:
            converFrame.origin.y -= converFrame.size.height ;
            animationView.alpha = 1 ;
            converView.frame = converFrame ;
            [animationView.layer setMask:converView.layer];
            break;
        case GenerallyAnimationConverLayerFormBottom:
            converFrame.origin.y = converFrame.size.height ;
            animationView.alpha = 1 ;
            converView.frame = converFrame ;
            [animationView.layer setMask:converView.layer];
            break;
        case GenerallyAnimationConverLayerFormCenter:
            
//            converView.image = SSENCRYPTED_IMAGE(@"CircleLayerImage.png");
            converView.image = [UIImage imageNamed:@"ShowSoftResources/CircleLayerImage.png"];
            converView.backgroundColor = [UIColor clearColor];
            converFrame.origin.x = converFrame.size.width / 2 - 1 ;
            converFrame.origin.y = converFrame.size.height / 2 - 1 ;
            converFrame.size = CGSizeMake(2, 2);
            converView.frame = converFrame ;
            [animationView.layer setMask:converView.layer];
            animationView.alpha = 0 ;
            
            converFrame = converBounds;
            converBounds = CGRectMake(-50, -50, converFrame.size.width+100, converFrame.size.height+100);
            
            viewAlphaValue = 1 ;
            break;
        case GenerallyAnimationFadeIn:
            animationView.alpha = 0 ;
            viewAlphaValue = 1 ;
            break;
        case GenerallyAnimationFadeOut:
            animationView.alpha = 1 ;
            viewAlphaValue = 0 ;
            break;
            
        
        default:
            break;
    }
    switch (animationType) {
        case GenerallyAnimationFallIn:
        case GenerallyAnimationPopOut:
            
            break;
            
        default:
            animationView.frame = lastFrame ;
            break;
    }
    
    [UIView animateWithDuration:animationTime delay:delayTime options:UIViewAnimationOptionCurveLinear animations:^{
        animationView.transform = CGAffineTransformScale(CGAffineTransformMakeRotation(0.0), fallValue, fallValue);
        animationView.frame = oriFrame ;
        animationView.alpha = viewAlphaValue ;
        
        converView.frame = converBounds ;
    }completion:^(BOOL finshed){
        [converView removeFromSuperview];
        
        if( completion ){
            completion();
        }
    }];
}

#pragma mark - 额外动画
- (void)generallyLineBack:(UIView *)animationView duration:(float)animationTime delayTime:(float)delayTime finishedBlock: (void (^)(void))completion{
    
    
    [UIView animateWithDuration:animationTime delay:delayTime options:UIViewAnimationOptionTransitionNone animations:^{
        animationView.transform =CGAffineTransformMakeScale(1, 0.005);
        
    }completion:^(BOOL finshed){
        if( finshed ){
            if( completion ){
                completion();
            }
        }
    }];
    
}
- (void)generallyLineOut:(UIView *)animationView duration:(float)animationTime delayTime:(float)delayTime finishedBlock: (void (^)(void))completion{
    
    animationView.transform =CGAffineTransformMakeScale(1, 0.005);
    
    [UIView animateWithDuration:animationTime delay:delayTime options:UIViewAnimationOptionTransitionNone animations:^{
        animationView.transform =CGAffineTransformMakeScale(1, 1);
        
    }completion:^(BOOL finshed){
        if( finshed ){
            if( completion ){
                completion();
            }
        }
    }];
    
}
- (void)generallyPopIn:(UIView *)animationView duration:(float)animationTime delayTime:(float)delayTime finishedBlock: (void (^)(void))completion{
    
    [UIView animateWithDuration:animationTime delay:delayTime options:UIViewAnimationOptionTransitionNone animations:^{
        animationView.transform =CGAffineTransformMakeScale(.1, .1);
        animationView.alpha = 0.1;
        
    }completion:^(BOOL finshed){
        if( finshed ){
            if( completion ){
                completion();
            }
        }
    }];
    
}

//退场动画
- (void)backTheViewAnimationWithView:(UIView *)animationview duration:(float)animationTime finishedBlock: (void (^)(void))completion{
    
    [UIView animateWithDuration:animationTime delay:0 options:UIViewAnimationOptionCurveLinear animations:^{
        animationview.alpha = 0 ;
        animationview.transform = CGAffineTransformScale(CGAffineTransformMakeRotation(0.0) , 2 , 2);
        
        
    }completion:^(BOOL finished){
        if(finished){
            if( completion ){
                completion();
            }
        }
    }];
}




#pragma mark - 图片处理

//屏幕截图
-(UIImage*)beginImageContext:(CGRect)rect View:(UIView*)view
{
    
    UIGraphicsBeginImageContext(view.frame.size); //currentView 当前的view
    [view.layer renderInContext:UIGraphicsGetCurrentContext()];
    UIImage *viewImage = UIGraphicsGetImageFromCurrentImageContext();
    UIGraphicsEndImageContext();
    
    //从全屏中截取指定的范围
    CGImageRef imageRef = viewImage.CGImage;
    
    CGImageRef imageRefRect =CGImageCreateWithImageInRect(imageRef, rect);
    UIImage *sendImage = [[UIImage alloc] initWithCGImage:imageRefRect];
    
    CGImageRelease(imageRefRect);

    
    return sendImage;
    
    /******截取图片保存的位置，如果想要保存，请把return向后移动*********/
    NSData*data=UIImagePNGRepresentation(viewImage);
    NSString*path=[NSHomeDirectory() stringByAppendingString:@"/documents/1.png"];
    [data writeToFile:path atomically:YES];
    
    NSLog(@"%@",path);
    
    UIImageWriteToSavedPhotosAlbum(sendImage, nil, nil, nil);
    CGImageRelease(imageRefRect);
    /***************/
    
}
//当前屏幕截图 像素不变
- (UIImage *)getScreenImageContext:(CGRect)rect View:(UIView *)view{
    UIGraphicsBeginImageContextWithOptions(view.frame.size, NO, 0.0); //分辨率与屏幕分辨率相同
    [view.layer renderInContext:UIGraphicsGetCurrentContext()];
    UIImage *viewImage = UIGraphicsGetImageFromCurrentImageContext();
    UIGraphicsEndImageContext();
    
    
    return viewImage;
}

//压缩图片
- (UIImage *)imageWithImageSimple:(UIImage*)image scaledToSize:(CGSize)newSize
{
    
    UIGraphicsBeginImageContext(newSize);
    [image drawInRect:CGRectMake(0,0,newSize.width,newSize.height)];
    UIImage* newImage = UIGraphicsGetImageFromCurrentImageContext();
    UIGraphicsEndImageContext();
    return newImage;
}
#pragma mark image scale utility
- (UIImage *)imageByScalingToMaxSize:(UIImage *)sourceImage {
    if (sourceImage.size.width < ORIGINAL_MAX_WIDTH) return sourceImage;
    CGFloat btWidth = 0.0f;
    CGFloat btHeight = 0.0f;
    if (sourceImage.size.width > sourceImage.size.height) {
        btHeight = ORIGINAL_MAX_WIDTH;
        btWidth = sourceImage.size.width * (ORIGINAL_MAX_WIDTH / sourceImage.size.height);
    } else {
        btWidth = ORIGINAL_MAX_WIDTH;
        btHeight = sourceImage.size.height * (ORIGINAL_MAX_WIDTH / sourceImage.size.width);
    }
    CGSize targetSize = CGSizeMake(btWidth, btHeight);
    return [self imageByScalingAndCroppingForSourceImage:sourceImage targetSize:targetSize];
}

- (UIImage *)imageByScalingAndCroppingForSourceImage:(UIImage *)sourceImage targetSize:(CGSize)targetSize {
    UIImage *newImage = nil;
    CGSize imageSize = sourceImage.size;
    CGFloat width = imageSize.width;
    CGFloat height = imageSize.height;
    CGFloat targetWidth = targetSize.width;
    CGFloat targetHeight = targetSize.height;
    CGFloat scaleFactor = 0.0;
    CGFloat scaledWidth = targetWidth;
    CGFloat scaledHeight = targetHeight;
    CGPoint thumbnailPoint = CGPointMake(0.0,0.0);
    if (CGSizeEqualToSize(imageSize, targetSize) == NO)
    {
        CGFloat widthFactor = targetWidth / width;
        CGFloat heightFactor = targetHeight / height;
        
        if (widthFactor > heightFactor)
        scaleFactor = widthFactor; // scale to fit height
        else
        scaleFactor = heightFactor; // scale to fit width
        scaledWidth  = width * scaleFactor;
        scaledHeight = height * scaleFactor;
        
        // center the image
        if (widthFactor > heightFactor)
        {
            thumbnailPoint.y = (targetHeight - scaledHeight) * 0.5;
        }
        else
        if (widthFactor < heightFactor)
        {
            thumbnailPoint.x = (targetWidth - scaledWidth) * 0.5;
        }
    }
    UIGraphicsBeginImageContext(targetSize); // this will crop
    CGRect thumbnailRect = CGRectZero;
    thumbnailRect.origin = thumbnailPoint;
    thumbnailRect.size.width  = scaledWidth;
    thumbnailRect.size.height = scaledHeight;
    
    [sourceImage drawInRect:thumbnailRect];
    
    newImage = UIGraphicsGetImageFromCurrentImageContext();
    if(newImage == nil) NSLog(@"could not scale image");
    
    //pop the context to get back to the default
    UIGraphicsEndImageContext();
    return newImage;
}

#pragma mark - 常用动画


//旋转动画
- (void)startXuanzhuanView:(UIView *)view{
    
    [UIView animateWithDuration:2 delay:0 options:UIViewAnimationOptionCurveEaseInOut animations:^{
        [UIView setAnimationTransition:UIViewAnimationTransitionFlipFromRight forView:view cache:NO];
    } completion:nil];
    
}

//弹跳视图动画
- (void)bounceTheView:(UIView *)bounceView{
    bounceView.transform = CGAffineTransformMakeScale(.1, .1);
    [UIView animateWithDuration:ChangeViewTime animations:^{
        bounceView.transform = CGAffineTransformMakeScale(1.2, 1.2);
    } completion:^(BOOL finished) {
        [UIView animateWithDuration:0.2 animations:^{
            bounceView.transform = CGAffineTransformMakeScale(.8, .8);
        } completion:^(BOOL finished) {
            [UIView animateWithDuration:0.2 animations:^{
                bounceView.transform = CGAffineTransformMakeScale(1, 1);
                
            }];
        }];
    }];
}

// 左右滑动指定的视图

+ (void)shakeAnimationInView : (UIView *)shakeView {
    // Animate the alert to show that the entered string was wrong
    // "Shakes" similar to OS X login screen
    CGAffineTransform moveRight = CGAffineTransformTranslate(CGAffineTransformIdentity, 4, 0);
    CGAffineTransform moveLeft = CGAffineTransformTranslate(CGAffineTransformIdentity, -4, 0);
    CGAffineTransform resetTransform = CGAffineTransformTranslate(CGAffineTransformIdentity, 0, 0);
    
    [UIView animateWithDuration:0.1 animations:^{
        // Translate left
        shakeView.transform = moveLeft;
        
    } completion:^(BOOL finished) {
        
        [UIView animateWithDuration:0.1 animations:^{
            
            // Translate right
            shakeView.transform = moveRight;
            
        } completion:^(BOOL finished) {
            
            [UIView animateWithDuration:0.1 animations:^{
                
                // Translate left
                shakeView.transform = moveLeft;
                
            } completion:^(BOOL finished) {
                
                [UIView animateWithDuration:0.1 animations:^{
                    
                    // Translate to origin
                    shakeView.transform = resetTransform;
                }];
            }];
            
        }];
    }];
    
}

@end
