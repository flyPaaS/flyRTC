//
//  mytoast.m
//  TestCustomView
//
//  Created by apple on 12-6-14.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#import "mytoast.h"
#import <QuartzCore/QuartzCore.h>

@implementation mytoast

- (id)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        // Initialization code
    }
    return self;
}

/*
// Only override drawRect: if you perform custom drawing.
// An empty implementation adversely affects performance during animation.
- (void)drawRect:(CGRect)rect
{
    // Drawing code
}
*/

- (void)__show {
    [UIView beginAnimations:@"show" context:nil];
    [UIView setAnimationDelegate:self];
    [UIView setAnimationDuration:0.2f];
    [UIView setAnimationDidStopSelector:@selector(__animationDidStop:__finished:__context:)];
    self.alpha = 1.0f;
    [UIView commitAnimations];
}
- (void)__hide {
    [self performSelectorOnMainThread:@selector(__hideThread) withObject:nil waitUntilDone:NO];
}
- (void)__hideThread {
    [UIView beginAnimations:@"hide" context:nil];
    [UIView setAnimationDelegate:self];
    [UIView setAnimationDuration:0.8f];
    [UIView setAnimationDidStopSelector:@selector(__animationDidStop:__finished:__context:)];
    self.alpha = 0.0f;
    [UIView commitAnimations];
}
- (void)__animationDidStop:(NSString *)animationID __finished:(NSNumber *)finished __context:(void *)context {
    if ([animationID isEqualToString:@"hide"]) {
        [self removeFromSuperview];
        self = nil;
    }
    else if ([animationID isEqualToString:@"show"]) {
        [NSTimer scheduledTimerWithTimeInterval:1 target:self selector:@selector(__hide) userInfo:nil repeats:NO];
    }
}
+ (mytoast *)__createWithText:(NSString *)text {
    float screenWidth = [UIScreen mainScreen].bounds.size.width;
    float screenHeight = [UIScreen mainScreen].bounds.size.height;
    float x = 10.0f;
    float width = screenWidth - x * 2.0f;
    
    UILabel *textLabel = [[UILabel alloc] init];
    textLabel.backgroundColor = [UIColor clearColor];
    textLabel.textAlignment = NSTextAlignmentCenter;
    textLabel.font = [UIFont systemFontOfSize:14];
    textLabel.textColor = RGB(255, 255, 255);
    textLabel.numberOfLines = 0;
    textLabel.lineBreakMode = UILineBreakModeWordWrap;
    CGSize sz = [text sizeWithFont:textLabel.font
                 constrainedToSize:CGSizeMake(width - 20.0f, 9999.0f)
                     lineBreakMode:textLabel.lineBreakMode];
    
    CGRect tmpRect;
    tmpRect.size.width = width;
    tmpRect.size.height = MAX(sz.height + 20.0f, 38.0f);
    tmpRect.origin.x = floor((screenWidth - width) / 2.0f);
    tmpRect.origin.y = floor(screenHeight - tmpRect.size.height - 15.0f);
    
    //by jiyonglong begin
    
    tmpRect.origin.y -= 50.0f;
    
    //by jiyonglong end
    
    mytoast *toast = [[mytoast alloc] initWithFrame:tmpRect] ;
    toast.backgroundColor = RGBA(0, 0, 0, 0.8f);
    CALayer *layer = toast.layer;
    layer.masksToBounds = YES;
    layer.cornerRadius = 10.0f;
    
    textLabel.text = text;
    tmpRect.origin.x = floor((toast.frame.size.width - sz.width) / 2.0f);
    tmpRect.origin.y = floor((toast.frame.size.height - sz.height) / 2.0f);
    
    tmpRect.size = sz;
    textLabel.frame = tmpRect;
    [toast addSubview:textLabel];
    [textLabel release];
    toast.alpha = 0.0f;
    
    return [toast autorelease];
}


+ (mytoast *)__createWithImage:(UIImage *)image {
    float screenWidth = [UIScreen mainScreen].bounds.size.width;
    float screenHeight = [UIScreen mainScreen].bounds.size.height;
    float x = 10.0f;
    float width = screenWidth - x * 2.0f;
    
    UIImageView *imageView = [[UIImageView alloc] initWithImage:image];
    CGSize sz = imageView.frame.size;
    
    CGRect tmpRect;
    tmpRect.size.width = width;
    tmpRect.size.height = MAX(sz.height + 20.0f, 38.0f);
    tmpRect.origin.x = floor((screenWidth - width) / 2.0f);
    tmpRect.origin.y = floor(screenHeight - tmpRect.size.height - 15.0f);
    
    mytoast *toast = [[mytoast alloc] initWithFrame:tmpRect];
    toast.backgroundColor = RGBA(0, 0, 0, 0.8f);
    CALayer *layer = toast.layer;
    layer.masksToBounds = YES;
    layer.cornerRadius = 5.0f;
    
    tmpRect.origin.x = floor((toast.frame.size.width - sz.width) / 2.0f);
    tmpRect.origin.y = floor((toast.frame.size.height - sz.height) / 2.0f);
    tmpRect.size = sz;
    imageView.frame = tmpRect;
    [toast addSubview:imageView];
    [imageView release];
    toast.alpha = 0.0f;
    return [toast autorelease];
}
/**
 * Show toast with text in application window
 * @param text Text to print in toast window
 */
+ (void)showWithText:(NSString *)text {
    mytoast *toast = [mytoast __createWithText:text];
    
    UIWindow *mainWindow = [[UIApplication sharedApplication] keyWindow];
    [mainWindow addSubview:toast];
   // [toast release];
    
    [toast __show];
}


/**
 * Show toast with image in application window
 * @param image Image to show in toast window
 */
+ (void)showWithImage:(UIImage *)image {
    mytoast *toast = [mytoast __createWithImage:image];
    UIWindow *mainWindow = [[UIApplication sharedApplication] keyWindow];
    [mainWindow addSubview:toast];
    //[toast release];
    [toast __show];
}

@end
