//
//  UCSCrashLogRequestManager.m
//  ucsvideosdk
//
//  Created by macmini on 16/1/12.
//  Copyright © 2016年 ucs. All rights reserved.
//

#import "UCSCrashLogRequestManager.h"
#import "UCSASIFormDataRequest.h"
#import <sys/utsname.h>
@implementation UCSCrashLogRequestManager
{
    UCSASIFormDataRequest * _request;
}

- (void)dealloc{
    
    self.callMode = nil;
    self.callId = nil;
    self.callee = nil;
    self.fileName = nil;
    
    [_request clearDelegatesAndCancel];
    _request = nil;
    
    [super dealloc];
}

- (void)uploadCrashRequest{
    NSURL * url = nil;
    
    if ([UCSPublicFunc getIsUseTestServer] && [[UCSPublicFunc getIsUseTestServer] isEqualToString:@"TESTMODE"])//测试
    {
        url = [NSURL URLWithString:UCS_UPLOAD_CRASH_TEST];
    }
    else if ([UCSPublicFunc getIsUseTestServer] && [[UCSPublicFunc getIsUseTestServer] isEqualToString:@"DEVMODE"])//DEV 模式
    {
        //开发环境
        url = [NSURL URLWithString:UCS_UPLOAD_CRASH_DEV];
    }
    else //正式
    {
        url = [NSURL URLWithString:UCS_UPLOAD_CRASH];
    }
    UCSASIFormDataRequest *request = [UCSASIFormDataRequest requestWithURL:url];
    
    
    [request addRequestHeader:@"Content-Type" value:@"multipart/form-data"];
    [request addRequestHeader:@"Accept" value:@"text/plain, */*; q=0.01"];
    [request setRequestMethod:@"POST"];
    
    [request setPostValue:[UserDefaultManager GetclientNumber] forKey:@"uid"];
    NSString * appid = [UserDefaultManager GetKeyChain:DATA_STORE_APPID];
    if (appid.length == 0) {
        appid = @"asdfghjkl";
    }
    [request setPostValue:appid forKey:@"app_id"];
    [request setPostValue:@"iOS" forKey:@"pv"];
    [request setPostValue:uVersion forKey:@"ver"];
    //[request setPostValue:[UIDevice currentDevice].systemVersion forKey:@"osv"];
    //[request setPostValue:[UIDevice currentDevice].model forKey:@"brand"];
    [request setPostValue:[UCSCrashLogRequestManager deviceString] forKey:@"model"];
    [request addFile:self.fileName forKey:@"file"];
    request.tag = 1001011;
    request.delegate = self;
    [request startAsynchronous];
    _request = [request retain];
}

//获得设备型号
+ (NSString*)deviceString
{
    // 需要#import "sys/utsname.h"
    struct utsname systemInfo;
    uname(&systemInfo);
    NSString *deviceString = [NSString stringWithCString:systemInfo.machine encoding:NSUTF8StringEncoding];
    return deviceString;
}



- (void)stopCrashRequest{
    
    [_request clearDelegatesAndCancel];
    _request = nil;
}

#pragma mark - UCSASIHTTPRequestDelegate
- (void)requestFinished:(UCSASIHTTPRequest *)request{
    NSDictionary * dataDic = [NSJSONSerialization JSONObjectWithData:request.responseData options:NSJSONReadingMutableContainers error:nil];
    // 上传成功 删除文件
    if ([dataDic[@"respCode"] isEqualToString:@"000000"]) {
        NSFileManager * fileManager = [NSFileManager defaultManager];
        BOOL ret = [fileManager removeItemAtPath:self.fileName error:nil];
        if (ret) {
            [[NSUserDefaults standardUserDefaults] setValue:@"" forKey:@"UCSLogName"];
        }
    }
    
    if (_delegate && [_delegate respondsToSelector:_select]) {
        [_delegate performSelector:_select withObject:nil];
    }
}

- (void)requestFailed:(UCSASIHTTPRequest *)request{
    // 不管
    if (_delegate && [_delegate respondsToSelector:_select]) {
        [_delegate performSelector:_select withObject:nil];
    }
}



@end
