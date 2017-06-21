//
//  OpenUDID.h
//  Baige_demo
//
//  Created by FredZhang on 14-9-24.
//  Copyright (c) 2014年 FredZhang. All rights reserved.
//

#import <Foundation/Foundation.h>

//
// Usage:
//    #include "OpenUDID.h"
//    NSString* openUDID = [OpenUDID value];
//

#define kOpenUDIDErrorNone          0
#define kOpenUDIDErrorOptedOut      1
#define kOpenUDIDErrorCompromised   2

@interface UCSOpenUDID : NSObject {
}
+ (NSString*) value;
+ (NSString*) valueWithError:(NSError**)error;
+ (void) setOptOut:(BOOL)optOutValue;

@end