//
//  AppHelpViewController.m
//  KCT_VOIP_Demo
//
//  Created by KCMac on 2017/1/20.
//  Copyright © 2017年 KCMac. All rights reserved.
//

#import "AppHelpViewController.h"

@interface AppHelpViewController ()

@end


@implementation AppHelpViewController

- (void)viewWillAppear:(BOOL)animated
{
    [super viewWillAppear:animated];
    [LoginManager sharedLoginManager].currentPageIndex = appPageIndexHelp;
}

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view from its nib.
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}


- (IBAction)close:(id)sender
{
    [self dismissViewControllerAnimated:YES completion:nil];
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
