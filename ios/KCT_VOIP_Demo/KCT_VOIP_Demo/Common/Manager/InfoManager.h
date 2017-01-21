//
//  InfoManager.h
//  KCT_VOIP_Demo_1.0
//
//  Created by KCT on 15/4/22.
//  Copyright (c) 2015年 KCT. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "Singleton.h"
#import <UIKit/UIKit.h>


@interface InfoManager : NSObject
singleton_interface(InfoManager)


@property (nonatomic ,copy) NSString * imtoken; //登录token
@property (nonatomic ,copy) NSString * nickname;
@property (nonatomic,copy) NSString * phone;
@property (nonatomic, copy) NSString * portraituri; //头像URLString
@property (nonatomic, copy) NSString * code;
@property(nonatomic,copy)NSString *loginUserName;//登录账号
@property(nonatomic,copy)NSString *loginPwd;//登录密码
@property(nonatomic,assign,getter=isLogin)BOOL login;//上次是否成功登陆，如果是，这次启动就后台登陆；不是，需要重新输入密码

//存取imtoken
- (NSString *)imtoke:(NSString *)tokenKey;
- (void)setImtoken:(NSString *)tokenKey WithImToken:(NSString *) imToken;

/**
 数据保存到沙盒，保存运行内存与沙盒的数据同步
 */
-(void)synchronizeToSandBox;

/*
 *程序一启动时从沙盒获取数据
 */
-(void)loadDataFromSandBox;



- (BOOL)isMessageNotificationOpen;
- (BOOL)isVoiceOpen;
- (BOOL)isShakeOpen;


- (void)saveMessageNotificationBool:(BOOL) ret;
- (void)saveVoiceOpenBool:(BOOL) ret;
- (void)saveShakeOpenBool:(BOOL) ret;


//保存、获取 聊天框背景图片
- (BOOL)saveChatviewBgImage:(UIImage *) image targetId:(NSString *) targetId;
- (UIImage *)chatViewBgImageWithTargetId:(NSString *) targetId;

// 当前默认的背景图序号
- (void)saveCurrentImageIndex:(NSInteger) index TargetId:(NSString *) targetId;
- (NSInteger)CurrentImageIndexWithTargetId:(NSString *) targetId;

@end
