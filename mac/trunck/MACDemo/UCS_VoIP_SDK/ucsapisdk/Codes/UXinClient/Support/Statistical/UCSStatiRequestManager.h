//
// UCSStatiRequestManager.h
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

// UCSStatiRequestManagerDelegate
@protocol UCSStatiRequestManagerDelegate <NSObject>
@optional
// 网络请求成功，收到返回
// nResult : 返回值
- (void) onLoginDidFinishLoading:(int) nResult;
// 网络请求出现错误
- (void) onLoginDidFailWithError:(NSError *)error;
@end

@interface UCSStatiRequestManager : NSObject
{
    id                          m_delegate;                  // delegate

}

@property (nonatomic, retain) UCSASIHTTPRequest *m_pUCSASIHTTPRequest;   // connection


+(UCSStatiRequestManager*)instance;


// 上报手机数据
- (void) reportPhoneInfoForStati:(NSString*)clientNumber;

// 上报错误码数据
- (void) reportErrorCodeInfoForStati:(NSString*)interfaceName withinterfaceDesc:(NSString*)interfaceDesc withErrorCode:(NSString*)errorCode withErrorDesc:(NSString*)errorDesc withClientNumber:(NSString*)clientNumber;


// 停止网络请求
- (void) stopRequest;

@end
