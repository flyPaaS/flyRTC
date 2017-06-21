//
//  SdkDataReporter.h
//  Tcplib
//
//  Created by KCT on 16/1/14.
//  Copyright © 2017年 KCTMAC. All rights reserved.
//


#import <Foundation/Foundation.h>
@class UCSAuthResponseClass;

@interface UCSTCPSdkDataReporter : NSObject

+(instancetype)sharedInstance;

- (void)reportSdkData:(UCSAuthResponseClass *) aAuthResponse;

@end
