//
//  SecurityURLAdaptor.h
//  UXinClient
//
//  Created by FengXing on 13-8-21.
//  Copyright (c) 2013年 UXIN CO. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "Signature/Signature.h"

@interface SecurityURLAdaptor : NSObject

+ (id)sharedAdaptor;
- (NSURL *)convert:(NSURL*)url;
- (BOOL)isSupport:(NSURL *)url;
- (NSString *)calcNewSign:(NSDictionary *)params;
@end

