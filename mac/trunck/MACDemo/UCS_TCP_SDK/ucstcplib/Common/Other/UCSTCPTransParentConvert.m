//
//  UCSTCPTransParentConvert.m
//  Tcplib
//
//  Created by KCT on 16/4/13.
//  Copyright © 2017年 KCTMAC. All rights reserved.
//

#import "UCSTCPTransParentConvert.h"
#import "TcpCustomLogger.h"


NSString * const UCSTcpSendTransParentDataResponseNotification = @"UCSTcpSendTransParentDataResponseNotification";
NSString * const UCSTcpDidReceiveTransParentDataNotification = @"UCSTcpDidReceiveTransParentDataNotification";

@implementation UCSTCPTransParentSubRequest

@end

@implementation UCSTCPTransParentResponse

@end

@implementation UCSTCPTransParentRequestBlockObj

@end



@implementation UCSTCPTransParentConvert

+(NSData *)convertTransParentRequest2Data:(UCSTCPTransParentSubRequest *)request{
    NSMutableData *mutableData = [[NSMutableData alloc] init];
    
    //1、包体结构
    NSDictionary *dic = @{@"msgid": request.requestIdentifier,
                          @"appid": request.appid,
                          @"fuserid": request.senderId,
                          @"tuserid": request.receiveId,
                          @"data": request.cmdString};
    NSData *jsonData = [NSJSONSerialization dataWithJSONObject:dic options:NSJSONWritingPrettyPrinted error:nil];
//    NSString *jsonString = [[NSString alloc] initWithData:jsonData encoding:NSUTF8StringEncoding];
    
    
    //2、业务包头
    TransParentFrameBodyHead frameBodyHead ;
    memset(&frameBodyHead, 0, sizeof(frameBodyHead));
    frameBodyHead.bodyHeadLength = sizeof(frameBodyHead);
    frameBodyHead.op = 1;
    frameBodyHead.bodyHeadLength = htons(frameBodyHead.bodyHeadLength);
    NSData *bodyHeadData = [NSData dataWithBytes:&frameBodyHead length:sizeof(frameBodyHead)];
    
    
    //3、数据包头
    TransParentFrameHead frameHead ;
    memset(&frameHead, 0, sizeof(frameHead));
    frameHead.packageLength = (UInt32)jsonData.length + sizeof(frameBodyHead) + sizeof(frameHead);
    frameHead.headerLength = sizeof(frameHead);
    frameHead.version = 1;
    frameHead.cmd = 4000;
    frameHead.seq = [UCSTCPTransParentConvert seq];
    frameHead.clientId = 0;
    frameHead.serverId = 0;
    
    frameHead.packageLength = htonl(frameHead.packageLength);
    frameHead.headerLength = htons(frameHead.headerLength);
    frameHead.version = htons(frameHead.version);
    frameHead.cmd = htonl(frameHead.cmd);
    frameHead.seq = htonl(frameHead.seq);
    frameHead.clientId = htonl(frameHead.clientId);
    frameHead.serverId = htonl(frameHead.serverId);
    NSData *headData = [NSData dataWithBytes:&frameHead length:sizeof(frameHead)];
    
    [mutableData appendData:headData];
    [mutableData appendData:bodyHeadData];
    [mutableData appendData:jsonData];
    
    return [mutableData copy];
}


+ (NSData *)convertTransParent2Data:(KCTTCPTransParent *)parent{
    NSMutableData *mutableData = [[NSMutableData alloc] init];
    
    NSUInteger serverID = [(NSNumber *)[parent valueForKey:@"serverId"] unsignedIntegerValue];
    
    //1、包体结构
    NSDictionary *dic = @{@"msgid": parent.requestIdentifier,
                          @"errcode": @"0"};
    NSData *jsonData = [NSJSONSerialization dataWithJSONObject:dic options:NSJSONWritingPrettyPrinted error:nil];
    
    //2、业务包头
    TransParentFrameBodyHead frameBodyHead ;
    memset(&frameBodyHead, 0, sizeof(frameBodyHead));
    frameBodyHead.bodyHeadLength = sizeof(frameBodyHead);
    frameBodyHead.op = 2;
    frameBodyHead.bodyHeadLength = htons(frameBodyHead.bodyHeadLength);
    NSData *bodyHeadData = [NSData dataWithBytes:&frameBodyHead length:sizeof(frameBodyHead)];
    
    
    //3、数据包头
    TransParentFrameHead frameHead ;
    memset(&frameHead, 0, sizeof(frameHead));
    frameHead.packageLength = (UInt32)jsonData.length + sizeof(frameBodyHead) + sizeof(frameHead);
    frameHead.headerLength = sizeof(frameHead);
    frameHead.version = 1;
    frameHead.cmd = 4000;
    frameHead.seq = [UCSTCPTransParentConvert seq];
    frameHead.clientId = 0;
    frameHead.serverId = serverID;
    
    frameHead.packageLength = htonl(frameHead.packageLength);
    frameHead.headerLength = htons(frameHead.headerLength);
    frameHead.version = htons(frameHead.version);
    frameHead.cmd = htonl(frameHead.cmd);
    frameHead.seq = htonl(frameHead.seq);
    frameHead.clientId = htonl(frameHead.clientId);
    frameHead.serverId = htonl(frameHead.serverId);
    NSData *headData = [NSData dataWithBytes:&frameHead length:sizeof(frameHead)];
    
    [mutableData appendData:headData];
    [mutableData appendData:bodyHeadData];
    [mutableData appendData:jsonData];
    
    return [mutableData copy];
}


+ (void)handleTransReponse:(NSData *)reData{
    
    //1、解析整个包头
    TransParentFrameHead frameHead ;
    memset(&frameHead, 0, sizeof(frameHead));
    [reData getBytes:&frameHead range:NSMakeRange(0, sizeof(frameHead))];
    frameHead.packageLength = ntohl(frameHead.packageLength);
    frameHead.headerLength = ntohs(frameHead.headerLength);
    frameHead.version = ntohs(frameHead.version);
    frameHead.cmd = ntohl(frameHead.cmd);
    frameHead.seq = ntohl(frameHead.seq);
    frameHead.clientId = ntohl(frameHead.clientId);
    frameHead.serverId = ntohl(frameHead.serverId);
    
    if (reData.length != frameHead.packageLength) {
        [[TcpCustomLogger sharedInstance] log:[NSString stringWithFormat:@"%@", NSStringFromSelector(_cmd)] detail:@"透传时，实际包长度和packageLength长度不一致，视为错误包，不解析"];
        return;
    }
    
    //2、解析业务包头
    TransParentFrameBodyHead bodyHead;
    memset(&bodyHead, 0, sizeof(bodyHead));
    [reData getBytes:&bodyHead range:NSMakeRange(sizeof(frameHead), sizeof(bodyHead))];
    bodyHead.bodyHeadLength = ntohs(bodyHead.bodyHeadLength);
    
    //3、解析包体数据
    NSUInteger loc = sizeof(frameHead) + sizeof(bodyHead);
    NSUInteger len = frameHead.packageLength - frameHead.headerLength - bodyHead.bodyHeadLength;
    NSData *bodyData = [reData subdataWithRange:NSMakeRange(loc ,len)];
    
    NSError *err;
    NSDictionary *dic = [NSJSONSerialization JSONObjectWithData:bodyData
                                                        options:NSJSONReadingMutableContainers
                                                          error:&err];
    if(err) {
        [[TcpCustomLogger sharedInstance] log:[NSString stringWithFormat:@"%@", NSStringFromSelector(_cmd)] detail:@"透传时，解析包体失败"];
        return;
    }
    
    if (bodyHead.op == 0x1) {
        //请求
        KCTTCPTransParent *transParent = [[KCTTCPTransParent alloc] init];
        transParent.requestIdentifier = dic[@"msgid"];
        transParent.senderUserId = dic[@"fuserid"];
        transParent.cmdString = dic[@"data"];
        [transParent setValue:[NSNumber numberWithUnsignedInteger:frameHead.serverId] forKey:@"serverId"];
        
        //广播收到透传请求
        [[NSNotificationCenter defaultCenter] postNotificationName:UCSTcpDidReceiveTransParentDataNotification object:transParent];
        
    }else if (bodyHead.op == 0x2){
        //响应
        NSString *msgid = dic[@"msgid"];
        NSString *errorCode = dic[@"errcode"];
        KCTError *error;
        if (errorCode) {
            int code = errorCode.intValue;
            switch (code) {
                case 0:
                    /**
                     @author WLS, 16-04-20 17:04:01
                     
                     没有错误，发送成功
                     */
                    break;
                case 1:
                    error = [KCTError errorWithCode:ErrorCode_OppositeSideIsNotOnline andDescription:@"对方不在线"];
                    break;
                case 2:
                    error = [KCTError errorWithCode:ErrorCode_Unreachable andDescription:@"数据不可达"];
                    break;
                case 3:
                    error = [KCTError errorWithCode:ErrorCode_TIMEOUT andDescription:@"超时"];
                    break;
                case 4:
                    error = [KCTError errorWithCode:ErrorCode_NonSupportVersion andDescription:@"对方版本不支持"];
                    break;
                case 5:
                    error = [KCTError errorWithCode:ErrorCode_LastTimeNoResponse andDescription:@"上一次透传请求未响应"];
                    break;
                case 6:
                    error = [KCTError errorWithCode:ErrorCode_TCPDisconnect andDescription:@"TCP连接已经断开"];
                    break;
                case 7:
                    error = [KCTError errorWithCode:ErrorCode_InvalidData andDescription:@"发送数据为空"];
                    break;
                case 8:
                    error = [KCTError errorWithCode:ErrorCode_EncryptingFail andDescription:@"加密失败"];
                    break;
                case 9:
                    error = [KCTError errorWithCode:ErrorCode_EmptyMessage andDescription:@"透传数据为空"];
                    break;
                case 10:
                    error = [KCTError errorWithCode:ErrorCode_MessageLengthTooLong andDescription:@"透传数据内容过长"];
                    break;
                case 11:
                    error = [KCTError errorWithCode:ErrorCode_InvalidUser andDescription:@"用户不存在"];
                    break;
                default:
                    error = [KCTError errorWithCode:ErrorCode_UnKnown andDescription:[NSString stringWithFormat:@"未知错误%d",code]];
                    break;
            }
        }
        
        
        UCSTCPTransParentResponse *reponse = [[UCSTCPTransParentResponse alloc] init];
        reponse.error = error;
        reponse.requestIdentifier = msgid;
        
        //广播透传响应
        [[NSNotificationCenter defaultCenter] postNotificationName:UCSTcpSendTransParentDataResponseNotification object:reponse];
    }
    
}

+ (KCTTCPTransParent *)convertData2TransParent:(NSData *)data{
    KCTTCPTransParent *transParent = [[KCTTCPTransParent alloc] init];
    
    
    
    return transParent;
}


+ (UInt32)seq{
    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
    NSNumber *seq = [defaults objectForKey:@"TransParentFrameHeadSeq"];
    if (seq == nil) {
        seq = @0;
    }
    
    NSInteger seqValue = seq.integerValue;
    NSInteger newSeqValue = seqValue + 1;
    NSNumber *newSeq = [NSNumber numberWithInteger:newSeqValue];
    [defaults setObject:newSeq forKey:@"TransParentFrameHeadSeq"];
    [defaults synchronize];
    
    return (UInt32)seqValue;
}


+ (NSString *)requestIdentifier{
    
    NSDate* dat = [NSDate dateWithTimeIntervalSinceNow:0];
    NSTimeInterval timeIntervalSince1970 =[dat timeIntervalSince1970] * 1000;
    NSString *timeString = [NSString stringWithFormat:@"%f", timeIntervalSince1970];
    long long msgIdTime = [timeString longLongValue];
    
    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
    NSString *currentName = [NSString stringWithFormat:@"%@_currentMsgSeq",[defaults stringForKey:@"loginUserId"]];
    NSString *currentSeq = [defaults stringForKey:currentName];
    NSString *msgID = [NSString stringWithFormat:@"%lld", msgIdTime+[currentSeq intValue]];
    
    return msgID;
}


+ (NSString *)senderUserId{
    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
    return [defaults stringForKey:@"loginUserId"];
}

@end
