//
//  ClientListViewController.m
//  KCT_VOIP_Demo
//
//  Created by KCMac on 2017/1/7.
//  Copyright © 2017年 KCMac. All rights reserved.
//

#import "ClientListViewController.h"
#import "Contact.h"
#import "KCTTcpClient.h"
#import "KCConst.h"
#import "CallListTableViewCell.h"
#import "MBProgressHUD+Add.h"
#import "mytoast.h"


@interface ClientListViewController ()
{
    NSMutableArray *sourceArrays;
}

@property(nonatomic,weak)IBOutlet UILabel *label;
@property(nonatomic,strong)NSIndexPath *currentIndexPath;
@property(nonatomic,strong)NSIndexPath *lastIndexPath;
@property(nonatomic,strong)IBOutlet UIView *line;
@property(nonatomic,strong)IBOutlet UITableView *tableView;

@end

#pragma mark ----------UIViewController-----

@implementation ClientListViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    
    self.line.backgroundColor = [UIColor colorWithRed:27/255.0 green:165/255.0 blue:38/255.0 alpha:1.0];
    self.label.text = @"请选择体验账号";
    [_tableView registerNib:[UINib nibWithNibName:@"CallListTableViewCell" bundle:nil] forCellReuseIdentifier:@"callCell"];
    sourceArrays = [NSMutableArray arrayWithArray:self.dataArray];
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}


#pragma mark ------------UIButton Action------
- (IBAction)buttonClick:(id)sender {
    
    if (self.currentIndexPath == nil) {
        [mytoast showWithText:@"请选择其中一个体验账号"];
        return;
    }
    
    NSDictionary *uidDic = [self.dataArray objectAtIndex:self.currentIndexPath.row];
    NSUserDefaults *userDefault = [NSUserDefaults standardUserDefaults];
    
    [userDefault setObject:uidDic forKey:kCallingUidKey];
    [userDefault synchronize];
    
    [self connectToCSService:uidDic];
}


#pragma mark ------------private function-----------

- (void)connectToCSService:(NSDictionary *)uidDic {
    NSString *uid = [uidDic objectForKey:@"client_number"];
    NSString *pwd = [uidDic objectForKey:@"client_pwd"];
    NSString *main_account = [self.accountInfoDic objectForKey:@"sid"];
    NSString *main_token = [self.accountInfoDic objectForKey:@"token"];;
    
    [MBProgressHUD showMessage:@"正在登入账号，请稍等..." toView:self.view];
    __weak __typeof(self)weakSelf = self;
    [[KCTTcpClient sharedTcpClientManager] connect:main_account accountToken:main_token clientNumber:uid clientPwd:pwd success:^(NSString *userId) {
        
        dispatch_async(dispatch_get_main_queue(), ^{
            [MBProgressHUD hideAllHUDsForView:weakSelf.view animated:YES];
            [mytoast showWithText:@"成功"];
            
            [sourceArrays removeObjectAtIndex:self.currentIndexPath.row];
            NSArray *arrays = [NSArray arrayWithArray:sourceArrays];
            self.loginBlock(uidDic,arrays);
            [weakSelf dismissViewControllerAnimated:YES completion:nil];
            [[NSNotificationCenter defaultCenter] postNotificationName:TCPConnectStateNotification object:KCTCPDidConnectNotification];
        });
        
    } failure:^(KCTError *error) {
        [MBProgressHUD hideAllHUDsForView:weakSelf.view animated:YES];
        [mytoast showWithText:@"失败"];
        [[NSNotificationCenter defaultCenter] postNotificationName:TCPConnectStateNotification object:KCTCPDidConnectNotification];
    }];
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
        //lastCell.accessoryType = UITableViewCellAccessoryNone;
        UIImage *btnImg2 = [UIImage imageNamed:@"checkBtn.png"];
        [currentCell.buttonT setImage:btnImg2 forState:UIControlStateNormal];
    } else {
        
    }
    self.lastIndexPath = indexPath;
}

-(void)unSelectCell:(id)sender{
    [self.tableView deselectRowAtIndexPath:[self.tableView indexPathForSelectedRow] animated:YES];
}

#pragma mark ----------UITableView-------------
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



#pragma mark -----------Orientation---------

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)toInterfaceOrientation
{
    return (toInterfaceOrientation == UIInterfaceOrientationPortrait);
}

- (BOOL)shouldAutorotate
{
    return NO;
}

- (UIInterfaceOrientationMask)supportedInterfaceOrientations
{
    return UIInterfaceOrientationMaskPortrait;//只支持这一个方向(正常的方向)
}


#pragma mark ------------end-------------

@end
