//
//  UCSStatisticalHelp.h
//  KCTApiSdk
//
//  Created by wenqinglin on 14-10-11.
//  Copyright (c) 2014年 ucs. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface UCSStatisticalHelp : NSObject

#pragma mark 手机信息 


+(UCSStatisticalHelp*)instance;

//运营商 operator

-(NSString*)getMCCAndMNCInfo;

//品牌 brand
-(NSString*)getPhoneBrand;

//机型
-(NSString *) getplatformString;

//手机序列号 imei

- (NSString *) getIMEI;

//imsi 需要越狱后才能获取？

- (NSString *) getIMSI;

//手机屏幕宽度 width

- (NSString *) getScreenWidth;

//手机屏幕高度 height

- (NSString *) getScreenHeight;


//手机mac地址 mac

- (NSString *) getMacAddress;


//cpu型号  cpu

-(NSString*)getCPUType;

//cpu频率  cpu_rate

- (NSString*) getCpuFrequency;

//内存 memory

- (NSUInteger)getTotalMemoryBytes;
//os 系统版本

-(NSString*)getSystemName;

//version 手机系统版本

-(NSString*)getSystemVersion;

//sdkVersion sdk版本

-(NSString*)getSDKVersion;

//demoVersion Demo版本

-(NSString*)getAppVersion;

//packageName 包名

-(NSString*)getpackageName;

//clientNumber 子账号

-(NSString*)getclientNumber;

//logDate 上报事件

-(NSString*)getlogDate;



#pragma mark 错误码

@end
