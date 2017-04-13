//
//  KCTVOIPViewEngine.h
//  KCT_IM_Demo
//
//  Created by KCT on 15/12/11.
//  Copyright © 2015年 KCT. All rights reserved.
//

#import <Foundation/Foundation.h>


#define GetViewWidth(view) view.frame.size.width

#define GetViewHeight(view) view.frame.size.height

#define GetViewX(view) view.frame.origin.x

#define GetViewY(view) view.frame.origin.y

#define CurrentBounds [UIScreen mainScreen].bounds

#define CurrentWidth [UIScreen mainScreen].bounds.size.width

#define CurrentHeight [UIScreen mainScreen].bounds.size.height

#define DevicesScale ([UIScreen mainScreen].bounds.size.height==480?1.00:[UIScreen mainScreen].bounds.size.height==568?1.00:[UIScreen mainScreen].bounds.size.height==667?1.17:1.29)

#define Adaptation(n) (n)*DevicesScale

#define CenterPoint(x,y) ((x)-(y))/2.0 //居中


#define RGBColor(r,g,b) [UIColor colorWithRed:r/255.0 green:g/255.0 blue:b/255.0 alpha:1]

#define GetRGBColor(rgbValue) [UIColor colorWithRed:((float)((rgbValue & 0xFF0000) >> 16))/255.0 green:((float)((rgbValue & 0xFF00) >> 8))/255.0 blue:((float)(rgbValue & 0xFF))/255.0 alpha:1.0]

//ps(px*2)字体转化成pt字体
#define PSToPtFont(ps) (ps)*3/4
#define GetTextFont(n) CurrentHeight==480?(n):(CurrentHeight==568?(n):(CurrentHeight==667?(n+2):(n+4)))

#define KCTNotiLocalNotification @"NotiLocalNotification"


typedef enum{
    KCT_CallType_Answer, //通话类型，接听
    KCT_CallType_Cancel, //通话类型，取消
    KCT_CallType_Disanswer,//通话类型，未接听
    KCT_CallType_reject   //通话类型，被叫拒绝接听
}KCTCallAction;


typedef enum{
    KCT_voipCall, // WLS，2015-12-11，（语音通话主叫）
    KCT_incomingVoipCall, // WLS，2015-12-11，（语音通话被叫）
    KCT_videoCall, // WLS，2015-12-11，（视频通话主叫）
    KCT_incomingVideoCall // WLS，2015-12-11，（视频通话被叫）
}KCTCallType;

@interface KCTVOIPViewEngine : NSObject<KCTEngineUIDelegate>


@property (strong,nonatomic)NSString *callBalance; // WLS，2015-12-15，免费通话的剩余时间
@property (strong,nonatomic)NSMutableArray *addressListArray; //WLS，2015-12-19，通讯录数据
@property (strong,nonatomic)NSMutableArray *nickNameArray; //WLS，2016-01-08，昵称数组
@property (assign,nonatomic)BOOL isCalling; //WLS，2016-01-15，是否处于通话中
@property (assign,nonatomic)BOOL isCallKit;

+(KCTVOIPViewEngine *)getInstance;


/**
 @author WLS, 15-12-11 18:12:11
 
 发起通话
 
 @param callNumber 被叫userid或者被叫的手机号
 @param callType   发起通话类型
 */
- (void)makingCallViewCallNumber:(NSString *)callNumber callType:(KCTCallTypeEnum)callType callName:(NSString *)callName;


//移除通话界面 add by WLS 20151116
- (void)releaseViewControler:(UIViewController *)releaseVC;


- (void)InactiveCall:(NSMutableDictionary *)callDic;


/**
 @author WLS, 16-04-12 10:04:54
 
 显示拨号视图
 */
- (void)showKeyboardView;

/**
 @author WLS, 16-04-12 10:04:54
 
 退出拨号视图
 */
- (void)releaseKeyboardViewAnimation:(BOOL)animation;


/**
 @author WLS, 15-12-14 17:12:15
 
 设置自定义编解码
 */
- (void)setVideoDecAndVideoEnc;


/**
 @author WLS, 15-12-15 17:12:47
 
 查询剩余的通话时间
 */
- (void)getCallBalance;


/**
 @author WLS, 16-01-11 18:01:51
 
 写入文件
 
 @param str 需要写入文件的字符串
 */
- (void)WriteToSandBox:(NSString *)str;


+ (BOOL)isHeadphone;


@end
