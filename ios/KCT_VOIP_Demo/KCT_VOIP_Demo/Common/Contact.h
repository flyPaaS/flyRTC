//
//  Contact.h
//  KCT_VOIP_Demo
//
//  Created by KCMac on 2017/1/7.
//  Copyright © 2017年 KCMac. All rights reserved.
//

#ifndef Contact_h
#define Contact_h
#import "DDLog.h"


#define kCallingUidKey  @"CallingUidKey"
#define kCalledUidKey  @"CalledUidKey"


#define getBalanceUrl @"http://%@/ottdemoapi/getBalance.do" //获取免费分钟数url


#define DevicesScale ([UIScreen mainScreen].bounds.size.height==480?1.00:[UIScreen mainScreen].bounds.size.height==568?1.00:[UIScreen mainScreen].bounds.size.height==667?1.17:1.29)

#define GetViewX(view) view.frame.origin.x
#define GetViewY(view) view.frame.origin.y
#define Adaptation(n) (n)*DevicesScale
#define GetViewWidth(view) view.frame.size.width
#define GetViewHeight(view) view.frame.size.height
#define CurrentHeight [UIScreen mainScreen].bounds.size.height
#define GetTextFont(n) CurrentHeight==480?(n):(CurrentHeight==568?(n):(CurrentHeight==667?(n+2):(n+4)))
#define CenterPoint(x,y) ((x)-(y))/2.0 //居中
#define GetRGBColor(rgbValue) [UIColor colorWithRed:((float)((rgbValue & 0xFF0000) >> 16))/255.0 green:((float)((rgbValue & 0xFF00) >> 8))/255.0 blue:((float)(rgbValue & 0xFF))/255.0 alpha:1.0]
#define RGBACOLOR(r,g,b,a) [UIColor colorWithRed:(r)/255.0 green:(g)/255.0 blue:(b)/255.0 alpha:(a)]
#define UIColorFromRGB(rgbValue) [UIColor  colorWithRed:((float)((rgbValue & 0xFF0000) >> 16))/255.0  green:((float)((rgbValue & 0xFF00) >> 8))/255.0  blue:((float)(rgbValue & 0xFF))/255.0 alpha:1.0]


//日志
#ifdef DEBUG
static const int ddLogLevel = DDLogLevelVerbose;
#else
static const int ddLogLevel = DDLogLevelError;
#endif



#endif /* Contact_h */
