//
//  AppDelegate.m
//  KCT_VOIP_Demo
//
//  Created by KCMac on 2017/1/7.
//  Copyright © 2017年 KCMac. All rights reserved.
//

#import "AppDelegate.h"
#import "GUIHeaders.h"
#import "KCSDKHeaders.h"
#import "KCTFuncEngine.h"
#import "KCTVOIPViewEngine.h"


@interface AppDelegate ()<KCTTCPDelegateBase>

@end

@implementation AppDelegate


- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    // Override point for customization after application launch.
    self.window = [[UIWindow alloc] init];
    self.window.frame = [UIScreen mainScreen].bounds;
    self.window.backgroundColor = [UIColor whiteColor];
    
    GetClientViewController *controller = [[GetClientViewController alloc] init];
    ExNavigationController *nav = [[ExNavigationController alloc] initWithRootViewController:controller];
    self.window.rootViewController = nav;
    
    [self.window makeKeyAndVisible];
    
    
    //设置tcp代理
    [[KCTTcpClient sharedTcpClientManager] setTcpDelegate:self];
    /**
     初始化voip功能类
     */
    [KCTFuncEngine getInstance];
    /**
     
     设置音视频编解码
     */
    [[KCTVOIPViewEngine getInstance] setVideoDecAndVideoEnc];
    
    return YES;
}


- (void)didConnectionStatusChanged:(KCTConnectionStatus)connectionStatus error:(KCTError *)error
{
    switch (connectionStatus) {
        case KCTConnectionStatus_BeClicked:
            [self kickOff:error];
            break;
            /*
        case KCTConnectionStatus_ReConnectFail:
            [[NSNotificationCenter defaultCenter] postNotificationName:TCPConnectStateNotification object:KCTCPDisConnectNotification];
            break;
        case KCTConnectionStatus_StartReConnect:
            [[NSNotificationCenter defaultCenter] postNotificationName:TCPConnectStateNotification object:KCTCPConnectingNotification];
            break;
        case KCTConnectionStatus_ReConnectSuccess:
            [[NSNotificationCenter defaultCenter] postNotificationName:TCPConnectStateNotification object:KCTCPDidConnectNotification];
            break;
        case KCTConnectionStatus_loginSuccess:
            [[NSNotificationCenter defaultCenter] postNotificationName:TCPConnectStateNotification object:KCTCPDidConnectNotification];
            break;
        case KCTConnectionStatus_ConnectFail:
            [[NSNotificationCenter defaultCenter] postNotificationName:TCPConnectStateNotification object:KCTCPDisConnectNotification];
            break;*/
        default:
            break;
    }
}

- (void)kickOff:(KCTError *)error
{
    // 被踢下线时 防止下次直接后台登录 清空token
    NSString *str = [NSString stringWithFormat:@"%@ code:%d",error.errorDescription,error.code];
    UIAlertView *alert = [[UIAlertView alloc] initWithTitle:@"提示" message:str delegate:nil cancelButtonTitle:@"ok" otherButtonTitles:nil, nil];
    [alert show];
}

- (void)didReceiveTransParentData:(KCTTCPTransParent *)objcts
{
    
}

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
}


- (void)applicationDidBecomeActive:(UIApplication *)application {
    // Restart any tasks that were paused (or not yet started) while the application was inactive. If the application was previously in the background, optionally refresh the user interface.
}


- (void)applicationWillTerminate:(UIApplication *)application {
    // Called when the application is about to terminate. Save data if appropriate. See also applicationDidEnterBackground:.
}


@end
