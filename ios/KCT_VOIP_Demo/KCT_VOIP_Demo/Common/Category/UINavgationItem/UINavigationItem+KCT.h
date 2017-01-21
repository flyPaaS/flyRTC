//
//  UINavigationItem+KCT.h
//  KCT_VOIP_Demo_1.0
//
//  Created by KCT on 15/5/5.
//  Copyright (c) 2015年 KCT. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface UINavigationItem (Barry)

//- (void)setLeftBarButtonItem:(UIBarButtonItem *)_leftBarButtonItem;
//- (void)setRightBarButtonItem:(UIBarButtonItem *)_rightBarButtonItem;


- (void)setCustomLeftBarButtonItem:(UIBarButtonItem *) leftBarButtonItem;
- (void)setCustomRightBarButtonItem:(UIBarButtonItem *) rightBarButtonItem;

@end
