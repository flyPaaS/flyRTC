//
//  UXinSoundManager.h
//  UXinClient
//
//  Created by Liam on 13-11-19.
//  Copyright (c) 2013年 UXIN CO. All rights reserved.
//

#import <Foundation/Foundation.h>




@interface UXinSoundManager : NSObject

+ (UXinSoundManager *) sharedInstance;

// 启动和暂停扬声器
- (void) LoudSpeaker:(BOOL) bOpen;

//  获取系统音量
- (float) getSystemVolume;

// 设置系统音量
- (void) setSystemVolume:(float) systemVolume;



- (void) switchToSpeaker:(BOOL)bToSpeaker;




@end
