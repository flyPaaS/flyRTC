//
//  UCSDialViewController.m
//  UXinClient
//
//  Created by Lingyun Han on 11-10-17.
//  Copyright 2011年 UXIN. All rights reserved.
//

#import "UCSDialViewController.h"

#import "SoftphoneManager.h"
#import "ugo.h"

#import <objc/message.h>
#import "KCTCommonClass.h"
#import "UCSCallingHttpRequest.h"
#import "UCSStatiRequestManager.h"
#import "CallLogManager.h"
#import "UCSRecordRequestManager.h"
#import "UCSConLogRequestManager.h"


#define Action_Sheet_Tag_Choose_Phone 1
#define AlertView_Tag_Show_Allow_Upload_Local_Contact_Tips 2

#define Action_Sheet_Tag_Clear_All_Calllist 2
#define callerNumber                                    @"callerNumber"
#define callerFname                                     @"callerFname"
#define call_Meetingflag                                 @"callMeetingFlag"
#define call_Userdata                                    @"callUserdata"

#define RecordTime 180  //录音时间 3分钟

@interface UCSDialViewController()
{
        BOOL                  m_bLoudSpeakerOpen;         // 免提是否打开
        BOOL                  m_bSilented;        // 是否已经静音
    
        UCSCallingHttpRequest *_callRequest;
        UCSCallingHttpRequest *_emodelRequest;
//        CallLogReport      *_logReport;
    
    
    //拨号超时检测
    int m_CallTime;//拨号超时
    NSTimer *m_CallTimeOut;//拨号超时计时
    BOOL IsCallTimeOutCheck;
    
    // add by pbf
    long nowAnswer;
    long nowHungUp;
    

    
    NSTimer * _recordTimer; // WLS，2016-01-08， 录音定时器
    
    UCSRecordRequestManager * _recordRequest;// WLS，2016-01-11， 录音上传请求
    
    NSMutableString * _sdkLogString; // CW 2016-1-13 会话日志需要上传的字符串
    NSMutableDictionary * _sdkLogDic;
    NSString * _callLogDetail; // WLS 2015-3-13 通话结束后的callLog日志。
    
    UCSConLogRequestManager * _sdkLogRequest; // CS 2016-1-13 会话日志上传请求
    
    
    NSString * _incomingCallid;
    NSString * _icomingCaller;
    
}


@end





@implementation UCSDialViewController

@synthesize mucsdelegate = _mucsdelegate; //ucs回调代理



static UCSDialViewController *seh = nil;

+ (UCSDialViewController*) share
{
	return seh;
}

#pragma mark - View lifecycle
- (id) init
{
    TIME_ANALYZE_BEGINMAIN( 5, 0, "UCSDialViewController - init" );
    self = [super init];
    if (self)
    {
//        // Initialization code here.
        seh = self;
 
        
    }
        _isUCSCalling = NO;
        m_bLoudSpeakerOpen = FALSE;//免提开关初始化
        m_bSilented =false;//静音初始化
      IsCallTimeOutCheck = NO;//是否进行拨号超时检测开关。
    return self;
}

- (void) dealloc
{
    seh = nil;
    _recordRequest = nil;
    
    [super dealloc];
}



// 准备所接受来电请求(支持昵称)
- (void) readyForRecvIncomingCall:(NSString*)callId  whithVideoFlag:(int)videoFlag withCallInfo:(NSDictionary *)callInfo//此处关键，根据组件传过来的UID查找手机好吗
{
    
    
    NSString *strUid = [callInfo objectForKey:callerNumber];
    UXLogNotice(@"支持昵称 UCS来电 uid:%@",strUid);
    UXLogNotice(@"支持昵称 UCS来电 callId:%@", callId);
    //    UXinFriend *f = [[UXinFriendsManager instance] getFriendByUID:[strUid longLongValue]];
    
    PhoneNetType currentNetType = [PublicFunc GetCurrentPhoneNetType];
    UXLogNotice(@"UCS网络类型 PNT_2G＝ %d", currentNetType);
    
    /*
     2g状态下接收来电的时候，不再将通话挂断
    if (PNT_2G == currentNetType)
    {
        // 非WIFI网络，不能作为被叫
        UXLogNotice(@"收到来电，但是网络不支持");
        [[SDKLogManager instance]saveSDKLogInfo:@"收到来电，但是网络不支持" withDetail:callId];
        [[SoftphoneManager instance] hangUpCall];
        return;
        
    }
     */
    
    [[SDKLogManager instance]saveSDKLogInfo:@"收到来电" withDetail:callId];
    
    
    //收到来电的时候 重置通话参数 add by WLS 20151009
    [self resetParameter];
    
    
    if ([UserDefaultManager GetLocalDataBoolen:DATA_STORE_ISRECORD]) {
        self.needRecord = YES;
    }
    
    _incomingCallid = [callId copy];
    _icomingCaller = [[callInfo objectForKey:callerNumber] copy];
    
    //UXLogAlert(@"need record : %d---%d",[UCSDialViewController share].needRecord,callId);

    
    
    if (videoFlag==0) { //语音通话
        
        //视频通话 add by WLS 20151009
        [UserDefaultManager SetLocalDataBoolen:NO key:DATA_STORE_ISVIDEO];
        
        /**
         @author WLS, 16-01-21 11:01:40
         
         储存会话记录格式
         */
        [self apendVoiceFormatWithCallId:callId incomingCall:@"收到语音来电"];
        
        
        //不支持昵称分支
        dispatch_async(dispatch_get_main_queue(), ^(void){
            
            [self onIncomingCallReceived:callId withcalltype:KCTCallType_VOIP withcallerNumber:[callInfo objectForKey:callerNumber]];
            
        });
        
        //支持昵称 add by kucky 20141203
        dispatch_async(dispatch_get_main_queue(), ^(void){
            
            [self onIncomingCallReceived:callId withcalltype:KCTCallType_VOIP withcallerinfo:callInfo];
            
        });
        
        
        
    }
    
    if (videoFlag==1) { //视频通话
        
    
        
        //视频通话 add by WLS 20151009
        [UserDefaultManager SetLocalDataBoolen:YES key:DATA_STORE_ISVIDEO];
        /**
         @author WLS, 16-01-21 11:01:40
         
         储存会话记录格式
         */
        [self apendVoiceFormatWithCallId:callId incomingCall:@"收到视频来电"];
        
        
        //不支持昵称分支
        dispatch_async(dispatch_get_main_queue(), ^(void){
            
            [self onIncomingCallReceived:callId withcalltype:KCTCallType_VideoPhone withcallerNumber:[callInfo objectForKey:callerNumber]];
            
        });
        
        ////支持昵称 add by kucky 20141203
        dispatch_async(dispatch_get_main_queue(), ^(void){
            
            [self onIncomingCallReceived:callId withcalltype:KCTCallType_VideoPhone withcallerinfo:callInfo];
            
        });
    }
    
    
 
    
}



//// 免提开关
- (void) loudSpeakerChanged:(BOOL) bOpen
{
//    m_bLoudSpeakerOpen = bOpen;
//    [[SoftphoneManager instance] setLoudSpeaker:bOpen];
//    dispatch_async(dispatch_get_main_queue(), ^(void){
//        NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];

        m_bLoudSpeakerOpen = bOpen;
        [[SoftphoneManager instance] setLoudSpeaker:bOpen];
        
//        [pool release];
//    });
    
}
//免提状态
- (BOOL)getLoudSpeakerOpenStatus
{
    
//    return m_bLoudSpeakerOpen;
    
    
    return [[SoftphoneManager instance] getLoudSpeakerState];
}

// 静音设置
- (void)setMicMute:(BOOL)on;
{
    [[SoftphoneManager instance] setMicStatus:on];
    m_bSilented = on;
    
}

//获取当前静音状态
- (BOOL)isMicMute;
{
    return m_bSilented;
}



// 点击接听按钮
- (void) onAnswerCall
{

    [self answerCall];
}

// 点击拒绝按钮
- (void) onHangupCall
{
    [self hangUpCall];
}

// 接听来电
- (void) answerCall
{
    
    [[SoftphoneManager instance] answerCall];
    
    UXLogNotice(@"接听电话");
}

//挂断来电
- (void) hangUpCall
{
    UXLogNotice(@"挂断电话");
    [[SoftphoneManager instance] hangUpCall];
    
}



// 发起回拨呼叫
- (void) OnCallBackBegin:(NSString *)phoneNumber showFromNum:(NSString*)fromNum showToNum:(NSString*)toNum;
{

    
    // 发起呼叫请求
    if (_callRequest == nil) {
        _callRequest = [[UCSCallingHttpRequest alloc] init];
    }
    
    //callId (WLS)
    [_callRequest callbackRequest:phoneNumber showFromNum:(NSString*)fromNum showToNum:(NSString*)toNum onSuccess:^(NSDictionary *json,NSString *callId) {
        [self onCallBackResult:json WithCallId:callId];
    } onFailed:^(NSDictionary *json,NSString *callId) {
        [self OnCallFailed:@"402003" andMsg:@"呼叫超时(网络连接问题)" withCallId:@"CallBACK"];
    }];
    [_callRequest startAsyncRequest];
}


// 取消回拨呼叫
- (void) CancelOnCallBackBegin:(NSString *)phoneNumber
{
    
    
    // 发起呼叫请求
    if (_callRequest == nil) {
        _callRequest = [[UCSCallingHttpRequest alloc] init];
    }
    
    [_callRequest cancelCallbackRequest:phoneNumber onSuccess:^(NSDictionary *json,NSString *callId){
        
    } onFailed:^(NSDictionary *json,NSString *callId){
        
        [self OnCallFailed:@"402003" andMsg:@"取消回拨失败" withCallId:@"CancelCallBACK Failed"];
        
    }];
    //    [_callRequest startAsyncRequest];
}

 


// 回拨请求发出后的返回值
// returnDic : 返回值
- (void) onCallBackResult:(NSDictionary *) returnDic WithCallId:(NSString *)callId
{
 
    KCTReason *reason = [[KCTReason alloc] init];
    
    int nResult = [[returnDic objectForKey:@"result"]intValue];
    if (0 == nResult)
    {
        int nCallMode = [[returnDic objectForKey:@"mode"]intValue];
        
        //NSLog(@"-------%@-------",reason.callId);

        
        if (2 == nCallMode) // 半回拨
        {
//            m_CallType = CALL_HISTORY_TYPE_BHB;
            NSLog(@"半回拨 Success");
        }
        else // 回拨
        {
            NSLog(@"回拨 Success");
            reason.reason = 0;
            reason.msg = @"回拨 Success";
            //callId (WLS)
            reason.callId = callId;
            [self onCallBackWithReason:reason];
            //NSLog(@"-------%@-------",reason.callId);
            
            
          
            
        }
        
 
    }
    else
    {
        NSString *strErr = @"";
        switch (nResult)
        {

            case 4: // 被叫格式错误
            {
                strErr = [NSString stringWithFormat:@"回拨请求发送失败(被叫号码格式错误，请核实后再拨)。"];
                reason.reason = 402025;
                reason.msg = strErr;
                [self onCallBackWithReason:reason];
            }
                break;
            case 5: // 主叫没有绑定手机号
            {
                strErr = [NSString stringWithFormat:@"回拨请求发送失败(未绑定手机号)。"];
                
                reason.reason = 402026;
                reason.msg = strErr;
                [self onCallBackWithReason:reason];
            }
                break;
            case 6: //  余额不足
            {
                
                  strErr = [NSString stringWithFormat:@"回拨请求发送失败(余额不足)。"];
                
                reason.reason = 402018;
                reason.msg = strErr;
                [self onCallBackWithReason:reason];
             
            }
                break;
             //--------------增加内部验证错误码支持20140905 add by kucky-------------
            case -1: // 参数缺少ac；ac校验失败
            {
                strErr = [NSString stringWithFormat:@"服务器内部错误"];
                reason.reason = 401022; //未正式定义
                reason.msg = strErr;
                [self onCallBackWithReason:reason];
            }
                break;
            case 1: // 参数缺少uid
            {
                strErr = [NSString stringWithFormat:@"服务器内部错误"];
                reason.reason = 401022;  //未正式定义
                reason.msg = strErr;
                [self onCallBackWithReason:reason];
            }
                break;
            case 3: // 参数缺少(securityver p v pv u sn sign SecurityFlag) ;security_flag值错误; sign验证失败 sn重复申请回拨
            {
                strErr = [NSString stringWithFormat:@"服务器内部错误"];
                reason.reason = 401022;  //未正式定义
                reason.msg = strErr;
                [self onCallBackWithReason:reason];
            }
                break;
            case 10: // ac过期 //未正式定义
            {
                strErr = [NSString stringWithFormat:@"服务器内部错误"];
                reason.reason = 401022;
                reason.msg = strErr;
                [self onCallBackWithReason:reason];
            }

                
            default:
            {
                strErr = [NSString stringWithFormat:@"回拨请求发送失败(%d)。", nResult];
                reason.reason = 401022;
                reason.msg = strErr;
                [self onCallBackWithReason:reason];
            }
                break;
        }
//        [self ShowCallingProcessingMsg:strErr];
//        [self SetCallStatus:@"呼叫失败"];
        
        // 启动关闭窗口定时器
//        [self endCall];
    }
    
    
    
    [reason release];
}




- (NSString*) getcallNum:(ugo_callev_pm_t*)pm
{
    //新版SDK 被叫的参数有可能在fuserid里面。 add by WLS 20151026
    //return  strlen(pm->fphone) == 0?(strlen(pm->fuserid) <= 1 ? [NSString stringWithUTF8String:pm->fuid]:[NSString stringWithUTF8String:pm->fuserid]):[NSString stringWithUTF8String:pm->fphone];
    //modify by wenqinglin
    return [NSString stringWithUTF8String:pm->fuid];
}

- (NSDictionary*) getcallInfo:(ugo_callev_pm_t*)pm
{
    NSString *callNum = [self getcallNum:pm];
    NSString *callfname = [NSString stringWithUTF8String:pm->fnickname];
    NSString *callUserdata = [NSString stringWithUTF8String:pm->user_data];
    
    
    return [NSDictionary dictionaryWithObjectsAndKeys:callNum,callerNumber,callfname,callerFname, callUserdata,call_Userdata,nil];
    
    
}


#pragma mark - Softphone Function

/**
 @author WLS, 15-12-02 14:12:49
 
 拷贝组件传过来的参数
 
 @param param     组件传过来的参数
 @param copyParam copy组件参数后的结构体对象
 */
- (void)copyInfoFromParam:(const void *)param copyParam:(void *)copyParam{
    
    ugo_callev_pm_t *pm = (ugo_callev_pm_t *)param;
    ugo_callev_pm_t *copyPm = (ugo_callev_pm_t *)copyParam;
    
    
    copyPm ->videoflag   = pm ->videoflag;
    copyPm ->audiofec    = pm ->audiofec;
    copyPm ->directfec   = pm ->directfec;
    copyPm ->directmulriplefec   = pm ->directmulriplefec;
    copyPm ->calltype    = pm ->calltype;
    copyPm ->meetingflag = pm ->meetingflag;
    copyPm ->ucalltype   = pm ->ucalltype;
    
    memcpy(copyPm ->callid, pm ->callid, sizeof(pm ->callid));
    memcpy(copyPm ->fuid, pm ->fuid, sizeof(pm ->fuid));
    memcpy(copyPm ->fuserid, pm ->fuserid, sizeof(pm ->fuserid));
    memcpy(copyPm ->fphone, pm ->fphone, sizeof(pm ->fphone));
    memcpy(copyPm ->fnickname, pm ->fnickname, sizeof(pm ->fnickname));
    memcpy(copyPm ->tuid, pm ->tuid, sizeof(pm ->tuid));
    memcpy(copyPm ->tuserid, pm ->tuserid, sizeof(pm ->tuserid));
    memcpy(copyPm ->tphone, pm ->tphone, sizeof(pm ->tphone));
    memcpy(copyPm ->user_data, pm ->user_data, sizeof(pm ->user_data));
    
    
}



// softphone回调
- (void) SoftPhoneCallback:(int) ev_type withReason:(int) ev_reason withDescription:(NSString *) strDescription withParam:(const void *) param
{
    [[SDKLogManager instance]saveSDKLogInfo:[NSString stringWithFormat:@"UCSDialViewController ev_type is -->%d ev_reason is --> %d strDescription -->%@",ev_type,ev_reason,strDescription] withDetail:nil];
    
    UXLogInfo(@"UCSDialViewController ev_type is -->%d ev_reason is --> %d",ev_type,ev_reason);
    
    
    __block void *copyParam = NULL;
    
    if (param != NULL) {
        
        /**
         @author WLS, 15-12-02 14:12:34
         
         拷贝组件传过来的参数
         */
        if (ev_type == eUGo_VIDEO_NETWORK_EV) {
            copyParam = malloc(sizeof(ME_video_network_state_t));
            memset(copyParam, 0, sizeof(ME_video_network_state_t));
            memcpy(copyParam, param, sizeof(ME_video_network_state_t));
        } else {
            copyParam = malloc(sizeof(ugo_callev_pm_t));
            [self copyInfoFromParam:param copyParam:copyParam];
        }
        
        
    }
    
    
    dispatch_async(dispatch_get_main_queue(), ^(void)
                   {
                       
                       
                       
                       
                       
                       
                       switch (ev_type)
                       {
                           case eUGo_CALLINCOMING_EV: // 来电
                           {
                               // ev_reason = eUGo_Reason_Success 表明是来电；否则是来电通知
                               if (eUGo_Reason_Success == ev_reason) {
//                                   ugo_callev_pm_t *pm = (ugo_callev_pm_t *)param;
                                   /**
                                    @author WLS, 15-12-02 14:12:43
                                    
                                    使用copy的对象进行数据解析，如果使用之前的传参，有可能会跳出传参的生命周期之后，里面的内容是乱码。
                                    
                                    */
                                   ugo_callev_pm_t *copyPm = (ugo_callev_pm_t *)copyParam;
                                   
                                   
                                   if (copyPm == NULL || (strlen(copyPm->fuid) <= 1&&strlen(copyPm->fuserid) <= 1)) {
                                       UXLogError(@"No UID Call");
                                       return;
                                   }
                                   
                                   NSDictionary *callInfo = [self getcallInfo:copyPm];
                                   
                                   //当手机号为空时传 clientID 否则就传手机号
//                                   NSString *callNum = strlen(copyPm->fphone) == 0?[NSString stringWithUTF8String:copyPm->fuid]:[NSString stringWithUTF8String:copyPm->fphone];
                                   
                                   //支持昵称 add by kucky 20141203
                                   [self readyForRecvIncomingCall:[NSString stringWithUTF8String:copyPm->callid] whithVideoFlag:copyPm->videoflag withCallInfo:callInfo];
                               }
                           }
                               break;
                               
                               
                           case eUGo_CALLDIALING_EV: // 拨打
                           {
                               /*************************************************/
                               // ugo_callev_pm_t 包含 uid videoflag callid
                               NSString *callid;
                               int videoflag = 0;
//                               ugo_callev_pm_t *pm = (ugo_callev_pm_t *)param;
                               /**
                                @author WLS, 15-12-02 14:12:43
                                
                                使用copy的对象进行数据解析，如果使用之前的传参，有可能会跳出传参的生命周期之后，里面的内容是乱码。
                                
                                */
                               ugo_callev_pm_t *copyPm = (ugo_callev_pm_t *)copyParam;
                               if (copyPm == NULL || (strlen(copyPm->fuid) <= 1&&strlen(copyPm->fuserid) <= 1)) {                                   UXLogError(@"No UID Call_DIALING %d",ev_reason);
                                   callid =@"";
                               }else
                               {
                                   callid = [NSString stringWithUTF8String:copyPm->callid];
                                   videoflag = copyPm->videoflag;
                               }
                               
                               
                               if (_sdkLogString) {
                                   NSRange range = [_sdkLogString rangeOfString:@"replaceCallid"];
                                   if (range.location != NSNotFound) {
                                       [_sdkLogString replaceCharactersInRange:range withString:callid];
                                   }
                               }
                               
                               /**
                                @author WLS, 16-01-28 11:01:33
                                
                                只有振铃和接通才不是会话结束，其余情况会话均结束。
                                */
                               self.isEndCall = YES;
                               /*************************************************/
                               switch (ev_reason)
                               {
                                       
                                       
                                       //                                       case eUGo_Reason_CallNotify: //来电通知
                                       //[self OnCallFailed:@"被叫不在线或网络不支持"];
                                       //add by kucky
                                       // ［self onCallProceeding:@"eUGo_CALLDIALING_EV----eUGo_Reason_CallNotify"];
                                       
                                       break;
                                   case eUGo_Reason_Ringing: // 响铃中
                                       
                                       //[[SoftphoneManager instance] playLocalMusic];
                                       // 状态写入数据 add by CW 2016-1-13
                                       [_sdkLogString appendFormat:@"%@:%@\n",[UCSPublicFunc getIPhoneNowTime],@"对方正在响铃，请等待接听"];
                                       
                                       
                                       [self onAlerting:callid withVideoflag:videoflag];// add by baige 2014年10月13日11:39:21
                                       UXLogNotice(@"对方正在响铃，请等待接听");
                                       
                                       self.isEndCall = NO;
                                       break;
                                   case eUGo_Reason_Success: // 呼叫成功
                                       //                                           [self OnCallConnected];
                                       //                                           [_logReport setConnected];
                                       //
                                       //
                                       nowAnswer = (long)[[NSDate date] timeIntervalSince1970];// add by pbf
                                       [self onAnswer:callid];//add by kucky
                                       UXLogNotice(@"呼叫成功");
                                       
                                       // 状态写入数据 add by CW 2016-1-13
                                       [_sdkLogString appendFormat:@"%@:%@\n",[UCSPublicFunc getIPhoneNowTime],@"对方接听，呼叫成功"];
                                       //对方接听之后 ，保存会话状态 add by WLS 20150907
                                       [UserDefaultManager SetLocalDataBoolen:YES key:DATA_STORE_ISTALK];//处于会话状态
                                       
                                       /**
                                        @author WLS, 16-01-08 12:01:39
                                        
                                        通话接通后开始录音
                                        */
                                       [self startRecordWithCallId:callid callee:(strlen(copyPm->tuserid) <= 1 ? [NSString stringWithUTF8String:copyPm->tuid]:[NSString stringWithUTF8String:copyPm->tuserid]) caller:[UserDefaultManager GetclientNumber]];
                                       
                                       self.isEndCall = NO;
                                       break;
                                   case eUGo_Reason_NoBalance: // 余额不足
                                       //                                           _isNoMoney = YES;
                                       [self OnCallFailed:@"402001" andMsg:@"余额不足" withCallId:callid];
                                       
                                       // 状态写入数据 add by CW 2016-1-13
                                       [_sdkLogString appendFormat:@"%@:%@\n",[UCSPublicFunc getIPhoneNowTime],@"余额不足，呼叫失败"];
                                       
                                       UXLogError(@"余额不足");
                                       break;
                                   case eUGo_Reason_NotFind: // 被叫不在线
                                       //                                           if (m_UXinCallType == UXinCallType_Free) {
                                       //                                               [self showFreeToDirectMsg];
                                       //                                           } else {
                                       [self OnCallFailed:@"402002" andMsg:[NSString stringWithFormat:@"被叫不在线[%d]",eUGo_Reason_NotFind] withCallId:callid];//被叫不在线或不存在
                                       
                                       // 状态写入数据 add by CW 2016-1-13
                                       [_sdkLogString appendFormat:@"%@:%@\n",[UCSPublicFunc getIPhoneNowTime],[NSString stringWithFormat:@"被叫不在线[%d]",eUGo_Reason_NotFind]];
                                       
                                       UXLogError(@"被叫不在线");
                                       //                                           }
                                       break;
                                   case eUGo_Reason_UserIdNotExist: // 被叫不存在
                                       //                                           if (m_UXinCallType == UXinCallType_Free) {
                                       //                                               [self showFreeToDirectMsg];
                                       //                                           } else {
                                       [self OnCallFailed:@"402023" andMsg:[NSString stringWithFormat:@"被叫不存在[%d]",eUGo_Reason_UserIdNotExist] withCallId:callid];//被叫不在线或不存在
                                       
                                       // 状态写入数据 add by CW 2016-1-13
                                       [_sdkLogString appendFormat:@"%@:%@\n",[UCSPublicFunc getIPhoneNowTime],[NSString stringWithFormat:@"被叫不存在[%d]",eUGo_Reason_NotFind]];
                                       
                                       UXLogError(@"被叫不存在");
                                       //                                           }
                                       break;
                                   case eUGo_Reason_NoResponse: // 呼叫超时(网络连接问题)
                                       [self OnCallFailed:@"402003" andMsg:@"呼叫超时(网络连接问题)" withCallId:callid];
                                       
                                       // 状态写入数据 add by CW 2016-1-13
                                       [_sdkLogString appendFormat:@"%@:%@\n",[UCSPublicFunc getIPhoneNowTime],@"呼叫超时(网络连接问题)"];
                                       
                                       UXLogError(@"呼叫超时(网络连接问题)");
                                       break;
                                   case eUGo_Reason_Reject: // 被叫拒绝接听
                                       [self OnCallEnded:@"402012" andMsg:@"对方拒绝接听" withCallId:callid];
                                       
                                       // 状态写入数据 add by CW 2016-1-13
                                       [_sdkLogString appendFormat:@"%@:%@\n",[UCSPublicFunc getIPhoneNowTime],@"对方拒绝接听"];
                                       
                                       UXLogNotice(@"对方拒绝接听");
                                       break;
                                   case eUGo_Reason_Busy: // 对方忙(正在通话中)
                                       [self OnCallEnded:@"402013" andMsg:@"对方忙(正在通话中)" withCallId:callid];
                                       
                                       // 状态写入数据 add by CW 2016-1-13
                                       [_sdkLogString appendFormat:@"%@:%@\n",[UCSPublicFunc getIPhoneNowTime],@"对方忙（正在通话中）"];
                                       
                                       UXLogNotice(@"对方忙(正在通话中)");
                                       break;
                                   case eUGo_Reason_Cancel: // 主动取消呼叫
                                       [self OnCallEnded:@"402014" andMsg:@"主动取消呼叫" withCallId:callid];
                                       
                                       // 状态写入数据 add by CW 2016-1-13
                                       [_sdkLogString appendFormat:@"%@:%@\n",[UCSPublicFunc getIPhoneNowTime],@"主动取消呼叫"];
                                       
                                       UXLogNotice(@"主动取消呼叫");
                                       break;
                                   case eUGo_Reason_NotAccept: // 服务不受理(媒体协商失败)
                                       [self OnCallFailed:@"402004" andMsg:@"服务不受理(媒体协商失败)" withCallId:callid];
                                       
                                       // 状态写入数据 add by CW 2016-1-13
                                       [_sdkLogString appendFormat:@"%@:%@\n",[UCSPublicFunc getIPhoneNowTime],@"服务不受理(媒体协商失败)"];
                                       
                                       UXLogError(@"服务不受理(媒体协商失败)");
                                       break;
                                   case eUGo_Reason_Freeze: // 账号已冻结(频繁呼叫/黑名单)
                                       [self OnCallFailed:@"402005" andMsg:@"主叫账号已冻结(频繁呼叫/黑名单)" withCallId:callid];
                                       
                                       // 状态写入数据 add by CW 2016-1-13
                                       [_sdkLogString appendFormat:@"%@:%@\n",[UCSPublicFunc getIPhoneNowTime],@"账号已冻结(频繁呼叫/黑名单)"];
                                       
                                       UXLogError(@"账号已冻结(频繁呼叫/黑名单)");
                                       break;
                                   case eUGo_Reason_Expired: // 账号已过期(长期未使用)
                                       [self OnCallFailed:@"402006" andMsg:@"账号已过期(长期未使用)" withCallId:callid];
                                       
                                       // 状态写入数据 add by CW 2016-1-13
                                       [_sdkLogString appendFormat:@"%@:%@\n",[UCSPublicFunc getIPhoneNowTime],@"账号已过期(长期未使用)"];
                                       
                                       UXLogError(@"账号已过期(长期未使用)");
                                       break;
                                   case eUGo_Reason_UnkownError:
                                       [self OnCallFailed:@"402007" andMsg:[NSString stringWithFormat:@"呼叫错误（%d）", ev_reason] withCallId:callid];
                                       
                                       // 状态写入数据 add by CW 2016-1-13
                                       [_sdkLogString appendFormat:@"%@:%@%d\n",[UCSPublicFunc getIPhoneNowTime],@"呼叫错误:",ev_reason];
                                       
                                       UXLogError([NSString stringWithFormat:@"呼叫错误（%d）", ev_reason]);
                                       break;
                                   case eUGo_Reason_TooShort: // 呼叫号码非法 add by kucky
                                       //new 20140609 19:37 统一，for 黄韜 add by kucky
                                       [self OnCallFailed:@"402008" andMsg:[NSString stringWithFormat:@"被叫号码异常[%d]",eUGo_Reason_TooShort] withCallId:callid];//被叫不在线或不存在
                                       UXLogError(@"被叫号码异常");
                                       //old
                                       //                                           [self OnCallFailed:@"402008"];
                                       //                                           UXLogError(@"呼叫号码非法");
                                       
                                       // 状态写入数据 add by CW 2016-1-13
                                       [_sdkLogString appendFormat:@"%@:%@\n",[UCSPublicFunc getIPhoneNowTime],[NSString stringWithFormat:@"被叫号码异常[%d]",eUGo_Reason_TooShort]];
                                       
                                       break;
                                   case eUGo_Reason_ConnectFaild: // 超时，连接失败 add by kucky
                                       [self OnCallFailed:@"402009" andMsg:@"接通超时" withCallId:callid];
                                       UXLogError(@"接通超时");
                                       
                                       // 状态写入数据 add by CW 2016-1-13
                                       [_sdkLogString appendFormat:@"%@:%@\n",[UCSPublicFunc getIPhoneNowTime],@"呼叫失败（超时）"];
                                       
                                       break;
                                       
                                   case eUGo_Reason_Forbidden: // 呼叫绑定号码 add by kucky
                                       [self OnCallFailed:@"402022" andMsg:@"呼叫绑定号码" withCallId:callid];
                                       
                                       // 状态写入数据 add by CW 2016-1-13
                                       [_sdkLogString appendFormat:@"%@:%@\n",[UCSPublicFunc getIPhoneNowTime],@"呼叫绑定号码"];
                                       
                                       UXLogError(@"呼叫绑定号码）");
                                       break;
                                   case eUGo_Reason_ProxyAuth: // 呼叫绑定号码 add by baige
                                       [self OnCallFailed:@"402041" andMsg:@"服务器鉴权失败" withCallId:callid];
                                       
                                       // 状态写入数据 add by CW 2016-1-13
                                       [_sdkLogString appendFormat:@"%@:%@\n",[UCSPublicFunc getIPhoneNowTime],@"服务器鉴权失败"];
                                       
                                       UXLogError(@"服务器鉴权失败");
                                       break;
                                   case eUGo_Reason_NoAnswer: // 被叫无应答 add by kucky
                                       [self OnCallFailed:@"402050" andMsg:@"被叫无应答" withCallId:callid];
                                       
                                       // 状态写入数据 add by CW 2016-1-13
                                       [_sdkLogString appendFormat:@"%@:%@\n",[UCSPublicFunc getIPhoneNowTime],@"被叫无应答"];
                                       
                                       UXLogNotice(@"被叫无应答");
                                       break;
                                   case eUGo_Reason_HungupRTPTimeout:
                                   case eUGo_Reason_HungupTCPDisconnected: // tcp断开链接，有可能是自己断开，也有可能对方断开。
                                       [self OnCallFailed:@"402051" andMsg:[NSString stringWithFormat:@"网络连接异常(链接断开)[%d]",ev_reason] withCallId:callid];
                                       
                                       // 状态写入数据 add by CW 2016-1-13
                                       [_sdkLogString appendFormat:@"%@:%@\n",[UCSPublicFunc getIPhoneNowTime],[NSString stringWithFormat:@"网络连接异常(链接断开)[%d]",ev_reason]];
                                       
                                       UXLogNotice(@"链接断开(自己断开或者对方断开)");
                                       break;
                                   case eUGo_Reason_BlackList:// 黑名单 add by WLS 20151023
                                       [self OnCallFailed:@"402052" andMsg:@"呼叫失败（线路频繁呼叫已被列入黑名单）" withCallId:callid];
                                       [_sdkLogString appendFormat:@"%@:%@\n",[UCSPublicFunc getIPhoneNowTime],@"呼叫失败（线路频繁呼叫已被列入黑名单）"];
                                       UXLogAlert(@"呼叫失败（线路频繁呼叫已被列入黑名单）");
                                       break;
                                   case eUGo_Reason_UnReachable:// 消息路由不可及 add by WLS 20160429
                                       [self OnCallFailed:@"402053" andMsg:@"消息不可及" withCallId:callid];
                                       [_sdkLogString appendFormat:@"%@:%@\n",[UCSPublicFunc getIPhoneNowTime],@"消息不可及"];
                                       UXLogAlert(@"消息不可及");
                                       break;
//                                   case eUGo_Reason_NotifyPeerNotFind:// 被叫Uid不存在，转直拨，智能呼叫中使用
//                                   case eUGo_Reason_NotifyPeerOffLine:// 被叫不在线，转直拨，智能呼叫中使用
//                                   case eUGo_Reason_NotifyPeerTimeout:// 免费呼叫超时，转直拨，智能呼叫中使用
//                                       break;
                                   default:  //10000-20000错误码透传
                                       
                                       
                                       //10000-20000错误码透传
                                       if (ev_reason>=10000&&ev_reason<=20000) {
                                           NSString *customErr = [NSString stringWithFormat:@"%d",ev_reason];
                                           
                                           // 状态写入数据 add by CW 2016-1-13
                                           [_sdkLogString appendFormat:@"%@:%@\n",[UCSPublicFunc getIPhoneNowTime],@"透传自定义错误码"];
                                           
                                           [self OnCallFailed:customErr andMsg:@"透传自定义错误码" withCallId:callid];
                                       }
                                       
                                       break;
                                       
                                       
                                       
                               }
                               /**
                                @author WLS, 16-03-14 10:03:52
                                
                                只有会话结束了，才要上传会话日志
                                */
                               if (_callLogDetail && self.isEndCall) {
                                   [self uploadConversationLogWithDetail:_callLogDetail];
                               }
                           }
                               break;
                               
                               //                            case eUGo_CALLINCOMING_EV: // 来电
                               //                             {
                               //                                   // ev_reason = eUGo_Reason_Success 表明是来电；否则是来电通知
                               //                                   if (eUGo_Reason_Success != ev_reason) {
                               //                                       UXLogNotice(@"来电通知");
                               //                                   } else {
                               //                                       UXLogNotice(@"未处理异常");
                               //                                   }
                               //                             }
                               //                            break;
                               
                           case eUGo_CALLHUNGUP_EV: // 挂断(会话已经建立)
                           {
                               /*************************************************/
                               // ugo_callev_pm_t 包含 uid videoflag callid
                               NSString *callid;
//                               ugo_callev_pm_t *pm = (ugo_callev_pm_t *)param;
                               /**
                                @author WLS, 15-12-02 14:12:43
                                
                                使用copy的对象进行数据解析，如果使用之前的传参，有可能会跳出传参的生命周期之后，里面的内容是乱码。
                                
                                */
                               ugo_callev_pm_t *copyPm = (ugo_callev_pm_t *)copyParam;
                               if (copyPm == NULL || (strlen(copyPm->fuid) <= 1&&strlen(copyPm->fuserid) <= 1)) {                                   UXLogError(@"No UID Call_HUNGUP");
                                   callid =@"";
                               }else
                               {
                                   callid = [NSString stringWithUTF8String:copyPm->callid];
                               }
                               
                               
                               
                               //                                   //通话状态挂断 重置一系列参数 add by WLS 20151009
                               //                                   [self resetParameter];
                               
                               
                               /**
                                @author WLS, 16-01-08 12:01:47
                                
                                会挂挂断的时候需要停止录音
                                */
                               [self stopRecord];
                               /**
                                @author WLS, 16-01-28 11:01:33
                                
                                会话结束，将会话结束的标示置为YES
                                */
                               self.isEndCall = YES;
                               /*************************************************/
                               switch (ev_reason)
                               {
                                       
                                       
                                       
                                       
                                   case eUGo_Reason_Success: // 主叫挂断
                                   case eUGo_Reason_Cancel:
                                   case eUGo_Reason_HungupMyself:
                                       [self OnCallEnded:@"402015" andMsg:@"已挂机" withCallId:callid];
                                       UXLogNotice(@"自己已挂机");
                                       //add by WSL 20150825 当下次偶现无法挂断的时候，查看日志是否已经收到挂断信息
                                       [[SDKLogManager instance] saveSDKLogInfo:@"主动调用组件Hungup通话" withDetail:[NSString stringWithFormat:@"已挂机"]];
                                       
                                       // 状态写入数据 add by CW 2016-1-13
                                       [_sdkLogString appendFormat:@"%@:%@\n",[UCSPublicFunc getIPhoneNowTime],@"主动调用组件挂断电话"];
                                       //add by pbf
                                       nowHungUp = (long)[[NSDate date] timeIntervalSince1970];
                                       [self reportVoiceQulity:nowAnswer andNowHungUp:nowHungUp];
                                       nowAnswer=0;
                                       nowHungUp=0;
                                       
                                       break;
                                   case eUGo_Reason_HungupPeer: // 被叫挂断
                                       [self OnCallEnded:@"402016" andMsg:@"对方已挂机" withCallId:callid];
                                       UXLogNotice(@"对方已挂机");
                                       
                                       // 状态写入数据 add by CW 2016-1-13
                                       [_sdkLogString appendFormat:@"%@:%@\n",[UCSPublicFunc getIPhoneNowTime],@"对方挂断电话"];
                                       
                                       //add by WSL 20150825 当下次偶现无法挂断的时候，查看日志是否已经收到挂断信息
                                       [[SDKLogManager instance] saveSDKLogInfo:@"主动调用组件Hungup通话" withDetail:[NSString stringWithFormat:@"对方已挂机"]];
                                       
                                       //add by pbf
                                       nowHungUp = (long)[[NSDate date] timeIntervalSince1970];
                                       [self reportVoiceQulity:nowAnswer andNowHungUp:nowHungUp];
                                       nowAnswer=0;
                                       nowHungUp=0;
                                       
                                       break;
                                   case eUGo_Reason_RtppTimeOut: // RTP超时挂断
                                       [self OnCallEnded:@"402017" andMsg:@"语音数据接收超时" withCallId:callid];
                                       UXLogNotice(@"语音数据接收超时");
                                       
                                       // 状态写入数据 add by CW 2016-1-13
                                       [_sdkLogString appendFormat:@"%@:%@\n",[UCSPublicFunc getIPhoneNowTime],@"语音数据接收超时挂断电话"];
                                       
                                       break;
                                   case eUGo_Reason_NoBalance:// 余额不足
                                       [self OnCallEnded:@"402018" andMsg:@"余额不足" withCallId:callid];
                                       UXLogNotice(@"余额不足");
                                       
                                       // 状态写入数据 add by CW 2016-1-13
                                       [_sdkLogString appendFormat:@"%@:%@\n",[UCSPublicFunc getIPhoneNowTime],@"余额不足挂断电话"];
                                       
                                       break;
                                   case eUGo_Reason_MsgTimeOut:
                                       [self OnCallEnded:@"402019" andMsg:@"呼叫信令超时" withCallId:callid];
                                       UXLogNotice(@"呼叫信令超时");
                                       
                                       // 状态写入数据 add by CW 2016-1-13
                                       [_sdkLogString appendFormat:@"%@:%@\n",[UCSPublicFunc getIPhoneNowTime],@"呼叫超时"];
                                       
                                       break;
                                   case eUGo_Reason_Forbidden:
                                       [self OnCallEnded:@"402020" andMsg:@"不能拨打自己" withCallId:callid];
                                       UXLogNotice(@"不能拨打自己");
                                       
                                       // 状态写入数据 add by CW 2016-1-13
                                       [_sdkLogString appendFormat:@"%@:%@\n",[UCSPublicFunc getIPhoneNowTime],@"不能拨打自己"];
                                       
                                       break;
                                   case eUGo_Reason_TooShort: // 呼叫号码非法 add by kucky
                                       //new 20140609 19:37 统一，for 黄韜 add by kucky
                                       [self OnCallFailed:@"402008" andMsg:[NSString stringWithFormat:@"被叫号码异常[%d]",eUGo_Reason_TooShort] withCallId:callid];//被叫不在线或不存在
                                       UXLogError(@"被叫号码异常");
                                       
                                       // 状态写入数据 add by CW 2016-1-13
                                       [_sdkLogString appendFormat:@"%@:%@\n",[UCSPublicFunc getIPhoneNowTime],[NSString stringWithFormat:@"被叫号码异常[%d]",eUGo_Reason_TooShort]];
                                       
                                       break;
                                   case eUGo_Reason_HungupTCPDisconnected:
                                   case eUGo_Reason_HungupRTPTimeout:
                                       // tcp断开链接，有可能是自己断开，也有可能对方断开。
                                       [self OnCallFailed:@"402051" andMsg:[NSString stringWithFormat:@"网络连接异常(链接断开)[%d]",ev_reason] withCallId:callid];
                                       UXLogNotice(@"链接断开(自己断开或者对方断开)");
                                       
                                       // 状态写入数据 add by CW 2016-1-13
                                       [_sdkLogString appendFormat:@"%@:%@\n",[UCSPublicFunc getIPhoneNowTime],[NSString stringWithFormat:@"网络连接异常(链接断开)[%d]",ev_reason]];
                                       
                                       break;
                                   default:
                                       
                                       [self OnCallEnded:@"402021" andMsg:[NSString stringWithFormat:@"%@[%d]",strDescription,ev_reason] withCallId:callid];
                                       UXLogNotice(strDescription);
                                       
                                       // 状态写入数据 add by CW 2016-1-13
                                       [_sdkLogString appendFormat:@"%@:%@\n",[UCSPublicFunc getIPhoneNowTime],[NSString stringWithFormat:@"%@[%d]",strDescription,ev_reason]];
                                       break;
                               }
                               
                               /**
                                @author WLS, 16-03-14 10:03:52
                                
                                只有会话结束了，才要上传会话日志
                                */
                               if (_callLogDetail && self.isEndCall) {
                                   [self uploadConversationLogWithDetail:_callLogDetail];
                               }
                           }
                               break;
                               
                           case eUGo_CALLANSWER_EV: // 接听
                           {
                               /*************************************************/
                               // ugo_callev_pm_t 包含 uid videoflag callid
                               NSString *callid;
//                               ugo_callev_pm_t *pm = (ugo_callev_pm_t *)param;
                               /**
                                @author WLS, 15-12-02 14:12:43
                                
                                使用copy的对象进行数据解析，如果使用之前的传参，有可能会跳出传参的生命周期之后，里面的内容是乱码。
                                
                                */
                               ugo_callev_pm_t *copyPm = (ugo_callev_pm_t *)copyParam;
                               if (copyPm == NULL || (strlen(copyPm->fuid) <= 1&&strlen(copyPm->fuserid) <= 1)) {                                   UXLogError(@"No UID Call_ANSWER");
                                   callid =@"";
                               }else
                               {
                                   callid = [NSString stringWithUTF8String:copyPm->callid];
                               }
                               
                               /*************************************************/
                               
                               switch (ev_reason)
                               {
                                       
                                       
                                       
                                   case eUGo_Reason_Success: // 接听成功
                                       //                                           [self OnCallConnected];
                                       nowAnswer = (long)[[NSDate date] timeIntervalSince1970];//add by pbf
                                       [self onAnswer:callid];//add by kucky
                                       UXLogNotice(@"接听成功");
                                       // 状态写入数据 add by CW 2016-1-13
                                       [_sdkLogString appendFormat:@"%@:%@\n",[UCSPublicFunc getIPhoneNowTime],@"接听电话成功"];
                                       
                                       //接听电话，保存会话状态 add by WLS 20150907
                                       [UserDefaultManager SetLocalDataBoolen:YES key:DATA_STORE_ISTALK];//处于会话状态
                                       
                                       
                                       /**
                                        @author WLS, 16-01-08 12:01:39
                                        
                                        通话接通后开始录音
                                        */
                                       [self startRecordWithCallId:_incomingCallid callee:[UserDefaultManager GetclientNumber] caller:_icomingCaller];
                                       
                                       
                                       
                                       break;
                                   default:
                                       [self OnCallFailed:@"402010" andMsg:@"会话已不存在" withCallId:callid];
                                       UXLogError(@"会话已不存在");
                                       
                                       // 状态写入数据 add by CW 2016-1-13
                                       [_sdkLogString appendFormat:@"%@:%@\n",[UCSPublicFunc getIPhoneNowTime],@"会话已经不存在"];
                                       
                                       break;
                               }
                           }
                               break;
                           case eUGo_UPSINGLEPASS_EV: {
                               //                                   [_logReport setSinglePass];
                           }
                               break;
                           case eUGo_DNSINGLEPASS_EV: {
                               //                                   [_logReport setSinglePass];
                           }
                               break;
                           case eUGo_TCPTRANSPORT_EV:
                           {
                               
                               /*************************************************/
                               // ugo_callev_pm_t 包含 uid videoflag callid
                               NSString *callid;
//                               ugo_callev_pm_t *pm = (ugo_callev_pm_t *)param;
                               /**
                                @author WLS, 15-12-02 14:12:43
                                
                                使用copy的对象进行数据解析，如果使用之前的传参，有可能会跳出传参的生命周期之后，里面的内容是乱码。
                                
                                */
                               ugo_callev_pm_t *copyPm = (ugo_callev_pm_t *)copyParam;
                               if (copyPm == NULL || (strlen(copyPm->fuid) <= 1&&strlen(copyPm->fuserid) <= 1)) {                                   UXLogError(@"No UID Call_TRANSPORT");
                                   callid =@"";
                               }else
                               {
                                   callid = [NSString stringWithUTF8String:copyPm->callid];
                               }
                               
                               /*************************************************/
                               switch (ev_reason)
                               {
                                       
                                       
                                       
                                       
                                   case eUGo_TCP_DISCONNECTED:
                                       [self OnCallFailed:@"402011" andMsg:@"无网络" withCallId:callid];
                                       UXLogError(@"无网络");
                                       break;
                                       
                                   default:
                                       break;
                               }
                           }
                               break;
                               //************网络状态上报************
                           case eUGo_NETWORK_EV:
                           {
                               /*************************************************/
                               // ugo_callev_pm_t 包含 uid videoflag callid
                               NSString *callid;
//                               ugo_callev_pm_t *pm = (ugo_callev_pm_t *)param;
                               /**
                                @author WLS, 15-12-02 14:12:43
                                
                                使用copy的对象进行数据解析，如果使用之前的传参，有可能会跳出传参的生命周期之后，里面的内容是乱码。
                                
                                */
                               ugo_callev_pm_t *copyPm = (ugo_callev_pm_t *)copyParam;
                               if (copyPm == NULL || (strlen(copyPm->fuid) <= 1&&strlen(copyPm->fuserid) <= 1)){
//                                   UXLogError(@"No UID Call_NETWORK_EV");
                                   callid =@"";
                               }else
                               {
                                   callid = [NSString stringWithUTF8String:copyPm->callid];
                               }
                               
                               //处于视频状态且对端处于接收状态 本端视频处于发送状态 这种情况会过滤网络状态上报 add by WLS 20151009
                               if ([UserDefaultManager GetLocalDataBoolen:DATA_STORE_ISVIDEO] &&
                                   ([UserDefaultManager GetLocalDataInt:DATA_STORE_LOCALCAMERAMODE]==CAMERA_SEND ||
                                    [UserDefaultManager GetLocalDataInt:DATA_STORE_REMOTECAMERAMODE]== CAMERA_RECEIVE)) {
                                       
                                   }else{
                                       /*************************************************/
                                       
                                       [self onNetWorkDetail:strDescription]; //回调通话网络状态 add by wls 2015.10.08
                                       //add by wenqinglin
                                       
                                       
                                       switch (ev_reason)
                                       {
                                           case eME_REASON_NETWORK_WELL: //优秀
                                               UXLogNotice(@"网络状态-->优秀");
                                               //[self onNetWorkState:KCTNetwork_Well networkDes:@""]; //回调通话网络状态 add by wls 2015.10.08
                                               break;
                                           case eME_REASON_NETWORK_GENERAL: //一般
                                               UXLogNotice(@"网络状态-->一般");
                                               //[self onNetWorkState:KCTNetwork_General networkDes:@""]; //回调通话网络状态 add by pbf 2015.09.14
                                               break;
                                               
                                           case eME_REASON_NETWORK_NICE: //良好
                                               UXLogNotice(@"网络状态-->良好");
                                               //[self onNetWorkState:KCTNetwork_Nice networkDes:@""]; // add by pbf 2015.09.14
                                               break;
                                               
                                           case eME_REASON_NETWORK_BAD: //很差
                                               UXLogNotice(@"网络状态-->很差");
                                               //[self onNetWorkState:KCTNetwork_Bad networkDes:@""]; // add by pbf 2015.09.14
                                               //                                       [self OnCallFailed:@"402040" andMsg:@"网络状态:较差" withCallId:callid];
                                               break;
                                               
                                           default:
                                               break;
                                       }
                                   }
                           }
                               
                               break;
                          case eUGo_VIDEO_NETWORK_EV:
                           {
                               if(ev_reason == eUGo_Reason_Success && copyParam!=NULL)
                               {
                                   char strNet[2048] = { 0 };
                                   char *tmpBuf = NULL;
                                   ME_network_state_t* netStateT = (ME_network_state_t*)copyParam;
                                   switch (netStateT->net_state) {
                                       case eME_REASON_NETWORK_NICE:
                                           tmpBuf = "nice";
                                           break;
                                       case eME_REASON_NETWORK_WELL:
                                           tmpBuf = "well";
                                           break;
                                       case eME_REASON_NETWORK_GENERAL:
                                           tmpBuf = "general";
                                           break;
                                       case eME_REASON_NETWORK_POOR:
                                           tmpBuf = "poor";
                                           break;
                                       case eME_REASON_NETWORK_BAD:
                                           tmpBuf = "bad";
                                           break;
                                       default:
                                           tmpBuf = "unknown";
                                           break;
                                   }
                                   ME_video_network_state_t* netState = (ME_video_network_state_t*)copyParam;
                                   sprintf(strNet, "vie state: %s, \n ice: %d, rtt: %d, \n lost: %d(s) %d(r)," \
                                           " \n rate: %d(s) %d(r), \n res: %dx%d(s) %dx%d(r)," \
                                           " \n frame: %d(s) %d(r) \n pt: %d(s) %d(r) \n codec: %s(s) %s(r) \n RelayCnt: %d \n",
                                           tmpBuf, netState->ice, netState->rttMs,
                                           netState->uplinkLost, netState->downlinkLost,
                                           netState->bitrate_bps,  netState->rec_bitrate,
                                           netState->width, netState->height,
                                           netState->decode_width, netState->decode_height,
                                           netState->target_fps, netState->decode_fps,
                                           netState->EncoderPt, netState->DecoderPt,
                                           netState->encCodec, netState->decCodec, netState->rtPOT_nCount);
                                   char rtPOT[256] = { 0 };
                                   for (int i = 0; i < netState->rtPOT_nCount; i++) {
                                       sprintf(rtPOT, " Relay_%d: %d(s) %d(r) \n Flow_a_%d: %dKB(s) %dKB(r) \n Flow_v_%d: %dKB(s) %dKB(r) \n", i, netState->rtPOT_SendIP[i], netState->rtPOT_RecvIP[i], i,netState->rtPOT_SendValue_a[i], netState->rtPOT_RecvValue_a[i],i, netState->rtPOT_SendValue_v[i], netState->rtPOT_RecvValue_v[i]);
                                       strcat(strNet, rtPOT);
                                   }
                                   //printf("--encCodec: %s-----decCodec: %s---\n",netState->encCodec,netState->decCodec);
                                   NSString *strState = [NSString stringWithUTF8String:strNet];
                                   [self onNetWorkState:KCTNetwork_Bad networkDes:strState];
                               }
                               
                           }
                               break;
                               //**************视频模式状态上报(只发送 只接收 正常)**************** add by WLS 20151008
                           case eUGo_REMOTE_VIDEO_STATE_NOTIFY_EV:
                           {
                               /*************************************************/
                               // ugo_callev_pm_t 包含 uid videoflag callid
                               NSString *callid;
//                               ugo_callev_pm_t *pm = (ugo_callev_pm_t *)param;
                               /**
                                @author WLS, 15-12-02 14:12:43
                                
                                使用copy的对象进行数据解析，如果使用之前的传参，有可能会跳出传参的生命周期之后，里面的内容是乱码。
                                
                                */
                               ugo_callev_pm_t *copyPm = (ugo_callev_pm_t *)copyParam;
                               if (copyPm == NULL || (strlen(copyPm->fuid) <= 1&&strlen(copyPm->fuserid) <= 1)) {
//                                   UXLogError(@"No UID Call_NETWORK_EV");
                                   callid =@"";
                               }else
                               {
                                   callid = [NSString stringWithUTF8String:copyPm->callid];
                               }
                               
                               
                               
                               
                               switch (ev_reason) {
                                       
                                   case 11:
                                   {
                                       [self onRemoteCameraMode:CAMERA_SEND]; // add by WLS 2015.10.09
                                       //发送模式
                                       [UserDefaultManager SetLocalDataInt:CAMERA_SEND key:DATA_STORE_REMOTECAMERAMODE];
                                       
                                   }
                                       break;
                                   case 20:
                                   {
                                       [self onRemoteCameraMode:CAMERA_RECEIVE]; // add by WLS 2015.10.09
                                       //接收模式
                                       [UserDefaultManager SetLocalDataInt:CAMERA_RECEIVE key:DATA_STORE_REMOTECAMERAMODE];
                                   }
                                       break;
                                   case 31:
                                   {
                                       [self onRemoteCameraMode:CAMERA_NORMAL]; // add by WLS 2015.10.09
                                       //正常模式
                                       [UserDefaultManager SetLocalDataInt:CAMERA_NORMAL key:DATA_STORE_REMOTECAMERAMODE];
                                   }
                                       break;
                                   default:
                                       break;
                               }
                               
                               
                               
                           }
                               break;
                               
                           case eUGo_GETDTMF_EV: // 当收到DTMF信号时触发
                           {
                               /*************************************************/
                               
                               NSString *strDTMFValue = [NSString stringWithFormat:@"%d",ev_reason];
                               
                               [self onDTMF:strDTMFValue];
                               NSLog(@"strDTMFValue--->%@",strDTMFValue);
                               
                               /*************************************************/
                               
                           }
                               
                               
                               break;
                               
                               
                               
                               
                           default:
                               
                               
                               break;
                               
                               
                               
                       }
                       
                       
                       /**
                        @author WLS, 15-12-02 15:12:36
                        
                        如果拷贝数据的指针不为空， 则释放掉
                        */
                       if (copyParam !=NULL) {
                           free(copyParam);
                       }
                       

                       
                   });
}

//重置参数 add by WLS 20151026
- (void)resetParameter{
    //挂断电话，保存会话状态 add by WLS 20150907
    [UserDefaultManager SetLocalDataBoolen:NO key:DATA_STORE_ISTALK];//不处于会话状态
    //重置对方视频模式参数 正常模式
    [UserDefaultManager SetLocalDataInt:CAMERA_NORMAL key:DATA_STORE_REMOTECAMERAMODE];
    //重置本端视频模式参数 正常模式
    [UserDefaultManager SetLocalDataInt:CAMERA_NORMAL key:DATA_STORE_LOCALCAMERAMODE];
    
    //拨打电话之前,将是否点击静音按钮置为no,刚建立会话点击静音按钮是不会生效的  add by WLS 20150907
    [UserDefaultManager SetLocalDataBoolen:NO key:DATA_STORE_ISCMUTE];
    
    //拨打电话的时候先设置不是视频通话 add by WLS 20150907
    [UserDefaultManager SetLocalDataBoolen:NO key:DATA_STORE_ISVIDEO];
    
    
    /**
     @author WLS, 16-01-08 12:01:26
     
     先需要录音的功能关闭，且如果处于录音状态下，则关闭
     */
    self.needRecord = NO;
    [self stopRecord];
    
}


// 通话结束
- (void) OnCallEnded:(NSString *) strReason andMsg:(NSString *)msg withCallId:(NSString*)callid
{
    //    [self stopPlaySound];
//    [[SoftphoneManager instance] setLoudSpeaker:NO];
//    [m_CallingTimer invalidate];
//    m_CallingTimer = nil;
//    m_bCalling = NO;
//    
//    //    if (m_CallTimeCount)
//    ////        [self SetCallStatus:[NSString stringWithFormat:@"通话结束 %@", [self TimerString]]];
//    //    else
//    [self SetCallStatus:@"通话结束"];
//    
//    //    [self ShowCallingProcessingMsg:strReason];
    
    [[SDKLogManager instance]saveSDKLogInfo:@"通话结束" withDetail:[NSString stringWithFormat:@"callid-->%@  strReason-->%@  msg-->%@",callid,strReason,msg]];

    
    KCTReason *reason = [[KCTReason alloc]init];
    reason.reason = [strReason integerValue];
    reason.msg =  msg;

    [self onHangUp:callid withReason:reason];//add by kucky
    
    //呼叫被释放回调上报 add by kucky 20141014
    [[UCSStatiRequestManager instance] reportErrorCodeInfoForStati:@"onHangUp:" withinterfaceDesc:@"呼叫被释放" withErrorCode:[NSString stringWithFormat:@"%@",strReason]withErrorDesc:msg withClientNumber:[UserDefaultManager GetclientNumber]];
 
    [reason release];
}
//
// 呼叫失败
// strReason : 失败原因
- (void) OnCallFailed:(NSString *) strReason andMsg:(NSString *)msg withCallId:(NSString*)callid
{
//    [m_CallingTimer invalidate];
//    m_CallingTimer = nil;
//    m_bCalling = NO;
//    m_CallTimeLabel.text = @"";
    
    //    [self stopPlaySound];
    //    [self startCallRequestTimer];
    //    [self SetCallStatus:@"呼叫失败"];
    //    [self ShowCallingProcessingMsg:strReason];.
    
    [[SDKLogManager instance]saveSDKLogInfo:@"呼叫失败" withDetail:[NSString stringWithFormat:@"callid-->%@  strReason-->%@  msg-->%@",callid,strReason,msg]];
    
    KCTReason *reason = [[KCTReason alloc]init];
    reason.reason = [strReason integerValue];
    reason.msg =  msg;
    [self onDialFailed:callid withReason:reason];//add by kucky
    UXLogNotice(@"呼叫失败 %@", strReason);
    [self endCall];
    
      //呼叫失败回调上报 add by kucky 20141014
    [[UCSStatiRequestManager instance] reportErrorCodeInfoForStati:@"onDialFailed:" withinterfaceDesc:@"呼叫失败" withErrorCode:[NSString stringWithFormat:@"%@",strReason]withErrorDesc:msg withClientNumber:[UserDefaultManager GetclientNumber]];
    
    [reason release];
}

// 启动关闭窗口
- (void) endCall
{
//    if (_isEnding) {
//        return;
//    }
//    _isEnding = YES;
//    // set the SoftphoneManager delegate
//    [[SoftphoneManager instance] setDelegate:[DialViewController share]];
//    g_ucscallingview_instance = nil;
//    
//    [self performSelectorOnMainThread:@selector(_endCall) withObject:nil waitUntilDone:YES];
}



//------------------------------------------------代理回调函数------------------------------------------------//
//获取当电通话Callid，前提是收到APNS推送消息后初始化callid-------->暂时删除
- (NSString *)getCurrentCallid
{
    NSString *callid =[[[SoftphoneManager instance]getCurrentCallid]retain];
    
    return [callid autorelease];
}


//接到呼叫
-(void)onIncomingCallReceived:(NSString *)callid withcalltype:(KCTCallTypeEnum)calltype withcallerNumber:(NSString *)uid
{
    // 收到voip呼叫，应用接到这个事件后，需要提醒用户来电或者根据自己短业务逻辑拒绝来电。
    if (nil != _mucsdelegate && [_mucsdelegate respondsToSelector:@selector(onIncomingCall:withcalltype:withcallerNumber:)])
    {
        
        //        [_mucsdelegate performSelector:@selector(onIncomingCallReceived:withcalltype:withcallerNumber:) withObject:uid withObject:uid ];
                ((id(*)(id,SEL,NSString *,KCTCallTypeEnum,NSString*))objc_msgSend)(_mucsdelegate, @selector(onIncomingCall:withcalltype:withcallerNumber:), callid,calltype,uid);
        
    }
}

//接到呼叫(支持昵称)
-(void)onIncomingCallReceived:(NSString *)callid withcalltype:(KCTCallTypeEnum)calltype withcallerinfo:(NSDictionary *)callinfo
{
    // 收到voip呼叫，应用接到这个事件后，需要提醒用户来电或者根据自己短业务逻辑拒绝来电。
    if (nil != _mucsdelegate && [_mucsdelegate respondsToSelector:@selector(onIncomingCall:withcalltype:withcallerinfo:)])
    {
        
        //        [_mucsdelegate performSelector:@selector(onIncomingCallReceived:withcalltype:withcallerNumber:) withObject:uid withObject:uid ];
        
        ((id(*)(id,SEL,NSString*,KCTCallTypeEnum,NSDictionary*))objc_msgSend)(_mucsdelegate, @selector(onIncomingCall:withcalltype:withcallerinfo:), callid,calltype,callinfo);
        
    }
}


//呼叫振铃中
- (void) onAlerting:(NSString*)callid withVideoflag:(int)videoflag;
{
    
    
    if (nil != _mucsdelegate && [_mucsdelegate respondsToSelector:@selector(onAlerting:)])
    {
        
        [_mucsdelegate onAlerting:callid];
       
    }
    else
    {
        if (nil != _mucsdelegate && [_mucsdelegate respondsToSelector:@selector(onAlerting:withVideoflag:)])
        {
            
            [_mucsdelegate onAlerting:callid withVideoflag:videoflag];
            
        }
    }
    
    //振铃后开始拨号计时。
    [self CallTimeOutCheck];
    
    
    
}


//被叫接听
-(void) onAnswer:(NSString*)callid;
{
    //
    if (nil != _mucsdelegate && [_mucsdelegate respondsToSelector:@selector(onAnswer:)])
    {
        
        [_mucsdelegate performSelector:@selector(onAnswer:) withObject:callid];

    }
    //除了振铃回调外，其他回调都要进行定时器释放。
    [self releaseCallNSTimer];
}
//呼叫失败需要细分
- (void) onDialFailed:(NSString*)callid  withReason:(KCTReason *) reason
{
    if (nil != _mucsdelegate && [_mucsdelegate respondsToSelector:@selector(onDialFailed:withReason:)])
    {
        
        [_mucsdelegate performSelector:@selector(onDialFailed:withReason:) withObject:callid withObject:reason];

    }
    
    //除了振铃回调外，其他回调都要进行定时器释放。
    [self releaseCallNSTimer];
}
//呼叫被释放
- (void) onHangUp:(NSString*)callid withReason:(KCTReason *)reason
{
    if (nil != _mucsdelegate && [_mucsdelegate respondsToSelector:@selector(onHangUp:withReason:)])
    {
        
        [_mucsdelegate performSelector:@selector(onHangUp:withReason:)  withObject:callid withObject:reason];

        
    }
    
    //除了振铃回调外，其他回调都要进行定时器释放。
    [self releaseCallNSTimer];
}
//回拨结果回调
- (void)onCallBackWithReason:(KCTReason*)reason;//需要更新具体实现
{
 
    if (nil != _mucsdelegate && [_mucsdelegate respondsToSelector:@selector(onCallBackWithReason:)])
    {
        
        [_mucsdelegate performSelector:@selector(onCallBackWithReason:) withObject:reason];
        
        
    }
        //上报回拨数据结果。add by kucky 20141014
       [[UCSStatiRequestManager instance] reportErrorCodeInfoForStati:@"onCallBackWithReason:" withinterfaceDesc:@"回拨结果" withErrorCode:[NSString stringWithFormat:@"%d",reason.reason]withErrorDesc:reason.msg withClientNumber:[UserDefaultManager GetclientNumber]];
    
}

// 语音质量WLS 20150921
- (void)onNetWorkState:(KCTNetworkState)networkState networkDes:(NSString *)networkDes
{
    if (nil != _mucsdelegate && [_mucsdelegate respondsToSelector:@selector(onNetWorkState: networkDes:)]) {
        //        [_mucsdelegate performSelector:@selector(onNetWorkState:) withObject:networkState];
        ((id(*)(id,SEL,int,NSString*))objc_msgSend)(_mucsdelegate, @selector(onNetWorkState:networkDes:), networkState,networkDes);
    }
}
// 语音质量原因上报WLS 2016029
- (void)onNetWorkDetail:(NSString *)networkDetail
{
    
    /**
     @author WLS, 16-01-29 16:01:34
     
     ice = 1 net state is good for video, rttMs=0, uplink lostrate = -1, dnlink lostrate = -1
     ice = 1时为rtpp通道，0时才是p2p通道
     */
    if (nil != _mucsdelegate && [_mucsdelegate respondsToSelector:@selector(onNetWorkDetail:)]) {
        //        [_mucsdelegate performSelector:@selector(onNetWorkState:) withObject:networkState];
        
        
        /**
         @author WLS, 16-02-17 15:02:22
         
         拼接参数
         result : cps返回的结果 0：表示无策略参数  1：表示有策略权限参数，但无驱动适配参数  2：表示有测试略权限参数，也有驱动适配参数
         iceenable : p2p探测使能
         audiofec : 音频FEC使能
         logreport : 日志上报使能
         autoadapter : 驱动自动适配
         vqmenable : 语音质量监控
         prtpenable : rtp压缩使能
         vp8enable : Vp8使能
         h264enable : H264使能
         startbitrate : 其实码率  cabel:150 ，wifi:120 ，3g:100 ，4g:100
         minbitrate : 最低码率  cabel:100 ，wifi:90 ，3g:80 ，4g:80
         maxbitrate : 最大码率  cabel:250 ，wifi:250 ，3g:150 ，4g:150
         firstpt : 默认视频codec Vp8:122，h264:121
         添加平台预设值
         presets
         bitrates
         fps
         */

        NSString * result = [NSString stringWithFormat:@"result : %@",[UserDefaultManager GetKeyChain:DATA_STORE_CPS_RESULT]];
        NSString * iceenable = [NSString stringWithFormat:@"iceenable : %@",[UserDefaultManager GetKeyChain:DATA_STORE_CPS_ICEENABLE]];
        NSString * audiofec = [NSString stringWithFormat:@"audiofec : %@",[UserDefaultManager GetKeyChain:DATA_STORE_CPS_AUDIOFEC]];
        NSString * logreport = [NSString stringWithFormat:@"logreport : %@",[UserDefaultManager GetKeyChain:DATA_STORE_CPS_LOGREPORT]];
        NSString * autoadapter = [NSString stringWithFormat:@"autoadapter : %@",[UserDefaultManager GetKeyChain:DATA_STORE_CPS_AUTOADAPTER]];
        NSString * vqmenable = [NSString stringWithFormat:@"vqmenable : %@",[UserDefaultManager GetKeyChain:DATA_STORE_CPS_VQMENABLE]];
        NSString * prtpenable = [NSString stringWithFormat:@"prtpenable : %@",[UserDefaultManager GetKeyChain:DATA_STORE_CPS_PRTPENABLE]];
        NSString * vp8enable = [NSString stringWithFormat:@"vp8enable : %@",[UserDefaultManager GetKeyChain:DATA_STORE_CPS_VP8ENABLE]];
        NSString * h264enable = [NSString stringWithFormat:@"h264enable : %@",[UserDefaultManager GetKeyChain:DATA_STORE_CPS_H264ENABLE]];
        NSString * startbitrate = [NSString stringWithFormat:@"startbitrate : %@",[UserDefaultManager GetKeyChain:DATA_STORE_CPS_STARTBITRATE]];
        NSString * minbitrate = [NSString stringWithFormat:@"minbitrate : %@",[UserDefaultManager GetKeyChain:DATA_STORE_CPS_MINBITRATE]];
        NSString * maxbitrate = [NSString stringWithFormat:@"maxbitrate : %@",[UserDefaultManager GetKeyChain:DATA_STORE_CPS_MAXBITRATE]];
        NSString * firstpt = [NSString stringWithFormat:@"firstpt : %@",[UserDefaultManager GetKeyChain:DATA_STORE_CPS_FIRSTPT]];
        NSString * presets = [NSString stringWithFormat:@"presets : %@",[UserDefaultManager GetKeyChain:DATA_STORE_CPS_PRESETS]];
        NSString * bitrates = [NSString stringWithFormat:@"bitrates : %@",[UserDefaultManager GetKeyChain:DATA_STORE_CPS_BITRATES]];
        NSString * fps = [NSString stringWithFormat:@"fps : %@",[UserDefaultManager GetKeyChain:DATA_STORE_CPS_FPS]];

        
        NSString * infoDetail = [NSString stringWithFormat:@"%@\n%@\n%@\n%@\n%@\n%@\n%@\n%@\n%@\n%@\n%@\n%@\n%@\n%@\n%@\n%@\n%@",networkDetail,
                                 result,
                                 iceenable,
                                 audiofec,
                                 logreport,
                                 autoadapter,
                                 vqmenable,
                                 prtpenable,
                                 vp8enable,
                                 h264enable,
                                 startbitrate,
                                 minbitrate,
                                 maxbitrate,
                                 firstpt,
                                 presets,
                                 bitrates,
                                 fps];
        
        
        ((id(*)(id,SEL,NSString *))objc_msgSend)(_mucsdelegate, @selector(onNetWorkDetail:),infoDetail);
    }
}


// 对方视频模式回调 add by WLS 20151009
- (void)onRemoteCameraMode:(KCTCameraType)type{
    
    if (nil != _mucsdelegate && [_mucsdelegate respondsToSelector:@selector(onRemoteCameraMode:)]) {
        ((id(*)(id,SEL,int))objc_msgSend)(_mucsdelegate, @selector(onRemoteCameraMode:), type);
    }
    
    
}

- (void)onDTMF:(NSString*)strDTMFvalue
{
    if (nil != _mucsdelegate && [_mucsdelegate respondsToSelector:@selector(onDTMF:)])
    {
        
        [_mucsdelegate performSelector:@selector(onDTMF:) withObject:strDTMFvalue];
        
        
    }

}



- (void)onScreenshotForVideo:(NSString*)cameraCapFilePath;
{
    if (nil != _mucsdelegate && [_mucsdelegate respondsToSelector:@selector(cameraCapture:)])
    {
        
        [_mucsdelegate performSelector:@selector(cameraCapture:) withObject:cameraCapFilePath];
        
        
    }
    
}


#pragma - mark 拨号超时检测

/*******************************拨号超时检测***********************************************/


//拨号超时检测
- (void)CallTimeOutCheck //拨号超时相关
{
    
    if (IsCallTimeOutCheck) {
        return;
    }
    
    
    m_CallTime = 0;
    IsCallTimeOutCheck = YES;
    
    m_CallTimeOut = [NSTimer scheduledTimerWithTimeInterval:1.0
                                                     target:self
                                                   selector:@selector(CallTimeOutAutoStop)
                                                   userInfo:nil
                                                    repeats:YES];
    m_CallTime = 0;
}
//拨号超时自动停止
-(void)CallTimeOutAutoStop //录音超时相关
{
    m_CallTime++;
    
    if(m_CallTime > 80) //20140901
    {
        // 停止录制，通过停止录音回调发送录音
        
        NSLog(@"对方无应答,呼叫超时自动挂断。");
        
        
        //释放呼叫超时定时器
        [self releaseCallNSTimer];
        
        
        [self hangUpCall]; //超过80秒自动挂断电话

        IsCallTimeOutCheck = NO;
        
    }
    
    NSLog(@"CallTimeOutAutoStop By %li",m_CallTime);
    
}


//释放呼叫超时定时器
-(void)releaseCallNSTimer
{
    
    if (nil != m_CallTimeOut)
    {
        [m_CallTimeOut invalidate];
        m_CallTimeOut = nil;
        m_CallTime = 0;
        IsCallTimeOutCheck = NO;
    }
    
}

-(void)reportVoiceQulity:(long)nowAnswers andNowHungUp:(long)nowHungUps
{
    long Voice_time =  nowHungUps-nowAnswers;
    //    if (nowAnswers>0 && Voice_time >2) { //通话大于2秒 add WLS 20151026
    
    //语音上报参数 0为不上报，1为上报。
    
    //无论通话时长 都上报语音质量
    if([[UserDefaultManager GetKeyChain:DATA_STORE_CPS_VQMENABLE] isEqualToString:@"1"])
    {
        [[SoftphoneManager instance]emodelValue];
        
    }
    
    
    //    }
    
    
    
    
}




#pragma mark - rtp包加密回调
-(void)DecryptCallBack:(char*) in_data withOutdata:(char*)out_data withByteIn:(int)bytes_in withByteOut:(int*)bytes_out
{
//    UXLogAlert(@"DecryptCallBack");
    
    if (_mucsdelegate && [_mucsdelegate respondsToSelector:@selector(decryptCallBack:withOutMsg:withInLen:withOutLen:)]) {
        
        
        ((id(*)(id,SEL,char *,char *,int ,int *))objc_msgSend)(_mucsdelegate, @selector(decryptCallBack:withOutMsg:withInLen:withOutLen:), in_data,out_data,bytes_in,bytes_out);
        
    }else{
        
        memcpy(out_data, in_data, bytes_in);
        *bytes_out = bytes_in;
        
        
    }
    
    
}

-(void)EncryptCallBack:(char*) in_data withOutdata:(char*)out_data withByteIn:(int )bytes_in withByteOut:(int*)bytes_out
{
//    UXLogAlert(@"DecryptCallBack");
    if (_mucsdelegate && [_mucsdelegate respondsToSelector:@selector(encryptCallBack:withOutMsg:withInLen:withOutLen:)]) {
        
        
        ((id(*)(id,SEL,char *,char *,int ,int *))objc_msgSend)(_mucsdelegate, @selector(encryptCallBack:withOutMsg:withInLen:withOutLen:), in_data,out_data,bytes_in,bytes_out);
        
    }else{
        
        memcpy(out_data, in_data, bytes_in);
        *bytes_out = bytes_in;
        
    }
    
}



/*******************************通话录音***********************************************/

#pragma mark - 录音文件上传

- (NSString *)getNowTime
{
    NSDate * date = [NSDate date];
    NSDateFormatter * formatter = [[NSDateFormatter alloc] init];
    formatter.dateFormat = @"yyyyMMddHHmmss";
    NSString * dateStr = [formatter stringFromDate:date];
    [formatter release];
    return dateStr;
}
- (NSString *)getRecordFileWithCallId:(NSString *)callId{
    
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString *documentsDirectory = [paths objectAtIndex:0];
    documentsDirectory = [documentsDirectory stringByAppendingPathComponent:@"recordFile"];
    
    NSFileManager *fileManager = [NSFileManager defaultManager];
    
    
    if ([fileManager fileExistsAtPath:documentsDirectory]) {
        [fileManager removeItemAtPath:documentsDirectory error:nil];
    }
    
    NSError *err = nil;
    [fileManager createDirectoryAtPath:documentsDirectory withIntermediateDirectories:YES attributes:nil error:&err];
    
    NSString *filePathMe = [documentsDirectory stringByAppendingPathComponent:[NSString stringWithFormat:@"%@_%@.wav",callId,[self getNowTime]]];
    
    [fileManager createFileAtPath:filePathMe contents:nil attributes:nil];

    
    if (err) {
        return nil;
    }
    
    
    return filePathMe;
}


/**
 @author WLS, 16-01-08 11:01:03
 
 开始录音
 */
- (void)startRecordWithCallId:(NSString *)callId callee:(NSString *)callee caller:(NSString *)caller{
    
    
    [_incomingCallid release];
    _incomingCallid = nil;
    [_icomingCaller release];
    _icomingCaller = nil;
    
    if (_recordTimer) {
        [self stopRecord];
        [_recordTimer invalidate];
        [_recordTimer release];
        _recordTimer = nil;
    }
    
    if (_recordRequest) {
        [_recordRequest stopRcordRequest];
        [_recordRequest release];
        _recordRequest = nil;
    }
    
    
    if (self.needRecord == NO) {
        /**
         @author WLS, 16-01-08 12:01:55
         
         不需要录音
         */
        
        return;
    }
    
    
    
    
    NSString * fileName = [self getRecordFileWithCallId:callId];
    
    if (fileName == nil) {
        
        
        UXLogAlert(@"创建录音文件失败");

        
        return;
    }
    
    if ([[SoftphoneManager instance] recordStartWithFileName:fileName]) {
        _recordTimer = [[NSTimer scheduledTimerWithTimeInterval:RecordTime target:self selector:@selector(stopRecord) userInfo:nil repeats:NO]retain];
        
        UXLogAlert(@"开始录音");
        
        _recordRequest = [[UCSRecordRequestManager alloc] init];
        _recordRequest.callId = callId;
        _recordRequest.fileName = fileName;
        _recordRequest.caller = caller;
        _recordRequest.callee = callee;
        if ([UserDefaultManager GetLocalDataBoolen:DATA_STORE_ISVIDEO]) {
            /**
             @author WLS, 16-01-11 10:01:56
             
             视频通话
             */
            _recordRequest.callMode = @"AFC";

        }else{
            _recordRequest.callMode = @"VFC";
        }
        
        return;
    }
    
    UXLogAlert(@"开始录音失败");
    
    
}

/**
 @author WLS, 16-01-08 11:01:21
 
 结束录音
 */
- (void)stopRecord{
    
    [_incomingCallid release];
    _incomingCallid = nil;
    [_icomingCaller release];
    _icomingCaller = nil;
    
    
    if (_recordTimer == nil) {
        /**
         @author WLS, 16-01-08 12:01:30
         
         此时通话超过3分钟，录音已经停止。
         */
        return;
    }
    
    

    
    if (_recordTimer) {
        [_recordTimer invalidate];
        [_recordTimer release];
        _recordTimer = nil;
    }
    
    
    if ([[SoftphoneManager instance] stopRecord]) {
        UXLogAlert(@"停止录音成功");
        /**
         @author WLS, 16-01-11 10:01:03
         
         开始上传录音文件
         */
        
        if ([UCSPublicFunc GetCurrentPhoneNetType] == PNT_WIFI) {
//            [_recordRequest uploadRecordRequest];
        }
        
        
    }else{
        UXLogAlert(@"停止录音失败");
    }
    
}



#pragma mark - 会话日志上传

- (NSString *)jsonStrWithLogStr{
    
    NSString * firstStr = [[_sdkLogString componentsSeparatedByString:@"***"]firstObject];
    NSString * lastStr = [[_sdkLogString componentsSeparatedByString:@"***"]lastObject];
    NSMutableArray * lastArray = [NSMutableArray arrayWithArray:[lastStr componentsSeparatedByString:@"&&&"]];
    for (int i = 0; i<lastArray.count; i++) {
        if (i == 0) {
            NSArray * logArray = [lastArray[i] componentsSeparatedByString:@"\n"];
            int n = 1;
            for (int j = 0; j< logArray.count; j++) {
                if ([logArray[j] isEqualToString:@""]) {
                    continue;
                }
                [_sdkLogDic setObject:logArray[j] forKey:[NSString stringWithFormat:@"%d",n++]];
            }
            break;
        }
    }
    
    
    // 转成json的二进制
    NSData * jsonData = [NSJSONSerialization dataWithJSONObject:_sdkLogDic options:NSJSONWritingPrettyPrinted error:nil];
    // 转成json的字符串
    NSString * jsonStr = [[NSString alloc] initWithData:jsonData encoding:NSUTF8StringEncoding];
    // 去掉回车和空格
    NSString * str = [[jsonStr stringByReplacingOccurrencesOfString:@"\n" withString:@""] stringByReplacingOccurrencesOfString:@" " withString:@""];
    [jsonStr release];
    
    return [NSString stringWithFormat:@"%@%@",firstStr,str];
}

/**
 @author WLS, 16-01-08 11:01:21
 
 会话结束上传日志
 */
- (void)uploadConversationLogWithDetail:(NSString *)detail{
    
    if (_sdkLogString == nil) {
        return;
    }
    if (_sdkLogRequest) {
        [_sdkLogRequest stopConversationRequest];
        [_sdkLogRequest release];
        _sdkLogRequest = nil;
    }
    
    if (self.isEndCall == NO) {
//        [_sdkLogString appendString:[NSString stringWithFormat:@"\n%@\n\n",detail]];
        if ([detail rangeOfString:@"CALL_LOG"].location != NSNotFound) {
            
            _callLogDetail = [detail copy];
            
        }else{
//            [_sdkLogString appendFormat:@"Report_%@:%@\n\n",[UCSPublicFunc getIPhoneNowTime],[[[[detail componentsSeparatedByString:@"\n"] componentsJoinedByString:@"~"] componentsSeparatedByString:@"\t"] componentsJoinedByString:@"~"]];
        }
        
        return;
    }
    [_sdkLogString appendString:[NSString stringWithFormat:@"&&&%@\n\n\n",detail]];
    [SDKLogManager saveConversationLogWithDetail:[[_sdkLogString stringByReplacingOccurrencesOfString:@"***" withString:@"\n"] stringByReplacingOccurrencesOfString:@"&&&" withString:@"\n"]];
    
    [_sdkLogDic setObject:[[detail componentsSeparatedByString:@"\n"] componentsJoinedByString:@"&&&"] forKey:@"CallLog"];
    
    
    
    self.isUCSCalling = NO;
    self.isEndCall = NO;
    
    _sdkLogRequest = [[UCSConLogRequestManager alloc] init];
    _sdkLogRequest.SDKLogStr = [self jsonStrWithLogStr];
    [_sdkLogRequest uploadConversationRequest];
    [_sdkLogString release];
    _sdkLogString = nil;
    
    [_sdkLogDic release];
    _sdkLogDic = nil;
    
    [_callLogDetail release];
    _callLogDetail = nil;
    
    [UserDefaultManager SetLocalDataString:nil key:DATA_STORE_SERVICEKEY];

}


- (void)apendVoiceFormatWithCallId:(NSString *)callid incomingCall:(NSString *)incomingStr{
    
    // 如果字符串内有值，先清空再拼接add by CW 2016-1-13
    if (_sdkLogString.length != 0 || _sdkLogString) {
        [_sdkLogString release];
        _sdkLogString = nil;
        
    }
    
    if (_sdkLogDic) {
        [_sdkLogDic release];
        _sdkLogDic = nil;
    }
    
    // 拨打电话将标记置为yes 再回调的时候判断改值是否为yes  为yes 则上传会话日志
    self.isUCSCalling = YES;
    
    //拨打电话时候将通话结束标志置为no;
    self.isEndCall = NO;
    
    if ([UserDefaultManager GetLocalDataBoolen:DATA_STORE_ISVIDEO] == NO) {
        _sdkLogString = [[NSMutableString stringWithFormat:@"CALLLOG\t%@\t%@\tSDK\tIOS\t%@\t***%@:%@\n",[UCSPublicFunc getIPhoneNowTime],@"AFC",callid?callid:@"replaceCallid",[UCSPublicFunc getIPhoneNowTime],incomingStr] retain];
        
        [UserDefaultManager SetLocalDataString:@"AFC" key:DATA_STORE_SERVICEKEY];
    }else{
        _sdkLogString = [[NSMutableString stringWithFormat:@"CALLLOG\t%@\t%@\tSDK\tIOS\t%@\t***%@:%@ p2p开关状态=%@\n",   [UCSPublicFunc getIPhoneNowTime],
                          @"VFC",
                          callid?callid:@"replaceCallid",
                          [UCSPublicFunc getIPhoneNowTime],
                          incomingStr,
                          [NSString stringWithFormat:@"%@",[UserDefaultManager  GetKeyChain:DATA_STORE_CPS_ICEENABLE]]] retain];
        [UserDefaultManager SetLocalDataString:@"VFC" key:DATA_STORE_SERVICEKEY];

    }
    
    _sdkLogDic = [[NSMutableDictionary alloc] init];
}

- (void)appendVoiceQulity:(id)value{
    
    [_sdkLogString appendFormat:@"&&&%@\n",value];
    
    [_sdkLogDic setObject:value forKey:@"emodelValue"];
    
    
    
}

@end
