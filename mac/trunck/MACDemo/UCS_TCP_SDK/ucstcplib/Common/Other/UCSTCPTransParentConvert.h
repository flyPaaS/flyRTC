//
//  UCSTCPTransParentConvert.h
//  Tcplib
//
//  Created by KCT on 16/4/13.
//  Copyright © 2017年 KCTMAC. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "KCTTCPCommonClass.h"
#import "KCTError.h"

#pragma pack(1)
typedef struct _TransParentFrameHead
{
    UInt32 packageLength;  //数据包长度 4字节
    UInt16 headerLength; //包头  2字节
    UInt16 version;   //版本号协议  2字节
    UInt32 cmd;   //命令号   4字节
    UInt32 seq;   //序列号  4字节
    UInt32 clientId;   //客户端id  4字节
    UInt32 serverId;  //业务服务器id 4字节
} TransParentFrameHead;  //24字节 透传包头


typedef struct _TransParentFrameBodyHead{
    UInt16 bodyHeadLength;  // 业务包头的长度 2字节
    UInt8 op ;  //操作码 1字节  1、透传请求 2、透传响应
}TransParentFrameBodyHead; //3字节 透传业务包头
#pragma pack()


extern NSString * const UCSTcpSendTransParentDataResponseNotification; //发送透传数据的回执通知
extern NSString * const UCSTcpDidReceiveTransParentDataNotification; //收到透传数据


/**
 *  @brief 透传请求类
 */
@interface UCSTCPTransParentSubRequest : KCTTCPTransParentRequest

@property (nonatomic, copy) NSString *senderId;
@property (nonatomic, copy) NSString *appid;

@end


/**
 *  @brief
 */
@interface UCSTCPTransParentResponse : NSObject

@property (nonatomic, copy) NSString *requestIdentifier;
@property (nonatomic, strong) KCTError *error;

@end



typedef void(^TransParentRequestSuccess)(KCTTCPTransParentRequest *request);
typedef void(^TransParentRequestFailure)(KCTTCPTransParentRequest *request, KCTError *error);
@interface UCSTCPTransParentRequestBlockObj : NSObject

@property (nonatomic, strong) KCTTCPTransParentRequest * request;

@property (nonatomic, copy) TransParentRequestSuccess success;

@property (nonatomic, copy) TransParentRequestFailure failure;

@property (nonatomic)time_t time;

@end


@interface UCSTCPTransParentConvert : NSObject

+ (NSData *)convertTransParentRequest2Data:(KCTTCPTransParentRequest *)request;

+ (NSData *)convertTransParent2Data:(KCTTCPTransParent *)parent;

+ (KCTTCPTransParent *)convertData2TransParent:(NSData *)data;

/**
 *  @brief 处理tcp返回的透传数据
 */
+ (void)handleTransReponse:(NSData *)reData;

+ (NSString*)requestIdentifier;

+ (NSString*)senderUserId;
@end
