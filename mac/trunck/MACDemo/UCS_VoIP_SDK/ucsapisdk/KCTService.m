//
//  UCSCallService.m
//  yzxapisdk
//
//  Created by wenqinglin on 14-4-2.
//  Copyright (c) 2014年 yzx. All rights reserved.
//

#import "KCTService.h"
#import <Foundation/Foundation.h>
#import <AVFoundation/AVFoundation.h>

#import "KCTEvent.h"
#import "SoftphoneManager.h"
#import "UCSDialViewController.h"
#import "ugo.h"


#import "UCSTCPClientManager.h" //tcp链接处理模块

#import "UCSCrashLogRequestManager.h"
#import "MacVideoView.h"


@interface KCTService()
{
    UCSDialViewController *_ucsdialviewshare;
    float                   m_fSystemVolume;            // 系统音量大小

    int  initSoftPhoneManagerResult;                     //组件初始化结果

    UCSCrashLogRequestManager * _crashManager;
}


@end

@implementation KCTService


static id _ucsbackdelegate =nil; //代理对象，用于回调


#pragma mark - -------------------音视频引擎初始化配置接口-----------------------
-(id)init
{
    self = [super init];
    
    if (self) {
        initSoftPhoneManagerResult = -1;
    }
    
    return self;
}

//初始化呼叫(初始化和注册一起)
- (KCTService *)initWithDelegate:(id<KCTEventDelegate>)delegate
{
    self = [super init];
    
    if (self) {
        
        
    }
    //音量初始化
    m_fSystemVolume= 1.0f;
    
    //ucs回调代理
    _ucsbackdelegate = [delegate retain] ;
    
    //如果组件结果不为0说明组件还没有初始化过，则组件需要进行初始化
    [self initwithSoftphoneManager];


    return self;
}


- (void)setIncomingCallMusicPath:(NSString *)path{
    [[SoftphoneManager instance] setLocalMusicPath:path];
}



-(void)dealloc
{
    
    [_ucsbackdelegate release];
    [_ucsdialviewshare release];
    
    [super dealloc];
}


//设置代理方法
-(void)setDelegate:(id< KCTEventDelegate>)delegate;
{
    [_ucsbackdelegate release];
    _ucsbackdelegate = nil;
    _ucsbackdelegate = [delegate retain];
    
    //如果组件结果不为0说明组件还没有初始化过，则组件需要进行初始化
    if(0!=initSoftPhoneManagerResult)
    {
        [self initwithSoftphoneManager];
    }
    
    //重新设置回调代理
     _ucsdialviewshare.mucsdelegate = _ucsbackdelegate;
 
}

-(void)initwithSoftphoneManager
{
    srand((unsigned int)time(NULL));//初始化随机数种子，确保每次随机数不一样。
    signal(SIGPIPE, SIG_IGN);// 忽略SIGPIPE信号，用于  解决关于SIGPIPE导致的程序退出，
    
    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^(void){
        NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
        
        
        //--------------------------------------VOIP回调代理模块初始化--------------------------------------//
        //组件回调代理
        _ucsdialviewshare = [[UCSDialViewController alloc ]init];
        //初始化操作，为UCSdialviewControl设置ucs回调代理。
        _ucsdialviewshare.mucsdelegate = _ucsbackdelegate;
        
        
        //--------------------------------------组件模块初始化--------------------------------------//
        //初始化组件
        initSoftPhoneManagerResult = [[SoftphoneManager instance] initSoftphoneManager];
        // set the SoftphoneManager delegate
        [[SoftphoneManager instance] setDelegate:[UCSDialViewController share]];  //为组件设置回调对象。
        
        
        //--------------------------------------TCP连接模块初始化--------------------------------------//
        [[UCSTCPClientManager instance] setUCSServiceDelegate:_ucsbackdelegate];
        [UCSTCPClientManager instance].tcpClientDelegate = [KCTTcpClient sharedTcpClientManager];
        if ([[KCTTcpClient sharedTcpClientManager] respondsToSelector:@selector(setVoipClient:)]) {
            [[KCTTcpClient sharedTcpClientManager] performSelector:@selector(setVoipClient:) withObject:[UCSTCPClientManager instance]];
        }
        
        
        /**
         @author WLS, 15-12-17 10:12:32
         
         若p2p开关打开，组件初始化会有延时 (3s)
         */
        if (nil != _ucsbackdelegate && [_ucsbackdelegate respondsToSelector:@selector(onInitEngineSuccessful:)])
        {
            [_ucsbackdelegate performSelectorOnMainThread:@selector(onInitEngineSuccessful:) withObject:0 waitUntilDone:NO];
        }
        
        
        /**
         @author WLS, 16-06-02 12:12:32
         
         捕捉系统异常
         */
        [self catchCrashLog];
        
        /**
         @author WLS, 16-06-02 12:12:32
         
         上传crash日志
         */
        [self upLoadCrashLog];
        
        
        [pool drain];
    });

}

/**
 @author WLS, 16-06-02 12:12:32
 
 捕捉系统异常
 */
- (void)catchCrashLog{
    //add by WLS 20151111 当外部使用其他崩溃日志截取的方法的时候， 内部崩溃日志截取则需要关闭
    NSUserDefaults * myDefaults = [NSUserDefaults standardUserDefaults];
    BOOL openCrashLog = [[myDefaults valueForKey:@"UCSOpenCrashLog"] boolValue];
    if (openCrashLog) {
        //add by WLS 20151111 当外部使用其他崩溃日志截取的方法的时候， 内部崩溃日志截取则需要关闭
        [[SDKLogManager instance]setSDKDefaultHandler]; //监听sdk异常 add by baige 2015年01月29日
    }
}

/**
 @author WLS, 16-06-02 12:12:32
 
 上传crash日志
 */
- (void)upLoadCrashLog{
    
    // 检查是否有崩溃日志，有则上传    CW 2016.1.13
    NSString * logName = [[NSUserDefaults standardUserDefaults] valueForKey:@"UCSLogName"];
    if (logName.length > 0) {
        NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask,YES);
        NSString * documentsDirectory  =[paths objectAtIndex:0];
        NSString * filePath = [documentsDirectory stringByAppendingPathComponent:logName];
        _crashManager = [[UCSCrashLogRequestManager alloc] init];
        _crashManager.fileName = filePath;
        _crashManager.select = @selector(uploadEnd);
        _crashManager.delegate = self;
        [_crashManager uploadCrashRequest];
    }
    
}

// 上传结束 释放内存 CW 2016.1.13
- (void)uploadEnd{
    
    [_crashManager stopCrashRequest];
    [_crashManager release];
    _crashManager = nil;
    
}

/**
 * 获取SDK版本信息
 */
- (NSString*) getKCTSDKVersion
{
    
    NSString* versionNum =uVersion;
    
    return [NSString stringWithFormat:@"%@_%@_%@",versionNum,[KCTTcpClient sharedTcpClientManager].version,[[SoftphoneManager instance]getSoftphoneVersion]];
    
}

/**
 * 设置IPv6网络环境
 */
- (void)setIpv6:(BOOL)isIpv6{
    
    [[SoftphoneManager instance] setIpv6Enable:isIpv6];
    [SoftphoneManager instance].isIpv6 = isIpv6;
}

- (void)setVideoCodecWithIndex:(NSInteger)videoIndex audioCodecIndex:(NSInteger)audioIndex
{
    [[SoftphoneManager instance] setVideoCodecWithIndex:videoIndex audioCodecIndex:audioIndex];
}

/**
 * 获取系统流量统计
 */
-(NSDictionary*) getNetWorkFlow
{
    
    return [UCSPublicFunc checkNetworkflow];
}


#pragma mark - -------------------互联网音频通话能力接口-----------------------
- (void)dial:(NSInteger)callType andCalled:(NSString *)calledNumber andUserdata:(NSString *)callerData
{
    
    m_fSystemVolume = [[UXinSoundManager sharedInstance] getSystemVolume];
    if (m_fSystemVolume < 0.1f) {
        [[UXinSoundManager sharedInstance] setSystemVolume:0.7f];
    }
    
    SoftphoneManager *mgr = [SoftphoneManager instance];
    
    
    if (calledNumber == nil || ([calledNumber isEqualToString:@""] && callType == 1)) {
        /**
         @author WLS, 16-05-24 09:05:21
         
         直拨号码不能为空
         */
        [[UCSDialViewController share]OnCallFailed:@"402008"  andMsg:@"被叫号码异常" withCallId:@""];
        return;
    }
    //add by kucky 20150208
    PhoneNetType pNetType = [PublicFunc GetCurrentPhoneNetType];
    if (pNetType == PNT_2G ) {
        
        [[UCSDialViewController share]OnCallFailed:@"402049"  andMsg:@"呼叫失败(网络状态差)" withCallId:@""];
        [[SDKLogManager instance]saveSDKLogInfo:@"呼叫失败(网络状态差)" withDetail:calledNumber];
        return;
        
    }else if (pNetType == PNT_UNKNOWN ) {
        
        
        [[UCSDialViewController share]OnCallFailed:@"402049"  andMsg:@"呼叫失败（当前网络不支持）" withCallId:@""];
        [[SDKLogManager instance]saveSDKLogInfo:@"呼叫失败（当前网络不支持）" withDetail:calledNumber];
        
        return;
        
    }
    
    
    //拨打电话的时候重置通话参数 add by WLS 20151009
    [[UCSDialViewController share]resetParameter];
    
    
    
    NSString * call = calledNumber;
    /**
     @author WLS, 16-01-11 09:01:28
     
     判断是否需要录音功能
     */
    if ([calledNumber hasPrefix:@"*#*"]) {
        [UCSDialViewController share].needRecord = YES;
    }else if([UserDefaultManager GetLocalDataBoolen:DATA_STORE_ISRECORD]){
        [UCSDialViewController share].needRecord = YES;
        call = [NSString stringWithFormat:@"*#*%@",calledNumber];
    }else{
        [UCSDialViewController share].needRecord = NO;

    }
    //UXLogAlert(@"need record : %d---%d",[UCSDialViewController share].needRecord,call);
    
    if (callType == 0 ) { //免费电话
        //        [mgr callPhoneNumber:@"" uid:calledNumber];
        [mgr callPhoneNumber:call withPN:@"" withCallMode:eUGo_CM_FREE andUserdata:callerData];
        [[SDKLogManager instance]saveSDKLogInfo:@"拨打免费电话" withDetail:call];
        
        
        [[UCSDialViewController share]apendVoiceFormatWithCallId:nil incomingCall:@"拨打语音通话"];
        
    } else if (callType == 1) {//直拨
        
        //        [mgr callPhoneNumber:calledNumber uid:@""];
        [mgr callPhoneNumber:@"" withPN:call withCallMode:eUGo_CM_DIRECT andUserdata:callerData];
        [[SDKLogManager instance]saveSDKLogInfo:@"拨打直拨电话" withDetail:call];
        
    }  else if (callType == 2) { //视频电话
        
        
        //拨打电话的时候设置视频通话 add by WLS 20150907
        [UserDefaultManager SetLocalDataBoolen:YES key:DATA_STORE_ISVIDEO];
        
        [mgr callPhoneNumberForVideo:call withPN:@"" withCallMode:eUGo_CM_FREE andUserdata:callerData];
        [[SDKLogManager instance]saveSDKLogInfo:@"拨打视频电话" withDetail:call];
        
        
        [[UCSDialViewController share]apendVoiceFormatWithCallId:nil incomingCall:@"拨打视频通话"];

    } else if (callType == 4) {//智能呼叫，必须传手机号
        
        [mgr callPhoneNumber:@"" withPN:call withCallMode:eUGo_CM_AUTO andUserdata:callerData];
        [[SDKLogManager instance]saveSDKLogInfo:@"拨打智能电话" withDetail:call];
        
    }else {
        return;
    }
    
}

//挂断
- (void) hangUp: (NSString*)callid{
    
    [_ucsdialviewshare hangUpCall];
    [[SDKLogManager instance]saveSDKLogInfo:@"挂断hangUp" withDetail:callid];
    
}

//被叫接听
- (void) answer: (NSString*)callid{
    
    [_ucsdialviewshare onAnswerCall];
    [[SDKLogManager instance]saveSDKLogInfo:@"被叫接听" withDetail:callid];
}

//被叫拒绝接听
- (void)reject: (NSString*)callid{
    
    [_ucsdialviewshare onHangupCall];
    [[SDKLogManager instance]saveSDKLogInfo:@"被叫拒绝接听" withDetail:callid];
}

//发送DTMF
- (BOOL)sendDTMF: (char)dtmf
{
    return [[SoftphoneManager instance] sendDTMF:dtmf];
}

//免提设置
- (void) setSpeakerphone:(BOOL)enable{
    [_ucsdialviewshare loudSpeakerChanged:enable];
}

//免提状态
- (BOOL) isSpeakerphoneOn{
    return [_ucsdialviewshare getLoudSpeakerOpenStatus];
}

// 静音设置
- (void)setMicMute:(BOOL)on{
    
    if (on == YES) {
        //开启静音状态 如果当前处于会话状态 则保存此次操作 add by WLS 20150907
        if ([UserDefaultManager GetLocalDataBoolen:DATA_STORE_ISTALK]) {
            [UserDefaultManager SetLocalDataBoolen:YES key:DATA_STORE_ISCMUTE];
        }
    }
    
    [_ucsdialviewshare setMicMute:on];
    
}

//获取当前静音状态
- (BOOL)isMicMute{
    
    return [_ucsdialviewshare isMicMute];
}

/**
 * 离线来电推送
 */
- (void) callIncomingPushRsp:(NSString*)callid  withVps:(NSInteger )vpsid withReason:(KCTReason*)reason{
    
    [[SoftphoneManager instance] incomingRsp:callid vpsid:vpsid withReason:reason];
    
}

- (void)incomingRspWhenBackground:(id)callID vpsid:(int)vpsid {
    [[SoftphoneManager instance] incomingRspWhenBackground:callID vpsid:vpsid];
}

//回拨
- (void) OnCallBackBegin:(NSString *)phoneNumber {
    
    [[UCSDialViewController share]OnCallBackBegin:phoneNumber showFromNum:nil showToNum:nil];
    
}

//回拨
- (void) OnCallBackBegin:(NSString *)phoneNumber showFromNum:(NSString*)fromNum showToNum:(NSString*)toNum {
    
    [[UCSDialViewController share]OnCallBackBegin:phoneNumber showFromNum:fromNum showToNum:toNum];
    [[SDKLogManager instance]saveSDKLogInfo:@"拨打回拨电话" withDetail:[NSString stringWithFormat:@"fromNum --> %@  toNum --> %@",fromNum,toNum]];
}

//取消回拨
- (void) CancelOnCallBackBegin:(NSString *)phoneNumber{
    
    [[UCSDialViewController share]CancelOnCallBackBegin:phoneNumber];
    [[SDKLogManager instance]saveSDKLogInfo:@"取消回拨电话" withDetail:[NSString stringWithFormat:@"phoneNumber --> %@",phoneNumber]];
    
}

//回拨呼叫
- (void)callBack:(NSString *)phoneNumber
{
    //    [_ucsdialviewshare OnCallBackBegin];
    [self OnCallBackBegin:phoneNumber];
}

//回拨呼叫（显号）
- (void)callBack:(NSString *)phoneNumber showFromNum:(NSString*)fromNum showToNum:(NSString*)toNum
{
    //    [_ucsdialviewshare OnCallBackBegin];
    [self OnCallBackBegin:phoneNumber showFromNum:fromNum showToNum:toNum];
    [[SDKLogManager instance]saveSDKLogInfo:@"拨打回拨电话" withDetail:[NSString stringWithFormat:@"fromNum --> %@  toNum --> %@",fromNum,toNum]];
}

//取消回拨呼叫
- (void)cancelCallBack:(NSString *)phoneNumber
{
    //    [_ucsdialviewshare OnCallBackBegin];
    [self CancelOnCallBackBegin:phoneNumber];
    [[SDKLogManager instance]saveSDKLogInfo:@"取消回拨电话" withDetail:[NSString stringWithFormat:@"phoneNumber --> %@",phoneNumber]];
}


#pragma mark - -------------------互联网视频能力------------------------
/**
 * 初始化视频显示控件（本地视频显示控件和对方视频显示控件）
 * 参数 frame 窗口大小
 * @return UIView 视频显示控件:
 */
- (NSOpenGLView *)allocCameraViewWithFrame:(CGRect)frame{
    
    NSOpenGLView * videoView = [[MacVideoView allocAndInitWithFrame:frame]retain];
    
    return [videoView autorelease];
}

//设置视频参数
-(BOOL)initCameraConfig:(NSOpenGLView*)localVideoView withRemoteVideoView:(NSOpenGLView*)remoteView{
    
    return [ [SoftphoneManager instance] initCameraConfig:localVideoView withRemoteView:remoteView];
}

/**
 * 设置视频显示参数
 * 获取摄像头个数
 */
- (int) getCameraNum{
    
    return  [[SoftphoneManager instance] getCameraNum];
}

/**
 * 摄像头切换
 * CameraIndex 摄像头位置
 * return YES 成功 NO 失败
 */
- (BOOL) switchCameraDevice:(int)CameraIndex{
    
    int temp = [[SoftphoneManager instance]switchCameraDevice:CameraIndex];
    
    if (temp==0) {
        return YES;
    }
    
    return NO;
}

/**
 *  切换视频模式：发送、接收、正常模式
 *
 *  @param type         CAMERA_RECEIVE : 只接收视频数据（只能接收到对方的视频）
                        CAMERA_SEND : 只发送视频数据（只让对方看到我的视频）
                        CAMERA_NORMAL : send receive preview
 *
 *  @return YES 成功 NO 失败
 */
- (BOOL) switchVideoMode:(KCTCameraType)type{
    
    int temp = [[SoftphoneManager instance] switchVideoMode:type];
    
    if (temp==0) {
        
        //当前处于会话状态 且 处于视频通话 保存设置的本端视频模式参数 add by WLS 20151026
        if ([UserDefaultManager GetLocalDataBoolen:DATA_STORE_ISVIDEO] && [UserDefaultManager GetLocalDataBoolen:DATA_STORE_ISTALK]) {
            [UserDefaultManager SetLocalDataInt:type key:DATA_STORE_LOCALCAMERAMODE];
        }
        
        return YES;
    }
    
    return NO;
}

/**
 *  自定义视频编码和解码参数 add by pbf 2015.09.14
 *
 *  @param ucsVideoEncAttr 编码参数
 *  @param ucsVideoDecAttr 解码参数
 */
- (void)setVideoAttr:(KCTVideoEncAttr*)ucsVideoEncAttr andVideoDecAttr:(KCTVideoDecAttr*)ucsVideoDecAttr{
    
    [[SoftphoneManager instance] setVideoAttr:ucsVideoEncAttr andVideoDecAttr:ucsVideoDecAttr];
}

//视频来电时是否支持预览 1视频来电时开启预览。2 为视频来电时开启预览
- (BOOL)setCameraPreViewStatu:(BOOL)isPreView;
{
    
    if (isPreView)
    {
        [UserDefaultManager setVideoPreView:2];
        
    }else
    {
        
        [UserDefaultManager setVideoPreView:1];
        
    }
    
    return YES;
    
}

/**
 *  获取视频来电时是否支持预览   add by pbf 2015.09.10
 *
 *  @return YES 支持预览 NO 不支持预览
 */
- (BOOL)isCameraPreviewStatu{
    
    int ret = [UserDefaultManager getVideoPreView];
    if (ret == 0) {
        return NO;
    } else if (ret == 1) {
        return NO;
    } else if (ret == 2) {
        return YES;
    }
    
    return YES;
}

/**
 * 旋转显示图像角度
 *
 * 参数 sendRotation      本地显示图像角度  数值为0 90 180 270
 * 参数 reciviedRotation  对端显示图像角度  数值为0 90 180 270
 * @return NO:  YES:
 */
- (BOOL)setRotationVideo:(unsigned int)sendRotation withReciviedRotation:(unsigned int)reciviedRotation{
    
    return [[SoftphoneManager instance] setCameraRotateAngleWithSend:sendRotation andRecivied:reciviedRotation];
    
}

//视频截图
- (void)cameraCapture:(int)islocal withFileName:(NSString*)filename withSavePath:(NSString*)savePath{
    
    int temp = [[SoftphoneManager instance]screenshotForVideo:islocal withFileName:filename withSavePath:savePath];
    
}


#pragma mark - -------------------通用能力------------------------

//正式环境下日志开关
-(void)openSdkLog:(BOOL)isOpenSdkLog
{
    [UserDefaultManager SetLocalDataBoolen:isOpenSdkLog key:FORMAL_LOG_ISOPEN];//保存主机地址

}

/**
 @author WLS, 16-02-01 15:02:05
 
 设置媒体引擎日志的开关
 */
-(BOOL)openMediaEngineLog:(BOOL)isOpenMediaEngineLog{
    
   return [[SoftphoneManager instance] setMediaEngineLogStatus:isOpenMediaEngineLog];
    
}

#pragma mark - -------------------智能硬件能力------------------------
/**
 @author WLS, 16-05-18 11:05:40
 
 是否是收费版本
 
 @return YES代表是收费版本 NO不是
 */
- (BOOL)isLicenseVersion{
    
    if ([uVersion rangeOfString:@"L"].location == NSNotFound) {
        return NO;
    }
    return YES;
    
}

/**
 * 设置rtp包加密控制开关，默认不支持加密
 * enable: YES 支持加密 NO 不支持加密
 * return YES 成功 NO 失败
 */
- (BOOL)setRTPEncEnable:(BOOL)enable{
    
    
    if (![self isLicenseVersion]) {
        /**
         @author WLS, 16-05-17 14:05:41
         
         不是收费版本的时候，rtp流的设置不允许打开
         */
        return NO;
    }
    
    return [[SoftphoneManager instance] setRtpEncEnable:enable];
    
}

/**
 * 设置AGC音效增益
 * plus: 增益系数。
 * @return YES 成功 NO 失败
 */
- (BOOL)setAGCPlus:(int)compressionGain targetDbfs:(int)targetDbfs{
    
    if (![self isLicenseVersion]) {
        /**
         @author WLS, 16-05-17 14:05:41
         
         不是收费版本的时候，AGC音效增益不允许打开
         */
        return NO;
    }
    
    return [[SoftphoneManager instance]setAgcCompressionGaindB:compressionGain targetDbfs:targetDbfs];
    
}



@end
