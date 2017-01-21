//
//  NSString+KCMessageInputView.m
//  KCT_VOIP_Demo_1.0
//
//  Created by KCT on 15/6/10.
//  Copyright (c) 2015年 KCT. All rights reserved.
//

#import "NSString+KCMessageInputView.h"

@implementation NSString (KCMessageInputView)

- (NSString *)stringByTrimingWhitespace {
    return [self stringByTrimmingCharactersInSet:[NSCharacterSet whitespaceAndNewlineCharacterSet]];
}

- (NSUInteger)numberOfLines {
    return [[self componentsSeparatedByString:@"\n"] count] + 1;
}

@end
