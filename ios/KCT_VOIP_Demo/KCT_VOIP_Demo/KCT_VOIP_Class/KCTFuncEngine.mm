
#import "KCTFuncEngine.h"
#import "KCTService.h"
#import "KCTTcpClient.h"
#import <AVFoundation/AVFoundation.h>
#import <objc/message.h>
#import "AppDelegate.h"
#import "KCTVOIPViewEngine.h"


NSString * const KCTSNotiHeadPhone = @"KCTSNotiHeadPhone";
NSString * const KCTNotiCallBalance = @"KCTNotiCallBalance";
NSString * const KCTNotiRefreshCallList = @"KCTNotiRefreshCallList";
NSString * const KCTNotiIncomingCall = @"KCTNotiIncomingCall";
NSString * const KCTNotiEngineSucess = @"KCTNotiEngineSucess";
NSString * const KCTNotiTCPTransParent = @"KCTNotiTCPTransParent";

@class VideoView;
@interface KCTFuncEngine ()
{
}

@end

@implementation KCTFuncEngine

@synthesize kctCallService = _kctCallService;
@synthesize userId = _userId;
@synthesize UIDelegate = _UIDelegate;


KCTFuncEngine* gKCTFuncEngine = nil;


//触发的监听事件 add by WLS 20151106
void audioRouteChangeListenerCallback (
                                       void *inUserData,
                                       AudioSessionPropertyID inID,
                                       UInt32 inDataSize,
                                       const void *inData)
{
    
    
    
    UInt32 propertySize = sizeof(CFStringRef);
    AudioSessionInitialize(NULL, NULL, NULL, NULL);
    CFStringRef state = nil;
    AudioSessionGetProperty(kAudioSessionProperty_AudioRoute
                            ,&propertySize,&state);
    //    NSLog(@"%@",state);//return @"Headphone" or @"Speaker" and so on.
    if((state == NULL) || (CFStringGetLength(state) == 0)){
        // Silent Mode
        //        NSLog(@"AudioRoute: SILENT, do nothing!");
        [[KCTFuncEngine getInstance] setIsHeadPhone:NO];
        
    } else {
        NSString* routeStr = (__bridge NSString*)state;
        //        NSLog(@"AudioRoute: %@", routeStr);
        /* Known values of route:
         * "Headset"
         * "Headphone"
         * "Speaker"
         * "SpeakerAndMicrophone"
         * "HeadphonesAndMicrophone"
         * "HeadsetInOut"
         * "ReceiverAndMicrophone"
         * "Lineout"
         */
        NSRange headphoneRange = [routeStr rangeOfString : @"Headphone"];
        NSRange headsetRange = [routeStr rangeOfString : @"Headset"];
        if (headphoneRange.location != NSNotFound || headsetRange.location != NSNotFound) {
            
            
            NSLog(@"有耳机");
            if ([[KCTFuncEngine getInstance] isHeadPhone]==NO) {
                [[KCTFuncEngine getInstance] setIsHeadPhone:YES];
                
                [[NSNotificationCenter defaultCenter] postNotificationName:KCTSNotiHeadPhone object:nil];
            }
            
            
        } else {
            
            NSLog(@"没有耳机");
            
            [[KCTFuncEngine getInstance] setIsHeadPhone:NO];
            
        }
    }
    
    
}




+(KCTFuncEngine *)getInstance
{
    @synchronized(self){
        if(gKCTFuncEngine == nil){
            gKCTFuncEngine = [[self alloc] init];
        }
    }
	return gKCTFuncEngine;
}


- (id)init
{
    if (self = [super init])
    {

        self.UIDelegate = [KCTVOIPViewEngine getInstance];
        
        
        
        KCTService* kctService = [[KCTService alloc] init];
        [kctService setDelegate:self];
        self.kctCallService = kctService;
        

        [NSTimer scheduledTimerWithTimeInterval:2 target:self selector:@selector(addNotiAudioRouteChange) userInfo:nil repeats:NO];
        
    }
    
    return self;
}

- (void)setSocketIpv6:(BOOL)isIpv6
{
    [self.kctCallService setIpv6:isIpv6];
}

- (void)addNotiAudioRouteChange{
    AudioSessionAddPropertyListener(kAudioSessionProperty_AudioRouteChange,audioRouteChangeListenerCallback, (__bridge void *)(self));

}

- (void)dealloc
{
    self.UIDelegate = nil;
    self.kctCallService = nil;
    self.userId = nil;
}


//-----------------------------------------------功能函数放置区域------------------------------------------------


#pragma mark - 呼叫控制函数
/**
 * 拨打电话
 * @param callType 电话类型
 * @param called 电话号(加国际码)或者VoIP号码
 * @return
 */
- (void)dial:(NSInteger)callType andCalled:(NSString *)calledNumber andUserdata:(NSString *)callerData
{
    [self.kctCallService dial:callType andCalled:calledNumber andUserdata:callerData];
}



/**
 * 挂断电话
 * @param callid 电话id
 * @param reason 预留参数
 */
- (void) hangUp: (NSString*)called
{
    [[KCTVOIPViewEngine getInstance] WriteToSandBox:[NSString stringWithFormat:@"挂断：%@",self.kctCallService]];

    [self.kctCallService hangUp:called];
}

/**
 * 接听电话
 * @param callid 电话id
 * V2.0
 */
- (void) answer: (NSString*)callId
{
    [self.kctCallService answer:callId];
}


/**
 * 拒绝呼叫(挂断一样,当被呼叫的时候被呼叫方的挂断状态)
 * @param callid 电话id
 * @param reason 拒绝呼叫的原因, 可以传入ReasonDeclined:用户拒绝 ReasonBusy:用户忙
 */
- (void)reject: (NSString*)called
{
     [self.kctCallService reject:called];
}


#pragma mark - DTMF函数
/**
 * 发送DTMF
 * @param callid 电话id
 * @param dtmf 键值
 */
- (BOOL)sendDTMF: (char)dtmf
{
    return [self.kctCallService sendDTMF:dtmf];
}

#pragma mark - 本地功能函数

/**
 * 免提设置
 * @param enable false:关闭 true:打开
 */
- (void) setSpeakerphone:(BOOL)enable
{
    [self.kctCallService setSpeakerphone:enable];
}


/**
 * 获取当前免提状态
 * @return false:关闭 true:打开
 */
- (BOOL) isSpeakerphoneOn
{
    return  [self.kctCallService isSpeakerphoneOn];
}


/**
 * 静音设置
 * @param on false:正常 true:静音
 */
- (void)setMicMute:(BOOL)on
{
    [self.kctCallService setMicMute:on];
}

/**
 * 获取当前静音状态
 * @return false:正常 true:静音
 */
- (BOOL)isMicMute
{
  return [self.kctCallService isMicMute];
}




/**
 * 获取SDK版本信息
 *
 */
- (NSString*) getKCTSDKVersion
{
    return [self.kctCallService getKCTSDKVersion];
}

/**
 * 获取流量分类统计信息
 *
 */
-(NSDictionary*) getNetWorkFlow
{
    return [self.kctCallService getNetWorkFlow];
}



#pragma mark - 视频能力
//-----------------------------------------视频能力分割线-------------------------------------------------------//

/**
 *  初始化视频显示控件（本地视频显示控件和对方视频显示控件）
 *
 *参数 frame 窗口大小
 *
 *@return UIView 视频显示控件:
 */
- (UIView *)allocCameraViewWithFrame:(CGRect)frame{
    
    return [self.kctCallService allocCameraViewWithFrame:frame];
    
}

/**
 * 设置视频显示参数
 *
 *参数 localVideoView 设置本地视频显示控件
 *参数 remoteView     设置对方视频显示控件
 *参数 width          设置发给对方视频的宽度
 *参数 height         设置发给对方视频的高度
 *
 *@return NO:  YES:
 */

- (BOOL)initCameraConfig:(UIView*)localVideoView withRemoteVideoView:(UIView*)remoteView
{
    return [self.kctCallService initCameraConfig:localVideoView withRemoteVideoView:remoteView];
    
}

/**
 *  自定义视频编码和解码参数
 *
 *  @param kctVideoEncAttr 编码参数
 *  @param kctVideoDecAttr 解码参数
 */
- (void)setVideoAttr:(KCTVideoEncAttr*)kctVideoEncAttr andVideoDecAttr:(KCTVideoDecAttr*)kctVideoDecAttr{
    
    [self.kctCallService setVideoAttr:kctVideoEncAttr andVideoDecAttr:kctVideoDecAttr];
    
}


/**
 * 旋转显示图像角度
 *
 * 参数 sendRotation      本地显示图像角度  数值为0 90 180 270
 * 参数 reciviedRotation  对端显示图像角度  数值为0 90 180 270
 *@return NO:  YES:
 */
- (BOOL)setRotationVideo:(unsigned int)sendRotation withReciviedRotation:(unsigned int)reciviedRotation{
    return [self.kctCallService setRotationVideo:sendRotation withReciviedRotation:reciviedRotation];
}


/**
 *
 * 获取摄像头个数
 */
- (int)getCameraNum
{
    return [self.kctCallService getCameraNum];
}


/**
 * 摄像头切换 后置摄像头：0 前置摄像头：1
 *return YES 成功 NO 失败
 */
- (BOOL)switchCameraDevice:(int)CameraIndex
{
    return [self.kctCallService switchCameraDevice:CameraIndex];
    
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
- (BOOL)switchVideoMode:(KCTCameraType)type{
    
    return [self.kctCallService switchVideoMode:type];
    
}


/**
 * 视频截图
 * islocal: 0 是远端截图 1 是本地截图。
 * return YES 成功 NO 失败
 */
- (void)cameraCapture:(int)islocal withFileName:(NSString*)filename withSavePath:(NSString*)savePath;
{
    [self.kctCallService cameraCapture:islocal withFileName:filename withSavePath:savePath];
    
}

/**
 * 视频来电时是否支持预览
 * isPreView: YES 支持预览 NO 不支持预览。
 * return YES 成功 NO 失败
 */
- (BOOL)setCameraPreViewStatu:(BOOL)isPreView
{
    return [self.kctCallService setCameraPreViewStatu:isPreView];
    
}


/**
 *  获取视频来电时是否支持预览
 *
 *  @return YES 支持预览 NO 不支持预览
 */
- (BOOL)isCameraPreviewStatu{
    return [self.kctCallService isCameraPreviewStatu];
    
}









//------------------------------------------------回调函数放置区域------------------------------------------------


#pragma mark VoIP通话的代理

//收到来电回调

- (void)onIncomingCall:(NSString*)callId withcalltype:(KCTCallTypeEnum) callType withcallerNumber:(NSString*)callerNumber
{
    
    
    if (self.UIDelegate && [self.UIDelegate respondsToSelector:@selector(incomingCallID:caller:phone:name:callStatus:callType:)])
    {
 
        [self.UIDelegate incomingCallID:callId caller:callerNumber phone:callerNumber name:callerNumber  callStatus:0 callType:[[NSNumber numberWithInt:callType] intValue] ];

    }
    if (self.UIDelegate && [self.UIDelegate respondsToSelector:@selector(responseVoipManagerStatus:callID:data:withVideoflag:)]) //更新拨号界面状态
    {
//        [self.UIDelegate responseVoipManagerStatus:ECallStatus_Incoming callID:callid data:caller];
    }
    

}



#pragma mark - 呼叫回调
//呼叫处理回调--------------------->待定
//- (void)onCallProceeding:(NSString*)callId{}
//呼叫振铃回调
- (void) onAlerting:(NSString*)called
{

    if (self.UIDelegate && [self.UIDelegate respondsToSelector:@selector(responseVoipManagerStatus:callID:data:withVideoflag:)])
    {
        [self.UIDelegate responseVoipManagerStatus:KCTCallStatus_Alerting callID:called data:nil withVideoflag:0];
    }

}
//接听回调
-(void) onAnswer:(NSString*)called
{
    if (self.UIDelegate && [self.UIDelegate respondsToSelector:@selector(responseVoipManagerStatus:callID:data:withVideoflag:)])
    {
        [self.UIDelegate responseVoipManagerStatus:KCTCallStatus_Answered callID:called data:nil withVideoflag:0];
    }
}
//呼叫失败回调
- (void) onDialFailed:(NSString*)callId  withReason:(KCTReason *) reason
{

    
    if (self.UIDelegate && [self.UIDelegate respondsToSelector:@selector(responseVoipManagerStatus:callID:data:withVideoflag:)])
    {
        [self.UIDelegate responseVoipManagerStatus:KCTCallStatus_Failed callID:callId data:reason withVideoflag:0];
    }
    


    
}
//释放通话回调
- (void) onHangUp:(NSString*)called withReason:(KCTReason *)reason
{
 

    
    if (self.UIDelegate && [self.UIDelegate respondsToSelector:@selector(responseVoipManagerStatus:callID:data:withVideoflag:)])
    {
        [self.UIDelegate responseVoipManagerStatus:KCTCallStatus_Released callID:called data:reason withVideoflag:0];
    }


}

// 语音质量回调实现
- (void)onNetWorkState:(KCTNetworkState)networkState networkDes:(NSString *)networkDes
{
    if (nil != _UIDelegate && [_UIDelegate respondsToSelector:@selector(showNetWorkState:networkDes:)]) {
        
        
        [_UIDelegate performSelector:@selector(showNetWorkState:networkDes:) withObject:[NSString stringWithFormat:@"%d",networkState] withObject:networkDes];
    }
}

// 语音质量原因回调
- (void)onNetWorkDetail:(NSString *)networkDetail{
    if (nil != _UIDelegate && [_UIDelegate respondsToSelector:@selector(showNetWorkDetail:)]) {
        
        [_UIDelegate performSelector:@selector(showNetWorkDetail:) withObject:networkDetail];
    }
}


//对端视频模式回调
- (void)onRemoteCameraMode:(KCTCameraType)type{
    
    if (nil != _UIDelegate && [_UIDelegate respondsToSelector:@selector(onRemoCameraMode:)]) {
        [self.UIDelegate onRemoCameraMode:type];
    }
    
    
}

//DTMF回调
- (void)onDTMF:(NSString*)value
{
    if (self.UIDelegate && [self.UIDelegate respondsToSelector:@selector(responseVoipManagerStatus:callID:data:withVideoflag:)])
    {
        KCTReason *uKCTReason = [[KCTReason alloc]init];
        uKCTReason.reason = 0;
        uKCTReason.msg = value;
        [self.UIDelegate responseVoipManagerStatus:KCTCallStatus_RecDTMFvalue callID:@"DTMF value" data:uKCTReason withVideoflag:0];
    }
    
    
}

//视频截图回调
- (void)onCameraCapture:(NSString*)cameraCapFilePath
{
    if (self.UIDelegate && [self.UIDelegate respondsToSelector:@selector(responseVoipManagerStatus:callID:data:withVideoflag:)])
    {
        KCTReason *uKCTReason = [[KCTReason alloc]init];
        uKCTReason.reason = 0;
        uKCTReason.msg = cameraCapFilePath;
        [self.UIDelegate responseVoipManagerStatus:KCTCallStatus_RecDTMFvalue callID:@"DTMF value" data:uKCTReason withVideoflag:0];
    }
    
    
}



//------------------------------------------------回调函数放置区域------------------------------------------------


///正式环境下日志开关
-(void)openSdkLog:(BOOL)isOpenSdkLog
{
//    [self.kctCallService openSdkLog:YES];//保存主机地址
    if ([self.kctCallService respondsToSelector:@selector(openSdkLog:)]) {
        ((id(*)(id,SEL,int))objc_msgSend)(self.kctCallService, @selector(openSdkLog:), YES);
    }
}

-(void)openMediaEngineLog:(BOOL)isOpenMediaEngineLog{
    
    if ([self.kctCallService respondsToSelector:@selector(openMediaEngineLog:)]) {
        ((BOOL(*)(id,SEL,BOOL))objc_msgSend)(self.kctCallService, @selector(openMediaEngineLog:), isOpenMediaEngineLog);
    }
    
}

- (void) callIncomingPushRsp:(NSString*)callid  withVps:(NSInteger)vpsid withReason:(KCTReason*)reason
{
    [self.kctCallService  callIncomingPushRsp:callid withVps:vpsid withReason:reason];
    
}

/**
 @author WLS, 16-01-11 18:01:09
 
 初始化成功
 
 @param result 结果
 */
- (void)onInitEngineSuccessful:(NSInteger)result{
    
    
    NSLog(@"version : %@",[self getKCTSDKVersion]);
    
    [[NSNotificationCenter defaultCenter] postNotificationName:KCTNotiEngineSucess object:nil];
//    [_kctCallService setRTPEncEnable:YES];
}
//- (BOOL)setAGCPlus:(int)compressionGain targetDbfs:(int)targetDbfs{
//    
//    
//    return [_kctCallService setAGCPlus:compressionGain targetDbfs:targetDbfs];
//}

//-(void)decryptCallBack:(char*)inData withOutMsg:(char*)outData withInLen:(int)bytesIn withOutLen:(int*)bytesOut
//{
//    
//    
//    memcpy(outData, inData, bytesIn);
//    *bytesOut = bytesIn;
//    
//        
//    
//    
//}
//
//-(void)encryptCallBack:(char*) inData withOutMsg:(char*)outData withInLen:(int )bytesIn withOutLen:(int*)bytesOut
//{
//    
//        
//    memcpy(outData, inData, bytesIn);
//    *bytesOut = bytesIn;
//    
//    
//}



@end
