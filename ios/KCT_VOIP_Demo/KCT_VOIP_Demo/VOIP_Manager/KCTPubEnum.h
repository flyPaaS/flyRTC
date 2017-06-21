//
//  KCTPubEnum.h

//
//  Created by wenqinglin on 14-5-28.
//  Copyright (c) 2014年 KCT. All rights reserved.
//

typedef enum
{
    KCTCallStatus_NO=0,               //没有呼叫
    KCTCallStatus_Calling,            //呼叫中
    KCTCallStatus_Proceeding,         //服务器有回应
    KCTCallStatus_Alerting,           //对方振铃
    KCTCallStatus_Answered,           //对方应答
    KCTCallStatus_Pasused,            //保持成功
    KCTCallStatus_Released,           //通话释放
    KCTCallStatus_Failed,             //呼叫失败
    KCTCallStatus_Incoming,           //来电
    KCTCallStatus_Transfered,         //
    KCTCallStatus_CallBack,           //
    KCTCallStatus_CallBackFailed,      //
    KCTCallStatus_RecDTMFvalue,      //接收到DTMF
    
    KCTCallStatus_Conference_StateNotify,  //电话会议
    KCTCallStatus_Conference_PassiveModeConvert,
    KCTCallStatus_Conference_ActiveModeConvert
}KCTCallStatus;



typedef enum
{
    KCTMsgStatus_Received,        //接受消息
    KCTMsgStatus_Send,            //发送消息成功
    KCTMsgStatus_Sending,         //发送消息中
    KCTMsgStatus_SendFailed       //发送消息失败
}KCTMsgStatusResult;


typedef enum{
    UMsgeState_Sending = 0,
    UMsgeState_SendSuccess,
    UMsgeState_SendFailed,
    UMsgeState_Send_OtherReceived,
    UMsgeState_Received
}KCTMsgState;

typedef enum{
    KCTMsg_Unread = 0,
    KCTMsg_IsRead
} KCTMsgReadState;
