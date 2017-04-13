//
//  MBProgressHUD+Add.h

//
//  Created by KCT on 13-4-18.
//  Copyright (c) 2013年 itcast. All rights reserved.
//

#import "MBProgressHUD.h"

@interface MBProgressHUD (Add)

//快速显示一个text
+ (void)show:(NSString *)text icon:(NSString *)icon view:(UIView *)view;

+ (void)showError:(NSString *)error toView:(UIView *)view;
+ (void)showSuccess:(NSString *)success toView:(UIView *)view;

+ (MBProgressHUD *)showMessage:(NSString *)message toView:(UIView *)view;


+ (void)showSuccess:(NSString *)success;
+ (void)showError:(NSString *)error;

+ (MBProgressHUD *)showMessage:(NSString *)message;

+ (void)hideHUDForView:(UIView *)view;
+ (void)hideHUD;



//登陆界面显示用户名、密码不能为空
+ (void)showUserNameORPassWordNotNull:(NSString *) message toView:(UIView *) view;
//聊天界面发送时显示网络
+ (void)showNoNetWork:(NSString *) text toView:(UIView *)view;
//屏幕中间显示文本，过一段时间消失
+ (void)showText:(NSString *) message toView:(UIView *) view;


// 通讯录显示索引
+ (void)showSectionTitle:(NSString *) title toView:(UIView *)view;

//显示一个下载圈
+ (MBProgressHUD *)showDownLoadSqureToView: (UIView*) view;


@end
