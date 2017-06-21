//
//  UserDefaultManager.m
//  UXinClient
//
//  Created by Lenya Han on 7/2/12.
//  Copyright (c) 2012 UXIN. All rights reserved.
//

#import "UserDefaultManager.h"
#import "UserKeyChainManager.h"
#import "Signature.h"
@implementation UserDefaultManager

// 默认账号为CHN
static BOOL     s_bAccountIsCHN           = YES;


- (id)init
{
    self = [super init];
    if (self) 
    {
        // Initialization code here.
    }
    
    return self;
}


// 从本地数据库获取用户账号
+ (NSString *) GetUserID
{
    NSString * data = [self GetKeyChain:DATA_STORE_ID];
    if (nil != data && [data isKindOfClass:[NSString class]])
    {
        return data;
    }
    
    return data;
}

//从本地数据库获取clientNumber
+ (NSString *) GetclientNumber{
    
    NSString * data = [self GetKeyChain:DATA_STORE_CLIENTNUMBER];
    if (nil != data && [data isKindOfClass:[NSString class]])
    {
        return data;
    }
    
    return data;
}


// 从本地数据库获取用户手机号码
+ (NSString *)GetUserMobile
{
    NSString * data = [self GetKeyChain:DATA_STORE_MOBILE];
    if (nil != data && [data isKindOfClass:[NSString class]])
    {
        return data;
    }
    
    return data;
}


// Set And Get AC
+ (void) SetIMSSID:(NSString *)strIMSSID
{
    if (nil != strIMSSID && [strIMSSID isKindOfClass:[NSString class]])
    {
        [UserDefaultManager SetLocalDataString:strIMSSID key:DATA_STORE_IM_SSID];
    }
}

+ (NSString *) GetIMSSID
{
    NSString *strIMSSID = [UserDefaultManager GetLocalDataString:DATA_STORE_IM_SSID];
    
    if (nil == strIMSSID)
    {
        strIMSSID = @"";
    }
    return strIMSSID;
}


//从本地数据库获取用户类型atype 新用户还是老用户 WLS
+ (int) GetUserAtype{
    
    NSString * data = [self GetKeyChain:DATA_STORE_ATYPE];
    if (nil != data && [data isKindOfClass:[NSString class]])
    {
        
        
        return [data intValue];
    }
    
    return 0;
    
}




static NSMutableDictionary* g_cacheDictKeyChain = nil;

// 设置keyChain数据
+ (void) SetKeyChain:(NSString *)data key:(NSString *)aKay
{
    if (nil == data || (nil != data && [data isKindOfClass:[NSString class]]))
    {
        [UserKeyChainManager saveData:data key:aKay];
        
        if( g_cacheDictKeyChain == nil )
        {
            g_cacheDictKeyChain = [[NSMutableDictionary alloc] initWithCapacity:64];
        }
        
        if( g_cacheDictKeyChain && aKay )
        {
            if ( [g_cacheDictKeyChain objectForKey:aKay] )
            {
                NSLog(@"remove data from g_cacheDictKeyChain2:%@ - %@", aKay, [g_cacheDictKeyChain objectForKey:aKay]);
                [g_cacheDictKeyChain removeObjectForKey:aKay];
            }
            if( data )
            {
                [g_cacheDictKeyChain setObject:data forKey:aKay];
                NSLog(@"set data to g_cacheDictKeyChain2:%@ - %@", aKay, data);
            }
        }
    }
}

//获得keyChain的通用数据
+ (NSString *) GetKeyChain:(NSString *)aKay
{
    NSString * data = nil;
    
    if( aKay )
    {
        if( g_cacheDictKeyChain )
        {
            data = [g_cacheDictKeyChain objectForKey:aKay];
            //NSLog(@"get data from g_cacheDictKeyChain:%@ - %@", aKay, data);
        }
        
        if( data == nil )
        {
            data = [UserKeyChainManager readData:aKay];
            if( data && g_cacheDictKeyChain )
            {
                [g_cacheDictKeyChain setObject:data forKey:aKay];
                NSLog(@"set data to g_cacheDictKeyChain:%@ - %@", aKay, data);
            }
        }
    }
    return data;
}

//删除所有keyChin数据
+ (void) deleteAllKeyChain
{
    [UserKeyChainManager deleteAllData];
    if( g_cacheDictKeyChain )
    {
        [g_cacheDictKeyChain removeAllObjects];
        NSLog(@"remove all data from g_cacheDictKeyChain");
    }
}




// 判断账号是否中国号码
// 通过保存的国家区号来判断
+ (BOOL) isAccountCHN
{    
    return s_bAccountIsCHN;
}

// 从本地数据库获对应key值下的数据(字符串)
// aKey : key值
// 返回值，返回key值下的数据
+ (NSString *) GetLocalDataString:(NSString *)aKey
{
    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
    
    if (nil == defaults || nil == aKey) {
        return nil;
    }
    
    NSString *strRet = [defaults objectForKey:aKey];
    
    return strRet;
}

// 从本地数据库获对应key值下的数据(Object)
+ (id) GetLocalDataObject:(NSString *)aKey
{
    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
    if (nil == defaults || nil == aKey)
    {
        return nil;
    }
    
    id strRet = [defaults objectForKey:aKey];
    
    return strRet;
}

// 设置本地数据库对应key值下的数据(字符串)
// aValue   : 要保存的数据
// aKey     : key值
+ (void)SetLocalDataString:(NSString *)aValue key:(NSString *)aKey
{
    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
    
    if (nil == defaults || nil == aKey)
    {
        return;
    }
    
    [defaults setValue:aValue forKey:aKey];
}

// 设置本地数据库对应key值下的数据(字object串)
+ (void) SetLocalDataObject:(id)aValue key:(NSString *)aKey
{
    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
    if (nil == defaults || nil == aKey)
    {
        return;
    }
    
    [defaults setValue:aValue forKey:aKey];
}

// 从本地数据库获对应key值下的数据(bool)
// aKey     : key值
// 返回值，返回存储的bool值
+ (bool) GetLocalDataBoolen:(NSString *)aKey
{
    bool bRet = false;
    
    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
    if (nil == defaults || nil == aKey)
    {
        bRet = false;
    }
    else
    {
        bRet = [defaults boolForKey:aKey];
    }
    
    return bRet;
}

// 设置本地数据库对应key值下的数据(bool)
// bValue   : 要保存的BOOL值
// aKey     : key值
+ (void) SetLocalDataBoolen:(bool)bValue key:(NSString *)aKey
{
    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
    if (nil == defaults || nil == aKey)
    {
        return;
    }
    
    [defaults setBool:bValue forKey:aKey];
}


// 获得int型。
+ (int) GetLocalDataInt:(NSString *)aKey
{
    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
    
    if (nil == defaults && aKey == nil)
    {
        return 0;
    }
    else
    {
        return [defaults integerForKey:aKey];
    }
}

// 设置int型。
+ (void) SetLocalDataInt:(int)num key:(NSString *)aKey
{
    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
    
    if (nil == defaults && aKey == nil)
    {
        return;
    }
    [defaults setInteger:num forKey:aKey];
}
// 获得ssid string
+ (NSString *) getCookieString 
{
    return [NSString stringWithFormat:@"ssid=%@; Path=/", [self GetLocalDataString:DATA_STORE_COOKIE]];  
}  

// 删除key
+ (void) removeLocalDataForKey:(NSString *)akey
{
    [[NSUserDefaults standardUserDefaults] removeObjectForKey:akey];
}


// 保存个人资料
+ (void) setUserInfo:(NSMutableDictionary *)data
{
    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
    [defaults synchronize];
    [defaults setObject:data forKey:[NSString stringWithFormat:DATA_USERINFO, [UserDefaultManager GetUserID]]];
}



// 视频状态是否支持预览
+ (int) getVideoPreView
{
    return [self GetLocalDataInt:DATA_STORE_UD_ISVIDEINGPREVIEW]?[self GetLocalDataInt:DATA_STORE_UD_ISVIDEINGPREVIEW]:1;
}
// 视频状态是否支持预览
+ (void) setVideoPreView:(int)isPreView
{
    [self SetLocalDataInt:isPreView key:DATA_STORE_UD_ISVIDEINGPREVIEW];
}


@end
