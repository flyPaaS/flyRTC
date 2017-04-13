//
//  LoginManager.h
//  KCT_VOIP_Demo
//
//  Created by KCMac on 2017/4/10.
//  Copyright © 2017年 KCMac. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "defines.h"

@interface LoginManager : NSObject

@property(nonatomic,assign) appPageIndexTag currentPageIndex;
@property(nonatomic,strong) UINavigationController *nav;

+(instancetype)sharedLoginManager;
- (void)addController:(UIViewController *)controller;
- (void)autoConnectCS;
- (void)autoPushGUI;
- (NSMutableArray *)getControllerArrays;

@end
