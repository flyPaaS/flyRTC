//
//  NSDateFormatter+Category.h
//  KCT_VOIP_Demo_1.0
//
//  Created by KCT on 15/4/22.
//  Copyright (c) 2015年 KCT. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface NSDateFormatter (Category)

+ (id)dateFormatter;
+ (id)dateFormatterWithFormat:(NSString *)dateFormat;

+ (id)defaultDateFormatter;/*yyyy-MM-dd HH:mm:ss*/

@end
