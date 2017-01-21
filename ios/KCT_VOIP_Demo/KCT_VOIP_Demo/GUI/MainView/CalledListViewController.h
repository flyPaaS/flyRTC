//
//  CalledListViewController.h
//  KCT_VOIP_Demo
//
//  Created by KCMac on 2017/1/16.
//  Copyright © 2017年 KCMac. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface CalledListViewController : UIViewController<UITableViewDelegate,UITableViewDataSource>


@property(nonatomic,strong)NSDictionary *callingUidDic;
@property(nonatomic,strong)NSArray *dataArray;
@property(nonatomic,strong)NSMutableDictionary *accountInfoDic;


@end
