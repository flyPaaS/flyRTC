//
//  KCTCommonClass.h
//  KCTApiSdk
//
//  Created by wenqinglin on 14-4-2.
//  Copyright (c) 2014年 yzx. All rights reserved.
//

#import <Foundation/Foundation.h>






typedef enum
{
    KCTCallType_VOIP       = 0, // 语音电话
    KCTCallType_VideoPhone = 2, // 视频电话
}KCTCallTypeEnum;


typedef enum
{
    KCTCli_ClientID = 0, // 号码为ClientID
    KCTCli_PHONE    = 1, // 号码为PHONE Number
    
}KCTClientType;


//摄像头位置
typedef enum
{
    CAMERA_REAR =0,    //后置摄像头
    CAMERA_FRONT = 1   //前置摄像头
    
    
}KCTSwitchCameraType;


//视频模式
typedef enum
{
    CAMERA_SEND,             //本地视频
    CAMERA_RECEIVE,          //远程视频
    CAMERA_NORMAL            //正常
}KCTCameraType;

//通话网络状态
typedef enum
{
    KCTNetwork_General  = 0, // 网络状况一般
    KCTNetwork_Nice,         // 网络状况良好
    KCTNetwork_Bad,          // 网络状况很差
    KCTNetwork_Well,         // 网络状态优秀
}KCTNetworkState;




//错误码
@interface KCTReason : NSObject
@property (nonatomic,assign) NSInteger reason;     //错误码
@property (nonatomic,copy) NSString *msg;        //错误原因
@property (nonatomic,copy) NSString *callId;     //消息发送者callID
@end




//视频编码参数设置
@interface KCTVideoEncAttr : NSObject
@property (nonatomic, assign) BOOL isUseCustomEnc;        // 是否使用自定义参数(分辨率、码率、帧率) YES：使用自定义视频参数 NO：使用默认视频参数
@property (nonatomic, assign) NSInteger uHeight;        // 视频编码分辨率：高
@property (nonatomic, assign) NSInteger uWidth;         // 视频编码分辨率：宽
@property (nonatomic, assign) NSInteger uStartBitrate;  // 开始码率
@property (nonatomic, assign) NSInteger uMaxFramerate;     // 帧率

@end


//视频解码参数设置
@interface KCTVideoDecAttr : NSObject
@property (nonatomic, assign) BOOL isUseCustomDec;        // 是否使用自定义参数(分辨率、码率、帧率) YES：使用自定义视频参数 NO：使用默认视频参数
@property (nonatomic, assign) NSInteger uHeight;        // 视频解码分辨率：高
@property (nonatomic, assign) NSInteger uWidth;         // 视频解码分辨率：宽

@end


//摄像头取景分辨率设置
@interface KCTCameraAttr : NSObject
@property (nonatomic,assign) BOOL isUseCustomDec;     //是否使用自定义参数 (分辨率、帧率)   YES:使用自定义摄像头参数 NO:使用默认参数
@property (nonatomic,assign) NSInteger uWidth;        //采集的视频分辨率 : 宽
@property (nonatomic,assign) NSInteger uHeight;       //采集的视频分辨率 : 高
@property (nonatomic,assign) NSInteger uMaxFramerate;     // 帧率

@end



@interface KCTCommonClass : NSObject


@end
