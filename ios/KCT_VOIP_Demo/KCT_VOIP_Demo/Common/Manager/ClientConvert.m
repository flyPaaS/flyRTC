//
//  ClientConvert.m
//  KCT_VOIP_Demo_1.0
//
//  Created by KCT on 15/5/26.
//  Copyright (c) 2015年 KCT. All rights reserved.
//

#import "ClientConvert.h"
#import <UIKit/UIKit.h>
#import "AddressBook.h"

@implementation ClientConvert


//这里加入几个测试账号
- (NSArray *)contactTestArray{
    NSMutableArray * array = [NSMutableArray array];
    
    AddressBook * book1 = [[AddressBook alloc] init];
    book1.name = @"测试账号1";
    [book1.phones setObject: @"18687654321" forKey:@"home"];
    
    AddressBook * book2 = [[AddressBook alloc] init];
    book2.name = @"测试账号2";
    [book2.phones setObject: @"18687654322" forKey:@"home"];
    
    AddressBook * book3 = [[AddressBook alloc] init];
    book3.name = @"测试账号3";
    [book3.phones setObject: @"18687654323" forKey:@"home"];
    
    [array addObject:book1];
    [array addObject:book2];
    [array addObject:book3];
    
    return array;
}


@end
