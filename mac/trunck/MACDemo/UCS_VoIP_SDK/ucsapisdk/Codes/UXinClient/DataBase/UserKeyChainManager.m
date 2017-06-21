//
//  WQUserDataManager.m
//  KeyChain
//
//  Created by Lenya Han on 7/10/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "UserKeyChainManager.h"
#import "KeyChain.h"


@implementation UserKeyChainManager

#define KEY_IN_KEYCHAIN [NSString stringWithFormat:@"%@.allinfo", kBundleID]

/** 
 *  @brief  存储数据 
 * 
 *  @param  data    密码内容 
 */  
+(void)saveData:(NSString *)data key:(NSString *)key
{  
    if (data == nil)
    {
        data = @"";
    }
    NSMutableDictionary *userKVPair = (NSMutableDictionary *)[KeyChain load:KEY_IN_KEYCHAIN];  
    if (!userKVPair)
    {
        userKVPair = [NSMutableDictionary dictionary];  
    }
    [userKVPair setObject:data forKey:key];  
    [KeyChain save:KEY_IN_KEYCHAIN data:userKVPair];  
}  

/** 
 *  @brief        读取密码 
 * 
 *  @param  key   数据对应的key
 *  @return       数据内容 
 */  
+(id)readData:(NSString *)key
{  
    NSMutableDictionary *userKVPair = (NSMutableDictionary *)[KeyChain load:KEY_IN_KEYCHAIN];  
    if (userKVPair == nil || [userKVPair count] < 1){
        return nil;
    }
    
    NSString *s = userKVPair[key];
    if (s) {
        return [[[NSString alloc] initWithString:s] autorelease];
    }
    
    return nil;
}  

/** 
 *  @brief        删除数据 
 *  @param  key   数据对应的key
 */  
+(void)deleteData:(NSString *)key
{  
    NSMutableDictionary *userKVPair = (NSMutableDictionary *)[KeyChain load:KEY_IN_KEYCHAIN];  
    if (!userKVPair)
    {
        userKVPair = [NSMutableDictionary dictionary];  
    }
    [userKVPair removeObjectForKey:key];
    [KeyChain save:KEY_IN_KEYCHAIN data:userKVPair];   

}
/** 
 *  @brief        删除数据 
 *  @param  key   数据对应的key
 */  
+(void)deleteAllData
{
     [KeyChain delete:KEY_IN_KEYCHAIN];  
}

@end
