//
//  LoginTableViewCell.h
//  KCT_VOIP_Demo
//
//  Created by KCMac on 2017/1/16.
//  Copyright © 2017年 KCMac. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface LoginTableViewCell : UITableViewCell<UITextFieldDelegate>

@property(weak,nonatomic)IBOutlet UIImageView *leftImageView;
@property(weak,nonatomic)IBOutlet UITextField *textField;


@end
