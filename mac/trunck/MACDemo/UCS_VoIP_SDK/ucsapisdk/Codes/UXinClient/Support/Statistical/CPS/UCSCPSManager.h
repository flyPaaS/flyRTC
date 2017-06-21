//
// UCSCPSManager.h
//  KCTApiSdk
//
//  Created by wenqinglin on 14-4-9.
//  Copyright (c) 2014年 ucs. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "UCSASIHTTPRequest.h"


#define  STORELOGINTYPE     [UserDefaultManager GetLocalDataString:DATA_STORE_LOGIN_TYPE]
#define  LOGINTYPESHOW(x)   [NSString stringWithFormat:@"%d", (x)]]
#define  STOREREQUESTTYPE   [UserDefaultManager GetLocalDataString:DATA_STORE_REQUEST_TYPE]

#define kSetICE2FEC2EMODEL2PRTPNotification					@"kSetICE2FEC2EMODEL2PRTPNotification"

// UCSCPSManagerDelegate
@protocol UCSCPSManagerDelegate <NSObject>
@optional
// 网络请求成功，收到返回
// nResult : 返回值
- (void) onLoginDidFinishLoading:(int) nResult;
// 网络请求出现错误
- (void) onLoginDidFailWithError:(NSError *)error;
@end

@interface UCSCPSManager : NSObject
{
    id                          m_delegate;                  // delegate

}

@property (nonatomic, retain) UCSASIHTTPRequest *m_pUCSASIHTTPRequest;   // connection


+(UCSCPSManager*)instance;


// 获取策略参数
- (void)  getCPSParameter;


// 停止网络请求
- (void) stopRequest;

@end
