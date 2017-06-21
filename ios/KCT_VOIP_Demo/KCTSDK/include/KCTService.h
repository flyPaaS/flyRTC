//
//  KCTCallService.h
//  yzxapisdk
//
//  Created by wenqinglin on 14-4-2.
//  Copyright (c) 2014年 yzx. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "KCTCommonClass.h"
#import "KCTEvent.h"
@class VideoView;
@interface KCTService : NSObject


#pragma mark - -------------------初始化函数-----------------------
//初始化实例
- (KCTService *)initWithDelegate:(id<KCTEventDelegate>)delegate;
//设置代理方法
-(void)setDelegate:(id< KCTEventDelegate>)delegate;



#pragma mark - -------------------呼叫控制函数-----------------------
/**
 * 拨打电话
 * @param callType 电话类型 0 语音电话  2 视频电话
 * @param called userid号码
 */
- (void)dial:(NSInteger)callType andCalled:(NSString *)calledNumber andUserdata:(NSString *)callerData;

/**
 * 挂断电话
 * @param callid 电话id
 */
- (void) hangUp: (NSString*)callid;

/**
 * 接听电话
 * @param callid 电话id
 */
- (void) answer: (NSString*)callid;

/**
 * 拒绝呼叫(挂断一样,当被呼叫的时候被呼叫方的挂断状态)
 * @param callid 电话id
 */
- (void)reject: (NSString*)callid;



#pragma mark - -------------------DTMF函数-----------------------
/**
 * 发送DTMF
 * @param callid 电话id
 * @param dtmf 键值
 */
- (BOOL)sendDTMF: (char)dtmf;



#pragma mark - -------------------本地功能-----------------------
/**
 * 免提设置
 * @param enable NO:关闭 YES:打开
 */
- (void) setSpeakerphone:(BOOL)enable;

/**
 * 获取当前免提状态
 * @return NO:关闭 YES:打开
 */
- (BOOL) isSpeakerphoneOn;

/**
 * 静音设置
 * @param on NO:正常 YES:静音
 */
- (void)setMicMute:(BOOL)on;

/**
 * 获取当前静音状态
 * @return NO 正常 YES 静音
 */
- (BOOL)isMicMute;



#pragma mark - -------------------辅助能力-----------------------
/**
 * 获取SDK版本信息
 */
- (NSString*) getKCTSDKVersion;

/**
 * 获取流量分类统计信息
 */
- (NSDictionary*) getNetWorkFlow;

/**
 * 设置当前网络是否是IPV6，默认IPV4，建议在初始化完成后设置，也可在拨打电话之前设置。
 * @param isIpv6 NO:不是IPv6  YES:是IPv6
 */
- (void)setIpv6:(BOOL)isIpv6;
- (void)setVideoCodecWithIndex:(NSInteger)videoIndex audioCodecIndex:(NSInteger)audioIndex;

#pragma mark - -------------------视频能力------------------------

/**
 * 初始化视频显示控件（本地视频显示控件和对方视频显示控件）
 * 参数 frame 窗口大小
 * @return UIView 视频显示控件:
 */
- (UIView *)allocCameraViewWithFrame:(CGRect)frame;

/**
 * 设置视频显示参数
 * 参数 localVideoView 设置本地视频显示控件
 * 参数 remoteView     设置对方视频显示控件
 * @return NO:  YES:
 */
-(BOOL)initCameraConfig:(UIView*)localVideoView withRemoteVideoView:(UIView*)remoteView;

/**
 * 自定义视频编码和解码参数
 * @param kctVideoEncAttr 编码参数
 * @param kctVideoDecAttr 解码参数
 */
- (void)setVideoAttr:(KCTVideoEncAttr*)kctVideoEncAttr andVideoDecAttr:(KCTVideoDecAttr*)kctVideoDecAttr;

/**
 * 旋转显示图像角度
 * 参数 sendRotation      本地显示图像角度  数值为0 90 180 270
 * 参数 reciviedRotation  对端显示图像角度  数值为0 90 180 270
 * @return NO:  YES:
 */
- (BOOL)setRotationVideo:(unsigned int)sendRotation withReciviedRotation:(unsigned int)reciviedRotation;

/**
 * 获取摄像头个数
 */
- (int)getCameraNum;

/**
 * 摄像头切换 后置摄像头：0 前置摄像头：1
 * @return YES 成功 NO 失败
 */
- (BOOL)switchCameraDevice:(int)CameraIndex;

/**
 * 切换视频模式：发送、接收、正常模式
 *
 * @param type         CAMERA_RECEIVE : 只接收视频数据（只能接收到对方的视频）
                        CAMERA_SEND    : 只发送视频数据（只让对方看到我的视频）
                        CAMERA_NORMAL  : send receive preview
 * @return YES 成功 NO 失败
 */
- (BOOL)switchVideoMode:(KCTCameraType)type;

/**
 * 视频截图
 * @param islocal: 0 是远端截图 1 是本地截图。
 * @param filename: 截图名称。
 * @param savePath: 存放路径。
 */
- (void)cameraCapture:(int)islocal withFileName:(NSString*)filename withSavePath:(NSString*)savePath;

/**
 * 视频来电时是否支持预览
 * isPreView: YES 支持预览 NO 不支持预览。
 * @return YES 成功 NO 失败
 */
- (BOOL)setCameraPreViewStatu:(BOOL)isPreView;

/**
 * 获取视频来电时是否支持预览
 * @return YES 支持预览 NO 不支持预览
 */
- (BOOL)isCameraPreviewStatu;



/**
 * 离线来电推送
 */
- (void)callIncomingPushRsp:(NSString*)callid  withVps:(NSInteger)vpsid withReason:(KCTReason*)reason;
- (void)incomingRspWhenBackground:(id)callID vpsid:(int)vpsid;

#pragma mark - -------------------智能硬件能力------------------------

/**
 * 设置rtp包加密控制开关，默认不支持加密
 * enable: YES 支持加密 NO 不支持加密
 * return YES 成功 NO 失败
 */
- (BOOL)setRTPEncEnable:(BOOL)enable;

/**
 * 设置AGC音效增益
 * 建议通话前设置。
 * compressionGain: 自适应语音增益的压缩增益:取值范围: 1- 30，此值越大音量越大；建议值为6 - 12，6为小音量，9为默认音量，12为大音量， 1 - 6 或者 12 - 30 的值需要谨慎使用。
 * targetDbfs : 自适应语音增益的目标电平: 取值范围:1---15; 此值越小音量越大；建议值为3 - 9，9为小音量，6为默认音量，3为大音量，1 - 3 或者 9 - 15 的值需要谨慎使用。
 * @return YES 成功 NO 失败
 */
- (BOOL)setAGCPlus:(int)compressionGain targetDbfs:(int)targetDbfs;
- (void)setIncomingCallMusicPath:(NSString *)path;


@end
