//
//  UCSVoipCallController.m
//  UCSVoipDemo
//
//  Created by wenqinglin on 14-5-22.
//  Copyright (c) 2014年 UCS. All rights reserved.
//


//NSString * const kCallBg01pngVoip           = @"call_bg01.png";
//NSString * const kCallHangUpButtonpng       = @"call_hang_up_button.png";
//NSString * const kCallHangUpButtonOnpng     = @"call_hang_up_button_on.png";

#import "UCSVoipCallController.h"
#import "UCSCommonClass.h"
#import "UCConst.h"
#import "Contact.h"
#import "UCSVOIPViewEngine.h"

@interface UCSVoipCallController ()



@property (strong,nonatomic) UIImageView *backgroundView; // WLS，2015-12-09，背景视图




@property (strong,nonatomic) UIView *callBackView; // WLS，2015-12-09，通话中背景视图
@property (strong,nonatomic) UIImageView *iconView; // WLS，2015-12-09，头像视图
@property (strong,nonatomic) UIButton *hangupButton; // WLS，2015-12-09，挂断按钮
@property (strong,nonatomic) UILabel *voipNumberLabel; // WLS，2015-12-09，对方号码标题
@property (strong,nonatomic) UIButton *callTimeLabel; // WLS，2015-12-09，通话时间标题
@property (strong,nonatomic) UIView *callFunctionView; // WLS，2015-12-09，通话中功能按钮背景视图
@property (strong,nonatomic) UIButton *handFreeButton; // WLS，2015-12-09，免提按钮
@property (strong,nonatomic) UIButton *keyBoradButton; // WLS，2015-12-09，显示键盘按钮
@property (strong,nonatomic) UIButton *muteButton; // WLS，2015-12-09，静音按钮
@property (strong,nonatomic) UIButton *answerButton; // WLS，2015-12-09，接听按钮(被叫使用)


@property (strong,nonatomic) UIView *keyBoardBackView; // WLS，2015-12-09，弹出键盘时候的背景视图
@property (strong,nonatomic) UIButton *keyBoardHangupButton; // WLS，2015-12-09，弹出键盘时候的挂断按钮
@property (strong,nonatomic) UIButton *hideKeyBoardButton; // WLS，2015-12-09，隐藏键盘按钮

@property (assign,nonatomic) BOOL isReleaseCall; // WLS，2015-12-19，是否已经移除过界面


@end

@implementation UCSVoipCallController

@synthesize callID;
@synthesize callerNo;
@synthesize voipNo;



#pragma mark - 添加移除通知
//添加通知
-(void)addNotification{
    //注册耳机监听 add by WLS 20151106
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(headPhoneChange) name:UCSNotiHeadPhone object:nil];
    /**
     @author WLS, 15-12-15 09:12:20
     
     踢线通知
     */
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(KickOffNoti) name:TCPKickOffNotification object:nil];
}
//移除通知
-(void)removeNotification{
    [[NSNotificationCenter defaultCenter] removeObserver:self name:UCSNotiHeadPhone object:nil];
    [[NSNotificationCenter defaultCenter] removeObserver:self name:TCPKickOffNotification object:nil];
    
}


#pragma mark - 界面规划

/**
 @author WLS, 15-12-09 17:12:59
 
 通话界面
 */
- (void)makeCallView{
    
    self.callBackView = [[UIView alloc] initWithFrame:self.view.bounds];
    self.callBackView.backgroundColor = [UIColor clearColor];
    [self.backgroundView addSubview:self.callBackView];
    

    /**
      @author WLS, 15-12-11 11:21:59
     
      头像
     */
    self.iconView = [[UIImageView alloc] initWithFrame:CGRectMake(CenterPoint(GetViewWidth(self.callBackView), Adaptation(110)), Adaptation(90), Adaptation(110), Adaptation(110))];
    self.iconView.layer.cornerRadius = GetViewHeight(self.iconView)/2.0;
    self.iconView.layer.masksToBounds = YES;
    self.iconView.backgroundColor = [UIColor blackColor];
    self.iconView.image = [UIImage imageNamed:@"默认头像"];
    [self.callBackView addSubview:self.iconView];
    if (CurrentHeight == 480) {
        CGRect frame = self.iconView.frame;
        frame.origin.y = Adaptation(60);
        self.iconView.frame = frame;
    }
    
    /**
     @author WLS, 15-12-11 11:21:59
     
     来电号码或者被叫号码
     */
    self.voipNumberLabel = [[UILabel alloc] initWithFrame:CGRectMake(CenterPoint(GetViewWidth(self.callBackView), Adaptation(170)), Adaptation(275), Adaptation(170), Adaptation(20))];
    self.voipNumberLabel.font = [UIFont systemFontOfSize:GetTextFont(21)];
    self.voipNumberLabel.textAlignment = NSTextAlignmentCenter;
    self.voipNumberLabel.textColor = [UIColor whiteColor];
    self.voipNumberLabel.text = self.callerName;
    [self.callBackView addSubview:self.voipNumberLabel];
    if (CurrentHeight == 480) {
        CGRect frame = self.voipNumberLabel.frame;
        frame.origin.y = Adaptation(225);
        self.voipNumberLabel.frame = frame;
    }
    
    
    /**
     @author WLS, 15-12-11 11:21:59
     
     对方通话状态或者时间
     */
    self.callTimeLabel = [UIButton buttonWithType:UIButtonTypeCustom];
    self.callTimeLabel.enabled = NO;
    self.callTimeLabel.frame = CGRectMake(GetViewX(self.voipNumberLabel), GetViewHeight(self.voipNumberLabel)+GetViewY(self.voipNumberLabel)+ Adaptation(5), GetViewWidth(self.voipNumberLabel), Adaptation(20));
    self.callTimeLabel.titleLabel.font = [UIFont systemFontOfSize:GetTextFont(13)];
    [self.callTimeLabel setTitleColor:[UIColor grayColor] forState:UIControlStateDisabled];
    [self.callTimeLabel setTitle:@"呼叫请求中" forState:UIControlStateDisabled];
    if (self.incomingCall) {
        [self.callTimeLabel setTitle:@"语音聊天" forState:UIControlStateDisabled];
        if ([[UCSTcpClient sharedTcpClientManager] getCurrentNetWorkStatus]==UCSReachableVia2G) {
            [self.callTimeLabel setTitle:@"语音聊天(当前网络状态差)" forState:UIControlStateDisabled];

        }
    }
    [self.callBackView addSubview:self.callTimeLabel];
   
    
    
    /**
     @author WLS, 15-12-11 11:21:59
     
     挂断按钮如果是来电，则还有接听按钮
     */
    self.hangupButton = [UIButton buttonWithType:UIButtonTypeCustom];
    [self.hangupButton addTarget:self action:@selector(hangupCall) forControlEvents:UIControlEventTouchUpInside];
    [self.hangupButton setImage:[UIImage imageNamed:@"挂断"] forState:UIControlStateNormal];
    [self.hangupButton setImage:[UIImage imageNamed:@"挂断-down"] forState:UIControlStateDisabled];
    [self.callBackView addSubview:self.hangupButton];
    if (self.incomingCall) {
        self.answerButton = [UIButton buttonWithType:UIButtonTypeCustom];
        [self.answerButton setImage:[UIImage imageNamed:@"接听"] forState:UIControlStateNormal];
        [self.answerButton setImage:[UIImage imageNamed:@"接听-down"] forState:UIControlStateDisabled];
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
     @author WLS, 15-12-11 12:12:12
     
     静音 扬声器 键盘按钮
     */
    [self makeCallFuntiongButton];
    
    
    
    [self changeCallNameAndTimeLabelFrame:NO];
}


/**
 @author WLS, 15-12-11 12:12:22
 
 静音 扬声器 键盘按钮
 */
- (void)makeCallFuntiongButton{
    
    self.callFunctionView = [[UIView alloc] initWithFrame:CGRectMake(Adaptation(30), Adaptation(395), GetViewWidth(self.callBackView) - Adaptation(60) , Adaptation(60))];
    self.callFunctionView.backgroundColor = [UIColor clearColor];
    [self.callBackView addSubview:self.callFunctionView];
    
    if (CurrentHeight == 480) {
        CGRect frame = self.callFunctionView.frame;
        frame.origin.y = Adaptation(305);
        self.callFunctionView.frame = frame;
    }
    
    
    
    CGFloat spaceWidth = (GetViewWidth(self.callFunctionView)  - GetViewHeight(self.callFunctionView)*3)/2.0;
    NSArray * imageArray = @[@"静音",@"拨号键盘",@"扬声器"];
    NSArray * selectImageArray = @[@"静音-down",@"拨号键盘-down",@"扬声器-down"];
    NSArray * titleArray = @[@"静音",@"拨号键盘",@"扬声器"];
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
            self.keyBoardBackView = button;
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
 @author WLS, 15-12-11 11:12:42
 
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
     @author WLS, 15-12-11 11:12:36
     
     如果接听按钮存在  则移除接听按钮
     */
    if (self.answerButton) {
        [self.answerButton removeFromSuperview];
        self.answerButton = nil;
    }
    
}


/**
 @author WLS, 15-12-17 15:12:45
 
 修改对方号码的标题和通话时间标题的位置
 
 @param isShowKeyBoard 是否处于显示键盘状态
 */
- (void)changeCallNameAndTimeLabelFrame:(BOOL)isShowKeyBoard{
    
    if (isShowKeyBoard) {
        /**
         @author WLS, 15-12-17 15:12:47
         
         此时显示键盘
         */
        
        [self.voipNumberLabel removeFromSuperview];
        [self.callTimeLabel removeFromSuperview];
        
        self.voipNumberLabel.frame = CGRectMake(0, Adaptation(50), GetViewWidth(self.keyBoardBackView), Adaptation(20));
        
        if (CurrentHeight == 480) {
            CGRect frame = self.voipNumberLabel.frame;
            frame.origin.y = Adaptation(20);
            self.voipNumberLabel.frame = frame;
        }
        
        [self.keyBoardBackView addSubview:self.voipNumberLabel];
        [self.keyBoardBackView addSubview:self.callTimeLabel];
        
    }else{
        [self.voipNumberLabel removeFromSuperview];
        [self.callTimeLabel removeFromSuperview];
        
        self.voipNumberLabel.frame = CGRectMake(0, Adaptation(215),GetViewWidth(self.callBackView), Adaptation(20));
        if (CurrentHeight == 480) {
            CGRect frame = self.voipNumberLabel.frame;
            frame.origin.y = Adaptation(180);
            self.voipNumberLabel.frame = frame;
        }
        
        [self.callBackView addSubview:self.voipNumberLabel];
        [self.callBackView addSubview:self.callTimeLabel];
    }
    
    self.callTimeLabel.frame = CGRectMake(0, GetViewHeight(self.voipNumberLabel)+ GetViewY(self.voipNumberLabel)+Adaptation(5), GetViewWidth(self.voipNumberLabel), GetViewHeight(self.voipNumberLabel));
    
}



/**
 @author WLS, 15-12-11 15:12:22
 
 键盘按键视图
 */
- (void)makeKeyBoaradView{
    
    self.keyBoardBackView = [[UIView alloc] initWithFrame:CGRectMake(Adaptation(35), 0, GetViewWidth(self.view) - Adaptation(70) , GetViewHeight(self.view))];
    self.keyBoardBackView.backgroundColor = [UIColor clearColor];
    [self.backgroundView addSubview:self.keyBoardBackView];
    
    CGFloat rowSpaceWidth = (GetViewWidth(self.keyBoardBackView)  - Adaptation(72)*3)/2.0;
    CGFloat colSpaceHeight = Adaptation(5);
    for (int i = 0; i< 12 ; i++) {
        NSInteger row = i%3;
        NSInteger col = i/3;
        
        UIButton * button = [UIButton buttonWithType:UIButtonTypeCustom];
        button.tag = i+1;
        [button setImage:[UIImage imageNamed:[NSString stringWithFormat:@"%d",button.tag]] forState:UIControlStateNormal];
        [button setImage:[UIImage imageNamed:[NSString stringWithFormat:@"%d-down",button.tag]] forState:UIControlStateHighlighted];
        button.frame = CGRectMake(row * (Adaptation(72)+rowSpaceWidth), col * (Adaptation(72) + colSpaceHeight)+ Adaptation(120) , Adaptation(72), Adaptation(72));
        if (CurrentHeight == 480) {
            button.frame = CGRectMake(row * (Adaptation(72)+rowSpaceWidth), col * (Adaptation(72) + colSpaceHeight)+ Adaptation(70) , Adaptation(72), Adaptation(72));
        }
        [button addTarget:self action:@selector(sendDTMFNumber:) forControlEvents:UIControlEventTouchUpInside];
        [self.keyBoardBackView addSubview:button];
        
    }
    
    
    /**
     @author WLS, 15-12-11 15:12:22
     
     键盘弹出来的挂断按钮
     */
    self.keyBoardHangupButton = [UIButton buttonWithType:UIButtonTypeCustom];
    self.keyBoardHangupButton.frame = CGRectMake(0, (Adaptation(72)+Adaptation(5))*4 + Adaptation(120)+ Adaptation(20), Adaptation(170), Adaptation(55));
    if (CurrentHeight == 480) {
        self.keyBoardHangupButton.frame = CGRectMake(0, (Adaptation(72)+Adaptation(5))*4 + Adaptation(70)+ Adaptation(20), Adaptation(170), Adaptation(55));
    }
    [self.keyBoardHangupButton setImage:[UIImage imageNamed:@"挂断大"] forState:UIControlStateNormal];
    [self.keyBoardHangupButton setImage:[UIImage imageNamed:@"挂断大-down"] forState:UIControlStateDisabled];
    [self.keyBoardHangupButton addTarget:self action:@selector(hangupCall) forControlEvents:UIControlEventTouchUpInside];
    [self.keyBoardBackView addSubview:self.keyBoardHangupButton];
    
    
    
    /**
     @author WLS, 15-12-11 16:12:12
     
     隐藏键盘按钮
     */
    self.hideKeyBoardButton = [UIButton buttonWithType:UIButtonTypeCustom];
    self.hideKeyBoardButton.frame = CGRectMake(GetViewWidth(self.keyBoardHangupButton), GetViewY(self.keyBoardHangupButton), GetViewWidth(self.keyBoardBackView) - GetViewWidth(self.keyBoardHangupButton), GetViewHeight(self.keyBoardHangupButton));
    [self.hideKeyBoardButton setTitle:@"隐藏" forState:UIControlStateNormal];
    self.hideKeyBoardButton.titleLabel.font = [UIFont systemFontOfSize:GetTextFont(22)];
    [self.hideKeyBoardButton setTitleColor:[UIColor whiteColor] forState:UIControlStateNormal];
    [self.hideKeyBoardButton setTitleColor:GetRGBColor(999999) forState:UIControlStateHighlighted];
    self.hideKeyBoardButton.backgroundColor = [UIColor clearColor];
    [self.hideKeyBoardButton addTarget:self action:@selector(hideKeyboardView) forControlEvents:UIControlEventTouchUpInside];
    [self.keyBoardBackView addSubview:self.hideKeyBoardButton];
    
    
    self.keyBoardBackView.hidden = YES;
}

- (void)disableAllButton{
    
    
    self.muteButton.enabled = NO;
    
    self.handFreeButton.enabled = NO;
    
    self.hangupButton.enabled = NO;
    
    self.answerButton.enabled = NO;
    
    self.keyBoradButton.enabled = NO;

    self.keyBoardHangupButton.enabled = NO;
}




#pragma mark - 生命周期

- (void)dealloc{
    
    
    [self.iconView removeFromSuperview];
    self.iconView = nil;
    
    [self.hangupButton removeFromSuperview];
    self.hangupButton = nil;
    
    [self.voipNumberLabel removeFromSuperview];
    self.voipNumberLabel = nil;
    
    [self.callTimeLabel removeFromSuperview];
    self.callTimeLabel = nil;
    
    [self.callFunctionView removeFromSuperview];
    self.callFunctionView = nil;
    
    [self.handFreeButton removeFromSuperview];
    self.handFreeButton = nil;
    
    [self.keyBoradButton removeFromSuperview];
    self.keyBoradButton = nil;
    
    [self.muteButton removeFromSuperview];
    self.muteButton = nil;
    
    [self.answerButton removeFromSuperview];
    self.answerButton = nil;
    
    [self.callBackView removeFromSuperview];
    self.callBackView = nil;
    
    [self.keyBoardHangupButton removeFromSuperview];
    self.keyBoardHangupButton = nil;
    
    [self.hideKeyBoardButton removeFromSuperview];
    self.hideKeyBoardButton = nil;
    
    [self.keyBoardBackView removeFromSuperview];
    self.keyBoardBackView = nil;
    
    [self.callBackView removeFromSuperview];
    self.callBackView = nil;
    
    
    self.callID = nil;
    self.callerNo = nil;
    self.iconString = nil;
    self.voipNo = nil;
    self.currentTime = nil;
    
    
}

- (UCSVoipCallController *)initWithCallerNo:(NSString *)phoneNo andVoipNo:(NSString *)voipNop andCallType:(NSInteger)type;
{
    if (self = [super init])
    {
        self.callerNo = phoneNo;
        self.voipNo = voipNop;
        self.isCallNow = NO;
        self.callDuration = @"";
        self.callerName = @"";
        hhInt = 0;
        mmInt = 0;
        ssInt = 0;
        voipCallType = type;
        self.isReleaseCall = NO;
        self.beReject = NO;
        [[UCSFuncEngine getInstance] setSpeakerphone:NO];
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
    
    
    // 设置通话过程中自动感应，黑屏，避免耳朵按到其他按键
    [UIDevice currentDevice].proximityMonitoringEnabled = YES;
    // 设置不自动进入锁屏待机状态
    [[UIApplication sharedApplication] setIdleTimerDisabled:YES];
    // 需要在通话中成为firstResponder，否则切换回前台后，听不到声音
    [self becomeFirstResponder];

    
    
    [self makeCallView];
    [self makeKeyBoaradView];
    
    [self addNotification];

}

- (void)viewWillAppear:(BOOL)animated
{
    
    [[UIApplication sharedApplication] beginReceivingRemoteControlEvents];
    
    [super viewWillAppear:animated];

    
    
}

- (void)viewWillDisappear:(BOOL)animated
{
    
    [[UIApplication sharedApplication] endReceivingRemoteControlEvents];
    
    [super viewWillDisappear:animated];
    
    [self removeNotification];

}

- (void)viewDidDisappear:(BOOL)animated
{
    //globalisVoipView = NO;
    [super viewDidDisappear:animated];
    
    // 设置通话过程中自动感应，黑屏，避免耳朵按到其他按键
    [UIDevice currentDevice].proximityMonitoringEnabled = NO;
    // 设置不自动进入锁屏待机状态
    [[UIApplication sharedApplication] setIdleTimerDisabled:NO];
    
    // 需要在通话中成为firstResponder，否则切换回前台后，听不到声音
    [self resignFirstResponder];
    
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

// 需要在通话中成为firstResponder，否则切换回前台后，听不到声音
- (BOOL)canBecomeFirstResponder {
    return YES;
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
             
             显示键盘
             */
            [self showKeyboardView];
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
 @author WLS, 15-12-11 16:12:05
 
 发送dtmf
 */
- (void)sendDTMFNumber:(UIButton *)button
{
    NSString *numberString = [NSString stringWithFormat:@"%ld",button.tag];

    if (button.tag == 10) {
        numberString = @"*";
    }else if (button.tag == 11){
        numberString = @"0";
    }else if (button.tag == 12){
        numberString = @"#";
    }
    
    char *tempdtf = [numberString cStringUsingEncoding:NSASCIIStringEncoding];
    [[UCSFuncEngine getInstance] sendDTMF:*tempdtf];
}


/**
 @author WLS, 15-12-11 16:12:45
 
 显示键盘视图
 */
- (void)showKeyboardView
{
    
    self.keyBoardBackView.hidden = NO;
    self.callBackView.hidden = YES;
    
    [self changeCallNameAndTimeLabelFrame:YES];
}

/**
 @author WLS, 15-12-11 16:12:45
 
 隐藏键盘视图
 */
- (void)hideKeyboardView
{
    
    self.keyBoardBackView.hidden = YES;
    self.callBackView.hidden = NO;
    
    [self changeCallNameAndTimeLabelFrame:NO];
}


/**
 @author WLS, 15-12-11 16:12:46
 
 挂断电话
 */
- (void)hangupCall
{
    
    
    [self setCallDuration];
    
    [self disableAllButton];
    
    self.hangupMySelf = YES;
//    [self.callTimeLabel setTitle:@"正在挂机" forState:UIControlStateDisabled];
    [[UCSFuncEngine getInstance] hangUp:self.callID];
    
    [[UCSVOIPViewEngine getInstance] WriteToSandBox:[NSString stringWithFormat:@"挂断通话：%@",[UCSFuncEngine getInstance]]];

    
    [NSTimer scheduledTimerWithTimeInterval:1.0f target:self selector:@selector(backFront) userInfo:nil repeats:NO];
    

}


/**
 @author WLS, 15-12-11 16:12:43
 
 接听电话
 */
- (void)answerCall{
    
    [[UCSFuncEngine getInstance] answer:self.callID];
}



#pragma mark - 功能
//免提事件
- (void)handfree
{
    
//    if ([[UCSFuncEngine getInstance] isHeadPhone]) {
//        //处于耳机模式 不允许设置免提 add by WLS 20151106
//        return;
//    }
    
    if ([UCSVOIPViewEngine  isHeadphone]) {
        //处于耳机模式 不允许设置免提 add by WLS 20151106
        return;
    }
    
    if (self.isCallNow == NO) {
        return;
    }
    
    //免提关：NO 免提开：YES
    BOOL returnValue = [[UCSFuncEngine getInstance] isSpeakerphoneOn];
    
    if (returnValue==NO)
    {
        [[UCSFuncEngine getInstance] setSpeakerphone:YES];
        self.handFreeButton.selected = YES;
    }else{
        [[UCSFuncEngine getInstance] setSpeakerphone:NO];
        self.handFreeButton.selected = NO;

    }
}
//静音事件
- (void)mute
{
    if (self.isCallNow ==NO) {
        return;
    }
    
    BOOL muteFlag = [[UCSFuncEngine getInstance] isMicMute];
    if (muteFlag == NO) {
        self.muteButton.selected = YES;;
        [[UCSFuncEngine getInstance] setMicMute:YES];//设置为静音
    }
    else
    {
        self.muteButton.selected = NO;;
        [[UCSFuncEngine getInstance] setMicMute:NO];//设置为非静音
    }
    
}


/**
 @author WLS, 15-12-11 17:12:10
 
 释放界面
 */
- (void)backFront
{
    
    if (self.isReleaseCall) {
        return;
    }
    self.isReleaseCall = YES;
    
    
    [[UCSVOIPViewEngine getInstance]releaseViewControler:self];
    
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

#pragma mark - 通话状态回调

/**
 @author WLS, 15-12-19 15:12:49
 
 网络状态回调
 
 @param currentNetwork 当前的网络状态
 */
- (void)networkStatusChange:(NSInteger)currentNetwork networdDes:(NSString *)networdDes{
    NSString * imageStr = nil;
    switch (currentNetwork) {
        case UCSNetwork_Bad:
        {
            //差
            imageStr = @"通话信号-无";
        }
            break;
        case UCSNetwork_General:
        {
            //一般
            imageStr = @"通话信号1";
            
        }
            break;
        case UCSNetwork_Nice:
        {
            //良好
            imageStr = @"通话信号2";
            
        }
            break;
        case UCSNetwork_Well:
        {
            //优秀
            imageStr = @"通话信号3";
            
        }
            break;
        default:
            break;
    }
    [self.callTimeLabel setImage:[UIImage imageNamed:imageStr] forState:UIControlStateDisabled];
    
    
}
-(void)responseVoipManagerStatus:(UCSCallStatus)event callID:(NSString*)callid data:(UCSReason *)data withVideoflag:(int)videoflag
{
  
    
    self.callTimeLabel.hidden = NO;

    switch (event)
    {
        case UCSCallStatus_Alerting:{
            [self.callTimeLabel setTitle:@"对方正在响铃" forState:UIControlStateDisabled];
            
        }
            break;
        case UCSCallStatus_Answered:
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
            [self changeHangupButtonFrame];
            
        }
            break;
            
        case UCSCallStatus_Released:
        {
            
            
            [self setCallDuration];
            
            
            [self disableAllButton];
            
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
        case UCSCallStatus_Failed:
        {
            [self disableAllButton];
            
            [self.callTimeLabel setImage:nil forState:UIControlStateDisabled];
            [self.callTimeLabel setTitle:data.msg forState:UIControlStateDisabled];
            if (data.reason==402050) {
                [self.callTimeLabel setTitle:@"对方无应答" forState:UIControlStateDisabled];
            }
            [NSTimer scheduledTimerWithTimeInterval:1.0f target:self selector:@selector(backFront) userInfo:nil repeats:NO];
            
        }
            break;
        case UCSCallStatus_Transfered:
        {
            [self disableAllButton];
            
            [NSTimer scheduledTimerWithTimeInterval:1.0f target:self selector:@selector(backFront) userInfo:nil repeats:NO];
        }
            break;
        case UCSCallStatus_Pasused:
        {
            [self.callTimeLabel setImage:nil forState:UIControlStateDisabled];
            [self.callTimeLabel setTitle:@"呼叫保持" forState:UIControlStateDisabled];
        }
            break;
        default:
            break;
    }
    
    
    
}



#pragma mark - 通知

//耳机监听 add by Wls 20151106
- (void)headPhoneChange{
    
    //免提关：NO 免提开：YES
    BOOL returnValue = [[UCSFuncEngine getInstance] isSpeakerphoneOn];
    
    if (returnValue){
//        [[UCSFuncEngine getInstance] setSpeakerphone:YES];
        [[UCSFuncEngine getInstance] setSpeakerphone:NO];
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

@end
