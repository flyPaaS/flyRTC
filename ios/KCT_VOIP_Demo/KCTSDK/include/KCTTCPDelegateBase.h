//
//  KCTTCPDelegateBase.h
//  Tcplib
//
//  Created by KCT on 15/11/11.
//  Copyright © 2016年 KCTMAC. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "KCTError.h"
@class KCTTCPTransParent;



@protocol KCTTCPDelegateBase <NSObject>

#pragma mark 连接状态

/*!
 *  @brief  连接状态变化时
 *
 *  @param connectionStatus 连接状态
 *  @param error            错误描述
 */
- (void)didConnectionStatusChanged:(KCTConnectionStatus)connectionStatus  error:(KCTError *) error;


#pragma mark 数据透传

/**
 *  @brief 收到透传数据时回调
 *
 *  @param objcts 透传实体类
 */
- (void)didReceiveTransParentData:(KCTTCPTransParent *)objcts;



@end
