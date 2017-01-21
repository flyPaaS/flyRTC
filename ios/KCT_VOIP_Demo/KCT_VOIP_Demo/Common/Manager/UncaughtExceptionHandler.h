//
//  UncaughtExceptionHandler.h
//  KCT_IM_Demo
//
//  Created by KCT on 15/7/27.
//  Copyright (c) 2015年 KCT. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface UncaughtExceptionHandler : NSObject{
    BOOL dismissed;
}

@end
void HandleException(NSException *exception);
void SignalHandler(int signal);


void InstallUncaughtExceptionHandler(void);
