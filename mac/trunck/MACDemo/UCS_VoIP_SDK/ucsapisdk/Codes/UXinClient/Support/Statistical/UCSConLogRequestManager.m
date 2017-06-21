//
//  UCSConLogRequestManager.m
//  ucsvideosdk
//
//  Created by macmini on 16/1/13.
//  Copyright © 2016年 ucs. All rights reserved.
//

#import "UCSConLogRequestManager.h"
#import "UCSASIFormDataRequest.h"

@implementation UCSConLogRequestManager
{
    UCSASIFormDataRequest * _conversationRequest;
}

- (void)dealloc{
    
    self.SDKLogStr = nil;
    
    [_conversationRequest clearDelegatesAndCancel];
    _conversationRequest = nil;
    [super dealloc];
}

- (void)uploadConversationRequest{
    NSURL * url = nil;
    
    if ([UCSPublicFunc getIsUseTestServer] && [[UCSPublicFunc getIsUseTestServer] isEqualToString:@"TESTMODE"])//测试
    {
        url = [NSURL URLWithString:UCS_UPLOAD_LOG_TEST];
    }
    else if ([UCSPublicFunc getIsUseTestServer] && [[UCSPublicFunc getIsUseTestServer] isEqualToString:@"DEVMODE"])//DEV 模式
    {
        //开发环境
        url = [NSURL URLWithString:UCS_UPLOAD_LOG_DEV];
    }
    else //正式
    {
        url = [NSURL URLWithString:UCS_UPLOAD_LOG];
    }
    UCSASIFormDataRequest *request = [UCSASIFormDataRequest requestWithURL:url];
    
    
    [request addRequestHeader:@"Content-Type" value:@"application/x-www-form-urlencoded"];
    [request addRequestHeader:@"Accept" value:@"text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8"];
    [request setRequestMethod:@"POST"];
    
    [request setPostValue:self.SDKLogStr forKey:@"log"];

    request.delegate = self;
    [request startAsynchronous];
    _conversationRequest = [request retain];
    
}

- (void)stopConversationRequest{
    [_conversationRequest clearDelegatesAndCancel];
    _conversationRequest = nil;
}

#pragma mark - UCSASIHTTPRequestDelegate
- (void)requestFinished:(UCSASIHTTPRequest *)request{
    NSDictionary * dataDic = [NSJSONSerialization JSONObjectWithData:request.responseData options:NSJSONReadingMutableContainers error:nil];
    if ([dataDic[@"respCode"] isEqualToString:@"000000"]) {
        // 上传成功
    }
//    UXLogAlert(@"会话日志上传---%@",dataDic);
}

- (void)requestFailed:(UCSASIHTTPRequest *)request{
//    UXLogAlert(@"会话日志上传失败");

}



@end
