//
//  KCTProtocolClass.m
//  Tcplib
//
//  Created by FredZhang on 15-4-11.
//  Copyright (c) 2016年 KCTMAC. All rights reserved.
//

#import "KCTProtocolClass.h"


@implementation KCTProtocolClass
UCSDescription
@end

//请求基类(BaseRequest)
@implementation UCSBaseRequestClass
UCSDescription
@end

//响应基类(BaseResponse)
@implementation UCSBaseResponseClass
UCSDescription
@end

//0.   登陆请求(AuthRequest)
@implementation UCSAuthRequestClass
UCSDescription
BaseRequestInit
@end

//1.   登陆响应(AuthResponse)
@implementation UCSAuthResponseClass
UCSDescription
BaseResponseInit
@end


//2.   Re-Auth请求(ReAuthRequest)
@implementation UCSReAuthRequestClass
UCSDescription
BaseRequestInit
@end
//3.   Re-Auth响应(ReauthResponse)
@implementation UCSReauthResponseClass
UCSDescription
BaseResponseInit
@end


@implementation UCSNoopRequestClass
UCSDescription
BaseRequestInit
@end

@implementation UCSNoopResponseClass
UCSDescription
BaseResponseInit
@end

//消息请求基类(MsgRequest)
@implementation UCSMsgRequestClass
UCSDescription

@end

//响应消息基类(MsgRespone)
@implementation UCSMsgResponeClass
UCSDescription

@end

//4.  点对点发送消息请求(SendMsgRequest)
@implementation UCSSendMsgRequestClass
UCSDescription
BaseRequestInit
@end

//5.  点对点发送消息应答(SendMsgRespone)
@implementation UCSSendMsgResponeClass
UCSDescription
BaseResponseInit
@end


////////
@implementation UCSCmdBufferClass
UCSDescription

@end
@implementation UCSCmdItemClass
UCSDescription

@end
@implementation UCSCmdListClass
UCSDescription

@end
////////

//////////////
//(KeyVal)
@implementation UCSKeyValClass
UCSDescription

@end
//(SyncKey)
@implementation UCSSyncKeyClass
UCSDescription

@end
//////////////

//6.    点对点消息同步请求(NewSyncRequest)
@implementation UCSNewSyncRequestClass
UCSDescription
BaseRequestInit
@end

//6.0    点对点消息同步请求(UCSInitNewSyncRequest)
@implementation UCSInitNewSyncRequestClass : NSObject
UCSDescription
BaseRequestInit
@end


@implementation UCSModContact : NSObject
UCSDescription
@end

@implementation UCSDelContact : NSObject
UCSDescription
@end

@implementation UCSAddMsgClass
UCSDescription
@end

@implementation UCSCmdRespClass
UCSDescription
@end

@implementation UCSCmdRespListClass
UCSDescription
@end
//7.    点对点消息同步应答(NewSyncRespone)
@implementation UCSNewSyncResponeClass
UCSDescription
BaseResponseInit
@end

//7.0    初始化消息同步应答(UCSInitNewSyncRespone)
@implementation UCSInitNewSyncResponeClass
UCSDescription
BaseResponseInit
@end


//8.    服务端Notify请求(NewNotify)
@implementation UCSNewNotifyClass      //客户端解析
UCSDescription

@end



//9.    同步有效Key检查(NewSyncCheck)
@implementation UCSNewSyncCheckClass
UCSDescription

@end


//10.   同步有效Key检查回应(NewSyncCheckResponse)
@implementation UCSNewSyncCheckResponseClass
UCSDescription

@end




//11.	上传语音请求(UploadVoiceRequest)
@implementation UCSUploadVoiceRequestClass
UCSDescription
BaseRequestInit
@end
//12.	上传语音回应(UploadVoiceResponse)
@implementation UCSUploadVoiceResponseClass
UCSDescription
BaseResponseInit
@end
//13.	下载语音请求(DownloadVoiceRequest)
@implementation UCSDownloadVoiceRequestClass
UCSDescription
BaseRequestInit
@end

#pragma mark 讨论组

//14.	下载语音回应(DownloadVoiceResponse)
@implementation UCSDownloadVoiceResponseClass
UCSDescription
BaseResponseInit
@end


//15.	上传消息图片请求(UploadMsgImgRequest)
@implementation UCSUploadMsgImgRequestClass
UCSDescription
BaseRequestInit
@end


//16.	上传消息图片回应(UploadMsgImgResponse)
@implementation UCSUploadMsgImgResponseClass
UCSDescription
BaseResponseInit
@end

//17.	下载消息图片请求(DownloadMsgImgRequest)
@implementation UCSDownloadMsgImgRequestClass
UCSDescription
BaseRequestInit
@end

//18.	下载消息图片回应(DownloadMsgImgResponse)
@implementation UCSDownloadMsgImgResponseClass
UCSDescription
BaseResponseInit
@end

//19.	上传视频请求(UploadVideoRequest)
@implementation UCSUploadVideoRequestClass
UCSDescription
BaseRequestInit
@end


//20.	上传视频回应(UploadVideoResponse)
@implementation UCSUploadVideoResponseClass
UCSDescription
BaseResponseInit
@end



//21.	下载视频请求(DownloadVideoRequest)
@implementation UCSDownloadVideoRequestClass
UCSDescription
BaseRequestInit
@end

//22.	下载视频回应(DownloadVideoResponse)
@implementation UCSDownloadVideoResponseClass
UCSDescription
BaseResponseInit

@end




#pragma mark 讨论组


//创建讨论组请求(CreateGroupRequest)
@implementation UCSCreateGroupRequestClass
UCSDescription
BaseRequestInit

@end

@implementation UCSGroupMemberClass
UCSDescription

@end
//创建讨论组响应（CreateGroupRespone）
@implementation UCSCreateGroupResponeClass
UCSDescription
BaseResponseInit
@end
//讨论组加人请求(AddGroupMemberRequest)
@implementation UCSAddGroupMemberRequestClass
UCSDescription
BaseRequestInit
@end
//讨论组加人响应（AddGroupMemberRespone）
@implementation UCSAddGroupMemberResponeClass
UCSDescription
BaseResponseInit
@end

//讨论组踢人请求(DelGroupMemberRequest)
@implementation UCSDelGroupMemberRequestClass
UCSDescription
BaseRequestInit
@end

//讨论组踢人响应（CreateGroupRespone）
@implementation UCSDelGroupMemberResponeClass
UCSDescription
BaseResponseInit
@end

@implementation UCSQuitChatRoomClass   //IGGQuitChatRoom_t
UCSDescription
@end

//讨论组成员退出请求(QuitGroupRequest)
@implementation UCSQuitGroupRequestClass
UCSDescription
BaseRequestInit
@end

//讨论组成员退出响应（QuitGroupRespone）
@implementation UCSQuitGroupResponeClass
UCSDescription
BaseResponseInit
@end

//修改讨论组信息请求(ModGroupTopicRequest)
@implementation UCSModGroupTopicRequestClass
UCSDescription
BaseRequestInit
@end

// 修改讨论组信息响应（ModGroupTopicRespone）
@implementation UCSModGroupTopicResponeClass
UCSDescription
BaseResponseInit
@end

// 获取讨论组信息请求（UCSGetDiscussionInfoRequest）
@implementation UCSGetDiscussionInfoRequestClass
UCSDescription
BaseRequestInit
@end

// 获取讨论组信息响应（UCSGetDiscussionInfoRespone）
@implementation UCSGetDiscussionInfoResponeClass
UCSDescription
BaseResponseInit
@end


//设置推送基类
@implementation UCSSendTokenRequestClass
UCSDescription
BaseRequestInit

@end

//设置推送响应基类
@implementation UCSSendTokenResponseClass
UCSDescription
BaseResponseInit
@end

#pragma mark 地理位置消息

//38.上传地理位置请求(SendLocationRequest)
@implementation UCSSendLocationRequestClass
UCSDescription
BaseRequestInit
@end


//39.上传地理位置响应(SendLocationResponse)
@implementation UCSSendLocationResponseClass
UCSDescription
BaseResponseInit
@end




//40.发送自定义消息请求(SendCustomMsgRequest)
@implementation UCSSendCustomMsgRequestClass
UCSDescription
BaseRequestInit
@end

//41.发送自定义消息响应(SendCustomMsgResponse)
@implementation UCSSendCustomMsgResponseClass
UCSDescription
BaseResponseInit
@end



