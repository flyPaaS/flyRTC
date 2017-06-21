//
//  TcpCustomLogger.m
//  Tcplib
//
//  Created by KCT on 15/9/28.
//  Copyright © 2016年 KCTMAC. All rights reserved.
//

#import "TcpCustomLogger.h"


#define ucs_tcpsdk_finderName @"ucs_tcpsdk"
#define ucs_tcpsdk_log @"ucs_tcpsdk_log.txt"


@implementation TcpCustomLogger


static id _instace;

+(instancetype)sharedInstance
{
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        _instace = [[self alloc] init];
    });
    return _instace;
}




/*!
 *  @brief  保存tcp信息到沙盒
 *
 *  @param str <#str description#>
 */
- (void)WriteToSandBox:(NSString *)str
{
    NSFileHandle *outFile = [NSFileHandle fileHandleForWritingAtPath:tcpSdk_applicationDocumentsDirectory()];
    //找到并定位到outFile的末尾位置(在此后追加文件)
    [outFile seekToEndOfFile];
    
    [outFile writeData:[str dataUsingEncoding:NSUTF8StringEncoding]];
    //关闭读写文件
    [outFile closeFile];
}




- (void)log:(NSString *)method  detail:(NSString *)detail{
    
    //dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
        
        NSString * string = [NSString stringWithFormat:@"\n时间%@  方法名:%@  详细信息:%@", [self getNowTime], method, detail];
        
#ifdef DEBUG
        NSLog(@"%@", string);
#endif
        
        [self WriteToSandBox:string];
        
    //});
    
}

- (NSString *)getNowTime{
    
    NSDate * date = [NSDate date];
    NSDateFormatter * formatter = [[NSDateFormatter alloc] init];
    formatter.dateFormat = @"yyyy-MM-dd HH:mm:ss";
    NSString * dateStr = [formatter stringFromDate:date];
    return dateStr;
}


//单个文件的大小
- (long long) fileSizeAtPath:(NSString*) filePath{
    
    NSFileManager* manager = [NSFileManager defaultManager];
    
    if ([manager fileExistsAtPath:filePath]){
        
        return [[manager attributesOfItemAtPath:filePath error:nil] fileSize];
    }
    return 0;
}

- (NSString *)filepath{
    
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString *documentsDirectory = [paths objectAtIndex:0];
    NSString *fileName;
    if (_fileName) {
        fileName = [_fileName copy];
    }else{
        fileName = @"tcpConnection.log";
    }
    NSString *filePath=[documentsDirectory stringByAppendingPathComponent:fileName];
    return filePath;
}

- (void)deleteFileWithPath:(NSString *) path{
    
    [[NSFileManager defaultManager] removeItemAtPath:path error:nil];
}

//日志存放路径
 NSString * tcpSdk_applicationDocumentsDirectory()
{
    
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString *documentsDirectory = [paths objectAtIndex:0];
    documentsDirectory = [documentsDirectory stringByAppendingPathComponent:ucs_tcpsdk_finderName];
    
    NSFileManager *fileManager = [NSFileManager defaultManager];
    if (![fileManager fileExistsAtPath:documentsDirectory])
    {
        NSError *err = nil;
        [fileManager createDirectoryAtPath:documentsDirectory withIntermediateDirectories:YES attributes:nil error:&err];
        
        NSString *filePathMe = [documentsDirectory stringByAppendingPathComponent:ucs_tcpsdk_log];
        //查找文件，如果不存在，就创建一个文件
        if (![fileManager fileExistsAtPath:filePathMe])
        {
            [fileManager createFileAtPath:filePathMe contents:nil attributes:nil];
            
        }
        else
        {
            //如果文件存在并且它的大小大于1M，则删除并且重新创建一个
            long long filesizes  = [[fileManager attributesOfItemAtPath:filePathMe error:nil] fileSize];
            if ((filesizes/(1024.0*1024.0))>1) {
                
                //删除当前文件
                [fileManager removeItemAtPath:filePathMe error:nil];
                //重新创建一个文件
                [fileManager createFileAtPath:filePathMe contents:nil attributes:nil];
            }
            
        }
        
        if (err) {
            
            return nil;
        }
        else
        {
            return [documentsDirectory stringByAppendingPathComponent:ucs_tcpsdk_log];
        }
    }
    else
    {
        return [documentsDirectory stringByAppendingPathComponent:ucs_tcpsdk_log];
    }
    
    return [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) lastObject];
}



@end
