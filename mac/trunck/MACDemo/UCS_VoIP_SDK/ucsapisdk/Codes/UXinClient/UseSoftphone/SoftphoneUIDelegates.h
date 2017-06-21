//
//  SoftphoneUIDelegates.h
//  moduleExperiment
//
//  Created by wenqinglin on 14-7-16.
//  Copyright (c) 2014年 UCPaaS. All rights reserved.
//

#import <Foundation/Foundation.h>

@protocol SoftphoneUIDelegates <NSObject>

- (void) SoftPhoneCallback:(int) ev_type withReason:(int) ev_reason withDescription:(NSString*) strDescription withParam:(const void*) code;

@end
