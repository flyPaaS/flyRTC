//
//  KCTMessageFrame.h
//  Tcplib
//
//  Created by KCT   on 15/10/28.
//  Copyright © 2016年 KCTMAC. All rights reserved.
//

#import <Foundation/Foundation.h>

#import "KCTProtocolClass.h"

@interface KCTMessageFrame : NSObject


// 消息回调。针对某个消息，可以指定具体回调处理对象
@property (nonatomic, assign) id delegate;



@property (nonatomic,strong)UCSSendMsgResponeClass * message_respone; //消息体

@property (nonatomic,assign)int seq; //序列号

@property (nonatomic,strong)NSData * message;
+ (id)decode:(NSData *)data;

@end
