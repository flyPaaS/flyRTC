//
//  UserDefaultManager.h
//  UXinClient
//
//  Created by Lenya Han on 7/2/12.
//  Copyright (c) 2012 UXIN. All rights reserved.
//
#import <Foundation/Foundation.h>
//-------------------------------------------常量定义------------------------------------------------------//
// 服务器类型
typedef enum
{
    ST_Normal = 0,
    ST_Proxy  = 1
}ServerType;


//-------------------------------------------UCSPAAS存储数据key定义------------------------------------------------//
#define DATA_STORE_LOGIN_TYPE                        @"logintype"
#define DATA_STORE_REQUEST_TYPE                     @"requesttype"
//是否要对这个CLientNumber数据进行上报 20141011
#define DATA_STORE_ISREPORTUID                     @"isreportuid"


#pragma mark - ------------------------------cps配置数据key定义----------------------------------------
//音频FEC使能
#define DATA_STORE_CPS_AUDIOFEC                             @"audiofec"
//驱动自动适配
#define DATA_STORE_CPS_AUTOADAPTER                          @"autoadapter"
//p2p探测使能
#define DATA_STORE_CPS_ICEENABLE                            @"iceenable"
//日志上报使能
#define DATA_STORE_CPS_LOGREPORT                            @"logreport"
//rtp压缩使能
#define DATA_STORE_CPS_PRTPENABLE                           @"prtpenable"
//语音质量监控
#define DATA_STORE_CPS_VQMENABLE                            @"vqmenable"
//有无策略参数 0:无 1:有
#define DATA_STORE_CPS_RESULT                               @"result"


//视频CPS参数
//VP8编码使能开关
#define DATA_STORE_CPS_VP8ENABLE                                @"vp8enable"
//h264编码使能开关
#define DATA_STORE_CPS_H264ENABLE                               @"h264enable"
//开始码率
#define DATA_STORE_CPS_STARTBITRATE                              @"startbitrate"
//最小码率
#define DATA_STORE_CPS_MINBITRATE                                @"minbitrate"
//最大码率
#define DATA_STORE_CPS_MAXBITRATE                                @"maxbitrate"
//默认视频编码
#define DATA_STORE_CPS_FIRSTPT                                   @"firstpt"
//平台预设值
#define DATA_STORE_CPS_PRESETS                                   @"presets"
//根据平台预设值设置的码率
#define DATA_STORE_CPS_BITRATES                                 @"bitrates"
//笨鱼平台预设值设置的帧率
#define DATA_STORE_CPS_FPS                                       @"fps"

//视频码率自定义编码参数
//是否使用自定义视频编码参数
#define DATA_STORE_UD_ISUSECUSTOME_ENC                            @"ud_isUseCustomEnc"
//分辨率：高
#define DATA_STORE_UD_UHEIGHT_ENC                            @"ud_HeightEnc"
//分辨率：宽
#define DATA_STORE_UD_UWIDTH_ENC                             @"ud_uWidthEnc"
//开始码率
#define DATA_STORE_UD_USTARTBITRATE_ENC                      @"ud_uStartBitrateEnc"
//最大码率
#define DATA_STORE_UD_UMAXBITRATE_ENC                        @"ud_usMaxBitrateEnc"
//最小码率
#define DATA_STORE_UD_UMINBITRATE_ENC                        @"ud_uMinbitrateEnc"
//帧率
#define DATA_STORE_UD_UMAXFRAMERATE_ENC                         @"ud_uMaxFramerateEnc"


//视频码率自定义解码参数
//是否使用自定义视频解码参数
#define DATA_STORE_UD_ISUSECUSTOME_DEC                            @"ud_isUseCustomDec"
//分辨率：高
#define DATA_STORE_UD_UHEIGHT_DEC                                 @"ud_HeightDec"
//分辨率：宽
#define DATA_STORE_UD_UWIDTH_DEC                                  @"ud_uWidthDec"
//帧率
#define DATA_STORE_UD_UMAXFRAMERATE_DEC                           @"ud_uMaxFramerateDec"


#pragma mark - ------------------------------摄像头设置采样自定义参数数据key定义----------------------------------------
//是否使用自定义参数
#define DATA_STORE_UD_ISUSECUSTOME_CAMERA                @"ud_isuseCustomCameraAttr"
//分辨率 : 高
#define DATA_STORE_UD_UHEIGHT_CAMERA                     @"ud_HeightCameraAttr"
//分辨率 : 宽
#define DATA_STORE_UD_UWIDTH_CAMERA                      @"ud_WidthCameraAttr"
//图像显示角度
#define DATA_STORE_UD_UROTATEANGLE_CAMERA                @"ud_RotateAngleCameraAttr"
//摄像头索引
#define DATA_STORE_UD_UCAMERAIDX_CAMERA                  @"ud_CameraIdxCameraAttr"
//帧率
#define DATA_STORE_UD_UMAXFRAMERATE_CAMERA               @"ud_uMaxFramerateCamera"
//默认的摄像头采样参数
#define DATA_STORE_UD_UDEFAULTSTATE_CAMERA               @"ud_uDefaultStateCamera"


//视频是否支持来电预览
#define DATA_STORE_UD_ISVIDEINGPREVIEW                           @"ud_isVideingPreview"




#pragma mark - ------------------------------登陆数据数据key定义----------------------------------------

#define DATA_STORE_ATYPE                    @"atype"                    //用于判断是新用户还是老用户
#define DATA_STORE_CLIENTNUMBER             @"clientNumber"             //clientNumber
#define DATA_STORE_MOBILE                   @"phone"                    // 绑定的手机号码
#define DATA_STORE_IM_SSID                  @"im_ssid"                  //回拨需要的请求头参数 ssid
#define DATA_STORE_ID                       @"kcid"                     // userid号码
#define DATA_STORE_APPID                    @"appid"                 // 应用appid
#define DATA_STORE_FNAME                    @"fname"                    // 昵称
#define DATA_STORE_OPENUOGLOG               @"isOpenUgoLog"               //是否打开ugo日志

#pragma mark - ------------------------------语音质量上报需要参数----------------------------------------
#define DATA_STORE_ISTALK                   @"isTalk"                   //是否处于会话状态
#define DATA_STORE_ISCMUTE                  @"isCmute"                  //通话过程中是否点击了静音按钮
#define DATA_STORE_ISVIDEO                  @"isVideo"                  //是否是视频通话
#define DATA_STORE_ISTSTATE                 @"isTstate"                 //是否重连
#define DATA_STORE_ISRECORD                 @"isRecord"                 //是否需要录音
#define DATA_STORE_SERVICEKEY               @"ServiceKey"               //业务关键字， 上传会话日志的时候需要


#pragma mark - ------------------------------对端与本端的视频模式参数保存----------------------------------------
#define DATA_STORE_LOCALCAMERAMODE                @"localCameraMode"    //本端视频模式
#define DATA_STORE_REMOTECAMERAMODE               @"remoteCameraMode"   //对端视频模式



#define DATA_STORE_COOKIE                   @"Cookie"                   // cocokie





#define FORMAL_LOG_ISOPEN          @"logopen"



#define DATA_USERINFO                       @"userinfo_%@"                           // 个人资料






#pragma mark - ------------------------------rtpp参数保存----------------------------------------
#define DATA_STORE_STUNLIST                 @"stunlist"
#define DATA_STORE_RTPPLIST                 @"rtpplist"

#define DATA_STORE_HTTPSERVER_LIST          @"httpserverlist"                        // http服务器列表

#define DATA_STORE_GET_STATIC_ADDR_TIME     @"getstaticaddrtime"                     // 获取静态地址时间
#define DATA_STORE_GET_STATIC_COMMON_TIME   @"getstaticcommontime"                   // 获取静态通用参数时间



#define DATA_STORE_GET_CALLLOG_CONFIG_TIME      @"getcalllogconfigtime"                  //获取上传电话日志配置时间







@interface UserDefaultManager : NSObject





// 从本地数据库获取用户账号
+ (NSString *) GetUserID;

//从本地数据库获取clientNumber
+ (NSString *) GetclientNumber;

// 从本地数据库获取用户手机号码
+ (NSString *) GetUserMobile;


// Set And Get IM_SSID
+ (void) SetIMSSID:(NSString *) strIMSSID;
+ (NSString *) GetIMSSID;


//从本地数据库获取用户类型atype 新用户还是老用户 WLS
+ (int) GetUserAtype;


// 设置keyChain数据
+ (void) SetKeyChain:(NSString *)data key:(NSString *)aKay;
//获得keyChain的通用数据
+ (NSString *) GetKeyChain:(NSString *)aKay;

//删除所有keyChin数据
+ (void)deleteAllKeyChain;

// 从本地数据库获对应key值下的数据(字符串)
+ (NSString *) GetLocalDataString:(NSString *)aKey;

// 从本地数据库获对应key值下的(object)
+ (id) GetLocalDataObject:(NSString *)aKey;

// 设置本地数据库对应key值下的数据(字符串)
+ (void) SetLocalDataString:(NSString *)aValue key:(NSString *)aKey;

// 设置本地数据库对应key值下的数据(字object串)
+ (void) SetLocalDataObject:(id)aValue key:(NSString *)aKey;

// 从本地数据库获对应key值下的数据(bool)
+ (bool) GetLocalDataBoolen:(NSString *)aKey;

// 设置本地数据库对应key值下的数据(bool)
+ (void) SetLocalDataBoolen:(bool)bValue key:(NSString *)aKey;


// 判断账号是否中国号码
+ (BOOL) isAccountCHN;

// 获得int型。
+ (int) GetLocalDataInt:(NSString *)aKey;

// 设置int型。
+ (void) SetLocalDataInt:(int) num key:(NSString *)aKey;

// 获得ssid string
+ (NSString *) getCookieString;

// 删除key
+ (void) removeLocalDataForKey:(NSString *)akey;

// 保存个人资料
+ (void) setUserInfo:(NSMutableDictionary *)data;



// 视频状态是否支持预览
+ (int) getVideoPreView;
// 视频状态是否支持预览
+ (void) setVideoPreView:(int)isPreView;
@end
