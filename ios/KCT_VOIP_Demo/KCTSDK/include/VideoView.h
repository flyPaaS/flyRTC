//
//  Header.h
//  vigo-lib
//
//  Created by softphone on 15/5/13.
//  Copyright (c) 2015年 国领电讯. All rights reserved.
//


// 公共函数定义

#import <UIKit/UIKit.h>
@interface VideoView


// 从本地数据库获取用户账号
+ (UIView *) allocAndInitWithFrame:(CGRect)frame;
+ (void) enableRender:(BOOL)enable;

@end

