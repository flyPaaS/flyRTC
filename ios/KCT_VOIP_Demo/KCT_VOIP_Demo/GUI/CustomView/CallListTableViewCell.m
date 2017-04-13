//
//  CallListTableViewCell.m
//  KCT_VOIP_Demo
//
//  Created by KCMac on 2017/1/16.
//  Copyright © 2017年 KCMac. All rights reserved.
//

#import "CallListTableViewCell.h"

@implementation CallListTableViewCell

- (void)awakeFromNib {
    [super awakeFromNib];
    CGRect frame = self.line.frame;
    self.line.frame = CGRectMake(frame.origin.x, frame.origin.y, frame.size.width, 0.5);
    self.line.backgroundColor = [UIColor lightGrayColor];
    // Initialization code
}

- (void)setSelected:(BOOL)selected animated:(BOOL)animated {
    [super setSelected:selected animated:animated];

    // Configure the view for the selected state
}

@end
