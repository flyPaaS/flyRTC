//
//  OffLinePushManager.h
//  Tcplib
//
//  Created by KCT on 15/9/26.
//  Copyright © 2016年 KCTMAC. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "KCTProtocolClass.h"
#import "UCSTCPManager.h"

@interface OffLinePushManager : NSObject

+(void)openOffLinePushWithEnvironment:(KCTPushEnvironment) environment clientId:(NSString *)clientId;

+(void)closeOffLinePushWithEnvironment:(KCTPushEnvironment) environment clientId:(NSString *)clientId;



@end
