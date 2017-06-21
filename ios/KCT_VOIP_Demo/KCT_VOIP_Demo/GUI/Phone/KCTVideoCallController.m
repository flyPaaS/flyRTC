////
////  KCTVideoCallController.m
//// KCTVOIPDemo
////
////  Created by wenqinglin on 14-5-22.
////  Copyright (c) 2014年 KCT. All rights reserved.
////
//
//
//
//
#import "KCTVideoCallController.h"
#import "KCTCommonClass.h"
#import "KCTPubEnum.h"
#import <mach/mach.h>

@class KCTVideoAttr;
@class Resolutions;
@class VideoView;

@interface KCTVideoCallController ()


@property (strong,nonatomic) UIImageView * backgroundView; // WLS，2015-12-14，背景视图


@property (strong,nonatomic) UIView *callBackView; // WLS，2015-12-14，通话中所有按钮的背景视图
@property (strong,nonatomic) UIButton *hangupButton; // WLS，2015-12-14，挂断按钮
@property (strong,nonatomic) UILabel *voipNumberLabel; // WLS，2015-12-14，对方号码标题
@property (strong,nonatomic) UIButton *callTimeLabel; // WLS，2015-12-14，通话时间标题
@property (strong,nonatomic) UIView *callFunctionView; // WLS，2015-12-14，通话中功能按钮视图
@property (strong,nonatomic) UIButton *handFreeButton; // WLS，2015-12-14，免提按钮
@property (strong,nonatomic) UIButton *closeCameraButton; // WLS，2015-12-14，关闭摄像头按钮
@property (strong,nonatomic) UIButton *muteButton; // WLS，2015-12-14，静音按钮
@property (strong,nonatomic) UIButton *switchCameraButton; // WLS，2015-12-14，切换摄像头按钮
@property (strong,nonatomic) UIButton *answerButton; // WLS，2015-12-14，接听按钮(被叫使用)


@property (strong,nonatomic) UIView *videoLocationView; // WLS，2015-12-14，本地视频视图
@property (strong,nonatomic) UIView *videoRemoteView; // WLS，2015-12-14，远程视频视图


@property (assign,nonatomic) BOOL isReleaseCall; // WLS，2015-12-19，是否已经移除过界面


@property (strong,nonatomic) UIView * incomingView; // WLS，2015-12-19，来电时候的界面
@property (strong,nonatomic) UILabel * informationLabel; // 展示信息的label
@end

@implementation KCTVideoCallController



#pragma mark - 界面规划
/**
 @author WLS, 15-12-14 14:12:23
 
 视频通话界面
 */
- (void)makeVideoCallView{
    
    
    
    /**
     @author WLS, 15-12-14 14:12:47
     
     本地视频窗口和远程视频窗口
     */
    self.videoRemoteView = [[KCTFuncEngine getInstance] allocCameraViewWithFrame:self.backgroundView.bounds];
    self.videoRemoteView.backgroundColor = [UIColor clearColor];
    [self.backgroundView addSubview:self.videoRemoteView];

    self.videoLocationView = [[KCTFuncEngine getInstance] allocCameraViewWithFrame:CGRectMake(Adaptation(220), Adaptation(220), Adaptation(90), Adaptation(120))];
    self.videoLocationView.backgroundColor = [UIColor clearColor];
    [self.backgroundView addSubview:self.videoLocationView];
    if (CurrentHeight !=480) {
        CGRect frame = self.videoLocationView.frame;
        frame.origin.y = Adaptation(270);
        self.videoLocationView.frame = frame;

    }
    
    
    
    
    
    
    /**
     @author WLS, 15-12-14 14:12:47
     
     通话过程中所有按钮的背景视图
     */
    self.callBackView = [[UIView alloc] initWithFrame:self.view.bounds];
    self.callBackView.backgroundColor = [UIColor clearColor];
    [self.backgroundView addSubview:self.callBackView];
    
    /**
     @author WLS, 16-02-24 16:02:52
     
     将本地视图调到 通话过程中的按钮背景视图之上。因为本地视频视图需要触发点击事件，否则点击到callbackview
     */
    [self.backgroundView insertSubview:self.videoLocationView aboveSubview:self.callBackView];
    
    
    /**
     @author WLS, 15-12-14 11:21:59
     
     来电号码或者被叫号码
     */
    self.voipNumberLabel = [[UILabel alloc] initWithFrame:CGRectMake(Adaptation(10), Adaptation(35), Adaptation(200), Adaptation(20))];
    self.voipNumberLabel.font = [UIFont systemFontOfSize:GetTextFont(21)];
    self.voipNumberLabel.textColor = [UIColor whiteColor];
    self.voipNumberLabel.text = self.callerName;
    [self.backgroundView addSubview:self.voipNumberLabel];
    
    
    /**
     @author WLS, 15-12-14 11:21:59
     
     对方通话状态或者时间
     */
    self.callTimeLabel = [UIButton buttonWithType:UIButtonTypeCustom];
    self.callTimeLabel.enabled = NO;
    self.callTimeLabel.frame = CGRectMake(Adaptation(10), GetViewHeight(self.voipNumberLabel)+GetViewY(self.voipNumberLabel)+ Adaptation(5), Adaptation(200),GetViewHeight(self.voipNumberLabel));
    self.callTimeLabel.titleLabel.font = [UIFont systemFontOfSize:GetTextFont(13)];
    self.callTimeLabel.contentHorizontalAlignment = UIControlContentHorizontalAlignmentLeft;
    [self.callTimeLabel setTitleColor:[UIColor grayColor] forState:UIControlStateDisabled];
    [self.callTimeLabel setTitle:@"呼叫请求中" forState:UIControlStateDisabled];
    [self.backgroundView addSubview:self.callTimeLabel];
    if (self.incomingCall) {
        /**
         @author WLS, 15-12-11 11:21:59
         
         如果处于来电状态 则通话状态的标题暂时先隐藏 ，等到通话建立起来，在显示
         */
        self.callTimeLabel.hidden = YES;
        
    }
    
    
    
    /**
     @author WLS, 15-12-14 14:14:47
     
     静音 扬声器 键盘按钮
     */
    [self makeCallFuntiongButton];
    
    
    /**
     @author WLS, 15-12-14 14:13:47
     
     挂断按钮如果是来电，则还有接听按钮
     */
    self.hangupButton = [UIButton buttonWithType:UIButtonTypeCustom];
    [self.hangupButton setImage:[UIImage imageNamed:@"挂断"] forState:UIControlStateNormal];
    [self.hangupButton setImage:[UIImage imageNamed:@"挂断-down"] forState:UIControlStateDisabled];
    [self.hangupButton addTarget:self action:@selector(hangupCall) forControlEvents:UIControlEventTouchUpInside];
    [self.callBackView addSubview:self.hangupButton];
    if (self.incomingCall) {
        self.answerButton = [UIButton buttonWithType:UIButtonTypeCustom];
        self.answerButton.backgroundColor = [UIColor whiteColor];
        [self.answerButton addTarget:self action:@selector(answerCall) forControlEvents:UIControlEventTouchUpInside];
        [self.callBackView addSubview:self.answerButton];
        
        self.hangupButton.frame = CGRectMake((GetViewWidth(self.callBackView)-Adaptation(60)*2)/3, Adaptation(470), Adaptation(60), Adaptation(60));
        self.answerButton.frame = CGRectMake(GetViewWidth(self.hangupButton)+GetViewX(self.hangupButton)*2, Adaptation(470), GetViewWidth(self.hangupButton), GetViewHeight(self.hangupButton));
        
        if (CurrentHeight == 480) {
            CGRect frame = self.hangupButton.frame;
            frame.origin.y = Adaptation(410);
            self.hangupButton.frame = frame;
            
            
            frame = self.answerButton.frame;
            frame.origin.y = Adaptation(410);
            self.answerButton.frame = frame;
        }

    }else{
        [self changeHangupButtonFrame];
        
    }
    
    
    /**
     @author WLS, 15-12-14 15:12:07
     
     切换摄像头按钮
    */
    self.switchCameraButton = [UIButton buttonWithType:UIButtonTypeCustom];
    self.switchCameraButton.frame = CGRectMake(GetViewWidth(self.callBackView)- Adaptation(50), GetViewY(self.voipNumberLabel)-Adaptation(10), Adaptation(50) , Adaptation(45));
    [self.switchCameraButton setImage:[UIImage imageNamed:@"前置摄影头切换"] forState:UIControlStateNormal];
//    self.switchCameraButton.contentHorizontalAlignment = UIControlContentHorizontalAlignmentRight;
    self.switchCameraButton.tag = 1;
    [self.switchCameraButton addTarget:self action:@selector(switchCameraButtonClick:) forControlEvents:UIControlEventTouchUpInside];
    [self.callBackView addSubview:self.switchCameraButton];
    
    /**
     @author WLS, 16-01-25 14:01:01
     
     通话未接通前，暂时隐藏按钮
     */
    self.switchCameraButton.hidden = YES;
    

    
}


/**
 @author WLS, 15-12-14 14:14:47
 
 静音 扬声器 键盘按钮
 */
- (void)makeCallFuntiongButton{

    
    
    self.callFunctionView = [[UIView alloc] initWithFrame:CGRectMake(Adaptation(30), Adaptation(395), GetViewWidth(self.callBackView) - Adaptation(60) , Adaptation(60))];
    if (CurrentHeight == 480) {
        CGRect frame = self.callFunctionView.frame;
        frame.origin.y = Adaptation(330);
        self.callFunctionView.frame = frame;
        
    }
    self.callFunctionView.backgroundColor = [UIColor clearColor];
    [self.callBackView addSubview:self.callFunctionView];
    
    
    CGFloat spaceWidth = (GetViewWidth(self.callFunctionView)  - GetViewHeight(self.callFunctionView)*3)/2.0;
    NSArray * imageArray = @[@"静音",@"摄像头",@"扬声器"];
    NSArray * selectImageArray = @[@"静音-down",@"摄像头-down",@"扬声器-down"];
    NSArray * titleArray = @[@"静音",@"摄像头",@"扬声器"];
    for (int i = 0; i<3; i++) {
        
        UIButton * button = [UIButton buttonWithType:UIButtonTypeCustom];
        button.frame = CGRectMake(i * (GetViewHeight(self.callFunctionView)+spaceWidth), 0, GetViewHeight(self.callFunctionView), GetViewHeight(self.callFunctionView));
        button.tag = i;
        [button setImage:[UIImage imageNamed:imageArray[i]] forState:UIControlStateNormal];
        [button setImage:[UIImage imageNamed:selectImageArray[i]] forState:UIControlStateSelected];
        [button setTitle:titleArray[i] forState:UIControlStateNormal];
        [button setTitleColor:RGBACOLOR(255, 255, 255, 0.5) forState:UIControlStateNormal];
        button.titleLabel.font = [UIFont systemFontOfSize:GetTextFont(11)];
        button.imageEdgeInsets = UIEdgeInsetsMake(5,10,21,button.titleLabel.bounds.size.width);
        button.titleEdgeInsets = UIEdgeInsetsMake(40, -button.titleLabel.bounds.size.width-40, 0, 0);
        [button addTarget:self action:@selector(funtionButtonClick:) forControlEvents:UIControlEventTouchUpInside];
        [self.callFunctionView addSubview:button];
        
        
        if (i == 0) {
            self.muteButton = button;
        }else if (i == 1){
            self.closeCameraButton = button;
        }else{
            self.handFreeButton = button;
        }
        
    }
    
    
    /**
     @author WLS, 15-12-11 12:12:37
     
     刚开始需要隐藏功能按钮，只有建立通话了才会显示
     */
//    if (self.incomingCall) {
    self.callFunctionView.hidden = YES;
//    }

    
    
}

/**
 @author WLS, 15-12-14 14:14:47
 
 修改挂断按钮的frame
 */
- (void)changeHangupButtonFrame{
    
    self.hangupButton.frame = CGRectMake(CenterPoint(GetViewWidth(self.callBackView), Adaptation(60)), Adaptation(470), Adaptation(60), Adaptation(60));
    
    if (CurrentHeight == 480) {
        CGRect frame = self.hangupButton.frame;
        frame.origin.y = Adaptation(410);
        self.hangupButton.frame = frame;
    }
    /**
     @author WLS, 15-12-14 13:12:36
     
     如果接听按钮存在  则移除接听按钮
     */
    if (self.answerButton) {
        [self.incomingView removeFromSuperview];
        self.incomingView = nil;
        
        [self.answerButton removeFromSuperview];
        self.answerButton = nil;
    }
    
}


/**
 @author WLS, 15-12-19 15:12:13
 
 被叫的初始界面
 */
- (void)makeIncomingView{
    if ([KCTVOIPViewEngine getInstance].isCallKit) {
        [self answerCall];
    }
    
    self.incomingView = [[UIView alloc] initWithFrame:self.view.bounds];
    self.incomingView.backgroundColor = self.backgroundView.backgroundColor;
    [self.backgroundView addSubview:self.incomingView];
    
    //add by wenqinglin
    [self.backgroundView bringSubviewToFront:self.videoLocationView];
    /**
     @author WLS, 15-12-11 11:21:59
     
     头像
     */
    UIImageView * iconView = [[UIImageView alloc] initWithFrame:CGRectMake(CenterPoint(GetViewWidth(self.incomingView), Adaptation(110)), Adaptation(90), Adaptation(110), Adaptation(110))];
    iconView.layer.cornerRadius = GetViewHeight(iconView)/2.0;
    iconView.layer.masksToBounds = YES;
    iconView.image = [UIImage imageNamed:@"默认头像"];
    [self.incomingView addSubview:iconView];
    
    if (CurrentHeight == 480) {
        CGRect frame = iconView.frame;
        frame.origin.y = Adaptation(60);
        iconView.frame = frame;
    }
    
    
    /**
     @author WLS, 15-12-11 11:21:59
     
     来电号码或者被叫号码
     */
    UILabel * voipNumberLabel = [[UILabel alloc] initWithFrame:CGRectMake(0, Adaptation(215),GetViewWidth(self.incomingView), Adaptation(20))];
    voipNumberLabel.font = [UIFont systemFontOfSize:GetTextFont(21)];
    voipNumberLabel.textAlignment = NSTextAlignmentCenter;
    voipNumberLabel.textColor = [UIColor whiteColor];
    voipNumberLabel.text = self.callerName;
    [self.incomingView addSubview:voipNumberLabel];
    if (CurrentHeight == 480) {
        CGRect frame = voipNumberLabel.frame;
        frame.origin.y = Adaptation(180);
        voipNumberLabel.frame = frame;
    }
    
    
    
    /**
     @author WLS, 15-12-11 11:21:59
     
     对方通话状态或者时间
     */
    UIButton * callTimeLabel = [UIButton buttonWithType:UIButtonTypeCustom];
    callTimeLabel.enabled = NO;
    callTimeLabel.frame = CGRectMake(GetViewX(voipNumberLabel), GetViewHeight(voipNumberLabel)+GetViewY(voipNumberLabel)+ Adaptation(5), GetViewWidth(voipNumberLabel), Adaptation(20));
    callTimeLabel.titleLabel.font = [UIFont systemFontOfSize:GetTextFont(13)];
    [callTimeLabel setTitleColor:[UIColor grayColor] forState:UIControlStateDisabled];
    [callTimeLabel setTitle:@"视频聊天" forState:UIControlStateDisabled];
    if ([[KCTTcpClient sharedTcpClientManager] getCurrentNetWorkStatus]==KCTReachableVia2G) {
        [callTimeLabel setTitle:@"视频聊天(当前网络状态差)" forState:UIControlStateDisabled];
        
    }
    [self.incomingView addSubview:callTimeLabel];
    
    
    
    /**
     @author WLS, 15-12-11 11:21:59
     
     挂断按钮如果是来电，则还有接听按钮
     */
    UIButton *hangupButton = [UIButton buttonWithType:UIButtonTypeCustom];
    [hangupButton addTarget:self action:@selector(hangupCall) forControlEvents:UIControlEventTouchUpInside];
    [hangupButton setImage:[UIImage imageNamed:@"挂断"] forState:UIControlStateNormal];
    [hangupButton setImage:[UIImage imageNamed:@"挂断-down"] forState:UIControlStateDisabled];
    [self.incomingView addSubview:hangupButton];
    if (self.incomingCall) {
        UIButton *answerButton = [UIButton buttonWithType:UIButtonTypeCustom];
        [answerButton setImage:[UIImage imageNamed:@"接听"] forState:UIControlStateNormal];
        [answerButton setImage:[UIImage imageNamed:@"接听-down"] forState:UIControlStateDisabled];
        [answerButton addTarget:self action:@selector(answerCall) forControlEvents:UIControlEventTouchUpInside];
        [self.incomingView addSubview:answerButton];
        
        hangupButton.frame = CGRectMake((GetViewWidth(self.incomingView)-Adaptation(60)*2)/3, Adaptation(470), Adaptation(60), Adaptation(60));
        answerButton.frame = CGRectMake(GetViewWidth(hangupButton)+GetViewX(hangupButton)*2, Adaptation(470), GetViewWidth(hangupButton), GetViewHeight(hangupButton));
        if (CurrentHeight == 480) {
            CGRect frame = hangupButton.frame;
            frame.origin.y = Adaptation(410);
            hangupButton.frame = frame;
            
            
            frame = answerButton.frame;
            frame.origin.y = Adaptation(410);
            answerButton.frame = frame;
            
        }
    }
}

- (UILabel *)informationLabel{
    if (_informationLabel == nil) {
        _informationLabel = [[UILabel alloc] initWithFrame:CGRectMake(0,GetViewY(self.callTimeLabel)+Adaptation(5), CurrentWidth, Adaptation(300))];
//        _informationLabel = [[UILabel alloc] initWithFrame:CGRectMake(0, GetViewHeight(self.callTimeLabel)+GetViewY(self.callTimeLabel)+Adaptation(5), CurrentWidth, Adaptation(300))];
        _informationLabel.numberOfLines = 0;
        _informationLabel.hidden = YES;
        _informationLabel.font = [UIFont systemFontOfSize:GetTextFont(12)];
        _informationLabel.textColor = [UIColor redColor];
        _informationLabel.backgroundColor = [UIColor clearColor];
    }
    return _informationLabel;
}



#pragma mark - 生命周期
- (void)dealloc
{
    [self removeRotation];
    
    [self.hangupButton removeFromSuperview];
    self.hangupButton = nil;
    
    [self.voipNumberLabel removeFromSuperview];
    self.voipNumberLabel = nil;
    
    [self.callTimeLabel removeFromSuperview];
    self.callTimeLabel = nil;
    
    [self.handFreeButton removeFromSuperview];
    self.handFreeButton = nil;
    
    [self.closeCameraButton removeFromSuperview];
    self.closeCameraButton = nil;
    
    [self.muteButton removeFromSuperview];
    self.muteButton = nil;
    
    [self.switchCameraButton removeFromSuperview];
    self.switchCameraButton = nil;
    
    [self.answerButton removeFromSuperview];
    self.answerButton = nil;
    
    [self.videoLocationView removeFromSuperview];
    self.videoLocationView = nil;
    
    [self.videoRemoteView removeFromSuperview];
    self.videoRemoteView = nil;
    
    [self.callFunctionView removeFromSuperview];
    self.callFunctionView = nil;
    
    [self.callBackView removeFromSuperview];
    self.callBackView = nil;
    
    [self.backgroundView removeFromSuperview];
    self.backgroundView = nil;
    
    
    [self.incomingView removeFromSuperview];
    self.incomingView =nil;
    

    self.callID = nil;
    self.callerName = nil;
    self.voipNo = nil;
    

}


- (KCTVideoCallController *)initWithCallerName:(NSString *)name  andVoipNo:(NSString *)voipNo{
    if (self = [super init])
    {
        self.callerName = name;
        self.voipNo = voipNo;
        self.isCallNow = NO;
        self.callDuration = @"";
        hhInt = 0;
        mmInt = 0;
        ssInt = 0;
        self.isReleaseCall = NO;
        self.beReject = NO;
        [[KCTFuncEngine getInstance] setSpeakerphone:NO];
        return self;
    }
    
    return nil;
}


- (void)viewDidLoad
{
    [super viewDidLoad];
    
    self.currentTime = [NSString stringWithFormat:@"%ld",time(NULL)];
    self.hangupMySelf = NO;
    
    self.backgroundView = [[UIImageView alloc] initWithFrame:self.view.bounds];
    self.backgroundView.backgroundColor = RGBACOLOR(22, 32, 39, 1);
    self.backgroundView.userInteractionEnabled = YES;
    [self.view addSubview:self.backgroundView];
    
    
    [self makeVideoCallView];
    
    if (self.incomingCall) {
        [self makeIncomingView];
    }
    
    
    /**
     @author WLS, 15-12-14 12:12:42
     
     注册旋转通知
    */
    [self registerRotation:@selector(getRotationAngle)];
    
    
    // 设置不自动进入锁屏待机状态
    [[UIApplication sharedApplication] setIdleTimerDisabled:YES];
    
    // 需要在通话中成为firstResponder，否则切换回前台后，听不到声音
    [self becomeFirstResponder];
    
    
    
    /**
     @author WLS, 15-12-14 15:12:57
     
     初始化视频界面
     */
    [[KCTFuncEngine getInstance] initCameraConfig:self.videoLocationView withRemoteVideoView:self.videoRemoteView];
    
    
    
    [self addNotification];
    // 添加1个手指5次点击事件
    UITapGestureRecognizer * thirdTap = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(thirdTap:)];
    thirdTap.numberOfTapsRequired = 3;
    thirdTap.numberOfTouchesRequired = 1;
    [self.videoLocationView addGestureRecognizer:thirdTap];
    // 添加信息label
    [self.view addSubview:self.informationLabel];
}




// 需要在通话中成为firstResponder，否则切换回前台后，听不到声音
- (BOOL)canBecomeFirstResponder {
    return YES;
}


- (void)viewWillAppear:(BOOL)animated
{
    
    [[UIApplication sharedApplication] beginReceivingRemoteControlEvents];
    
    [super viewWillAppear:animated];
    
    if (self.isActivity) {

        //从后台进入前台 强制设置为前置摄像头 (WLS)
        [[KCTFuncEngine getInstance] switchCameraDevice:CAMERA_FRONT];

    }
}

- (void)viewWillDisappear:(BOOL)animated
{
    //     [self.kctFuncEngine setCameraTorchMode:NO];//设置闪光为关闭状态
    [[UIApplication sharedApplication] endReceivingRemoteControlEvents];
    
    [super viewWillDisappear:animated];
    
    [self removeNotification];
    
    
}

- (void)viewDidDisappear:(BOOL)animated
{
    //globalisVoipView = NO;
    [super viewDidDisappear:animated];
    
    // 设置不自动进入锁屏待机状态
    [[UIApplication sharedApplication] setIdleTimerDisabled:NO];
    
    // 需要在通话中成为firstResponder，否则切换回前台后，听不到声音
    [self resignFirstResponder];
}



- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.ƒ
}



#pragma mark - 按钮点击事件
- (void)funtionButtonClick:(UIButton *)button{
    switch (button.tag) {
        case 0:{
            /**
             @author WLS, 15-12-11 12:12:13
             
             静音
             */
            
            [self mute];
            
        }
            break;
        case 1:{
            /**
             @author WLS, 15-12-11 12:12:20
             
             关闭摄像头
             */
            [self closeCamera];
        }
            break;
        case 2:{
            /**
             @author WLS, 15-12-11 12:12:29
             
             免提
             */
            [self handfree];
        }
            break;
        default:
            break;
    }
    
    
}
/**
 @author WLS, 15-12-14 16:12:46
 
 挂断电话
 */
- (void)hangupCall
{
    AppDelegate *app = (AppDelegate *)[UIApplication sharedApplication].delegate;
    NSLog(@"KCTVideoCallController 660");
    [app stopCall];
    [KCTVOIPViewEngine getInstance].isCallKit = NO;
    
    [self setCallDuration];
    
    self.hangupButton.enabled = NO;
    self.answerButton.enabled = NO;
    self.hangupMySelf = YES;
//    [self.callTimeLabel setTitle:@"正在挂机" forState:UIControlStateDisabled];
    [[KCTFuncEngine getInstance] hangUp:self.callID];
    
    [[KCTVOIPViewEngine getInstance] WriteToSandBox:[NSString stringWithFormat:@"挂断通话：%@",[KCTFuncEngine getInstance]]];

    
    [NSTimer scheduledTimerWithTimeInterval:2.0f target:self selector:@selector(backFront) userInfo:nil repeats:NO];
    
    
}


/**
 @author WLS, 15-12-14 16:12:43
 
 接听电话
 */
- (void)answerCall{
    
    [[KCTFuncEngine getInstance] answer:self.callID];
}


/**
 @author WLS, 15-12-14 15:12:51
 
 切换摄像头
 */
- (void)switchCameraButtonClick:(UIButton *)button{
    if (button.tag == 1) {
        /**
         @author WLS, 15-12-14 15:12:42
         
         后置摄像头
         */
        [[KCTFuncEngine getInstance] switchCameraDevice:CAMERA_REAR];
        button.tag = 0;
    }else if (button.tag == 0){
        /**
         @author WLS, 15-12-14 15:12:49
         
         前置摄像头
         */
        [[KCTFuncEngine getInstance] switchCameraDevice:CAMERA_FRONT];
        button.tag = 1;
        
    }
    
    
}

#pragma mark - 功能
// 1个手指 5击事件
- (void)thirdTap:(UITapGestureRecognizer *)thirdTap{
    NSLog(@"1个触摸点 5次点击事件触发");
    self.informationLabel.hidden = NO;
}


- (void)touchesBegan:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event{
    
    
    UITouch * touch = [touches anyObject];
    if (touch.view == self.videoLocationView && self.informationLabel.hidden == NO) {
        
        self.informationLabel.hidden = YES;
        return;
    }
    
    if (self.callFunctionView.hidden) {
        /**
         @author WLS, 15-12-14 17:12:16
         
         不处于通话中，未接听
         */
        
    }else{
        /**
         @author WLS, 15-12-14 17:12:16
         
         通话中，已接听
         */
        self.callBackView.hidden = !self.callBackView.hidden;
        
    }
    
}
//免提事件
- (void)handfree
{
//    if ([[KCTFuncEngine getInstance] isHeadPhone]) {
//        //处于耳机模式 不允许设置免提 add by WLS 20151106
//        return;
//    }

    if ([KCTVOIPViewEngine  isHeadphone]) {
        //处于耳机模式 不允许设置免提 add by WLS 20151106
        return;
    }
    
    if (self.isCallNow == NO) {
        /**
         @author WLS, 15-12-21 11:12:12
         
         只有通话后才能设置扬声器
         */
        return;
    }
    
    //免提关：NO 免提开：YES
    BOOL returnValue = [[KCTFuncEngine getInstance] isSpeakerphoneOn];
    
    if (returnValue==NO)
    {
        [[KCTFuncEngine getInstance] setSpeakerphone:YES];
        self.handFreeButton.selected = YES;
    }else{
        [[KCTFuncEngine getInstance] setSpeakerphone:NO];
        self.handFreeButton.selected = NO;
        
    }
}
//静音事件
- (void)mute
{
    if (self.isCallNow ==NO) {
        return;
    }
    
    BOOL muteFlag = [[KCTFuncEngine getInstance] isMicMute];
    if (muteFlag == NO) {
        self.muteButton.selected = YES;;
        [[KCTFuncEngine getInstance] setMicMute:YES];//设置为静音
    }
    else
    {
        self.muteButton.selected = NO;;
        [[KCTFuncEngine getInstance] setMicMute:NO];//设置为非静音
    }
}


//关闭摄像头
- (void)closeCamera{
    
    if (self.isCallNow == NO) {
        return;
    }
    
    self.closeCameraButton.selected = !self.closeCameraButton.selected;
    if (self.closeCameraButton.selected) {
        //关闭
        [[KCTFuncEngine getInstance] switchVideoMode:CAMERA_RECEIVE];

    }else{
        //打开
        [[KCTFuncEngine getInstance] switchVideoMode:CAMERA_NORMAL];

    }
    
}


/**
 @author WLS, 15-12-14 17:12:10
 
 释放界面
 */
- (void)backFront
{
    if (self.isReleaseCall) {
        return;
    }
    self.isReleaseCall = YES;
    
    
    [[KCTVOIPViewEngine getInstance]releaseViewControler:self];
    
    
}


/**
 @author WLS, 15-12-14 15:12:34
 
 旋转屏幕角度
 
 @param rotationAngle 角度
 */
- (void)rotationCameraAngle:(unsigned int)rotationAngle{
    
    [[KCTFuncEngine getInstance] setRotationVideo:rotationAngle withReciviedRotation:rotationAngle];
    
}





#pragma mark - ModelEngineUIDelegate

/**
 @author WLS, 15-12-19 15:12:49
 
 网络状态回调
 
 @param currentNetwork 当前的网络状态
 */
- (void)networkStatusChange:(NSInteger)currentNetwork networdDes:(NSString *)networdDes{
    NSString * imageStr = nil;
    switch (currentNetwork) {
        case KCTNetwork_Bad:
        {
            //差
            imageStr = @"视频信号-无";
        }
            break;
        case KCTNetwork_General:
        {
            //一般
            imageStr = @"视频信号1";

        }
            break;
        case KCTNetwork_Nice:
        {
            //良好
            imageStr = @"视频信号2";

        }
            break;
        case KCTNetwork_Well:
        {
            //优秀
            imageStr = @"视频信号3";

        }
            break;
        default:
            break;
    }
    [self.callTimeLabel setImage:[UIImage imageNamed:imageStr] forState:UIControlStateDisabled];
    self.informationLabel.text = networdDes;
}

- (void)networkDetailChange:(NSString *)currentNetworkDetail{
    
    //self.informationLabel.text = [NSString stringWithFormat:@"%@\nCPU : %0.2f",currentNetworkDetail,[self getCurrentCPU]*100];
    
}


-(void)responseVoipManagerStatus:(KCTCallStatus)event callID:(NSString*)callid data:(KCTReason *)data withVideoflag:(int)videoflag
{
    
    
    self.callTimeLabel.hidden = NO;
    
    switch (event)
    {
        case KCTCallStatus_Alerting:{
            [self.callTimeLabel setTitle:@"对方正在响铃" forState:UIControlStateDisabled];
            
        }
            break;
        case KCTCallStatus_Answered:
        {
            self.isCallNow = YES;
            
            [self.callTimeLabel setTitle:@"00:00" forState:UIControlStateDisabled];
            
            if (![timer isValid])
            {
                timer = [NSTimer timerWithTimeInterval:1.0f target:self selector:@selector(updateRealtimeLabel) userInfo:nil repeats:YES];
                [[NSRunLoop currentRunLoop] addTimer:timer forMode:NSDefaultRunLoopMode];
                [timer fire];
            }
            
            /**
             @author WLS, 15-12-11 17:12:12
             
             当通话接通后，显示功能按钮，并调整挂断按钮的位置
             */
            self.callFunctionView.hidden = NO;
            self.switchCameraButton.hidden = NO;
            [self handfree];
            
            [self changeHangupButtonFrame];
            
        }
            break;
            
        case KCTCallStatus_Released:
        {
            
            AppDelegate *app = (AppDelegate *)[UIApplication sharedApplication].delegate;
            NSLog(@"KCTVideoCallCongtroller 964");
            [app stopCall];
            
            [self setCallDuration];
            
            self.hangupButton.enabled = NO;
            [self.callTimeLabel setImage:nil forState:UIControlStateDisabled];
//            if (self.incomingCall) {
//                [self.callTimeLabel setTitle:@"已挂机" forState:UIControlStateDisabled];
//                
//            }else{
//                [self.callTimeLabel setTitle:data.msg forState:UIControlStateDisabled];
//                
//            }
            
            
            if (data.reason == 402016) {
                [self.callTimeLabel setTitle:@"对方已挂机" forState:UIControlStateDisabled];
            }else if (data.reason == 402012) {
                /**
                 @author WLS, 15-12-21 11:12:04
                 
                 被叫拒绝接听
                 */
                self.beReject = YES;
                [self.callTimeLabel setTitle:@"对方拒绝接听" forState:UIControlStateDisabled];
            }else if (data.reason == 402013){
                
                [self.callTimeLabel setTitle:data.msg forState:UIControlStateDisabled];
                
            }else{
                
                if (!_isCallNow && !self.hangupMySelf && !self.incomingCall) {
                    [self.callTimeLabel setTitle:@"对方无应答" forState:UIControlStateDisabled];
                }
            }
            [NSTimer scheduledTimerWithTimeInterval:1.0f target:self selector:@selector(backFront) userInfo:nil repeats:NO];
            
        }
            break;
        case KCTCallStatus_Failed:
        {
            self.hangupButton.enabled = NO;
            
            [self.callTimeLabel setImage:nil forState:UIControlStateDisabled];
            [self.callTimeLabel setTitle:data.msg forState:UIControlStateDisabled];
            if (data.reason==402050) {
                [self.callTimeLabel setTitle:@"对方无应答" forState:UIControlStateDisabled];
            }
            
            [NSTimer scheduledTimerWithTimeInterval:1.0f target:self selector:@selector(backFront) userInfo:nil repeats:NO];
            
        }
            break;
        case KCTCallStatus_Transfered:
        {
            self.answerButton.enabled = NO;
            self.hangupButton.enabled = NO;
            [NSTimer scheduledTimerWithTimeInterval:1.0f target:self selector:@selector(backFront) userInfo:nil repeats:NO];
        }
            break;
        case KCTCallStatus_Pasused:
        {
            [self.callTimeLabel setImage:nil forState:UIControlStateDisabled];
            [self.callTimeLabel setTitle:@"呼叫保持" forState:UIControlStateDisabled];
        }
            break;
        default:
            break;
    }
    
    
    
}


- (void)updateRealtimeLabel
{
    ssInt +=1;
    if (ssInt >= 60) {
        mmInt += 1;
        ssInt -= 60;
        if (mmInt >=  60) {
            hhInt += 1;
            mmInt -= 60;
            if (hhInt >= 24) {
                hhInt = 0;
            }
        }
    }
    
    if(ssInt > 0 && ssInt % 4 == 0 )
    {
        
    }
    if (hhInt > 0) {
        [self.callTimeLabel setTitle:[NSString stringWithFormat:@"%02d:%02d:%02d",hhInt,mmInt,ssInt] forState:UIControlStateDisabled];
        self.callDuration = self.callTimeLabel.titleLabel.text;
    }
    else
    {
        [self.callTimeLabel setTitle:[NSString stringWithFormat:@"%02d:%02d",mmInt,ssInt] forState:UIControlStateDisabled];
        self.callDuration = self.callTimeLabel.titleLabel.text;
        
    }
}



/**
 @author WLS, 15-12-18 16:12:11
 
 设置通话时间
 */
- (void)setCallDuration{
    
    if ([timer isValid])
    {
        [timer invalidate];
        timer = nil;
    }
    
    if (self.isCallNow) {
        /**
         @author WLS, 15-12-18 16:12:39
         
         如果处于通话状态，挂断或者是异常挂断，保存通话时间
         */
        if ([self.callTimeLabel.titleLabel.text isEqualToString:@"正在挂机"]||
            [self.callTimeLabel.titleLabel.text componentsSeparatedByString:@":"].count <=1) {
        }else{
            self.callDuration = self.callTimeLabel.titleLabel.text;
            
        }
    }
}




#pragma mark - 通知

//添加通知
-(void)addNotification{
    //注册耳机监听 add by WLS 20151106
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(headPhoneChange) name:KCTSNotiHeadPhone object:nil];
    /**
     @author WLS, 15-12-15 09:12:20
     
     踢线通知
     */
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(KickOffNoti) name:TCPKickOffNotification object:nil];
}
//移除通知
-(void)removeNotification{
    [[NSNotificationCenter defaultCenter] removeObserver:self name:KCTSNotiHeadPhone object:nil];
    [[NSNotificationCenter defaultCenter] removeObserver:self name:TCPKickOffNotification object:nil];

}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
    return (interfaceOrientation == UIInterfaceOrientationPortrait);
}


//注册旋转通知 add by WLS
-(void)registerRotation:(SEL)method
{
    
    [[UIDevice currentDevice] beginGeneratingDeviceOrientationNotifications];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:method
                                                 name:UIDeviceOrientationDidChangeNotification object:nil];
}
//删除旋转通知 add by WLS
-(void)removeRotation
{
    [[NSNotificationCenter defaultCenter] removeObserver:self];
    [[UIDevice currentDevice] endGeneratingDeviceOrientationNotifications];
}


//获取屏幕旋转 add by WLS
- (void)getRotationAngle{
    
    //组件固定一个方向  add by wenqinglin
    [self rotationCameraAngle:0];
    
    /*
    //获得设备方向
    UIDeviceOrientation deviceOrientation = [UIDevice currentDevice].orientation;
    
    if (deviceOrientation == UIDeviceOrientationPortrait) {
        //竖屏  home键在底部
        [self rotationCameraAngle:0];
        
    }
    else if (deviceOrientation == UIDeviceOrientationPortraitUpsideDown) {
        //竖屏  home键在顶部
        [self rotationCameraAngle:180];
        
    }
    else if (deviceOrientation == UIDeviceOrientationLandscapeLeft) {
        //横屏 home键再左边
        [self rotationCameraAngle:90];
    }
    else if (deviceOrientation == UIDeviceOrientationLandscapeRight) {
        [self rotationCameraAngle:270];
    }else{
        
        [self rotationCameraAngle:0];
        
    }
    */
}


//耳机监听 add by Wls 20151106
- (void)headPhoneChange{
    
    //免提关：NO 免提开：YES
    BOOL returnValue = [[KCTFuncEngine getInstance] isSpeakerphoneOn];
    
    if (returnValue){
//        [[KCTFuncEngine getInstance] setSpeakerphone:YES];
        [[KCTFuncEngine getInstance] setSpeakerphone:NO];
        self.handFreeButton.selected = NO;
        
    }
    
}

/**
 @author WLS, 15-12-11 17:12:23
 
 被踢线
 */
-(void)KickOffNoti
{
    
    [self hangupCall];
    
    
}


- (float)getCurrentCPU{
    kern_return_t			kr = { 0 };
    task_info_data_t		tinfo = { 0 };
    mach_msg_type_number_t	task_info_count = TASK_INFO_MAX;
    
    kr = task_info( mach_task_self(), TASK_BASIC_INFO, (task_info_t)tinfo, &task_info_count );
    if ( KERN_SUCCESS != kr )
        return 0.0f;
    
    task_basic_info_t		basic_info = { 0 };
    thread_array_t			thread_list = { 0 };
    mach_msg_type_number_t	thread_count = { 0 };
    
    thread_info_data_t		thinfo = { 0 };
    thread_basic_info_t		basic_info_th = { 0 };
    
    basic_info = (task_basic_info_t)tinfo;
    
    // get threads in the task
    kr = task_threads( mach_task_self(), &thread_list, &thread_count );
    if ( KERN_SUCCESS != kr )
        return 0.0f;
    
    long	tot_sec = 0;
    long	tot_usec = 0;
    float	tot_cpu = 0;
    
    for ( int i = 0; i < thread_count; i++ )
    {
        mach_msg_type_number_t thread_info_count = THREAD_INFO_MAX;
        
        kr = thread_info( thread_list[i], THREAD_BASIC_INFO, (thread_info_t)thinfo, &thread_info_count );
        if ( KERN_SUCCESS != kr )
            return 0.0f;
        
        basic_info_th = (thread_basic_info_t)thinfo;
        if ( 0 == (basic_info_th->flags & TH_FLAGS_IDLE) )
        {
            tot_sec = tot_sec + basic_info_th->user_time.seconds + basic_info_th->system_time.seconds;
            tot_usec = tot_usec + basic_info_th->system_time.microseconds + basic_info_th->system_time.microseconds;
            tot_cpu = tot_cpu + basic_info_th->cpu_usage / (float)TH_USAGE_SCALE;
        }
    }
    
    kr = vm_deallocate( mach_task_self(), (vm_offset_t)thread_list, thread_count * sizeof(thread_t) );
    if ( KERN_SUCCESS != kr )
        return 0.0f;
    
    return tot_cpu;
}



@end
