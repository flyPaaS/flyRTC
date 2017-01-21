//
//  InfoManager.m
//  KCT_VOIP_Demo_1.0
//
//  Created by KCT on 15/4/22.
//  Copyright (c) 2015年 KCT. All rights reserved.
//

#import "InfoManager.h"

#define IMTOKENKEY @"imToken"
#define NICKNAMEKEY @"nickName"
#define PHONEKEY @"phone"
#define PORTRAITURIKEY @"portraituri"
#define CODEKEY @"code"

//#define UserNameKey @"USER_NAME"
//#define PwdKey @"PASSWORD"
#define LoginKey @"Login"

#define KMessageNotificationOpen [NSString stringWithFormat:@"%@%@", @"messageNotificationOpen", self.phone]
#define KVoiceOpen [NSString stringWithFormat:@"%@%@", @"voiceOpen", self.phone]
#define KShakeOPen [NSString stringWithFormat:@"%@%@", @"shakeOpen", self.phone]

@implementation InfoManager
singleton_implementation(InfoManager)

- (void)setImtoken:(NSString *)tokenKey WithImToken:(NSString *) imToken{
    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
    [defaults setObject:imToken forKey:tokenKey];
    [defaults synchronize];
}

- (NSString *)imtoke:(NSString *)tokenKey{
    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
    return [defaults valueForKey:tokenKey];
}


-(void)synchronizeToSandBox{
    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
    //    [defaults setObject:self.loginUserName forKey:UserNameKey];
    //    [defaults setObject:self.loginPwd forKey:PwdKey];
    
    [defaults setObject:self.nickname forKey:NICKNAMEKEY];
    [defaults setObject:self.phone forKey:PHONEKEY];
    [defaults setObject:self.portraituri forKey:PORTRAITURIKEY];
    [defaults setObject:self.imtoken forKey:IMTOKENKEY];
    [defaults setObject:self.code forKey:CODEKEY];
    [defaults setBool:self.login forKey:LoginKey];
    
    [defaults synchronize];
}

-(void)loadDataFromSandBox{
    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
    
    self.nickname = [defaults objectForKey:NICKNAMEKEY];
    self.phone = [defaults objectForKey:PHONEKEY];
    self.portraituri = [defaults objectForKey:PORTRAITURIKEY];
    self.imtoken = [defaults objectForKey:IMTOKENKEY];
    self.code = [defaults objectForKey:CODEKEY];
    self.login = [defaults boolForKey:LoginKey];
    
    //    self.loginUserName = [defaults objectForKey:UserNameKey];
    //    self.loginPwd = [defaults objectForKey:PwdKey];
}



- (BOOL)isMessageNotificationOpen{
    BOOL ret = YES;
    NSUserDefaults * defaults = [NSUserDefaults standardUserDefaults];
    if ([[defaults stringForKey:KMessageNotificationOpen] isEqualToString:@"NO"]) {
        ret = NO;
    }
    
    
    return ret;
}

- (BOOL)isVoiceOpen{
    BOOL ret = YES;
    NSUserDefaults * defaults = [NSUserDefaults standardUserDefaults];
    if ([[defaults stringForKey:KVoiceOpen] isEqualToString:@"NO"]) {
        ret = NO;
    }
    
    return ret;
}

- (BOOL)isShakeOpen{
    BOOL ret = YES;
    NSUserDefaults * defaults = [NSUserDefaults standardUserDefaults];
    if ([[defaults stringForKey:KShakeOPen] isEqualToString:@"NO"]) {
        ret = NO;
    }
    
    return ret;
}


- (void)saveMessageNotificationBool:(BOOL)ret{
    NSUserDefaults * defaults = [NSUserDefaults standardUserDefaults];
    if (ret) {
        [defaults setObject:@"YES" forKey:KMessageNotificationOpen];
    }else{
        [defaults setObject:@"NO" forKey:KMessageNotificationOpen];
    }
    [defaults synchronize];
}

- (void)saveShakeOpenBool:(BOOL)ret{
    NSUserDefaults * defaults = [NSUserDefaults standardUserDefaults];
    if (ret) {
        [defaults setObject:@"YES" forKey:KShakeOPen];
    }else{
        [defaults setObject:@"NO" forKey:KShakeOPen];
    }
    [defaults synchronize];
}

- (void)saveVoiceOpenBool:(BOOL)ret{
    NSUserDefaults * defaults = [NSUserDefaults standardUserDefaults];
    if (ret) {
        [defaults setObject:@"YES" forKey:KVoiceOpen];
    }else{
        [defaults setObject:@"NO" forKey:KVoiceOpen];
    }
    [defaults synchronize];
}


#pragma mark 保存、获取聊天背景图片

//压缩图片质量
+(UIImage *)reduceImage:(UIImage *)image percent:(float)percent
{
    NSData *imageData = UIImageJPEGRepresentation(image, percent);
    UIImage *newImage = [UIImage imageWithData:imageData];
    return newImage;
}
- (BOOL)saveChatviewBgImage:(UIImage *) image targetId:(NSString *) targetId{
    
    NSString * fileName = [NSString stringWithFormat:@"%@%@", @"chatbackBgImage", targetId];
    NSArray* paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString* path = [paths objectAtIndex:0];
    NSString* filePath = [path stringByAppendingPathComponent:fileName];
    NSData *imageData = UIImageJPEGRepresentation(image, 0.00001);
    BOOL ret = [imageData writeToFile:filePath atomically:YES];
    return ret;
}

- (UIImage *)chatViewBgImageWithTargetId:(NSString *) targetId{
    NSString * fileName = [NSString stringWithFormat:@"%@%@", @"chatbackBgImage", targetId];
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString *path = [paths objectAtIndex:0];
    NSString* filePath = [path stringByAppendingPathComponent:fileName];
    NSError* err = [[NSError alloc] init];
    NSData* data = [[NSData alloc] initWithContentsOfFile:filePath
                                                  options:NSDataReadingMapped
                                                    error:&err];
    UIImage* img = nil;
    if(data != nil)
    {
        img = [[UIImage alloc] initWithData:data];
    }
    else
    {
        img = [UIImage imageNamed:@"chat_backgroud_sky"];
    }
    return img;
}

#pragma mark 当前默认的背景图序号
- (void)saveCurrentImageIndex:(NSInteger) index TargetId:(NSString *) targetId {
    NSUserDefaults * userDefault = [NSUserDefaults standardUserDefaults];
    NSString * key = [NSString stringWithFormat:@"%@%@%@", @"chatViewBackImage", targetId, self.phone];
    NSString * indexStr = [NSString stringWithFormat:@"%ld", index];
    [userDefault setObject:indexStr forKey:key];
    [userDefault synchronize];
    
}
- (NSInteger)CurrentImageIndexWithTargetId:(NSString *) targetId{
    NSUserDefaults * userDefault = [NSUserDefaults standardUserDefaults];
    NSString * key = [NSString stringWithFormat:@"%@%@%@", @"chatViewBackImage", targetId, self.phone];
    NSString * indexStr = [userDefault stringForKey:key];
    if (!indexStr) {
        return 2; //没有取默认的
    }
    NSInteger index = [indexStr integerValue];
    return index;
}

@end
