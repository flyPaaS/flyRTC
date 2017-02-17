//
//  KCTVOIPViewEngine.m
//  KCT_IM_Demo
//
//  Created by KCT on 15/12/11.
//  Copyright © 2015年 KCT. All rights reserved.
//

#import "KCTVOIPViewEngine.h"
#import "AppDelegate.h"
#import "KCTVoipCallController.h"
#import "KCTVideoCallController.h"
#import "KCTKeyboradViewController.h"
#import "KCTUserDefaultManager.h"
#import <AudioToolbox/AudioToolbox.h>
#import <AVFoundation/AVFoundation.h>
#import "AFURLResponseSerialization.h"
#import "KCTVoipCallListModel.h"
#import "KCTCallRecordsModel.h"
#import "KCTVoipDBManager.h"
#import "KCTChangeTheViewController.h"
#import "InfoManager.h"
#import "Contact.h"
#import "NSDictionary+JsonBase64.h"


static SystemSoundID shake_sound_enter_id = 0;



@interface KCTVOIPViewEngine()

@property (assign,nonatomic)UIWindow * window;


@property (strong,nonatomic)KCTVoipCallController *callViewController; // WLS，2015-12-11，（语音通话主叫界面）
@property (strong,nonatomic)KCTVoipCallController *incomingCallViewController; // WLS，2015-12-11，（语音通话被叫界面）

@property (strong,nonatomic)KCTVideoCallController *videoViewController; // WLS，2015-12-11，（视频主叫界面）
@property (strong,nonatomic)KCTVideoCallController *incomingVideoViewController; // WLS，2015-12-11，（视频被叫界面）

@property (strong,nonatomic)KCTKeyboradViewController *keyboradViewContoller; // WLS，2016-04-12，拨号界面

@property (assign,nonatomic)KCTCallType callType; // WLS，2015-12-11，（通话类型，主叫还是被叫。）
@property (strong,nonatomic)UIViewController * releaseView;

@end

@implementation KCTVOIPViewEngine

KCTVOIPViewEngine * kctVoipViewEngine = nil;

+(KCTVOIPViewEngine *)getInstance
{
    @synchronized(self){
        if(kctVoipViewEngine == nil){
            kctVoipViewEngine = [[self alloc] init];
        }
    }
    return kctVoipViewEngine;
}


- (id)init
{
    if (self = [super init])
    {
        
        self.window = [(AppDelegate *)[UIApplication sharedApplication].delegate window];
        
        
        _isCalling = NO;
        
    }
    
    return self;
}

- (void)dealloc
{
    self.window = nil;
}

/**
 @author WLS, 15-12-14 17:12:15
 
 设置自定义编解码
 */
- (void)setVideoDecAndVideoEnc{
    //自定义视频编码参数
    KCTVideoEncAttr *vEncAttr = [[KCTVideoEncAttr alloc] init] ;
    
    vEncAttr.isUseCustomEnc = YES;
    vEncAttr.uWidth = 320;
    vEncAttr.uHeight = 240;
    vEncAttr.uStartBitrate = 200;
    vEncAttr.uMaxFramerate = 15;
    
    
    
    //自定义视频解码参数
    KCTVideoDecAttr *vDecAttr = [[KCTVideoDecAttr alloc] init] ;
    
    vDecAttr.isUseCustomDec = YES;
    vDecAttr.uWidth = 320;
    vDecAttr.uHeight = 240;
    
    
    [[KCTFuncEngine getInstance] setVideoAttr:vEncAttr andVideoDecAttr:vDecAttr];
    
    
    
    
    
    //设置视频来电时是否支持预览。
    [[KCTFuncEngine getInstance] setCameraPreViewStatu:NO];
}


- (void)makingCallViewCallNumber:(NSString *)callNumber callType:(KCTCallTypeEnum)callType callName:(NSString *)callName{
    
    
    if (self.callViewController || self.incomingCallViewController || self.videoViewController || self.incomingVideoViewController) {
        /**
         @author WLS, 15-12-11 19:12:19
         
         如果界面存在,说明连续拨打了两次同一个号码，这里就不做重复拨打操作。
         */
        [[KCTVOIPViewEngine getInstance] WriteToSandBox:@"已有界面，不能发起通话"];

        return;
    }
    
    /**
     @author WLS, 16-04-12 11:04:43
     
     移除键盘视图
     */
    [self releaseKeyboardViewAnimation:NO];
    
    
    self.isCalling = YES;
    
    
    
    NSString * callUserid = callNumber;
    if ([callUserid hasPrefix:@"+86"]) {
        NSRange range = [callUserid rangeOfString:@"+86"];
        callUserid = [callUserid substringFromIndex:range.length];
    }
    
    
    
    NSString * nickName = callName;
//    if ([self getAddressListNameFromUserId:callNumber]) {
//        nickName = [self getAddressListNameFromUserId:callNumber];
//    }else{
//        
//        if ([self getNickNameFromeUserId:callNumber]) {
//            nickName = [self getNickNameFromeUserId:callNumber];
//        }else{
//            nickName = callUserid;
//        }
//        
//    }
    //因为不需要匹配通讯录，所以nickName默认就是userid（wenqinglin 2017-01-21）
    nickName = callUserid;
    
    
    [[KCTVOIPViewEngine getInstance] WriteToSandBox:[NSString stringWithFormat:@"发起通话：%@---%@---%@",callUserid,callName,nickName]];

    if (callType == KCTCallType_VOIP || callType == 1) {
        
        NSString * userId = @"";
        NSString * phoneNumber = @"";
        if (callType == 1) {
            /**
             @author WLS, 15-12-11 18:12:55
             
             单向外呼
             */
            phoneNumber = callUserid;
        }else{
            /**
             @author WLS, 15-12-11 18:12:07
             
             免费通话
             */
            userId = callUserid;
        }
        
        self.callType = KCT_voipCall;
        
        KCTVoipCallController * voipCallVC = [[KCTVoipCallController alloc] initWithCallerNo:phoneNumber andVoipNo:userId andCallType:callType];
        voipCallVC.incomingCall = NO;
        voipCallVC.callID = callUserid;
        voipCallVC.callerName = nickName;
        voipCallVC.callType = callType;
        self.callViewController = voipCallVC;
        [self pushToTheViewController:voipCallVC];
        
        [[KCTFuncEngine getInstance] dial:callType andCalled:callUserid andUserdata:@"语音通话"];
        
        
        
    }else if (callType == KCTCallType_VideoPhone){
        self.callType = KCT_videoCall;
        
        KCTVideoCallController * videoCallVC = [[KCTVideoCallController alloc] initWithCallerName:nickName andVoipNo:callUserid];
        videoCallVC.incomingCall = NO;
        videoCallVC.callID = callUserid;
        self.videoViewController = videoCallVC;
        [self pushToTheViewController:videoCallVC];
        
        [[KCTFuncEngine getInstance] dial:callType andCalled:callUserid andUserdata:@"视频通话"];

    }
    
    
}

/**
 @author WLS, 15-12-11 19:12:39
 
 弹起通话界面
 
 @param pushVC 需要弹起的界面
 */
- (void)pushToTheViewController:(UIViewController * )pushVC{
    
    pushVC.view.frame = self.window.bounds;
    [self.window addSubview:pushVC.view];
    [pushVC generyallyAnimationWithView:pushVC.view animationType:GenerallyAnimationSliderFormBottom duration:0.2 delayTime:0 finishedBlock:nil];
    
}


/**
 @author WLS, 15-12-11 18:12:54
 
 移除界面
 
 @param releaseVC 需要移除的界面
 */
- (void)releaseViewControler:(UIViewController *)releaseVC{
    
    if (self.releaseView == releaseVC || releaseVC == nil) {
        

        
        return;
    }
    
    self.releaseView = releaseVC;

    
    /**
     @author WLS, 15-12-19 10:12:13
     
     创建数据库模型
     */
    [self makeDBModel:releaseVC callDic:nil];

    
    
    [releaseVC generyallyAnimationWithView:releaseVC.view animationType:GenerallyAnimationSliderToBottom duration:0.2 delayTime:0 finishedBlock:^{
        [releaseVC.view removeFromSuperview];
        if (releaseVC == self.callViewController) {
            self.callViewController = nil;
        }else if (releaseVC == self.incomingCallViewController){
            self.incomingCallViewController = nil;
        }else if (releaseVC == self.videoViewController){
            self.videoViewController = nil;
        }else if (releaseVC == self.incomingVideoViewController){
            self.incomingVideoViewController = nil;
        }
        self.releaseView =nil;

        
    }];

    
}


/**
 @author WLS, 16-04-12 10:04:54
 
 显示拨号视图
 */
- (void)showKeyboardView{
    
    if (self.videoViewController) {
        return;
    }
    self.keyboradViewContoller = [[KCTKeyboradViewController alloc] init];
    self.keyboradViewContoller.view.frame = self.window.bounds;
    [self.window addSubview:self.keyboradViewContoller.view];
    [self.keyboradViewContoller generyallyAnimationWithView:self.keyboradViewContoller.view animationType:GenerallyAnimationSliderFormBottom duration:0.2 delayTime:0 finishedBlock:nil];
}

/**
 @author WLS, 16-04-12 10:04:54
 
 退出拨号视图
 */
- (void)releaseKeyboardViewAnimation:(BOOL)animation{
    
    if (self.keyboradViewContoller == nil) {
        return;
    }
    if (animation) {
        [self.keyboradViewContoller generyallyAnimationWithView:self.keyboradViewContoller.view animationType:GenerallyAnimationSliderToBottom duration:0.2 delayTime:0 finishedBlock:^{
            [self.keyboradViewContoller.view removeFromSuperview];
            self.keyboradViewContoller = nil;
            
        }];
    }else{
        [self.keyboradViewContoller.view removeFromSuperview];
        self.keyboradViewContoller = nil;
    }
    
    
}



/**
 *  判断开不开震动
 */
- (BOOL)needShake:(NSString *) fromChatter{
    BOOL ret = YES;
    if ([InfoManager sharedInfoManager].isShakeOpen == NO) {
        ret = NO;
    }
    return ret;
}


#pragma  mark - 通话代理


- (void)showNetWorkState:(NSString *)networkStateStr networkDes:(NSString *)networkDes{
    
    if (self.callType == KCT_voipCall) {
        [self.callViewController networkStatusChange:networkStateStr.integerValue networdDes:networkDes];
    }else if (self.callType == KCT_incomingVoipCall){
        [self.incomingCallViewController networkStatusChange:networkStateStr.integerValue networdDes:networkDes];
    }else if (self.callType == KCT_videoCall){
        [self.videoViewController networkStatusChange:networkStateStr.integerValue networdDes:networkDes];
    }else{
        [self.incomingVideoViewController networkStatusChange:networkStateStr.integerValue networdDes:networkDes];
    }
    
}

- (void)showNetWorkDetail:(NSString *)networkDetail{
    
    if (self.callType == KCT_videoCall) {
        [self.videoViewController networkDetailChange:networkDetail];
    }else if (self.callType == KCT_incomingVideoCall){
        [self.incomingVideoViewController networkDetailChange:networkDetail];
    }else{
    }
    
}



//关闭扬声器
- (void)playAudioBack{
    [[AVAudioSession sharedInstance] setCategory:AVAudioSessionCategoryPlayAndRecord error:nil];
    
}

//来电信息
-(void)incomingCallID:(NSString*)callid caller:(NSString*)caller phone:(NSString*)phone name:(NSString*)name callStatus:(int)status callType:(NSInteger)calltype{
    

    if (self.callViewController || self.incomingCallViewController || self.videoViewController || self.incomingVideoViewController) {
        //如果存在来电界面，那先将之前的来电通话挂断，再接受新的来电通话界面 add by WLS 20151113
        //        [self.incomingVC hangup];
        [self.callViewController.view removeFromSuperview];
        [self.incomingCallViewController.view removeFromSuperview];
        [self.videoViewController.view removeFromSuperview];
        [self.incomingVideoViewController.view removeFromSuperview];
        self.callViewController = nil;
        self.incomingCallViewController  = nil;
        self.videoViewController = nil;
        self.incomingVideoViewController = nil;
        [[KCTVOIPViewEngine getInstance] WriteToSandBox:@"已有界面，需要移除"];

    }
    
    
    /**
     @author WLS, 16-04-12 11:04:43
     
     移除键盘视图
     */
    [self releaseKeyboardViewAnimation:NO];
    
    
    self.isCalling = YES;
    [[NSNotificationCenter defaultCenter] postNotificationName:KCTNotiIncomingCall object:nil];
    
    /**
     @author WLS, 15-12-19 10:12:05
     
     主叫的昵称，暂时为通讯录里面的名字。
     */
//    NSString * nickName = [self getAddressListNameFromUserId:caller];
//    if (nickName == nil) {
//        nickName = [self getNickNameFromeUserId:caller];
//        if (nickName == nil) {
//            nickName = caller;
//        }
//    }
    //因为不需要匹配通讯录，所以nickName默认就是userid（wenqinglin 2017-01-21）
    NSString *nickName = caller;
    
    [[KCTVOIPViewEngine getInstance] WriteToSandBox:@"收到来电"];
    
    if (calltype == KCTCallType_VOIP)  //语音电话
    {
        
        self.callType = KCT_incomingVoipCall;
        
        [KCTUserDefaultManager SetLocalDataObject:nil key:KCTNotiLocalNotification];
        UIApplicationState state = [UIApplication sharedApplication].applicationState;//获取应用程序状态，（是前台运行还是后台运行）
        
//        //播放自己的本地来电提示文件
//        NSString *path = [[NSBundle mainBundle] pathForResource:@"call_voice" ofType:@"wav"];
//        if (path) {
//            
//            //扬声器 (WLS)
//            [[AVAudioSession sharedInstance] setCategory:AVAudioSessionCategoryPlayback error:nil];
//            
//            //注册声音到系统
//            AudioServicesCreateSystemSoundID((__bridge CFURLRef)[NSURL fileURLWithPath:path],&shake_sound_enter_id);
//            AudioServicesPlaySystemSound(shake_sound_enter_id);
//            
//            //将调成话筒(WLS)
//            [self performSelector:@selector(playAudioBack) withObject:nil afterDelay:1];
//            
//        }
        
        if (state == UIApplicationStateActive || state == UIApplicationStateInactive)
        {//弹出alert
            //将通话界面推到window窗口上 add by WLS 20151116
            //如果在前台运行
            KCTVoipCallController * incomingCallVC = [[KCTVoipCallController alloc] initWithCallerNo:phone andVoipNo:caller andCallType:calltype];
            incomingCallVC.incomingCall = YES;
            incomingCallVC.callID = callid;
            incomingCallVC.callerName = nickName;
            self.incomingCallViewController = incomingCallVC;
            [self pushToTheViewController:incomingCallVC];
            
            
            // 收到消息时，震动
            if ([self needShake:nil]&&[InfoManager sharedInfoManager].isMessageNotificationOpen) {
                AudioServicesPlaySystemSound(kSystemSoundID_Vibrate);
            }
            
        }
        else   //如果程序在后台运行，则发送本地通知，收到来电本地通知后，点击即可接通来电
        {
            UILocalNotification *notification=[[UILocalNotification alloc] init];
            if (notification!=nil) {
                NSDate *now=[NSDate new];
                notification.fireDate=[now dateByAddingTimeInterval:1.0f];
                notification.timeZone=[NSTimeZone defaultTimeZone];
                notification.alertBody= [NSString stringWithFormat:@"语音电话"];
                notification.soundName = UILocalNotificationDefaultSoundName;
                notification.repeatInterval =0;//重复次数
                
                notification.userInfo = [NSDictionary dictionaryWithObjectsAndKeys:@"语音电话",@"callType",phone,@"phone",caller,@"caller",callid,@"callid",nickName,@"nickName", nil];
                [[UIApplication sharedApplication]   scheduleLocalNotification:notification];
                
                
                [KCTUserDefaultManager SetLocalDataObject:notification.userInfo key:KCTNotiLocalNotification];
            }
            
        }
    }
    else if(calltype == 2)  //视频
    {
        
        
        self.callType = KCT_incomingVideoCall;

        
        [KCTUserDefaultManager SetLocalDataObject:nil key:KCTNotiLocalNotification];
        UIApplicationState state = [UIApplication sharedApplication].applicationState;//获取应用程序状态，（是前台运行还是后台运行）
        
        
//        //播放自己的本地来电提示文件
//        NSString *path = [[NSBundle mainBundle] pathForResource:@"call_voice" ofType:@"wav"];
//        if (path) {
//            //注册声音到系统
//            
//            //扬声器 (WLS)
//            [[AVAudioSession sharedInstance] setCategory:AVAudioSessionCategoryPlayback error:nil];
//            
//            AudioServicesCreateSystemSoundID((__bridge CFURLRef)[NSURL fileURLWithPath:path],&shake_sound_enter_id);
//            AudioServicesPlaySystemSound(shake_sound_enter_id);
//            
//            //将调成话筒(WLS)
//            [self performSelector:@selector(playAudioBack) withObject:nil afterDelay:1];
//        }
        
        //如果在前台运行
        //|| state == UIApplicationStateInactive add by pbf 2015.09.22
        //UIApplicationStateInactive当前应用正在前台运行，但是并不接收事件（当前或许正在执行其它代码），如果按照之前if (state == UIApplicationStateActive)，会造成用户第一次安装程序后收到视频来电时会弹出系统弹框提示访问摄像头，此时收到来电的消息会先到达，而后才会进入系统的- (void)applicationDidBecomeActive:(UIApplication *)application方法将程序状态改为UIApplicationStateActive，所以此时就会出现程序在前台却不弹出视频来电视图incomingVideolView，给用户造成单通的现象的错觉
        if (state == UIApplicationStateActive || state == UIApplicationStateInactive)
        {
            //将通话界面推到window窗口上 add by WLS 20151116
            
            KCTVideoCallController* incomingVideolView = [[KCTVideoCallController alloc] initWithCallerName:nickName andVoipNo:caller];
            incomingVideolView.incomingCall = YES;
            incomingVideolView.callID = callid;
            incomingVideolView.isActivity = NO;
            self.incomingVideoViewController = incomingVideolView;
            [self pushToTheViewController:incomingVideolView];
            
            
            // 收到消息时，震动
            if ([self needShake:nil]&&[InfoManager sharedInfoManager].isMessageNotificationOpen) {
                AudioServicesPlaySystemSound(kSystemSoundID_Vibrate);
            }

        }else{
            UILocalNotification *notification=[[UILocalNotification alloc] init];
            if (notification!=nil) {
                NSDate *now=[NSDate new];
                notification.fireDate=[now dateByAddingTimeInterval:1.0f];
                notification.timeZone=[NSTimeZone defaultTimeZone];
                notification.alertBody= [NSString stringWithFormat:@"视频电话"];
                notification.soundName = UILocalNotificationDefaultSoundName;
                notification.repeatInterval =0;//重复次数
                
//                notification.userInfo = [NSDictionary dictionaryWithObjectsAndKeys:@"视频电话",@"callType",phone,@"phone",caller,@"caller",callid,@"callid",status,@"status",nickName,@"nickName", nil];

                notification.userInfo = [NSDictionary dictionaryWithObjectsAndKeys:@"视频电话",@"callType",
                                         phone,@"phone",
                                         caller,@"caller",
                                         callid,@"callid",
                                         nickName,@"nickName",
                                          nil];

                NSLog(@"%@",notification.userInfo);
                [[UIApplication sharedApplication]   scheduleLocalNotification:notification];
                
                [KCTUserDefaultManager SetLocalDataObject:notification.userInfo key:KCTNotiLocalNotification];
            }
            
        }
    }
    

   
}



//通话状态回调
-(void)responseVoipManagerStatus:(KCTCallStatus)event callID:(NSString*)callid data:(KCTReason *)data withVideoflag:(int)videoflag{
    
    
    /**
     @author WLS, 15-12-14 12:12:23
     
     当进入后台收到来电，对方又挂断的时候，移除本地存储的通话信息
     */
    if ([UIApplication sharedApplication].applicationState == UIApplicationStateBackground) {
        switch (event)
        {
            case KCTCallStatus_Released:
            {
                /**
                 @author WLS, 15-12-19 10:12:29
                 
                 首先先要判断有没有通话
                 */
                
                NSMutableDictionary * notifDic = [KCTUserDefaultManager GetLocalDataObject:KCTNotiLocalNotification];
                if (notifDic) {
                    /**
                     @author WLS, 15-12-19 10:12:26
                     
                     如果存在通话，则保留到未接来电到数据库，因为此时是没有接听到电话的
                     */
                    [self makeDBModel:nil callDic:notifDic];
                    
                }
                
                [KCTUserDefaultManager SetLocalDataObject:nil key:KCTNotiLocalNotification];
            }
                break;
                
            default:
                break;
        }
    }
    
    [[KCTVOIPViewEngine getInstance] WriteToSandBox:[NSString stringWithFormat:@"收到信令：%d---%@---%@",event,callid,data.msg]];

    if (self.callType == KCT_voipCall) {
        [self.callViewController responseVoipManagerStatus:event callID:callid data:data withVideoflag:videoflag];
    }else if (self.callType == KCT_incomingVoipCall){
        [self.incomingCallViewController responseVoipManagerStatus:event callID:callid data:data withVideoflag:videoflag];
    }else if (self.callType == KCT_videoCall){
        [self.videoViewController responseVoipManagerStatus:event callID:callid data:data withVideoflag:videoflag];
    }else{
        [self.incomingVideoViewController responseVoipManagerStatus:event callID:callid data:data withVideoflag:videoflag];
    }
    
    
}




/**
 @author WLS, 15-12-15 17:12:47
 
 获取设置的ip地址，如果没有设置，则默认imactivity.ucpaas.com
 */
- (NSString *)getIP
{
    NSUserDefaults * defaults = [NSUserDefaults standardUserDefaults];
    NSString * asAddress = [defaults stringForKey:@"asAddress"];
    NSString * asPort = [defaults stringForKey:@"asPort"];
    if (asAddress.length == 0) {
        asAddress = @"imactivity.ucpaas.com";
    }
    NSString * address;
    if (asPort.length != 0) {
        address = [NSString stringWithFormat:@"%@:%@",asAddress,asPort];
    }else{
        address = asAddress;
    }
    return address;
}

/**
 @author WLS, 15-12-15 17:12:47
 
 查询剩余的通话时间
 */
- (void)getCallBalance{
    
    if ([InfoManager sharedInfoManager].phone == NULL ||
        [InfoManager sharedInfoManager].phone == nil) {
        return;
    }
    
    NSString * url = [NSString stringWithFormat:getBalanceUrl,[self getIP]];

    NSDictionary *params = @{@"phone":[InfoManager sharedInfoManager].phone};
    
    NSString * paramsStr = [NSDictionary jsonbase64WithDictionary:params];
    
    
    NSMutableURLRequest *request = [NSMutableURLRequest requestWithURL:[NSURL URLWithString:url]];
    request.HTTPMethod = @"POST";
    request.HTTPBody = [paramsStr dataUsingEncoding:NSUTF8StringEncoding];
    [request setValue:@"application/json; encoding=utf-8" forHTTPHeaderField:@"Content-Type"];
    [request setValue:@"application/json" forHTTPHeaderField:@"Accept"];
    
    [NSURLConnection sendAsynchronousRequest:request queue:[NSOperationQueue mainQueue] completionHandler:^(NSURLResponse * _Nullable response, NSData * _Nullable data, NSError * _Nullable connectionError) {
        if (data) {
            NSDictionary * requestDic = [NSJSONSerialization JSONObjectWithData:data options:NSJSONReadingMutableContainers error:nil];
            
            NSString * result = [requestDic objectForKey:@"result"];
            int code = result.intValue;
            if (code==0) {
                self.callBalance = [NSString stringWithFormat:@"%ld",(NSInteger)[[requestDic objectForKey:@"balance"] integerValue]/100000];
            }
            
            [[NSNotificationCenter defaultCenter] postNotificationName:KCTNotiCallBalance object:nil];
        } else {
            
        }
    }];
}


#pragma mark - 系统进入前台

//后台运行的时候，有语音电话过来的话，本地通知提示，点击之后app今天active状态，并且启动接听语音电话界面
-(void)InactiveCall:(NSMutableDictionary *)callDic
{

    NSDictionary *aDic = callDic;
    if ([[aDic objectForKey:@"callType"]isEqualToString:@"语音电话"]) {
        
        
        KCTVoipCallController * incomingCallVC = [[KCTVoipCallController alloc] initWithCallerNo:[aDic objectForKey:@"phone"] andVoipNo:[aDic objectForKey:@"caller"] andCallType:KCTCallType_VOIP];
        incomingCallVC.incomingCall = YES;
        incomingCallVC.callerName = [aDic objectForKey:@"nickName"];
        incomingCallVC.callID = [aDic objectForKey:@"callid"];
        self.incomingCallViewController = incomingCallVC;
        [self pushToTheViewController:incomingCallVC];
        
    }
    else if([[aDic objectForKey:@"callType"]isEqualToString:@"视频电话"])
    {
        KCTVideoCallController* incomingVideolView = [[KCTVideoCallController alloc] initWithCallerName:[aDic objectForKey:@"nickName"] andVoipNo:[aDic objectForKey:@"caller"]];
        incomingVideolView.incomingCall = YES;
        incomingVideolView.isActivity = YES;
        incomingVideolView.callID = [aDic objectForKey:@"callid"];
        self.incomingVideoViewController = incomingVideolView;
        [self pushToTheViewController:incomingVideolView];
    }
    
    
}


- (NSString *)getErrorDescription:(NSInteger)code{
    
    NSString * errorDescription = @"";
    
    switch (code) {
        case 2001:
            errorDescription = @"验证码过期";
            break;
        case 2002:
            errorDescription = @"验证码不正确";
            break;
        case 2003:
            errorDescription = @"手机号码为空";
            break;
        case 2004:
            errorDescription = @"验证码为空";
            break;
        case 2005:
            errorDescription = @"昵称为空";
            break;
        case 2006:
            errorDescription = @"查无数据";
            break;
        case 2007:
            errorDescription = @"发送短信验证码失败";
            break;
        case 2008:
            errorDescription = @"创建用户失败";
            break;
        case 2009:
            errorDescription = @"参数不正确";
            break;
        case 2099:
            errorDescription = @"其他错误";
            break;
        default:
            break;
    }
    
    
    return errorDescription;
}


- (NSString *)getNickNameFromeUserId:(NSString *)userid{
    NSUserDefaults *userDefault = [NSUserDefaults standardUserDefaults];
    NSDictionary *dic = [userDefault objectForKey:kCalledUidKey];
    NSString *num = [dic objectForKey:@"client_number"];
    
    return num;
//    for (KCTConversation * conversation in self.nickNameArray) {
//        if (conversation.conversationType == KCT_IM_SOLOCHAT) {
//            /**
//             @author WLS, 16-01-08 15:01:22
//             
//             单聊
//             */
//            if ([conversation.targetId isEqualToString:userid]) {
//                return conversation.conversationTitle;
//            }
//            
//            
//        }
//        
//        
//    }
    
    
    return nil;
}


#pragma mark - 数据库操作
/**
 @author WLS, 15-12-19 10:12:43
 
 存储到数据库
 
 @param releaseVC 点击挂断或者被挂断的通话界面
 @param callDic   没有弹起通话界面，只是在后台被挂断，此时的收到来电的主叫数据
 */
- (void)makeDBModel:(UIViewController *)releaseVC callDic:(NSDictionary *)callDic{
    
    if (releaseVC == nil && callDic == nil) {
        return;
    }
    
    KCTVoipCallListModel * callModel = [[KCTVoipCallListModel alloc] init];
    callModel.headPortrait = @"";
    callModel.time = [NSString stringWithFormat:@"%ld",time(NULL)];
    if (releaseVC == nil) {
        
        /**
         @author WLS, 15-12-19 10:12:48
         
         此时通话未建立，被叫通话没有接听
         */
        callModel.userId = [callDic objectForKey:@"caller"];
        callModel.nickName = [callDic objectForKey:@"nickName"];
        callModel.callDuration = @"";
        callModel.callType = [NSString stringWithFormat:@"%d",[[callDic objectForKey:@"callType"]isEqualToString:@"语音电话"]?KCTCallType_VOIP:KCTCallType_VideoPhone];
        callModel.callStatus = [NSString stringWithFormat:@"%d",KCT_CallType_Disanswer];
        callModel.sendCall = @"0";
        
    }else if ([releaseVC isKindOfClass:[KCTVoipCallController class]] ) {
        
        KCTVoipCallController * callModelVC = (KCTVoipCallController *)releaseVC;
        if (callModelVC.incomingCall) {
            
            /**
             @author WLS, 15-12-19 10:12:36
             
             语音通话被叫
             */
            callModel.userId = callModelVC.voipNo;
            callModel.nickName = callModelVC.callerName;
            callModel.callDuration = callModelVC.callDuration;
            callModel.time = callModelVC.currentTime?callModelVC.currentTime:callModel.time;
            callModel.callType = [NSString stringWithFormat:@"%d",KCTCallType_VOIP];
            if (callModelVC.isCallNow) {
                callModel.callStatus = [NSString stringWithFormat:@"%d",KCT_CallType_Answer];
            }else{
                callModel.callStatus = [NSString stringWithFormat:@"%d",KCT_CallType_Disanswer];
            }
            callModel.sendCall = @"0";
            

        }else{
            
            /**
             @author WLS, 15-12-19 10:12:26
             
             语音通话主叫
             */
            callModel.userId = callModelVC.voipNo;
            callModel.nickName = callModelVC.callerName;
            callModel.callDuration = callModelVC.callDuration;
            callModel.time = callModelVC.currentTime?callModelVC.currentTime:callModel.time;
            callModel.callType = [NSString stringWithFormat:@"%d",callModelVC.callType];
            if (callModelVC.isCallNow) {
                callModel.callStatus = [NSString stringWithFormat:@"%d",KCT_CallType_Answer];
            }else{
                if (callModelVC.beReject) {
                    /**
                     @author WLS, 15-12-21 11:12:48
                     
                     被叫拒绝接听
                     */
                    callModel.callStatus = [NSString stringWithFormat:@"%d",KCT_CallType_Cancel];
                    
                }else{
                    callModel.callStatus = [NSString stringWithFormat:@"%d",KCT_CallType_Cancel];
                    
                }
            }
            callModel.sendCall = @"1";
            
            if (callModelVC.callType == 1) {
                callModel.userId = callModelVC.callerNo;
    
                /**
                 @author WLS, 15-12-19 10:12:10
                 
                 如果拨打的是单向外呼，在通话结束后，需要重新更新免费通话时间
                 */
                [self getCallBalance];
            }
        }
        
        
    }else if ([releaseVC isKindOfClass:[KCTVideoCallController class]]){
        
        KCTVideoCallController * callModelVC = (KCTVideoCallController *)releaseVC;
        if (callModelVC.incomingCall) {
            /**
             @author WLS, 15-12-19 10:12:42
             
             视频通话被叫
             */
            callModel.userId = callModelVC.voipNo;
            callModel.nickName = callModelVC.callerName;
            callModel.callDuration = callModelVC.callDuration;
            callModel.time = callModelVC.currentTime?callModelVC.currentTime:callModel.time;
            callModel.callType = [NSString stringWithFormat:@"%d",KCTCallType_VideoPhone];
            if (callModelVC.isCallNow) {
                callModel.callStatus = [NSString stringWithFormat:@"%d",KCT_CallType_Answer];
            }else{
                callModel.callStatus = [NSString stringWithFormat:@"%d",KCT_CallType_Disanswer];
            }
            callModel.sendCall = @"0";
            
        }else{
            /**
             @author WLS, 15-12-19 10:12:42
             
             视频通话主叫
             */
            callModel.userId = callModelVC.voipNo;
            callModel.nickName = callModelVC.callerName;
            callModel.callDuration = callModelVC.callDuration;
            callModel.time = callModelVC.currentTime?callModelVC.currentTime:callModel.time;
            callModel.callType = [NSString stringWithFormat:@"%d",KCTCallType_VideoPhone];
            if (callModelVC.isCallNow) {
                callModel.callStatus = [NSString stringWithFormat:@"%d",KCT_CallType_Answer];
            }else{
                if (callModelVC.beReject) {
                    /**
                     @author WLS, 15-12-21 11:12:48
                     
                     被叫拒绝接听
                     */
                    callModel.callStatus = [NSString stringWithFormat:@"%d",KCT_CallType_Cancel];
                    
                }else{
                    callModel.callStatus = [NSString stringWithFormat:@"%d",KCT_CallType_Cancel];
                    
                }
            }
            callModel.sendCall = @"1";

        }
        
        

    }
    
    
    
    if ([callModel.userId isEqualToString:callModel.nickName]) {
        callModel.nickName = @"";
    }
    
    
    
    if ([callModel checkModelInfo]) {
        return;
    }
    
    KCTCallRecordsModel * recordModel = [[KCTCallRecordsModel alloc] init];
    [recordModel getInfoFromCallListModel:callModel];
    
    
    
//    
    NSLog(@"%@-------%@---------%@",self.incomingVideoViewController.voipNo,callModel.userId,recordModel.userId);
//    NSLog(@"%@-------%@---------%@",self.callViewController,callModel.nickName,recordModel.nickName);
//    NSLog(@"%@----------------%@",callModel.time,recordModel.time);
//    NSLog(@"%@-------%@---------%@",self.callViewController,callModel.callDuration,recordModel.callDuration);
//    NSLog(@"%@----------------%@",callModel.headPortrait,recordModel.headPortrait);
//    NSLog(@"%@----------------%@",callModel.callType,recordModel.callType);
//    NSLog(@"%@----------------%@",callModel.callStatus,recordModel.callStatus);
//    NSLog(@"%@----------------%@",callModel.sendCall,recordModel.sendCall);

    
    NSArray *arr = [[KCTVoipDBManager shareInstance] getDataWithDB:@"KCTVoipCallListModel" UserId:callModel.userId];
    if (arr.count != 0) {
        /**
         @author WLS, 15-12-19 10:12:11
         
         说明表中存在这个联系人，需要更新
         */
        
        for (KCTVoipCallListModel * model in arr) {
            callModel.isTop = model.isTop;
        }
        
        [[KCTVoipDBManager shareInstance] updateDBWithModel:callModel];
    }else{
        callModel.isTop = @"no";
        [[KCTVoipDBManager shareInstance] insertDBWithModel:callModel];
    }
//    for (int i = 0; i < 40; i++) {
        [[KCTVoipDBManager shareInstance] insertDBWithModel:recordModel];
//    }

    [[KCTVOIPViewEngine getInstance] WriteToSandBox:[NSString stringWithFormat:@"写入数据库"]];

    /**
     @author WLS, 15-12-19 14:12:38
     
     当数据库存储完成后，发通知更新电话界面
     */
    self.isCalling = NO;
    [[NSNotificationCenter defaultCenter] postNotificationName:KCTNotiRefreshCallList object:nil];
    
}

#pragma mark - 文件写入


- (NSString *)getNowTime
{
    NSDate * date = [NSDate date];
    NSDateFormatter * formatter = [[NSDateFormatter alloc] init];
    formatter.dateFormat = @"yyyy-MM-dd HH:mm:ss";
    NSString * dateStr = [formatter stringFromDate:date];
    return dateStr;
}
- (void)WriteToSandBox:(NSString *)str
{
    
    
    
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString *documentsDirectory = [paths objectAtIndex:0];
    NSString *filePath=[documentsDirectory stringByAppendingPathComponent:@"KCTVoipLog.txt"];
    NSFileManager * fileManager = [NSFileManager defaultManager];
    if ([[NSFileManager defaultManager] fileExistsAtPath:filePath])
    {
        //如果文件存在并且它的大小大于1M，则删除并且重新创建一个
        long long filesizes  = [[fileManager attributesOfItemAtPath:filePath error:nil] fileSize];
        if ((filesizes/(1024.0*1024.0))>1) {
            
            //删除当前文件
            [fileManager removeItemAtPath:filePath error:nil];
            //重新创建一个文件
            [fileManager createFileAtPath:filePath contents:nil attributes:nil];
        }
        
        NSFileHandle *outFile = [NSFileHandle fileHandleForWritingAtPath:filePath];
        //找到并定位到outFile的末尾位置(在此后追加文件)
        [outFile seekToEndOfFile];
        
        [outFile writeData:[[NSString stringWithFormat:@"\n\n==========%@=========\n%@",[self getNowTime],str] dataUsingEncoding:NSUTF8StringEncoding]];
        //关闭读写文件
        [outFile closeFile];
    }else{
        // 如果文件不存在 则创建并且将文件写入
        [fileManager createFileAtPath:filePath contents:nil attributes:nil];
        [str writeToFile:filePath atomically:YES encoding:NSUTF8StringEncoding error:nil];
    }

}

+ (BOOL)isHeadphone
{
    UInt32 propertySize = sizeof(CFStringRef);
    CFStringRef state = nil;
    AudioSessionGetProperty(kAudioSessionProperty_AudioRoute
                            ,&propertySize,&state);
    //return @"Headphone" or @"Speaker" and so on.
    //根据状态判断是否为耳机状态
    if ([(__bridge NSString *)state isEqualToString:@"Headphone"] ||[(__bridge NSString *)state isEqualToString:@"HeadsetInOut"])
    {
        return YES;
    }else {
        return NO;
    } 
}

@end
