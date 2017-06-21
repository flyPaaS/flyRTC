//
//  VideoWindowController.h
//  KCT_VOIP_Demo
//
//  Created by KCMac on 2017/6/5.
//  Copyright © 2017年 flypass. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import <AVFoundation/AVFoundation.h>


@protocol VideoDelegate <NSObject>

- (void)callDidHangup;

@end

@interface VideoWindowController : NSWindowController

@property(nonatomic,strong) NSString *callId;
@property(nonatomic,assign) id<VideoDelegate>delegate;

- (void)callPhone:(NSString *)callid;
-(void)responseVoipManagerStatus:(KCTCallStatus)event callID:(NSString*)callid data:(KCTReason *)data withVideoflag:(int)videoflag;
- (void)networkStatusChange:(NSInteger)currentNetwork networdDes:(NSString *)networdDes;

@end
