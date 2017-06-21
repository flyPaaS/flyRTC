//
//  UCSRecordRequestManager.m
//  ucsvideosdk
//
//  Created by KCT   on 16/1/8.
//  Copyright © 2016年 ucs. All rights reserved.
//

#import "UCSRecordRequestManager.h"
#import "UCSASIFormDataRequest.h"

@interface UCSRecordRequestManager()<UCSASIHTTPRequestDelegate>
{
    
    UCSASIFormDataRequest * _request;
    
}

@end
@implementation UCSRecordRequestManager


- (void)dealloc{
    
    self.callMode = nil;
    self.callId = nil;
    self.callee = nil;
    self.fileName = nil;
    self.caller = nil;
    
    [_request clearDelegatesAndCancel];
    _request = nil;
    
    [super dealloc];
    
}

- (void)uploadRecordRequest{
    
    
    NSURL * url = nil;
    
    if ([UCSPublicFunc getIsUseTestServer] && [[UCSPublicFunc getIsUseTestServer] isEqualToString:@"TESTMODE"])//测试
    {
        url = [NSURL URLWithString:UCS_UPLOAD_RECORD_TEST];
    }
    else if ([UCSPublicFunc getIsUseTestServer] && [[UCSPublicFunc getIsUseTestServer] isEqualToString:@"DEVMODE"])//DEV 模式
    {
        //开发环境
        url = [NSURL URLWithString:UCS_UPLOAD_RECORD_DEV];
    }
    else //正式
    {
        url = [NSURL URLWithString:UCS_UPLOAD_RECORD];
    }

    
    
    NSDictionary *requestDic = [NSDictionary dictionaryWithObjectsAndKeys:[UserDefaultManager GetclientNumber],@"uid",
                                self.callId,@"call_id",
                                [UserDefaultManager GetKeyChain:DATA_STORE_APPID],@"app_id",
                                self.caller,@"caller",
                                self.callee,@"callee",
                                self.callMode,@"call_mode",nil];
    
    
    UCSASIFormDataRequest *request = [UCSASIFormDataRequest requestWithURL:url];
    
    
    [request addRequestHeader:@"Content-Type" value:@"multipart/form-data"];
    [request addRequestHeader:@"Accept" value:@"text/plain, */*; q=0.01"];
    //    [request addRequestHeader:@"Authorization" value:auth_base64];
    [request setRequestMethod:@"POST"];
    
    NSArray * keysArray = [requestDic allKeys];
    for (int i = 0; i<keysArray.count; i++) {
        
        [request setPostValue:[requestDic objectForKey:keysArray[i]] forKey:keysArray[i]];
        
    }
    
//    [request addFile:self.fileName forKey:@"file"];
    [request addData:[NSData dataWithContentsOfFile:self.fileName] withFileName:[NSString stringWithFormat:@"%@",[[self.fileName componentsSeparatedByString:@"/"] lastObject]] andContentType:@"audio/wav" forKey:@"file"];
    
    
    [request setDelegate:self];
    [request startAsynchronous];
    
    _request = [request retain];
    
}

- (void)stopRcordRequest{
    
    [_request clearDelegatesAndCancel];
    _request = nil;
}


- (void)requestFinished:(UCSASIHTTPRequest *)request{
    
    NSDictionary * dataDic = [NSJSONSerialization JSONObjectWithData:request.responseData options:NSJSONReadingMutableContainers error:nil];
    
//    UXLogAlert(@"录音文件上传---%@",dataDic);
    
    
}
- (void)requestFailed:(UCSASIHTTPRequest *)request{
    
//    UXLogAlert(@"录音文件上传失败");

}

@end
