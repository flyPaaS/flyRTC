//
//  CalledListWindowController.h
//  KCT_VOIP_Demo
//
//  Created by KCMac on 2017/5/22.
//  Copyright © 2017年 flypass. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@protocol CalledListDelegate <NSObject>

- (void)showSelfWindow;
- (void)windowBack;

@end


@interface CalledListWindowController : NSWindowController

@property(nonatomic,strong)NSArray *sourceArray;
@property(nonatomic,assign) id<CalledListDelegate>delegate;

@end
