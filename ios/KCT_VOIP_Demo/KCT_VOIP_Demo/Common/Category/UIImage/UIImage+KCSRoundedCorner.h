//
//  UIImage+KCSRoundedCorner.h
//  KCT_VOIP_Demo_1.0
//
//  Created by KCT on 15/6/25.
//  Copyright (c) 2015年 KCT. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface UIImage (KCSRoundedCorner)

- (UIImage *)roundedCornerImage:(NSInteger)cornerSize borderSize:(NSInteger)borderSize;

@end
