//
//  UCSStatisticalHelp.m
//  KCTApiSdk
//
//  Created by wenqinglin on 14-10-11.
//  Copyright (c) 2014年 ucs. All rights reserved.
//

#import "UCSStatisticalHelp.h"
#import <CoreTelephony/CTTelephonyNetworkInfo.h>
#import <CoreTelephony/CTCarrier.h>
#import "UIDevice+IOKit_Extensions.h"



#include <sys/types.h>
#include <sys/sysctl.h>
#include <mach/machine.h>
#import "UCSOpenUDID.h"
@implementation UCSStatisticalHelp


static UCSStatisticalHelp *g_instance = nil;



+(UCSStatisticalHelp*)instance{
    
    if(nil==g_instance){
        g_instance = [[UCSStatisticalHelp alloc]init];
    }

    return  g_instance;
}



//运营商 operator

-(NSString*)getMCCAndMNCInfo
{
    //CTTelephonyNetworkInfo* ctt=[[[CTTelephonyNetworkInfo alloc]init]autorelease];
    
//    return [NSDictionary dictionaryWithObjectsAndKeys:ctt.subscriberCellularProvider.mobileNetworkCode,@"MNC",
//            ctt.subscriberCellularProvider.mobileCountryCode,@"MCC", nil];
    
    
    return @"NA";
    //return [self getCarrier:ctt.subscriberCellularProvider.mobileCountryCode andMNC:ctt.subscriberCellularProvider.mobileNetworkCode];
    
}


/* China - CN
 * MCC    MNC    Brand    Operator                Status        Bands (MHz)                                    References and notes
 * 460    00            China Mobile            Operational    GSM 900/GSM 1800 UMTS (TD-SCDMA) 1880/2010
 * 460    01            China Unicom            Operational    GSM 900/GSM 1800/ UMTS 2100                    CDMA network sold to China Telecom, WCDMA commercial trial started in May 2009 and in full commercial operation as of October 2009.
 * 460    02            China Mobile            Operational    GSM 900/GSM 1800/ UMTS (TD-SCDMA) 1880/2010
 * 460    03            China Telecom            Operational    CDMA 800/cdma evdo 2100
 * 460    05            China Telecom            Operational
 * 460    06            China Unicom            Operational    GSM 900/GSM 1800/UMTS 2100
 * 460    07            China Mobile            Operational    GSM 900/GSM 1800/UMTS (TD-SCDMA) 1880/2010
 * 460    20            China Tietong            Operational    GSM-R
 * NA    NA            China Telecom&China Unicom    Operational
 */
- (NSString*)getCarrier:(NSString*)mcc  andMNC:(NSString*)mnc
{
    if (mcc == nil || [mcc isEqualToString:@""] ) {
        return @"Unknown";
    }
    else {
        if ([mcc isEqualToString:@"460"]) {
            NSInteger MNC = [mnc intValue];
            switch (MNC) {
                case 00:
                case 02:
                case 07:
                    return @"China Mobile";
                    break;
                case 01:
                case 06:
                    return @"China Unicom";
                    break;
                case 03:
                case 05:
                    return @"China Telecom";
                    break;
                default:
                    break;
            }
        }
    }
    return @"Unknown";
}

//品牌 brand

-(NSString*)getPhoneBrand{
    
    return [NSString stringWithFormat:@"%@",@"Apple"];

}


//ios终端型号



-(NSString *) getplatformString{
    
    NSString *platform = [self platform];
    
    //iPhone
    if ([platform isEqualToString:@"iPhone1,1"])   return@"iPhone 1G";
    if ([platform isEqualToString:@"iPhone1,2"])   return@"iPhone 3G";
    if ([platform isEqualToString:@"iPhone2,1"])   return@"iPhone 3GS";
    if ([platform isEqualToString:@"iPhone3,1"])   return@"iPhone 4";
    if ([platform isEqualToString:@"iPhone3,2"])   return@"Verizon iPhone 4";
    if ([platform isEqualToString:@"iPhone3,3"])   return@"iPhone 4 (CDMA)";
    if ([platform isEqualToString:@"iPhone4,1"])   return @"iPhone 4s";
    if ([platform isEqualToString:@"iPhone5,1"])   return @"iPhone 5 (GSM/WCDMA)";
    if ([platform isEqualToString:@"iPhone5,2"])   return @"iPhone 5 (CDMA)";
    if ([platform isEqualToString:@"iPhone5,3"])   return @"iPhone 5c (CDMA)";
    if ([platform isEqualToString:@"iPhone5,4"])   return @"iPhone 5c (GSM)";
    if ([platform isEqualToString:@"iPhone6,1"])   return @"iPhone 5s (CDMA)";
    if ([platform isEqualToString:@"iPhone6,2"])   return @"iPhone 5s (GSM)";
    
    //iPot Touch
    if ([platform isEqualToString:@"iPod1,1"])     return@"iPod Touch 1G";
    if ([platform isEqualToString:@"iPod2,1"])     return@"iPod Touch 2G";
    if ([platform isEqualToString:@"iPod3,1"])     return@"iPod Touch 3G";
    if ([platform isEqualToString:@"iPod4,1"])     return@"iPod Touch 4G";
    if ([platform isEqualToString:@"iPod5,1"])     return@"iPod Touch 5G";
    //iPad
    if ([platform isEqualToString:@"iPad1,1"])     return@"iPad";
    if ([platform isEqualToString:@"iPad2,1"])     return@"iPad 2 (WiFi)";
    if ([platform isEqualToString:@"iPad2,2"])     return@"iPad 2 (GSM)";
    if ([platform isEqualToString:@"iPad2,3"])     return@"iPad 2 (CDMA)";
    if ([platform isEqualToString:@"iPad2,4"])     return@"iPad 2 New";
    if ([platform isEqualToString:@"iPad2,5"])     return@"iPad Mini (WiFi)";
    if ([platform isEqualToString:@"iPad2,6"])     return@"iPad Mini (GSM)";
    if ([platform isEqualToString:@"iPad2,7"])     return@"iPad Mini (CDMA)";
    if ([platform isEqualToString:@"iPad3,1"])     return@"iPad 3 (WiFi)";
    if ([platform isEqualToString:@"iPad3,2"])     return@"iPad 3 (CDMA)";
    if ([platform isEqualToString:@"iPad3,3"])     return@"iPad 3 (GSM)";
    if ([platform isEqualToString:@"iPad3,4"])     return@"iPad 4 (WiFi)";
    if ([platform isEqualToString:@"iPad3,5"])     return@"iPad 4 (GSM)";
    if ([platform isEqualToString:@"iPad3,6"])     return@"iPad 4 (CDMA)";
    if ([platform isEqualToString:@"i386"] || [platform isEqualToString:@"x86_64"])        return@"Simulator";
    
    return platform;
    
}


-(NSString *) platform{
    
    size_t size;
    
    sysctlbyname("hw.machine", NULL, &size, NULL, 0);
    
    char *machine = malloc(size);
    
    sysctlbyname("hw.machine", machine, &size, NULL, 0);
    
    NSString *platform = [NSString stringWithCString:machine encoding:NSUTF8StringEncoding];
    
    free(machine);
    
    return platform;
    
}



//手机序列号 imei

- (NSString *) getIMEI{
    
 
    
//    NSString *str_imei = [UIDevice getMacAddress ];
    NSString *str_imei = [UCSOpenUDID value];
    
    
    return  str_imei;

}

//imsi 越狱？
- (NSString *) getIMSI{
    
    NSString *str_imsi = @"0";
    
    return str_imsi;
    
}


//手机屏幕宽度 width

- (NSString *) getScreenWidth{
    
    //NSString *width = [NSString stringWithFormat:@"%.0f",__MainScreen_Width];
    
    return 500;

}

//手机屏幕高度 height

- (NSString *) getScreenHeight{
    
    //NSString *height = [NSString stringWithFormat:@"%.0f",__MainScreen_Height];
    
    return 500;
    
}

//手机mac地址 mac


- (NSString *) getMacAddress{
    
 
    //modify by wenqinglin
    //NSString *str_macaddress = [[UIDevice currentDevice] macaddress];
     //NSString *str_macaddress = [UIDevice getMacAddress];
    NSString *str_macaddress = @"None";
    
    
    return  str_macaddress;
    
}





//cpu型号  cpu

-(NSString*)getCPUType
{
 
  
    
    //NSString *str_cputype = [UIDevice getcpuType];
    NSString *str_cputype = @"none Type";
    
    return  str_cputype;
    

}




 

//cpu频率  cpu_rate

//获取CPU频率
- (NSString*) getCpuFrequency
{
    //modify by wenqinglin
    //NSString *str_cpuFrequency = [UIDevice getcpuFrequency];
    NSString *str_cpuFrequency = @"no cpuFrequency";
    
    return  str_cpuFrequency;
}





//内存 memory

- (NSUInteger)getTotalMemoryBytes
{
    size_t size = sizeof(int);
    int results;
    int mib[2] = {CTL_HW, HW_PHYSMEM};
    sysctl(mib, 2, &results, &size, NULL, 0);
    return (NSUInteger) results/1024/1024;
}

//os 系统名称

-(NSString*)getSystemName{
    
   //NSString *str_SystemName =  [[UIDevice currentDevice]systemName];
    
    return @"Mac";
    
}

//version 手机系统版本

-(NSString*)getSystemVersion{
    
    //NSString *str_SystemName =  [[UIDevice currentDevice]systemVersion];
    
    return @"1.0";
    
}

//sdkVersion sdk版本

-(NSString*)getSDKVersion{
    
    
    NSString* versionNum =uVersion;
 
    
    return versionNum;
    
}


//demoVersion Demo版本


-(NSString*)getAppVersion
{
    NSDictionary *infoDict = [[NSBundle mainBundle] infoDictionary];
    NSString *versionNum = [infoDict  objectForKey:@"CFBundleShortVersionString"];
 
    
    return  versionNum;
}


//packageName 包名

-(NSString*)getpackageName
{
    NSString *identifier = [[NSBundle mainBundle] bundleIdentifier];
    
    return  identifier;
}



//clientNumber 子账号

-(NSString*)getclientNumber
{
    NSString *clientNumber = [UserDefaultManager GetUserID];
    
    return  clientNumber;
}




//logDate 上报事件

-(NSString*)getlogDate
{
 
        NSDateFormatter *formatter=[[[NSDateFormatter alloc] init] autorelease];
        [formatter setTimeZone:[NSTimeZone localTimeZone]];
        [formatter setDateFormat:@"yyyy-MM-dd HH:mm:ss"];
        NSString *myDate = [formatter stringFromDate:[NSDate date]];
        
        NSArray *ymdhms = [myDate componentsSeparatedByString:@" "];
        
        NSArray *ymd = [ymdhms[0] componentsSeparatedByString:@"-"];
        
        NSArray *hms = [ymdhms[1] componentsSeparatedByString:@":"];
        
        NSString *timeSp = [NSString stringWithFormat:@"%@%@%@%@%@%@",ymd[0], ymd[1],ymd[2],hms[0],hms[1],hms[2]];
        
        return timeSp;
  
    
}



@end
