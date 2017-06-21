//
//  KCTVoipDBManager.m
//  KCT_IM_Demo
//
//  Created by macmini on 15/12/15.
//  Copyright © 2015年 KCMac. All rights reserved.
//

#import "KCTVoipDBManager.h"
#import "KCTVoipCallListModel.h"
#import "KCTCallRecordsModel.h"
#import "FMDatabase.h"
#import "FMDatabaseAdditions.h"
#import <objc/runtime.h>
#import "InfoManager.h"
#import "DDLogMacros.h"


#define VOIP_DB_NAME @"voip.sqlite3"
#define VOIP_CALL_LIST @"KCTVoipCallListModel"  // 语音通话表
#define VOIP_CALL_RECORD @"KCTCallRecordsModel" // 通话记录表

@implementation KCTVoipDBManager


+ (instancetype)shareInstance
{
    static KCTVoipDBManager * manager = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        manager = [[KCTVoipDBManager alloc] init];
    });
    
    return manager;
}

- (instancetype)init
{
    self = [super init];
    if (self) {
//        _voipQueue = [self voipQueue];
    }
    
    return self;
}

- (NSString *)pathForUserId:(NSString *)userId {
    
    if (!userId || [userId isEqualToString:@""]) {
        userId = @"0";
    }
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString *documentsDirectory = [paths objectAtIndex:0];
    NSFileManager *fileManager = [NSFileManager defaultManager];
    NSString *filePath=[documentsDirectory stringByAppendingPathComponent:userId];
    BOOL isDirectory = TRUE;
    if (![fileManager fileExistsAtPath:filePath isDirectory:&isDirectory]) {
        [fileManager createDirectoryAtPath:filePath withIntermediateDirectories:TRUE attributes:nil error:nil];
    }
    return filePath;
}

- (FMDatabaseQueue *)voipQueue
{
    if (_voipQueue == nil) {
        NSString * userId = [InfoManager sharedInfoManager].phone;
        NSString * filepath = [[self pathForUserId:userId] stringByAppendingPathComponent:VOIP_DB_NAME];
        DDLogWarn(@"fileoath is %@",filepath);
        _voipQueue = [[FMDatabaseQueue alloc] initWithPath:filepath];
        [_voipQueue inDatabase:^(FMDatabase *db) {
            // 检查表是否存在 不存在则创建
            if (![db tableExists:VOIP_CALL_LIST]) {
                NSString * callListSql1 = [NSString stringWithFormat:@"create table if not exists %@ ",VOIP_CALL_LIST];
                NSArray * propertyList = [self getPropertyList:VOIP_CALL_LIST];
                NSMutableString * callListSql2 = [NSMutableString string];
                for ( int i = 0; i < propertyList.count; i++) {
                    (i == 0)?[callListSql2 appendFormat:@"%@ text",propertyList[i]]:[callListSql2 appendFormat:@",%@ text",propertyList[i]];
                }
                NSString * callListSpl = [NSString stringWithFormat:@"%@(%@)",callListSql1,callListSql2];
                // 创建对应的表
                if (![db executeUpdate:callListSpl]) {
                    DDLogWarn(@">>>>>>>creat %@ table error ",VOIP_CALL_LIST);
                }
            }
            // 检查表是否存在 不存在则创建
            if (![db tableExists:VOIP_CALL_RECORD]) {
                NSString * callRecordsSql1 = [NSString stringWithFormat:@"create table if not exists %@ ",VOIP_CALL_RECORD];
                NSArray * propertyList = [self getPropertyList:VOIP_CALL_RECORD];
                NSMutableString * callRecordsSql2 = [NSMutableString string];
                for ( int i = 0; i < propertyList.count; i++) {
                    (i == 0)?[callRecordsSql2 appendFormat:@"%@ text",propertyList[i]]:[callRecordsSql2 appendFormat:@",%@ text",propertyList[i]];
                }
                NSString * callRecordsSql = [NSString stringWithFormat:@"%@(%@)",callRecordsSql1,callRecordsSql2];
                // 创建对应的表
                if (![db executeUpdate:callRecordsSql]) {
                    DDLogWarn(@">>>>>>>>>creat %@ table error ",VOIP_CALL_RECORD);
                }
            }
            //            [_voipQueue close];
        }];
    }
    return _voipQueue;
}

// 获取表名对应的模型的所有成员属性
- (NSMutableArray *)getPropertyList:(NSString *)tableName {
    unsigned int count;
    // Ivar：模型对应的成员变量名 成员变量类型
    Ivar * vars = class_copyIvarList(NSClassFromString(tableName), &count);
    NSMutableArray * propertyArr = [NSMutableArray array];
    for (int i = 0; i < count; i++) {
        Ivar var = vars[i];
        [propertyArr addObject:[NSString stringWithUTF8String:ivar_getName(var)]];
    }
    return propertyArr;
}
// insert into KCT_IM_CONERSATION(targetId,conversationTitle,categoryId,draft,isTop,lastTime,topTime,unReadMsgCount,extra1,extra2,extra3,lastestMessageId,localTime,extra6) values (?,?,?,?,?,?,?,?,?,?,?,?,?,?);
- (BOOL)insertDBWithModel:(id)model
{
    __block BOOL result;
    // 获取表名
    NSString * className = NSStringFromClass(object_getClass(model));
    // 查找数据库中的数据条数，如果大于100条 则删除最后一条
//    NSArray * allArray = [self getAllDataFromDb:className];
//    if (allArray.count >= 140) {
//        id m = allArray.lastObject;
//        [self deleteWithModelTime:m];
//    }
    NSArray * propertyList = [self getPropertyList:className];
    NSString * sql1 = [NSString stringWithFormat:@"insert into %@",className];
    NSMutableString * key = [NSMutableString string]; //
    NSMutableString * value = [NSMutableString string];
    NSMutableArray * argumentsArr = [NSMutableArray array]; // 保存所有参数的值的数组
    for (int i = 0; i < propertyList.count; i++) {
        (i == 0)?[key appendFormat:@"%@",propertyList[i]]:[key appendFormat:@",%@",propertyList[i]];
        (i == 0)?[value appendString:@"?"]:[value appendString:@",?"];
        // 模型里面每一个属性的值
        [argumentsArr addObject:[model valueForKey:propertyList[i]]];
    }
    NSString * sql = [NSString stringWithFormat:@"%@ (%@) values (%@)",sql1,key,value];
    DDLogWarn(@">>>>>>>>>插入数据库sql is %@",sql);
    [self.voipQueue inDatabase:^(FMDatabase *db) {
        if (![db executeUpdate:sql withArgumentsInArray:argumentsArr]) {
            result = NO;
            DDLogWarn(@">>>>>>>insert error %@",className);
        }
    }];
    [self.voipQueue close];
    return result;
}

- (BOOL)updateDBWithModel:(id)model
{
    __block BOOL result;
    NSString * className = NSStringFromClass(object_getClass(model));
    NSArray * propertyList = [self getPropertyList:className];
    NSString * sql1 = [NSString stringWithFormat:@"update %@ set ",className];
    NSMutableString * key = [NSMutableString string];
    NSMutableArray * argumentsArr = [NSMutableArray array];
    for (int i = 0; i < propertyList.count; i++) {
        (i == 0)?[key appendFormat:@"%@=?",propertyList[i]]:[key appendFormat:@",%@=?",propertyList[i]];
        [argumentsArr addObject:[model valueForKey:propertyList[i]]];
    }
    // 参数部分还缺少 where 字段N=?
    [argumentsArr addObject:[model valueForKey:propertyList[0]]];
    // update %@ set 字段1=?,字段2=?,... where 字段1=?
    NSString * sql = [NSString stringWithFormat:@"%@%@ where %@=?",sql1,key,propertyList[0]];
    DDLogWarn(@">>>>>>>>>>>更新数据库sql is %@",sql);
    [self.voipQueue inDatabase:^(FMDatabase *db) {
        if (![db executeUpdate:sql withArgumentsInArray:argumentsArr]) {
            DDLogWarn(@">>>>>>update error %@",className);
            result = NO;
        }
    }];
    [self.voipQueue close];
    return result;
}

- (NSArray *)getAllDataFromDb:(NSString *)tableName
{
    NSString * sql = [NSString stringWithFormat:@"select * from %@",tableName];
    __block NSMutableArray * allArray = [NSMutableArray array];
    __block NSMutableArray * topArray = [NSMutableArray array];
    __block NSMutableArray * disTopArray = [NSMutableArray array];
    NSArray * propertyList = [self getPropertyList:tableName];
    [self.voipQueue inDatabase:^(FMDatabase *db) {
        FMResultSet * set = [db executeQuery:sql];
        while ([set next]) {
            // 初始化一个tableName对应的模型的对象
            id obj = [[NSClassFromString(tableName) alloc] init];
            // 遍历该模型的所有属性，使用KVC进行赋值
            for (int i = 0; i < propertyList.count; i++) {
                [obj setValue:[set stringForColumn:propertyList[i]] forKey:propertyList[i]];
            }
            if ([obj isKindOfClass:[KCTVoipCallListModel class]]) {
                KCTVoipCallListModel * model = (KCTVoipCallListModel *)obj;
                if ([model.isTop isEqualToString:@"yes"]) {
                    [topArray addObject:model];
                }else{
                    [disTopArray addObject:model];
                }
            }
//            [allArray addObject:obj];
        }
        [set close];
    }];
    [self.voipQueue close];

    NSSortDescriptor *descriptor = [[NSSortDescriptor alloc]initWithKey:@"time" ascending:NO];
    NSArray *descriptoArray = [NSArray arrayWithObjects:descriptor,nil];
    NSArray *topArray1 = [topArray sortedArrayUsingDescriptors:descriptoArray];
    NSArray *disTopArray1 = [disTopArray sortedArrayUsingDescriptors:descriptoArray];
    [allArray addObjectsFromArray:topArray1];
    [allArray addObjectsFromArray:disTopArray1];
    _topCount = topArray1.count;
    if (allArray.count > 100) {
        for (int i = 100; i < allArray.count; i++) {
            id model = allArray[i];
            [self deleteModelToDB:model];
        }
        NSRange range ;
        range.location = 100;
        range.length = allArray.count - 100;
        [allArray removeObjectsInRange:range];
//        array = [allArray subarrayWithRange:range];
    }
    return allArray;
}

- (id)getDataWithDB:(NSString *)tableName UserId:(NSString *)userId
{
    NSArray * propertyList = [self getPropertyList:tableName];
    NSString * sql = [NSString stringWithFormat:@"select * from %@ where _userId = ?;",tableName];
    __block NSMutableArray * allArray = [NSMutableArray array];
    DDLogWarn(@"%@",propertyList[0]);
    [self.voipQueue inDatabase:^(FMDatabase *db) {
        FMResultSet * set = [db executeQuery:sql,userId];
        DDLogWarn(@"%@",[NSString stringWithFormat:sql,userId]);
        while ([set next]) {
            __block id obj = [[NSClassFromString(tableName) alloc] init];
            for (int i = 0; i < propertyList.count; i++) {
                [obj setValue:[set stringForColumn:propertyList[i]] forKey:propertyList[i]];
            }
            [allArray addObject:obj];
        }
        [set close];
    }];
    [self.voipQueue close];
    NSSortDescriptor *descriptor = [[NSSortDescriptor alloc]initWithKey:@"time" ascending:NO];
    NSArray *descriptoArray = [NSArray arrayWithObjects:descriptor,nil];
    NSArray *array = [allArray sortedArrayUsingDescriptors:descriptoArray];
    [allArray removeAllObjects];
    [allArray addObjectsFromArray:array];
    if (allArray.count > 100) {
        for (int i = 100; i < allArray.count; i++) {
            KCTCallRecordsModel * model = allArray[i];
            [self deleteWithModelTime:model];
        }
        NSRange range ;
        range.location = 100;
        range.length = allArray.count - 100;
        [allArray removeObjectsInRange:range];
        //        array = [allArray subarrayWithRange:range];
    }
    return allArray;
}

// 删除某个表的一条数据
- (BOOL)deleteModelToDB:(id)model{
    __block BOOL result;
    NSString * className = [NSString stringWithUTF8String:object_getClassName(model)];
    NSArray * propertyList = [self getPropertyList:className];
    NSString * sql = [NSString stringWithFormat:@"delete from %@ where %@=?",className,propertyList[0]];
    [_voipQueue inDatabase:^(FMDatabase *db) {
        
        result =[db executeUpdate:sql,[model valueForKey:propertyList[0]]];
        
    }];
    if (result) {
        [self deleteAllRecordeWothUserId:[model valueForKey:propertyList[0]]];
    }
    return result;
}

// 根据时间删除某个表的一条数据
- (BOOL)deleteWithModelTime:(id)model{
    __block BOOL result;
    NSString * className = [NSString stringWithUTF8String:object_getClassName(model)];
    NSString * sql = [NSString stringWithFormat:@"delete from %@ where _time=?",className];
    [_voipQueue inDatabase:^(FMDatabase *db) {
        
        result =[db executeUpdate:sql,[model valueForKey:@"time"]];
        
    }];
    return result;
}


// 根据userid删除所有的会话纪录
- (BOOL)deleteAllRecordeWothUserId:(NSString *)userId{
    __block BOOL result;
    NSString * sql = [NSString stringWithFormat:@"delete from KCTCallRecordsModel where _userId=?"];
    [_voipQueue inDatabase:^(FMDatabase *db) {
        
        result =[db executeUpdate:sql,userId];
        
    }];
    return result;
}


@end
