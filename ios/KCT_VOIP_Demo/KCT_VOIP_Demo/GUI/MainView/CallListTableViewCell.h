//
//  CallListTableViewCell.h
//  KCT_VOIP_Demo
//
//  Created by KCMac on 2017/1/16.
//  Copyright © 2017年 KCMac. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface CallListTableViewCell : UITableViewCell

@property(weak,nonatomic)IBOutlet UIImageView *leftImageView;
@property(weak,nonatomic)IBOutlet UILabel *label1;
@property(weak,nonatomic)IBOutlet UILabel *label2;
@property(weak,nonatomic)IBOutlet UIButton *buttonT;
@property(weak,nonatomic)IBOutlet UIView *line;

@end
