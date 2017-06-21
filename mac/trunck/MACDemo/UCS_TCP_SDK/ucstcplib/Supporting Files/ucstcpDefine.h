//
//  imDefine.h
//  Tcplib
//
//  Created by FredZhang on 15-4-10.
//  Copyright (c) 2016年 KCTMAC. All rights reserved.
//

#ifndef ucstcplib_imDefine_h
#define ucstcplib_imDefine_h


#define kNotiResponse_Login_1010 @"Response1010"

#define kReLogin_Request_Notification @"reLogin_Request_Notification"
#define kNotiResponse_reLogin_1020 @"Response1020"

#define kNotiResponse_Login_1011 @"Response1011" //登录失败

#define kNotiResponse_HeartBeat_1030 @"Response1030"

#define kNotiResquest_noop_104 @"Resquest104"
#define kNotiResponse_noop_1040 @"Response1040"

#define kNotiRequest_SendMessage_203 @"203"                //向服务器请求发送消息接口
#define kNotiResponse_SendMessage_2030 @"Response2030"     //服务器响应发送消息接口

#define kNotiRequest_Message_201 @"201"
#define kNotiReceive_Message_2010 @"Response2010"          //接收消息接口

#define kNotiReceive_initMessage_2020 @"Response2020"          //接收初始化消息接口

#define kNotiReceive_initMessage_2040 @"Response2040"          //上传语音消息回应
#define kNotiReceive_initMessage_2050 @"Response2050"          //下载语音消息接口

#define kNotiReceive_initMessage_2060 @"Response2060"          //上传图片消息接口
#define kNotiReceive_initMessage_2070 @"Response2070"          //下载消息接口

#define kNotiReceive_newMsgNotify_2080 @"Response2080"          //notify

#define kNotiReceive_creatDiscussion_2090 @"Response2090"          //notify
#define kNotiReceive_addDiscussionMember_2100 @"Response2100"          //notify
#define kNotiReceive_deleteDiscussionpMember_2110 @"Response2110"          //notify
#define kNotiReceive_quitDiscussion_2120 @"Response2120"          //notify

#define kConnectionWillDisConnectNotification     @"kConnectionWillDisConnectNotification"


#define KTcpHandleFailWhenLogin    @"tcpHandleFailWhenLogin"    //登陆时，tcp握手失败

#define KTcpErrorCodeBeClicked    @"tcpErrorCodeBeClicked "    //踢线

#define kConnectWithNextIpNotification @"connectWithNextIpNotification"    //切换ip登陆

#define kReconnectFailNotification   @"reconnectFailNotification"   //重连失败


#define TCPWEAKSELF typeof(self) __weak weakSelf = self;



enum UCS_im_event
{
    eAuthRequest =	1,      //登陆请求
    eAuthResponse =	10001,//登陆响应
    
    eReAuthRequest =2,////Re-Auth请求
    eReauthResponse	=10002,//Re-Auth响应
    
    eSendMsgRequest	=3,//点对点发送消息请求(SendMsgRequest)
    eSendMsgRespone	=10003,//点对点发送消息应答(SendMsgRespone)
    
    eNewSyncRequest	=4,//点对点消息同步请求(NewSyncRequest)
    eNewSyncRespone	=10004,//点对点消息同步应答(NewSyncRespone)
    
    eNewNotify	=5,//服务端Notify请求(NewNotify)
    
    eNewSyncCheck	=6,//同步有效Key检查(NewSyncCheck)
    eNewSyncCheckRespone	=10006,//同步有效Key检查回应(NewSyncCheck)
    
    eUploadVoiceRequest	=7,//上传语音请求(UploadVoiceRequest)
    eUploadVoiceResponse	=10007,//上传语音回应(UploadVoiceRequest)
    
    eDownloadVoiceRequest		=8,//下载语音请求(DownloadVoiceRequest)
    eDownloadVoiceResponse		=10008,//下载语音回应(DownloadVoiceRequest)
    
    eUploadMsgImgRequest	=9,//上传消息图片请求
    eUploadMsgImgResponse	=10009,//上传消息图片响应
    
    eDownloadMsgImgRequest	=10,//下载消息图片请求
    eDownloadMsgImgResponse	=10010,//下载消息图片响应
    
    eUploadVideoRequest	=11,//上传视频请求
    eUploadVideoResponse	=10011,//上传视频响应
    
    eDownloadVideoRequest	=12,//下载视频请求
    eDownloadVideoResponse	=10012,//下载视频响应
    
    eCreateGroup	=13,//讨论组创建请求
    eCreateGroupResponse =10013,//讨论组创建响应
    
    eAddGroupMember =14,//讨论组加人请求
    eAddGroupMemberResponse	=10014,//讨论组加人响应
    
    eDelGroupMember	=15,//讨论组踢人请求
    eDelGroupMemberResponse	=10015,//讨论组踢人响应
    
    eQuitGroup	=16,//讨论组成员主动退出请求
    eQuitGroupResponse	=10016,//讨论组成员主动退出请求响应
    
    eModGroupTopic	=17,//修改讨论组信息请求
    eModGroupTopicResponse	=10017,//修改讨论组信息响应
    
    eSendTokenRequest	=600073,//Token发送消息请求(SendMsgRequest)
    eSendTokenResponse	=10600073//Token发送消息应答(SendMsgRespone)

};




#endif
