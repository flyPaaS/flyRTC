//
//  UCSExceptionHandler.h
//  ucsvoicesdk
//
//  Created by KCT   on 15/7/27.
//  Copyright (c) 2015年 ucs. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface UCSExceptionHandler : NSObject
{
    BOOL dismissed;
}

@end
void UCSVoipHandleException(NSException *exception);
void UCSVoipSignalHandler(int signal);


void UCSInstallUncaughtExceptionHandler(void);

