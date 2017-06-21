//
//  UCSDialViewController.h
//  UXinClient
//
//  Created by Lingyun Han on 11-10-17.
//  Copyright 2011年 UXIN. All rights reserved.
//


#import "KCTEvent.h"
#import "SoftphoneUIDelegates.h"

@interface UCSDialViewController : NSObject <SoftphoneUIDelegates>
{
    
    float                   m_fSystemVolume;            // 系统音量大小
}



@property (nonatomic, assign)   id<KCTEventDelegate>   mucsdelegate;

@property (nonatomic,assign)BOOL needRecord; // WLS，2016-01-08，（是否需要通话中录音）


@property (nonatomic,assign)BOOL isUCSCalling; //上传会话日志需要
@property (nonatomic,assign)BOOL isEndCall; //会话结束，等待最后一次组件日志，然后上传会话日志。

+ (UCSDialViewController*) share;




- (void)SoftPhoneCallback:(int) ev_type withReason:(int) ev_reason withDescription:(NSString *) strDescription withParam:(const void *) param;

//释放通话
- (void)hangUpCall;
//被叫接听
- (void)onAnswerCall;
//被叫拒绝接听
- (void)onHangupCall;
//回拨呼叫
- (void)OnCallBackBegin:(NSString *)phoneNumber showFromNum:(NSString*)fromNum showToNum:(NSString*)toNum;
//取消回拨呼叫
- (void)CancelOnCallBackBegin:(NSString *)phoneNumber;
//获取当前通话callid
- (NSString *)getCurrentCallid;

// 免提开关
- (void)loudSpeakerChanged:(BOOL) bOpen;
- (BOOL)getLoudSpeakerOpenStatus;
- (void)setMicMute:(BOOL)on;
- (BOOL)isMicMute;


//振铃
- (void)onAlerting:(NSString*)callid withVideoflag:(int)videoflag;
//已接通
- (void)onAnswer:(NSString*)callid;
//已挂断
- (void)onHangUp:(NSString*)callid withReason:(KCTReason *)reason;
//呼叫失败回调
- (void)OnCallFailed:(NSString *) strReason andMsg:(NSString *)msg withCallId:(NSString*)callid;
//正在接通
- (void)onCallBackWithReason:(KCTReason*)reason;

- (void)onDialFailed:(NSString*)callid  withReason:(KCTReason *) reason;

- (void)onScreenshotForVideo:(NSString*)cameraCapFilePath;


//重置通话的参数 add by WLS 20151009
- (void)resetParameter;


//rtp 加密
-(void)DecryptCallBack:(char*)in_data withOutdata:(char*)out_data withByteIn:(int)bytes_in withByteOut:(int*)bytes_out;
// rtp解密
-(void)EncryptCallBack:(char*)in_data withOutdata:(char*)out_data withByteIn:(int)bytes_in withByteOut:(int*)bytes_out;

/**
 @author WLS, 16-01-21 11:01:21
 
 发起通话的时候，需要记录会话信息
 
 @param callid      会话id
 @param incomingStr 会话详情
 */
- (void)apendVoiceFormatWithCallId:(NSString *)callid incomingCall:(NSString *)incomingStr;
@end
