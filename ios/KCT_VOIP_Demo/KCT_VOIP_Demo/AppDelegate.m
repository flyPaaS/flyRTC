//
//  AppDelegate.m
//  KCT_VOIP_Demo
//
//  Created by KCMac on 2017/1/7.
//  Copyright © 2017年 KCMac. All rights reserved.
//

#import "AppDelegate.h"
#import <UIKit/UIKit.h>
#import <UserNotifications/UserNotifications.h>
#import <PushKit/PushKit.h>
#import <CallKit/CallKit.h>
#import <AVFoundation/AVFoundation.h>
#include <Intents/Intents.h>

@interface AppDelegate ()<KCTTCPDelegateBase,UNUserNotificationCenterDelegate,PKPushRegistryDelegate,CXProviderDelegate>
{
    NSString *_callid;
    int _vpsid;
    NSUUID *_calledUUID;
    CXProvider* _provider;
    CXCallController* _callController;
    BOOL _isFromAddressBook;
    NSString *_callNumber;
}


@end


@implementation AppDelegate


#pragma mark-
#pragma mark---------UIApplication Delegate------------
- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    // Override point for customization after application launch.
    NSLog(@"--didFinishLaunchingWithOptions---");
    self.window = [[UIWindow alloc] init];
    self.window.frame = [UIScreen mainScreen].bounds;
    self.window.backgroundColor = [UIColor whiteColor];
    //[self redirectConsoleLogToDocumentFolder];

    
    [LoginManager sharedLoginManager].currentPageIndex = -1;
    
    LoginViewController *controller = [[LoginViewController alloc] init];
    ExNavigationController *nav = [[ExNavigationController alloc] initWithRootViewController:controller];
    self.window.rootViewController = nav;
    [[LoginManager sharedLoginManager] addController:controller];
    [LoginManager sharedLoginManager].nav = nav;
    
    [self.window makeKeyAndVisible];
    
    //设置tcp代理
    [[KCTTcpClient sharedTcpClientManager] setTcpDelegate:self];
    /**
     初始化voip功能类
     */
    KCTFuncEngine *funcEngine = [KCTFuncEngine getInstance];
    //dialling_tone
    NSString *musicPath = [[NSBundle mainBundle] pathForResource:@"dialling_tone" ofType:@"pcm"];
    [funcEngine setIncomingCallMusicPath:musicPath];
    
    /**
    
     
     设置音视频编解码
     */
    [[KCTVOIPViewEngine getInstance] setVideoDecAndVideoEnc];
    
    
    //push voip
    dispatch_queue_t mainQueue = dispatch_get_main_queue();
    PKPushRegistry *voipRegistry = [[PKPushRegistry alloc] initWithQueue:mainQueue];
    voipRegistry.delegate = self;
    voipRegistry.desiredPushTypes = [NSSet setWithObject:PKPushTypeVoIP];
    
    [self initProvider];
    return YES;
}


- (BOOL)application:(UIApplication *)application continueUserActivity:(NSUserActivity *)userActivity restorationHandler:(void(^)(NSArray * __nullable restorableObjects))restorationHandler
{
    INInteraction *interaction = userActivity.interaction;
    INIntent *intent = interaction.intent;
    
    if([userActivity.activityType isEqualToString:@"INStartVideoCallIntent"]) {
        INPerson *person = [(INStartVideoCallIntent *)intent contacts][0];
        CXHandle *handleNumber = [[CXHandle alloc] initWithType:(CXHandleType)person.personHandle.type value:person.personHandle.value];
        _calledUUID = [NSUUID UUID];
        CXStartCallAction* action = [[CXStartCallAction alloc] initWithCallUUID:_calledUUID handle:handleNumber];
        action.video = YES;
        CXTransaction* transaction = [[CXTransaction alloc] init];
        [transaction addAction:action];
        
        [_callController requestTransaction:transaction completion:^(NSError * _Nullable error) {
            if (error) {
                NSLog(@"requestTransaction error %@", error);
            }
        }];
        NSString *clientNumber = person.personHandle.value;
        if ([LoginManager sharedLoginManager].currentPageIndex < appPageIndexCalled) {
            [[LoginManager sharedLoginManager] callAutoLogin];
        }
        _isFromAddressBook = YES;
        //clientNumber = @"73145051196748";
        
        if ([KCTTcpClient sharedTcpClientManager].login_isConnected) {
            [[KCTVOIPViewEngine getInstance] makingCallViewCallNumber:clientNumber callType:KCTCallType_VideoPhone callName:@""];
            _isFromAddressBook = NO;
        } else {
            _callNumber = clientNumber;
        }
        
        return YES;
    }
    
    return YES;
}


#pragma mark
#pragma mark------PKPushRegistryDelegate---------
- (void)pushRegistry:(PKPushRegistry *)registry didInvalidatePushTokenForType:(PKPushType)type {
    NSLog(@"didInvalidatePushTokenForType");
}


- (void)pushRegistry:(PKPushRegistry *)registry didUpdatePushCredentials:(PKPushCredentials *)credentials forType:(PKPushType)type {
    
    [[KCTTcpClient sharedTcpClientManager] setPushEnvironment:KCTPushEnvironment_Production deviceToken:credentials.token];
}


- (CXProviderConfiguration *)config{
    static CXProviderConfiguration* configInternal = nil;
    
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        configInternal = [[CXProviderConfiguration alloc] initWithLocalizedName:@"flyRTC"];
        configInternal.supportsVideo = YES;
        configInternal.maximumCallsPerCallGroup = 1;
        configInternal.maximumCallGroups = 1;
        configInternal.supportedHandleTypes = [NSSet setWithObject:@(CXHandleTypePhoneNumber)];
        UIImage* iconMaskImage = [UIImage imageNamed:@"个人详情_直拨"];
        configInternal.iconTemplateImageData = UIImagePNGRepresentation(iconMaskImage);
        configInternal.ringtoneSound = @"Ringtone.caf";
    });
    
    return configInternal;
}

- (void)initProvider
{
    _provider = [[CXProvider alloc] initWithConfiguration:[self config]];
    [_provider setDelegate:self queue:nil];
    _callController = [[CXCallController alloc] init];
}

- (void)pushRegistry:(PKPushRegistry *)registry didReceiveIncomingPushWithPayload:(PKPushPayload *)payload forType:(PKPushType)type {
    
    NSDictionary *userInfo = payload.dictionaryPayload;
    UIApplication *application = [UIApplication sharedApplication];
    UIApplicationState state = application.applicationState;
    NSLog(@"-----didReceiveIncomingPushWithPayload --%@-",userInfo);
    if (state == UIApplicationStateBackground)
    {
        if (userInfo)
        {
            NSDictionary *apsDic = [userInfo objectForKey:@"aps"];
            NSString *callid = [apsDic objectForKey:@"callid"];
            NSString *callNum = [apsDic objectForKey:@"calleruid"];
            NSNumber *numVpsid = [apsDic objectForKey:@"vpsid"];
            NSNumber *numAction = [apsDic objectForKey:@"action"];
            
            if (callid == nil)
            {
                return;
            }
            if ([numAction integerValue] == 0 && numAction != nil) {
                NSLog(@"--cab=-stopCall");
                [self stopCall];
                return;
            }
            [KCTVOIPViewEngine getInstance].isCallKit = YES;
            
            if ([LoginManager sharedLoginManager].currentPageIndex < appPageIndexCalled){
                [[LoginManager sharedLoginManager] autoConnectCS];
            }
            int vps = [numVpsid intValue];
            _callid = callid;
            _vpsid = vps;
            
            
            _calledUUID = [NSUUID UUID];
            NSString* calling = callNum;
            CXCallUpdate* update = [[CXCallUpdate alloc] init];
            [update setLocalizedCallerName:calling];
            update.hasVideo = YES;
            update.remoteHandle = [[CXHandle alloc] initWithType:CXHandleTypePhoneNumber value:calling];
            [[AVAudioSession sharedInstance] setCategory:AVAudioSessionCategoryPlayAndRecord error:nil];
            [_provider reportNewIncomingCallWithUUID:_calledUUID update:update completion:^(NSError * _Nullable error) {
                if (error) {
                    NSLog(@"report error");
                }
            }];
        }
    }

}


- (void)callPhone {
    NSLog(@"--callPhone-%@   %d-",_callNumber,_isFromAddressBook);
    if (_callNumber && _isFromAddressBook) {
        [[KCTVOIPViewEngine getInstance] makingCallViewCallNumber:_callNumber callType:KCTCallType_VideoPhone callName:@""];
        _isFromAddressBook = NO;
        _callNumber = nil;
    }
}

- (void)sendPushResp
{
    
    NSMutableDictionary * notifDic = [KCTUserDefaultManager GetLocalDataObject:KCTNotiLocalNotification];
    if (notifDic.allKeys.count == 0)
    {
        if (_callid != nil && _vpsid != 0)
        {
            KCTFuncEngine *funcEngine = [KCTFuncEngine getInstance];
            [funcEngine incomingRspWhenBackground:_callid vpsid:_vpsid];
            _callid = nil;
            _vpsid = 0;
        }
        else
        {
            NSLog(@"--push callid is %@  vpsid %d",_callid,_vpsid);
        }
    }
    else
    {
        NSLog(@"push notifDic %@",notifDic);
    }
}


#pragma mark-
#pragma mark------CXProviderDelegate------
- (void)providerDidReset:(CXProvider *)provider
{
    NSLog(@"*****providerDidReset******");
}


- (void)provider:(CXProvider *)provider performEndCallAction:(CXEndCallAction *)action{
    NSLog(@"----performEndCallAction----");
    
    NSUUID* currentID = _calledUUID;
    if ([[action.callUUID UUIDString] isEqualToString:[currentID UUIDString]]) {
        NSLog(@"--endCall curren id :%@---",_calledUUID);
        if ([KCTTcpClient sharedTcpClientManager].login_isConnected)
        {
            NSLog(@"--endCall hangup---");
            [[KCTFuncEngine getInstance] hangUp:_callid];
            _callid = nil;
            _vpsid = 0;
        }
        
        [self stopCall];
        [action fulfill];
    } else {
        [action fail];
    }
    _calledUUID = nil;
}

- (void)provider:(CXProvider *)provider performStartCallAction:(CXStartCallAction *)action
{
    [action fulfill];
    
}

- (void)provider:(CXProvider *)provider performAnswerCallAction:(CXAnswerCallAction *)action
{
    NSLog(@"--performAnswerCallAction--");
    [action fulfill];
    [KCTVOIPViewEngine getInstance].isCallKit = YES;
    if ([LoginManager sharedLoginManager].currentPageIndex < appPageIndexCalled) {
        [[LoginManager sharedLoginManager] autoPushGUI];
    }
    
    NSMutableDictionary * notifDic = [KCTUserDefaultManager GetLocalDataObject:KCTNotiLocalNotification];
    if (notifDic.allKeys.count == 0)
    {
        NSLog(@"performAnswerCallAction dic is null");
    }
    else
    {
        [[KCTVOIPViewEngine getInstance] InactiveCall:notifDic];
        [KCTUserDefaultManager SetLocalDataObject:nil key:KCTNotiLocalNotification];
    }
    
    dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(1 * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
        [KCTVOIPViewEngine getInstance].isCallKit = NO;
    });
}


- (void)stopCall
{
    
    if (_calledUUID)
    {
        _callid = nil;
        _vpsid = 0;
        [KCTVOIPViewEngine getInstance].isCallKit = NO;
        
        CXEndCallAction* endAction = [[CXEndCallAction alloc] initWithCallUUID:_calledUUID];
        CXTransaction* transaction = [[CXTransaction alloc] init];
        [transaction addAction:endAction];
        
        [_callController requestTransaction:transaction completion:^(NSError * _Nullable error) {
            if (error) {
                NSLog(@"requestTransaction error %@", error);
            }
        }];
    }
    
}

- (void)applicationWillResignActive:(UIApplication *)application {
    // Sent when the application is about to move from active to inactive state. This can occur for certain types of temporary interruptions (such as an incoming phone call or SMS message) or when the user quits the application and it begins the transition to the background state.
    // Use this method to pause ongoing tasks, disable timers, and invalidate graphics rendering callbacks. Games should use this method to pause the game.
}


- (void)applicationDidEnterBackground:(UIApplication *)application {
    NSLog(@"background");
    [self stopCall];
    // Use this method to release shared resources, save user data, invalidate timers, and store enough application state information to restore your application to its current state in case it is terminated later.
    // If your application supports background execution, this method is called instead of applicationWillTerminate: when the user quits.
}


- (void)applicationWillEnterForeground:(UIApplication *)application {
    // Called as part of the transition from the background to the active state; here you can undo many of the changes made on entering the background.
    UIApplicationState state = application.applicationState;//获取应用程序状态，（是前台运行还是后台运行）
    
    NSMutableDictionary * notifDic = [KCTUserDefaultManager GetLocalDataObject:KCTNotiLocalNotification];
    NSLog(@"--applicationWillEnterForeground---%@",notifDic);
    if (notifDic==nil) {
        
        return;
    }
    //如果在前台运行
    if (state == UIApplicationStateActive || state == UIApplicationStateInactive){//弹出alert
        if (notifDic.allKeys.count==0) {
            //            [self showSignInNotification:notification.alertBody];
        }else{
            [[KCTVOIPViewEngine getInstance] InactiveCall:notifDic];
        }
    }else{
        if (notifDic.allKeys.count==0) {
            return;
        }else{
            [[KCTVOIPViewEngine getInstance] InactiveCall:notifDic];
        }
    }
    [KCTUserDefaultManager SetLocalDataObject:nil key:KCTNotiLocalNotification];
}


- (void)applicationDidBecomeActive:(UIApplication *)application {
    // Restart any tasks that were paused (or not yet started) while the application was inactive. If the application was previously in the background, optionally refresh the user interface.
}


- (void)applicationWillTerminate:(UIApplication *)application {
    // Called when the application is about to terminate. Save data if appropriate. See also applicationDidEnterBackground:.
}


#pragma mark-
#pragma mark----------KCTTCPDelegateBase----------

- (void)didConnectionStatusChanged:(KCTConnectionStatus)connectionStatus error:(KCTError *)error
{
    switch (connectionStatus) {
        case KCTConnectionStatus_BeClicked:
            [self kickOff:error];
            break;
        case KCTConnectionStatus_loginSuccess:
        {
            [self sendPushResp];
            [self callPhone];
            [[NSNotificationCenter defaultCenter] postNotificationName:TCPConnectStateNotification object:KCTCPDidConnectNotification];
        }
            break;
        case KCTConnectionStatus_ReConnectSuccess:
        {
            [self sendPushResp];
            [self callPhone];
            [[NSNotificationCenter defaultCenter] postNotificationName:TCPConnectStateNotification object:KCTCPDidConnectNotification];
        }
            
            break;
        case KCTConnectionStatus_StartReConnect:
        {
            //[MBProgressHUD showMessage:@"开始重连..." toView:self.window];
            [[NSNotificationCenter defaultCenter] postNotificationName:TCPConnectStateNotification object:KCTCPConnectingNotification];
        }
            break;
        case KCTConnectionStatus_ReConnectFail:
        {
            //[MBProgressHUD hideAllHUDsForView:self.window animated:YES];
            [[NSNotificationCenter defaultCenter] postNotificationName:TCPConnectStateNotification object:KCTCPDisConnectNotification];
        }
            break;
            /*
             
             case KCTConnectionStatus_ConnectFail:
             [[NSNotificationCenter defaultCenter] postNotificationName:TCPConnectStateNotification object:KCTCPDisConnectNotification];
             break;*/
        default:
            break;
    }
}


- (void)didReceiveTransParentData:(KCTTCPTransParent *)objcts
{
    
}

#pragma mark-
#pragma mark-------private---------

- (void)kickOff:(KCTError *)error
{
    // 被踢下线时 防止下次直接后台登录 清空token
    NSString *str = [NSString stringWithFormat:@"%@ code:%d",error.errorDescription,error.code];
    UIAlertView *alert = [[UIAlertView alloc] initWithTitle:@"提示" message:str delegate:nil cancelButtonTitle:@"ok" otherButtonTitles:nil, nil];
    [alert show];
}


- (void) redirectConsoleLogToDocumentFolder {
    
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString *documentsDirectory = [paths objectAtIndex:0];
    NSString *logPath = [documentsDirectory stringByAppendingPathComponent:@"console_log.txt"];
    freopen([logPath cStringUsingEncoding:NSASCIIStringEncoding],"a+",stderr);
}


@end





