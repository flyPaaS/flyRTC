//
//  KCTCommonClass.m
//  yzxapisdk
//
//  Created by wenqinglin on 14-4-2.
//  Copyright (c) 2014年 yzx. All rights reserved.
//

#import "KCTCommonClass.h"
#include <ifaddrs.h>
#include <sys/socket.h>
#include <net/if.h>


@implementation KCTCommonClass


@end





//视频编码参数设置
@implementation KCTVideoEncAttr
@synthesize isUseCustomEnc = _isUseCustomEnc;
@synthesize uHeight = _uHeight;                 // 视频编码分辨率：高
@synthesize uWidth = _uWidth;                 // 视频编码分辨率：宽
@synthesize uStartBitrate = _uStartBitrate;  // 开始码率
@synthesize uMaxFramerate = _uMaxFramerate;     // 最大帧率

-(void)dealloc
{
    
    [super dealloc];
}


@end




//视频解码参数设置
@implementation KCTVideoDecAttr
@synthesize isUseCustomDec = _isUseCustomDec;
@synthesize uHeight = _uHeight;                 // 视频解码分辨率：高
@synthesize uWidth = _uWidth;                   // 视频解码分辨率：宽

-(void)dealloc
{
    
    [super dealloc];
}

@end



@interface KCTCameraAttr()

@property (nonatomic,assign) NSInteger uRotateAngle;  //视频图像旋转的角度 (暂时用不到)
@property (nonatomic,assign) NSInteger uCameraIdx;    //摄像头位置 : 1代表前置摄像头   0代表后置摄像头  -1代表无效 (暂时用不到)

@end
//摄像头取景分辨率设置
@implementation KCTCameraAttr



@synthesize isUseCustomDec = _isUseCustomDec;
@synthesize uWidth = _uWidth;     //视频采样分辨率 : 宽
@synthesize uHeight = _uHeight;   //视频采样分辨率 : 高
@synthesize uRotateAngle = _uRotateAngle;  //图像旋转的角度
@synthesize uCameraIdx = _uCameraIdx;      //摄像头索引
@synthesize uMaxFramerate = _uMaxFramerate;     // 最大帧率


- (void)dealloc{
    
    [super dealloc];
    
}


@end
