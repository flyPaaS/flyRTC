//
//  NSString+KCMessageInputView.h
//  KCT_VOIP_Demo_1.0
//
//  Created by KCT on 15/6/10.
//  Copyright (c) 2015年 KCT. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface NSString (KCMessageInputView)

- (NSString *)stringByTrimingWhitespace;


- (NSUInteger)numberOfLines;

@end
