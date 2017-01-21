//
//  UIImage+KCImage.m
//  KCT_VOIP_Demo_1.0
//
//  Created by KCT on 15/5/29.
//  Copyright (c) 2015年 KCT. All rights reserved.
//

#import "UIImage+KCImage.h"

@implementation UIImage (KCImage)


+ (UIImage *)resizedImageWithName:(NSString*)ImgName
{
    return [UIImage resizedImageWithName:ImgName left:0.5 top:0.5];
}


+ (UIImage *)resizedImageWithName:(NSString*)ImgName left:(CGFloat)left top:(CGFloat)top
{
    UIImage *image = [UIImage imageNamed:ImgName];
    return [image stretchableImageWithLeftCapWidth:image.size.width * left topCapHeight:image.size.height * top ];
}

@end
