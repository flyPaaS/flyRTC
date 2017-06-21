//
//  PublicFunc.m
//  UXinClient
//
//  Created by Liam Peng on 11-10-28.
//  Copyright 2011年 UXIN. All rights reserved.
//

#import "PublicFunc.h"
#import "rc4.h"
#import "uxinbase64.h"
#import<CoreTelephony/CTTelephonyNetworkInfo.h>
#import<CoreTelephony/CTCarrier.h>
#include <libkern/OSAtomic.h>
#include <mach-o/loader.h>
#include <dlfcn.h>

#define IM_IMAGE_WIDTH_MIN         36
#define IM_IMAGE_HEIGHT_MIN        36
#define IM_IMAGE_WIDTH_MAX         100
#define IM_IMAGE_HEIGHT_MAX        100

static NSString *KCID_Dictionary_Synch = @"KCID_Dictionary_Synch";



int GetSignalStrength()
{
    void *libHandle = dlopen("/System/Library/Frameworks/CoreTelephony.framework/CoreTelephony", RTLD_LAZY);    int (*CTGetSignalStrength)();
    CTGetSignalStrength = dlsym(libHandle, "CTGetSignalStrength");
    if( CTGetSignalStrength == NULL) {
        return -1;
    }
    int result = CTGetSignalStrength();
    dlclose(libHandle);
    return result;
}

@implementation PublicFunc

- (id)init
{
    self = [super init];
    if (self) 
    {
        // Initialization code here.
    }
    
    return self;
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
    nPhoneNetType = PNT_WIFI;
    return nPhoneNetType;
}


// 处理电话号码，去除多余的字符,'-' '(' ')' '+86'等，得到纯粹的，干净的号码
// strPhoneNumber : 电话号码
+ (NSString *) DealWithPhoneNumber:(NSString *)strPhoneNumber
{
    NSString *strReault = [strPhoneNumber stringByReplacingOccurrencesOfString:@" " withString:@""];
    // 去掉'-'
    strReault = [strReault stringByReplacingOccurrencesOfString:@"-" withString:@""];
    // 去掉'('和')'
    strReault = [strReault stringByReplacingOccurrencesOfString:@"(" withString:@""];
    strReault = [strReault stringByReplacingOccurrencesOfString:@")" withString:@""];
    
    // 判断账号是中国的，则要去掉+86，0086，17951等前缀
    if ([UserDefaultManager isAccountCHN])
    {
        // 去掉'+86'
        strReault = [strReault stringByReplacingOccurrencesOfString:@"+86" withString:@""];
        
        BOOL bPreFix = NO;
        
        // 去掉号码前面加的"0086"
        bPreFix = [strReault hasPrefix:@"0086"];
        if (bPreFix)
        {
            strReault = [strReault stringByReplacingCharactersInRange:NSMakeRange(0, 4) withString:@""];
        }
        
        // 去掉号码前面加的“12593”
        bPreFix = [strReault hasPrefix:@"12593"];
        if (bPreFix)
        {
            strReault = [strReault stringByReplacingCharactersInRange:NSMakeRange(0, 5) withString:@""];
        }
        
        // 去掉号码前面加的“17951”
        bPreFix = [strReault hasPrefix:@"17951"];
        if (bPreFix)
        {
            strReault = [strReault stringByReplacingCharactersInRange:NSMakeRange(0, 5) withString:@""];
        }
        
        // 去掉号码前面加的“17911”
        bPreFix = [strReault hasPrefix:@"17911"];
        if (bPreFix)
        {
            strReault = [strReault stringByReplacingCharactersInRange:NSMakeRange(0, 5) withString:@""];
        }
        bPreFix = [strReault hasPrefix:@"17909"];
        if (bPreFix)
        {
            strReault = [strReault stringByReplacingCharactersInRange:NSMakeRange(0, 5) withString:@""];
        }
    }
    
    if ([strReault length] < 1)
    {
        return @"";
    }
    else
    {
        // 1.(38).013-8000  // 0xC2 0xA0 // NO-BREAK SPACE
        // 31 C2 A0 28 33 38 30 29 C2 A0 30 31 33 2D 38 30 30 30
        // 31 C2 A0    33 38 30    C2 A0 30 31 33    38 30 30 30

        // ;&nbsp

        // http://www.utf8-chartable.de/unicode-utf8-table.pl?names=-&utf8=0x
        // http://stackoverflow.com/questions/15835849/why-are-these-unicode-characters-not-printed-although-i-set-my-environment-to-ut
        //
        // You won't see much even if your terminal is configured to work with UTF-8 because
        // the characters you are 'displaying' are:
        // 0xC2 0x82 = U+0082
        // 0xC2 0x81 = U+0081
        // 0xC2 0x80 = U+0080
        //
        // These are control-characters from the C1 set. I have a data file which documents:
        //
        // # C1 Controls (0x80 - 0x9F) are from ISO/IEC 6429:1992
        // # It does not define names for 80, 81, or 99.
        // 80 U+0080
        // 81 U+0081
        // 82 U+0082 BPH BREAK PERMITTED HERE
        //
        // So you don't see anything because you aren't displaying any graphic characters.
        // If you change your 0x82 to 0xA2, for example (and 0x81 to 0xA1, and 0x80 to 0xA0),
        // then you'll be more likely to get some visible output:
        //
        //    0xC2 0xA2 = U+00A2
        //    0xC2 0xA1 = U+00A1
        //    0xC2 0xA0 = U+00A0
        //
        //    A0 U+00A0 NO-BREAK SPACE
        //    A1 U+00A1 INVERTED EXCLAMATION MARK
        //    A2 U+00A2 CENT SIGN

        NSString* strNewResult = @"";
        for( int i=0; i<[strReault length]; ++i )
        {
            NSString* strTest = [[strReault substringFromIndex:i] substringToIndex:1];
            if( strTest &&
               (NSOrderedSame == [strTest compare:@"0"] ||
                NSOrderedSame == [strTest compare:@"1"] ||
                NSOrderedSame == [strTest compare:@"2"] ||
                NSOrderedSame == [strTest compare:@"3"] ||
                NSOrderedSame == [strTest compare:@"4"] ||
                NSOrderedSame == [strTest compare:@"5"] ||
                NSOrderedSame == [strTest compare:@"6"] ||
                NSOrderedSame == [strTest compare:@"7"] ||
                NSOrderedSame == [strTest compare:@"8"] ||
                NSOrderedSame == [strTest compare:@"9"] ) )
            {
                strNewResult = [NSString stringWithFormat:@"%@%@", strNewResult, strTest];
            }
            else
            {
                //const unsigned char* pTest = (const unsigned char*)[strTest UTF8String];
                NSLog(@"DealWithPhoneNumber WHAT? %@", strTest);
            }
        }

        return strNewResult;
    }
}







// 消息提示框(只一个按钮的类型)
// msgboxID     : ID
// strTitle     : 标题
// strContent   : 内容
// strBtnName   : 按钮名称
// delegate     : 委托窗口，不需要则设置为nil
+ (void) ShowMessageBox:(int)msgboxID titleName:(NSString *)strTitle contentText:(NSString *)strContent cancelBtnName:(NSString *)strBtnName delegate:(id)delegate
{
//    UIAlertView *msgbox = [[UIAlertView alloc] initWithTitle:strTitle
//                                                     message:strContent 
//                                                    delegate:delegate 
//                                           cancelButtonTitle:strBtnName
//                                           otherButtonTitles:nil];
//    msgbox.tag = msgboxID;
//    [msgbox show];
//    [msgbox release];
    NSAlert *alert = [NSAlert alertWithMessageText:strTitle defaultButton:strBtnName alternateButton:nil otherButton:nil informativeTextWithFormat:strContent];
    int index = [alert runModal];
}







// RC4加密
// aInput   :   需要加密的串
// aKey     :   Key
// 返回值，RC4加密后的十六进制字符串
+ (NSString*) EncryptRC4:(NSString*)aInput key:(NSString*)aKey
{
    NSString *result = aInput;
    
    RC4_KEY key;
	unsigned char *p = NULL;
    
	p = (unsigned char *)[aKey cStringUsingEncoding:NSISOLatin1StringEncoding];
	_RC4_set_key(&key, [aKey length], p);
    
    unsigned char* src = (unsigned char *)[aInput cStringUsingEncoding:NSISOLatin1StringEncoding];
	int nLen = [aInput length];
    
	unsigned char *pStrReaultEncrypt = (unsigned char *)malloc(nLen * 2 + 1);
    
	_RC4(&key, nLen, src, pStrReaultEncrypt);
	pStrReaultEncrypt[nLen * 2] = 0;
    
    char *pASCII16 = [PublicFunc ByteToHex:pStrReaultEncrypt length:nLen];
    result = [NSString stringWithCString:pASCII16 encoding:NSISOLatin1StringEncoding];
    free(pStrReaultEncrypt);
    free(pASCII16);

    return result;
}

+(NSString*)DecryptRC4:(NSString*)aInput key:(NSString*)aKey
{
    NSString *result = aInput;
    RC4_KEY key;
	unsigned char *p = NULL;
	p = (unsigned char *)[aKey cStringUsingEncoding:NSISOLatin1StringEncoding];
	_RC4_set_key(&key, [aKey length], p);
    const char * src2 = (const char *)[aInput cStringUsingEncoding:NSISOLatin1StringEncoding];
	int nLen = [aInput length];
    unsigned char *src=[PublicFunc HexToByte:src2];
	unsigned char *pStrReaultEncrypt = (unsigned char *)malloc(nLen/2 + 1);
	_RC4(&key, nLen/2, src, pStrReaultEncrypt);
	pStrReaultEncrypt[nLen/2] = 0;
    result = [NSString stringWithCString:(char *)pStrReaultEncrypt encoding:NSISOLatin1StringEncoding];
    free(pStrReaultEncrypt);
    free(src);
    return result;
}
// 字节码转换为十六进制字符串
// vByte    :   字节码地址
// vLen     :   长度
// 返回值，char*，返回转换后的16进制字符串，空间新分配，需  hungup 
+ (char*) ByteToHex:(const unsigned char*) vByte length:(const int)vLen
{
	if(!vByte)
		return NULL;
    
	char* tmp = (char *)malloc(vLen * 2 + 1); // 一个字节两个十六进制码，最后多一个'\0'
    
	int tmp2;
	for (int i = 0; i < vLen; ++i)
	{
		tmp2 = (int)(vByte[i]) / 16;
		tmp[i * 2] = (char)(tmp2 + ((tmp2 > 9) ? 'A'-10 : '0'));
		tmp2 = (int)(vByte[i]) % 16;
		tmp[i * 2 + 1] = (char)(tmp2 + ((tmp2 > 9) ? 'A' -10 : '0'));
	}
    
	tmp[vLen * 2] = '\0';
	return tmp;
}

// 十六进制字符串转换为字节码(注意要大写字母)
// szHex    :   十六进制字符串
// 返回值，unsigned char*，返回转换后的字节码地址，空间新分配，需  hungup 
+ (unsigned char*) HexToByte:(const char*) szHex
{
	if(!szHex) 
		return NULL;
    
	int iLen = (int)strlen(szHex);
    
	if (iLen <= 0 || 0 != iLen % 2) 
		return NULL;
    
	unsigned char* pbBuf = (unsigned char *)malloc(iLen / 2);  // 数据缓冲区
    
	int tmp1,tmp2;
	for (int i = 0; i < iLen / 2; ++i)
	{
		tmp1 = (int)szHex[i * 2] - (((int)szHex[i * 2] >= 'A') ? 'A' - 10 : '0');
        
		if(tmp1 >= 16) {
            free(pbBuf);
            return NULL;
        }
			
        
		tmp2 = (int)szHex[i * 2 + 1] - (((int)szHex[i * 2 + 1] >= 'A') ? 'A' - 10 : '0');
        
		if(tmp2 >= 16) {
            free(pbBuf);
            return NULL;
        }
			
        
		pbBuf[i] = (tmp1 * 16 + tmp2);
	}
    
	return pbBuf;
}



// 获取联盟ID
+ (NSString *) GetInviteID
{
    NSString* string = [[NSBundle mainBundle] pathForResource:@"Info" ofType:@"plist"];
    
    NSDictionary *tem  = [NSDictionary dictionaryWithContentsOfFile:string];
    
    NSString *inviteID ;
    
    if ([tem objectForKey:@"inviteID"])
    {
        inviteID = [tem objectForKey:@"inviteID"];
    }
    else
    {
        inviteID = @"0";
    }
    
    return inviteID;
}




// 时间转换
+ (NSString *) intervalSinceNow: (NSString *) theDate 
{
    NSDateFormatter *date=[[NSDateFormatter alloc] init];
    [date setTimeZone:[NSTimeZone localTimeZone]];
    [date setDateFormat:@"yyyy-MM-dd HH:mm:ss"];
    NSDate *d=[date dateFromString:theDate];
    
    NSTimeInterval late=[d timeIntervalSince1970]*1;
    
    NSDate* dat = [NSDate dateWithTimeIntervalSinceNow:0];
    NSTimeInterval now=[dat timeIntervalSince1970]*1;
    NSString *timeString=@"";
    
    [date setDateFormat:@"H"];
    int today =   [[date stringFromDate:dat] intValue] *3600;
    
    [date setDateFormat:@"m"];
    
    today +=  [[date stringFromDate:dat] intValue]*60;
    
    NSTimeInterval cha= now - late;
    
    if (cha < today) 
    {
        [date setDateFormat:@"HH:mm"];
        
        timeString=[date stringFromDate:d];
    }
    else if (cha >today &&cha< today +24*3600) 
    {
        timeString=@"昨天";
    }
    else
    {
        [date setDateFormat:@"M月d日"];
        
        timeString=[date stringFromDate:d];
    }
    [date release];
    return timeString;
}


// 是否是3G号码
// phoneNumber : 号码
+ (BOOL) is3GTelephoneNumber:(NSString *) phoneNumber
{
    BOOL rv = NO; 
    if ([phoneNumber length] > 3)
    {
        NSInteger first3Number = [[[PublicFunc DealWithPhoneNumber:phoneNumber] substringToIndex:3] integerValue];
        switch (first3Number)
        {
            case 186:
            case 185:
            case 188:
            case 187:
            case 189:
            case 180:
                rv = YES;
                break;
            default:
                break;
        }   
    }
    
    return rv;
}





//图片按比例缩放
+ (CGSize)scaledImage:(CGSize)artworkSize
{
    float Wo = artworkSize.width;
    float Ho = artworkSize.height;
    float x = 0;
    float y = 0;
    if(Wo >= IM_IMAGE_WIDTH_MIN && Wo <= IM_IMAGE_WIDTH_MAX && Ho >= IM_IMAGE_HEIGHT_MIN && Ho <= IM_IMAGE_HEIGHT_MAX) //中中
    {
        x = Wo;
        y = Ho;
    }
    else if(Wo < IM_IMAGE_WIDTH_MIN && Ho < IM_IMAGE_HEIGHT_MIN) //小小
    {
        
        if (Wo > Ho)
        {
            x = Wo * IM_IMAGE_HEIGHT_MIN / Ho;
            y = IM_IMAGE_HEIGHT_MIN;
        }
        else
        {
            
            y = Ho * IM_IMAGE_WIDTH_MIN / Wo;
            x = IM_IMAGE_WIDTH_MIN;
        }
        
    }
    else if(Wo > IM_IMAGE_HEIGHT_MAX && Wo > Ho) //大小
    {
        y = Ho * IM_IMAGE_WIDTH_MAX / Wo;
        if (y >= IM_IMAGE_HEIGHT_MIN)
        {
            x = IM_IMAGE_WIDTH_MAX;
        }
        else
        {
            x = Wo * IM_IMAGE_HEIGHT_MIN / Ho;
            if (x > IM_IMAGE_WIDTH_MAX)
            {
                x = IM_IMAGE_WIDTH_MAX; // 取中间
            }
            y = IM_IMAGE_HEIGHT_MIN;
            
        }
    }
    else if(Ho > IM_IMAGE_HEIGHT_MAX && Ho > Wo) //小大
    {
        x = Wo * IM_IMAGE_HEIGHT_MAX / Ho;
        if (x >= IM_IMAGE_WIDTH_MIN)
        {
            y = IM_IMAGE_HEIGHT_MAX;
        }
        else
        {
            y = Ho * IM_IMAGE_HEIGHT_MIN / Wo;
            if (y > IM_IMAGE_HEIGHT_MAX)
            {
                y = IM_IMAGE_HEIGHT_MAX; // 取中间
            }
            x = IM_IMAGE_WIDTH_MIN;
        }
        
    }
    else if(Wo >= IM_IMAGE_WIDTH_MIN && Wo <= IM_IMAGE_WIDTH_MAX && Ho < IM_IMAGE_HEIGHT_MIN) //中小
    {
        y = IM_IMAGE_HEIGHT_MIN;
        x = Wo * y / Ho;
        if (x > IM_IMAGE_WIDTH_MAX)
        {
            x = IM_IMAGE_WIDTH_MAX; // 取中间
        }
    }
    else if(Ho >= IM_IMAGE_HEIGHT_MIN && Ho <= IM_IMAGE_WIDTH_MIN && Wo < IM_IMAGE_WIDTH_MIN) //小中
    {
        x = IM_IMAGE_HEIGHT_MIN;
        y = Ho * x / Wo;
        if (y > IM_IMAGE_HEIGHT_MAX)
        {
            y = IM_IMAGE_HEIGHT_MAX; // 取中间
        }
    }
    else if(Wo == Ho)        //相等的时候
    {
        if (Wo > IM_IMAGE_WIDTH_MAX)
        {
            x = IM_IMAGE_WIDTH_MAX;
            y = IM_IMAGE_HEIGHT_MAX;
        }
        else if(Wo <= IM_IMAGE_WIDTH_MAX && Wo >= IM_IMAGE_WIDTH_MIN)
        {
            x = Wo;
            y = Ho;
        }
        else
        {
            x = IM_IMAGE_WIDTH_MIN;
            y = IM_IMAGE_HEIGHT_MIN;
        }
    }
    CGSize newSize = CGSizeMake(x, y);
    return  newSize;
}








// 判断文件是否存在
// strFileName : 文件路径
+ (BOOL) isFileExist:(NSString *) strFileName
{
    return [[NSFileManager defaultManager] fileExistsAtPath:strFileName];
}








+ (NSString *)genRandomSn
{
    return [[[[NSString stringWithFormat:@"%@%lX%X", @"110110",
               time(NULL), rand() % 65535] stringByReplacingOccurrencesOfString:@"\r" withString:@""] stringByReplacingOccurrencesOfString:@"\n" withString:@""] stringByReplacingOccurrencesOfString:@":" withString:@""];
}


+ (NSString *)ucsCheckNullString:(NSString *)str{
    
    if (str == nil ||
        [str isKindOfClass:[NSNull class]]) {
        return nil;
    }
    
    return str;
}


@end
