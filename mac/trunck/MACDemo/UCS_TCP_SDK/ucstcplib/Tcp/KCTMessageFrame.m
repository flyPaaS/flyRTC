//
//  KCTMessageFrame.m
//  Tcplib
//
//  Created by KCT   on 15/10/28.
//  Copyright © 2016年 KCTMAC. All rights reserved.
//

#import "KCTMessageFrame.h"
#import "UCSTCPManager.h"

@interface KCTMessageFrame ()


@property IMTCPFrameHead message_head; //消息头


@end

@implementation KCTMessageFrame


- (void)dealloc{
    
    self.message_respone = nil;
    
}

+ (id)decode:(NSData *)data{
    
    IMTCPFrameHead head;
    memset(&head, 0, sizeof(head)); //结构体清零
    
    [data getBytes:(void *)&head length:sizeof(head)];  ///从 _recvData中拷贝sizeof(head)个字节到&head中
    
    head.packageLength = ntohl(head.packageLength);   //ntohl()是将一个无符号长整形数从网络字节顺序转换为主机字节顺序。
    head.headerLength = ntohs(head.headerLength);
    head.version = ntohs(head.version);
    head.cmd = ntohl(head.cmd);
    head.seq = ntohl(head.seq);
    
    KCTMessageFrame * frame = [[KCTMessageFrame alloc] init];
    frame.message_head = head;
    

    //反序列化消息体。后面添加
    frame.message = data;
    
    frame.seq = head.seq;
    
    return frame;
    
}

@end
