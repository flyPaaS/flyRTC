//
//  UIView+Convenience.h
//  KCT_VOIP_Demo_1.0
//
//  Created by KCT on 15/5/11.
//  Copyright (c) 2015年 KCT. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface UIView (Convenience)

@property (nonatomic) CGPoint frameOrigin;
@property (nonatomic) CGSize frameSize;

@property (nonatomic) CGFloat frameX;
@property (nonatomic) CGFloat frameY;

@property (nonatomic) CGFloat frameRight;
@property (nonatomic) CGFloat frameBottom;

@property (nonatomic) CGFloat frameWidth;
@property (nonatomic) CGFloat frameHeight;

- (BOOL)containsSubView:(UIView *)subView;
- (BOOL)containsSubViewOfClassType:(Class)class;
- (void)removeAllSubViews;

@end
