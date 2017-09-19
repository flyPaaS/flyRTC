//
//  AppDelegate.m
//  SoftphoneDemo
//
//  Created by Liam Peng on 12-6-12.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#if TARGET_OS_IPHONE
#import "VideoView.h"
#include "webrtc/modules/video_render/ios/video_render_ios_view.h"


@implementation VideoView



+ (UIView *) allocAndInitWithFrame:(CGRect)frame
{
    VideoRenderIosView *v = nil;
    v= [[VideoRenderIosView alloc]initWithFrame:frame];
    return v;
}
+ (void)enableRender:(BOOL)enable
{
    [VideoRenderIosView enableRender:enable];
}


@end
							
#endif









