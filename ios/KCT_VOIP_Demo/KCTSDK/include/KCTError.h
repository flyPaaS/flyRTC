//
//  KCTError.h
//  Tcplib
//
//  Created by KCT on 15/8/27.
//  Copyright (c) 2016年 KCTMAC. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "KCTTcpDefine.h"

/*!
 @class
 @brief SDK错误信息定义类
 @discussion
 */
@interface KCTError : NSObject


/*!
 *  @brief  错误信息描述，有可能为空
 */
@property (nonatomic,copy) NSString *errorDescription;


/*!
 *  @brief  错误码
 */
@property (nonatomic,assign) KCTErrorCode code;


/*!
 *  @brief  创建一个KCTError实例对象
 *
 *  @param errorCode        错误码
 *  @param errorDescription 错误描述
 *
 *  @return KCTError实例对象
 */
+(instancetype)errorWithCode:(KCTErrorCode)errorCode
              andDescription:(NSString *)errorDescription;

@end
