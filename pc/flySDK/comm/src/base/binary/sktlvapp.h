#ifndef _SK_TLV_APP_H_
#define _SK_TLV_APP_H_

#include "iTLVPickle.h"
//using namespace Comm;

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define IGG_TYPE_DEFINE	(1024)
#define IGG_FUNC_DEFINE	(19995)

enum {
	eFuncIGGMicroMsgSKGetSelfSvnInfo = IGG_FUNC_DEFINE,
	eFuncIGGMicroMsgSKGetMetainfoFull,
	eFuncIGGMicroMsgSKGetMetainfo,
	eFuncIGGMicroMsgSKEcho
};

enum {
	eTypeIGGMicroMsgEchoInfo = Comm::eTypeSKUserDefine + IGG_TYPE_DEFINE + 1024,
	eTypeBaseRequest,
	eTypeBaseResponse,
	eTypeWebProxyInfo,
	eTypeBirthday,
	eTypeAuthRequest,
	eTypeAuthResponse,
	eTypeAuthBySKRequest,
	eTypeAuthBySKResponse,
	eTypeRtpp,
	eTypeRtppList,
	eTypeMedia,
	eTypeMediaList,
	eTypeSDP,
	eTypeCallReq,
	eTypeCallRsp,
	eTypeCallAck,
	eTypeCallRing,
	eTypeByeReq,
	eTypeUpdate,
	eTypeUpdateRsp,
	eTypeDirectNfy,
	eTypeRouteUpdate,
	eTypeDtmfInfo,
    /* add by Vinton.Liu on 2016-04-05 for add internal state passthrough signalling begin */
	eTypeInTransReq,
	eTypeInTransRsp
    /* Vinton.Liu add end */
};

typedef struct tagIGGMicroMsgEchoInfo {
	unsigned int iEchoLen;
	char * pcEchoStr;
} IGGMicroMsgEchoInfo_t;

// Auth
typedef struct tagIGGBodyHead_t 
{
	int Ret;
	unsigned int Uin;
	unsigned short CmdId;
	int ServerIdLen;
	unsigned char * ServerId;
	unsigned char DeviceId[16];
	short CompressVersion;
	short CompressAlgorithm;
	short CryptAlgorithm;
	unsigned int CompressLen;
	unsigned int CompressedLen;
}IGGBodyHead_t;

typedef struct tagIGGBaseRequest {
	char sSessionKey[36];				//SESSION KEY
	unsigned int iUin;					//UIN
	unsigned char cDeviceID[16];		//设备ID
	int iClientVersion;					//客户端版本号
	char sDeviceType[132];				//设备类型
	unsigned int iScene;				//场景标识符（参考enSceneStatus宏定义）
	unsigned int iSeq;
} IGGBaseRequest_t;

typedef struct tagIGGAuthRequest {
	IGGBaseRequest_t tBaseRequest;
	Comm::SKBuiltinString_t tAccount;		//账号
	Comm::SKBuiltinString_t tPwd;			//密码（需要MD5），如果是FB用户则填写TOKEN（不需要MD5）
	char * pcTimeZone;						//时区
	char * pcLanguage;						//设备语言
	char * pcAuthTicket;						
	char * pcRealCountry;						//国家
	Comm::SKBuiltinString_t tRandomEncryKey;	//客户端产生的随机加密密钥
	Comm::SKBuiltinString_t tDevSid;
	Comm::SKBuiltinString_t tDevPwd;
	Comm::SKBuiltinString_t tLoginToken;
} IGGAuthRequest_t;

typedef struct tagIGGBaseResponse {
	int iRet;
	Comm::SKBuiltinString_t tErrMsg;
	unsigned int iSeq;
} IGGBaseResponse_t;

typedef struct tagIGGBirthday {
	unsigned int iYear;
	unsigned int iMonth;
	unsigned int iDay;
} IGGBirthday_t;

typedef struct tagIGGWebProxyInfo {
	int iIPCount;
	Comm::SKBuiltinString_t * ptIPList;
} IGGWebProxyInfo_t;

typedef struct tagIGGAuthResponse {
	IGGBaseResponse_t tBaseResponse;
	unsigned int iUin;							//UIN
	Comm::SKBuiltinString_t tUserName;			//用户名
	Comm::SKBuiltinString_t tNickName;			//昵称
	Comm::SKBuiltinString_t tBindEmail;
	Comm::SKBuiltinString_t tBindMobile;
	unsigned int iStatus;
	char sSessionKey[36];						//SESSION KEY
	Comm::SKBuiltinBuffer_t tImgBuf;			//头像BUFF
	unsigned int iNewVersion;					//新版本信息，0则表示已经是最新
	unsigned int iRegType;						//注册类型
	unsigned int iProfileFlag;
	unsigned int iUserSex;						//性别（参照enMMSexType宏定义）
	unsigned int iUserStatus;					//用户状态enUserStatus
	Comm::SKBuiltinString_t tFirstName;			
	Comm::SKBuiltinString_t tLastName;	
	IGGBirthday_t tBirthday;
	IGGWebProxyInfo_t tWebProxyInfo;            //短连接IP信息
	Comm::SKBuiltinString_t tSafeUserName;		//Safe用户名
	Comm::SKBuiltinString_t tClientNumber;
	Comm::SKBuiltinString_t tAppid;
} IGGAuthResponse_t;

typedef struct tagIGGAuthBySKRequest {
	IGGBaseRequest_t tBaseRequest;				//iScene字段 填写MM_SCENE_REAUTH
} IGGAuthBySKRequest_t;

typedef struct tagIGGAuthBySKResponse {
	IGGBaseResponse_t tBaseResponse;
	unsigned int iUin;							//Uin(客户端用得到就用，用不到可以不理会)
	unsigned int iNewVersion;					//新版本信息
	IGGWebProxyInfo_t tWebProxyInfo;            //短连接IP信息
	unsigned int iSysTime;					//时间戳
} IGGAuthBySKResponse_t;

// voip
typedef struct tagRtpp{
	Comm::SKBuiltinString_t	cIp;   //Ip地址
	int  iDelay;		       //网络延迟
	int	 iLost;	               //丢包率
}Rtpp_t; 

typedef struct tagRtppList{
	int  iCount;
	Rtpp_t*  ptList;
}RtppList_t;

typedef struct tagMedia{
	short  iPr;		 //Codec优先级别，值越小级别越高
	short  iPt;		 //负载类型 ilbc/speex/g729
	char   cType;	 //(a/v) 对应Audio/video
	int    iPort;    //传输端口
}Media_t;

typedef struct tagMediaList{
	int  iCount;
	Media_t*	  ptList;
}MediaList_t;

typedef struct tagSDP{
	Comm::SKBuiltinString_t  cConIp;	//连接ip地址
	Comm::SKBuiltinString_t  Ver;		    //媒体版本
	RtppList_t  tRtpplist;	//Rtpp报告列表
	MediaList_t  tMedialist;	//媒体类型列表
	Comm::SKBuiltinBuffer_t  cIce;		    //ice信息
    unsigned char support_v; 
    Comm::SKBuiltinString_t through;
}SDP_t;


typedef struct
{
	Comm::SKBuiltinString_t callid;        //唯一标识一个会话，最长64
	SDP_t  callsvr_sdp;
	int 	type;                    //呼叫类型 
	Comm::SKBuiltinString_t  fuserid;      //最长32
	Comm::SKBuiltinString_t  fuid;         //最长32
	Comm::SKBuiltinString_t  fphone;       //手机号,最长32
	Comm::SKBuiltinString_t  fnickname;    //呢称，最长128
	Comm::SKBuiltinString_t  touserid;      //最长32
	Comm::SKBuiltinString_t  tophone;      //被叫手机号
	Comm::SKBuiltinString_t  ver;          //最长64
	int  sflag;
	Comm::SKBuiltinString_t	 through;   //最长512
	Comm::SKBuiltinString_t	 userdata;  //最长512
	unsigned char  meeting;         //是否为会议呼叫
	unsigned char  fpv;             //平台, 0: all ,1:andriod ,2:ios, 3:pc
	unsigned char  fconntype;       //网络类型,,1:wifi,2:2G,4:3G,8:4G
}CallReq_t;

typedef struct
{
	Comm::SKBuiltinString_t  callid;        //唯一标识一个会话，最长64
	SDP_t  callsvr_sdp;
	int 	errcode;           
	Comm::SKBuiltinString_t	 ver;
	int         sflag;
	Comm::SKBuiltinString_t	 through;
}CallRsp_t;

typedef struct
{
	Comm::SKBuiltinString_t  callid;
	SDP_t  callsvr_sdp;
	int  ice_mode;  //Transport type, 0: p2p, 1: rtpp
	int  ice_type;  //Dynamic mode, 0: support, 1: unsupport
}CallAck_t;

typedef struct
{
	Comm::SKBuiltinString_t	  callid;
	SDP_t  callsvr_sdp; 
	Comm::SKBuiltinString_t  through;
	int  sflag;
}CallRing_t;

typedef struct
{
	Comm::SKBuiltinString_t	  callid; 
	int  errcode;
	Comm::SKBuiltinString_t	  through;
}ByeReq_t;

typedef struct
{
	Comm::SKBuiltinString_t	callid;
	Comm::SKBuiltinString_t   mip;
	int                 aport;
	int                 vport;
	Comm::SKBuiltinString_t   apt;
	Comm::SKBuiltinString_t   vpt;
	Comm::SKBuiltinString_t	through;
}CallUpdate_t;

typedef struct
{
	Comm::SKBuiltinString_t	  callid;
	int                   errcode;
	int                   ice_mode;
	Comm::SKBuiltinString_t	  through;
	Comm::SKBuiltinString_t	  apt;  //最长64, 音频能力 多个能力时候以逗号分隔 如18,107
	Comm::SKBuiltinString_t	  vpt;  //最长64, 视频能力 多个能力时候以逗号分隔 如107,108
}CallUpdateRsp_t;


typedef struct
{
	Comm::SKBuiltinString_t	  callid;
	int  errcode;
	Comm::SKBuiltinString_t	  through;
}DirectNfy_t;

typedef struct
{
	Comm::SKBuiltinString_t	  callid; 
	Comm::SKBuiltinString_t	  through;
	Comm::SKBuiltinString_t	  userid;
}RouteUpdate_t;

typedef struct
{
	Comm::SKBuiltinString_t	  callid; 
	Comm::SKBuiltinString_t	  dtmf;
}DtmfInfo_t;

/* add by Vinton.Liu on 2016-04-05 for add internal state passthrough signalling begin */
typedef struct
{
    Comm::SKBuiltinString_t	  callid;
    Comm::SKBuiltinString_t	  through; 
} InTransReq_t;

typedef struct
{
	Comm::SKBuiltinString_t	  callid; 
	int  errcode;
} InTransRsp_t;
/* Vinton.Liu add end */

extern const Comm::SKMetaInfo_t g_tMetaSipexMsg;

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif 




