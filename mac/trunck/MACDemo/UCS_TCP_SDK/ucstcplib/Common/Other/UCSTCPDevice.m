//
//  UCSTCPDevice.m
//  Tcplib
//
//  Created by KCT on 16/4/7.
//  Copyright © 2017年 KCTMAC. All rights reserved.
//

#import "UCSTCPDevice.h"
#include <sys/sysctl.h>


@implementation UCSTCPDevice

- (BOOL)issSimulator {
    static dispatch_once_t one;
    static BOOL simu = NO;
    dispatch_once(&one, ^{
        NSString *model = [self machineModel];
        if ([model isEqualToString:@"x86_64"] || [model isEqualToString:@"i386"]) {
            simu = YES;
        }
    });
    return simu;
}

- (NSString *)machineModel {
    static dispatch_once_t one;
    static NSString *model;
    dispatch_once(&one, ^{
        size_t size;
        sysctlbyname("hw.machine", NULL, &size, NULL, 0);
        char *machine = malloc(size);
        sysctlbyname("hw.machine", machine, &size, NULL, 0);
        model = [NSString stringWithUTF8String:machine];
        free(machine);
    });
    return model;
    
}


@end
