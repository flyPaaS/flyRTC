//
//  CodecWindowController.h
//  KCT_VOIP_Demo
//
//  Created by KCMac on 2017/6/9.
//  Copyright © 2017年 flypass. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@protocol CodecSelectDelegate <NSObject>

- (void)selectVideoCodec:(NSInteger)videoCodecIndex audio:(NSInteger)audioCodecIndex;

@end





@interface CodecWindowController : NSWindowController
@property(nonatomic,assign)id<CodecSelectDelegate>delegate;


@end
