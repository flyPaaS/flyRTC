//
//  TcpCustomLogger.h
//  Tcplib
//
//  Created by KCT on 15/9/28.
//  Copyright © 2016年 KCTMAC. All rights reserved.
//

#import <Foundation/Foundation.h>



@interface TcpCustomLogger : NSObject

/*!
 *  @author barry, 15-09-28 11:09:31
 *
 *  @brief  文件名,不设置就使用默认文件名
 */
@property (nonatomic, copy) NSString * fileName;



/*!
 *  @author barry, 15-11-20 15:11:27
 *
 *  @brief  控制是否打印
 */
@property (nonatomic, assign) BOOL shouldLogtoTTY;



+(instancetype)sharedInstance;



/*!
 *  @author barry, 15-09-28 11:09:24
 *
 *  @brief  保存log消息到沙盒中
 *
 *  @param log <#log description#>
 */
- (void)log:(NSString *) method  detail:(NSString *)detail;

@end
