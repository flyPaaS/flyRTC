
//
//  UCSCPSManager.m
//  KCTApiSdk
//
//  Created by wenqinglin on 14-4-9.
//  Copyright (c) 2014年 ucs. All rights reserved.
//

#import "UCSCPSManager.h"

#import "Utility.h"
#import "UserDefaultManager.h"
#import "UCSStatisticalHelp.h"
#import "UCSCPSManager.h"
#define REPORTDATATYPE @"reportDataType"

#define  strpv         @"iphone"


@implementation UCSCPSManager

static UCSCPSManager *g_instance = nil;


+(UCSCPSManager*)instance{

    if (nil==g_instance) {
        g_instance = [[UCSCPSManager alloc] init];
    }
    
    return  g_instance;
    
}



// 初始化
- (id) init
{
    self = [super init];
    if (self) {
        m_delegate = nil;
        
        self.m_pUCSASIHTTPRequest = nil;
 
 
        
    }
    return self;
}

// dealloc
- (void) dealloc
{

    self.m_pUCSASIHTTPRequest = nil;

    
    [super dealloc];
}

// 停止网络请求
- (void) stopRequest
{
    if (nil != self.m_pUCSASIHTTPRequest)
    {
        [self.m_pUCSASIHTTPRequest clearDelegatesAndCancel];
    }
}

- (void) startAsynchronousWithURL:(NSURL *) url
{
  
    
    self.m_pUCSASIHTTPRequest = [UCSASIHTTPRequest requestWithURL:url];
    [self.m_pUCSASIHTTPRequest setUseCookiePersistence:YES];
 
    
    // 设定委托，委托自己实现异步请求方法
//    char ch[100] = {0};
//    Security((unsigned char*)&ch[0]);
//    [self.m_pUCSASIHTTPRequestLogIn addRequestHeader:@"SecurityFlag" value:[NSString stringWithCString:ch encoding:NSUTF8StringEncoding]];
    
    [self.m_pUCSASIHTTPRequest setDelegate:self];
    // 开始异步请求
    [self.m_pUCSASIHTTPRequest startAsynchronous];
}

//获取时间戳 格式为:20140814104428 
- (NSString*) gettimeSp:(NSDate*)datenow
{
    NSDateFormatter *formatter=[[[NSDateFormatter alloc] init] autorelease];
    [formatter setTimeZone:[NSTimeZone localTimeZone]];
    [formatter setDateFormat:@"yyyy-MM-dd HH:mm:ss"];
    NSString *myDate = [formatter stringFromDate:datenow];
    
    NSArray *ymdhms = [myDate componentsSeparatedByString:@" "];
    
    NSArray *ymd = [ymdhms[0] componentsSeparatedByString:@"-"];
    
    NSArray *hms = [ymdhms[1] componentsSeparatedByString:@":"];
    
    NSString *timeSp = [NSString stringWithFormat:@"%@%@%@%@%@%@",ymd[0], ymd[1],ymd[2],hms[0],hms[1],hms[2]];
    
    return timeSp;

}



 




#pragma mark 上报手机数据
//上报手机数据
- (void)  getCPSParameter
{
    
    NSString *reportType = @"CPSparamter";
    
    
    
    int nSnRandNum = rand();
    NSString *uid = [UserDefaultManager GetclientNumber];
    NSString *sign = [[UxinUtility shareUtility] md5String:[NSString stringWithFormat:@"%d%@", nSnRandNum, PUBLIC_KEY]];
    NSString *pv = strpv;
    NSString *app_ver = uVersion;
//    NSString *app_ver = @"none";
    NSString *api_level = @"none";
    //NSString *osv = [[UIDevice currentDevice] systemVersion];
    NSString *osv = @"1.0";
    NSString *brand = ucsBrandid;
    NSString *model = @"iphone";
    NSString *imei = [[UCSStatisticalHelp instance] getIMEI];
    NSString *nettype =[NSString stringWithFormat:@""];
    
    /**
     @author WLS, 16-03-17 17:03:42
     
     添加app_id 和 user_id字段
     */
    NSString *app_id = [UserDefaultManager GetKeyChain:DATA_STORE_APPID];
    NSString *user_id = [UserDefaultManager GetUserID];
    
    
    PhoneNetType currentNetType = [PublicFunc GetCurrentPhoneNetType];
    if (PNT_UNKNOWN == currentNetType){//网络状态不好或者无网络
        
        
        
    }else{//网络状态正常
        
        switch (currentNetType) {
            case PNT_2G:
                nettype =[NSString stringWithFormat:@"0"];
                break;
            case PNT_WIFI:
                nettype =[NSString stringWithFormat:@"1"];
                break;
            case PNT_3G:
                nettype =[NSString stringWithFormat:@"2"];
                break;
            case PNT_4G:
                nettype =[NSString stringWithFormat:@"3"];
                break;
                //            case PNT_Cabel:
                //                nettype =[NSString stringWithFormat:@"4"];
                //                break;
                
            default:
                break;
        }
        
    }
    
    
    
    
    
    
    
    NSURL *url = nil;
    
    if ([UCSPublicFunc getIsUseTestServer] && [[UCSPublicFunc getIsUseTestServer] isEqualToString:@"TESTMODE"])//测试
    {
        url = [NSURL URLWithString:[NSString stringWithFormat:@"http://%@/v2/get_audiodevice?sn=%d&sign=%@&uid=%@&pv=%@&app_ver=%@&api_level=%@&osv=%@&brand=%@&model=%@&imei=%@&nettype=%@&app_id=%@&user_id=%@",UCS_GETCPSADDRESS,nSnRandNum,sign,uid,pv,app_ver,api_level,osv,brand,model,imei,nettype,app_id,user_id]]; //测试地址
        
        
//        url = [NSURL URLWithString:[NSString stringWithFormat:@"http://121.201.55.70:9997/v2/get_audiodevice?sn=%d&sign=%@&uid=%@&pv=%@&app_ver=%@&api_level=%@&osv=%@&brand=%@&model=%@&imei=%@&nettype=%@",nSnRandNum,sign,uid,pv,app_ver,api_level,osv,brand,model,imei,nettype]]; //测试地址
        UXLogAlert(@"----cps----:%@",url);

    }
    else if ([UCSPublicFunc getIsUseTestServer] && [[UCSPublicFunc getIsUseTestServer] isEqualToString:@"DEVMODE"])//DEV 模式
    {
        url = [NSURL URLWithString:[NSString stringWithFormat:@"http://172.16.10.36:9997/v2/get_audiodevice?sn=%d&sign=%@&uid=%@&pv=%@&app_ver=%@&api_level=%@&osv=%@&brand=%@&model=%@&imei=%@&nettype=%@&app_id=%@&user_id=%@",nSnRandNum,sign,uid,pv,app_ver,api_level,osv,brand,model,imei,nettype,app_id,user_id]]; //开发地址
    }
    else //正式
    {
        url = [NSURL URLWithString:[NSString stringWithFormat:@"http://cps.ucpaas.com:9997/v2/get_audiodevice?sn=%d&sign=%@&uid=%@&pv=%@&app_ver=%@&api_level=%@&osv=%@&brand=%@&model=%@&imei=%@&nettype=%@&app_id=%@&user_id=%@",nSnRandNum,sign,uid,pv,app_ver,api_level,osv,brand,model,imei,nettype,app_id,user_id]]; //正式地址
        
    }
    
    
    
    
    
    
    UCSASIHTTPRequest *request = [UCSASIHTTPRequest requestWithURL:url];
    
    //    [request setValidatesSecureCertificate:NO];//请求为HTTPS时需要设置这个属性
    
    //    [request addRequestHeader:@"Content-Type" value:@"application/json; encoding=utf-8"];
    //    [request addRequestHeader:@"Accept" value:@"application/json"];
    //    [request addRequestHeader:@"Authorization" value:auth_base64];
    //    [request setRequestMethod:@"POST"];
    //    [request setPostBody:tempJsonData];
    [request setDelegate:self];
    //    NSDictionary *httpDic = [NSDictionary dictionaryWithObjectsAndKeys:reportType,REPORTDATATYPE,nil];
    //    request.userInfo = httpDic;
    self.m_pUCSASIHTTPRequest = request;
    [request startAsynchronous];
    
    
    //        [jsonString release];
    
    //    }
    
    //        [reportData release];
    
    
    
    
}


 

 




#pragma mark - UCSASIHTTPRequestDelegate methods

// 请求失败，获取 error
- ( void )requestFailed:( UCSASIHTTPRequest *)request
{
    
    
    if (request == self.m_pUCSASIHTTPRequest)
    {
        

        

                if (nil != m_delegate && [m_delegate respondsToSelector:@selector(onLoginDidFailWithError:)])
                {
                    [m_delegate onLoginDidFailWithError:[request error]];
                }
 
    }
}


// 请求结束，获取 Response 数据
- (void) requestFinished:(UCSASIHTTPRequest *)request
{
    if (request == self.m_pUCSASIHTTPRequest)
    {
        NSData *m_webData = [request responseData];
        NSMutableDictionary *returnDic = [NSMutableDictionary dictionary];
        [returnDic setDictionary:[NSJSONSerialization JSONObjectWithData:m_webData options:0 error:nil]];
        NSLog(@"----------------UCSCPSManager-------------requestFinished:%@",returnDic);

//        NSMutableDictionary *resultDic =[returnDic objectForKey:@"resp"];
      
        



        int nRet = -1;//HTTP_REQUEST_SERVER_ERROR;
 

//         if ([[request.userInfo objectForKey:REPORTDATATYPE] isEqualToString:@"phoneInfo"])
//        {
        
            
//            nRet = [[returnDic objectForKey:@"result"] integerValue] ;
        
        
            /**
             @author WLS, 16-02-17 15:02:58
             
             保存cps返回结果
             */
            [UserDefaultManager SetKeyChain:[NSString stringWithFormat:@"%@",[returnDic objectForKey:@"result"]] key:DATA_STORE_CPS_RESULT];

            /**
             @author WLS, 16-01-20 14:02:43
             
             是否开启通话录音
             */
            BOOL record_enable = [[returnDic objectForKey:@"record_enable"]boolValue];
            [UserDefaultManager SetLocalDataBoolen:record_enable key:DATA_STORE_ISRECORD];

        
            NSMutableDictionary *permissionDic = [returnDic objectForKey:@"permission"] ;
            if ([returnDic count] > 0)
            {
               
                nRet = 0;
                
                switch (nRet)
                {
                    case 0: // success
                    {
                        
                        
                        //保存策略控制参数
                        [self setCPSParamer:permissionDic];
                        
                        
                        
                        
                    }
                        break;
                        
                    default:
                        break;
                }
            }
        
        
//        }
//         else if ([[request.userInfo objectForKey:REPORTDATATYPE] isEqualToString:@"errorCodeInfo"])
//        {
//        
//            if ([returnDic count] > 0)
//            {
//                nRet = [[returnDic objectForKey:@"code"] intValue];
//                
//                switch (nRet)
//                {
//                    case 0: // success
//                    {
//                        
//                        //上报错误码信息成功
//                        
//                        
//                        
//                    }
//                        break;
//                        
//                    default:
//                        break;
//                }
//            }
        
            
//        }
        
        
        
        if (nil != m_delegate && [m_delegate respondsToSelector:@selector(onLoginDidFinishLoading:)])
        {

            [m_delegate performSelector:@selector(onLoginDidFinishLoading:) withObject:nRet];
        }
    }
}

// save the session string
- (void)request:(UCSASIHTTPRequest *)request didReceiveResponseHeaders:(NSDictionary *)responseHeaders
{
    if (request == self.m_pUCSASIHTTPRequest)
    {
        NSString *cookie = [responseHeaders valueForKey:@"Set-Cookie"]; // It is your cookie
        cookie = [cookie stringByReplacingOccurrencesOfString:@"ssid=" withString:@""];
        cookie = [cookie stringByReplacingOccurrencesOfString:@"; Path=/" withString:@""];
        
//        [UserDefaultManager SetLocalDataString:cookie key:DATA_STORE_COOKIE];
        
//        NSLog(@"The session string is : %@", cookie);
    }
}


-(void)setCPSParamer:(NSMutableDictionary*)permissionDic{
    //策略控制参数
    NSString *audiofec = [NSString stringWithFormat:@"%d",[[permissionDic objectForKey:@"audiofec"] intValue]];
//    NSString *audiofec = @"1";

    NSString *autoadapter = [NSString stringWithFormat:@"%d",[[permissionDic objectForKey:@"autoadapter"]intValue]];
    NSString *iceenable = [NSString stringWithFormat:@"%d",[[permissionDic objectForKey:@"iceenable"]intValue]];
//    NSString *iceenable = @"1";
    
    NSString *logreport = [NSString stringWithFormat:@"%d",[[permissionDic objectForKey:@"logreport"] intValue]];
    NSString *prtpenable = [NSString stringWithFormat:@"%d",[[permissionDic objectForKey:@"prtpenable"] intValue]];
    NSString *vqmenable = [NSString stringWithFormat:@"%d",[[permissionDic objectForKey:@"vqmenable"] intValue]];
    //视频码率控制参数
    NSMutableDictionary *videoDic = [permissionDic objectForKey:@"video"];
    NSString *vp8enable = [NSString stringWithFormat:@"%d",[[videoDic objectForKey:@"vp8enable"] intValue]];
    NSString *h264enable = [NSString stringWithFormat:@"%d",[[videoDic objectForKey:@"h264enable"] intValue]];
    NSString *startbitrate = [NSString stringWithFormat:@"%d",[[videoDic objectForKey:@"startbitrate"] intValue]];
    NSString *minbitrate = [NSString stringWithFormat:@"%d",[[videoDic objectForKey:@"minbitrate"] intValue]];
    NSString *maxbitrate = [NSString stringWithFormat:@"%d",[[videoDic objectForKey:@"maxbitrate"] intValue]];
//    NSString *startbitrate = @"200";
//    NSString *minbitrate = @"30";
//    NSString *maxbitrate = @"500";
    
    NSString *firstpt = [NSString stringWithFormat:@"%d",[[videoDic objectForKey:@"firstpt"] intValue]];
    
    
    
    /**
     @author WLS, 16-02-26 14:02:13
     
     添加平台预设值的参数，根据平台预设值设置帧率码率。
     */
    NSString * presets = [NSString stringWithFormat:@"%@",[videoDic objectForKey:@"presets"]];
    
//    NSString * presets = @"1,0,-1:0,0,0:0,0,0";

    NSString * bitrates = [NSString stringWithFormat:@"%@",[videoDic objectForKey:@"bitrates"]];
    
//    NSString * bitrates = @"200,-1,-1:200,-1,-1:200,-1,-1";
    
    NSString * fps = [NSString stringWithFormat:@"%@",[videoDic objectForKey:@"fps"]];
    
//    NSString * fps = @"10,10,-1:12,15,15:12,15,15";

    
    [UserDefaultManager SetKeyChain:audiofec key:DATA_STORE_CPS_AUDIOFEC];
    [UserDefaultManager SetKeyChain:autoadapter key:DATA_STORE_CPS_AUTOADAPTER];
    [UserDefaultManager SetKeyChain:iceenable key:DATA_STORE_CPS_ICEENABLE];
    [UserDefaultManager SetKeyChain:logreport key:DATA_STORE_CPS_LOGREPORT];
    [UserDefaultManager SetKeyChain:prtpenable key:DATA_STORE_CPS_PRTPENABLE];
    [UserDefaultManager SetKeyChain:vqmenable key:DATA_STORE_CPS_VQMENABLE];
    
    
    [UserDefaultManager SetKeyChain:vp8enable key:DATA_STORE_CPS_VP8ENABLE];
    [UserDefaultManager SetKeyChain:h264enable key:DATA_STORE_CPS_H264ENABLE];
    [UserDefaultManager SetKeyChain:startbitrate key:DATA_STORE_CPS_STARTBITRATE];
    [UserDefaultManager SetKeyChain:minbitrate key:DATA_STORE_CPS_MINBITRATE];
    [UserDefaultManager SetKeyChain:maxbitrate key:DATA_STORE_CPS_MAXBITRATE];
    [UserDefaultManager SetKeyChain:firstpt key:DATA_STORE_CPS_FIRSTPT];
    /**
     @author WLS, 16-02-26 14:02:13
     
     添加平台预设值的参数，根据平台预设值设置帧率码率。
     */
    [UserDefaultManager SetKeyChain:presets key:DATA_STORE_CPS_PRESETS];
    [UserDefaultManager SetKeyChain:bitrates key:DATA_STORE_CPS_BITRATES];
    [UserDefaultManager SetKeyChain:fps key:DATA_STORE_CPS_FPS];

    
    
    
    
    //设置通知组件设置参数
    [[NSNotificationCenter defaultCenter] postNotificationName:kSetICE2FEC2EMODEL2PRTPNotification object:nil];
    
    
}


@end
