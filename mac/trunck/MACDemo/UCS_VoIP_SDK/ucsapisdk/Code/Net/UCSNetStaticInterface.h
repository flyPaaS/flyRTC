//
//  UCSNetStaticInterface.h
//  UXinClient
//
//  Created by Liam on 13-11-26.
//  Copyright (c) 2013年 UXIN CO. All rights reserved.
//
#import "UCSHttpRequest.h"
#import <Foundation/Foundation.h>

// 静态接口
@interface UCSNetStaticInterface : NSObject
{
    UCSHttpRequest *m_pUCSHttpRequestSvrAddr;
}

@property (nonatomic,strong)NSTimer * getRtppTimer; //用于拉取rtpp列表 15分钟拉取一次rtpp。 add by WLS 20150819


+ (UCSNetStaticInterface *) shareInstance;


// 服务器地址
- (void) getTheSvrAddr;

//停止定时拉取rtpp列表 WLS 20150915
- (void)stopGetRtpp;

@end
