//
//  PublicFunc.h
//  UXinClient
//
//  Created by Liam Peng on 11-10-28.
//  Copyright 2011年 UXIN. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <AddressBook/AddressBook.h>

// 电话当前网络类型
typedef enum
{
    PNT_UNKNOWN = 0,    // 未知,无网络
    PNT_WIFI    = 1,    // WIFI
    PNT_2G      = 2,    // 2G
    PNT_3G      = 3,    // 3G
    PNT_4G      = 4     // 4G   add by  WLS 20151013
}PhoneNetType;


/**
 *  @brief 用法与NSLog一样，用于打印日志，其会将文件名和行号打印出来
 */
#define UCSDEBUGLog( s, ... ) \
NSLog( @"<<<%@::%@::%d>>> %@", [[NSString stringWithUTF8String:__FILE__] lastPathComponent], \
[NSString stringWithUTF8String: __FUNCTION__],       \
__LINE__, [NSString stringWithFormat:(s), ##__VA_ARGS__] )

// 地址薄释放内存
#define FreeMemory_CFRelease(p)    if (nil != p) {CFRelease(p); p = nil; }

// 公共函数定义
static BOOL bUseTestServerForServerGroup = NO;  // YES:服务器组使用的服务器标识别


extern int GetSignalStrength();

@interface PublicFunc : NSObject {
}



// 获取电话当前网络类型
+ (PhoneNetType) GetCurrentPhoneNetType;


// 处理电话号码，去除多余的字符,'-' '(' ')' '+86'等，得到纯粹的，干净的号码
+ (NSString *) DealWithPhoneNumber:(NSString *)strPhoneNumber;



// 消息提示框(只一个按钮的类型)
+ (void) ShowMessageBox:(int)msgboxID titleName:(NSString *)strTitle contentText:(NSString *)strContent cancelBtnName:(NSString *)strBtnName delegate:(id)delegate;




// RC4加密
+ (NSString*) EncryptRC4:(NSString*)aInput key:(NSString*)aKey;

// RC4解密
+(NSString*) DecryptRC4:(NSString*)aInput key:(NSString*)aKey;

// 字节码转换为十六进制字符串
+ (char*) ByteToHex:(const unsigned char*) vByte length:(const int)vLen;

// 十六进制字符串转换为字节码(注意要大写字母)
+ (unsigned char*) HexToByte:(const char*) szHex;


// 获取联盟ID
+ (NSString *) GetInviteID;


// 时间转换
+ (NSString *) intervalSinceNow: (NSString *) theDate;


// 是否是3G号码
+ (BOOL) is3GTelephoneNumber:(NSString *) phoneNumber;





//图片按比例缩放
+ (CGSize)scaledImage:(CGSize)artworkSize;




// 判断文件是否存在
// strFileName : 文件路径
+ (BOOL) isFileExist:(NSString *) strFileName;







// 创建一个随机数的流水号
+(NSString *)genRandomSn;



+ (NSString *)ucsCheckNullString:(NSString *)str;




@end
