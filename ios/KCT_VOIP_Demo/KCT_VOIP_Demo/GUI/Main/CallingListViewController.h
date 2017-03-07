//
//  CallingListViewController.h
//  KCT_VOIP_Demo
//
//  Created by KCMac on 2017/1/7.
//  Copyright © 2017年 KCMac. All rights reserved.
//

#import <UIKit/UIKit.h>


typedef void(^loginSusscessBlock)(NSDictionary *dic,NSArray *arrays);

@interface CallingListViewController : UIViewController<UITableViewDelegate,UITableViewDataSource>

@property(nonatomic,strong)NSDictionary *callingUidDic;
@property(nonatomic,strong)NSArray *dataArray;
@property(nonatomic,strong)NSMutableDictionary *accountInfoDic;
@property(nonatomic,copy)loginSusscessBlock loginBlock;
@property(nonatomic,assign)BOOL isAutoLogin;


- (void)autoConnectService;


@end
