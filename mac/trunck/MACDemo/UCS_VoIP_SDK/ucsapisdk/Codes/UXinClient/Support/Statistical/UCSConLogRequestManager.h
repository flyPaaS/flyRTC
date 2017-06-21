//
//  UCSConLogRequestManager.h
//  ucsvideosdk
//
//  Created by macmini on 16/1/13.
//  Copyright © 2016年 ucs. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface UCSConLogRequestManager : NSObject<UCSASIHTTPRequestDelegate>

@property (copy,nonatomic)NSString * SDKLogStr; //会话sdk日志内容
@property (assign,nonatomic)id delegate;
@property (assign,nonatomic)SEL select;
// 上传会话日志
- (void)uploadConversationRequest;

- (void)stopConversationRequest;

@end
