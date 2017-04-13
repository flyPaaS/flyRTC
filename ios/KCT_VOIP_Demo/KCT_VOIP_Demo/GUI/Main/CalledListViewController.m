//
//  CalledListViewController.m
//  KCT_VOIP_Demo
//
//  Created by KCMac on 2017/1/16.
//  Copyright © 2017年 KCMac. All rights reserved.
//

#import "CalledListViewController.h"



@interface CalledListViewController ()
{
    NSMutableArray *_sourceArrays;
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
    // Do any additional setup after loading the view from its nib.
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

#pragma mark ---------private function------------
- (void)exitapp:(id)sender
{
    [mytoast showWithText:@"账号已退出"];
    [self.navigationController popViewControllerAnimated:YES];
    [[KCTTcpClient sharedTcpClientManager] login_uninitWithFlag:YES];
}

- (void)cellButtonClick:(id)sender
{
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
