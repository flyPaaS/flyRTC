//
//  KCTKeyboradViewController.m
//  KCT_IM_Demo
//
//  Created by KCT on 16/4/12.
//  Copyright © 2016年 KCT. All rights reserved.
//

#import "KCTKeyboradViewController.h"
#import "Contact.h"
#import "KCTVOIPViewEngine.h"

@interface KCTKeyboradViewController ()


@property (strong,nonatomic) UIImageView *backgroundView; // WLS，2016-04-12，背景视图

@property (strong,nonatomic) UIButton *callButton; // WLS，2016-04-12，发起呼叫按钮
@property (strong,nonatomic) UIButton *backButton; // WLS，2016-04-12，返回按钮
@property (strong,nonatomic) UILabel *numberLabel; // WLS，2016-04-12，号码label

@end

@implementation KCTKeyboradViewController

/**
 @author WLS, 16-04-12 10:12:22
 
 键盘按键视图
 */
- (void)makeKeyBoaradView{
    
    UIView *keyBoardBackView = [[UIView alloc] initWithFrame:CGRectMake(Adaptation(35), 0, GetViewWidth(self.view) - Adaptation(70) , GetViewHeight(self.view))];
    keyBoardBackView.backgroundColor = [UIColor clearColor];
    [self.backgroundView addSubview:keyBoardBackView];
    
    
    self.numberLabel = [[UILabel alloc] initWithFrame:CGRectMake(0, Adaptation(50), GetViewWidth(keyBoardBackView), Adaptation(30))];
    self.numberLabel.backgroundColor = [UIColor clearColor];
    self.numberLabel.textColor = [UIColor whiteColor];
    self.numberLabel.font = [UIFont systemFontOfSize:GetTextFont(22)];
    self.numberLabel.text = @"";
    self.numberLabel.textAlignment = NSTextAlignmentCenter;
    [keyBoardBackView addSubview:self.numberLabel];
    
    
    CGFloat rowSpaceWidth = (GetViewWidth(keyBoardBackView)  - Adaptation(72)*3)/2.0;
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
        [keyBoardBackView addSubview:button];
        
    }
    
    
    /**
     @author WLS, 15-12-11 15:12:22
     
     键盘弹出来的挂断按钮
     */
    self.callButton = [UIButton buttonWithType:UIButtonTypeCustom];
    self.callButton.frame = CGRectMake(CenterPoint(GetViewWidth(keyBoardBackView), Adaptation(72)), (Adaptation(72)+Adaptation(5))*4 + Adaptation(120)+ Adaptation(20),Adaptation(72), Adaptation(72));
    if (CurrentHeight == 480) {
        self.callButton.frame = CGRectMake(CenterPoint(GetViewWidth(keyBoardBackView), Adaptation(72)), (Adaptation(72)+Adaptation(5))*4 + Adaptation(70)+ Adaptation(20), Adaptation(72), Adaptation(72));
    }
    [self.callButton setBackgroundImage:[UIImage imageNamed:@"接听"] forState:UIControlStateNormal];
    [self.callButton setBackgroundImage:[UIImage imageNamed:@"接听-down"] forState:UIControlStateDisabled];
    [self.callButton addTarget:self action:@selector(callButtonClick) forControlEvents:UIControlEventTouchUpInside];
    [keyBoardBackView addSubview:self.callButton];
    
    
    
    /**
     @author WLS, 15-12-11 16:12:12
     
     隐藏键盘按钮
     */
    self.backButton = [UIButton buttonWithType:UIButtonTypeCustom];
    self.backButton.frame = CGRectMake(GetViewWidth(self.callButton)+GetViewX(self.callButton), GetViewY(self.callButton), GetViewWidth(keyBoardBackView) - GetViewX(self.callButton)-GetViewWidth(self.callButton), GetViewHeight(self.callButton));
    [self.backButton setTitle:@"返回" forState:UIControlStateNormal];
    self.backButton.titleLabel.font = [UIFont systemFontOfSize:GetTextFont(24)];
    [self.backButton setTitleColor:[UIColor whiteColor] forState:UIControlStateNormal];
    [self.backButton setTitleColor:GetRGBColor(999999) forState:UIControlStateHighlighted];
    self.backButton.backgroundColor = [UIColor clearColor];
    [self.backButton addTarget:self action:@selector(hideKeyboardView) forControlEvents:UIControlEventTouchUpInside];
    [keyBoardBackView addSubview:self.backButton];
    
    
}




- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.
    
    self.backgroundView = [[UIImageView alloc] initWithFrame:self.view.bounds];
    self.backgroundView.backgroundColor = RGBACOLOR(22, 32, 39, 1);
    self.backgroundView.userInteractionEnabled = YES;
    [self.view addSubview:self.backgroundView];
    
    [self makeKeyBoaradView];
    
}


#pragma mark - 按钮点击事件

- (void)callButtonClick{
    
    
    if (self.numberLabel.text == nil  || [self.numberLabel.text isEqualToString:@""]) {
        
        UIAlertView * alertView = [[UIAlertView alloc] initWithTitle:@"提示" message:@"被叫号码不能为空" delegate:nil cancelButtonTitle:@"确定" otherButtonTitles:nil, nil];
        [alertView show];
        return;
    }
    
    [[KCTVOIPViewEngine getInstance] makingCallViewCallNumber:self.numberLabel.text callType:1 callName:self.numberLabel.text];
    
}

- (void)hideKeyboardView{
    
    [[KCTVOIPViewEngine getInstance] releaseKeyboardViewAnimation:YES];
    
    
}

- (void)sendDTMFNumber:(UIButton *)button{
    NSString *numberString = [NSString stringWithFormat:@"%ld",button.tag];
    
    if (button.tag == 10) {
        numberString = @"*";
    }else if (button.tag == 11){
        numberString = @"0";
    }else if (button.tag == 12){
        numberString = @"#";
    }

    
    self.numberLabel.text = [NSString stringWithFormat:@"%@%@",self.numberLabel.text,numberString];
}


- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

/*
#pragma mark - Navigation

// In a storyboard-based application, you will often want to do a little preparation before navigation
- (void)prepareForSegue:(UIStoryboardSegue *)segue sender:(id)sender {
    // Get the new view controller using [segue destinationViewController].
    // Pass the selected object to the new view controller.
}
*/

@end
