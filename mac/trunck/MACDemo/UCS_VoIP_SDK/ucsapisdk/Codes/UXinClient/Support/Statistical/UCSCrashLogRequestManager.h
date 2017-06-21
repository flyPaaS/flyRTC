//
//  UCSCrashLogRequestManager.h
//  ucsvideosdk
//
//  Created by macmini on 16/1/12.
//  Copyright © 2016年 ucs. All rights reserved.
//

#import <Foundation/Foundation.h>



@interface UCSCrashLogRequestManager : NSObject<UCSASIHTTPRequestDelegate>

@property (retain,nonatomic)NSString * callId;
@property (retain,nonatomic)NSString * callee; //被叫号码
@property (retain,nonatomic)NSString * callMode;//呼叫模式
@property (retain,nonatomic)NSString * fileName; //文件地址
@property (assign,nonatomic)id delegate;
@property (assign,nonatomic)SEL select;
// 上传crash日志
- (void)uploadCrashRequest;

- (void)stopCrashRequest;

+ (NSString*)deviceString;


@end
