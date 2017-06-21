//
//  OffLinePushManager.m
//  Tcplib
//
//  Created by KCT on 15/9/26.
//  Copyright © 2016年 KCTMAC. All rights reserved.
//

#import "OffLinePushManager.h"

#ifdef kLocalAreaNetworkEnvironment
#define apnsRegisterUrl @"http://192.168.0.8:8097"
#else
#define apnsRegisterUrl @"http://59.110.10.28:8097"
#endif


@implementation OffLinePushManager

+ (void)openOffLinePushWithEnvironment:(KCTPushEnvironment) environment clientId:(NSString *)clientId{ 
    
    [OffLinePushManager sendPushSettingDataWithFlag:YES  environment:environment clientId:clientId];
    
}


+(void)closeOffLinePushWithEnvironment:(KCTPushEnvironment) environment clientId:(NSString *)clientId{
    [OffLinePushManager sendPushSettingDataWithFlag:NO environment:environment clientId:clientId];
}



+(void)sendPushSettingDataWithFlag:(BOOL) open environment:(KCTPushEnvironment) environment clientId:(NSString *)clientId{
    
    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
    NSData *deviceToken = [defaults objectForKey:@"UCSDeviceToken"];
    if (deviceToken.length <= 0) {
        return;
    }
    
    NSString *pushToken = [[[[deviceToken description]
                             
                             stringByReplacingOccurrencesOfString:@"<" withString:@""]
                            
                            stringByReplacingOccurrencesOfString:@">" withString:@""]
                           
                           stringByReplacingOccurrencesOfString:@" " withString:@""] ;
    NSString *url = nil;
    NSString *appid = [[NSBundle mainBundle]bundleIdentifier];
    if (open) {
        url = [NSString stringWithFormat:@"%@/apnproxy/v1/tokenregister?token=%@&appid=%@&clientnum=%@",apnsRegisterUrl,pushToken,appid,clientId];
        NSLog(@"token:%@",pushToken);
        NSMutableURLRequest *request = [NSMutableURLRequest requestWithURL:[NSURL URLWithString:url]];
        request.HTTPMethod = @"GET";
        //发送请求
        [NSURLConnection sendAsynchronousRequest:request queue:[NSOperationQueue mainQueue] completionHandler:^(NSURLResponse * _Nullable response, NSData * _Nullable data, NSError * _Nullable connectionError) {
            if (data) {
                NSDictionary * dict = [NSJSONSerialization JSONObjectWithData:data options:NSJSONReadingMutableContainers error:nil];
                NSLog(@"apns register %@",dict);
            } else {
                NSLog(@"apns register fail");
            }
        }];
        
    } else {
        url = [NSString stringWithFormat:@"%@/apnproxy/v1/tokencancel?token=%@&appid=%@&clientnum=%@",apnsRegisterUrl,pushToken,appid,clientId];
        NSMutableURLRequest *request = [NSMutableURLRequest requestWithURL:[NSURL URLWithString:url]];
        request.HTTPMethod = @"GET";
        //发送请求
        [NSURLConnection sendAsynchronousRequest:request queue:[NSOperationQueue mainQueue] completionHandler:^(NSURLResponse * _Nullable response, NSData * _Nullable data, NSError * _Nullable connectionError) {
            if (data) {
                NSDictionary * dict = [NSJSONSerialization JSONObjectWithData:data options:NSJSONReadingMutableContainers error:nil];
                NSLog(@"cancel %@",dict);
            } else {
                
            }
        }];
    }
}


@end
