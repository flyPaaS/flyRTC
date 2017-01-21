//
//  SanboxHelper.m
//  KCT_VOIP_Demo_1.0
//
//  Created by KCT on 15/4/24.
//  Copyright (c) 2015年 KCT. All rights reserved.
//

#import "SanboxHelper.h"

@implementation SanboxHelper

+ (NSString *)homePath{
    return NSHomeDirectory();
}

+ (NSString *)appPath
{
    NSArray * paths = NSSearchPathForDirectoriesInDomains(NSApplicationDirectory, NSUserDomainMask, YES);
    return [paths objectAtIndex:0];
}

+ (NSString *)docPath
{
    NSArray * paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    return [paths objectAtIndex:0];
}

+ (NSString *)libPrefPath
{
    NSArray * paths = NSSearchPathForDirectoriesInDomains(NSLibraryDirectory, NSUserDomainMask, YES);
    return [[paths objectAtIndex:0] stringByAppendingFormat:@"/Preference"];
}

+ (NSString *)libCachePath
{
    NSArray * paths = NSSearchPathForDirectoriesInDomains(NSLibraryDirectory, NSUserDomainMask, YES);
    return [[paths objectAtIndex:0] stringByAppendingFormat:@"/Caches"];
}

+ (NSString *)tmpPath
{return [NSHomeDirectory() stringByAppendingFormat:@"/tmp"];
}

+ (BOOL)hasLive:(NSString *)path
{
    if ( NO == [[NSFileManager defaultManager] fileExistsAtPath:path] )
    {
        return [[NSFileManager defaultManager] createDirectoryAtPath:path
                                         withIntermediateDirectories:YES
                                                          attributes:nil
                                                               error:NULL];
    }
    
    return NO;
}

@end
