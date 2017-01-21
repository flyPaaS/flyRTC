//
//  HttpRequestEngine.h
//  KCT_VOIP_Demo
//
//  Created by KCMac on 2017/1/7.
//  Copyright © 2017年 KCMac. All rights reserved.
//

#import <Foundation/Foundation.h>

typedef void(^requestSuccessBlock)(NSDictionary *responseDict);
typedef void(^requestFailBlock)(NSDictionary *responseDict);

@interface HttpRequestEngine : NSObject

+(id)engineInstance;

- (void)login:(NSString *)sid token:(NSString *)token successBlock:(requestSuccessBlock)successBlock failBlock:(requestFailBlock)failBlock;

- (void)getClientList:(NSString *)sid token:(NSString *)token appId:(NSString *)appId successBlock:(requestSuccessBlock)successBlock failBlock:(requestFailBlock)failBlock;

@end
