//
//  CallingListViewController.m
//  KCT_VOIP_Demo
//
//  Created by KCMac on 2017/1/7.
//  Copyright © 2017年 KCMac. All rights reserved.
//

#import "CallingListViewController.h"


@interface CallingListViewController ()
{
    NSInteger _segmentSelectIndex;
}

//@property(nonatomic,weak)IBOutlet UILabel *label;
@property(nonatomic,strong)NSIndexPath *currentIndexPath;
@property(nonatomic,strong)NSIndexPath *lastIndexPath;
@property(nonatomic,strong)IBOutlet UIView *line;
@property(nonatomic,strong)IBOutlet UITableView *tableView;
@property(nonatomic,strong)UISegmentedControl *segmentedControl;


@end

#pragma mark ----------UIViewController-----

@implementation CallingListViewController

- (void)viewWillAppear:(BOOL)animated
{
    [super viewWillAppear:animated];
    self.navigationController.navigationBar.hidden = NO;
    [LoginManager sharedLoginManager].currentPageIndex = appPageIndexCalling;
}

- (void)viewDidLoad {
    [super viewDidLoad];
    
    _segmentSelectIndex = 0;
    
    [self.navigationItem setHidesBackButton:YES];
    self.title = @"flyCAN";
//    NSArray *segItems = @[@"flySR",@"flyCAN"];
//    _segmentedControl = [[UISegmentedControl alloc] initWithItems:segItems];
//    _segmentedControl.frame = CGRectMake(0, 0, 160, 30);
//    self.navigationItem.titleView = _segmentedControl;
//    _segmentedControl.selectedSegmentIndex = 0;
//    [_segmentedControl addTarget:self action:@selector(segmentedChanged:) forControlEvents:UIControlEventValueChanged];
    
    self.line.backgroundColor = [UIColor colorWithRed:27/255.0 green:165/255.0 blue:38/255.0 alpha:1.0];
    [_tableView registerNib:[UINib nibWithNibName:@"CallListTableViewCell" bundle:nil] forCellReuseIdentifier:@"callCell"];
    if (self.isFromAdressBook) {
        [self autoConnectService];
    }
}



- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}


#pragma mark ------------UIButton Action------
- (IBAction)buttonClick:(id)sender {
    _isAutoLogin = NO;

    if (self.currentIndexPath == nil) {
        [mytoast showWithText:@"请选择其中一个体验账号"];
        return;
    }
    
    NSDictionary *uidDic = [self.dataArray objectAtIndex:self.currentIndexPath.row];
    if (_segmentSelectIndex == 1) {
        uidDic = [self.flycanDataArray objectAtIndex:self.currentIndexPath.row];
    }
    NSString *clientNum = [uidDic objectForKey:@"client_number"];
    NSUserDefaults *userDefault = [NSUserDefaults standardUserDefaults];
    
    [userDefault setObject:clientNum forKey:kLastLoginClientId];
    [userDefault setObject:[NSNumber numberWithInteger:_segmentSelectIndex] forKey:kLastSegmentSelect];
    [userDefault setObject:uidDic forKey:kCallingUidKey];
    [userDefault synchronize];
    
    //get login state
    NSString *sid = [userDefault objectForKey:kAccountSid];
    if ([KCTTcpClient sharedTcpClientManager].login_isConnected) {
        [self connectToCSService:uidDic];
    }
    else
    {
        [[HttpRequestEngine engineInstance] getLoginStateWithSid:sid clientNum:clientNum stateBlock:^(NSDictionary *responseDict) {
            if (responseDict) {
                NSDictionary *resp = [responseDict objectForKey:@"resp"];
                if (resp) {
                    NSString *stateStr = [resp objectForKey:@"state"];
                    if ([stateStr intValue] != 1) {
                        [self connectToCSService:uidDic];
                    } else {
                        [mytoast showWithText:@"该账号已经登录"];
                    }
                }else {
                    [mytoast showWithText:@"服务器错误"];
                }
            }else {
                [mytoast showWithText:@"服务器错误"];
            }
        }];
    }
}

#pragma mark-
#pragma mark-------------public-------
- (void)autoConnectService {
    [self autoConnectToCSService];
}

#pragma mark ------------private function-----------

- (void)autoConnectToCSService {
    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
    NSNumber *lastSegmentIndex = [defaults objectForKey:kLastSegmentSelect];
    _segmentSelectIndex = [lastSegmentIndex integerValue];
    _segmentedControl.selectedSegmentIndex = _segmentSelectIndex;
    NSString *clientId = [defaults objectForKey:kLastLoginClientId];
    if (clientId) {
        NSString *format = [NSString stringWithFormat:@"client_number = '%@'",clientId];
        NSPredicate *predicate = [NSPredicate predicateWithFormat:format];
        NSArray *dicArray = [self.dataArray filteredArrayUsingPredicate:predicate];
        if (_segmentSelectIndex == 1) {
            dicArray = [self.flycanDataArray filteredArrayUsingPredicate:predicate];
        }
        NSDictionary *uidDic = [dicArray objectAtIndex:0];
        if (uidDic) {
            NSUserDefaults *userDefault = [NSUserDefaults standardUserDefaults];
            [userDefault setObject:[NSNumber numberWithInteger:_segmentSelectIndex] forKey:kLastSegmentSelect];
            [userDefault setObject:clientId forKey:kLastLoginClientId];
            [userDefault setObject:uidDic forKey:kCallingUidKey];
            [userDefault synchronize];
            
            NSInteger index = [self.dataArray indexOfObject:uidDic];
            if (_segmentSelectIndex == 1) {
                index = [self.flycanDataArray indexOfObject:uidDic];
            }
            NSIndexPath *indexPath = [NSIndexPath indexPathForRow:index inSection:0];
            self.currentIndexPath = indexPath;
            
            [self connectToCSService:uidDic];
        }
    }
}

- (void)connectToCSService:(NSDictionary *)uidDic {
    NSString *uid = [uidDic objectForKey:@"client_number"];
    NSString *pwd = [uidDic objectForKey:@"client_pwd"];
    NSString *main_account = [self.accountInfoDic objectForKey:@"sid"];
    NSString *main_token = [self.accountInfoDic objectForKey:@"token"];;
    
    [MBProgressHUD showMessage:@"正在登入账号，请稍等..." toView:self.view];
    __weak __typeof(self)weakSelf = self;
    [[KCTTcpClient sharedTcpClientManager] connect:main_account accountToken:main_token clientNumber:uid clientPwd:pwd success:^(NSString *userId) {
        
        BOOL isIpv6 = [[KCTTcpClient sharedTcpClientManager] isSocketIPV6];
        if (isIpv6)
        {
            [[KCTFuncEngine getInstance] setSocketIpv6:isIpv6];
        }
        
        dispatch_async(dispatch_get_main_queue(), ^{
            [MBProgressHUD hideAllHUDsForView:weakSelf.view animated:YES];
            [mytoast showWithText:@"登入成功"];
            
            NSMutableArray *tempArray = [self.dataArray mutableCopy];
            if (_segmentSelectIndex == 1) {
                tempArray = [self.flycanDataArray mutableCopy];
            }
            [tempArray removeObjectAtIndex:self.currentIndexPath.row];
            NSArray *arrays = [NSArray arrayWithArray:tempArray];
            
            weakSelf.loginBlock(uidDic,arrays);
            CalledListViewController *controller;
            if (_isAutoLogin) {
                NSMutableArray *arrays = [[LoginManager  sharedLoginManager] getControllerArrays];
                if ([arrays count] >= 3) {
                    controller = (CalledListViewController *)[arrays objectAtIndex:2];
                } else {
                    controller = [[CalledListViewController alloc] init];
                }
            } else {
                controller = [[CalledListViewController alloc] init];
            }
            
            controller.callingUidDic = uidDic;
            controller.dataArray = arrays;
            if (_isAutoLogin) {
                [[LoginManager sharedLoginManager] addController:controller];
            }
            if (!_isAutoLogin) {
            [weakSelf.navigationController pushViewController:controller animated:YES];
            }
            
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

#pragma mark-
#pragma mark-------segmentedChanged---------
- (void)segmentedChanged:(id)sender {
    UISegmentedControl *segment = (UISegmentedControl *)sender;
    _segmentSelectIndex = segment.selectedSegmentIndex;
    self.lastIndexPath = nil;
    [self.tableView reloadData];
}


#pragma mark ----------UITableView-------------
- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {
    if (_segmentSelectIndex == 1) {
        return self.flycanDataArray.count;
    }
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
    if (_segmentSelectIndex == 1) {
        uidDict = [self.flycanDataArray objectAtIndex:indexPath.row];
    }
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
