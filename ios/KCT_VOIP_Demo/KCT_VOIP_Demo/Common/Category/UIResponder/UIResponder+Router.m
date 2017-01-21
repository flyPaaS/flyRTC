//
//  UIResponder+Router.m
//  KCT_VOIP_Demo_1.0
//
//  Created by KCT on 15/4/22.
//  Copyright (c) 2015年 KCT. All rights reserved.
//

#import "UIResponder+Router.h"

@implementation UIResponder (Router)

- (void)routerEventWithName:(NSString *)eventName userInfo:(NSDictionary *)userInfo
{
    [[self nextResponder] routerEventWithName:eventName userInfo:userInfo];
}

@end
