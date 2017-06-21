//
//  AppDelegate.m
//  KCT_VOIP_Demo
//
//  Created by KCMac on 2017/5/22.
//  Copyright © 2017年 flypass. All rights reserved.
//

#import "AppDelegate.h"

@interface AppDelegate ()<KCTTCPDelegateBase>
{
    
}
@property(nonatomic,strong) LoginWindowController *loginWin;

@end

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
    // Insert code here to initialize your application
    
    //设置tcp代理
    [[KCTTcpClient sharedTcpClientManager] setTcpDelegate:self];
    
    // 设置音视频编解码
    [self setVideoDecAndVideoEnc];
    
   _loginWin = [[LoginWindowController alloc] initWithWindowNibName:@"LoginWindowController"];
    [self.loginWin.window center];
    [self.loginWin.window orderFront:nil];
    
    //[self redirectConsoleLogToDocumentFolder];
}


- (void)applicationWillTerminate:(NSNotification *)aNotification {
    // Insert code here to tear down your application
}


- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)sender
{
#pragma unused (sender)
    return YES;
}


#pragma mark---funtion
- (void) redirectConsoleLogToDocumentFolder {
    
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString *documentsDirectory = [paths objectAtIndex:0];
    NSString *logPath = [documentsDirectory stringByAppendingPathComponent:@"qinglin_console_log.txt"];
    freopen([logPath cStringUsingEncoding:NSASCIIStringEncoding],"a+",stderr);
}



- (void)setVideoDecAndVideoEnc{
    //自定义视频编码参数
    KCTVideoEncAttr *vEncAttr = [[KCTVideoEncAttr alloc] init] ;
    
    vEncAttr.isUseCustomEnc = YES;
    vEncAttr.uWidth = 360;
    vEncAttr.uHeight = 480;
    vEncAttr.uStartBitrate = 200;
    vEncAttr.uMaxFramerate = 15;
    
    
    
    //自定义视频解码参数
    KCTVideoDecAttr *vDecAttr = [[KCTVideoDecAttr alloc] init] ;
    
    vDecAttr.isUseCustomDec = YES;
    vDecAttr.uWidth = 320;
    vDecAttr.uHeight = 240;
    
    
    [[VoipEngine getInstance] setVideoAttr:vEncAttr andVideoDecAttr:vDecAttr];
    
    
    
    
    
    //设置视频来电时是否支持预览。
    [[VoipEngine getInstance] setCameraPreViewStatu:NO];
}



@end
