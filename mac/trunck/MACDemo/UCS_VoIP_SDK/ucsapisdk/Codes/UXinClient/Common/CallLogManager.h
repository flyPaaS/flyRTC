//
//  CallLogManager.h
//  UXinClient
//
//  Created by Liam on 13-5-30.
//  Copyright (c) 2013年 UXIN. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "UCSHttpRequest.h"
#import "UCSHttpUploadFileManager.h"
#import "UCSASIHTTPRequest.h"

@interface CallLogManager : NSObject
{
    UCSHttpRequest             *m_pUCSHttpRequestGetCallLogConfig;// 获取日志上报配置
    int                         nSize;
    BOOL                        bSubmiting;
}


@property (nonatomic, readwrite)    BOOL        bNeedUploadCalllog;
@property (nonatomic, readwrite)    int         nMaxSize;
@property (nonatomic, copy)         NSString    *strCalled;
@property (nonatomic, retain)       NSMutableDictionary *dicUploadInfo;


+ (CallLogManager *) instance;

// 获取电话日志上传配置
- (void) getCallLogConfig;

- (void) saveCallLogInfo:(NSString *) summary withDetail:(NSString *) detail;

- (void) submitLogToServer;

@end
