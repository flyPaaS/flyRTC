//
//  UIDevice+IOKit_Extensions.m
//  KCTApiSdk
//
//  Created by wenqinglin on 14-10-11.
//  Copyright (c) 2014年 ucs. All rights reserved.
//



//#import "network.h"
#include <sys/types.h>
#include <sys/sysctl.h>
#import <mach/mach_host.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <sys/socket.h>
#include <net/if.h>
#include <net/if_dl.h>
#include <ifaddrs.h>

#import "UIDevice+IOKit_Extensions.h"





#pragma mark IOKit miniheaders

/*
@implementation UIDevice(IOKit_Extensions)

#pragma mark platform information


+ (NSString *) getSysInfoByName:(char *)typeSpecifier
{
    size_t size;
    sysctlbyname(typeSpecifier, NULL, &size, NULL, 0);
    
    char *answer = malloc(size);
    sysctlbyname(typeSpecifier, answer, &size, NULL, 0);
    
    NSString *results = [NSString stringWithCString:answer encoding: NSUTF8StringEncoding];
    
    free(answer);
    return results;
}


+ (NSString *) platform
{
    return [self getSysInfoByName:"hw.machine"];
}

+ (NSUInteger) platformType
{
    NSString *platform = [self platform];
    
    // The ever mysterious iFPGA
    if ([platform isEqualToString:@"iFPGA"])        return UIDeviceIFPGA;
    
    
    //    // iPhone
    //    if ([platform isEqualToString:@"iPhone1,1"])    return UIDevice1GiPhone;
    //    if ([platform isEqualToString:@"iPhone1,2"])    return UIDevice3GiPhone;
    //    if ([platform hasPrefix:@"iPhone2"])            return UIDevice3GSiPhone;
    //    if ([platform hasPrefix:@"iPhone3"])            return UIDevice4iPhone;
    //    if ([platform hasPrefix:@"iPhone4"])            return UIDevice4SiPhone;
    //    if ([platform hasPrefix:@"iPhone5"])            return UIDevice5iPhone;
    //    if ([platform hasPrefix:@"iPhone6"])            return UIDevice5SiPhone;
    //
    //    // iPod
    //    if ([platform hasPrefix:@"iPod1"])              return UIDevice1GiPod;
    //    if ([platform hasPrefix:@"iPod2"])              return UIDevice2GiPod;
    //    if ([platform hasPrefix:@"iPod3"])              return UIDevice3GiPod;
    //    if ([platform hasPrefix:@"iPod4"])              return UIDevice4GiPod;
    //
    //    // iPad
    //    if ([platform hasPrefix:@"iPad1"])              return UIDevice1GiPad;
    //    if ([platform hasPrefix:@"iPad2"])              return UIDevice2GiPad;
    //    if ([platform hasPrefix:@"iPad3"])              return UIDevice3GiPad;
    //    if ([platform hasPrefix:@"iPad4"])              return UIDevice4GiPad;
    
    
    
    //iPhone
    if ([platform isEqualToString:@"iPhone1,1"])   return UIDevice1GiPhone;
    if ([platform isEqualToString:@"iPhone1,2"])   return UIDevice3GiPhone;
    if ([platform isEqualToString:@"iPhone2,1"])   return UIDevice3GSiPhone;
    if ([platform isEqualToString:@"iPhone3,1"])   return UIDevice4iPhone;
    if ([platform isEqualToString:@"iPhone3,2"])   return UIDevice4iPhone;
    if ([platform isEqualToString:@"iPhone3,3"])   return UIDevice4iPhone;
    if ([platform isEqualToString:@"iPhone4,1"])   return UIDevice4SiPhone;
    if ([platform isEqualToString:@"iPhone5,1"])   return UIDevice5iPhone;
    if ([platform isEqualToString:@"iPhone5,2"])   return UIDevice5iPhone;
    if ([platform isEqualToString:@"iPhone5,3"])   return UIDevice5CiPhone;
    if ([platform isEqualToString:@"iPhone5,4"])   return UIDevice5CiPhone;
    if ([platform isEqualToString:@"iPhone6,1"])   return UIDevice5SiPhone;
    if ([platform isEqualToString:@"iPhone6,2"])   return UIDevice5SiPhone;
    
    //iPot Touch
    if ([platform isEqualToString:@"iPod1,1"])     return UIDevice1GiPod;
    if ([platform isEqualToString:@"iPod2,1"])     return UIDevice2GiPod;
    if ([platform isEqualToString:@"iPod3,1"])     return UIDevice3GiPod;
    if ([platform isEqualToString:@"iPod4,1"])     return UIDevice4GiPod;
    if ([platform isEqualToString:@"iPod5,1"])     return UIDevice5GiPod;
    //iPad
    if ([platform isEqualToString:@"iPad1,1"])     return UIDevice1GiPad;
    if ([platform isEqualToString:@"iPad2,1"])     return UIDevice2GiPad;
    if ([platform isEqualToString:@"iPad2,2"])     return UIDevice2GiPad;
    if ([platform isEqualToString:@"iPad2,3"])     return UIDevice2GiPad;
    if ([platform isEqualToString:@"iPad2,4"])     return UIDevice2GiPad;
    if ([platform isEqualToString:@"iPad2,5"])     return UIDevice2GiPad;
    if ([platform isEqualToString:@"iPad2,6"])     return UIDevice2GiPad;
    if ([platform isEqualToString:@"iPad2,7"])     return UIDevice2GiPad;
    if ([platform isEqualToString:@"iPad3,1"])     return UIDevice3GiPad;
    if ([platform isEqualToString:@"iPad3,2"])     return UIDevice3GiPad;
    if ([platform isEqualToString:@"iPad3,3"])     return UIDevice3GiPad;
    if ([platform isEqualToString:@"iPad3,4"])     return UIDevice3GiPad;
    if ([platform isEqualToString:@"iPad3,5"])     return UIDevice3GiPad;
    if ([platform isEqualToString:@"iPad3,6"])     return UIDevice3GiPad;
    if ([platform isEqualToString:@"iPad4,1"])     return UIDevice4GiPad;
    
    // Apple TV
    if ([platform hasPrefix:@"AppleTV2"])           return UIDeviceAppleTV2;
    if ([platform hasPrefix:@"AppleTV3"])           return UIDeviceAppleTV3;
    
    if ([platform hasPrefix:@"iPhone"])             return UIDeviceUnknowniPhone;
    if ([platform hasPrefix:@"iPod"])               return UIDeviceUnknowniPod;
    if ([platform hasPrefix:@"iPad"])               return UIDeviceUnknowniPad;
    if ([platform hasPrefix:@"AppleTV"])            return UIDeviceUnknownAppleTV;
    
    // Simulator thanks Jordan Breeding
    if ([platform hasSuffix:@"86"] || [platform isEqual:@"x86_64"])
    {
        BOOL smallerScreen = [[UIScreen mainScreen] bounds].size.width < 768;
        return smallerScreen ? UIDeviceSimulatoriPhone : UIDeviceSimulatoriPad;
    }
    
    
    
    
    
    
    return UIDeviceUnknown;
}



#pragma mark cpu information
+ (NSString *) getcpuType
{
    switch ([self platformType])
    {
        //iphone
        case UIDevice3GiPhone: return IPHONE_3G_CPUTYPE;
        case UIDevice3GSiPhone: return IPHONE_3GS_CPUTYPE;
        case UIDevice4iPhone: return IPHONE_4_CPUTYPE;
        case UIDevice4SiPhone: return IPHONE_4S_CPUTYPE;
        case UIDevice5iPhone: return IPHONE_5_CPUTYPE;
        case UIDevice5CiPhone: return IPHONE_5C_CPUTYPE;
        case UIDevice5SiPhone: return IPHONE_5S_CPUTYPE;
        //iPod 
        case UIDevice4GiPod: return IPOD_4G_CPUTYPE;
        default: return IOS_CPUTYPE_UNKNOWN;
    }
}

+ (NSString *) getcpuFrequency
{
    switch ([self platformType])
    {
        //iphone
        case UIDevice3GiPhone: return IPHONE_3G_CPUFREQUENCY;
        case UIDevice3GSiPhone: return IPHONE_3GS_CPUFREQUENCY;
        case UIDevice4iPhone: return IPHONE_4_CPUFREQUENCY;
        case UIDevice4SiPhone: return IPHONE_4S_CPUFREQUENCY;
        case UIDevice5iPhone: return IPHONE_5_CPUFREQUENCY;
        case UIDevice5CiPhone: return IPHONE_5C_CPUFREQUENCY;
        case UIDevice5SiPhone: return IPHONE_5S_CPUFREQUENCY;
            
        //iPod
        case UIDevice4GiPod: return IPOD_4G_CPUFREQUENCY;
        default: return IOS_CPUFREQUENCY_UNKNOWN;
    }
}


+(NSString *) getplatformString{
    
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


//+(NSString *) platformTpye{
//    
//    size_t size;
//    
//    sysctlbyname("hw.machine", NULL, &size, NULL, 0);
//    
//    char *machine = malloc(size);
//    
//    sysctlbyname("hw.machine", machine, &size, NULL, 0);
//    
//    NSString *platform = [NSString stringWithCString:machine encoding:NSUTF8StringEncoding];
//    
//    free(machine);
//    
//    return platform;
//    
//}


@end

*/


