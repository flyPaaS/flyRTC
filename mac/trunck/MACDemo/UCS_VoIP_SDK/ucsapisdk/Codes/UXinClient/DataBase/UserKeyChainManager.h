//
//  WQUserDataManager.h
//  KeyChain
//
//  Created by Lenya Han on 7/10/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>


@interface UserKeyChainManager : NSObject
/** 
 *  @brief  存储数据 
 * 
 *  @param  data    密码内容 
 */  
+(void)saveData:(NSString *)data key:(NSString *)key;  
/** 
 *  @brief        读取密码 
 * 
 *  @param  key   数据对应的key
 *  @return       数据内容 
 */  
+(id)readData:(NSString *)key;  

/** 
 *  @brief        删除数据 
 *  @param  key   数据对应的key
 */  
+(void)deleteData:(NSString *)key; 

/** 
 *  @brief        删除数据 
 *  @param  key   数据对应的key
 */  
+(void)deleteAllData; 
@end
