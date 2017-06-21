/* SoftphoneManager.h
 *
 * Created by Liam
 * 2012.6.14
 */

#import <Foundation/Foundation.h>
#import <AppKit/AppKit.h>
#import "SoftphoneUIDelegates.h"
#import "KCTCommonClass.h"
#import "NuBSON.h"
//#import "Config.h"
#ifndef UGO_ENABLED

//#import "ugo.h"
#else
//#import "ugo_test.h"
#endif
@class KCTReason;
@class KCTVideoEncAttr;
@class KCTVideoDecAttr;
@class KCTCameraAttr;
@class MacVideoView;

@interface SoftphoneManager : NSObject
{
@private
    id<SoftphoneUIDelegates> delegate;
}

@property (nonatomic, retain) id<SoftphoneUIDelegates> delegate;
@property (nonatomic, assign) KCTReason * reasons;
@property (nonatomic, assign) BOOL isIpv6;
@property (nonatomic,retain) NSString *localMusicPath;

+ (SoftphoneManager*) instance;


#pragma mark - --------------------------组件状态设置--------------------------
// 设置Softphone
- (BOOL) setSoftphoneManager;

// 初始化Softphone
- (BOOL) initSoftphoneManager;

//开启UDP Ping 检测
- (void)startVoipListPing;

- (void)setVideoCodecWithIndex:(NSInteger)videoCodecIndex audioCodecIndex:(NSInteger)audioCodecIndex;
#pragma mark - --------------------------互联网音频能力--------------------------
// 拨打电话
- (BOOL) callPhoneNumber:(NSString *) strUid withPN:(NSString *) strPhoneNumber withCallMode:(int)callMode  andUserdata:(NSString *)callerData;

// 挂断
- (BOOL) hangUpCall;

// 接听
- (BOOL) answerCall;

// 发送DTMF
- (BOOL) sendDTMF:(char) mDigit;

// 开关免提
- (BOOL) setLoudSpeaker:(BOOL) bOpen;

// 获取免提状态
- (BOOL) getLoudSpeakerState;

// 开关Mic，是否静音
- (BOOL) setMicStatus:(BOOL) bOpen;


#pragma mark - --------------------------互联网视频能力--------------------------
// 设置视频信息 add by WLS 2015.10.26
- (int) initCameraConfig:(NSView *) Local_pImage1 withRemoteView:(NSView *) Remote_pImage2;

// 拨打视频电话
- (BOOL) callPhoneNumberForVideo:(NSString *) strUid withPN:(NSString *) strPhoneNumber withCallMode:(int)callMode andUserdata:(NSString *)callerData;

// 获取摄像头个数
- (int) getCameraNum;

// 前后摄像头切换 return 0 成功 -1 失败 add by WLS 2015.10.26
- (int) switchCameraDevice:(int)CameraIndex;

// 切换视频模式：发送、接收、正常模式 add by WLS 20151026
- (int) switchVideoMode:(KCTCameraType)type;

// 自定义视频编码和解码参数 add by WLS 2015.10.26
- (void)setVideoAttr:(KCTVideoEncAttr*)ucsVideoEncAttr andVideoDecAttr:(KCTVideoDecAttr*)ucsVideoDecAttr;

// 摄像头取样的视频参数
//- (BOOL)setVideoCameraAttr:(KCTCameraAttr*)ucsCameraAttr;

// 设置摄像头旋转角度0 90 180 270
- (BOOL)setCameraRotateAngleWithSend:(unsigned int)sendRotateAngle andRecivied:(unsigned int)reciviedRotateAngle;

// 视频截屏
-(int)screenshotForVideo:(int)islocal withFileName:(NSString*)filename withSavePath:(NSString*)savePath;


#pragma mark - --------------------------其他能力--------------------------
// @author WLS, 16-02-01 15:02:17 开关日志存储
- (BOOL) setMediaEngineLogStatus:(BOOL) bOpen;

// 获取电话组件版本号
- (NSString *) getSoftphoneVersion;

/**
 @author WLS, 16-01-08 10:01:16
 
 添加录音功能，通话过程中录音
 
 @param file 文件路径
 @return 返回操作结果
 */
- (BOOL)recordStartWithFileName:(NSString *)file;

/**
 @author WLS, 16-01-08 11:01:21
 
 停止录音
 
 @return 返回操作结果
 */
- (BOOL)stopRecord;

/**
 @author WLS, 16-05-23 15:05:26
 
 语音质量上报
 */
- (void)emodelValue;
- (void)setIncomingCallMusicPath:(NSString *)path;
//播放本地 铃声
- (void)playLocalMusic;
/**
 @author WLS, 16-05-31 10:05:26
 
 设置IPV6网络环境，在通话前设置有效
 */
- (void)setIpv6Enable:(BOOL)isIpv6;

- (void)setDefaultRttp;

#pragma mark - --------------------------push续活能力--------------------------
- (void)incomingRspWhenBackground:(id)callID vpsid:(int)vpsid;

- (void)incomingRsp:callID vpsid:(NSInteger)vpsid withReason:(KCTReason*)reason;

- (void)incomingRsp:callID vpsid:(int)vpsid;

- (NSString *)getCurrentCallid;


#pragma mark - --------------------------创建callid--------------------------
//创建callid
- (NSString*)createCallidByClientNumber:(NSString*)clientNumber;


#pragma mark - 设置AGC增益
/**
 @author WLS, 16-05-20 10:05:23
 
 通话前或者通话中设置均有效，建议通话前设置。
 
 @param compressionGain 自适应语音增益的压缩增益:取值范围: 1- 30，此值越大音量越大；建议值为6 - 12，6为小音量，9为默认音量，12为大音量， 1 - 6 或者 12 - 30 的值需要谨慎使用。
 
 @param  自适应语音增益的目标电平: 取值范围:1---15; 此值越小音量越大；建议值为3 - 9，9为小音量，6为默认音量，3为大音量，1 - 3 或者 9 - 15 的值需要谨慎使用。
 
 @return YES设置成功，NO设置失败
 */
- (BOOL)setAgcCompressionGaindB:(int)compressionGaind targetDbfs:(int)targetDbfs;


#pragma mark - rtp包加密控制
/**
 @author WLS, 16-02-24 19:02:18
 
 设置rtp包加密控制开关
 
 @param enable 开关
 
 @return YES为设置成功 , NO为设置失败
 */
- (BOOL)setRtpEncEnable:(BOOL)enable;


@end


