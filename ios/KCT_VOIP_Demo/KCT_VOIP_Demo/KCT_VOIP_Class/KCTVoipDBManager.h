//
//  KCTVoipDBManager.h
//  KCT_IM_Demo
//
//  Created by macmini on 15/12/15.
//  Copyright © 2015年 KCMac. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "FMDatabaseQueue.h"

@interface KCTVoipDBManager : NSObject
@property (strong,nonatomic)FMDatabaseQueue * voipQueue;
@property (assign,nonatomic)NSInteger topCount;
+ (instancetype)shareInstance;

// 插入数据
- (BOOL)insertDBWithModel:(id)model;

// 删除数据  删除某个模型对应的数据
- (BOOL)deleteModelToDB:(id)model;

// 根据userid删除所有的会话纪录
- (BOOL)deleteAllRecordeWothUserId:(NSString *)userId;

// 更新数据
- (BOOL)updateDBWithModel:(id)model;

// 查找通话列表所有数据
- (NSArray *)getAllDataFromDb:(NSString *)tableName;

// 查找某张表中userId对应的通话详情
- (id)getDataWithDB:(NSString *)tableName UserId:(NSString *)userId;

@end
