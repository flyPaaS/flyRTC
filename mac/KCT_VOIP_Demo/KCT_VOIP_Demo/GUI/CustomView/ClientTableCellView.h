//
//  ClientTableCellView.h
//  KCT_VOIP_Demo
//
//  Created by KCMac on 2017/5/22.
//  Copyright © 2017年 flypass. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@interface ClientTableCellView : NSTableCellView

@property(nonatomic,weak)IBOutlet NSImageView *imageViewT;
@property(nonatomic,weak)IBOutlet NSTextField *textFieldT;
@property(nonatomic,weak)IBOutlet NSButton *button;


@end
