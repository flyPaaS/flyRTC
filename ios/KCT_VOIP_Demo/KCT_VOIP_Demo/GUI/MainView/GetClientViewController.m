//
//  GetClientViewController.m
//  KCT_VOIP_Demo
//
//  Created by KCMac on 2017/1/7.
//  Copyright © 2017年 KCMac. All rights reserved.
//

#import "GetClientViewController.h"
#import "CommonHeaders.h"
#import "ClientListViewController.h"
#import "MBProgressHUD+Add.h"
#import "CalledListViewController.h"
#import "AppHelpViewController.h"
#import "mytoast.h"


@interface GetClientViewController ()
{
    
}
@property(weak,nonatomic)IBOutlet UITableView *tableView;

@end



@implementation GetClientViewController


#pragma mark ---------UIViewController ------------

- (void)viewWillAppear:(BOOL)animated
{
    [super viewWillAppear:animated];
    self.navigationController.navigationBar.hidden = YES;
}


- (void)viewDidLoad {
    [super viewDidLoad];
    
    [_tableView registerNib:[UINib nibWithNibName:@"LoginTableViewCell" bundle:nil] forCellReuseIdentifier:@"loginCell"];
    
    // Do any additional setup after loading the view from its nib.
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

#pragma mark  ----------UITableView------------

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {
    return 2;
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath {
    
    LoginTableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:@"loginCell"];
    
    if (!cell) {
        cell = [[LoginTableViewCell alloc] initWithStyle:UITableViewCellStyleValue1 reuseIdentifier:@"loginCell"];
    }
    if (indexPath.row == 0) {
        cell.textField.placeholder = @"输入用户名";
#ifdef DEBUG
        cell.textField.text = @"119140589@qq.com";
#endif
        cell.leftImageView.image = [UIImage imageNamed:@"login_admin.png"];
        
    } else {
        cell.textField.placeholder = @"输入密码";
#ifdef DEBUG
        cell.textField.text = @"kc123456";
#endif
        cell.textField.secureTextEntry = YES;
        cell.leftImageView.image = [UIImage imageNamed:@"login_pwd.png"];
    }
    return cell;
}



#pragma mark ----------UIButton Event--------

- (IBAction)apphelp:(id)sender
{
    AppHelpViewController *controller = [[AppHelpViewController alloc] init];
    [self presentViewController:controller animated:YES completion:nil];
}


- (IBAction)getClientIDList:(id)sender {
    LoginTableViewCell *row0 = [self.tableView cellForRowAtIndexPath:[NSIndexPath indexPathForRow:0 inSection:0]];
    LoginTableViewCell *row1 = [self.tableView cellForRowAtIndexPath:[NSIndexPath indexPathForRow:1 inSection:0]];
    NSString *userId = row0.textField.text;
    NSString *pwd = row1.textField.text;
    
    if ([userId isEqualToString:@""] || [pwd isEqualToString:@""]) {
        [mytoast showWithText:@"用户名或密码不能为空，请看右上方的\"帮助\""];
        return;
    }
    
    [MBProgressHUD showMessage:@"正在获取账号，请稍等..." toView:self.view];
    
    __weak __typeof(self)weakSelf = self;
    [[HttpRequestEngine engineInstance] login:userId token:pwd successBlock:^(NSDictionary *responseDict) {
        NSDictionary *respDic = [responseDict objectForKey:@"resp"];
        NSNumber *nRespCode = [respDic objectForKey:@"respCode"];
        if ([nRespCode intValue] == 0 && nRespCode != nil) {
            NSArray *array = [respDic objectForKey:@"client"];
            ClientListViewController *controller = [[ClientListViewController alloc] init];
            controller.dataArray = array;
            NSMutableDictionary *dic = [[NSMutableDictionary alloc] init];
            [dic setObject:[respDic objectForKey:@"sid"] forKey:@"sid"];
            [dic setObject:[respDic objectForKey:@"token"] forKey:@"token"];
            controller.accountInfoDic = dic;
            
            [MBProgressHUD hideAllHUDsForView:weakSelf.view animated:YES];
            [mytoast showWithText:@"登录成功"];
            
            controller.loginBlock = ^(NSDictionary *dic,NSArray *arrays) {
                CalledListViewController *controller = [[CalledListViewController alloc] init];
                controller.callingUidDic = dic;
                controller.dataArray = arrays;
                [self.navigationController pushViewController:controller animated:YES];
            };
            [weakSelf presentViewController:controller animated:YES completion:^{
                
            }];
        } else {
            int iRespCode = [nRespCode intValue];
            NSString *message = [NSString stringWithFormat:@"登录失败，错误码：%d",iRespCode];
            [mytoast showWithText:message];
            [MBProgressHUD hideAllHUDsForView:weakSelf.view animated:YES];
        }
    } failBlock:^(NSDictionary *responseDict) {
        [mytoast showWithText:@"登录失败"];
        [MBProgressHUD hideAllHUDsForView:weakSelf.view animated:YES];
    }];
}


#pragma mark -------Orientation------
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

#pragma mark ------------end----------

@end
