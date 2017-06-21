//
//  UCSPublicDef.h
//
//  Created by Liam Peng on 14-04-08.
//  Copyright 2014年 KCT. All rights reserved.
//

#ifndef ucsapisdk_PublicDef_h
#define ucsapisdk_PublicDef_h

#define R(obj)              {if(obj) {[obj release]; obj = nil;}}
#define L(obj)              NSLocalizedString(obj, nil)



//-------------------------------------------UCS_SDK-------------------------------------------//





//-------------------------------------------UCS_SDK-------------------------------------------//


















//-------------------------------------------以下为格式命名参考-------------------------------------------//









///////////////////////


// 设备屏幕大小
#define __MainScreenFrame           [[UIScreen mainScreen] bounds]
// 设备屏幕宽
#define __MainScreen_Width          __MainScreenFrame.size.width
// 设备屏幕高
#define __MainScreen_Height         __MainScreenFrame.size.height
// 20,表示状态栏高度.如3.5inch 的高,得到的__MainScreenFrame.size.height是480,而去掉电量那条状态栏,我们真正用到的是460;

//-------------------------------------------便捷宏定义-------------------------------------------//
#define UXIN_MEM_RELEASE(x)     if (nil != (x)){[(x) release];(x)=nil;}
#define UXIN_HTTP_RELEASE(x)    if (nil != (x)){[(x) stopRequest];[(x) release];(x)=nil;}
//-------------------------------------------便捷宏定义-------------------------------------------//



//-------------------------------------------服务器接口-----------------------------------------------------//
// 接口密钥KEY
#define PUBLIC_KEY                          @"k1oET&Yh7@EQnp2XdTP1o/Vo="
#define PUBLIC_KEY_NEW                      @"sdvb753!@&*qpaltrko#$%^159plmokn" //from yangruibao

// 请求失败，未能正确得到服务器返回json数据
#define HTTP_REQUEST_SERVER_ERROR           -1000







// 问题反馈接口
#if UseTestServer
#define FEEDBACK_PRIZES                     @"http://183.62.140.171:8002/index.php?contact/addContact/Title/%@/Problem/%@/sign/%@/Version/%@/userId/%@/myBrand/%@/myModel/%@/myOs/%@/client/uxin/phone/%@"
#else
#define FEEDBACK_PRIZES                     @"http://m.uxin.com/index.php?contact/addContact/Title/%@/Problem/%@/sign/%@/Version/%@/userId/%@/myBrand/%@/myModel/%@/myOs/%@/client/uxin/phone/%@"
#endif

// 查询新版本、系统公告、动态信息
#if UseTestServer
#define SYSTEM_MESSAGE_SET                  @"http://183.62.140.171/index.php?update/index/version/%@/os/%@/invite/%@/imei/%@/sign/%@/msgid/%@/cfgid/%@/uid/%@/phone/%@/product/%@"
#else
#define SYSTEM_MESSAGE_SET                  @"http://m.uxin.com/index.php?update/index/version/%@/os/%@/invite/%@/imei/%@/sign/%@/msgid/%@/cfgid/%@/uid/%@/phone/%@/product/%@"
#endif


#endif
