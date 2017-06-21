/* SoftphoneManager.mm
 *
 * Created by Liam
 * 2012.6.14
 */
#import <MediaPlayer/MediaPlayer.h>
#import "SoftphoneManager.h"

#import "MediaEngineInterface.h"
#import "CallLogManager.h"
#import "UCSTCPClientManager.h"
#import "ugo.h"
#import "UCSCallingHttpRequest.h"
#import "CustomUDPListPing.h"
#import "Config.h"
#import "sys/utsname.h"
#import "UCSQualityRequestManager.h"
#import "UIDevice+IOKit_Extensions.h"
#import "UCSCPSManager.h"
#import "UCSDialViewController.h"
//截图存放路径
#define DATA_STORE_VIDEOCAPTRUE_FILEPATH                          @"store_videocaptrue_filepath"



#ifdef VIDEO_ENABLED
#include "ViGoEngine.h"
#else
//#include "VoGoEngine.h"
#endif

using namespace gl_media_engine;

gl_media_engine::MediaEngineInterface* g_pMediaEngin = NULL;

//early media control
static int g_CallMode = eUGo_CM_AUTO;

// Softphone Manager实例
static SoftphoneManager* theSoftphoneManager  = nil;


typedef struct tag_stuct_video_payload_info
{
    int iPayLoadType;
    char cRemoteVideoIp[32];
    int	iRemoteVideoPort;
    int iLocalVideoPort;
    char ucVideoEnable;
} svideo_payload_info;

//@interface SoftphoneManager(private)

//@end

@class MacVideoView;

@implementation SoftphoneManager
{
    UCSCallingHttpRequest  *_callRequest;
    NSString            *_rspCallId;
    int                 _rspCallVsp;
    KCTReason        *_ucsReason;
    CustomUDPListPing   *_voipListPing;
    
}

@synthesize delegate;
@synthesize reasons;

- (id) init
{
    self = [super init];
    return self;
}

+ (SoftphoneManager*) instance
{
    if (nil == theSoftphoneManager)
    {
        theSoftphoneManager = [[SoftphoneManager alloc] init];
    }
    
    return theSoftphoneManager;
}


#pragma mark - --------------------------与组件绑定的3个方法  SoftPhoneCallback logcallback_func  UgoSendCallback--------------------------
#if defined (UGO_ENABLED)

void SoftPhoneCallback(int ev_type, int ev_reason, const char* something, const void* param)
{
    //    UXLogNotice(@"SoftPhoneCallback: %d %d %s", ev_type, ev_reason, something);
    //    UXLogNotice(@"SoftPhoneCallback:kucky-------------------", ev_type, ev_reason, something);
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
    SoftphoneManager *pSPManager = [SoftphoneManager instance];
    id delegate = pSPManager.delegate;
    if (delegate)
    {
        //        UXLogNotice(@"SoftPhoneCallback:kucky------------I'm In-------", ev_type, ev_reason, something);
        NSString *strDescription = [NSString stringWithCString:something encoding:NSUTF8StringEncoding];
        if ([delegate respondsToSelector:@selector(SoftPhoneCallback:withReason:withDescription:withParam:)]) {
            [delegate SoftPhoneCallback:ev_type withReason:ev_reason withDescription:strDescription withParam:param];
        }
    }
//    if (ev_type == eUGo_CALLINCOMING_EV) {
//        [pSPManager playLocalMusic];
//    }
    [pool release];
}

#else

#endif


#ifdef UGO_ENABLED
static void logcallback_func(char* summary, char* detail, enum eME_TraceLevel level)
{
//    printf("-----------------------------------\n");
//    printf("Level:%d\nReason:%s\nDetails:\n%s\n", level, summary, detail);
//    
//    UXLogNotice(@"logcallback_func: %s %s", summary, detail);
    //    if ([CallLogManager instance].bNeedUploadCalllog)
    //    {
    NSString *strSummary = [NSString stringWithUTF8String:summary];
    NSString *strDetail = [NSString stringWithUTF8String:detail];
    
    [[CallLogManager instance] saveCallLogInfo:strSummary withDetail:[NSString stringWithFormat:@"level: %x   detail : %s",level,detail]];
    
    //                [[SDKLogManager instance]saveSDKLogInfo:strSummary withDetail:strDetail];
    if (level == kME_TraceReport) {
        if ([UCSDialViewController share].isUCSCalling) {
            UCSDialViewController * dial = [UCSDialViewController share];
            dispatch_async(dispatch_get_main_queue(), ^{
                [dial performSelector:@selector(uploadConversationLogWithDetail:) withObject:strDetail];
                
            });
            
        }
    }
    
    //    }
}
#else

#endif


typedef struct _packTagHead{
    UInt16 headerLength; //包头  2字节
    UInt16 packageLength;//数据包长度 2字节
    
}packTagHead;


//组件收到消息后（拨号，挂断，接听），通过此方法向服务器更新操作状态。
void UgoSendCallback(const char* msg, int slen)
{
    NSData *megData = [NSData dataWithBytes:msg length:slen];
    
    //tcp_event_node_t
    dispatch_async(dispatch_get_main_queue(), ^(void){
        [[UCSTCPClientManager instance] asynSendRequest:megData];
    });
}

// generic log handler for debug version   // 未知序列 add by kucky
static void linphone_iphone_log_handler(int lev, const char *fmt, va_list args)
{
    NSString* format = [[NSString alloc] initWithCString:fmt encoding:[NSString defaultCStringEncoding]];
    NSLogv(format,args);
    NSString* formatedString = [[NSString alloc] initWithFormat:format arguments:args];
    [format release];
    [formatedString release];
}



#pragma mark - --------------------------组件状态设置--------------------------
// 设置Softphone
- (BOOL) setSoftphoneManager
{
#if !(TARGET_IPHONE_SIMULATOR)
    
#if defined (UGO_ENABLED)
    
//    MPVolumeView *mp;
//       MPMusicPlayerController *musicPlayer = [MPMusicPlayerController applicationMusicPlayer];
//    if (musicPlayer.volume < 0.1f)
//        musicPlayer.volume = 0.8f;
//    NSLog(@"sysVolume[%f]", musicPlayer.volume);
#endif
    
#endif
    
    return YES;
}

// 初始化Softphone
- (BOOL) initSoftphoneManager
{
    int nRes = -1;
    
#if !(TARGET_IPHONE_SIMULATOR)

    

#if defined (UGO_ENABLED)
    nRes = UGo_load_media_engine();
    if (0 != nRes)
    {
        NSLog(@"load_media_engine failed");
    }
    
    ugo_cb_vtable_t cb_table;
    cb_table.event_cb = SoftPhoneCallback;
    cb_table.log_cb = logcallback_func;
    cb_table.send_cb = UgoSendCallback;
    cb_table.screen_shot_cb=screen_shot_cb; //视频截图回调
    /**
     @author WLS, 16-02-24 19:02:16
     
     rtp包加密回调
     */
    cb_table.encrypt = UgoEncryptCallBack;
    cb_table.decrypt = UgoDecryptCallBack;
    nRes = UGo_callback_vtable(&cb_table);
    
    
    [self setSoftphoneManager];
    
    NSLog(@"setSoftphoneManager sucess.");
    nRes = UGo_init();
    
    NSLog(@"UGo_init sucess.");
    /**
     @author WLS, 16-05-20 14:05:20
     
     组件初始化成功后 设置默认的cps参数
     */
    [self setConfigByCPS];
    
    
    /**
     @author WLS, 16-02-01 15:02:27
     
     设置打印日志，并且存储到本地
     */
    ugo_log_trace_pm_t tracepm;
    memset(&tracepm, 0, sizeof(tracepm));
    
    /**
     @author WLS, 16-02-26 14:02:28
     
     日志级别为0x4000 + 0x00ff
     */
    tracepm.level = [UserDefaultManager GetLocalDataBoolen:DATA_STORE_OPENUOGLOG]==YES?0x40ff:kME_TraceNone;
    tracepm.level = 0x40ff;
    if (tracepm.level == 0x40ff) {
        NSString * file = [SDKLogManager getLogFileWithName:@"MediaEngineLog"];
        memcpy(tracepm.filepath, [file cStringUsingEncoding:NSASCIIStringEncoding], [file length]);
        
        NSString * ugoFile = [SDKLogManager getLogFileWithName:@"ugoLog"];
        UGo_enable_stack_log([ugoFile cStringUsingEncoding:NSASCIIStringEncoding]);

    }
    
    UGo_set_log_file(&tracepm);
    
    
#else
   
    
#endif
    
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(getTheBestIPSuccessNotification) name:kGetTheBestIPSuccessNotification object:nil];
    
    //监听动态策略设置广播
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(setConfigByCPS) name:kSetICE2FEC2EMODEL2PRTPNotification object:nil];
    
#endif
    
    if (0 != nRes)
    {
        NSLog(@"init failed");
    }
    
    //组件初始化成功之后 获取默认的摄像头的采样参数 WLS
    //已经不需要获取摄像头采样 20150929
    //    [self setDefaultCamereAtrr];
    
    return YES;
}

//组件初始化成功之后 获取默认的摄像头的采样参数
- (void)setDefaultCamereAtrr{
    
    NSDictionary * defaultCameraState = [UserDefaultManager GetLocalDataObject:DATA_STORE_UD_UDEFAULTSTATE_CAMERA];//默认的摄像头采样参数 add by WLS 20150624
    
    if (defaultCameraState == nil){
        
        
        //保存默认的摄像头采样参数 (WLS 20150624)
        ME_video_camera_param_t stAttr_camera;
        
        UGo_video_get_camera_state(&stAttr_camera);
        
        //保存默认的摄像头采样参数 (WLS 20150624)
        defaultCameraState = [NSDictionary dictionaryWithObjectsAndKeys:[NSString stringWithFormat:@"%d",stAttr_camera.usHeight],DATA_STORE_UD_UHEIGHT_CAMERA,
                              [NSString stringWithFormat:@"%d",stAttr_camera.usWidth],DATA_STORE_UD_UWIDTH_CAMERA,
                              [NSString stringWithFormat:@"%d",stAttr_camera.usRotateAngle],DATA_STORE_UD_UROTATEANGLE_CAMERA,
                              [NSString stringWithFormat:@"%d",stAttr_camera.usCameraIdx],DATA_STORE_UD_UCAMERAIDX_CAMERA,
                              [NSString stringWithFormat:@"%d",stAttr_camera.ucMaxFps],DATA_STORE_UD_UMAXFRAMERATE_CAMERA,nil];
        
        [UserDefaultManager SetLocalDataObject:defaultCameraState key:DATA_STORE_UD_UDEFAULTSTATE_CAMERA];
        
    }
    
}


//当UCSTCP连接成功后组件需要更新连接状态。
- (void)uxOnConnect
{
#if !(TARGET_IPHONE_SIMULATOR)
    
    /*********************************配置ugo_cfg_t 第二方案 ************************************************************************************/
    ugo_cfg_t ucfg;
    memset(&ucfg, 0, sizeof(ucfg));
    ucpaas_int32 result = UGo_get_config(UGO_CFG_PARAM_MODULE_ID, &ucfg, 0);
    NSLog(@"result:%d",result);
    
    
    char ch[65] = {0};//初始化数组
    bzero(ch, 0);     //全部设置为0
    
    //    UXLogInfo(@"hjl--- 当前用户的uid(%@),当前用户的电话号码(%@)",[UserDefaultManager GetUserID],[UserDefaultManager GetUserMobile]);
    [[UserDefaultManager GetclientNumber] getCString:ch maxLength:64 encoding:NSUTF8StringEncoding];//将uid填充到ch
    
    strncpy(ucfg.uid, ch, 64); //将ch填充到ucfig.uid中
    
    //bzero(ch, 0); //全部设置为0
    [[UserDefaultManager GetUserMobile] getCString:ch maxLength:64 encoding:NSUTF8StringEncoding]; //将phonenumber填充到ch
    strncpy(ucfg.phone, ch, 64); //将ch填充到ucfg.phone中
    
    //bzero(ch, 0);     //全部设置为0
    [[UserDefaultManager GetUserID] getCString:ch maxLength:64 encoding:NSUTF8StringEncoding];//将userid填充到ch
    strncpy(ucfg.userid, ch, 64); //将ch填充到ucfig.userid中
    
    ucfg.atype = 2;//旧版SDK使用的是clientNumber：1 ，新版本SDK使用的是userid:0，但是目前这个SDK是测试版本所以atype：2
    
    /*****************从setSoftphoneManager 移过来**************************/
    int video_PreViewMode = [UserDefaultManager getVideoPreView];
    ucfg.video_enabled = 1; //视频开关,1.普通视2.视频支持预览
    ucfg.rc4_enabled = FALSE;   //rc4 加密
    ucfg.platform = 0x00;       //平台为ios
    ucfg.csrv_enabled = FALSE;
    ucfg.tlv_enabled = FALSE; // 是否启用IM3.0中的二进制协议，1为使用，0为不使用 add by WLS 20151026 组件版本2.14.3.0
    ucfg.compress_enabled = FALSE; //二进制压缩使能 add by WLS 20151026
    //ipv6
    //ucfg.ipv6_enabled = TRUE;
    strcpy(ucfg.brand, "uxin"); //平台为KCT  
    /*****************从setSoftphoneManager 移过来**************************/
    
    
    UGo_set_config(UGO_CFG_PARAM_MODULE_ID, &ucfg, 0);
    
    
    
    //设置rtpp接收超时时间  add by WLS 2016-01-06
    ME_RTP_cfg_t rtpConfig;
    UGo_get_config(ME_RTP_CFG_MODULE_ID, &rtpConfig, 0);
    rtpConfig.uiRTPTimeout = 20;
    UGo_set_config(ME_RTP_CFG_MODULE_ID, &rtpConfig, 0);
    
    
    /*********************************配置ugo_cfg_t 第二方案 ************************************************************************************/
    
    /**
     @author WLS, 16-04-29 11:04:22
     
     如果是重连，使用eUGo_TCP_RECONNECTED这个枚举值。
     */
    UGo_TcpTransport_state state = [UCSTCPClientManager instance].isRelogin?eUGo_TCP_RECONNECTED:eUGo_TCP_CONNECTED;
    UGo_tcp_update_state(state);
    if (_rspCallId) {
        PhoneNetType currentNetType = [PublicFunc GetCurrentPhoneNetType];
        if (PNT_2G == currentNetType)
        {
            //            [PublicFunc ShowMessageBox:0 titleName:nil contentText:@"收到有信来电，但是您当前网络不支持免费接听(请确保WiFi、3G连接正常)。" cancelBtnName:@"我知道了" delegate:nil];
            NSLog(@"%@",@"收到来电，但是您当前网络不支持免费接听(请确保WiFi、3G连接正常)。");
            
            [self DisablePush];
            
            
        } else {
            
            
            if (reasons.reason == 0) {//支持推送
                
                [self EnablePush];
            }else{
                
                [self DisablePush]; //不支持推送
                
            }
            
            
            
        }
    }
    [_rspCallId release], _rspCallId = nil;  [_ucsReason release]; _ucsReason = nil;
    
    
    NSArray *arrayVoipList = [UserDefaultManager GetLocalDataObject:DATA_STORE_RTPPLIST];
    //    arrayVoipList = [NSArray arrayWithObjects:@"113.31.89.138:7801",@"103.227.120.248:7801",@"221.228.76.248:7801",@"120.195.155.199:7801",@"116.211.105.61:7801",@"113.57.219.53:7801",@"111.47.123.71:7801",@"183.60.126.7:7801",@"163.177.24.22:7801",@"183.232.28.141:7801",@"113.31.89.132:7801", nil];
    //        arrayVoipList = [NSArray arrayWithObjects:@"113.31.89.132",@"113.31.89.132:7801",@"113.31.82.188:7801",@"113.31.82.172:7801",nil];
    /*
    if (nil != arrayVoipList && [arrayVoipList count] > 0)
    {
        if (_voipListPing != nil) {
            [_voipListPing stopAllPing];
            _voipListPing.didNotPostNoti = YES;
            [_voipListPing release];
            _voipListPing = nil;
        }
        _voipListPing = [[CustomUDPListPing alloc] init];
        
        _voipListPing.isIpv6 = self.isIpv6;
        [_voipListPing setPingList:arrayVoipList];
        [_voipListPing startPing];
    }*/
    
    //配置视频自定义参数
    [self initWithUserDefineForVideo];
#endif
}

- (void)uxOnConnect2
{
#if !(TARGET_IPHONE_SIMULATOR)
    
    
    return;
    /*********************************配置ugo_cfg_t 第二方案 ************************************************************************************/
    ugo_cfg_t ucfg;
    memset(&ucfg, 0, sizeof(ucfg));
    ucpaas_int32 result = UGo_get_config(UGO_CFG_PARAM_MODULE_ID, &ucfg, 0);
    NSLog(@"result:%d",result);
    
    
    char ch[65] = {0};//初始化数组
    bzero(ch, 0);     //全部设置为0
    
    //    UXLogInfo(@"hjl--- 当前用户的uid(%@),当前用户的电话号码(%@)",[UserDefaultManager GetUserID],[UserDefaultManager GetUserMobile]);
    [[UserDefaultManager GetclientNumber] getCString:ch maxLength:64 encoding:NSUTF8StringEncoding];//将uid填充到ch
    
    strncpy(ucfg.uid, ch, 64); //将ch填充到ucfig.uid中
    
    bzero(ch, 0); //全部设置为0
    [[UserDefaultManager GetUserMobile] getCString:ch maxLength:64 encoding:NSUTF8StringEncoding]; //将phonenumber填充到ch
    strncpy(ucfg.phone, ch, 64); //将ch填充到ucfg.phone中
    
    bzero(ch, 0);     //全部设置为0
    [[UserDefaultManager GetUserID] getCString:ch maxLength:64 encoding:NSUTF8StringEncoding];//将userid填充到ch
    strncpy(ucfg.userid, ch, 64); //将ch填充到ucfig.userid中
    
    ucfg.atype = [UserDefaultManager GetUserAtype];//旧版SDK使用的是clientNumber：1 ，新版本SDK使用的是userid:0，但是目前这个SDK是测试版本所以atype：2
    
    /*****************从setSoftphoneManager 移过来**************************/
    int video_PreViewMode = [UserDefaultManager getVideoPreView];
    ucfg.video_enabled = video_PreViewMode; //视频开关,1.普通视2.视频支持预览
    ucfg.rc4_enabled = FALSE;   //rc4 加密
    ucfg.platform = 0x02;       //平台为ios
    ucfg.csrv_enabled = FALSE;
    ucfg.tlv_enabled = FALSE; // 是否启用IM3.0中的二进制协议，1为使用，0为不使用 add by WLS 20151026 组件版本2.14.3.0
    ucfg.compress_enabled = TRUE; //二进制压缩使能 add by WLS 20151026
    strcpy(ucfg.brand, "yzx"); //平台为KCT  
    /*****************从setSoftphoneManager 移过来**************************/
    
    
    UGo_set_config(UGO_CFG_PARAM_MODULE_ID, &ucfg, 0);
    
    
    
    //设置rtpp接收超时时间  add by WLS 2016-01-06
    ME_RTP_cfg_t rtpConfig;
    UGo_get_config(ME_RTP_CFG_MODULE_ID, &rtpConfig, 0);
    rtpConfig.uiRTPTimeout = 20;
    UGo_set_config(ME_RTP_CFG_MODULE_ID, &rtpConfig, 0);
    
    
    /*********************************配置ugo_cfg_t 第二方案 ************************************************************************************/
    
    /**
     @author WLS, 16-04-29 11:04:22
     
     如果是重连，使用eUGo_TCP_RECONNECTED这个枚举值。
     */
    UGo_TcpTransport_state state = [UCSTCPClientManager instance].isRelogin?eUGo_TCP_RECONNECTED:eUGo_TCP_CONNECTED;
    UGo_tcp_update_state(state);
    if (_rspCallId) {
        PhoneNetType currentNetType = [PublicFunc GetCurrentPhoneNetType];
        if (PNT_2G == currentNetType)
        {
            //            [PublicFunc ShowMessageBox:0 titleName:nil contentText:@"收到有信来电，但是您当前网络不支持免费接听(请确保WiFi、3G连接正常)。" cancelBtnName:@"我知道了" delegate:nil];
            NSLog(@"%@",@"收到来电，但是您当前网络不支持免费接听(请确保WiFi、3G连接正常)。");
            
            [self DisablePush];
            
            
        } else {
            
            
            if (reasons.reason == 0) {//支持推送
                
                [self EnablePush];
            }else{
                
                [self DisablePush]; //不支持推送
                
            }
            
            
            
        }
    }
    [_rspCallId release], _rspCallId = nil;  [_ucsReason release]; _ucsReason = nil;
    
    
    NSArray *arrayVoipList = [UserDefaultManager GetLocalDataObject:DATA_STORE_RTPPLIST];
    //    arrayVoipList = [NSArray arrayWithObjects:@"113.31.89.138:7801",@"103.227.120.248:7801",@"221.228.76.248:7801",@"120.195.155.199:7801",@"116.211.105.61:7801",@"113.57.219.53:7801",@"111.47.123.71:7801",@"183.60.126.7:7801",@"163.177.24.22:7801",@"183.232.28.141:7801",@"113.31.89.132:7801", nil];
    //        arrayVoipList = [NSArray arrayWithObjects:@"113.31.89.132",@"113.31.89.132:7801",@"113.31.82.188:7801",@"113.31.82.172:7801",nil];
    if (nil != arrayVoipList && [arrayVoipList count] > 0)
    {
        if (_voipListPing != nil) {
            [_voipListPing stopAllPing];
            _voipListPing.didNotPostNoti = YES;
            [_voipListPing release];
            _voipListPing = nil;
        }
        _voipListPing = [[CustomUDPListPing alloc] init];
        _voipListPing.isIpv6 = self.isIpv6;
        
        [_voipListPing setPingList:arrayVoipList];
        [_voipListPing startPing];
    }
    
    //配置视频自定义参数
    [self initWithUserDefineForVideo];
#endif
}




//开启UDP Ping 检测
- (void)startVoipListPing{
    
    
    NSArray *arrayVoipList = [UserDefaultManager GetLocalDataObject:DATA_STORE_RTPPLIST];
    
    //    arrayVoipList = [NSArray arrayWithObjects:@"113.31.89.138:7801",@"103.227.120.248:7801",@"221.228.76.248:7801",@"120.195.155.199:7801",@"116.211.105.61:7801",@"113.57.219.53:7801",@"111.47.123.71:7801",@"183.60.126.7:7801",@"163.177.24.22:7801",@"183.232.28.141:7801",@"113.31.89.132:7801", nil];
    //        arrayVoipList = [NSArray arrayWithObjects:@"113.31.89.132",@"113.31.89.132:7801",@"113.31.82.188:7801",@"113.31.82.172:7801",nil];
    if (nil != arrayVoipList && [arrayVoipList count] > 0)
    {
        if (_voipListPing != nil) {
            [_voipListPing stopAllPing];
            _voipListPing.didNotPostNoti = YES;
            [_voipListPing release];
            _voipListPing = nil;
        }
        _voipListPing = [[CustomUDPListPing alloc] init];
        
        _voipListPing.isIpv6 = self.isIpv6;
        [_voipListPing setPingList:arrayVoipList];
        [_voipListPing startPing];
    }
    
}

//当UCSTCP连接断开后组件需要更新连接状态。
- (void)uxOnDisconnect
{
#if !(TARGET_IPHONE_SIMULATOR)
    UGo_tcp_update_state(eUGo_TCP_DISCONNECTED);
#endif
}


//当UCSTCP接收到协议时，需要转发给组件
- (void)uxOnRecived:(NSData *)recivedData
{
#if !(TARGET_IPHONE_SIMULATOR)
    UGo_tcp_recv_msg((int)[recivedData length], (void*)[recivedData bytes]);
#endif
}


#pragma mark - --------------------------互联网音频能力--------------------------
// 拨打电话
- (BOOL) callPhoneNumber:(NSString *) strUid withPN:(NSString *) strPhoneNumber withCallMode:(int)callMode andUserdata:(NSString *)callerData
{
    BOOL bRet = NO;
    int nRes = -1;
    g_CallMode = callMode;
#if !(TARGET_IPHONE_SIMULATOR)
#if defined (UGO_ENABLED)
    
    dispatch_async(dispatch_get_current_queue(), ^{
        NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
        
        ugo_call_dialing_para_t callpm;
        callpm.call_mode = g_CallMode;//g_CallMode;
        callpm.video_enabled = FALSE; //当需要拨打视频电话时，此开关需打开，否则默认为拨打语音电话。
        
        if ([strPhoneNumber length] == 0 ) {
            if ([strUid length] == 0)
                return;
            //            UGo_call_dialing([strUid UTF8String], "");
            strcpy(callpm.tuid, [strUid UTF8String]);
            strcpy(callpm.tphone, "");
        } else if ([strUid length] == 0){
            //            UGo_call_dialing("", [strPhoneNumber UTF8String]);
            strcpy(callpm.tuid, "");
            strcpy(callpm.tphone, [strPhoneNumber UTF8String]);
        } else {
            //            UGo_call_dialing([strUid UTF8String], [strPhoneNumber UTF8String]);
            strcpy(callpm.tuid, [strUid UTF8String]);
            strcpy(callpm.tphone, [strPhoneNumber UTF8String]);
        }
        
        //        strcpy(callpm.name, "");         //拨打电话其它未用到字段赋默认值 add by baige 2015年02月02日
        
        
        if ([callerData length] == 0)
        {
            strcpy(callpm.user_data, "");
        }
        else
        {
            strcpy(callpm.user_data, [callerData UTF8String]);
        }
        
        
        callpm.calltype=0;
        strcpy(callpm.reserved, "");
        
        int nRes = UGo_call_dialing(&callpm);
        UXLogNotice(@"UGo_call_dialing: %@ %@", strPhoneNumber, strUid);
        if (_callRequest == nil) {
            _callRequest = [[UCSCallingHttpRequest alloc] init];
        }
        [_callRequest emodelReauest:strPhoneNumber caller:[UserDefaultManager GetUserID]];
        [pool drain];
    });
    
    
    
    
    //    call_dialing_pm_t callpm;
    //    callpm.call_mode = callMode;//g_CallMode;
    //    strcpy(callpm.uid, [strUid UTF8String]);
    //    strcpy(callpm.uphone, [strPhoneNumber UTF8String]);
    //    nRes = UGo_call_dialing(&callpm);
#else
    //    nRes = TGo_request_call([strUid UTF8String]);
#endif
#endif
    if (0 == nRes)
    {
        bRet = YES;
    }
    return bRet;
}

// 挂断
- (BOOL) hangUpCall
{
    BOOL bRet = NO;
    int nRes = -1;
    
#if !(TARGET_IPHONE_SIMULATOR)
#if defined (UGO_ENABLED)
    
    dispatch_async(dispatch_get_current_queue(), ^{
        NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
        
        int nRes = UGo_call_hangup(eUGo_Reason_HungupMyself);
        //        int nRes = UGo_call_hangup();
        [[CallLogManager instance] saveCallLogInfo:@"主动调用组件Hungup通话" withDetail:nil];
        
        [_callRequest startAsyncRequest];
        
        UXLogNotice(@"UGo_call_hangup %d", nRes);
        [pool drain];
    });
    
    //    nRes = UGo_call_hangup();
#else
    //    nRes = TGo_hangup_call();
#endif
#endif
    
    if (0 == nRes)
    {
        bRet = YES;
    }
    return bRet;
}


// 接听
- (BOOL) answerCall
{
    BOOL bRet = NO;
    int nRes = -1;
    
#if !(TARGET_IPHONE_SIMULATOR)
#if defined (UGO_ENABLED)
    
    dispatch_async(dispatch_get_current_queue(), ^{
        NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
        //        int nRes = UGo_call_answered();
        int nRes = UGo_call_answered();
        
        [_callRequest release];
        _callRequest = nil;
        UXLogNotice(@"UGo_call_answered %d", nRes);
        [pool drain];
    });
    //    nRes = UGo_call_answered();
#else
    //    nRes = TGo_answer_call();
#endif
#endif
    
    if (0 == nRes)
    {
        bRet = YES;
    }
    return bRet;
}


// 发送DTMF
- (BOOL) sendDTMF:(char) mDigit
{
    BOOL bRet = NO;
    int nRes = -1;
    
#if !(TARGET_IPHONE_SIMULATOR)
#if defined (UGO_ENABLED)
    nRes = UGo_send_DTMF(mDigit);
#else
    //    nRes = TGo_send_DTMF(mDigit);
#endif
#endif
    
    if (0 == nRes)
    {
        bRet = YES;
    }
    return bRet;
}


// 开关免提
- (BOOL) setLoudSpeaker:(BOOL) bOpen
{
    BOOL bRet = NO;
    int nRes = -1;
    int ret = -1;
#if !(TARGET_IPHONE_SIMULATOR)
#if defined (UGO_ENABLED)
    nRes = UGo_set_loudspeaker_status(bOpen);
    
    ME_ENV_cfg_t env_pm;
    env_pm.status = bOpen;
    env_pm.networktype = 0;
    ret = UGo_set_config(ME_ENV_CFG_MODULE_ID, (void *)&env_pm, 0);
#else
    //    nRes = TGo_set_speaker_mute(bOpen);
#endif
#endif
    
    if (0 == nRes)
    {
        bRet = YES;
    }
    
    [[SDKLogManager instance]saveSDKLogInfo:@"设置扬声器" withDetail:[NSString stringWithFormat:@"nRes: %d   ret : %d",nRes,ret]];
    
    
    
    return bRet;
}


// 获取免提状态
- (BOOL) getLoudSpeakerState{
    
    /**
     @author WLS, 15-12-17 09:12:28
     
     使用组件提供的方法获取免提状态
     */
    BOOL loudspeaker_status =  UGo_get_loudspeaker_status()==1?YES:NO;
    
    return loudspeaker_status;
}


// 开关Mic，是否静音
- (BOOL) setMicStatus:(BOOL) bOpen
{
    BOOL bRet = NO;
    int nRes = -1;
    
#if !(TARGET_IPHONE_SIMULATOR)
#if defined (UGO_ENABLED)
    nRes = UGo_set_mic_mute(bOpen);
#else
    //    nRes = TGo_set_mic_mute(bOpen);
#endif
#endif
    
    if (0 == nRes)
    {
        bRet = YES;
    }
    
    return bRet;
}


#pragma mark - --------------------------互联网视频能力--------------------------

-(void)initWithUserDefineForVideo
{
    
    
    //*****************************视频编码**********************************//
    //**********************如果用户启用自定义视频参数则优先设置自定义参数**********************//
    
    NSString *str_isUseCustomENC = [UserDefaultManager GetKeyChain:DATA_STORE_UD_ISUSECUSTOME_ENC];//是否启用自定义参数
    
    if ([str_isUseCustomENC isEqual:@"1"])
    {
        
        ME_video_enc_param_t stEnc;
        UGo_get_config(ME_VIDEO_ENC_CFG_MODULE_ID, &stEnc, 0);
        
        //获取用户自定义的视频参数
        NSString *str_uHeightEnc = [UserDefaultManager GetKeyChain:DATA_STORE_UD_UHEIGHT_ENC];
        NSString *str_uWidthEnc = [UserDefaultManager GetKeyChain:DATA_STORE_UD_UWIDTH_ENC];
        NSString *str_uStartBitRateEnc = [UserDefaultManager GetKeyChain:DATA_STORE_UD_USTARTBITRATE_ENC];
        NSString *str_uMaxFrameRateEnc = [UserDefaultManager GetKeyChain:DATA_STORE_UD_UMAXFRAMERATE_ENC];
        
        
        //设置用户自定义的视频参数
        stEnc.usWidth = [str_uHeightEnc intValue];//组件要求宽高倒置 add by WLS 2015.10.26
        stEnc.usHeight = [str_uWidthEnc intValue];// add by WLS 2015.10.26
        stEnc.usStartBitrate = [str_uStartBitRateEnc intValue];
        stEnc.ucMaxFramerate = [str_uMaxFrameRateEnc intValue];
        //         stEnc.ucComplexity = 3;
        
        
        UGo_set_config(ME_VIDEO_ENC_CFG_MODULE_ID, &stEnc, 0);
        
    }
    
    //**********************如果用户启用自定义视频参数则优先设置自定义参数**********************//
    //注释 add by WLS 20150928  已经取消该接口设置
    //    //***********************摄像头取景分辨率设置**********************//
    //
    //
    //        NSString *str_isUseCustomCamera = [UserDefaultManager GetKeyChain:DATA_STORE_UD_ISUSECUSTOME_CAMERA];//是否启用自定义参数
    //
    //
    //        if ([str_isUseCustomCamera isEqual:@"1"])
    //        {
    //
    //            ME_video_camera_param_t stAttr_camera;
    //
    //            UGo_video_get_camera_state(&stAttr_camera);
    //
    //
    //            //获取摄像头取样的视频参数
    //            NSString *str_uWidthCamera = [UserDefaultManager GetKeyChain:DATA_STORE_UD_UWIDTH_CAMERA];
    //            NSString *str_uHeightCamera = [UserDefaultManager GetKeyChain:DATA_STORE_UD_UHEIGHT_CAMERA];
    ////            NSString *str_uRateAngleCamera = [UserDefaultManager GetKeyChain:DATA_STORE_UD_UROTATEANGLE_CAMERA];
    ////            NSString *str_uCameraIdx = [UserDefaultManager GetKeyChain:DATA_STORE_UD_UCAMERAIDX_CAMERA];
    //            NSString *str_uMaxFrameRateCamera = [UserDefaultManager GetKeyChain:DATA_STORE_UD_UMAXFRAMERATE_CAMERA];
    //
    //            //设置摄像头取样的视频参数，高宽互相换提高清晰度。
    //            stAttr_camera.usHeight = [str_uWidthCamera intValue];
    //            stAttr_camera.usWidth =  [str_uHeightCamera intValue];
    ////            stAttr_camera.usRotateAngle = [str_uRateAngleCamera intValue];
    ////            stAttr_camera.usCameraIdx = [str_uCameraIdx intValue];
    //            stAttr_camera.ucMaxFps = [str_uMaxFrameRateCamera intValue];
    //
    //
    //
    //           UGo_video_set_camera(&stAttr_camera);
    //
    //        }
    //
    //
    //
    //    //**********************摄像头取景分辨率设置**********************//
    //*****************************视频编码**********************************//
    
    
    
    
    
    //*****************************视频解码**********************************//
    //**********************如果用户启用自定义视频解码参数则优先设置自定义参数**********************//
    
    NSString *str_isUseCustomDEC = [UserDefaultManager GetKeyChain:DATA_STORE_UD_ISUSECUSTOME_DEC];//是否启用自定义参数
    
    if ([str_isUseCustomDEC isEqual:@"1"])
    {
        
        ME_video_dec_info_t stDec;
        UGo_get_config(ME_VIDEO_DEC_CFG_MODULE_ID, &stDec, 0);
        
        //获取用户自定义的视频参数
        NSString *str_uHeightDec = [UserDefaultManager GetKeyChain:DATA_STORE_UD_UHEIGHT_DEC];
        NSString *str_uWidthDec = [UserDefaultManager GetKeyChain:DATA_STORE_UD_UWIDTH_DEC];
        //            NSString *str_uMaxFrameRateDec = [UserDefaultManager GetKeyChain:DATA_STORE_UD_UMAXFRAMERATE_DEC]; // 注释 by pbf 2015.09.11
        
        
        //设置用户自定义的视频参数
        //            stDec.usWidth = 640; //组件要求写死640 add by pbf 2015.8.20 注释 by pbf 2015.09.15 新版本组件不用填写宽高
        //            stDec.usHeight = 640; // add by pbf 2015.8.20
        //            stDec.ucMaxFramerate = [str_uMaxFrameRateDec intValue]; // 注释 by pbf 2015.09.11
        
        UGo_set_config(ME_VIDEO_DEC_CFG_MODULE_ID, &stDec, 0);
        
    }
    
    //**********************如果用户启用自定义视频解码参数则优先设置自定义参数**********************//
    //*****************************视频解码**********************************//
    
    
    
}


/**
 *  设置视频信息 add by WLS 2015.10.26
 *
 *  @param Local_pImage1  接收端(本地)窗口
 *  @param Remote_pImage2 发送端(远程)窗口
 *
 */
- (int) initCameraConfig:(NSOpenGLView *) Local_pImage1 withRemoteView:(NSOpenGLView *) Remote_pImage2
{
    // 1、保证两个窗口都不为空，如果有一个为空则下发错误码
    if ((!Local_pImage1) || (!Remote_pImage2)) {
        return NO;
    }
    
#ifdef VIDEO_ENABLED
    ME_video_render_param_t stVideoRenderConfig;
    
    ME_CTRL_cfg_t stTGoConfig;
    
#ifdef UGO_ENABLED
    UGo_get_config(ME_VIDEO_RENDER_CFG_MODULE_ID, &stVideoRenderConfig, 0);
#else
    TGo_get_config(&stTGoConfig, &stVIEConfig, NULL, NULL);
#endif
    
    
    //判断系统位数 add by WLS 20150930
//    if (sizeof(long) == 8) {
//        //64位系统
//        NSString *str=@"definition";
//        char *profile=(char*)[str UTF8String];
//        
//        NSString *str1=@"high";
//        char *profile1=(char*)[str1 UTF8String];
//        
//        UGo_video_preset(profile,profile1);
//        
//    }else{
//        //32位系统
//        NSString *str=@"definition";
//        char *profile=(char*)[str UTF8String];
//        
//        NSString *str1=@"low";
//        char *profile1=(char*)[str1 UTF8String];
//        
//        UGo_video_preset(profile,profile1);
//        
//    }
    
    
    //******************************************************视频编码**************************************************************//
    //**********************如果用户启用自定义视频参数则优先设置自定义参数**********************//
    //是否启用自定义参数
    NSString *str_isUseCustomENC = [UserDefaultManager GetKeyChain:DATA_STORE_UD_ISUSECUSTOME_ENC];
    if ([str_isUseCustomENC isEqual:@"1"])
    {
        
        
    }else {
        ME_video_enc_param_t stEnc;
        UGo_get_config(ME_VIDEO_ENC_CFG_MODULE_ID, &stEnc, 0);
        stEnc.usWidth = 240;//组件要求宽高倒置 add by WLS 2015.10.26
        stEnc.usHeight = 320;// add by WLS 2015.10.26
        stEnc.usStartBitrate = 200;
        stEnc.ucMaxFramerate = 15;
        
        UGo_set_config(ME_VIDEO_ENC_CFG_MODULE_ID, &stEnc, 0);
        
    }
    
    //***************************视频解码******************************//
    //**********************如果用户启用自定义视频解码参数则优先设置自定义参数**********************//
    //是否启用自定义参数
    NSString *str_isUseCustomDEC = [UserDefaultManager GetKeyChain:DATA_STORE_UD_ISUSECUSTOME_DEC];
    if ([str_isUseCustomDEC isEqual:@"1"])
    {
        
        
        
    } else{
        ME_video_dec_info_t stDec;
        UGo_get_config(ME_VIDEO_DEC_CFG_MODULE_ID, &stDec, 0);
        //        stDec.usWidth = 640;//组件要求写死640 add by pbf 2015.8.20 注释 by pbf 2015.09.15 新版本组件不用填写宽高
        //        stDec.usHeight = 640;//组件要求写死640 add by pbf 2015.8.20
        UGo_set_config(ME_VIDEO_DEC_CFG_MODULE_ID, &stDec, 0);
    }
    //**********************如果用户启用自定义视频解码参数则优先设置自定义参数**********************//
    //***************************视频解码******************************//
    
    
    
    
    
    //**********************本端视频显示窗口，对端视频显示窗口**********************//
    
    stVideoRenderConfig.pWindowRemote = Remote_pImage2;
    stVideoRenderConfig.pWindowLocal = Local_pImage1;
    
    //renderMode 如果为1 则设置显示的图像为视图的宽高，为了防止视图压缩拉伸。  如果为0，则默认全屏显示. add by WLS 20150929
    stVideoRenderConfig.renderMode = 0;
    
    if (Remote_pImage2) {
        stVideoRenderConfig.renderMode = 1;
        stVideoRenderConfig.remoteHeight = Remote_pImage2.frame.size.height;
        stVideoRenderConfig.remoteWidth = Remote_pImage2.frame.size.width;
    }
    
    //**********************本端视频显示窗口，对端视频显示窗口**********************//
    
    stTGoConfig.ucVideoEnable = true;
    
#ifdef UGO_ENABLED
    UGo_set_config(ME_VIDEO_RENDER_CFG_MODULE_ID, &stVideoRenderConfig, 0);
    
    ugo_cfg_t ucfg;
    
    UGo_get_config(UGO_CFG_PARAM_MODULE_ID, &ucfg, 0);
    
    //add by wenqinglin
    UGo_video_start(9);
    if (ucfg.video_enabled==2)
    {
        UGo_video_start(7);
    }
    
    
    
    //    [self switchCameraDevice:1];//默认设置为前置摄像头，解决本地推送视频呼叫无法预览问题。
#else
    TGo_set_config(&stTGoConfig, &stVIEConfig, NULL, NULL);
#endif
    
#endif
    
    
    return YES;
}

// 拨打视频电话
- (BOOL) callPhoneNumberForVideo:(NSString *) strUid withPN:(NSString *) strPhoneNumber withCallMode:(int)callMode andUserdata:(NSString *)callerData
{
    BOOL bRet = NO;
    int nRes = -1;
    g_CallMode = callMode;
#if !(TARGET_IPHONE_SIMULATOR)
#if defined (UGO_ENABLED)
    
    dispatch_async(dispatch_get_current_queue(), ^{
        NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
        
        ugo_call_dialing_para_t callpm;
        callpm.call_mode = eUGo_CM_FREE;//g_CallMode;
        callpm.video_enabled = TRUE; //当需要拨打视频电话时，此开关需打开，否则默认为拨打语音电话。
        
        if ([strPhoneNumber length] == 0 ) {
            if ([strUid length] == 0)
                return;
            //            UGo_call_dialing([strUid UTF8String], "");
            strcpy(callpm.tuid, [strUid UTF8String]);
            strcpy(callpm.tphone, "");
        } else if ([strUid length] == 0){
            //            UGo_call_dialing("", [strPhoneNumber UTF8String]);
            strcpy(callpm.tuid, "");
            strcpy(callpm.tphone, [strPhoneNumber UTF8String]);
        } else {
            //            UGo_call_dialing([strUid UTF8String], [strPhoneNumber UTF8String]);
            strcpy(callpm.tuid, [strUid UTF8String]);
            strcpy(callpm.tphone, [strPhoneNumber UTF8String]);
        }
        
        //        strcpy(callpm.name, "");     //拨打电话其它未用到字段赋默认值 add by baige 2015年02月02日
        
        
        if ([callerData length] == 0)
        {
            strcpy(callpm.user_data, "");
        }
        else
        {
            strcpy(callpm.user_data, [callerData UTF8String]);
        }
        
        strcpy(callpm.user_data, "");
        
        callpm.calltype = 0;
        strcpy(callpm.reserved, "");
        
        int nRes = UGo_call_dialing(&callpm);
        UXLogNotice(@"UGo_call_dialing: %@ %@", strPhoneNumber, strUid);
        if (_callRequest == nil) {
            _callRequest = [[UCSCallingHttpRequest alloc] init];
        }
        [_callRequest emodelReauest:strPhoneNumber caller:[UserDefaultManager GetUserID]];
        [pool drain];
    });
    
    
    
    
    //    call_dialing_pm_t callpm;
    //    callpm.call_mode = callMode;//g_CallMode;
    //    strcpy(callpm.uid, [strUid UTF8String]);
    //    strcpy(callpm.uphone, [strPhoneNumber UTF8String]);
    //    nRes = UGo_call_dialing(&callpm);
#else
    //    nRes = TGo_request_call([strUid UTF8String]);
#endif
#endif
    if (0 == nRes)
    {
        bRet = YES;
    }
    return bRet;
}

// 拨打视频电话
- (BOOL) callPhoneNumberForVideo2:(NSString *) strUid withPN:(NSString *) strPhoneNumber withCallMode:(int)callMode andUserdata:(NSString *)callerData
{
    BOOL bRet = NO;
    int nRes = -1;
    g_CallMode = callMode;
#if !(TARGET_IPHONE_SIMULATOR)
#if defined (UGO_ENABLED)
    
    
    dispatch_async(dispatch_get_current_queue(), ^{
        NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
        
        ugo_call_dialing_para_t callpm;
        callpm.call_mode = eUGo_CM_FREE;//g_CallMode;
        callpm.video_enabled = TRUE; //当需要拨打视频电话时，此开关需打开，否则默认为拨打语音电话。
        
        if ([strPhoneNumber length] == 0 ) {
            if ([strUid length] == 0)
                return;
            //            UGo_call_dialing([strUid UTF8String], "");
            strcpy(callpm.tuid, [strUid UTF8String]);
            strcpy(callpm.tphone, "");
        } else if ([strUid length] == 0){
            //            UGo_call_dialing("", [strPhoneNumber UTF8String]);
            strcpy(callpm.tuid, "");
            strcpy(callpm.tphone, [strPhoneNumber UTF8String]);
        } else {
            //            UGo_call_dialing([strUid UTF8String], [strPhoneNumber UTF8String]);
            strcpy(callpm.tuid, [strUid UTF8String]);
            strcpy(callpm.tphone, [strPhoneNumber UTF8String]);
        }
        
        //        strcpy(callpm.name, "");     //拨打电话其它未用到字段赋默认值 add by baige 2015年02月02日
        
        
        if ([callerData length] == 0)
        {
            strcpy(callpm.user_data, "");
        }
        else
        {
            strcpy(callpm.user_data, [callerData UTF8String]);
        }
        
        callpm.calltype=0;
        strcpy(callpm.reserved, "");
        
        int nRes = UGo_call_dialing(&callpm);
        UXLogNotice(@"UGo_call_dialing: %@ %@", strPhoneNumber, strUid);
        if (_callRequest == nil) {
            _callRequest = [[UCSCallingHttpRequest alloc] init];
        }
        [_callRequest emodelReauest:strPhoneNumber caller:[UserDefaultManager GetUserID]];
        [pool drain];
    });
    
    
    
    
    //    call_dialing_pm_t callpm;
    //    callpm.call_mode = callMode;//g_CallMode;
    //    strcpy(callpm.uid, [strUid UTF8String]);
    //    strcpy(callpm.uphone, [strPhoneNumber UTF8String]);
    //    nRes = UGo_call_dialing(&callpm);
#else
    //    nRes = TGo_request_call([strUid UTF8String]);
#endif
#endif
    if (0 == nRes)
    {
        bRet = YES;
    }
    return bRet;
}


//获取摄像头个数
- (int) getCameraNum
{
    //    NSMutableDictionary *dic_infoCamera = [[[NSMutableDictionary alloc]init] autorelease];
    int i = -1;
#ifdef VIDEO_ENABLED
    
    
#ifdef UGO_ENABLED
    
    
    
#if !(TARGET_IPHONE_SIMULATOR)
    
    i= UGo_video_get_camera_count();
    
#endif
    
    
    
#endif
    
    
#endif
    
    return i;
}


// return 0 成功 -1 失败 前后摄像头切换
- (int) switchCameraDevice:(int)CameraIndex
{
    int nRet= -1;
    int ret = -1;
#ifdef VIDEO_ENABLED
    
    
#ifdef UGO_ENABLED
    
    
#if !(TARGET_IPHONE_SIMULATOR)
    [[SDKLogManager instance]saveSDKLogInfo:@"开始切换摄像头" withDetail:@"开始切换摄像头"];

    ME_video_camera_param_t info_camera;
    
    ret = UGo_video_get_camera_state(&info_camera);
    info_camera.usCameraIdx = CameraIndex;
    
    //    //当视频参数启用自定义设置时分辨率需要跟随自定义
    //    if ([[UserDefaultManag  er GetKeyChain:DATA_STORE_UD_ISUSECUSTOME_ENC] isEqual:@"1"])
    //    {
    //
    //        NSString *str_uHeightEnc = [UserDefaultManager GetKeyChain:DATA_STORE_UD_UHEIGHT_ENC];
    //        NSString *str_uWidthEnc = [UserDefaultManager GetKeyChain:DATA_STORE_UD_UWIDTH_ENC];
    //        NSString *str_uMaxFrameRateEnc = [UserDefaultManager GetKeyChain:DATA_STORE_UD_UMAXFRAMERATE_ENC];
    //        info_camera.usHeight = [str_uHeightEnc integerValue];
    //        info_camera.usWidth = [str_uWidthEnc integerValue];
    //        info_camera.ucMaxFps = [str_uMaxFrameRateEnc integerValue];
    //
    //
    //    }
    
    
    
    nRet = UGo_video_set_camera(&info_camera);
    
    
    [[SDKLogManager instance]saveSDKLogInfo:@"切换摄像头" withDetail:[NSString stringWithFormat:@"nRet: %d   ret : %d",nRet,ret]];
    
#endif
    
#endif
    
#endif
    return nRet;
    
}


/**
 *  切换视频模式：发送、接收、正常模式
 *
 *  @param type         CAMERA_RECEIVE : 只接收视频数据（只能接收到对方的视频）
 CAMERA_SEND : 只发送视频数据（只让对方看到我的视频）
 CAMERA_NORMAL : send receive preview
 *
 *  @return 0 成功 -1 失败
 */
- (int) switchVideoMode:(KCTCameraType)type
{
    int nRet = -1;
#ifdef VIDEO_ENABLED
    
    
#ifdef UGO_ENABLED
    
    
#if !(TARGET_IPHONE_SIMULATOR)
    
    //现将所有的模式打开， 而后根据type值选择关闭相应的模式。 add by WLS 20151009
    nRet = UGo_video_start(0x1F);
    
    if (type == CAMERA_SEND) {
        //发送模式 ， 在所有模式的开启的情况下 关闭接收模式 和 对端视图显示 add by WLS 20151009
        
        //                    nRet = UGo_video_stop(0x04);
        //                    nRet = UGo_video_stop(0x10);
        nRet = UGo_video_stop(0x14);
        
    }else if (type == CAMERA_RECEIVE){
        //接收模式 ， 在所有模式的开启的情况下 关闭发送模式 和 本地视图显示 add by WLS  20151009
        //                    nRet = UGo_video_stop(0x01);
        //                    nRet = UGo_video_stop(0x02);
        //                    nRet = UGo_video_stop(0x08);
        nRet = UGo_video_stop(0x0B);
        
        
    }
    
    //                // 1、开启预览
    //                if (type == CAMERA_SEND) {
    //                    UGo_video_stop(7);
    //
    //                    nRet = UGo_video_start(1); //UGo_video_start: 参数为1:采集图像，但不发送视频流  参数为2:发送视频流 参数为4:接收视频流 参数为7:采集图像、发送视频流、接收视频流
    //                    nRet = UGo_video_start(2);
    //                } else if (type == CAMERA_RECEIVE) {
    //                    UGo_video_stop(7);
    //
    //                    nRet = UGo_video_start(4);
    //                } else if (type == CAMERA_NORMAL) {
    //                    nRet = UGo_video_start(7);
    //                }
    
#endif
    
#endif
    
#endif
    
    return nRet;
}


/**
 *  自定义视频编码和解码参数 add by WLS 2015.10.26
 *
 *  @param ucsVideoEncAttr 编码参数
 *  @param ucsVideoDecAttr 解码参数
 */
- (void)setVideoAttr:(KCTVideoEncAttr*)ucsVideoEncAttr andVideoDecAttr:(KCTVideoDecAttr*)ucsVideoDecAttr
{
    /******************************************设置编码参数*******************************************************/
    if (ucsVideoEncAttr) {
        //是否使用自定义编码视频参数
        BOOL ud_isUseCustomEnc = ucsVideoEncAttr.isUseCustomEnc;
        //分辨率高
        NSInteger ud_HeightEnc = ucsVideoEncAttr.uHeight;
        //分辨率宽
        NSInteger ud_WidthEnc = ucsVideoEncAttr.uWidth;
        //开始码率
        NSInteger ud_StartBitrateEnc = ucsVideoEncAttr.uStartBitrate;
        //最大帧率
        NSInteger uMaxFramerateEnc = ucsVideoEncAttr.uMaxFramerate;
        
        [UserDefaultManager SetKeyChain:[NSString stringWithFormat:@"%d",ud_isUseCustomEnc] key:DATA_STORE_UD_ISUSECUSTOME_ENC];
        [UserDefaultManager SetKeyChain:[NSString stringWithFormat:@"%d",ud_HeightEnc] key:DATA_STORE_UD_UHEIGHT_ENC];
        [UserDefaultManager SetKeyChain:[NSString stringWithFormat:@"%d",ud_WidthEnc] key:DATA_STORE_UD_UWIDTH_ENC];
        [UserDefaultManager SetKeyChain:[NSString stringWithFormat:@"%d",ud_StartBitrateEnc] key:DATA_STORE_UD_USTARTBITRATE_ENC];
        [UserDefaultManager SetKeyChain:[NSString stringWithFormat:@"%d",uMaxFramerateEnc] key:DATA_STORE_UD_UMAXFRAMERATE_ENC];
    }
    
    /******************************************设置解码参数*******************************************************/
    if (ucsVideoDecAttr) {
        //是否使用自定义编码视频参数
        BOOL ud_isUseCustomDec = ucsVideoDecAttr.isUseCustomDec;
        //分辨率高
        NSInteger ud_HeightDec = ucsVideoDecAttr.uHeight;
        //分辨率宽
        NSInteger ud_WidthDec = ucsVideoDecAttr.uWidth;
        
        [UserDefaultManager SetKeyChain:[NSString stringWithFormat:@"%d",ud_isUseCustomDec] key:DATA_STORE_UD_ISUSECUSTOME_DEC];
        [UserDefaultManager SetKeyChain:[NSString stringWithFormat:@"%d",ud_HeightDec] key:DATA_STORE_UD_UHEIGHT_DEC];
        [UserDefaultManager SetKeyChain:[NSString stringWithFormat:@"%d",ud_WidthDec] key:DATA_STORE_UD_UWIDTH_DEC];
    }
}


//#pragma mark - 自定义摄像头取样的视频参数 add by WLS 20150624 注释 by pbf 2015.09.14
//- (BOOL)setVideoCameraAttr:(KCTCameraAttr *)ucsCameraAttr{
//
//    //摄像头的采样默认参数
//    NSDictionary * defaultCameraState = [UserDefaultManager GetLocalDataObject:DATA_STORE_UD_UDEFAULTSTATE_CAMERA];
//
//    if (defaultCameraState) {
//        //没有设置的参数恢复默认值
//        if (ucsCameraAttr.uHeight == 0) {
//            ucsCameraAttr.uHeight = [[defaultCameraState objectForKey:DATA_STORE_UD_UHEIGHT_CAMERA] integerValue];
//        }
//        if (ucsCameraAttr.uWidth == 0) {
//            ucsCameraAttr.uWidth = [[defaultCameraState objectForKey:DATA_STORE_UD_UWIDTH_CAMERA] integerValue];
//        }
////        if (ucsCameraAttr.uRotateAngle == 0) {
////            ucsCameraAttr.uRotateAngle = [[defaultCameraState objectForKey:DATA_STORE_UD_UROTATEANGLE_CAMERA] integerValue];
////        }
////        if (ucsCameraAttr.uCameraIdx == 0) {
////            ucsCameraAttr.uCameraIdx = [[defaultCameraState objectForKey:DATA_STORE_UD_UCAMERAIDX_CAMERA] integerValue];
////        }
//        if (ucsCameraAttr.uMaxFramerate == 0) {
//            ucsCameraAttr.uMaxFramerate = [[defaultCameraState objectForKey:DATA_STORE_UD_UMAXFRAMERATE_CAMERA] integerValue];
//        }
//
//    }
//
//
//
//
//    //是否使用自定义摄像头采样参数
//    [UserDefaultManager SetKeyChain:[NSString stringWithFormat:@"%d",ucsCameraAttr.isUseCustomDec] key:DATA_STORE_UD_ISUSECUSTOME_CAMERA];
//    //分辨率 高
//    [UserDefaultManager SetKeyChain:[NSString stringWithFormat:@"%d",ucsCameraAttr.uHeight] key:DATA_STORE_UD_UHEIGHT_CAMERA];
//    //分辨率 宽
//    [UserDefaultManager SetKeyChain:[NSString stringWithFormat:@"%d",ucsCameraAttr.uWidth] key:DATA_STORE_UD_UWIDTH_CAMERA];
////    //采样显示的角度
////    [UserDefaultManager SetKeyChain:[NSString stringWithFormat:@"%d",ucsCameraAttr.uRotateAngle] key:DATA_STORE_UD_UROTATEANGLE_CAMERA];
////    //采样摄像头索引
////    [UserDefaultManager SetKeyChain:[NSString stringWithFormat:@"%d",ucsCameraAttr.uCameraIdx] key:DATA_STORE_UD_UCAMERAIDX_CAMERA];
//    //帧率
//    [UserDefaultManager SetKeyChain:[NSString stringWithFormat:@"%d",ucsCameraAttr.uMaxFramerate] key:DATA_STORE_UD_UMAXFRAMERATE_CAMERA];
//
//
//
//    if (![[UCSTCPManager instance] isConnected]) {
//        //如果tcp不处于链接状态 不需要设置组件视频采样参数
//    }else{
//
//        ME_video_camera_param_t stAttr_camera;
//
//        UGo_video_get_camera_state(&stAttr_camera);
//        if (ucsCameraAttr.isUseCustomDec) {
//            //自定义设置
//
//            //设置摄像头取样的视频参数，高宽互相换提高清晰度。
//            stAttr_camera.usHeight = ucsCameraAttr.uWidth;
//            stAttr_camera.usWidth =  ucsCameraAttr.uHeight;
////            stAttr_camera.usRotateAngle = ucsCameraAttr.uRotateAngle;
////            stAttr_camera.usCameraIdx = ucsCameraAttr.uCameraIdx;
//            stAttr_camera.ucMaxFps = ucsCameraAttr.uMaxFramerate;
//
//
//        }else{
//            //恢复默认
//
//            if (defaultCameraState == nil) {
//                //默认值没有
//                return NO;
//            }
//
//            //设置摄像头取样的视频参数，高宽互相换提高清晰度。
//            stAttr_camera.usHeight = [[defaultCameraState objectForKey:DATA_STORE_UD_UWIDTH_CAMERA] integerValue];
//            stAttr_camera.usWidth =  [[defaultCameraState objectForKey:DATA_STORE_UD_UHEIGHT_CAMERA] integerValue];
////            stAttr_camera.usRotateAngle = [[defaultCameraState objectForKey:DATA_STORE_UD_UROTATEANGLE_CAMERA] integerValue];
////            stAttr_camera.usCameraIdx = [[defaultCameraState objectForKey:DATA_STORE_UD_UCAMERAIDX_CAMERA] integerValue];
//            stAttr_camera.ucMaxFps = [[defaultCameraState objectForKey:DATA_STORE_UD_UMAXFRAMERATE_CAMERA] integerValue];
//
//
//        }
//        UGo_video_set_camera(&stAttr_camera);
//
//
//
//    }
//
//
//    return YES;
//}


//设置摄像头旋转角度0 90 180 270 本地图像旋转角度  对端图像旋转角度
- (BOOL)setCameraRotateAngleWithSend:(unsigned int)sendRotateAngle andRecivied:(unsigned int)reciviedRotateAngle{
    
    
    int nRes = UGo_video_set_send_recive_rotation(sendRotateAngle,reciviedRotateAngle);
    
    if (nRes != 0) {
        return NO;
    }
    return YES;
    
    
}


/**
 * 视频截图
 * islocal: 0 是远端截图 1 是本地截图。
 *
 */
-(int)screenshotForVideo:(int)islocal withFileName:(NSString*)filename withSavePath:(NSString*)savePath
{
    
    NSString *cameraCaptureFilePath = [[savePath stringByAppendingPathComponent:filename] stringByAppendingString:@".bmp"];
    
    [UserDefaultManager SetLocalDataString:cameraCaptureFilePath key:DATA_STORE_VIDEOCAPTRUE_FILEPATH];
    
    int result = -1;
    
#if !(TARGET_IPHONE_SIMULATOR)
    
    result  = UGo_screenshot_start(1,RGB24);
    
#endif
    
    
    
    return result;
    
}


//视频截图回调函数
static void screen_shot_cb(const ucpaas_uint8* dst_argb, int dst_stride, int width, int height, int islocal, ScreenType screen_type)
{
    NSFileManager *fileManager = [NSFileManager defaultManager];
    
    NSString *filePath =  [UserDefaultManager GetLocalDataString:DATA_STORE_VIDEOCAPTRUE_FILEPATH];;
    
    //查找文件，如果不存在，就创建一个文件
    if (![fileManager fileExistsAtPath:filePath]) {
        [fileManager createFileAtPath:filePath contents:nil attributes:nil];
    }
    const char * outfile =[filePath UTF8String];
    
    // filePath	NSPathStore2 *	@"/var/mobile/Containers/Data/Application/F581A544-FF46-4BE3-A252-53B8EE7143D6/Documents/UserNameAndPassWord.txt"	0x00000001701c0870
    bmp_write((unsigned char*)dst_argb, width, height, islocal,(char*)outfile);
    
    //    if (filePath==nil) {
    //        return;
    //    }
    
    id delegate = [SoftphoneManager instance].delegate;
    if (delegate)
    {
        
        if ([delegate respondsToSelector:@selector(onScreenshotForVideo:)]) {
            [delegate onScreenshotForVideo:filePath];
        }
    }
    
}


//视频截图写入函数
static int bmp_write(unsigned char *image, int xsize, int ysize, int islocal, char *filename)
{
    unsigned char header[54] = {
        0x42, 0x4d, 0, 0, 0, 0, 0, 0, 0, 0,
        54, 0, 0, 0, 40, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 24, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0
    };
    
    long file_size = (long)xsize * (long)ysize * 3 + 54;
    header[2] = (unsigned char)(file_size & 0x000000ff);
    header[3] = (file_size >> 8) & 0x000000ff;
    header[4] = (file_size >> 16) & 0x000000ff;
    header[5] = (file_size >> 24) & 0x000000ff;
    
    long width = xsize;
    header[18] = width & 0x000000ff;
    header[19] = (width >> 8) & 0x000000ff;
    header[20] = (width >> 16) & 0x000000ff;
    header[21] = (width >> 24) & 0x000000ff;
    
    long height = ysize;
    header[22] = height & 0x000000ff;
    header[23] = (height >> 8) & 0x000000ff;
    header[24] = (height >> 16) & 0x000000ff;
    header[25] = (height >> 24) & 0x000000ff;
    
    char fname_bmp[128];
    sprintf(fname_bmp, "%s_%d", filename, islocal);
    
    FILE *fp;
    if (!(fp = fopen(filename, "wb")))
        return -1;
    
    printf("filename=%s\n",filename);
    fwrite(header, sizeof(unsigned char), 54, fp);
    fwrite(image, sizeof(unsigned char), (size_t)(long)xsize * ysize * 3, fp);
    
    fclose(fp);
    
    
    
    
    return 0;
}


#pragma mark - --------------------------其他能力--------------------------

/**
 @author WLS, 16-02-01 15:02:17
 
 开关日志存储
 */
- (BOOL) setMediaEngineLogStatus:(BOOL) bOpen{
    
    [UserDefaultManager SetLocalDataBoolen:bOpen key:DATA_STORE_OPENUOGLOG];
    
    /**
     @author WLS, 16-02-01 15:02:27
     
     设置打印日志，并且存储到本地
     */
    ugo_log_trace_pm_t tracepm;
    memset(&tracepm, 0, sizeof(tracepm));
    tracepm.level = bOpen==YES?kME_TraceDefault:kME_TraceNone;
    if (tracepm.level == kME_TraceDefault) {
        NSString * file = [SDKLogManager getLogFileWithName:@"MediaEngineLog"];
        memcpy(tracepm.filepath, [file cStringUsingEncoding:NSASCIIStringEncoding], [file length]);
    }
    int ret = UGo_set_log_file(&tracepm);
    
    if (ret == 0) {
        return YES;
    }else{
        return NO;
    }
    
}


// 获取电话组件版本号
- (NSString *) getSoftphoneVersion
{
    NSString *strVersion = @"1.0.0";
    
#if !(TARGET_IPHONE_SIMULATOR)
    char tgo_version[21] = {0};
    
    UGo_get_version(tgo_version);
    
    strVersion = [NSString stringWithFormat:@"%@",[NSString stringWithUTF8String:tgo_version]];
    
#endif
    
    return strVersion;
}


/**
 @author WLS, 16-01-08 10:01:16
 
 添加录音功能，通话过程中录音
 
 @param file 文件路径
 @return 返回操作结果
 */
- (BOOL)recordStartWithFileName:(NSString *)file{
    
    
    media_file_record_pm_t  fileRecord;
    fileRecord.iMode = eME_RECORD_ALL;
    fileRecord.iFileFormat = kME_FileFormatWavFile;
    for (int i = 0; i<256; i++) {
        fileRecord.filepath[i] = 0;
    }
    memcpy(fileRecord.filepath, [file cStringUsingEncoding:NSASCIIStringEncoding], [file length]);
    
    [[SDKLogManager instance]saveSDKLogInfo:@"录音文件地址" withDetail:[NSString stringWithFormat:@"filepath : %s---file : %s",fileRecord.filepath,[file cStringUsingEncoding:NSASCIIStringEncoding]]];
    
    int ret = UGo_record_start(&fileRecord);
    
    if (ret == -1) {
        return NO;
    }
    
    return YES;
}

/**
 @author WLS, 16-01-08 11:01:21
 
 停止录音
 
 @return 返回操作结果
 */
- (BOOL)stopRecord{
    
    int ret = UGo_record_stop();
    
    if (ret == -1) {
        return NO;
    }
    
    return YES;
}


-(NSString*)GetCurrentPhoneNetType{
    
    PhoneNetType currentNetType = [PublicFunc GetCurrentPhoneNetType];
    if (PNT_WIFI == currentNetType)
    {
        return @"wifi";
        
    }else if(PNT_2G == currentNetType)
    {
        return @"2g";
        
    }else if(PNT_3G == currentNetType){
        
        return @"3g";
        
    } else if (PNT_4G == currentNetType){
        //加上4g 的区分 add by WLS 20151013
        return @"4g";
        
    }
    
    return @"UNKNOWN";
}



//当从cps动态服务器获取参数并保存完成后调用 setConfigByCPS;
- (void)setConfigByCPS
{
    
    
    /*******************************动态策略上报参数配置***************************************/
    
    /**
     @author WLS, 16-05-20 12:05:10
     
     设置ICE
     */
    NSString *iceEnForCPS = [UserDefaultManager  GetKeyChain:DATA_STORE_CPS_ICEENABLE];
    iceEnForCPS = [PublicFunc ucsCheckNullString:iceEnForCPS] ? iceEnForCPS: @"0";
    
    ugo_cfg_ice_t ice_cfg;
    memset(&ice_cfg, 0, sizeof(ice_cfg));
    ice_cfg.ice_enabled = [iceEnForCPS intValue]==1?TRUE:FALSE;
//    ice_cfg.ice_enabled = FALSE;
    if (ice_cfg.ice_enabled) {
        
        NSArray *arraystunList = [UserDefaultManager GetLocalDataObject:DATA_STORE_STUNLIST];
        const char *stunIP = [arraystunList count]>0?[arraystunList[0] UTF8String]:[[NSString stringWithFormat:@"%@",@"stunserver.org"] UTF8String];
        
        strcpy(ice_cfg.stun_server, stunIP);//stun.softjoys.com:3478
    }
    UGo_set_config(UGO_CFG_ICE_MODULE_ID,(void*)&ice_cfg, 0);//Warning: Libinfo call to mDNSResponder on main thread.   add by kucky 20141225.
    
    
    
    /**
     @author WLS, 16-05-20 12:05:09
     
     设置fec配置参数，开启fec功能,设置emodel配置参数，开启emodel获取功能
     */
    ME_CTRL_cfg_t ME_config;
    UGo_get_config(ME_CTRL_CFG_MODULE_ID, &ME_config, 0);
    
    //ipv6
    //ME_config.ucIPv6Enable = TRUE;
    //FEC 开关
    NSString *fecEnForCPS = [UserDefaultManager  GetKeyChain:DATA_STORE_CPS_AUDIOFEC];
    fecEnForCPS = [PublicFunc ucsCheckNullString:fecEnForCPS] ? fecEnForCPS : @"1";
    ME_config.ucFecEnable = [fecEnForCPS intValue]==1?TRUE:FALSE;
    
    //语音质量参数获取开关
    NSString *emodelForCPS = [UserDefaultManager  GetKeyChain:DATA_STORE_CPS_VQMENABLE];
    emodelForCPS = [PublicFunc ucsCheckNullString:emodelForCPS] ? emodelForCPS : @"1";
    ME_config.ucEmodelEnable = [emodelForCPS intValue];
    
    //PRTP
    NSString *prtpEnForCPS = [UserDefaultManager  GetKeyChain:DATA_STORE_CPS_PRTPENABLE];
    prtpEnForCPS = [PublicFunc ucsCheckNullString:prtpEnForCPS] ? prtpEnForCPS : @"1";
    ME_config.ucRealTimeType = [prtpEnForCPS intValue];
    UGo_set_config(ME_CTRL_CFG_MODULE_ID, &ME_config, 0);
    
    /*******************************动态策略上报参数配置***************************************/
    
    
    /*******************************动态策略视频码率参数配置***************************************/
    
//    NSString * h264enableForCPS = @"1";//[UserDefaultManager  GetKeyChain:DATA_STORE_CPS_H264ENABLE];
//    NSString * vp8enableForCPS =  @"0";//[UserDefaultManager  GetKeyChain:DATA_STORE_CPS_VP8ENABLE];
    NSString * h264enableForCPS = [UserDefaultManager  GetKeyChain:DATA_STORE_CPS_H264ENABLE];
    NSString * vp8enableForCPS =  [UserDefaultManager  GetKeyChain:DATA_STORE_CPS_VP8ENABLE];
    NSString * startbitrateForCPS = [UserDefaultManager  GetKeyChain:DATA_STORE_CPS_STARTBITRATE];
    NSString * minbitrateForCPS = [UserDefaultManager  GetKeyChain:DATA_STORE_CPS_MINBITRATE];
    NSString * maxbitrateForCPS = [UserDefaultManager  GetKeyChain:DATA_STORE_CPS_MAXBITRATE];
    NSString * firstptForCPS = [UserDefaultManager  GetKeyChain:DATA_STORE_CPS_FIRSTPT];
    
    //视频平台预设值
    NSString * presetsForCPS = [UserDefaultManager  GetKeyChain:DATA_STORE_CPS_PRESETS];
    NSString * bitratesForCPS = [UserDefaultManager  GetKeyChain:DATA_STORE_CPS_BITRATES];
    NSString * fpsForCPS = [UserDefaultManager  GetKeyChain:DATA_STORE_CPS_FPS];
    NSArray * presetsForCPSArray = [presetsForCPS componentsSeparatedByString:@":"];
    NSArray * bitratesForCPSArray = [bitratesForCPS componentsSeparatedByString:@":"];
    NSArray * fpsForCPSArray = [fpsForCPS componentsSeparatedByString:@":"];
    
    
    ME_video_enc_param_t stEnc;
    UGo_get_config(ME_VIDEO_ENC_CFG_MODULE_ID, &stEnc, 0);
    
    for(int i=0; i< stEnc.usNum; i++)
    {
        if(stEnc.baseCodec[i].ucPlType == 121){
            h264enableForCPS = [PublicFunc ucsCheckNullString:h264enableForCPS] ? h264enableForCPS : @"1";
            //h264
            stEnc.baseCodec[i].enable = [h264enableForCPS intValue];
        }
        if(stEnc.baseCodec[i].ucPlType == 122){
            vp8enableForCPS = [PublicFunc ucsCheckNullString:vp8enableForCPS] ? vp8enableForCPS : @"1";
            //vp8
            stEnc.baseCodec[i].enable = [vp8enableForCPS intValue];
        }
    }
    
    //modify by wenqinglin
    startbitrateForCPS = @"500";
    //startbitrateForCPS = [PublicFunc ucsCheckNullString:startbitrateForCPS] ? startbitrateForCPS : @"200";
    minbitrateForCPS = [PublicFunc ucsCheckNullString:minbitrateForCPS] ? minbitrateForCPS : @"30";
    maxbitrateForCPS = [PublicFunc ucsCheckNullString:maxbitrateForCPS] ? maxbitrateForCPS : @"500";
    firstptForCPS = [PublicFunc ucsCheckNullString:firstptForCPS] ? firstptForCPS : @"121";
    stEnc.usStartBitrate = [startbitrateForCPS intValue];
    stEnc.usMinBitrate = [minbitrateForCPS intValue];
    stEnc.usMaxBitrate = [maxbitrateForCPS intValue];
    stEnc.ucFirstSelectedPt = [firstptForCPS intValue];
    
    /**
     @author WLS, 16-02-26 14:02:35
     
     看看cps是否有配置平台预设值
     */
    if (presetsForCPSArray.count>1 &&
        bitratesForCPSArray.count>1 &&
        fpsForCPSArray.count>1) {
        
        int presets[3][3] = {0};
        int bitrates[3][3] = {0};
        int fps[3][3] = {0};
        
        for (int i = 0; i<3; i++) {
            NSArray * presetsNumber = [presetsForCPSArray[i] componentsSeparatedByString:@","];
            NSArray * bitratesNumber = [bitratesForCPSArray[i] componentsSeparatedByString:@","];
            NSArray * fpsNumber = [fpsForCPSArray[i] componentsSeparatedByString:@","];

            for (int j = 0; j<3; j++) {
                
                presets[i][j] = [presetsNumber[j] intValue];
                bitrates[i][j] = [bitratesNumber[j] intValue];
                fps[i][j] = [fpsNumber[j] intValue];
            }
            
        }
        
        memcpy(stEnc.enc_param_t.presets, presets, sizeof(presets));
        memcpy(stEnc.enc_param_t.bitrates, bitrates, sizeof(bitrates));
        memcpy(stEnc.enc_param_t.fps, fps, sizeof(fps));
        
        
    }else{
        presetsForCPSArray = @[@"1,0,-1",@"0,0,0",@"0,0,0"];
        bitratesForCPSArray = @[@"200,-1,-1",@"200,-1,-1",@"200,-1,-1"];
        fpsForCPSArray = @[@"10,10,-1",@"12,15,15",@"12,15,15"];
        
        int presets[3][3] = {0};
        int bitrates[3][3] = {0};
        int fps[3][3] = {0};
        
        for (int i = 0; i<3; i++) {
            NSArray * presetsNumber = [presetsForCPSArray[i] componentsSeparatedByString:@","];
            NSArray * bitratesNumber = [bitratesForCPSArray[i] componentsSeparatedByString:@","];
            NSArray * fpsNumber = [fpsForCPSArray[i] componentsSeparatedByString:@","];
            
            for (int j = 0; j<3; j++) {
                
                presets[i][j] = [presetsNumber[j] intValue];
                bitrates[i][j] = [bitratesNumber[j] intValue];
                fps[i][j] = [fpsNumber[j] intValue];
            }
            
        }
        
        memcpy(stEnc.enc_param_t.presets, presets, sizeof(presets));
        memcpy(stEnc.enc_param_t.bitrates, bitrates, sizeof(bitrates));
        memcpy(stEnc.enc_param_t.fps, fps, sizeof(fps));
    
    }
    
    NSLog(@,"ME_VIDEO_ENC_CFG_MODULE_ID: %d",ME_VIDEO_ENC_CFG_MODULE_ID);
    int ret = UGo_set_config(ME_VIDEO_ENC_CFG_MODULE_ID, &stEnc, 0);
    NSLog(@,"ret: %d",ret);
    /*******************************动态策略视频码率参数配置***************************************/
    
    
    
    /**
     @author WLS, 16-01-14 15:01:12
     
     默认通话编解码为amr
     switch(pltype){
     case 18:
     strcpy(setcodec.codecs[0].plname, "g729");
     break;
     
     case 107:
     strcpy(setcodec.codecs[0].plname, "amr");
     break;
     
     case 115:
     strcpy(setcodec.codecs[0].plname, "silkwb");
     break;
     
     case 106:
     strcpy(setcodec.codecs[0].plname, "silk");
     break;
     
     case 111:
     strcpy(setcodec.codecs[0].plname, "opus");
     break;
     
     case 113:
     strcpy(setcodec.codecs[0].plname, "ilbc");
     break;
     
     default:
     pltype = 107;
     strcpy(setcodec.codecs[0].plname, "amr");
     
     case 116:
     strcpy(setcodec.codecs[0].plname, "opusfb");
     break;
     
     break;
     }
     */
    ME_codec_list_t setcodec;
    UGo_get_config(ME_CODECS_CFG_MODULE_ID, &setcodec, 0);
    memset(&setcodec, 0x00, ME_CODEC_LIST_T_SIZE);
    setcodec.num = 1;
    setcodec.codecs[0].enabled = TRUE;
    setcodec.codecs[0].pltype = 111;
    strcpy(setcodec.codecs[0].plname, "opus");
    UGo_set_config(ME_CODECS_CFG_MODULE_ID, &setcodec, 0);

    
//    /**
//     @author WLS, 16-05-11 12:05:22
//     
//     关闭AEC
//     */
//    ME_VQE_cfg_t vqeCfg = { 0 };
//    UGo_get_config(ME_VQE_CFG_MODULE_ID, &vqeCfg, 0);
//    vqeCfg.Ec_enable = 0;
//    UGo_set_config(ME_VQE_CFG_MODULE_ID, &vqeCfg, 0);
    
}


// 上报语音质量 add by kucky  20141030
- (void)emodelValue
{
#if !(TARGET_IPHONE_SIMULATOR)
    ugo_emodel_info_tag emodelInfo;
    memset(&emodelInfo, 0x00, sizeof(emodelInfo));
    if (UGo_get_emodel_value(&emodelInfo) == 0)//获取语音质量参数值成功
    {
        
        ME_emodel_info_t emodel = emodelInfo.emodel;
        ugo_call_info_t pcp_call_pm = emodelInfo.callinfo;
        //        NSMutableDictionary *dic = [[[NSMutableDictionary alloc] init] autorelease];
        //        if (emodel.emodel_mos.flag)
        //            [dic setValue:[NSString stringWithFormat:@"%.2lf,%.2lf,%.2lf", emodel.emodel_mos.min, emodel.emodel_mos.max, emodel.emodel_mos.average] forKey:@"M"];
        //        if (emodel.emodel_tr.flag)
        //            [dic setValue:[NSString stringWithFormat:@"%.2lf,%.2lf,%.2lf", emodel.emodel_tr.min, emodel.emodel_tr.max, emodel.emodel_tr.average] forKey:@"T"];
        //        if (emodel.emodel_burstr.flag)
        //            [dic setValue:[NSString stringWithFormat:@"%.2lf,%.2lf,%.2lf", emodel.emodel_burstr.min, emodel.emodel_burstr.max, emodel.emodel_burstr.average] forKey:@"B"];
        //        if (emodel.emodel_ppl.flag)
        //            [dic setValue:[NSString stringWithFormat:@"%.2lf,%.2lf,%.2lf", emodel.emodel_ppl.min, emodel.emodel_ppl.max, emodel.emodel_ppl.average] forKey:@"P"];
        //        if (emodel.emodel_ie.flag)
        //            [dic setValue:[NSString stringWithFormat:@"%.2lf,%.2lf,%.2lf", emodel.emodel_ie.min, emodel.emodel_ie.max, emodel.emodel_ie.average] forKey:@"I"];
        
        
        
        
        
        NSNumber  *e_mos_min = [NSNumber numberWithDouble:emodel.emodel_mos.min];
        NSNumber * e_mos_max = [NSNumber numberWithDouble:emodel.emodel_mos.max];//
        NSNumber * e_mos_avg = [NSNumber numberWithDouble:emodel.emodel_mos.average];
        NSNumber * e_loss_min = [NSNumber numberWithDouble:emodel.emodel_ppl.min];
        NSNumber * e_loss_max = [NSNumber numberWithDouble:emodel.emodel_ppl.max];
        NSNumber * e_loss_avg = [NSNumber numberWithDouble:emodel.emodel_ppl.average];
        NSNumber * e_delay_min = [NSNumber numberWithDouble:emodel.delay.min];
        NSNumber * e_delay_max = [NSNumber numberWithDouble:emodel.delay.max];
        NSNumber * e_delay_avg = [NSNumber numberWithDouble:emodel.delay.average];
        NSNumber * e_jitter_min = [NSNumber numberWithDouble:emodel.jitbuf.min];
        NSNumber * e_jitter_max = [NSNumber numberWithDouble:emodel.jitbuf.max];;
        NSNumber * e_jitter_avg = [NSNumber numberWithDouble:emodel.jitbuf.average];
        //        NSNumber * e_snr = [NSNumber numberWithDouble:emodel.pcp_call_pm.snr];
        //        NSNumber * e_frate = [NSNumber numberWithDouble:emodel.pcp_call_pm.frate]; add by pbf
        NSNumber * e_snr = [NSNumber numberWithInt:0]; // add by pbf 2015.09.15
        NSNumber * e_frate = [NSNumber numberWithInt:0]; // add by pbf 2015.09.15
        NSString *e_ver = [self getSoftphoneVersion];
        NSString *e_net = [self GetCurrentPhoneNetType];
        NSString *e_sgw = [NSString stringWithFormat:@"%s",pcp_call_pm.sgw];
        NSString *e_mgw = [NSString stringWithFormat:@"%s",pcp_call_pm.mgw];
        //NSString *e_pv = [UIDevice getplatformString];
        NSString *e_pv = @"Mac";
        NSString *e_caller = [UserDefaultManager GetUserID];
        NSString *e_callee = @"";
        NSString *e_mcodec = [NSString stringWithFormat:@"%s",pcp_call_pm.mcodec];
        NSNumber *e_cmode = [NSNumber numberWithInt:pcp_call_pm.cmode];
        NSNumber *e_mmode = [NSNumber numberWithInt:pcp_call_pm.mmode];
        NSNumber *e_ctime = [NSNumber numberWithInt:pcp_call_pm.ctime];
        NSNumber *e_cstate =[NSNumber numberWithInt:pcp_call_pm.cstate];
        NSNumber *e_role =  [NSNumber numberWithInt:pcp_call_pm.role];
        //add by pbf
        NSString *e_callid = [NSString stringWithFormat:@"%s",pcp_call_pm.callid];
        
        
        //语音质量上报功能性完善 add by WLS 20150928
        NSNumber * e_mgport = [NSNumber numberWithInt:pcp_call_pm.mgwport];//媒体网关：端口
        NSNumber *e_cmute = [NSNumber numberWithInt:[UserDefaultManager GetLocalDataBoolen:DATA_STORE_ISCMUTE]]; //是否通话过程中点击静音按钮
        NSNumber *e_sp_reson= [NSNumber numberWithInt:pcp_call_pm.spreason]; //单通原因
        NSNumber *e_psend = [NSNumber numberWithInt:emodel.pkt_stat.pktSnd]; //发包个数
        NSNumber *e_precieve = [NSNumber numberWithInt:emodel.pkt_stat.pktRecv]; //收包个数
        NSNumber *e_tstate = [NSNumber numberWithInt:1]; //是否重连  0:未连接  1:已链接  2:重连
        NSNumber *e_isvideo = [NSNumber numberWithInt:[UserDefaultManager GetLocalDataBoolen:DATA_STORE_ISVIDEO]]; //是否是视频通话
        //        NSNumber *e_screenlock = [NSNumber numberWithInt:[UserDefaultManager GetLocalDataBoolen:DATA_STORE_ISSCREENLOCK]];//是否锁屏 WLS 20150924
        NSNumber *e_screenlock = [NSNumber numberWithInt:0];//是否锁屏 WLS 20150924
        
        NSDictionary *tempreportData = [[[NSDictionary alloc]
                                         initWithObjectsAndKeys:
                                         e_mos_min,
                                         @"mos_min",
                                         e_mos_max,
                                         @"mos_max",
                                         e_mos_avg,
                                         @"mos_avg",
                                         e_loss_min,
                                         @"loss_min",
                                         e_loss_avg,
                                         @"loss_avg",
                                         e_loss_max,
                                         @"loss_max",
                                         e_delay_min,
                                         @"delay_min",
                                         e_delay_max,
                                         @"delay_max",
                                         e_delay_avg,
                                         @"delay_avg",
                                         e_jitter_min,
                                         @"jitter_min",
                                         e_jitter_max,
                                         @"jitter_max",
                                         e_jitter_avg,
                                         @"jitter_avg",
                                         e_snr,
                                         @"snr",
                                         e_frate,
                                         @"frate",
                                         e_ver,
                                         @"ver",
                                         e_net,
                                         @"net",
                                         e_sgw,
                                         @"sgw",
                                         e_mgw,
                                         @"mgw",
                                         e_pv,
                                         @"pv",
                                         e_caller,
                                         @"caller",
                                         e_callee,
                                         @"callee",
                                         e_mcodec,
                                         @"mcodec",
                                         e_cmode,
                                         @"cmode",
                                         e_mmode,
                                         @"mmode",
                                         e_ctime,
                                         @"ctime",
                                         e_cstate,
                                         @"cstate",
                                         e_role,
                                         @"role",
                                         e_callid,
                                         @"callid",
                                         e_mgport,
                                         @"mgport",
                                         e_sp_reson,
                                         @"sp_reson",
                                         e_cmute,
                                         @"cmute",
                                         e_psend,
                                         @"psend",
                                         e_isvideo,
                                         @"isvideo",
                                         e_precieve,
                                         @"precieve",
                                         e_tstate,
                                         @"tstate",
                                         e_screenlock,
                                         @"screenlock",
                                         nil] autorelease];
        
        
        //上报语音质量
        [[UCSQualityRequestManager instance] reportQualityForSP:tempreportData];
        UCSDialViewController * dial = [UCSDialViewController share];
        [dial performSelector:@selector(appendVoiceQulity:) withObject:tempreportData];
        
    } else //获取语音质量参数值失败
    {
        ME_emodel_info_t emodel = emodelInfo.emodel;
        ugo_call_info_t pcp_call_pm = emodelInfo.callinfo;
        
        //组件上报 add by pbf
        NSNumber  *e_mos_min = [NSNumber numberWithDouble:emodel.emodel_mos.min];
        NSNumber * e_mos_max = [NSNumber numberWithDouble:emodel.emodel_mos.max];//
        NSNumber * e_mos_avg = [NSNumber numberWithDouble:emodel.emodel_mos.average];
        NSNumber * e_loss_min = [NSNumber numberWithDouble:emodel.emodel_ppl.min];
        NSNumber * e_loss_max = [NSNumber numberWithDouble:emodel.emodel_ppl.max];
        NSNumber * e_loss_avg = [NSNumber numberWithDouble:emodel.emodel_ppl.average];
        NSNumber * e_delay_min = [NSNumber numberWithDouble:emodel.delay.min];
        NSNumber * e_delay_max = [NSNumber numberWithDouble:emodel.delay.max];
        NSNumber * e_delay_avg = [NSNumber numberWithDouble:emodel.delay.average];
        NSNumber * e_jitter_min = [NSNumber numberWithDouble:emodel.jitbuf.min];
        NSNumber * e_jitter_max = [NSNumber numberWithDouble:emodel.jitbuf.max];;
        NSNumber * e_jitter_avg = [NSNumber numberWithDouble:emodel.jitbuf.average];
        //        NSNumber * e_snr = [NSNumber numberWithDouble:emodel.pcp_call_pm.snr];
        //        NSNumber * e_frate = [NSNumber numberWithDouble:emodel.pcp_call_pm.frate];
        NSNumber * e_snr = [NSNumber numberWithInt:0]; // add by pbf 2015.09.15
        NSNumber * e_frate = [NSNumber numberWithInt:0]; // add by pbf 2015.09.15
        NSString *e_ver = [self getSoftphoneVersion];
        NSString *e_net = [self GetCurrentPhoneNetType];
        NSString *e_sgw = [NSString stringWithFormat:@"%s",pcp_call_pm.sgw];
        NSString *e_mgw = [NSString stringWithFormat:@"%s",pcp_call_pm.mgw];
        //NSString *e_pv = [UIDevice getplatformString];
        NSString *e_pv = @"MAC";
        NSString *e_caller = [UserDefaultManager GetUserID];
        NSString *e_callee = @"";
        NSString *e_mcodec = [NSString stringWithFormat:@"%s",pcp_call_pm.mcodec];
        NSNumber *e_cmode = [NSNumber numberWithInt:pcp_call_pm.cmode];
        NSNumber *e_mmode = [NSNumber numberWithInt:pcp_call_pm.mmode];
        NSNumber *e_ctime = [NSNumber numberWithInt:pcp_call_pm.ctime];
        NSNumber *e_cstate =[NSNumber numberWithInt:pcp_call_pm.cstate];
        NSNumber *e_role =  [NSNumber numberWithInt:pcp_call_pm.role];
        
        NSString *e_callid = [NSString stringWithFormat:@"%s",pcp_call_pm.callid];
        
        
        //语音质量上报功能性完善 add by WLS 20150928
        NSNumber * e_mgport = [NSNumber numberWithInt:pcp_call_pm.mgwport];//媒体网关：端口
        NSNumber *e_cmute = [NSNumber numberWithInt:[UserDefaultManager GetLocalDataBoolen:DATA_STORE_ISCMUTE]]; //是否通话过程中点击静音按钮
        NSNumber *e_sp_reson= [NSNumber numberWithInt:pcp_call_pm.spreason]; //单通原因
        NSNumber *e_psend = [NSNumber numberWithInt:emodel.pkt_stat.pktSnd]; //发包个数
        NSNumber *e_precieve = [NSNumber numberWithInt:emodel.pkt_stat.pktRecv]; //收包个数
        NSNumber *e_tstate = [NSNumber numberWithInt:1]; //是否重连  0:未连接  1:已链接  2:重连
        NSNumber *e_isvideo = [NSNumber numberWithInt:[UserDefaultManager GetLocalDataBoolen:DATA_STORE_ISVIDEO]]; //是否是视频通话
        //        NSNumber *e_screenlock = [NSNumber numberWithInt:[UserDefaultManager GetLocalDataBoolen:DATA_STORE_ISSCREENLOCK]];//是否锁屏 WLS 20150924
        NSNumber *e_screenlock = [NSNumber numberWithInt:0];//是否锁屏 WLS 20150924
        
        
        
        NSDictionary *tempreportData = [[[NSDictionary alloc]
                                         initWithObjectsAndKeys:
                                         e_mos_min,
                                         @"mos_min",
                                         e_mos_max,
                                         @"mos_max",
                                         e_mos_avg,
                                         @"mos_avg",
                                         e_loss_min,
                                         @"loss_min",
                                         e_loss_avg,
                                         @"loss_avg",
                                         e_loss_max,
                                         @"loss_max",
                                         e_delay_min,
                                         @"delay_min",
                                         e_delay_max,
                                         @"delay_max",
                                         e_delay_avg,
                                         @"delay_avg",
                                         e_jitter_min,
                                         @"jitter_min",
                                         e_jitter_max,
                                         @"jitter_max",
                                         e_jitter_avg,
                                         @"jitter_avg",
                                         e_snr,
                                         @"snr",
                                         e_frate,
                                         @"frate",
                                         e_ver,
                                         @"ver",
                                         e_net,
                                         @"net",
                                         e_sgw,
                                         @"sgw",
                                         e_mgw,
                                         @"mgw",
                                         e_pv,
                                         @"pv",
                                         e_caller,
                                         @"caller",
                                         e_callee,
                                         @"callee",
                                         e_mcodec,
                                         @"mcodec",
                                         e_cmode,
                                         @"cmode",
                                         e_mmode,
                                         @"mmode",
                                         e_ctime,
                                         @"ctime",
                                         e_cstate,
                                         @"cstate",
                                         e_role,
                                         @"role",
                                         e_callid,
                                         @"callid",
                                         e_mgport,
                                         @"mgport",
                                         e_sp_reson,
                                         @"sp_reson",
                                         e_cmute,
                                         @"cmute",
                                         e_psend,
                                         @"psend",
                                         e_isvideo,
                                         @"isvideo",
                                         e_precieve,
                                         @"precieve",
                                         e_tstate,
                                         @"tstate",
                                         e_screenlock,
                                         @"screenlock",
                                         nil] autorelease];
        
        
        //上报语音质量
        [[UCSQualityRequestManager instance] reportQualityForSP:tempreportData];
        
        UCSDialViewController * dial = [UCSDialViewController share];
        [dial performSelector:@selector(appendVoiceQulity:) withObject:tempreportData];
    }
    
#endif
    
}


- (void)setDefaultRttp{
    
    
    NSArray *arrayVoipList = [UserDefaultManager GetLocalDataObject:DATA_STORE_RTPPLIST];
    ugo_rtpp_cfg_t rtpp_cfg;
    memset(&rtpp_cfg, 0, UGO_RTPP_CFG_T_SIZE);
    for (int i = 0,j = 0; i <[arrayVoipList count]; i++) {
        rtpp_cfg.rtpp_num = j + 1;
        rtpp_cfg.rtpps[j].delay = 40;
        rtpp_cfg.rtpps[j].lost = 0;
        NSString *IPPort = [arrayVoipList objectAtIndex:i];
        NSString *strIP = @"";
        
        NSRange range = [IPPort rangeOfString:@":"];//判断是否包含“:”
        if (range.location != NSNotFound) { //如果不是没有找到端口，则将ip和端口分开。
            strIP = [IPPort substringToIndex:range.location];
        } else {//如果没有端口则使用默认的518端口
            strIP = IPPort;
        }
        
        //----------------------------------------------------------------
        
        strncpy(rtpp_cfg.rtpps[j].ip, [strIP UTF8String], 64);
        j++;
    }
    
    if (rtpp_cfg.rtpp_num<1) {
        NSLog(@"可正常使用RTPP服务器台数为%d",rtpp_cfg.rtpp_num);
    }
#if !(TARGET_IPHONE_SIMULATOR)
    //    UGo_set_rtpp_config(&rtpp_cfg);
    UGo_set_config(UGO_RTPP_CFG_MODULE_ID,(void*)&rtpp_cfg, 0);
#endif
}

//获取最优rtpp线路并设置
- (void)getTheBestIPSuccessNotification
{
    NSArray *theBestIP = [_voipListPing bestIPAndPortList];
    //释放ping值的功能对象 add by WLS 20151118
    [_voipListPing release];
    _voipListPing = nil;
    
    ugo_rtpp_cfg_t rtpp_cfg;
    memset(&rtpp_cfg, 0, UGO_RTPP_CFG_T_SIZE);
    //    for (int i = 0,j=0; i < [theBestIP count] && i < UGO_MAX_RTPP ; ++i)  // rtpp 再组件2.7只能设置5个，2.8版本已经改为10个。
    for (int i = 0,j=0; i < [theBestIP count] && i < 5 ; ++i)  // rtpp 再组件2.7只能设置5个，2.8版本已经改为10个。
    {
        
        
        
        CustomPingPackage *pkg = [theBestIP objectAtIndex:i];
        
        //保存ping值信息 WLS 20150817
        [pkg savePingInfo];
        
        
        
        //        if ([pkg delay]>300||[pkg loss]>=1.0) { //如果延迟超过300或者丢包大于等于1则说明服务器无法连通或者端口号不正确或者此端口的服务没有正常运行
        //
        //            continue;
        //        }
        
        rtpp_cfg.rtpp_num = j + 1;
        rtpp_cfg.rtpps[j].delay = [pkg delay];
        rtpp_cfg.rtpps[j].lost = (ucpaas_int32)[pkg loss] * 100;
        
        NSLog(@"[pkg ip]- %@",pkg.ip);
        NSLog(@"[pkg delay]- %d",[pkg delay]);
        NSLog(@"[pkg loss]-  %d",(ucpaas_int32)[pkg loss]*100);
        
        //------------------------将IP与端口进行分解----------------------------------------
        NSString *IPPort = pkg.ip;
        NSString *strIP = @"";
        
        NSRange range = [IPPort rangeOfString:@":"];//判断是否包含“:”
        if (range.location != NSNotFound) { //如果不是没有找到端口，则将ip和端口分开。
            strIP = [IPPort substringToIndex:range.location];
        } else {//如果没有端口则使用默认的518端口
            strIP = IPPort;
        }
        
        //----------------------------------------------------------------
        
        strncpy(rtpp_cfg.rtpps[j].ip, [strIP UTF8String], 64);
        j++;
    }
    
    if (rtpp_cfg.rtpp_num<1) {
        NSLog(@"可正常使用RTPP服务器台数为%d",rtpp_cfg.rtpp_num);
    }
    
    
#if !(TARGET_IPHONE_SIMULATOR)
    //    UGo_set_rtpp_config(&rtpp_cfg);
    UGo_set_config(UGO_RTPP_CFG_MODULE_ID,(void*)&rtpp_cfg, 0);
#endif
}


- (void)setIncomingCallMusicPath:(NSString *)path {
    self.localMusicPath = path;
}

//播放本地 铃声
- (void)playLocalMusic{
    return;
    UGo_stop_file();
    char playPath[128] = {0};
    sprintf(playPath, "%s", [self.localMusicPath cStringUsingEncoding:NSUTF8StringEncoding]);
    printf("Dialing to Play:%s\n", playPath);
    media_file_play_pm_t playpm;
    playpm.filehandle.fd = NULL;
    playpm.iDirect = 0;
    playpm.iFileFormat = 7;
    playpm.iLoop = 1;
    strcpy(playpm.filepath.filename, playPath);
    UGo_play_file(kME_FILE_PATHNAME, &playpm);
}


- (void)setVideoCodecWithIndex:(NSInteger)videoCodecIndex audioCodecIndex:(NSInteger)audioCodecIndex
{
    /****set Video codec config****/
    ME_video_enc_param_t stEnc;
    UGo_get_config(ME_VIDEO_ENC_CFG_MODULE_ID, &stEnc, 0);
    for (int i = 0; i< stEnc.usNum; i++)
    {
        if (videoCodecIndex == 0 && stEnc.baseCodec[i].ucPlType == 121)
        {
            stEnc.ucFirstSelectedPt = 121;
            stEnc.ucUseHwEnc = 0;
            stEnc.ucUseHwDec = 0;
            NSLog(@"setSoftphoneManager video codec[%s-%d]\n", stEnc.baseCodec[i].ucPlName,stEnc.baseCodec[i].ucPlType);
        }
        else if (videoCodecIndex == 1 && stEnc.baseCodec[i].ucPlType == 121)
        {
            stEnc.ucFirstSelectedPt = 121;
            stEnc.ucUseHwEnc = 1;
            stEnc.ucUseHwDec = 1;
            NSLog(@"setSoftphoneManager video codec[%s-%d]\n", stEnc.baseCodec[i].ucPlName,stEnc.baseCodec[i].ucPlType);
        }
        else if (videoCodecIndex == 2 && stEnc.baseCodec[i].ucPlType == 125)
        {
            stEnc.ucFirstSelectedPt = 125;
            stEnc.ucUseHwEnc = 0;
            stEnc.ucUseHwDec = 0;
            NSLog(@"setSoftphoneManager video codec[%s-%d]\n", stEnc.baseCodec[i].ucPlName,stEnc.baseCodec[i].ucPlType);
        }
        else if (videoCodecIndex == 3 && stEnc.baseCodec[i].ucPlType == 125)
        {
            stEnc.ucFirstSelectedPt = 125;
            stEnc.ucUseHwEnc = 1;
            stEnc.ucUseHwDec = 1;
            NSLog(@"setSoftphoneManager video codec[%s-%d]\n", stEnc.baseCodec[i].ucPlName,stEnc.baseCodec[i].ucPlType);
        }
        
    }
    UGo_set_config(ME_VIDEO_ENC_CFG_MODULE_ID, &stEnc, 0);
    
    
    /****set Audio codec config****/
    ME_codec_list_t setcodec;
    memset(&setcodec,0,sizeof(ME_codec_list_t));
    char plname[8][32] = { "opus","amr", "silkwb", "silk", "g729","opusnb", "ilbc" ,"aac"};
    switch(audioCodecIndex)
    {
        case 0:
            memcpy(setcodec.codecs[0].plname, plname[0], 32);
            setcodec.codecs[0].pltype = 111;
            break;
            
//        case 1:
//            memcpy(setcodec.codecs[0].plname, plname[1], 32);
//            setcodec.codecs[0].pltype = 107;
//            break;
//            
//        case 2:
//            memcpy(setcodec.codecs[0].plname, plname[2], 32);
//            setcodec.codecs[0].pltype = 115;
//            break;
//            
//        case 3:
//            memcpy(setcodec.codecs[0].plname, plname[3], 32);
//            setcodec.codecs[0].pltype = 106;
//            break;
//            
//        case 4:
//            memcpy(setcodec.codecs[0].plname, plname[4], 32);
//            setcodec.codecs[0].pltype = 18;
//            break;
//            
//        case 5:
//            memcpy(setcodec.codecs[0].plname, plname[5], 32);
//            setcodec.codecs[0].pltype = 112;
//            break;
//            
//        case 6:
//            memcpy(setcodec.codecs[0].plname, plname[6], 32);
//            setcodec.codecs[0].pltype = 113;
//            break;
            
        default:
            memcpy(setcodec.codecs[0].plname, plname[7], 32);
            setcodec.codecs[0].pltype = 124;
            break;
    }
    NSLog(@"setSoftphoneManager audio codec[%s-%d]\n", setcodec.codecs[0].plname,setcodec.codecs[0].pltype);
    setcodec.num = 1;
    setcodec.codecs[0].enabled = TRUE;
    UGo_set_config(ME_CODECS_CFG_MODULE_ID, &setcodec, 0);
}


/**
 @author WLS, 16-05-31 10:05:26
 
 设置IPV6网络环境，在通话前设置有效
 */
- (void)setIpv6Enable:(BOOL)isIpv6{
    
    int ret = -1;
    
    ME_CTRL_cfg_t Me_config;
    memset(&Me_config, 0, sizeof(Me_config));
    UGo_get_config(ME_CTRL_CFG_MODULE_ID, &Me_config, 0);
    Me_config.ucIPv6Enable = isIpv6?true:false;
    ret = UGo_set_config(ME_CTRL_CFG_MODULE_ID, &Me_config, 0);

    [[SDKLogManager instance]saveSDKLogInfo:@"设置ctrl的IPV6" withDetail:[NSString stringWithFormat:@"设置ctrl的IPV6 : %d",ret]];
    
    ugo_cfg_t ucfg;
    memset(&ucfg, 0, sizeof(ucfg));
    UGo_get_config(UGO_CFG_PARAM_MODULE_ID, &ucfg, 0);
    ucfg.ipv6_enabled = isIpv6?true:false;
    ret = UGo_set_config(UGO_CFG_PARAM_MODULE_ID, &ucfg, 0);

    [[SDKLogManager instance]saveSDKLogInfo:@"设置ugo的IPV6" withDetail:[NSString stringWithFormat:@"设置ugo的IPV6 : %d",ret]];
    
    ugo_cfg_ice_t ice_cfg;
    memset(&ice_cfg, 0, sizeof(ice_cfg));
    UGo_get_config(UGO_CFG_ICE_MODULE_ID, &ice_cfg, 0);
    if (ice_cfg.ice_enabled) {
        
        ice_cfg.ipv6_enabled = isIpv6?true:false;
        ret = UGo_set_config(UGO_CFG_ICE_MODULE_ID,(void*)&ice_cfg, 0);
        
        [[SDKLogManager instance]saveSDKLogInfo:@"设置ice的IPV6" withDetail:[NSString stringWithFormat:@"设置ice的IPV6 : %d",ret]];
    }
    
}

#pragma mark - --------------------------push续活能力--------------------------
//支持推送
- (void)EnablePush
{
    
    
    //            UGo_ios_call_incoming_rsp([_rspCallId UTF8String], _rspCallVsp, eUGo_Reason_NetworkDisable);
    const char *_ucsrspCallId  = [_rspCallId cStringUsingEncoding:NSASCIIStringEncoding];
    
    ugo_ios_incoming_rsp_pm_t callparam ;
    
    
    memset(&callparam, 0, sizeof(callparam));
    
    strcpy(callparam.callid, _ucsrspCallId);
    callparam.vpsid = _rspCallVsp;
    callparam.reason = eUGo_Reason_Success;
    //    strcpy(callparam.reserved, 0);
    
    UGo_ios_call_incoming_rsp(&callparam);
    
    
    
    
}

//不支持推送
- (void)DisablePush
{
    //            UGo_ios_call_incoming_rsp([_rspCallId UTF8String], _rspCallVsp, eUGo_Reason_NetworkDisable);
    
    const char *_ucsrspCallId  = [_rspCallId cStringUsingEncoding:NSUTF8StringEncoding];
    ugo_ios_incoming_rsp_pm_t callparam ;
    
    
    memset(&callparam, 0, sizeof(callparam));
    
    strcpy(callparam.callid, _ucsrspCallId);
    callparam.vpsid = _rspCallVsp;
    callparam.reason = eUGo_Reason_NetworkDisable;
    //    strcpy(callparam.reserved, 0);
    
    UGo_ios_call_incoming_rsp(&callparam);
}

//当app还在后台的时候，收到推送
- (void)incomingRspWhenBackground:(id)callID vpsid:(int)vpsid
{
    _rspCallId = [callID copy];
    _rspCallVsp = vpsid;
    if (_rspCallId) {
        PhoneNetType currentNetType = [PublicFunc GetCurrentPhoneNetType];
        if (PNT_2G == currentNetType)
        {
            //            [PublicFunc ShowMessageBox:0 titleName:nil contentText:@"收到有信来电，但是您当前网络不支持免费接听(请确保WiFi、3G连接正常)。" cancelBtnName:@"我知道了" delegate:nil];
            NSLog(@"%@",@"收到来电，但是您当前网络不支持免费接听(请确保WiFi、3G连接正常)。");
            
            [self DisablePush];
            
            
        } else {
            
            
            if (reasons.reason == 0) {//支持推送
                
                [self EnablePush];
            }else{
                
                [self DisablePush]; //不支持推送
                
            }
            
            
            
        }
    }
    [_rspCallId release], _rspCallId = nil;
}

//用于推送，续活APP
- (void)incomingRsp:callID vpsid:(NSInteger)vpsid withReason:(KCTReason*)reason
{
    _rspCallId = [callID copy];
    NSString *strVpsStr = [NSString stringWithFormat:@"%d",(int)vpsid];
    _rspCallVsp = [strVpsStr intValue];
    _ucsReason = [reason copy];
}


//用于推送，续活APP
- (void)incomingRsp:callID vpsid:(int)vpsid
{
    _rspCallId = [callID copy];
    _rspCallVsp = vpsid;
}

//?
- (NSString *)getCurrentCallid
{
    return [[_rspCallId retain]autorelease];
}


#pragma mark - --------------------------创建callid--------------------------
- (NSString*)createCallidByClientNumber:(NSString*)clientNumber
{
    
    const char *c_uid =  [clientNumber cStringUsingEncoding:NSUTF8StringEncoding];
    
    char c_callid[64];
    
    NSString  *callid = nil;
    
    if(create_callid(c_uid, c_callid)==0)//callid 创建成功
    {
        callid = [NSString stringWithFormat:@"%s",c_callid];
    }else
    {
        callid = [NSString stringWithFormat:@"create_callid_fail"];
    }
    
    
    
    
    return callid;
    
    
}

const int PCP_ERR = -1;
const int PCP_OK = 0;

int create_callid(IN const char* uid, OUT char* callid)
{
    char alp[6] = {0};
    static unsigned nsceq = 0;
    
    if (callid == NULL)
        return PCP_ERR;
    
    if (nsceq++ > 999)
        nsceq = 0;
    
    cm_rand_alphabet5(alp);
    
    
    
    if (uid != NULL && strlen(uid) != 0)
    {
        sprintf(callid, "%s%s%d%d", uid, alp, cm_rand_time(), nsceq);
    }
    else
    {
        sprintf(callid, "null%s%d%d", alp, cm_rand_time(), nsceq);
    }
    
    return PCP_OK;
}

const char kAlphabetUp[] = "FGELMNOPQRWXHISTUVKABCDJYZ.";
const char kAlphabetLow[] = "uvwxmghijknopalrstbcdefqyz.";

int cm_rand_alphabet5(OUT char alp_str[6])
{
    int i = 0;
    int j = 0;
    int rand_num = cm_rand_time();
    
    for (i = 0; i < 5; i++)
    {
        j = (rand_num+i+j) % 26;
        if (i%2)
        {
            alp_str[i] = kAlphabetUp[j];
        }
        else
        {
            alp_str[i] = kAlphabetLow[j];
        }
    }
    
    alp_str[i] = '\0';
    
    return RES_OK;
}



int cm_rand_time(void)
{
    int rand_num = 0;
    
    srand((unsigned)time(NULL));
    
    rand_num = rand();
    
    return rand_num;
}


#pragma mark - 设置AGC增益
- (BOOL)setAgcCompressionGaindB:(int)compressionGaind targetDbfs:(int)targetDbfs{
    
    if (compressionGaind<1 || compressionGaind>30  ||
        targetDbfs < 1 || targetDbfs > 15) {

        return NO;
    }
    
    
    ME_VQE_cfg_t ME_Vqe_config;
    UGo_get_config(ME_VQE_CFG_MODULE_ID, &ME_Vqe_config, 0);
    ME_Vqe_config.Agc_Rx_enable = YES;
    ME_Vqe_config.Agc_compressionGaindB = compressionGaind;
    ME_Vqe_config.Agc_targetDbfs = targetDbfs;
    
    int ret = UGo_set_config(ME_VQE_CFG_MODULE_ID, &ME_Vqe_config, 0);
    
    if (ret == 0) {
        return YES;
    }else{
        return NO;
    }
    
}

#pragma mark - rtp包加密控制以及回调

/**
 @author WLS, 16-02-24 19:02:18
 
 设置rtp包加密控制开关
 
 @param enable 开关
 
 @return YES为设置成功 , NO为设置失败
 */
- (BOOL)setRtpEncEnable:(BOOL)enable{
    
    ME_CTRL_cfg_t ME_config;
    UGo_get_config(ME_CTRL_CFG_MODULE_ID, &ME_config, 0);
    
   
    ME_config.ucRtpEncEnable = enable ? true : false;
    
    int ret = UGo_set_config(ME_CTRL_CFG_MODULE_ID, &ME_config, 0);
    
    if (ret == 0) {
        return YES;
    }else{
        return NO;
    }
    
}

/**
 * in_data  加密前的数据
 * out_data 加密后的数据
 * bytes_in 加密前数据的长度
 * bytes_out 加密后数据的长度
 *
 **/
void UgoEncryptCallBack(ucpaas_uint8* in_data,
                        ucpaas_uint8* out_data,
                        ucpaas_int32 bytes_in,
                        ucpaas_int32* bytes_out)
{
    
    
    
    
    id delegate = [SoftphoneManager instance].delegate;
    if (delegate)
    {
        if ([delegate respondsToSelector:@selector(EncryptCallBack:withOutdata:withByteIn:withByteOut:)]) {
            [delegate EncryptCallBack:(char*) in_data withOutdata:(char*)out_data withByteIn:(int)bytes_in withByteOut:(int*)bytes_out];
        }

    }
    
    
}
/**
 * in_data  解密前的数据
 * out_data 解密后的数据
 * bytes_in 解密前数据的长度
 * bytes_out 解密后数据的长度
 *
 **/
void UgoDecryptCallBack(ucpaas_uint8* in_data,
                        ucpaas_uint8* out_data,
                        ucpaas_int32 bytes_in,
                        ucpaas_int32* bytes_out)
{
    
    
    id delegate = [SoftphoneManager instance].delegate;
    if (delegate)
    {
        if ([delegate respondsToSelector:@selector(DecryptCallBack:withOutdata:withByteIn:withByteOut:)]) {
            [delegate DecryptCallBack:(char*) in_data withOutdata:(char*)out_data withByteIn:(int)bytes_in withByteOut:(int*)bytes_out];
        }
        
    }
    
}



@end
