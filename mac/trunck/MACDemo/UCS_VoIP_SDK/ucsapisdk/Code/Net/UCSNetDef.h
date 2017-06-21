//
//  NetDef.h
//  UXinClient
//
//  Created by Liam Peng on 12-1-15.
//  Copyright (c) 2012年 UXIN. All rights reserved.
//

#ifndef ucsapisdk_UCSNetDef_h
#define ucsapisdk_UCSNetDef_h

//-------------------------------------------服务器接口密钥-----------------------------------------------------//

// 接口密钥KEY
#define PUBLIC_KEY                              @"k1oET&Yh7@EQnp2XdTP1o/Vo="
#define PUBLIC_KEY_LOGIN                        @"~U!X@I#N$"
#define PUBLIC_KEY_GET_MO_NUMBER                @"c&18H$n!*G3k"

//-------------------------------------------服务器区分测试与发布版本---------------------------------//
#define UseTestServer                                   1   // 0：正式服务器，1：测试服务器 add by kucky 20141103
#define IsTestMode                                          //  是否为测试模式,测试模式使用.144服务器(注释 IsTestMode 后为正式服务器)

// 正式服务器
#define UCS_HTTP_SERVER                                @"http://im.ucpaas.com:8887/"
#define UCS_COMMBOOK_SERVER                            @"http://commbook.ucpaas.com:9098/"

// 代理服务器
#define UCS_COMMBOOK_SERVER_PROXY                      @"http://wap.uxin.com/UCS_commbook/"


// 测试服务器
//#define UCS_HTTP_SERVER_TEST--                           @"http://172.16.10.34/"
//#define UCS_HTTPS_SERVER_TEST--                           @"https://172.16.10.34/"
#define UCS_COMMBOOK_SERVER_TEST                       @"http://183.62.140.171:9098/"








#pragma mark - ------------------------------------回拨地址配置-----------------------------


//回拨地址--144测试
#define UCS_URL_CALLBACK_TEST                            [NSString stringWithFormat:@"%@%@", @"http://113.31.89.144:8887/", @"v3/callback?brandid=%@&sn=%@&called=%@&securityver=%@&uid=%@&p=%@&pv=%@&v=%@&u=%@&callid=%@&sign=%@&userid=%@&atype=%@"]

//回拨地址--开发环境
#define UCS_URL_CALLBACK_DEV                            [NSString stringWithFormat:@"%@%@", @"http://172.16.10.34:8020/", @"v3/callback?brandid=%@&sn=%@&called=%@&securityver=%@&uid=%@&p=%@&pv=%@&v=%@&u=%@&callid=%@&sign=%@&userid=%@&atype=%@"]

//回拨地址--正式
#define UCS_URL_CALLBACK                            [NSString stringWithFormat:@"%@%@", @"http://172.16.10.126:8020/", @"v3/callback?brandid=%@&sn=%@&called=%@&securityver=%@&uid=%@&p=%@&pv=%@&v=%@&u=%@&callid=%@&sign=%@&userid=%@&atype=%@"]




//取消回拨地址--144测试
#define UCS_URL_CANCELCALLBACK_TEST                            [NSString stringWithFormat:@"%@%@", @"http://113.31.89.144:8887/", @"v3/cancelcall?brandid=%@&sn=%@&called=%@&uid=%@&callid=%@&sign=%@&userid=%@&atype=%@"]

//取消回拨地址--开发环境
#define UCS_URL_CANCELCALLBACK_DEV                            [NSString stringWithFormat:@"%@%@", @"http://172.16.10.34:8020/", @"v3/cancelcall?brandid=%@&sn=%@&called=%@&uid=%@&callid=%@&sign=%@&userid=%@&atype=%@"]

//取消回拨地址--正式
#define UCS_URL_CANCELCALLBACK                            [NSString stringWithFormat:@"%@%@", @"http://172.16.10.126:8020/", @"v3/cancelcall?brandid=%@&sn=%@&called=%@&uid=%@&callid=%@&sign=%@&userid=%@&atype=%@"]










#pragma mark - ------------------------------------rtpp拉取地址配置-----------------------------

#define UCS_CPSPORT [[[NSUserDefaults standardUserDefaults] stringForKey:@"cpsPort"] isEqualToString:@""]||[[NSUserDefaults standardUserDefaults] stringForKey:@"cpsPort"]==nil?@"9997":[[NSUserDefaults standardUserDefaults] stringForKey:@"cpsPort"]
#define UCS_CPSADDRESS [[[NSUserDefaults standardUserDefaults] stringForKey:@"cpsAddress"] isEqualToString:@""]||[[NSUserDefaults standardUserDefaults] stringForKey:@"cpsAddress"]==nil?@"113.31.89.144":[[NSUserDefaults standardUserDefaults] stringForKey:@"cpsAddress"]
#define UCS_GETCPSADDRESS [NSString stringWithFormat:@"%@:%@",UCS_CPSADDRESS,UCS_CPSPORT]


/**
 @author WLS, 16-02-29 10:02:51
 
 rtpp拉取的测试环境
 */
#define UCS_STATIC_ADDRESS_TEST                      [NSString stringWithFormat:@"http://%@/%@", UCS_GETCPSADDRESS, @"v2/getrtpplist?clientnum=%@"]

/**
 @author WLS, 16-02-29 10:02:51
 
 rtpp拉取的开发环境
 */
#define UCS_STATIC_ADDRESS_DEV                      [NSString stringWithFormat:@"%@%@", @"http://113.31.89.144:9997/", @"v2/getrtpplist?clientnum=%@"]

/**
 @author WLS, 16-02-29 10:02:51
 
 rtpp拉取的正式环境
 */
#define UCS_STATIC_ADDRESS                      [NSString stringWithFormat:@"%@%@", @"http://cps.ucpaas.com:9997/", @"v2/getrtpplist?clientnum=%@"]




//#define UCS_STATIC_ADDRESS                      [NSString stringWithFormat:@"%@%@", @"http://113.31.89.131/", @"static/address1.txt"]
// 备用服务器地址
#define UCS_STATIC_ADDRESS2                     @"http://113.31.89.131/static/address.txt"





#pragma mark - ------------------------------------会话日志上报、crash日志上报、录音文件上报----------------

#define UCSUPLOADURL_TEST @"http://113.31.130.125:9094/log-storage/upload"
#define UCSUPLOADURL_DEV @"http://113.31.130.125:9094/log-storage/upload"
#define UCSUPLOADURL @"http://ulog.ucpaas.com/log-storage/upload"
/**
 @author WLS, 16-01-28 17:01:56
 
 测试环境 录音文件上报
 */
#define UCS_UPLOAD_RECORD_TEST [NSString stringWithFormat:@"%@%@",UCSUPLOADURL_TEST,@"/record"]
/**
 @author WLS, 16-01-28 17:01:11
 
 开发环境 录音文件上报
 */
#define UCS_UPLOAD_RECORD_DEV [NSString stringWithFormat:@"%@%@",UCSUPLOADURL_DEV,@"/record"]
/**
 @author WLS, 16-01-28 17:01:32
 
 线上环境 录音文件上报
 */
#define UCS_UPLOAD_RECORD [NSString stringWithFormat:@"%@%@",UCSUPLOADURL,@"/record"]



/**
 @author WLS, 16-01-28 17:01:56
 
 测试环境 会话日志上报
 */
#define UCS_UPLOAD_LOG_TEST [NSString stringWithFormat:@"%@%@",UCSUPLOADURL_TEST,@"/log"]
/**
 @author WLS, 16-01-28 17:01:11
 
 开发环境 会话日志上报
 */
#define UCS_UPLOAD_LOG_DEV [NSString stringWithFormat:@"%@%@",UCSUPLOADURL_DEV,@"/log"]
/**
 @author WLS, 16-01-28 17:01:32
 
 线上环境 会话日志上报
 */
#define UCS_UPLOAD_LOG [NSString stringWithFormat:@"%@%@",UCSUPLOADURL,@"/log"]



/**
 @author WLS, 16-01-28 17:01:56
 
 测试环境 会话日志上报
 */
#define UCS_UPLOAD_CRASH_TEST [NSString stringWithFormat:@"%@%@",UCSUPLOADURL_TEST,@"/crash"]
/**
 @author WLS, 16-01-28 17:01:11
 
 开发环境 会话日志上报
 */
#define UCS_UPLOAD_CRASH_DEV [NSString stringWithFormat:@"%@%@",UCSUPLOADURL_DEV,@"/crash"]
/**
 @author WLS, 16-01-28 17:01:32
 
 线上环境 会话日志上报
 */
#define UCS_UPLOAD_CRASH [NSString stringWithFormat:@"%@%@",UCSUPLOADURL,@"/crash"]


#endif
