//
// UCSQualityRequestManager.h
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

// UCSQualityRequestManagerDelegate
@protocol UCSQualityRequestManagerDelegate <NSObject>
@optional
// 网络请求成功，收到返回
// nResult : 返回值
- (void) onLoginDidFinishLoading:(int) nResult;
// 网络请求出现错误
- (void) onLoginDidFailWithError:(NSError *)error;
@end

@interface UCSQualityRequestManager : NSObject
{
    id                          m_delegate;                  // delegate

}

@property (nonatomic, retain) UCSASIHTTPRequest *m_pUCSASIHTTPRequest;   // connection


+(UCSQualityRequestManager*)instance;


// 上报手机数据
- (void) reportQualityForSP:(NSDictionary*)qualData;



// 停止网络请求
- (void) stopRequest;

@end
