//
//  TimeUtil.m
//  KCT_VOIP_Demo
//
//  Created by KCMac on 2017/1/7.
//  Copyright © 2017年 KCMac. All rights reserved.
//

#import "TimeUtil.h"

@implementation TimeUtil

//获取时间戳 格式为:20170114104428
+ (NSString*)gettimeSp:(NSDate*)datenow
{
    NSDateFormatter *formatter=[[NSDateFormatter alloc] init];
    [formatter setTimeZone:[NSTimeZone localTimeZone]];
    [formatter setDateFormat:@"yyyy-MM-dd HH:mm:ss"];
    NSString *myDate = [formatter stringFromDate:datenow];
    
    NSArray *ymdhms = [myDate componentsSeparatedByString:@" "];
    
    NSArray *ymd = [ymdhms[0] componentsSeparatedByString:@"-"];
    
    NSArray *hms = [ymdhms[1] componentsSeparatedByString:@":"];
    
    NSString *timeSp = [NSString stringWithFormat:@"%@%@%@%@%@%@",ymd[0], ymd[1],ymd[2],hms[0],hms[1],hms[2]];
    
    return timeSp;
}

@end
