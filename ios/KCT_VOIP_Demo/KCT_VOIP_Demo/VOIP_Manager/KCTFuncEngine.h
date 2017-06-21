
#import <UIKit/UIKit.h>
#import "KCTEvent.h"
#import "KCTService.h"
#import "KCTPubEnum.h"





extern NSString * const KCTSNotiHeadPhone;
extern NSString * const KCTNotiCallBalance;
extern NSString * const KCTNotiRefreshCallList;
extern NSString * const KCTNotiIncomingCall;
extern NSString * const KCTNotiEngineSucess;
extern NSString * const KCTNotiTCPTransParent;

@protocol KCTEngineUIDelegate <NSObject>
@optional
//来电信息
-(void)incomingCallID:(NSString*)callid caller:(NSString*)caller phone:(NSString*)phone name:(NSString*)name callStatus:(int)status callType:(NSInteger)calltype;

//通话状态回调
-(void)responseVoipManagerStatus:(KCTCallStatus)event callID:(NSString*)callid data:(KCTReason *)data withVideoflag:(int)videoflag;



// 语音质量展示回调
- (void)showNetWorkState:(NSString *)networkStateStr networkDes:(NSString *)networkDes;
// 对端视频模式回调
- (void)onRemoCameraMode:(KCTCameraType)type;
@end


//这个类是属于上面业务层与SDK之间的调度层，负责调用SDK接口和接收分发SDK的回调
@interface KCTFuncEngine : NSObject<KCTEventDelegate>


@property (nonatomic, retain)KCTService            *kctCallService;

@property (nonatomic, assign)NSString  *userId;

@property (nonatomic, assign)id<KCTEngineUIDelegate>UIDelegate;//UI业务代理

@property (nonatomic,assign) BOOL isHeadPhone; //是否有耳机 add by WLS 20151106



+(KCTFuncEngine *)getInstance;


- (void)setVideoCodecWithIndex:(NSInteger)videoIndex audioCodecIndex:(NSInteger)audioIndex;

#pragma mark - -------------------呼叫控制函数-------------------
/**
 * 拨打电话
 * @param callType 电话类型
 * @param called 电话号(加国际码)或者VoIP号码
 * @return
 */
- (void)dial:(NSInteger)callType andCalled:(NSString *)calledNumber andUserdata:(NSString *)callerData;

/**
 * 挂断电话
 * @param callid 电话id
 * @param reason 预留参数
 */
- (void) hangUp: (NSString*)called;

/**
 * 接听电话
 * @param callid 电话id
 * V2.0
 */
- (void) answer: (NSString*)callId;

/**
 * 拒绝呼叫(挂断一样,当被呼叫的时候被呼叫方的挂断状态)
 * @param callid 电话id
 * @param reason 拒绝呼叫的原因, 可以传入ReasonDeclined:用户拒绝 ReasonBusy:用户忙
 */
- (void)reject: (NSString*)called;



#pragma mark - -------------------DTMF函数-------------------
/**
 * 发送DTMF
 * @param callid 电话id
 * @param dtmf 键值
 */
- (BOOL)sendDTMF: (char)dtmf;

#pragma mark - -------------------本地功能函数-------------------

/**
 * 免提设置
 * @param enable false:关闭 true:打开
 */
- (void) setSpeakerphone:(BOOL)enable;


/**
 * 获取当前免提状态
 * @return false:关闭 true:打开
 */
- (BOOL) isSpeakerphoneOn;


/**
 * 静音设置
 * @param on false:正常 true:静音
 */
- (void)setMicMute:(BOOL)on;

/**
 * 获取当前静音状态
 * @return false:正常 true:静音
 */
- (BOOL)isMicMute;



/**
 * 获取SDK版本信息
 *
 */
- (NSString*) getKCTSDKVersion;

/**
 * 获取流量分类统计信息
 *
 */
-(NSDictionary*) getNetWorkFlow;



#pragma mark - -------------------视频能力------------------------

/**
*  初始化视频显示控件（本地视频显示控件和对方视频显示控件）
*
*参数 frame 窗口大小
*
*@return UIView 视频显示控件:
*/
- (UIView *)allocCameraViewWithFrame:(CGRect)frame;

/**
 * 设置视频显示参数
 *
 *参数 localVideoView 设置本地视频显示控件
 *参数 remoteView     设置对方视频显示控件
 *
 *@return NO:  YES:
 */
-(BOOL)initCameraConfig:(UIView*)localVideoView withRemoteVideoView:(UIView*)remoteView;


/**
 *  自定义视频编码和解码参数
 *
 *  @param kctVideoEncAttr 编码参数
 *  @param kctVideoDecAttr 解码参数
 */
- (void)setVideoAttr:(KCTVideoEncAttr*)kctVideoEncAttr andVideoDecAttr:(KCTVideoDecAttr*)kctVideoDecAttr;


/**
 * 旋转显示图像角度
 *
 * 参数 sendRotation      本地显示图像角度  数值为0 90 180 270
 * 参数 reciviedRotation  对端显示图像角度  数值为0 90 180 270
 *@return NO:  YES:
 */
- (BOOL)setRotationVideo:(unsigned int)sendRotation withReciviedRotation:(unsigned int)reciviedRotation;


/**
 *
 * 获取摄像头个数
 */
- (int)getCameraNum;


/**
 * 摄像头切换 后置摄像头：0 前置摄像头：1
 *return YES 成功 NO 失败
 */
- (BOOL)switchCameraDevice:(int)CameraIndex;


/**
 *  切换视频模式：发送、接收、正常模式
 *
 *  @param type         CAMERA_RECEIVE : 只接收视频数据（只能接收到对方的视频）
                        CAMERA_SEND : 只发送视频数据（只让对方看到我的视频）
                        CAMERA_NORMAL : send receive preview
 *
 *  @return YES 成功 NO 失败
 */
- (BOOL)switchVideoMode:(KCTCameraType)type;


/**
 * 视频截图
 * @param islocal: 0 是远端截图 1 是本地截图。
 * @param filename: 截图名称。
 * @param savePath: 存放路径。
 *
 */
- (void)cameraCapture:(int)islocal withFileName:(NSString*)filename withSavePath:(NSString*)savePath;


/**
 * 视频来电时是否支持预览
 * isPreView: YES 支持预览 NO 不支持预览。
 * return YES 成功 NO 失败
 */
- (BOOL)setCameraPreViewStatu:(BOOL)isPreView;


/**
 *  获取视频来电时是否支持预览
 *
 *  @return YES 支持预览 NO 不支持预览
 */
- (BOOL)isCameraPreviewStatu;




/**
 * 验证验证码
 *
 */

/**
 * App 通过SDK验证手机号，注册时调用验证请求
 * @param sid  String 开发者主账号
 * @param appid  String 开发者应用id
 * @param appName  String 开发者针对该应用的定义
 * @param codetype  int 验证码下发方式 1:短信 2:语音
 * @param phone  String 用户手机号
 * @param minutes int 验证码有效时间（秒）
 * @param business 业务类型 1: 注册，2：绑定手机
 * @return int 0: MID验证成功，1:已下发验证码到用户，2：手机号码无效
 */

-(void)getVerificationCode:(NSString *)sid withAppid:(NSString *)appid withAppName:(NSString *)appName withCodetype:(int)codetype withPhone:(NSString *)phone withSeconds:(int)seconds withBusiness:(int)business;

/**
 * 用户输入验证码提交验证
 * @param sid  String 开发者主账号
 * @param appid  String 开发者应用id
 * @param phone  String 用户手机号
 * @param code  String 验证码
 * @return int 0: 验证成功，1：验证码错误，2：验证码超时
 */
-(void)doVerificationCode:(NSString *)sid withAppid:(NSString *)appid withPhone:(NSString *)phone withVerifycode:(NSString *)verifycode;


///正式环境下日志开关
-(void)openSdkLog:(BOOL)isOpenSdkLog;


-(void)openMediaEngineLog:(BOOL)isOpenMediaEngineLog;


///续活接口
- (void) callIncomingPushRsp:(NSString*)callid  withVps:(NSInteger)vpsid withReason:(KCTReason*)reason;
- (void)incomingRspWhenBackground:(id)callID vpsid:(int)vpsid;

//- (BOOL)setAGCPlus:(int)compressionGain targetDbfs:(int)targetDbfs;

- (void)setSocketIpv6:(BOOL)isIpv6;
- (void)setIncomingCallMusicPath:(NSString *)path;

@end
