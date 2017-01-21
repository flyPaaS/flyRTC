//
//  UIImage+KCImage.h
//  KCT_VOIP_Demo_1.0
//
//  Created by KCT on 15/5/29.
//  Copyright (c) 2015年 KCT. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface UIImage (KCImage)

/**
 *  返回一张可以自由拉伸的图片
 */
+ (UIImage *)resizedImageWithName:(NSString*)ImgName;

/**
 *  返回一张可以自由拉伸的图片
 */
+ (UIImage *)resizedImageWithName:(NSString*)ImgName left:(CGFloat)left top:(CGFloat)top;

@end
