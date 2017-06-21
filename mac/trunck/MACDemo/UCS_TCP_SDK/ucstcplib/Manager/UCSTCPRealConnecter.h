//
//  UCSTCPRealConnecter.h
//  Tcplib
//
//  Created by KCT on 16/3/31.
//  Copyright © 2017年 KCTMAC. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface UCSTCPRealConnecter : NSObject
+(instancetype)sharedInstance;

@property(nonatomic, assign) BOOL realConnected; //和云平台的真实连接情况

@end
