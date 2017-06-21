//
//  KCTTCPCommonClass.h
//  Tcplib
//
//  Created by KCT on 16/4/11.
//  Copyright © 2017年 KCTMAC. All rights reserved.
//

#import <Foundation/Foundation.h>



/**
 *  @brief 透传请求类
 */
@interface KCTTCPTransParentRequest : NSObject

/**
 *  @brief 请求标识，用于区分请求
 */
@property (nonatomic,copy) NSString *requestIdentifier;

/*!
 *  @brief  接收方的userId
 */
@property (nonatomic,copy) NSString *receiveId;


/*!
 *  @brief  透传命令，最长256个字节
 */
@property (nonatomic,copy) NSString *cmdString;


/*!
 *  @brief 实例化一个透传请求类
 *
 *  @param cmd   透传的命令
 *  @param receiveId 接收方的userId
 *
 *  @return 一个透传请求类
 */
+(instancetype)initWithCmdString:(NSString*)cmdString
                  receiveId:(NSString *)receiveId;
@end




/**
 *  @brief 透传数据类
 */
@interface KCTTCPTransParent : NSObject

/**
 *  @brief 发送者赋予的透传标识
 */
@property (nonatomic, copy) NSString *requestIdentifier;

/*!
 *  @brief  发送方的userId
 */
@property (nonatomic,copy) NSString *senderUserId;


/*!
 *  @brief  透传命令，最长256个字节
 */
@property (nonatomic,copy) NSString *cmdString;

@end
