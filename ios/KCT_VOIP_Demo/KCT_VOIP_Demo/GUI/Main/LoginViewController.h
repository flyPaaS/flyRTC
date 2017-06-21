//
//  LoginViewController.h
//  KCT_VOIP_Demo
//
//  Created by KCMac on 2017/1/7.
//  Copyright © 2017年 KCMac. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "CallingListViewController.h"

@interface LoginViewController : UIViewController

@property(nonatomic,assign) BOOL isAutoLogin;
@property(weak,nonatomic)CallingListViewController *callController;
@property(nonatomic,assign) BOOL isFromAdressBook;

- (void)loginWithUser:(NSString *)userId pwd:(NSString *)pwd;

@end
