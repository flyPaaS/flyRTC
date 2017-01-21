//
//  UIImage+KCSResize.h
//  KCT_VOIP_Demo_1.0
//
//  Created by KCT on 15/6/25.
//  Copyright (c) 2015年 KCT. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface UIImage (KCSResize)

- (UIImage *)croppedImage:(CGRect)bounds;
- (UIImage *)thumbnailImage:(NSInteger)thumbnailSize
          transparentBorder:(NSUInteger)borderSize
               cornerRadius:(NSUInteger)cornerRadius
       interpolationQuality:(CGInterpolationQuality)quality;
- (UIImage *)resizedImage:(CGSize)newSize
     interpolationQuality:(CGInterpolationQuality)quality;
- (UIImage *)resizedImageWithContentMode:(UIViewContentMode)contentMode
                                  bounds:(CGSize)bounds
                    interpolationQuality:(CGInterpolationQuality)quality;

@end
