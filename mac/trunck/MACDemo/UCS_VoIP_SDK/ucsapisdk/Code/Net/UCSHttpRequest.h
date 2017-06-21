//
//  UCSHttpRequest.h
//  UXinClient
//
//  Created by Liam Peng on 12-5-8.
//  Copyright (c) 2012年 UXIN. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "UCSASIHTTPRequest.h"

@class UCSHttpRequest;


// UCSHttpRequestDelegate
@protocol UCSHttpRequestDelegate <NSObject>
@optional
// 网络请求成功，收到返回
// nResult : 返回值
- (void) onUCSHttpRequestDidFinishLoading:(UCSHttpRequest *) pUCSHttpRequest withData:(NSData *) data withResult:(int) nResult;
// 网络请求出现错误
- (void) onUCSHttpRequestDidFailWithError:(UCSHttpRequest *) pUCSHttpRequest withError:(NSError *) error;
@end

// UXin Http Request
@interface UCSHttpRequest : NSObject
{
    UCSASIHTTPRequest      *m_pUCSASIHTTPRequest;
    
    id                  m_delegate;
    BOOL                m_bPost;
    BOOL                m_bLogined;
    BOOL                m_bWillReturnJson;      // 是否将返回Json
}

@property (nonatomic, readwrite)    BOOL    isNeedLoginWhenSessionOutDate;
@property (nonatomic, readwrite)    BOOL    bPwdErrorAlerShowed;
@property (nonatomic, retain)       NSString *m_strURL;
@property (nonatomic, assign)       id      delegate;

// 初始化请求方式，Post或者Get
- (id) initWithRequestStyle:(BOOL) bPost isWillReturnJson:(BOOL) bJson;
// 设置URL及delegate
- (void) setURLAndDelegate:(NSString *) strURL withDelegate:(id) delegate;
// 开始异步请求
- (void) startAsyncRequest;
// 停止请求
- (void) stopRequest;
// set PostValue
- (void)setPostValue:(id <NSObject>)value forKey:(NSString *)key;

- (void)addRequestHeader:(NSString *)header value:(NSString *)value;
- (void)addPostData:(id)data withFileName:(NSString *)fileName andContentType:(NSString *)contentType forKey:(NSString *)key;

@end
