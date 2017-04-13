//
//  HttpRequestEngine.m
//  KCT_VOIP_Demo
//
//  Created by KCMac on 2017/1/7.
//  Copyright © 2017年 KCMac. All rights reserved.
//

#import "HttpRequestEngine.h"
#import "TimeUtil.h"
#import "DataEncryption.h"


#ifdef kLocalAreaNetworkEnvironment
#define kRestApiBaseURL   @"http://192.168.0.8:80"
//#define kRestApiBaseURL   @"http://192.168.0.112:80"
#else
#define kRestApiBaseURL   @"http://60.205.137.243:80"

#endif





@interface HttpRequestEngine()

@property(nonatomic,copy)requestSuccessBlock successBlock;
@property(nonatomic,copy)requestFailBlock failBlock;

@end

static HttpRequestEngine *detailInstance = nil;

@implementation HttpRequestEngine

+(id)engineInstance
{
    @synchronized(self){
        static dispatch_once_t pred;
        dispatch_once(&pred, ^{
            detailInstance = [[self alloc] init];
        });
    }
    
    return detailInstance;
}

- (NSString *)getTimeSp {
    NSDate *nowDate = [NSDate date];
    NSString *times = [TimeUtil gettimeSp:nowDate];
    return times;
}

- (void)login:(NSString *)sid token:(NSString *)token successBlock:(requestSuccessBlock)successBlockT failBlock:(requestFailBlock)failBlockT {
    self.successBlock = successBlockT;
    self.failBlock = failBlockT;
    
    NSString *timeSp = [self getTimeSp];
    NSString *orBase64 = [NSString stringWithFormat:@"%@:%@",sid,timeSp];
    NSString *orMd5 = [NSString stringWithFormat:@"%@%@%@",sid,token,timeSp];
    //base64、Md5加密后
    NSString *deBase64 = [DataEncryption encodeBase64String:orBase64];
    NSString *deMd5 = [DataEncryption md5CapitalizedString:orMd5];
    NSString *url = [NSString stringWithFormat:@"%@/2014-06-30/Accounts/%@/login?sig=%@",kRestApiBaseURL,sid,deMd5];
    //printf("-----url is : %s",[url UTF8String]);
    NSMutableDictionary *bodyDict = [[NSMutableDictionary alloc] init];
    NSMutableDictionary *accountDict = [[NSMutableDictionary alloc] init];
    [accountDict setObject:sid forKey:@"username"];
    [accountDict setObject:token forKey:@"password"];
    [bodyDict setObject:accountDict forKey:@"account"];
    
    
    NSData *bodyData=[NSJSONSerialization dataWithJSONObject:bodyDict options:NSJSONWritingPrettyPrinted error:nil];
    
    NSMutableURLRequest *request = [NSMutableURLRequest requestWithURL:[NSURL URLWithString:url]];
    request.HTTPMethod = @"POST";
    request.HTTPBody = bodyData;
    [request setValue:@"application/json; charset=utf-8" forHTTPHeaderField:@"Content-Type"];
    [request setValue:deBase64 forHTTPHeaderField:@"Authorization"];
    [request setValue:@"application/json" forHTTPHeaderField:@"Accept"];
    //发送请求
    [NSURLConnection sendAsynchronousRequest:request queue:[NSOperationQueue mainQueue] completionHandler:^(NSURLResponse * _Nullable response, NSData * _Nullable data, NSError * _Nullable connectionError) {
        if (data) {
            NSDictionary * dict = [NSJSONSerialization JSONObjectWithData:data options:NSJSONReadingMutableContainers error:nil];
            self.successBlock(dict);
        } else {
            self.failBlock(connectionError.userInfo);
        }
    }];
    
}

- (void)newLogin:(NSString *)sid token:(NSString *)token successBlock:(requestSuccessBlock)successBlockT failBlock:(requestFailBlock)failBlockT {
    self.successBlock = successBlockT;
    self.failBlock = failBlockT;
    
    NSString *timeSp = [self getTimeSp];
    NSString *orBase64 = [NSString stringWithFormat:@"%@:%@",sid,timeSp];
    NSString *orMd5 = [NSString stringWithFormat:@"%@%@%@",sid,token,timeSp];
    //base64、Md5加密后
    NSString *deBase64 = [DataEncryption encodeBase64String:orBase64];
    NSString *deMd5 = [DataEncryption md5CapitalizedString:orMd5];
    NSString *url = [NSString stringWithFormat:@"%@/2014-06-30/Accounts/%@/loginNewDemo?sig=%@",kRestApiBaseURL,sid,deMd5];
    //printf("-----url is : %s",[url UTF8String]);
    NSMutableDictionary *bodyDict = [[NSMutableDictionary alloc] init];
    NSMutableDictionary *accountDict = [[NSMutableDictionary alloc] init];
    [accountDict setObject:sid forKey:@"username"];
    [accountDict setObject:token forKey:@"password"];
    [bodyDict setObject:accountDict forKey:@"account"];
    
    
    NSData *bodyData=[NSJSONSerialization dataWithJSONObject:bodyDict options:NSJSONWritingPrettyPrinted error:nil];
    
    NSMutableURLRequest *request = [NSMutableURLRequest requestWithURL:[NSURL URLWithString:url]];
    request.HTTPMethod = @"POST";
    request.HTTPBody = bodyData;
    [request setValue:@"application/json; charset=utf-8" forHTTPHeaderField:@"Content-Type"];
    [request setValue:deBase64 forHTTPHeaderField:@"Authorization"];
    [request setValue:@"application/json" forHTTPHeaderField:@"Accept"];
    //发送请求
    [NSURLConnection sendAsynchronousRequest:request queue:[NSOperationQueue mainQueue] completionHandler:^(NSURLResponse * _Nullable response, NSData * _Nullable data, NSError * _Nullable connectionError) {
        if (data) {
            NSDictionary * dict = [NSJSONSerialization JSONObjectWithData:data options:NSJSONReadingMutableContainers error:nil];
            self.successBlock(dict);
        } else {
            self.failBlock(connectionError.userInfo);
        }
    }];
}


- (void)getLoginStateWithSid:(NSString *)sid clientNum:(NSString *)clientNum stateBlock:(loginStateBlock)stateBlock {
    NSString *url = [NSString stringWithFormat:@"%@/2014-06-30/Accounts/%@/userState",kRestApiBaseURL,sid];
    NSString *userInfoKey = @"4";
    NSString *bodyStr = [NSString stringWithFormat:@"%@:%@",clientNum,userInfoKey];
    NSData *bodyData = [bodyStr dataUsingEncoding:NSUTF8StringEncoding];
//    NSMutableDictionary *bodyDict = [[NSMutableDictionary alloc] init];
//    [bodyDict setObject:userInfoKey forKey:clientNum];
//    NSData *bodyData=[NSJSONSerialization dataWithJSONObject:bodyDict options:NSJSONWritingPrettyPrinted error:nil];
    
    NSMutableURLRequest *request = [NSMutableURLRequest requestWithURL:[NSURL URLWithString:url]];
    request.HTTPMethod = @"POST";
    request.HTTPBody = bodyData;
    [request setValue:@"application/json; charset=utf-8" forHTTPHeaderField:@"Content-Type"];
    [request setValue:@"application/json" forHTTPHeaderField:@"Accept"];
    //发送请求
    [NSURLConnection sendAsynchronousRequest:request queue:[NSOperationQueue mainQueue] completionHandler:^(NSURLResponse * _Nullable response, NSData * _Nullable data, NSError * _Nullable connectionError) {
        if (data) {
            NSDictionary * dict = [NSJSONSerialization JSONObjectWithData:data options:NSJSONReadingMutableContainers error:nil];
            stateBlock(dict);
        } else {
            stateBlock(nil);
        }
    }];
}


- (void)getClientList:(NSString *)sid token:(NSString *)token appId:(NSString *)appId successBlock:(requestSuccessBlock)successBlockT failBlock:(requestFailBlock)failBlockT {
    
    self.successBlock = successBlockT;
    self.failBlock = failBlockT;
    
    NSString *timeSp = [self getTimeSp];
    NSString *orBase64 = [NSString stringWithFormat:@"%@:%@",sid,timeSp];
    NSString *orMd5 = [NSString stringWithFormat:@"%@%@%@",sid,token,timeSp];
    //base64、Md5加密后
    NSString *deBase64 = [DataEncryption encodeBase64String:orBase64];
    NSString *deMd5 = [DataEncryption md5CapitalizedString:orMd5];
    NSString *url = [NSString stringWithFormat:@"%@/2014-06-30/Accounts/%@/clientList?sig=%@",kRestApiBaseURL,sid,deMd5];
    
    NSMutableDictionary *bodyDict = [[NSMutableDictionary alloc] init];
    NSMutableDictionary *clientDict = [[NSMutableDictionary alloc] init];
    [clientDict setObject:appId forKey:@"appId"];
    [bodyDict setObject:clientDict forKey:@"client"];
    
    
    NSData *bodyData=[NSJSONSerialization dataWithJSONObject:bodyDict options:NSJSONWritingPrettyPrinted error:nil];

    NSMutableURLRequest *request = [NSMutableURLRequest requestWithURL:[NSURL URLWithString:url]];
    request.HTTPMethod = @"POST";
    request.HTTPBody = bodyData;
    [request setValue:@"application/json; charset=utf-8" forHTTPHeaderField:@"Content-Type"];
    [request setValue:deBase64 forHTTPHeaderField:@"Authorization"];
    [request setValue:@"application/json" forHTTPHeaderField:@"Accept"];
    //发送请求
    [NSURLConnection sendAsynchronousRequest:request queue:[NSOperationQueue mainQueue] completionHandler:^(NSURLResponse * _Nullable response, NSData * _Nullable data, NSError * _Nullable connectionError) {
            if (data) {
                NSDictionary * dict = [NSJSONSerialization JSONObjectWithData:data options:NSJSONReadingMutableContainers error:nil];
                self.successBlock(dict);
            } else {
                self.failBlock(connectionError.userInfo);
            }
    }];
    
}

@end
