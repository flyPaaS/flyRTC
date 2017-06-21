//
//  UCSPublicFunc.m
//  yzxapisdk
//
//  Created by wenqinglin on 14-4-9.
//  Copyright (c) 2014年 yzx. All rights reserved.
//

#import "UCSPublicFunc.h"
#import "UCSNetDef.h"
#include <ifaddrs.h>
#include <sys/socket.h>
#include <net/if.h>
#include <sys/time.h>

@implementation UCSPublicFunc


- (id)init
{
    self = [super init];
    if (self)
    {
        // Initialization code here.
    }
    
    return self;
}


// 获取Http服务器地址
+ (NSString *) GetHttpServer {
    
    if (UseTestServer == 1)
    {
//        return UCS_HTTPS_SERVER_TEST;
    }

    
    return UCS_HTTP_SERVER;
}

// 获取电话当前网络类型
+ (PhoneNetType) GetCurrentPhoneNetType
{
    //    return PNT_2G;  // 3G Test
    PhoneNetType nPhoneNetType = PNT_UNKNOWN;
    if ([KCTTcpClient sharedTcpClientManager].getCurrentNetWorkStatus == KCTReachableViaWiFi)
    {
        nPhoneNetType = PNT_WIFI;
    }
    else if ([KCTTcpClient sharedTcpClientManager].getCurrentNetWorkStatus != KCTNotReachable)
    {
        int n = [KCTTcpClient sharedTcpClientManager].getCurrentNetWorkStatus;
        if (n == KCTReachableVia2G) {
            nPhoneNetType = PNT_2G;
        } else if (n == KCTReachableVia3G  || n == KCTReachableViaLTE) {
            nPhoneNetType = PNT_3G;
        }else if ( n == KCTReachableVia4G){
            nPhoneNetType = PNT_4G;
        } else {
            
            // 这里其实是非WiFi下的网络，包括2G和3G,这里粗略的判断，可能不准确
            if ([PublicFunc is3GTelephoneNumber:[UserDefaultManager GetUserMobile]])
            {
                nPhoneNetType = PNT_3G;
            }
            else
            {
                nPhoneNetType = PNT_2G;
            }
            
        }
    }
    
    return nPhoneNetType;
}


//*****************************网络流量监控**************************************************************//

+(NSString *)bytesToAvaiUnit:(int)bytes
{
	if(bytes < 1024)		// B
	{
		return [NSString stringWithFormat:@"%dB", bytes];
	}
	else if(bytes >= 1024 && bytes < 1024 * 1024)	// KB
	{
		return [NSString stringWithFormat:@"%.1fKB", (double)bytes / 1024];
	}
	else if(bytes >= 1024 * 1024 && bytes < 1024 * 1024 * 1024)	// MB
	{
		return [NSString stringWithFormat:@"%.2fMB", (double)bytes / (1024 * 1024)];
	}
	else	// GB
	{
		return [NSString stringWithFormat:@"%.3fGB", (double)bytes / (1024 * 1024 * 1024)];
	}
}






+(NSDictionary*)checkNetworkflow{
    
    struct ifaddrs *ifa_list = 0, *ifa;
    
    if (getifaddrs(&ifa_list) == -1)
        
    {
        
        return nil;
    }
    
    uint32_t iBytes     = 0;
    
    uint32_t oBytes     = 0;
    
    uint32_t allFlow    = 0;
    
    uint32_t wifiIBytes = 0;
    
    uint32_t wifiOBytes = 0;
    
    uint32_t wifiFlow   = 0;
    
    uint32_t wwanIBytes = 0;
    
    uint32_t wwanOBytes = 0;
    
    uint32_t wwanFlow   = 0;
    
    
        #if defined(__LP64__)
            struct timeval32 time ;
        #else
            struct timeval time ;
        #endif
    
    
 
    
    for (ifa = ifa_list; ifa; ifa = ifa->ifa_next)
        
    {
        
        if (AF_LINK != ifa->ifa_addr->sa_family)
            
            continue;
        
        if (!(ifa->ifa_flags & IFF_UP) && !(ifa->ifa_flags & IFF_RUNNING))
            
            continue;
        
        if (ifa->ifa_data == 0)
            
            continue;
        // Not a loopback device.
        
        // network flow
        
        if (strncmp(ifa->ifa_name, "lo", 2))
            
        {
            
            struct if_data *if_data = (struct if_data *)ifa->ifa_data;
            
            iBytes += if_data->ifi_ibytes;
            
            oBytes += if_data->ifi_obytes;
            
            allFlow = iBytes + oBytes;
            
            time = if_data->ifi_lastchange;
             
            // NSLog(@"1111===%s :iBytes is %d, oBytes is %d", ifa->ifa_name, iBytes, oBytes);
            
        }
        
        //<span style="font-family: Tahoma, Helvetica, Arial, 宋体, sans-serif; ">WIFI流量统计功能</span>
        
        if (!strcmp(ifa->ifa_name, "en0"))
            
        {
            struct if_data *if_data = (struct if_data *)ifa->ifa_data;
            
            wifiIBytes += if_data->ifi_ibytes;
            
            wifiOBytes += if_data->ifi_obytes;
            
            wifiFlow    = wifiIBytes + wifiOBytes;
            
        }
        
        //<span style="font-family: Tahoma, Helvetica, Arial, 宋体, sans-serif; ">3G和GPRS流量统计</span>
        
        if (!strcmp(ifa->ifa_name, "pdp_ip0"))
            
        {
            
            struct if_data *if_data = (struct if_data *)ifa->ifa_data;
            
            wwanIBytes += if_data->ifi_ibytes;
            
            wwanOBytes += if_data->ifi_obytes;
            
            wwanFlow    = wwanIBytes + wwanOBytes;
            
            //NSLog(@"111122===%s :iBytes is %d, oBytes is %d",  ifa->ifa_name, iBytes, oBytes);
            
        }
        
    }
    
    freeifaddrs(ifa_list);
    
    
        NSString *changeTime=[NSString stringWithFormat:@"%s",ctime(&time)];
        NSLog(@"changeTime==%@",changeTime);
    
    NSString *receivedBytes= [self bytesToAvaiUnit:iBytes];
    NSLog(@"receivedBytes==%@",receivedBytes);
    
    NSString *sentBytes       = [self bytesToAvaiUnit:oBytes];
    NSLog(@"sentBytes==%@",sentBytes);
    
    NSString *networkFlow      = [self bytesToAvaiUnit:allFlow];
    NSLog(@"networkFlow==%@",networkFlow);
    
    NSString *wifiReceived   = [self bytesToAvaiUnit:wifiIBytes];
    NSLog(@"wifiReceived==%@",wifiReceived);
    
    NSString *wifiSent       = [self bytesToAvaiUnit: wifiOBytes];
    NSLog(@"wifiSent==%@",wifiSent);
    
    NSString *wifiBytes      = [self bytesToAvaiUnit:wifiFlow];
    NSLog(@"wifiBytes==%@",wifiBytes);
    
    NSString *wwanReceived   = [self bytesToAvaiUnit:wwanIBytes];
    NSLog(@"wwanReceived==%@",wwanReceived);
    
    NSString *wwanSent       = [self bytesToAvaiUnit:wwanOBytes];
    NSLog(@"wwanSent==%@",wwanSent);
    
    NSString *wwanBytes      = [self bytesToAvaiUnit:wwanFlow];
    NSLog(@"wwanBytes==%@",wwanBytes);
    
    //    changeTime,@"changeTime",
    
    NSDictionary *dictionary = [NSDictionary dictionaryWithObjectsAndKeys:changeTime,@"changeTime",receivedBytes,@"receivedBytes",sentBytes,@"sentBytes",networkFlow,@"networkFlow",wifiReceived,@"wifiReceived",wifiSent,@"wifiSent",wifiBytes,@"wifiBytes",wwanReceived,@"wwanReceived",wwanSent,@"wwanSent",wwanBytes,@"wwanBytes", nil];
    
    return dictionary;
    
}
//*****************************网络流量监控**************************************************************//




//默认返回文件大小单位为字节，除以1024转换成K
+(long long) fileSizeAtPath:(NSString*) filePath{
    NSFileManager* manager = [NSFileManager defaultManager];
    if ([manager fileExistsAtPath:filePath]){
        return [[manager attributesOfItemAtPath:filePath error:nil] fileSize]/1024;
    }
    return 0;
}


//获取连接测试地址的标志
+(NSString*)getIsUseTestServer {
    NSString *isUseTestServer = [UserDefaultManager GetLocalDataString:@"UseTestServer"];
    if (isUseTestServer && [isUseTestServer isEqualToString:@"TESTMODE"]) {
        return @"TESTMODE";
    }
    
    if (isUseTestServer && [isUseTestServer isEqualToString:@"DEVMODE"]) {
        return @"DEVMODE";
    }
  
    return nil;
}





+ (NSString *)getIPhoneNowTime{
    NSDate * date = [NSDate date];
    NSDateFormatter * formatter = [[NSDateFormatter alloc] init];
    formatter.dateFormat = @"yyyy-MM-dd HH:mm:ss";
    NSString * dateStr = [formatter stringFromDate:date];
    [formatter release];
    return dateStr;
}


#if 0
NSFileManager * fileManager = [NSFileManager defaultManager];
if ([[NSFileManager defaultManager] fileExistsAtPath:path])
{
    //如果文件存在并且它的大小大于3M，则删除并且重新创建一个
    long long filesizes  = [[fileManager attributesOfItemAtPath:path error:nil] fileSize];
    if ((filesizes/(1024.0*1024.0))>3) {
        
        //删除当前文件
        [fileManager removeItemAtPath:path error:nil];
        //重新创建一个文件
        [fileManager createFileAtPath:path contents:nil attributes:nil];
    }
    
    NSFileHandle *outFile = [NSFileHandle fileHandleForWritingAtPath:path];
    //找到并定位到outFile的末尾位置(在此后追加文件)
    [outFile seekToEndOfFile];
    
    [outFile writeData:[strError dataUsingEncoding:NSUTF8StringEncoding]];
    //关闭读写文件
    [outFile closeFile];
}else{
    // 如果文件不存在 则创建并且将文件写入
    [fileManager createFileAtPath:path contents:nil attributes:nil];
    [strError writeToFile:path atomically:YES encoding:NSUTF8StringEncoding error:nil];
}
// 获取文件名，如果不存在则重新创建
NSString * logName = [[NSUserDefaults standardUserDefaults] valueForKey:@"UCSLogName"];
if (logName.length == 0 || !logName) {
    logName = [NSString stringWithFormat:@"crash_%@_%@_%@.txt",[UserDefaultManager GetKeyChain:DATA_STORE_APPID],[UserDefaultManager GetclientNumber],strTime];
    [[NSUserDefaults standardUserDefaults] setValue:logName forKey:@"UCSLogName"];
    [[NSUserDefaults standardUserDefaults] synchronize];
}
// 需要上传的文件 写入
NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask,YES);
NSString * documentsDirectory  =[paths objectAtIndex:0];
NSString * filePath = [documentsDirectory stringByAppendingPathComponent:logName];
#endif



@end
