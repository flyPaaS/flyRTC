//
//  UINavigationItem+KCT.m
//  KCT_VOIP_Demo_1.0
//
//  Created by KCT on 15/5/5.
//  Copyright (c) 2015年 KCT. All rights reserved.
//

#import "UINavigationItem+KCT.h"

@implementation UINavigationItem (Barry)


#if __IPHONE_OS_VERSION_MAX_ALLOWED > __IPHONE_6_1



- (void)setCustomLeftBarButtonItem:(UIBarButtonItem *) leftBarButtonItem{
    
    if ([[[UIDevice currentDevice] systemVersion] floatValue] >= 7.0)
    {
        UIBarButtonItem *negativeSeperator = [[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemFixedSpace target:nil action:nil];
        negativeSeperator.width = -10;
        
        if (leftBarButtonItem)
        {
            [self setLeftBarButtonItems:@[negativeSeperator, leftBarButtonItem]];
        }
        else
        {
            [self setLeftBarButtonItems:@[negativeSeperator]];
        }
    }
    else
    {
        [self setLeftBarButtonItem:leftBarButtonItem animated:NO];
    }
}
- (void)setCustomRightBarButtonItem:(UIBarButtonItem *) rightBarButtonItem{
    
    if ([[[UIDevice currentDevice] systemVersion] floatValue] >= 7.0)
    {
        UIBarButtonItem *negativeSeperator = [[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemFixedSpace target:nil action:nil];
        negativeSeperator.width = -12;
        
        if (rightBarButtonItem)
        {
            [self setRightBarButtonItems:@[negativeSeperator, rightBarButtonItem]];
        }
        else
        {
            [self setRightBarButtonItems:@[negativeSeperator]];
        }
    }
    else
    {
        [self setRightBarButtonItem:rightBarButtonItem animated:NO];
    }
}

//- (void)setLeftBarButtonItem:(UIBarButtonItem *)_leftBarButtonItem
//{
//
//    if ([[[UIDevice currentDevice] systemVersion] floatValue] >= 7.0)
//    {
//        UIBarButtonItem *negativeSeperator = [[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemFixedSpace target:nil action:nil];
//        negativeSeperator.width = -10;
//        
//        if (_leftBarButtonItem)
//        {
//            [self setLeftBarButtonItems:@[negativeSeperator, _leftBarButtonItem]];
//        }
//        else
//        {
//            [self setLeftBarButtonItems:@[negativeSeperator]];
//        }
//    }
//    else
//    {
//        [self setLeftBarButtonItem:_leftBarButtonItem animated:NO];
//    }
//}
//
//- (void)setRightBarButtonItem:(UIBarButtonItem *)_rightBarButtonItem
//{
//    if ([[[UIDevice currentDevice] systemVersion] floatValue] >= 7.0)
//    {
//        UIBarButtonItem *negativeSeperator = [[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemFixedSpace target:nil action:nil];
//        negativeSeperator.width = -12;
//        
//        if (_rightBarButtonItem)
//        {
//            [self setRightBarButtonItems:@[negativeSeperator, _rightBarButtonItem]];
//        }
//        else
//        {
//            [self setRightBarButtonItems:@[negativeSeperator]];
//        }
//    }
//    else
//    {
//        [self setRightBarButtonItem:_rightBarButtonItem animated:NO];
//    }
//}

#endif

@end
