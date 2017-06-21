//
//  LoginManager.m
//  KCT_VOIP_Demo
//
//  Created by KCMac on 2017/4/10.
//  Copyright © 2017年 KCMac. All rights reserved.
//

#import "LoginManager.h"

@interface LoginManager ()

@property(nonatomic,strong) NSMutableArray *controllerArrays;


@end

static LoginManager *g_loginInstace;

@implementation LoginManager

+(instancetype)sharedLoginManager
{
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        g_loginInstace = [[LoginManager alloc] init];
    });
    return g_loginInstace;
}

+(id)allocWithZone:(struct _NSZone *)zone
{
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        g_loginInstace = [super allocWithZone:zone];
    });
    return g_loginInstace;
}

- (void)addController:(UIViewController *)controller {
    if (!self.controllerArrays) {
        _controllerArrays = [[NSMutableArray alloc] initWithCapacity:3];
    }
    if ([self.controllerArrays containsObject:controller]) {
        NSLog(@"已经在数组里");
    }
    else
    {
        [self.controllerArrays addObject:controller];
    }
}

- (void)callAutoLogin {
    LoginViewController *loginContrl = (LoginViewController *)[self.controllerArrays objectAtIndex:0];
    loginContrl.isFromAdressBook = YES;
    switch (self.currentPageIndex) {
        case appPageIndexLogin:
        {
            [loginContrl loginWithUser:@"119140589@qq.com" pwd:@"kc123456"];
        }
            break;
        case appPageIndexHelp:
        {
            [loginContrl loginWithUser:@"119140589@qq.com" pwd:@"kc123456"];
        }
            break;
        case appPageIndexCalling:
        {
            [loginContrl.callController autoConnectService];
        }
            break;
        case appPageIndexCalled:
        {
            
        }
            break;
        default:
            break;
    }
}

- (void)autoConnectCS {
    
    if (self.controllerArrays.count == 0) {
        return;
    }
    if (self.controllerArrays.count >= 3) {
        return;
    }
    
    LoginViewController *loginContrl = (LoginViewController *)[self.controllerArrays objectAtIndex:0];
    loginContrl.isAutoLogin = YES;
    switch (self.currentPageIndex) {
        case appPageIndexLogin:
        {
            [loginContrl loginWithUser:@"119140589@qq.com" pwd:@"kc123456"];
        }
            break;
        case appPageIndexHelp:
        {
            [loginContrl loginWithUser:@"119140589@qq.com" pwd:@"kc123456"];
        }
            break;
        case appPageIndexCalling:
        {
            [loginContrl.callController autoConnectService];
        }
            break;
        case appPageIndexCalled:
        {
            
        }
            break;
        default:
            break;
    }
}


- (void)autoPushGUI{
    if (self.controllerArrays.count == 0) {
        return;
    }
    LoginViewController *loginContrl = (LoginViewController *)[self.controllerArrays objectAtIndex:0];
    loginContrl.isAutoLogin = YES;
    switch (self.currentPageIndex) {
        case appPageIndexLogin:
        {
            if (self.controllerArrays.count >= 2) {
                CallingListViewController *callingContrl = (CallingListViewController *)[self.controllerArrays objectAtIndex:1];
                [self.nav pushViewController:callingContrl animated:YES];
            }
            if (self.controllerArrays.count >= 3) {
                CalledListViewController *calledContrl = (CalledListViewController *)[self.controllerArrays objectAtIndex:2];
                [self.nav pushViewController:calledContrl animated:YES];
            }
        }
            break;
        case appPageIndexHelp:
        {
            [loginContrl dismissViewControllerAnimated:YES completion:nil];
            if (self.controllerArrays.count >= 2) {
                CallingListViewController *callingContrl = (CallingListViewController *)[self.controllerArrays objectAtIndex:1];
                [self.nav pushViewController:callingContrl animated:YES];
            }
            if (self.controllerArrays.count >= 3) {
                CalledListViewController *calledContrl = (CalledListViewController *)[self.controllerArrays objectAtIndex:2];
                [self.nav pushViewController:calledContrl animated:YES];
            }
        }
            break;
        case appPageIndexCalling:
        {
            if (self.controllerArrays.count >= 3) {
                CalledListViewController *calledContrl = (CalledListViewController *)[self.controllerArrays objectAtIndex:2];
                [self.nav pushViewController:calledContrl animated:YES];
            }
        }
            break;
        case appPageIndexCalled:
        {
            
        }
            break;
        default:
            break;
    }
}


- (NSMutableArray *)getControllerArrays
{
    return self.controllerArrays;
}


#pragma mark-
#pragma mark---------private-----





@end
