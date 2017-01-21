////
////  UCSVideoCallController.h
////  UCSVoipDemo
////
////  Created by wenqinglin on 14-5-22.
////  Copyright (c) 2014年 KCT. All rights reserved.
////
//
//#define kKeyboardBtnpng             @"call_interface_icon_01.png"
//#define kKeyboardBtnOnpng           @"call_interface_icon_01_on.png"
//#define kHandsfreeBtnpng            @"call_interface_icon_03.png"
//#define kHandsfreeBtnOnpng          @"call_interface_icon_03_on.png"
//#define kMuteBtnpng                 @"call_interface_icon_02.png"
//#define kMuteBtnOnpng               @"call_interface_icon_02_on.png"
//
#import <UIKit/UIKit.h>
#import "UCSPubEnum.h"
#import "UCSCommonClass.h"

//@class  VideoView;
@interface UCSVideoCallController : UIViewController
{
    int hhInt;
    int mmInt;
    int ssInt;
    NSTimer *timer;
    
    
}

@property (nonatomic,retain) NSString *callID;
@property (nonatomic,retain) NSString *callerName;
@property (nonatomic,retain) NSString *voipNo;

@property (assign,nonatomic) BOOL incomingCall; //WLS，2015-12-14，(处于被叫界面)

@property (nonatomic, assign) BOOL isActivity; //WLS，2015-12-14，是否是从后台进入前台

@property (copy,nonatomic) NSString * callDuration; // WLS，2015-12-18，(通话时间)
@property (assign,nonatomic) BOOL isCallNow; // WLS，2015-12-18，(是否处于通话状态)
@property (assign,nonatomic) BOOL beReject; // WLS，2015-12-21，(被叫是否拒绝接听)
@property (assign,nonatomic) BOOL hangupMySelf; // WLS，2016-11-19，(是否是自己点击了挂断按钮)

@property (copy,nonatomic) NSString * currentTime; // WLS，2016-01-13，(当前收到来电的时间或者发起通话的时间)


/*name:被叫人的姓名，用于界面的显示(自己选择)
 voipNop:被叫人的voip账号，用于网络免费电话(也可用于界面的显示,自己选择)
 */
- (UCSVideoCallController *)initWithCallerName:(NSString *)name  andVoipNo:(NSString *)voipNo;


-(void)responseVoipManagerStatus:(UCSCallStatus)event callID:(NSString*)callid data:(UCSReason *)data withVideoflag:(int)videoflag;


- (void)networkStatusChange:(NSInteger)currentNetwork networdDes:(NSString *)networdDes;

- (void)networkDetailChange:(NSString *)currentNetworkDetail;


@end
