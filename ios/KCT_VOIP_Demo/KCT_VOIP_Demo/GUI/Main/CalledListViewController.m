//
//  CalledListViewController.m
//  KCT_VOIP_Demo
//
//  Created by KCMac on 2017/1/16.
//  Copyright © 2017年 KCMac. All rights reserved.
//

#import "CalledListViewController.h"
#import <AVFoundation/AVCaptureDevice.h>
#import<AVFoundation/AVMediaFormat.h>

@interface CalledListViewController ()<CodecViewControllerDelegate>
{
    NSMutableArray *_sourceArrays;
    NSInteger _videoIndex;
    NSInteger _audioIndex;
}

@property(nonatomic,strong)NSIndexPath *currentIndexPath;
@property(nonatomic,strong)NSIndexPath *lastIndexPath;
@property(nonatomic,strong)IBOutlet UIView *line;
@property(nonatomic,strong)IBOutlet UITableView *tableView;

@end


#pragma mark -------------UIViewController-------------
@implementation CalledListViewController

- (void)viewWillAppear:(BOOL)animated
{
    [super viewWillAppear:animated];
    self.navigationController.navigationBar.hidden = NO;
    [LoginManager sharedLoginManager].currentPageIndex = appPageIndexCalled;
}

- (void)viewDidLoad {
    [super viewDidLoad];
    
    self.line.backgroundColor = [UIColor colorWithRed:27/255.0 green:165/255.0 blue:38/255.0 alpha:1.0];
    [_tableView registerNib:[UINib nibWithNibName:@"CallListTableViewCell" bundle:nil] forCellReuseIdentifier:@"callCell"];
    _sourceArrays = [NSMutableArray arrayWithArray:self.dataArray];
    self.title = @"视频";
    
    //left button
    UIButton *leftButton = [UIButton buttonWithType:UIButtonTypeCustom];
    leftButton.frame = CGRectMake(0, 0, 50, 23);
    UIImage *iamge = [UIImage imageNamed:@"common_list_back.png"];
    [leftButton setImage:iamge forState:UIControlStateNormal];
    [leftButton setImage:iamge forState:UIControlStateHighlighted];
    [leftButton addTarget:self action:@selector(exitapp:) forControlEvents:UIControlEventTouchUpInside];
    UIBarButtonItem *item = [[UIBarButtonItem alloc] initWithCustomView:leftButton];
    self.navigationItem.leftBarButtonItem = item;
    
    UIColor *color = RGB(17, 121, 255);
    UIButton *rightButton = [UIButton buttonWithType:UIButtonTypeCustom];
    rightButton.frame = CGRectMake(0, 0, 80, 30);
    [rightButton setTitleColor:color forState:UIControlStateNormal];
    [rightButton setTitleColor:[UIColor blueColor] forState:UIControlStateHighlighted];
    [rightButton setTitle:@"Codec" forState:UIControlStateNormal];
    [rightButton addTarget:self action:@selector(codecSetting:) forControlEvents:(UIControlEventTouchUpInside)];
    UIBarButtonItem *rItem = [[UIBarButtonItem alloc] initWithCustomView:rightButton];
    self.navigationItem.rightBarButtonItem = rItem;
    
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(loginFail:) name:@"loginFail" object:nil];
    // Do any additional setup after loading the view from its nib.
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}


- (void)dealloc {
    
    [[NSNotificationCenter defaultCenter] removeObserver:self];
}
#pragma mark ---------private function------------

- (void)loginFail:(NSNotification *)notification {
    NSNumber *result = notification.object;
    if ([result intValue] == 12) {
        [mytoast showWithText:@"Ssid超时"];
    } else if ([result intValue] == 13) {
        [mytoast showWithText:@"当前其他人正在用此账号登录，待会重试"];
    }
    
    [self loginout];
}


- (void)exitapp:(id)sender
{
    [mytoast showWithText:@"账号已退出"];
    [self loginout];
}

- (void)loginout {
    [self.navigationController popViewControllerAnimated:YES];
    [[KCTTcpClient sharedTcpClientManager] login_uninitWithFlag:YES];
}

- (void)cellButtonClick:(id)sender
{
    if (![self cameraAuthorization]) {
        return;
    }
    
    if (![self microphoneAuthorization]) {
        return;
    }
    
    UIButton *button = (UIButton *)sender;
    NSIndexPath *indexPath = [NSIndexPath indexPathForRow:button.tag inSection:0];
    self.currentIndexPath = indexPath;
    CallListTableViewCell *currentCell = [_tableView cellForRowAtIndexPath:indexPath];
    CallListTableViewCell *lastCell = [_tableView cellForRowAtIndexPath:self.lastIndexPath];
    
    
    if (self.lastIndexPath == nil) {
        UIImage *btnImg = [UIImage imageNamed:@"checkBtn.png"];
        [currentCell.buttonT setImage:btnImg forState:UIControlStateNormal];
    } else if(indexPath != self.lastIndexPath){
        UIImage *btnImg = [UIImage imageNamed:@"unCheckBtn.png"];
        [lastCell.buttonT setImage:btnImg forState:UIControlStateNormal];
        UIImage *btnImg2 = [UIImage imageNamed:@"checkBtn.png"];
        [currentCell.buttonT setImage:btnImg2 forState:UIControlStateNormal];
    } else {
        
    }
    self.lastIndexPath = indexPath;
    
    NSDictionary *uidDict = [self.dataArray objectAtIndex:indexPath.row];
    NSString *client_number = [uidDict objectForKey:@"client_number"];
    [self makeCallWithNumber:client_number];
}

- (BOOL)cameraAuthorization {
    AVAuthorizationStatus authStatus = [AVCaptureDevice authorizationStatusForMediaType:AVMediaTypeVideo];
    if (authStatus == AVAuthorizationStatusRestricted || authStatus ==AVAuthorizationStatusDenied) {
        UIAlertView * alart = [[UIAlertView alloc]initWithTitle:@"温馨提示" message:@"请您设置允许APP访问您的相机->设置->隐私->相机" delegate:self cancelButtonTitle:@"取消" otherButtonTitles:@"确定", nil];
        [alart show];
        return NO;
    }
    return YES;
}

- (BOOL)microphoneAuthorization {
    AVAuthorizationStatus authStatus = [AVCaptureDevice authorizationStatusForMediaType:AVMediaTypeAudio];
    if (authStatus == AVAuthorizationStatusRestricted || authStatus ==AVAuthorizationStatusDenied) {
        UIAlertView * alart = [[UIAlertView alloc]initWithTitle:@"温馨提示" message:@"请您设置允许APP访问您的麦克风->设置->隐私->麦克风" delegate:self cancelButtonTitle:@"取消" otherButtonTitles:@"确定", nil];
        [alart show];
        return NO;
    }
    return YES;
}


//根据被点击按钮的索引处理点击事件
-(void)alertView:(UIAlertView *)alertView clickedButtonAtIndex:(NSInteger)buttonIndex
{
    if (buttonIndex == 1) {
        [[UIApplication sharedApplication] openURL:[NSURL URLWithString:UIApplicationOpenSettingsURLString]];
    }
}


- (void)codecSetting:(id)sender
{
    CodecViewController *controller = [[CodecViewController alloc] init];
    controller.delegate = self;
    controller.videoIndex = _videoIndex;
    controller.audioIndex = _audioIndex;
    
    [self.navigationController pushViewController:controller animated:YES];
}

- (void)makeCallWithNumber:(NSString *)clientNumber {
    [[KCTVOIPViewEngine getInstance] makingCallViewCallNumber:clientNumber callType:KCTCallType_VideoPhone callName:@""];
}

#pragma mark -----------UITableView----------

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {
    return self.dataArray.count;
}

- (CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath
{
    return 60;
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath {
    static NSString * indentfier = @"callCell";
    CallListTableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:indentfier];
    
    if (!cell) {
        cell = [[CallListTableViewCell alloc] initWithStyle:UITableViewCellStyleValue1 reuseIdentifier:indentfier];
    }
    NSDictionary *uidDict = [self.dataArray objectAtIndex:indexPath.row];
    
    NSString *client_number = [uidDict objectForKey:@"client_number"];
    NSString *mobile = [uidDict objectForKey:@"mobile"];
    int mod = indexPath.row%6;
    NSString *imageName = [NSString stringWithFormat:@"head%d_small.png",mod+1];
    cell.leftImageView.image = [UIImage imageNamed:imageName];
    cell.label1.text = client_number;
    cell.label2.text = mobile;
    UIImage *btnImg = [UIImage imageNamed:@"unCheckBtn.png"];
    [cell.buttonT setImage:btnImg forState:UIControlStateNormal];
    [cell.buttonT addTarget:self action:@selector(cellButtonClick:) forControlEvents:UIControlEventTouchUpInside];
    cell.buttonT.tag = indexPath.row;
    return cell;
}


- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath {
    
    CallListTableViewCell *currentCell = [_tableView cellForRowAtIndexPath:indexPath];
    UIButton *btn = currentCell.buttonT;
    [self cellButtonClick:btn];
    currentCell.selectionStyle = UITableViewCellSelectionStyleNone;
}


#pragma mark-----codec delete----
- (void)selectVideoCodec:(NSInteger)videoCodecIndex audio:(NSInteger)audioCodecIndex
{
    _videoIndex = videoCodecIndex;
    _audioIndex = audioCodecIndex;
    [[KCTFuncEngine getInstance] setVideoCodecWithIndex:videoCodecIndex audioCodecIndex:audioCodecIndex];
}



#pragma mark ----------Orientation-----------

- (BOOL)shouldAutorotate
{
    return NO;
}

- (UIInterfaceOrientationMask)supportedInterfaceOrientations
{
    return UIInterfaceOrientationMaskPortrait;//只支持这一个方向(正常的方向)
}


@end
