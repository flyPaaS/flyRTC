
// We need all the log functions visible so we set this to DEBUG

#import "UXLogging.h"
#define MAX_LOG_SIZE 1024*1024
static int AddLogFile(NSString *fname);
static void uncaughtExceptionHandler(NSException *exception);
static int  logfd = 0;
static void AddStderrOnce()
{
	static dispatch_once_t onceToken;
	dispatch_once(&onceToken, ^{
		asl_add_log_file(NULL, STDERR_FILENO);
#if SUPPORT_LOG_FILE
        return ;
        logfd = AddLogFile([NSTemporaryDirectory() stringByAppendingPathComponent:@"YZX_SDK_IOS.log"]);
        NSSetUncaughtExceptionHandler(&uncaughtExceptionHandler);
        
#endif
	});
}


void _UXLog(int LEVEL, NSString *format, ...)
{
	AddStderrOnce();
    
//    if ([UserDefaultManager GetLocalDataBoolen:FORMAL_LOG_ISOPEN])
//    {
        va_list args;
        va_start(args, format);
        NSString *message = [[NSString alloc] initWithFormat:format arguments:args];
        asl_log(NULL, NULL, (LEVEL), "%s\r\n", [message UTF8String]);
        [message release];
        va_end(args);
//    }
}

static int AddLogFile(NSString *fname)
{
	int result = 1;
	int fd = -1;
	NSDictionary *attrs = [[NSFileManager defaultManager] attributesOfItemAtPath:fname error:NULL];
	unsigned long long fsize = [attrs fileSize];
	if (fsize > MAX_LOG_SIZE) {
		fd = open([fname fileSystemRepresentation], (O_RDWR|O_CREAT|O_TRUNC), (S_IRWXU|S_IRWXG|S_IRWXO));
	} else {
		fd = open([fname fileSystemRepresentation], (O_RDWR|O_CREAT|O_APPEND), (S_IRWXU|S_IRWXG|S_IRWXO));
	}
	if (fd != -1) {
		result = asl_add_log_file(NULL, fd);
       	printf("AddLogFile %s (%d)", [fname fileSystemRepresentation], result);
	}
    return fd;
}

void uncaughtExceptionHandler(NSException *exception)
{
//    if ([UserDefaultManager GetLocalDataBoolen:FORMAL_LOG_ISOPEN])
//    {
        UXLogCritical(@"CRASH: %@", exception);
        UXLogCritical(@"Stack Trace: %@", [exception callStackSymbols]);
        asl_close(NULL);
        if (logfd > 0) {
            close(logfd), logfd = 0;
        }
//    }
}