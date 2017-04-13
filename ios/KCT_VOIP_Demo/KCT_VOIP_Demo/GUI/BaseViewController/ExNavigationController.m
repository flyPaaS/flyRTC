//
//  ExNavigationController.m
//  KCT_VOIP_Demo
//
//  Created by KCMac on 2017/1/16.
//  Copyright © 2017年 KCMac. All rights reserved.
//

#import "ExNavigationController.h"

@interface ExNavigationController ()

@end

@implementation ExNavigationController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}


//是否跟随屏幕旋转
-(BOOL)shouldAutorotate{
    //return self.topViewController.shouldAutorotate;
    return NO;
}
//支持旋转的方向有哪些
-(UIInterfaceOrientationMask)supportedInterfaceOrientations{
    //return [self.topViewController supportedInterfaceOrientations];
    return UIInterfaceOrientationMaskPortrait;
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
