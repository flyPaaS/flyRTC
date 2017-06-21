//
//  CallingHttpRequest.h
//  UXinClient
//
//  Created by FengXing on 13-11-15.
//  Copyright (c) 2013年 UXIN CO. All rights reserved.
//

typedef void (^CallRequestBlock)(NSDictionary *json,NSString * callId);

#import <Foundation/Foundation.h>
//#import "CallinfoObject.h"

@interface UCSCallingHttpRequest : NSObject

- (void)callbackRequest:(NSString *)phone showFromNum:(NSString*)fromNum showToNum:(NSString*)toNum onSuccess:(CallRequestBlock)sucess onFailed:(CallRequestBlock)failed;

- (void)cancelCallbackRequest:(NSString *)PhoneNumber onSuccess:(CallRequestBlock)success onFailed:(CallRequestBlock)failed;

- (void)emodelReauest:(NSString *)cphone caller:(NSString *)cuid;

- (void)startAsyncRequest;

@property (nonatomic, copy) CallRequestBlock successBlock;
@property (nonatomic, copy) CallRequestBlock failedBlock;

@property (nonatomic, copy) NSMutableArray *callBackRequestQueue;    /// 回拨请求发送队列，用于取消回拨
@end
