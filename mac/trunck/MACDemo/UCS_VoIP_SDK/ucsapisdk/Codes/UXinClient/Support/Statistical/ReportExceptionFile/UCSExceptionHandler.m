#import "UCSExceptionHandler.h"
#include <libkern/OSAtomic.h>
#include <execinfo.h>

//http://www.cocoachina.com/newbie/tutorial/2012/0829/4672.html
NSString * const UCSUncaughtExceptionHandlerSignalExceptionName = @"UCSUncaughtExceptionHandlerSignalExceptionName";
NSString * const UCSUncaughtExceptionHandlerSignalKey = @"UCSUncaughtExceptionHandlerSignalKey";
NSString * const UCSUncaughtExceptionHandlerAddressesKey = @"UCSUncaughtExceptionHandlerAddressesKey";

volatile int32_t UCSUncaughtExceptionCount = 0;
const int32_t UCSUncaughtExceptionMaximum = 10;

const NSInteger UCSUncaughtExceptionHandlerSkipAddressCount = 4;
const NSInteger UCSUncaughtExceptionHandlerReportAddressCount = 5;

@implementation UCSExceptionHandler

+ (NSArray *)backtrace
{
    void* callstack[128];
    int frames = backtrace(callstack, 128);
    char **strs = backtrace_symbols(callstack, frames);
    
    int i;
    NSMutableArray *backtrace = [NSMutableArray arrayWithCapacity:frames];
    for (
         i = UCSUncaughtExceptionHandlerSkipAddressCount;
         i < UCSUncaughtExceptionHandlerSkipAddressCount +
         UCSUncaughtExceptionHandlerReportAddressCount;
         i++)
    {
        [backtrace addObject:[NSString stringWithUTF8String:strs[i]]];
    }
    free(strs);
    
    return backtrace;
}


- (void)validateAndSaveCriticalApplicationData:(NSException *) exception
{
    NSString * reason = [exception reason];
    NSString * rAddressesKey = [[exception userInfo] objectForKey:UCSUncaughtExceptionHandlerAddressesKey];
    
    NSString *strError = [NSString stringWithFormat:@"\n\n\n崩溃原因:\n%@\n堆栈信息:\n%@",  reason, rAddressesKey];
    
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask,YES);
    NSString * documentsDirectory  =[paths objectAtIndex:0];
    documentsDirectory = [documentsDirectory stringByAppendingPathComponent:@"更详细的bug捕获errorFile.txt"];
    
    
    NSString *path = documentsDirectory;
    if ([[NSFileManager defaultManager]fileExistsAtPath:path])
    {
        NSString *lasterror = [[NSString alloc] initWithContentsOfFile:path encoding:NSUTF8StringEncoding error:nil];
        strError  = [NSString stringWithFormat:@"%@%@", lasterror, strError];
    }
    [strError writeToFile:path atomically:YES encoding:NSUTF8StringEncoding error:nil];
}

- (void)handleException:(NSException *)exception
{
//    [self validateAndSaveCriticalApplicationData:exception];
    
//    UIAlertView *alert =
//    [[[UIAlertView alloc]
//      initWithTitle:NSLocalizedString(@"抱歉，程序出现了异常", nil)
//      message:[NSString stringWithFormat:NSLocalizedString(
//                                                           @"如果点击继续，程序有可能会出现其他的问题，建议您还是点击退出按钮并重新打开\n\n"
//                                                           @"异常原因如下:\n%@\n%@", nil),
//               [exception reason],
//               [[exception userInfo] objectForKey:UCSUncaughtExceptionHandlerAddressesKey]]
//      delegate:self
//      cancelButtonTitle:NSLocalizedString(@"退出", nil)
//      otherButtonTitles:NSLocalizedString(@"继续", nil), nil]
//     autorelease];
//    [alert show];
    

    
    CFRunLoopRef runLoop = CFRunLoopGetCurrent();
    CFArrayRef allModes = CFRunLoopCopyAllModes(runLoop);
    
    while (!dismissed)
    {
        for (NSString *mode in (__bridge NSArray *)allModes)
        {
            CFRunLoopRunInMode((CFStringRef)mode, 0.001, false);
        }
    }
    
    CFRelease(allModes);
    
    NSSetUncaughtExceptionHandler(NULL);
    signal(SIGABRT, SIG_DFL);
    signal(SIGILL, SIG_DFL);
    signal(SIGSEGV, SIG_DFL);
    signal(SIGFPE, SIG_DFL);
    signal(SIGBUS, SIG_DFL);
    signal(SIGPIPE, SIG_IGN);  //忽略SIGPIPE信号
    
    if ([[exception name] isEqual:UCSUncaughtExceptionHandlerSignalExceptionName])
    {
        kill(getpid(), [[[exception userInfo] objectForKey:UCSUncaughtExceptionHandlerSignalKey] intValue]);
    }
    else
    {
        [exception raise];
    }
}

@end

void UCSVoipHandleException(NSException *exception)
{
    int32_t exceptionCount = OSAtomicIncrement32(&UCSUncaughtExceptionCount);
    if (exceptionCount > UCSUncaughtExceptionMaximum)
    {
        return;
    }
    
    NSArray *callStack = [UCSExceptionHandler backtrace];
    NSMutableDictionary *userInfo =
    [NSMutableDictionary dictionaryWithDictionary:[exception userInfo]];
    [userInfo
     setObject:callStack
     forKey:UCSUncaughtExceptionHandlerAddressesKey];
    
    [[[UCSExceptionHandler alloc] init]
     performSelectorOnMainThread:@selector(handleException:)
     withObject:
     [NSException
      exceptionWithName:[exception name]
      reason:[exception reason]
      userInfo:userInfo]
     waitUntilDone:YES];
}

void UCSVoipSignalHandler(int signal)
{
    int32_t exceptionCount = OSAtomicIncrement32(&UCSUncaughtExceptionCount);
    if (exceptionCount > UCSUncaughtExceptionMaximum)
    {
        return;
    }
    
    NSMutableDictionary *userInfo =
    [NSMutableDictionary
     dictionaryWithObject:[NSNumber numberWithInt:signal]
     forKey:UCSUncaughtExceptionHandlerSignalKey];
    
    NSArray *callStack = [UCSExceptionHandler backtrace];
    [userInfo
     setObject:callStack
     forKey:UCSUncaughtExceptionHandlerAddressesKey];
    
    [[[UCSExceptionHandler alloc] init]
     performSelectorOnMainThread:@selector(handleException:)
     withObject:
     [NSException
      exceptionWithName:UCSUncaughtExceptionHandlerSignalExceptionName
      reason:
      [NSString stringWithFormat:
       NSLocalizedString(@"Signal %d was raised.", nil),
       signal]
      userInfo:
      [NSDictionary
       dictionaryWithObject:[NSNumber numberWithInt:signal]
       forKey:UCSUncaughtExceptionHandlerSignalKey]]
     waitUntilDone:YES];
}

void UCSInstallUncaughtExceptionHandler(void)
{
    NSSetUncaughtExceptionHandler(&UCSVoipHandleException);
    signal(1, UCSVoipSignalHandler);
    signal(2, UCSVoipSignalHandler);
    signal(3, UCSVoipSignalHandler);
    signal(4, UCSVoipSignalHandler);
    signal(5, UCSVoipSignalHandler);
    signal(6, UCSVoipSignalHandler);
    
    signal(7, UCSVoipSignalHandler);
    signal(8, UCSVoipSignalHandler);
    signal(9, UCSVoipSignalHandler);
    signal(10, UCSVoipSignalHandler);
    signal(11, UCSVoipSignalHandler);
    signal(12, UCSVoipSignalHandler);
    
    signal(13, UCSVoipSignalHandler);
    signal(14, UCSVoipSignalHandler);
    signal(15, UCSVoipSignalHandler);
    signal(16, UCSVoipSignalHandler);
    signal(17, UCSVoipSignalHandler);
    signal(18, UCSVoipSignalHandler);
    
    signal(19, UCSVoipSignalHandler);
    signal(20, UCSVoipSignalHandler);
    signal(21, UCSVoipSignalHandler);
    signal(22, UCSVoipSignalHandler);
    signal(23, UCSVoipSignalHandler);
    signal(24, UCSVoipSignalHandler);
    
    signal(25, UCSVoipSignalHandler);
    signal(26, UCSVoipSignalHandler);
    signal(27, UCSVoipSignalHandler);
    signal(28, UCSVoipSignalHandler);
    signal(29, UCSVoipSignalHandler);
    signal(30, UCSVoipSignalHandler);
    signal(31, UCSVoipSignalHandler);
}
