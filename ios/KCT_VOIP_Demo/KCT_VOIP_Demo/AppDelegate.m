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


@interface AppDelegate ()<KCTTCPDelegateBase,UNUserNotificationCenterDelegate>
{
    NSString *_callid;
    int _vpsid;
    BOOL _isReceiveCmd;
    BOOL _isGoLoginPage;
}

@property(nonatomic,strong)LoginViewController *loginController;

@end


@implementation AppDelegate

#pragma mark-
#pragma mark---------UIApplication Delegate------------
- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    // Override point for customization after application launch.
    self.window = [[UIWindow alloc] init];
    self.window.frame = [UIScreen mainScreen].bounds;
    self.window.backgroundColor = [UIColor whiteColor];
    
    [self registerAPNS];

    application.applicationIconBadgeNumber = 0;
    NSDictionary * userInfo = [launchOptions objectForKey:UIApplicationLaunchOptionsRemoteNotificationKey];
    
    self.pageIndex = -1;
    LoginViewController *controller = [[LoginViewController alloc] init];
    ExNavigationController *nav = [[ExNavigationController alloc] initWithRootViewController:controller];
    self.window.rootViewController = nav;
    self.loginController = controller;
    
    [self.window makeKeyAndVisible];
    
    //设置tcp代理
    [[KCTTcpClient sharedTcpClientManager] setTcpDelegate:self];
    [[KCTTcpClient sharedTcpClientManager] setPushAppid:kAccessId appKey:kAccessKey launchOptions:launchOptions];
    /**
     初始化voip功能类
     */
    KCTFuncEngine *funcEngine = [KCTFuncEngine getInstance];
    //dialling_tone
    NSString *musicPath = [[NSBundle mainBundle] pathForResource:@"dialling_tone" ofType:@"pcm"];
    [funcEngine setIncomingCallMusicPath:musicPath];

    if (userInfo) {
        _isGoLoginPage = YES;
        [controller autoLogin:self.pageIndex];
        NSDictionary *apsDic = [userInfo objectForKey:@"aps"];
        NSString *callid = [apsDic objectForKey:@"callid"];
        NSNumber *numVpsid = [apsDic objectForKey:@"vpsid"];
        int vpsid = [numVpsid intValue];
        _callid = callid;
        _vpsid = vpsid;
    }
    
    
    /**
    
     
     设置音视频编解码
     */
    [[KCTVOIPViewEngine getInstance] setVideoDecAndVideoEnc];
    _isReceiveCmd = NO;
    return YES;
}



#pragma mark-
#pragma mark-------push ----------
- (void)application:(UIApplication *)application didRegisterForRemoteNotificationsWithDeviceToken:(NSData *)deviceToken {
    
    // 设置推送环境
    [[KCTTcpClient sharedTcpClientManager] setPushEnvironment:KCTPushEnvironment_Production deviceToken:deviceToken];
}

- (void)application:(UIApplication *)application didFailToRegisterForRemoteNotificationsWithError:(NSError *)error {
    NSLog(@"[XGDemo] register APNS fail.\n[XGDemo] reason : %@", error);
}


/**
 收到通知的回调
 
 @param application  UIApplication 实例
 @param userInfo 推送时指定的参数
 */
- (void)application:(UIApplication *)application didReceiveRemoteNotification:(NSDictionary *)userInfo {
    
    if (!_isGoLoginPage) {
        [self.loginController autoLogin:self.pageIndex];
    }
    
    
    KCTFuncEngine *funcEngine = [KCTFuncEngine getInstance];
    if (userInfo) {
        NSDictionary *apsDic = [userInfo objectForKey:@"aps"];
        NSString *callid = [apsDic objectForKey:@"callid"];
        NSNumber *numVpsid = [apsDic objectForKey:@"vpsid"];
        int vps = [numVpsid intValue];
        
        if ([KCTTcpClient sharedTcpClientManager].login_isConnected) {
            if (_isReceiveCmd) {
                //已经收到来电信令，不需要上报push
            } else {
                [funcEngine incomingRspWhenBackground:callid vpsid:vps];
            }
        } else {
            _callid = callid;
            _vpsid = vps;
        }
    }
}


/**
 收到静默推送的回调
 
 @param application  UIApplication 实例
 @param userInfo 推送时指定的参数
 @param completionHandler 完成回调
 */
- (void)application:(UIApplication *)application didReceiveRemoteNotification:(NSDictionary *)userInfo fetchCompletionHandler:(void (^)(UIBackgroundFetchResult))completionHandler {
    
//    UILocalNotification *backgroudMsg = [[UILocalNotification alloc] init];
//    backgroudMsg.alertBody= @"You receive a new call";
//    backgroudMsg.soundName = @"ring.caf";
//    backgroudMsg.applicationIconBadgeNumber = [[UIApplication sharedApplication]applicationIconBadgeNumber] + 1;
//    [[UIApplication sharedApplication] presentLocalNotificationNow:backgroudMsg];
    
    completionHandler(UIBackgroundFetchResultNewData);
}


#pragma mark-
#pragma mark----------ios10 Api------------
// iOS 10 新增 API
// iOS 10 会走新 API, iOS 10 以前会走到老 API
#if __IPHONE_OS_VERSION_MAX_ALLOWED >= __IPHONE_10_0
// App 用户点击通知的回调
// 无论本地推送还是远程推送都会走这个回调
- (void)userNotificationCenter:(UNUserNotificationCenter *)center didReceiveNotificationResponse:(UNNotificationResponse *)response withCompletionHandler:(void(^)())completionHandler {
    
    if (!_isGoLoginPage) {
        [self.loginController autoLogin:self.pageIndex];
    }
    
    NSDictionary *userInfo = response.notification.request.content.userInfo;
    KCTFuncEngine *funcEngine = [KCTFuncEngine getInstance];
    if (userInfo) {
        NSDictionary *apsDic = [userInfo objectForKey:@"aps"];
        NSString *callid = [apsDic objectForKey:@"callid"];
        NSNumber *numVpsid = [apsDic objectForKey:@"vpsid"];
        int vps = [numVpsid intValue];
        
        if ([KCTTcpClient sharedTcpClientManager].login_isConnected) {
            if (_isReceiveCmd) {
                //已经收到来电信令，不需要上报push
            } else {
                [funcEngine incomingRspWhenBackground:callid vpsid:vps];
            }
        } else {
            _callid = callid;
            _vpsid = vps;
        }
    }
    
    completionHandler();
}

// App 在前台弹通知需要调用这个接口
- (void)userNotificationCenter:(UNUserNotificationCenter *)center willPresentNotification:(UNNotification *)notification withCompletionHandler:(void (^)(UNNotificationPresentationOptions options))completionHandler {
    
//    if ([KCTTcpClient sharedTcpClientManager].login_isConnected) {
//        
//    } else {
//        if (!_isGoLoginPage) {
//            [self.loginController autoLogin:self.pageIndex];
//        }
//    }
    //completionHandler(UNNotificationPresentationOptionBadge | UNNotificationPresentationOptionSound | UNNotificationPresentationOptionAlert);
}
#endif


- (void)applicationWillResignActive:(UIApplication *)application {
    // Sent when the application is about to move from active to inactive state. This can occur for certain types of temporary interruptions (such as an incoming phone call or SMS message) or when the user quits the application and it begins the transition to the background state.
    // Use this method to pause ongoing tasks, disable timers, and invalidate graphics rendering callbacks. Games should use this method to pause the game.
}


- (void)applicationDidEnterBackground:(UIApplication *)application {
    // Use this method to release shared resources, save user data, invalidate timers, and store enough application state information to restore your application to its current state in case it is terminated later.
    // If your application supports background execution, this method is called instead of applicationWillTerminate: when the user quits.
}


- (void)applicationWillEnterForeground:(UIApplication *)application {
    // Called as part of the transition from the background to the active state; here you can undo many of the changes made on entering the background.
    UIApplicationState state = application.applicationState;//获取应用程序状态，（是前台运行还是后台运行）
    
    NSMutableDictionary * notifDic = [KCTUserDefaultManager GetLocalDataObject:KCTNotiLocalNotification];
    
    if (notifDic==nil) {
        _isReceiveCmd = NO;
        return;
    }
    //如果在前台运行
    if (state == UIApplicationStateActive || state == UIApplicationStateInactive){//弹出alert
        if (notifDic.allKeys.count==0) {
            //            [self showSignInNotification:notification.alertBody];
        }else{
            _isReceiveCmd = YES;
            [[KCTVOIPViewEngine getInstance] InactiveCall:notifDic];
        }
    }else{
        if (notifDic.allKeys.count==0) {
            _isReceiveCmd = NO;
            return;
        }else{
            _isReceiveCmd = YES;
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
            if (_callid) {
                KCTFuncEngine *funcEngine = [KCTFuncEngine getInstance];
                [funcEngine incomingRspWhenBackground:_callid vpsid:_vpsid];
                _callid = nil;
                _vpsid = 0;
            }
            
            [[NSNotificationCenter defaultCenter] postNotificationName:TCPConnectStateNotification object:KCTCPDidConnectNotification];
        }
            break;
        case KCTConnectionStatus_ReConnectSuccess:
        {
            //[MBProgressHUD hideAllHUDsForView:self.window animated:YES];
            if (_callid) {
                KCTFuncEngine *funcEngine = [KCTFuncEngine getInstance];
                [funcEngine incomingRspWhenBackground:_callid vpsid:_vpsid];
                _callid = nil;
                _vpsid = 0;
            }
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

//本app不支持ios8以下
//registerPush
- (void)registerAPNS {
    float sysVer = [[[UIDevice currentDevice] systemVersion] floatValue];
#if __IPHONE_OS_VERSION_MAX_ALLOWED >= __IPHONE_10_0
    if (sysVer >= 10) {
        // iOS 10
        [self registerPush10];
    } else if (sysVer >= 8) {
        // iOS 8-9
        [self registerPush8to9];
    } else {
        // before iOS 8
    }
#else
    if (sysVer < 8) {
        // before iOS 8
        [self registerPushBefore8];
    } else {
        // iOS 8-9
        [self registerPush8to9];
    }
#endif
}

- (void)registerPush10{
#if __IPHONE_OS_VERSION_MAX_ALLOWED >= __IPHONE_10_0
    UNUserNotificationCenter *center = [UNUserNotificationCenter currentNotificationCenter];
    center.delegate = self;
    
    
    [center requestAuthorizationWithOptions:UNAuthorizationOptionBadge | UNAuthorizationOptionSound | UNAuthorizationOptionAlert completionHandler:^(BOOL granted, NSError * _Nullable error) {
        if (granted) {
        }
    }];
    [[UIApplication sharedApplication] registerForRemoteNotifications];
#endif
}

- (void)registerPush8to9{
    UIUserNotificationType types = UIUserNotificationTypeBadge | UIUserNotificationTypeSound | UIUserNotificationTypeAlert;
    UIUserNotificationSettings *mySettings = [UIUserNotificationSettings settingsForTypes:types categories:nil];
    [[UIApplication sharedApplication] registerUserNotificationSettings:mySettings];
    [[UIApplication sharedApplication] registerForRemoteNotifications];
}


@end
