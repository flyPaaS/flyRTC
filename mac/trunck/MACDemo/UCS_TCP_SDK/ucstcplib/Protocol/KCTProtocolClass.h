//
//  KCTProtocolClass.h
//  Tcplib
//
//  Created by FredZhang on 15-4-11.
//  Copyright (c) 2016年 KCTMAC. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <objc/runtime.h>

#define UCSDescription  \
- (NSString *)description\
{\
NSString *result = @"";\
NSArray *proNames = [self getAllProperties];\
for (int i = 0; i < proNames.count; i++) {\
NSString *proName = [proNames objectAtIndex:i];\
id  proValue = [self valueForKey:proName];\
if ([proValue isKindOfClass:[NSData class]]) {\
NSData *data = (NSData *)proValue;\
NSUInteger length = data.length;\
proValue = [NSString stringWithFormat:@"NSData类型数据 长度%ld",(unsigned long)length];\
}\
result = [result stringByAppendingFormat:@"%@:%@\n",proName,proValue];\
}\
return result;\
}\
\
- (NSArray *)getAllProperties\
{\
u_int count;\
objc_property_t *properties  =class_copyPropertyList([self class], &count);\
NSMutableArray *propertiesArray = [NSMutableArray arrayWithCapacity:count];\
for (int i = 0; i<count; i++)\
{\
const char* propertyName =property_getName(properties[i]);\
[propertiesArray addObject: [NSString stringWithUTF8String: propertyName]];\
}\
free(properties);\
return propertiesArray;\
}



#define BaseRequestInit \
- (instancetype)init{\
self = [super init];\
if (self) {\
_tBaseRequest = [[UCSBaseRequestClass alloc] init];\
}\
return self;\
}


#define BaseResponseInit  \
- (instancetype)init{\
self = [super init];\
if (self) {\
_tBaseResponse = [[UCSBaseResponseClass alloc] init];\
}\
return self;\
}


@interface KCTProtocolClass : NSObject

@end

//请求基类(BaseRequest)
@interface UCSBaseRequestClass : NSObject
@property (nonatomic, retain) NSString *sSessionKey;
@property (nonatomic, retain) NSString *iUin;
@property (nonatomic, retain) NSString *cDeviceID;
@property (nonatomic, retain) NSString *iClientVersion;  //业务+网络状态 按二进制去算
@property (nonatomic, retain) NSString *sDeviceType;
@property (nonatomic, retain) NSString *iScene;
@property (nonatomic, retain) NSString *iSeq;
@end

//响应基类(BaseResponse)
@interface UCSBaseResponseClass : NSObject
@property (nonatomic, assign) int iRet;
@property (nonatomic, retain) NSString *tErrMsg;
@property (nonatomic, assign) int iSeq;  //iseq主要用来校验请求和相应的一致性，同一对请求和相应的iseq应该是一致的，iseq由sdk端发送到服务器，服务器在返回
@end

//0.   登陆请求(AuthRequest)
@interface UCSAuthRequestClass : NSObject
@property (nonatomic, retain) UCSBaseRequestClass *tBaseRequest;
@property (nonatomic, retain) NSString *tAccount;   //登陆账号 用户名目前支持 绑定邮箱/绑定手机
@property (nonatomic, retain) NSString *tPwd;      //登陆密码（需要MD5），如果是第三方用户则填写TOKEN（不需要MD5）
@property (nonatomic, retain) NSString *pcTimeZone; //时区
@property (nonatomic, retain) NSString *pcLanguage;  //语言
@property (nonatomic, retain) NSString *pcAuthTicket;  //暂时没用了
@property (nonatomic, retain) NSString *pcRealCountry;  //登陆时所在的国家
@property (nonatomic, retain) NSString *tRandomEncryKey; //客户端产生的随机加密密钥
@property (nonatomic, retain) NSString *tDevSid;
@property (nonatomic, retain) NSString *tDevPwd;
@property (nonatomic, retain) NSString *tLoginToken;
@end

//1.   登陆响应(AuthResponse)
@interface UCSAuthResponseClass : NSObject
@property (nonatomic, retain) UCSBaseResponseClass *tBaseResponse;
@property (nonatomic, assign) int iUin; //用户的UIN
@property (nonatomic, retain) NSString *tUserName; //用户的UserName 支持用户修改一次
@property (nonatomic, retain) NSString *tBindEmail; //用户绑定的邮箱
@property (nonatomic, retain) NSString *tBindMobile; //用户绑定的手机
@property (nonatomic, assign) int iStatus; //参照enIGGStatus
@property (nonatomic, retain) NSString *sSessionKey; //服务端产生的SessionKey
@property (nonatomic, retain) NSString *tImgBuf;  //用户的小头像信息
@property (nonatomic, assign)  int iNewVersion;  //新版本信息，0表示当前客户端已经是最新
@property (nonatomic, assign) int iRegType;  //用户账号的类型，参照enIGGRegType
@property (nonatomic, assign) int iProfileFlag;   //默认填1
@property (nonatomic, assign) int iUserSex;  //用户性别
@property (nonatomic, assign) int iUserStatus;  //用户状态，参照enUserStatus
@property (nonatomic, retain) NSString *tFirstName;     //first name
@property (nonatomic, retain) NSString *tLastName;     //lastname
@property (nonatomic, retain) NSString *tBirthday;      //用户出生日期
@property (nonatomic, retain) NSString *tWebProxyInfo;  //短连接服务器的IP信息
@property (nonatomic, retain) NSString *tSafeUserName;  //用户的safeusername
@property (nonatomic, retain) NSString *tClientNumber;  //用户的client
@property (nonatomic, retain) NSString *tAppid;  //aaptd

@end


//2.   Re-Auth请求(ReAuthRequest)
@interface UCSReAuthRequestClass : NSObject
@property (nonatomic, retain) UCSBaseRequestClass *tBaseRequest;
@end
//3.   Re-Auth响应(ReauthResponse)
@interface UCSReauthResponseClass : NSObject
@property (nonatomic, retain) UCSBaseResponseClass *tBaseResponse;
@property (nonatomic, assign) int Uin;   //用户UIN
@property (nonatomic, retain) NSString *iNewVersion;    //新版本号，0表示客户端的版本已经最新
@property (nonatomic, retain) NSString *tWebProxyInfo;  //短连接服务器的IP信息
@end


@interface UCSNoopRequestClass : NSObject
@property (nonatomic, retain) UCSBaseRequestClass *tBaseRequest;
@end

@interface UCSNoopResponseClass : NSObject
@property (nonatomic, retain) UCSBaseResponseClass *tBaseResponse;
@property (nonatomic, assign) int Uin;   //用户UIN
@property (nonatomic, retain) NSString *iNewVersion;    //新版本号，0表示客户端的版本已经最新
@property (nonatomic, retain) NSString *tWebProxyInfo;  //短连接服务器的IP信息
@end


#pragma mark 点对点发送消息
//消息请求基类(MsgRequest)
@interface UCSMsgRequestClass : NSObject
@property (nonatomic, retain) NSString *tFromUserName;  //客户端当前登录的用户名字
@property (nonatomic, retain) NSString *tToUserName; //此条消息发送的目标用户名字
@property (nonatomic, assign) int iType; //消息类型，详见mmsyncdef.h
@property (nonatomic, retain) NSString *pcContent;  //表情数据，客户端根据用户输入获取
@property (nonatomic, assign) int iCreateTime; //创建此条消息的时间戳
@property (nonatomic, retain) NSString *pcClientMsgId; //消息唯一标识,暂用(mmtestproxy_fuin_tuin_时间戳(精确到毫秒,以免发送同一秒内多条消息标识一样的情况))
@property (nonatomic, retain) NSString *pcMsgSource; //消息内容，用户输入的数据
@property (nonatomic, assign) int iEmojiFlag; //是否是自定义表情，客户端判断
@end

//响应消息基类(MsgRespone)
@interface UCSMsgResponeClass : NSObject
@property (nonatomic, assign) int iRet; //消息处理结果 0为成功
@property (nonatomic, retain) NSString *tFromUserName; //发送方名字
@property (nonatomic, retain) NSString *tToUserName;//接收方名字
@property (nonatomic, assign) int iMsgId;//此消息的Index编号
@property (nonatomic, retain) NSString *pcClientMsgId;//消息唯一标识、消息id
@property (nonatomic, assign) int iCreateTime;//创建时间
@property (nonatomic, assign) int iType;  //消息类型
@end


//4.  点对点发送消息请求(SendMsgRequest)
@interface UCSSendMsgRequestClass : NSObject
@property (nonatomic, retain) UCSBaseRequestClass *tBaseRequest;
@property (nonatomic, retain) NSString *iCount; //客户端统计的此次发送的消息个数
@property (nonatomic, retain) NSArray *MsgRequestArr;  //UCSMsgRequestClass对象
@end
//5.  点对点发送消息应答(SendMsgRespone)
@interface UCSSendMsgResponeClass : NSObject
@property (nonatomic, retain) UCSBaseResponseClass *tBaseResponse;
@property (nonatomic, retain) NSString *iCount;
@property (nonatomic, retain) NSArray *MsgResponeArr;  //UCSMsgResponeClass对象
@end


////////
@interface UCSCmdBufferClass : NSObject
@property (nonatomic, retain) NSString *iLen;
@property (nonatomic, retain) NSString *pcBuff;
@end
@interface UCSCmdItemClass : NSObject
@property (nonatomic, retain) NSString *iCmdId;
@property (nonatomic, retain) UCSCmdBufferClass *tCmdBuf;
@end
@interface UCSCmdListClass : NSObject
@property (nonatomic, retain) NSString *iCount;
@property (nonatomic, retain) NSArray *ptList; //UCSCmdItemClass对象
@end
////////

//////////////
//(KeyVal)
@interface UCSKeyValClass : NSObject
@property (nonatomic, assign) int iKey;
@property (nonatomic, assign) int iVal;
@end
//(SyncKey)
@interface UCSSyncKeyClass : NSObject
@property (nonatomic, assign) int iKeyCount;
@property (nonatomic, retain) NSArray *ptKey;  //UCSKeyValClass数组，具体消息结构体
@end
//////////////


//6.    点对点消息同步请求(NewSyncRequest)
@interface UCSNewSyncRequestClass : NSObject
@property (nonatomic, retain) UCSBaseRequestClass *tBaseRequest;
@property (nonatomic, retain) UCSCmdListClass *tOplog;
@property (nonatomic, retain) NSString *iSelector; //选择同步的内容
@property (nonatomic, retain) UCSSyncKeyClass *tKeyBuf; //特殊字段，详见本部分特殊字段说明,需要先封包
@property (nonatomic, retain) NSString *iScene; //同步的场景，定义于mmsyncdef.h
@end
//6.0    初始化消息同步请求(UCSInitNewSyncReques)
@interface UCSInitNewSyncRequestClass : NSObject
@property (nonatomic, retain) UCSBaseRequestClass *tBaseRequest;
@property (nonatomic, retain) UCSCmdListClass *tOplog;
@property (nonatomic, retain) NSString *iSelector; //选择同步的内容
@property (nonatomic, retain) UCSSyncKeyClass *tCurrentKeyBuf; //特殊字段，详见本部分特殊字段说明,需要先封包。
@property (nonatomic, retain) UCSSyncKeyClass *tMaxKeyBuf;
@property (nonatomic, retain) NSString *iScene; //同步的场景，定义于mmsyncdef.h
@end

//暂时是讨论组实体
@interface UCSModContact : NSObject
@property (nonatomic, retain) NSString * tUserName;		//讨论组id
@property (nonatomic, retain) NSString *  tNickName;		//讨论组名
@property (nonatomic, assign) unsigned int iContactType;				//
@property (nonatomic, assign) unsigned int iSource;					//
@property (nonatomic, retain) NSString * pcSignature;						//
@property (nonatomic, assign) unsigned int iRoomMemberCount;			//人数
@property (nonatomic, retain) NSArray * ptRoomMemberList;	// UCSGroupMemberClass，成员列表
@property (nonatomic, retain) NSString * pcChatRoomOwner;					//创建者
@property (nonatomic, assign) unsigned int iMaxMemberCount; 			//讨论组最大人数
@property (nonatomic, assign) int iType;								//
@end

@interface UCSDelContact : NSObject
@property (nonatomic, retain) NSString * tUserName;
@end


@interface UCSAddMsgClass : NSObject
@property (nonatomic, assign) int  iMsgId;
@property (nonatomic, retain) NSString * tFromUserName; //单聊 userid   群组：id@chatroom 讨论组:id@group
@property (nonatomic, retain) NSString *  tToUserName;  //自己的id
@property (nonatomic, assign) int  iMsgType;    //消息类型，文本，图片等
@property (nonatomic, retain) NSString *tContent;  //发送的内容
@property (nonatomic, assign) int  iStatus;
@property (nonatomic, assign) int  iImgStatus;
@property (nonatomic, strong) NSData *tImgBuf;   //图片的字节buf
@property (nonatomic, assign) int  iCreateTime;    //这条消息被发送者发出的时间
@property (nonatomic, retain) NSString *pcMsgSource;  // 发送者 userid+昵称
@property (nonatomic, retain) NSString *pcPushContent;
@end

@interface UCSCmdRespClass : NSObject
@property (nonatomic, retain) NSString *iCmdId;
@property (nonatomic, retain) NSString *iRet;
@end
@interface UCSCmdRespListClass : NSObject
@property (nonatomic, retain) NSString *iCount;
@property (nonatomic, retain) NSArray *ptList; //多个UCSCmdRespClass对象
@end
//7.    点对点消息同步应答(NewSyncRespone)
@interface UCSNewSyncResponeClass : NSObject
@property (nonatomic, retain) UCSBaseResponseClass *tBaseResponse;
@property (nonatomic, retain) NSString *iRet; //同步结果标记
@property (nonatomic, retain) NSArray *tMsgList;//UCSAddMsgClass数组，回复的数据内容，详见客户端字段来源于使用
@property (nonatomic, retain) NSArray *tContactList; //UCSModContact 数组
//@property (nonatomic, retain) NSString *tdeleteString;
@property (nonatomic, strong) NSArray * tDeleteList; //退出某个讨论组的时候回调，不管是主动还是被动。每一个元素是讨论组id
@property (nonatomic, retain) NSString *tModifyString;
@property (nonatomic, retain) NSString *iContinueFlag; //还需要同步的iSelector,0为不需要再NewSync
@property (nonatomic, retain) UCSSyncKeyClass *tKeyBuf; //特殊字段，详见本部分特殊字段说明,需要先封包
@property (nonatomic, retain) NSString *iState; //同步状态 定义与mysyncdef.h
//@property (nonatomic, retain) UCSCmdRespListClass *tOpCmdResp; // 对应请求包中的tOplog回复

@end
//7.0    初始化消息同步应答(UCSInitNewSyncRespone)
@interface UCSInitNewSyncResponeClass : NSObject
@property (nonatomic, retain) UCSBaseResponseClass *tBaseResponse;
@property (nonatomic, retain) NSString *iRet; //同步结果标记
@property (nonatomic, retain) NSArray *tMsgList;//UCSAddMsgClass数组，回复的数据内容，详见客户端字段来源于使用
@property (nonatomic, retain) NSArray *tContactList; //UCSModContact 数组

/*
  当讨论组名称被改变、增加人、T人时，tContactList不为空，里面是变化后的成员，tDeleteList为空；
  当你被T出去，或者主动离开的时候，tDeleteList不为空，里面是退出组的组id，tContactList为空
 
 */
//@property (nonatomic, retain) NSString *tdeleteString;
@property (nonatomic, strong) NSArray * tDeleteList; //退出某个讨论组的时候回调，不管是主动还是被动。每一个元素是讨论组id
@property (nonatomic, retain) NSString *iContinueFlag; //还需要同步的iSelector,0为不需要再NewSync
@property (nonatomic, retain) UCSSyncKeyClass *tCurrentKeyBuf; //特殊字段，详见本部分特殊字段说明,需要先封包
@property (nonatomic, retain) UCSSyncKeyClass *tMaxKeyBuf;
@property (nonatomic, retain) NSString *iState; //同步状态 定义与mysyncdef.h
//@property (nonatomic, retain) UCSCmdRespListClass *tOpCmdResp; // 对应请求包中的tOplog回复
@end


//8.    服务端Notify请求(NewNotify)
@interface UCSNewNotifyClass : NSObject     //客户端解析
@property (nonatomic, retain) NSString *packageLength;  //包长度   (需要ntohl)
@property (nonatomic, retain) NSString *headerLength; //包头长度  (需要ntohs)
@property (nonatomic, retain) NSString *version;//协议版本号   (需要ntohs)
@property (nonatomic, retain) NSString *cmd;//命令号  REQ_NEW_NOTIFY (需要ntohl)
@property (nonatomic, retain) NSString *seq;              //(需要ntohl)
@property (nonatomic, retain) NSString *selector;  //需要newsync的selector  (需要ntohl)
@end



//9.    同步有效Key检查(NewSyncCheck)
@interface UCSNewSyncCheckClass : NSObject
@property (nonatomic, retain) NSString *iUin;   //用户uin
@property (nonatomic, retain) NSString *iSyncKeyLen;  //tSyncKeyBuf的长度
@property (nonatomic, retain) UCSSyncKeyClass * tSyncKeyBuf; //直接发送,需要先封包
@end


//10.   同步有效Key检查回应(NewSyncCheckResponse)
@interface UCSNewSyncCheckResponseClass : NSObject
@property (nonatomic, retain) NSString *packageLength;  //包长度   (需要ntohl)
@property (nonatomic, retain) NSString *headerLength; //包头长度  (需要ntohs)
@property (nonatomic, retain) NSString *version;//协议版本号   (需要ntohs)
@property (nonatomic, retain) NSString *cmd;//命令号  RESP_NEW_SYNCCHK (需要ntohl)
@property (nonatomic, retain) NSString *seq;              //(需要ntohl)
@property (nonatomic, retain) NSString *selector;  //需要newsync的selector  (需要ntohl)
@end




//11.	上传语音请求(UploadVoiceRequest)
@interface UCSUploadVoiceRequestClass : NSObject
@property (nonatomic, retain) UCSBaseRequestClass *tBaseRequest;
@property (nonatomic, retain) NSString *pcFromUserName;		//发起端用户名
@property (nonatomic, retain) NSString *pcToUserName;		//目标用户名
@property (nonatomic, retain) NSString *iOffset;				//起始位置，第一个包填0
@property (nonatomic, retain) NSString *iLength;				//当前包的BUFF长度
@property (nonatomic, retain) NSString *pcClientMsgId;		//客户端自定义的声音标识
@property (nonatomic, retain) NSString *iMsgId;				//第一个包填0，后续的包填写服务器返回的ID
@property (nonatomic, retain) NSString *iVoiceLength;			//时间长度
@property (nonatomic, retain) NSData *tData;			    //语音BUFF
@property (nonatomic, retain) NSString *iEndFlag;				//结束标志（发送结束=1，否则=0）
@property (nonatomic, retain) NSString *iCancelFlag;			//取消标志（取消发送=1，否则=0）
@property (nonatomic, retain) NSString *pcMsgSource;			//该字段不填
@property (nonatomic, retain) NSString *iVoiceFormat;			//声音格式
@property (nonatomic, retain) NSString *iUICreateTime;		//时间戳
@property (nonatomic, retain) NSString *iForwardFlag;			//填0
@end
//12.	上传语音回应(UploadVoiceResponse)
@interface UCSUploadVoiceResponseClass : NSObject
@property (nonatomic, retain) UCSBaseResponseClass *tBaseResponse;
@property (nonatomic, retain) NSString *pcFromUserName;			//同Req
@property (nonatomic, retain) NSString *pcToUserName;			//同Req
@property (nonatomic, assign) int iOffset;					//指定一下个包的位置
@property (nonatomic, assign) int iLength;					//同Req
@property (nonatomic, assign) int iCreateTime;              //暂时不需要
@property (nonatomic, retain) NSString *pcClientMsgId;			//同Req
@property (nonatomic, assign) int iMsgId;					//服务器创建的MsgId
@property (nonatomic, retain) NSString *iVoiceLength;				//同Req
@property (nonatomic, retain) NSString *iEndFlag;					//同Req
@property (nonatomic, retain) NSString *iCancelFlag;				//同Req
@end
//13.	下载语音请求(DownloadVoiceRequest)
@interface UCSDownloadVoiceRequestClass : NSObject
@property (nonatomic, retain) UCSBaseRequestClass *tBaseRequest;
@property (nonatomic, assign) int iMsgId;					//从服务器得到的msgId
@property (nonatomic, assign) int iOffset;					//起始位置
@property (nonatomic, assign) int iLength;					//请求长度，总长度
@property (nonatomic, retain) NSString *pcClientMsgId;			//从服务器得到的ClientMsgId
@end



//14.	下载语音回应(DownloadVoiceResponse)
@interface UCSDownloadVoiceResponseClass : NSObject
@property (nonatomic, retain) UCSBaseResponseClass *tBaseResponse;
@property (nonatomic, assign) int iMsgId;					//（req）
@property (nonatomic, assign) int iOffset;					//（req）
@property (nonatomic, assign) int iLength;					//实际发送的长度
@property (nonatomic, assign) int iVoiceLength;				//播放长度
@property (nonatomic, retain) NSString *pcClientMsgId;			//（req）
@property (nonatomic, retain) NSData *tData;			        //语音BUFF
@property (nonatomic, assign) int iEndFlag;					//结束标志
@property (nonatomic, assign) int iCancelFlag;				//取消标志
@end


//15.	上传消息图片请求(UploadMsgImgRequest)
@interface UCSUploadMsgImgRequestClass : NSObject
@property (nonatomic, retain) UCSBaseRequestClass *tBaseRequest;
@property (nonatomic, retain) NSString *pcClientMsgId;       //消息ID，格式参考如下
@property (nonatomic, retain) NSString *pcFromUserName;    //发起者
@property (nonatomic, retain) NSString *pcToUserName;       //接收者
@property (nonatomic, retain) NSString *iTotalLen;			//总大小
@property (nonatomic, retain) NSString *iStartPos;				//起始位置
@property (nonatomic, retain) NSString *iDataLen;				//用不到，填
@property (nonatomic, retain) NSData *tData;				//BUFF（数据+数据长度）
@property (nonatomic, retain) NSString *iMsgType;			//消息图片的类型
@property (nonatomic, retain) NSString *pcMsgSource;
@property (nonatomic, retain) NSString *iCompressType;		//是否是原图发送
@property (nonatomic, retain) NSString *iNetType;				//暂不用
@property (nonatomic, retain) NSString *iPhotoFrom;			//暂不用
@property (nonatomic, retain) NSString *pcMediaId;			//暂不用
@property (nonatomic, retain) NSString *pcCDNBigImgUrl;		//CDN 原图URL
@property (nonatomic, retain) NSString *pcCDNMidImgUrl;		//CND 大图URL
@property (nonatomic, retain) NSString *pcAESKey;			//CND 公钥
@property (nonatomic, retain) NSString *iEncryVer;			//CND 相关
@property (nonatomic, retain) NSString *iCDNBigImgSize;		//上传CDN 的原图大小
@property (nonatomic, retain) NSString *iCDNMidImgSize;		//上传CDN 的大图大小
@property (nonatomic, retain) NSString *pcMD5;             //图片MD5
@end


//16.	上传消息图片回应(UploadMsgImgResponse)
@interface UCSUploadMsgImgResponseClass : NSObject
@property (nonatomic, retain) UCSBaseResponseClass *tBaseResponse;
@property (nonatomic, assign) int iMsgId;						//服务端产生的MSGID
@property (nonatomic, retain) NSString *pcClientMsgId;				//(Req)
@property (nonatomic, retain) NSString *pcFromUserName;				//(Req)
@property (nonatomic, retain) NSString *pcToUserName;				//(Req)
@property (nonatomic, assign) int iTotalLen;					//(Req)
@property (nonatomic, assign) int iStartPos;						//下一个包的起始位置
@property (nonatomic, assign) int iDataLen;						//(Req）
@property (nonatomic, assign) int iCreateTime;					//完整接收之后产生的时间戳
@end

//17.	下载消息图片请求(DownloadMsgImgRequest)
@interface UCSDownloadMsgImgRequestClass : NSObject
@property (nonatomic, retain) UCSBaseRequestClass *tBaseRequest;
@property (nonatomic, assign) int iMsgId;						//MsgId
@property (nonatomic, retain) NSString *pcFromUserName;
@property (nonatomic, retain) NSString *pcToUserName;
@property (nonatomic, assign) int iTotalLen;					//总长度（第一个包为0）
@property (nonatomic, assign) int iStartPos;						//起始位置（第一个为0）
@property (nonatomic, assign) int iDataLen;						//用不到，填0
@property (nonatomic, assign) int iCompressType;				//是否需要原图
@end

//18.	下载消息图片回应(DownloadMsgImgResponse)
@interface UCSDownloadMsgImgResponseClass : NSObject
@property (nonatomic, retain) UCSBaseResponseClass *tBaseResponse;
@property (nonatomic, assign) int iMsgId;						//(Req)
@property (nonatomic, retain) NSString *pcFromUserName;				//(Req)
@property (nonatomic, retain) NSString *pcToUserName;				//(Req)
@property (nonatomic, assign) int iTotalLen;					//总大小
@property (nonatomic, assign) int iStartPos;						//(Req)
@property (nonatomic, assign) int iDataLen;						//(Req)
@property (nonatomic, retain) NSData *tData;				        //BUFF（数据+数据长度）
@end

//19.	上传视频请求(UploadVideoRequest)
@interface UCSUploadVideoRequestClass : NSObject
@property (nonatomic, retain) UCSBaseRequestClass *tBaseRequest;
@property (nonatomic, retain) NSString *pcClientMsgId;         //客户端定义的标识符
@property (nonatomic, retain) NSString *pcFromUserName;      //发送端用户名
@property (nonatomic, retain) NSString *pcToUserName;        //接收端用户名
@property (nonatomic, retain) NSString *iThumbTotalLen;       //缩略图总大小
@property (nonatomic, retain) NSString *iThumbStartPos;        //缩略图起始位置
@property (nonatomic, retain) UCSCmdBufferClass *tThumbData;           //缩略图数据
@property (nonatomic, retain) NSString *iVideoTotalLen;        //视频数据总大小
@property (nonatomic, retain) NSString *iVideoStartPos;         //视频数据起始位置
@property (nonatomic, retain) UCSCmdBufferClass *tVideoData;            //视频数据
@property (nonatomic, retain) NSString *iPlayLength;           //不填
@property (nonatomic, retain) NSString *iNetworkEnv;          //不填
@property (nonatomic, retain) NSString *iCameraType;          //不填
@property (nonatomic, retain) NSString *iFuncFlag;             //不填
@property (nonatomic, retain) NSString *pcMsgSource;          //不填
@property (nonatomic, retain) NSString *pcCDNVideoUrl;       //视频数据CDN URL
@property (nonatomic, retain) NSString *pcAESKey;            //CDN KEY
@property (nonatomic, retain) NSString *iEncryVer;             //CDN相关
@end


//20.	上传视频回应(UploadVideoResponse)
@interface UCSUploadVideoResponseClass : NSObject
@property (nonatomic, retain) UCSBaseResponseClass *tBaseResponse;
@property (nonatomic, retain) NSString *pcClientMsgId;       //(Req)
@property (nonatomic, retain) NSString *iMsgId;             //服务端产生的一个msgId
@property (nonatomic, retain) NSString *iThumbStartPos;      //下一包的起始位置
@property (nonatomic, retain) NSString *iVideoStartPos;       //下一包的起始位置
@end



//21.	下载视频请求(DownloadVideoRequest)
@interface UCSDownloadVideoRequestClass : NSObject
@property (nonatomic, retain) UCSBaseRequestClass *tBaseRequest;
@property (nonatomic, retain) NSString *iMsgId ;               //从服务器得到的msgid
@property (nonatomic, retain) NSString *iTotalLen ;             //视频总大小（第一个包填0）
@property (nonatomic, retain) NSString *iStartPos  ;            // 起始位置（第一个包填0）
@property (nonatomic, retain) NSString *iNetworkEnv  ;         //网络环境
@property (nonatomic, retain) NSString *iMxPackSize  ;          //客户端指定最大包大小//暂时不起作用
@end

//22.	下载视频回应(DownloadVideoResponse)
@interface UCSDownloadVideoResponseClass : NSObject
@property (nonatomic, retain) UCSBaseResponseClass *tBaseResponse;
@property (nonatomic, retain) NSString *iMsgId ;             //(Req)
@property (nonatomic, retain) NSString *iTotalLen;            //视频数据总大小
@property (nonatomic, retain) NSString *iStartPos ;           // (Req)
@property (nonatomic, retain) UCSCmdBufferClass *tData ;               //视频数据（数据+长度）

@end




#pragma mark 讨论组
//讨论组成员
@interface UCSGroupMemberClass : NSObject
@property (nonatomic, retain) NSString *tMemberName;  //id
@property (nonatomic, retain) NSString *tMemberNickName;  //昵称
@property (nonatomic, assign) int iMemberStatus;
@end
//创建讨论组请求(CreateGroupRequest)
@interface UCSCreateGroupRequestClass : NSObject
@property (nonatomic, retain) UCSBaseRequestClass *tBaseRequest;
@property (nonatomic, retain) NSString *introduce; //讨论组主题
@property (nonatomic, assign) int iMemberCount; //人数
@property (nonatomic, retain) NSArray * ptMemberList; //UCSGroupMemberClass成员列表
@end

//创建讨论组响应（CreateGroupRespone）
@interface UCSCreateGroupResponeClass : NSObject
@property (nonatomic, retain) UCSBaseResponseClass *tBaseResponse;
@property (nonatomic, retain) NSString *tCreater;
@property (nonatomic, retain) NSString *introduce; //讨论组主题
@property (nonatomic, retain) NSString *tPYInitial; //主题的拼音
@property (nonatomic, retain) NSString *tQuanPin; //讨论组主题的全拼
@property (nonatomic, assign) int iMemberCount; //讨论组成员数
@property (nonatomic, retain) NSArray *ptMemberList; // UCSGroupMemberClass 成员列表
@property (nonatomic, assign) unsigned int iChatRoomId; //讨论组id
@property (nonatomic, retain) UCSCmdBufferClass *tImgBuf; //暂不使用
@property (nonatomic, retain) NSString *pcBigHeadImgUrl; //头像大图URL 暂不使用
@property (nonatomic, retain) NSString *pcSmallHeadImgUrl; //头像小图URL 暂不使用
@property (nonatomic, retain) NSString * iMaxMemberCount; //讨论组允许的最大成员数
@end
//讨论组加人请求(AddGroupMemberRequest)
@interface UCSAddGroupMemberRequestClass : NSObject
@property (nonatomic, retain) UCSBaseRequestClass *tBaseRequest;
@property (nonatomic, assign) int iMemberCount; //人数
@property (nonatomic, retain) NSArray * ptMemberList; //UCSGroupMemberClass成员列表
@property (nonatomic, assign) unsigned int iChatRoomId; //讨论组id
@end
//讨论组加人响应（AddGroupMemberRespone）
@interface UCSAddGroupMemberResponeClass : NSObject

@property (nonatomic, retain) UCSBaseResponseClass *tBaseResponse;
@property (nonatomic, assign) int iMemberCount; //讨论组成员数
@property (nonatomic, retain) NSArray *ptMemberList; //UCSGroupMemberClass成员列表
@property (nonatomic, assign) unsigned int iChatRoomId; //讨论组id
@end

//讨论组踢人请求(DelGroupMemberRequest)
@interface UCSDelGroupMemberRequestClass : NSObject
@property (nonatomic, retain) UCSBaseRequestClass *tBaseRequest;
@property (nonatomic, assign) int iMemberCount; //踢出的成员数
@property (nonatomic, retain) NSArray * ptMemberList; //成员列表
@property (nonatomic, assign) unsigned int iChatRoomId; //讨论组id
@end

//讨论组踢人响应（CreateGroupRespone）
@interface UCSDelGroupMemberResponeClass : NSObject
@property (nonatomic, retain) UCSBaseResponseClass *tBaseResponse;
@property (nonatomic, assign) int iMemberCount; //踢出的成员数
@property (nonatomic, retain) NSArray *ptMemberList; //踢出的成员列表
@property (nonatomic, assign) unsigned int iChatRoomId; //讨论组id
@end

@interface UCSQuitChatRoomClass : NSObject  //IGGQuitChatRoom_t
@property (nonatomic, assign) unsigned int iChatRoomId;
@property (nonatomic, retain) NSString *tUserName;
@end

//讨论组成员退出请求(QuitGroupRequest)
@interface UCSQuitGroupRequestClass : NSObject
@property (nonatomic, retain) UCSBaseRequestClass *tBaseRequest;
@property (nonatomic, retain) UCSQuitChatRoomClass *tQuitChatRoom;
@end

//讨论组成员退出响应（QuitGroupRespone）
@interface UCSQuitGroupResponeClass : NSObject
@property (nonatomic, retain) UCSBaseResponseClass *tBaseResponse;
@property (nonatomic, retain) UCSQuitChatRoomClass *tQuitChatRoom;
@end


//修改讨论组信息请求(ModGroupTopicRequest)
@interface UCSModGroupTopicRequestClass : NSObject
@property (nonatomic, retain) UCSBaseRequestClass *tBaseRequest;
@property (nonatomic, assign) unsigned int iChatRoomId;
@property (nonatomic, assign) unsigned int iChatRoomType;
@property (nonatomic, retain) NSString *tChatRoomName; //特殊字段，详见本部分特殊字段说明,需要先封包
@property (nonatomic, retain) NSString *tChatRoomTopic; //同步的场景，定义于mmsyncdef.h
@end

// 修改讨论组信息响应（ModGroupTopicRespone）
@interface UCSModGroupTopicResponeClass : NSObject
@property (nonatomic, retain) UCSBaseResponseClass *tBaseResponse;
@property (nonatomic, retain) NSString *iRet; //同步结果标记
@property (nonatomic, retain) UCSCmdListClass *CmdList;;  //回复的数据内容，
@property (nonatomic, retain) NSString * iContinueFlag; //还需要同步的iSelector,0为不需要再NewSync
@property (nonatomic, retain) UCSCmdBufferClass * tKeyBuf; //特殊字段
@property (nonatomic, retain) NSString * iState; ///同步状态 定义与mysyncdef.h
@property (nonatomic, retain) UCSCmdRespListClass * tOpCmdResp; //对应请求包中的tOplog回复
@end



//修改讨论组信息请求(ModGroupTopicRequest)
@interface UCSGetDiscussionInfoRequestClass : NSObject
@property (nonatomic, retain) UCSBaseRequestClass *tBaseRequest;
@property (nonatomic, retain) UCSCmdListClass *tOplog;
@property (nonatomic, retain) NSString *  iSelector; //选择同步的内容
@property (nonatomic, retain) UCSCmdBufferClass *tKeyBuf; //特殊字段，详见本部分特殊字段说明,需要先封包
@property (nonatomic, retain) NSString *iScene; //同步的场景，定义于mmsyncdef.h
@end
// 修改讨论组信息响应（ModGroupTopicRespone）
@interface UCSGetDiscussionInfoResponeClass : NSObject
@property (nonatomic, retain) UCSBaseResponseClass *tBaseResponse;
@property (nonatomic, retain) NSString *iRet; //同步结果标记
@property (nonatomic, retain) UCSCmdListClass *CmdList;;  //回复的数据内容，
@property (nonatomic, retain) NSString * iContinueFlag; //还需要同步的iSelector,0为不需要再NewSync
@property (nonatomic, retain) UCSCmdBufferClass * tKeyBuf; //特殊字段
@property (nonatomic, retain) NSString * iState; ///同步状态 定义与mysyncdef.h
@property (nonatomic, retain) UCSCmdRespListClass * tOpCmdResp; //对应请求包中的tOplog回复
@end





#pragma mark 离线推送

//4.  离线推送请求(SendTokenRequest)
@interface UCSSendTokenRequestClass : NSObject
@property (nonatomic, retain) UCSBaseRequestClass *tBaseRequest;
@property (nonatomic,retain) NSString *pcToken;
@property (nonatomic,retain) NSString *pcSound;
@property (nonatomic,retain) NSString *pcVoipSound;
@property (nonatomic,assign) unsigned int iStatus;
@property (nonatomic,assign) int isDev;
@property (nonatomic,assign) unsigned int iOptCode;

@end

//5.  离线推送应答(SendTokenResponse)
@interface UCSSendTokenResponseClass : NSObject
@property (nonatomic, retain) UCSBaseResponseClass *tBaseResponse;
@end



#pragma mark 地理位置
//38.上传地理位置请求(SendLocationRequest)
@interface UCSSendLocationRequestClass : NSObject

/*
 typedef struct tagIGGSendLocationRequest {
 IGGBaseRequest_t tBaseRequest;
 char * pcClientMsgId;
 char * pcFromUserName;
 char * pcToUserName;
 char * pcContent;
 unsigned int iTotalLen;						//◊‹¥Û–°
 unsigned int iStartPos;						//∆ ºŒª÷√
 unsigned int iDataLen;						//”√≤ªµΩ£¨ÃÓ0
 Comm::SKBuiltinBuffer_t tData;				//BUFF£® ˝æ›+ ˝æ›≥§∂»£©
 char * pcMsgSource;
 char * pcMD5;								//<MD5>
 } IGGSendLocationRequest_t;
 */

@property (nonatomic, strong) UCSBaseRequestClass *tBaseRequest;
@property (nonatomic, copy) NSString *pcClientMsgId;       //消息ID，格式参考如下
@property (nonatomic, copy) NSString *pcFromUserName;    //发起者
@property (nonatomic, copy) NSString *pcToUserName;       //接收者
@property (nonatomic, copy) NSString *pcContent;    //地理位置信息,格式JSON字符串
@property (nonatomic, copy) NSString *iTotalLen;			//图片总大小,地图的缩略图
@property (nonatomic, copy) NSString *iStartPos;				//起始位置
@property (nonatomic, strong) NSData *tData;				//BUFF（数据+数据长度）
@property (nonatomic, copy) NSString *iMsgType;			//消息类型，枚举值：MM_DATA_LOCATION：48
@property (nonatomic, copy) NSString *pcMsgSource; //来源，不填
@property (nonatomic, copy) NSString *pcMD5;             //图片MD5
@end


//39.上传地理位置响应(SendLocationResponse)
@interface UCSSendLocationResponseClass : NSObject
@property (nonatomic, strong) UCSBaseResponseClass *tBaseResponse;
@property (nonatomic, assign) int iMsgId;						//服务端产生的MSGID
@property (nonatomic, copy) NSString *pcClientMsgId;				//(Req)
@property (nonatomic, copy) NSString *pcFromUserName;				//(Req)
@property (nonatomic, copy) NSString *pcToUserName;				//(Req)
@property (nonatomic, assign) int iTotalLen;					//(Req)
@property (nonatomic, assign) int iStartPos;						//下一个包的起始位置
@property (nonatomic, assign) int iDataLen;						//(Req）
@property (nonatomic, assign) int iCreateTime;					//完整接收之后产生的时间戳
@end


#pragma mark 自定义消息

//40.发送自定义消息请求(SendCustomMsgRequest)
@interface UCSSendCustomMsgRequestClass : NSObject
@property (nonatomic, strong) UCSBaseRequestClass *tBaseRequest;
@property (nonatomic, copy) NSString *pcClientMsgId;       //消息ID，格式参考如下
@property (nonatomic, copy) NSString *pcFromUserName;    //发起者
@property (nonatomic, copy) NSString *pcToUserName;       //接收者
@property (nonatomic, copy) NSString *pcContent;    //置空
@property (nonatomic, copy) NSString *iTotalLen;			//总长度
@property (nonatomic, copy) NSString *iStartPos;				//起始位置
@property (nonatomic, strong) NSData *tData;				//BUFF（数据+数据长度）
@property (nonatomic, copy) NSString *iMsgType;			//消息类型，枚举值：
@property (nonatomic, copy) NSString *pcMsgSource; //来源，不填
@property (nonatomic, copy) NSString *pcMD5;             //图片MD5
@end

//41.发送自定义消息响应(SendCustomMsgResponse)
@interface UCSSendCustomMsgResponseClass : NSObject
@property (nonatomic, strong) UCSBaseResponseClass *tBaseResponse;
@property (nonatomic, assign) int iMsgId;						//服务端产生的MSGID
@property (nonatomic, copy) NSString *pcClientMsgId;				//(Req)
@property (nonatomic, copy) NSString *pcFromUserName;				//(Req)
@property (nonatomic, copy) NSString *pcToUserName;				//(Req)
@property (nonatomic, assign) int iTotalLen;					//(Req)
@property (nonatomic, assign) int iStartPos;						//下一个包的起始位置
@property (nonatomic, assign) int iDataLen;						//(Req）
@property (nonatomic, assign) int iCreateTime;
@end

//typedef struct flyPass_auth_header_tag
//{
//    int type;                             /* call mode */
//    int op;                             /* trans mode */
//    ucpaas_int32 ctime;                             /* cost time of telephone */
//    ucpaas_int32 cstate;                            /* comunicate state */
//    ucpaas_int32 role;                              /* call role, offer or answer */
//    ucpaas_int8  callid[UGO_MAX_CALLID_LEN];        /* callid */
//    ucpaas_int8  mgw[UGO_MAX_IP_ADDR_LEN];        /* ip of media gateway */
//    ucpaas_int8  sgw[UGO_MAX_IP_ADDR_LEN];        /* ip of signalling gateway */
//    ucpaas_int8  mcodec[UGO_MAX_CODEC_NAME_LEN];    /* codec of media */
//    ucpaas_int32 mgwport;                           /* port of media gateway */
//    ucpaas_int32 ismute;                            /* if mircrophone is muted when hang up */
//    ucpaas_int32 spreason;
//} flyPass_auth_info_t;



