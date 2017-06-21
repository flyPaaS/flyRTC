//
//  VoipEngine.h
//  KCT_VOIP_Demo
//
//  Created by KCMac on 2017/5/26.
//  Copyright © 2017年 flypass. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "KCTPubEnum.h"
#import <objc/message.h>


@protocol VoipEngineDelegate <NSObject>

@optional
//来电信息
-(void)incomingCallID:(NSString*)callid caller:(NSString*)caller phone:(NSString*)phone name:(NSString*)name callStatus:(int)status callType:(NSInteger)calltype;

//通话状态回调
-(void)responseVoipManagerStatus:(KCTCallStatus)event callID:(NSString*)callid data:(KCTReason *)data withVideoflag:(int)videoflag;



// 语音质量展示回调
- (void)showNetWorkState:(NSString *)networkStateStr networkDes:(NSString *)networkDes;
// 对端视频模式回调
- (void)onRemoCameraMode:(KCTCameraType)type;

@end



//这个类是属于上面业务层与SDK之间的调度层，负责调用SDK接口和接收分发SDK的回调
@interface VoipEngine : NSObject<KCTEventDelegate>

@property(nonatomic,assign) id<VoipEngineDelegate>UIDelegate;

+(VoipEngine *)getInstance;


- (void)setVideoCodecWithIndex:(NSInteger)videoIndex audioCodecIndex:(NSInteger)audioIndex;

#pragma mark - -------------------呼叫控制函数-------------------
/**
 * 拨打电话
 * @param callType 电话类型
 * @param called 电话号(加国际码)或者VoIP号码
 * @return
 */
- (void)dial:(NSInteger)callType andCalled:(NSString *)calledNumber andUserdata:(NSString *)callerData;

/**
 * 挂断电话
 * @param callid 电话id
 * @param reason 预留参数
 */
- (void) hangUp: (NSString*)called;

/**
 * 接听电话
 * @param callid 电话id
 * V2.0
 */
- (void) answer: (NSString*)callId;

/**
 * 拒绝呼叫(挂断一样,当被呼叫的时候被呼叫方的挂断状态)
 * @param callid 电话id
 * @param reason 拒绝呼叫的原因, 可以传入ReasonDeclined:用户拒绝 ReasonBusy:用户忙
 */
- (void)reject: (NSString*)called;

#pragma mark - -------------------本地功能函数-------------------

/**
 * 免提设置
 * @param enable false:关闭 true:打开
 */
- (void) setSpeakerphone:(BOOL)enable;


/**
 * 获取当前免提状态
 * @return false:关闭 true:打开
 */
- (BOOL) isSpeakerphoneOn;


/**
 * 静音设置
 * @param on false:正常 true:静音
 */
- (void)setMicMute:(BOOL)on;

/**
 * 获取当前静音状态
 * @return false:正常 true:静音
 */
- (BOOL)isMicMute;

- (NSString*) getKCTSDKVersion;


#pragma mark - -------------------视频能力------------------------

/**
 *  初始化视频显示控件（本地视频显示控件和对方视频显示控件）
 *
 *参数 frame 窗口大小
 *
 *@return UIView 视频显示控件:
 */
- (NSOpenGLView *)allocCameraViewWithFrame:(CGRect)frame;

/**
 * 设置视频显示参数
 *
 *参数 localVideoView 设置本地视频显示控件
 *参数 remoteView     设置对方视频显示控件
 *
 *@return NO:  YES:
 */
-(BOOL)initCameraConfig:(NSView*)localVideoView withRemoteVideoView:(NSView*)remoteView;

/**
 *  自定义视频编码和解码参数
 *
 *  @param kctVideoEncAttr 编码参数
 *  @param kctVideoDecAttr 解码参数
 */
- (void)setVideoAttr:(KCTVideoEncAttr*)kctVideoEncAttr andVideoDecAttr:(KCTVideoDecAttr*)kctVideoDecAttr;


/**
 * 旋转显示图像角度
 *
 * 参数 sendRotation      本地显示图像角度  数值为0 90 180 270
 * 参数 reciviedRotation  对端显示图像角度  数值为0 90 180 270
 *@return NO:  YES:
 */
- (BOOL)setRotationVideo:(unsigned int)sendRotation withReciviedRotation:(unsigned int)reciviedRotation;


/**
 *
 * 获取摄像头个数
 */
- (int)getCameraNum;

/**
 * 摄像头切换 后置摄像头：0 前置摄像头：1
 *return YES 成功 NO 失败
 */
- (BOOL)switchCameraDevice:(int)CameraIndex;


/**
 *  切换视频模式：发送、接收、正常模式
 *
 *  @param type         CAMERA_RECEIVE : 只接收视频数据（只能接收到对方的视频）
 CAMERA_SEND : 只发送视频数据（只让对方看到我的视频）
 CAMERA_NORMAL : send receive preview
 *
 *  @return YES 成功 NO 失败
 */
- (BOOL)switchVideoMode:(KCTCameraType)type;


/**
 * 视频截图
 * @param islocal: 0 是远端截图 1 是本地截图。
 * @param filename: 截图名称。
 * @param savePath: 存放路径。
 *
 */
- (void)cameraCapture:(int)islocal withFileName:(NSString*)filename withSavePath:(NSString*)savePath;


/**
 * 视频来电时是否支持预览
 * isPreView: YES 支持预览 NO 不支持预览。
 * return YES 成功 NO 失败
 */
- (BOOL)setCameraPreViewStatu:(BOOL)isPreView;


/**
 *  获取视频来电时是否支持预览
 *
 *  @return YES 支持预览 NO 不支持预览
 */
- (BOOL)isCameraPreviewStatu;

///正式环境下日志开关
-(void)openSdkLog:(BOOL)isOpenSdkLog;


-(void)openMediaEngineLog:(BOOL)isOpenMediaEngineLog;


@end
