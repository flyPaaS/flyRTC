//
//  ClientListViewController.h
//  KCT_VOIP_Demo
//
//  Created by KCMac on 2017/1/7.
//  Copyright © 2017年 KCMac. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "KCTFuncEngine.h"

typedef enum _clientType {
    clientTypeCalling,
    clientTypeCalled
}clientType;

typedef void(^loginSusscessBlock)(NSDictionary *dic,NSArray *arrays);

@interface ClientListViewController : UIViewController<UITableViewDelegate,UITableViewDataSource>

@property(nonatomic,assign)clientType listType;
@property(nonatomic,strong)NSDictionary *callingUidDic;
@property(nonatomic,strong)NSArray *dataArray;
@property(nonatomic,strong)NSMutableDictionary *accountInfoDic;
@property(nonatomic,copy)loginSusscessBlock loginBlock;

@end
