//
//  VideoWindowController.m
//  KCT_VOIP_Demo
//
//  Created by KCMac on 2017/6/5.
//  Copyright © 2017年 flypass. All rights reserved.
//

#import "VideoWindowController.h"

@interface VideoWindowController ()
{
    int hhInt;
    int mmInt;
    int ssInt;
    NSTimer *timer;
}

@property(nonatomic,weak) IBOutlet NSView *remoteBackgroundView;
@property(nonatomic,weak) IBOutlet NSTextField *nickLabel;
@property(nonatomic,weak) IBOutlet NSTextField *timesLabel;
@property(nonatomic,weak) IBOutlet NSView *localBackgroundView;
@property(nonatomic,weak) IBOutlet NSView *bottomView;
@property(nonatomic,strong) IBOutlet NSTextField *infoLabel;
@property(nonatomic,strong) NSOpenGLView *videoLocationView;
@property(nonatomic,strong) NSOpenGLView *videoRemoteView;



@end


@implementation VideoWindowController



- (void)windowDidLoad {
    [super windowDidLoad];
    
    CGRect locationRect = self.localBackgroundView.bounds;
    
    self.videoLocationView = [[VoipEngine getInstance] allocCameraViewWithFrame:locationRect];
    self.localBackgroundView.layer.backgroundColor = [NSColor blackColor].CGColor;
    [self.localBackgroundView addSubview:self.videoLocationView];
    
    
    CGRect remoteRect = self.remoteBackgroundView.bounds;
    self.videoRemoteView = [[VoipEngine getInstance] allocCameraViewWithFrame:remoteRect];
    self.remoteBackgroundView.layer.backgroundColor = [NSColor blackColor].CGColor;
    [self.remoteBackgroundView addSubview:self.videoRemoteView];
    
    [[VoipEngine getInstance] initCameraConfig:self.videoLocationView withRemoteVideoView:self.videoRemoteView];
    
    [self.nickLabel removeFromSuperview];
    [self.remoteBackgroundView addSubview:self.nickLabel positioned:NSWindowAbove relativeTo:self.videoRemoteView];
    [self.timesLabel removeFromSuperview];
    [self.remoteBackgroundView addSubview:self.timesLabel positioned:NSWindowAbove relativeTo:self.videoRemoteView];
    
    [self.localBackgroundView removeFromSuperview];
    [self.remoteBackgroundView addSubview:self.localBackgroundView positioned:NSWindowAbove relativeTo:self.videoRemoteView];
    
    [self.bottomView removeFromSuperview];
    [self.remoteBackgroundView addSubview:self.bottomView positioned:NSWindowAbove relativeTo:self.videoRemoteView];
    NSColor *color = [NSColor colorWithRed:0 green:0 blue:0 alpha:0.8];
    self.bottomView.layer.backgroundColor = color.CGColor;
    
    //window close button
    NSButton *closeBtn = [self.window standardWindowButton:NSWindowCloseButton];
    [closeBtn setTarget:self];
    [closeBtn setAction:@selector(closeWindow:)];
    
    //data
    self.nickLabel.stringValue = self.callId;
    
    
    [self.window setBackgroundColor:[NSColor colorWithRed:0.0 green:0.0 blue:0.0 alpha:1]];
    self.window.titlebarAppearsTransparent = YES;
    
    [self.infoLabel removeFromSuperview];
    self.infoLabel.textColor = [NSColor redColor];
    self.infoLabel.hidden = YES;
    [self.remoteBackgroundView addSubview:self.infoLabel positioned:NSWindowAbove relativeTo:self.videoRemoteView];
    
    NSClickGestureRecognizer *recognizer = [[NSClickGestureRecognizer alloc] initWithTarget:self action:@selector(ss)];
    recognizer.numberOfClicksRequired = 3;
    [self.videoLocationView addGestureRecognizer:recognizer];
    
//    NSTrackingArea* trackingArea = [[NSTrackingArea alloc] initWithRect:self.videoLocationView.bounds options: (NSTrackingMouseEnteredAndExited | NSTrackingActiveAlways) owner:self userInfo:nil];
//    [self.videoLocationView addTrackingArea:trackingArea];
    
    
    // Implement this method to handle any initialization after your window controller's window has been loaded from its nib file.
}




//- (void) mouseEntered:(NSEvent*)theEvent {
//    // Mouse entered tracking area.
//    //self.infoLabel.hidden = NO;
//}
//
//- (void) mouseExited:(NSEvent*)theEvent {
//    // Mouse exited tracking area.
//    //self.infoLabel.hidden = YES;
//}



#pragma mark----public
- (void)callPhone:(NSString *)callid
{
    [[VoipEngine getInstance] dial:KCTCallType_VideoPhone andCalled:callid andUserdata:@"视频通话"];
}


-(void)responseVoipManagerStatus:(KCTCallStatus)event callID:(NSString*)callid data:(KCTReason *)data withVideoflag:(int)videoflag
{
    self.timesLabel.hidden = NO;
    
    switch (event)
    {
        case KCTCallStatus_Alerting:{
            self.timesLabel.stringValue = @"对方正在响铃";
            
        }
            break;
        case KCTCallStatus_Answered:
        {
            self.timesLabel.stringValue = @"00:00";
            
            if (![timer isValid])
            {
                timer = [NSTimer timerWithTimeInterval:1.0f target:self selector:@selector(updateRealtimeLabel) userInfo:nil repeats:YES];
                [[NSRunLoop currentRunLoop] addTimer:timer forMode:NSDefaultRunLoopMode];
                [timer fire];
            }
            
            /**
             @author WLS, 15-12-11 17:12:12
             
             当通话接通后，显示功能按钮，并调整挂断按钮的位置
             */
//            self.callFunctionView.hidden = NO;
//            self.switchCameraButton.hidden = NO;
//            [self handfree];
//            
//            [self changeHangupButtonFrame];
            
        }
            break;
            
        case KCTCallStatus_Released:
        {
            
//            AppDelegate *app = (AppDelegate *)[UIApplication sharedApplication].delegate;
//            NSLog(@"KCTVideoCallCongtroller 964");
//            [app stopCall];
//            
//            [self setCallDuration];
//            
//            self.hangupButton.enabled = NO;
//            [self.callTimeLabel setImage:nil forState:UIControlStateDisabled];
            //            if (self.incomingCall) {
            //                [self.callTimeLabel setTitle:@"已挂机" forState:UIControlStateDisabled];
            //
            //            }else{
            //                [self.callTimeLabel setTitle:data.msg forState:UIControlStateDisabled];
            //
            //            }
            
            
            if (data.reason == 402016) {
                self.timesLabel.stringValue = @"对方已挂机";
            }else if (data.reason == 402012) {
                /**
                 @author WLS, 15-12-21 11:12:04
                 
                 被叫拒绝接听
                 */
                self.timesLabel.stringValue = @"对方拒绝接听";
            }else if (data.reason == 402013){
                
                //[self.callTimeLabel setTitle:data.msg forState:UIControlStateDisabled];
                
            }else{
                self.timesLabel.stringValue = @"对方无应答";
            }
            [self delayCloseWindow];
            
        }
            break;
        case KCTCallStatus_Failed:
        {
            
            if (data.reason==402050) {
                self.timesLabel.stringValue = @"对方无应答";
            }
            
            [self delayCloseWindow];
        }
            break;
        case KCTCallStatus_Transfered:
        {
            [self delayCloseWindow];
        }
            break;
        case KCTCallStatus_Pasused:
        {
            self.timesLabel.stringValue = @"呼叫保持";
        }
            break;
        default:
            break;
    }
     
}


- (void)networkStatusChange:(NSInteger)currentNetwork networdDes:(NSString *)networdDes{
    NSString * imageStr = nil;
    switch (currentNetwork) {
        case KCTNetwork_Bad:
        {
            //差
            imageStr = @"视频信号-无";
        }
            break;
        case KCTNetwork_General:
        {
            //一般
            imageStr = @"视频信号1";
            
        }
            break;
        case KCTNetwork_Nice:
        {
            //良好
            imageStr = @"视频信号2";
            
        }
            break;
        case KCTNetwork_Well:
        {
            //优秀
            imageStr = @"视频信号3";
            
        }
            break;
        default:
            break;
    }
    self.infoLabel.stringValue = networdDes;
}


#pragma mark----private

- (void)setMute
{
    UInt32 propertySize = 0;
    OSStatus status = noErr;
    AudioObjectPropertyAddress propertyAOPA;
    propertyAOPA.mElement = kAudioObjectPropertyElementMaster;
    propertyAOPA.mScope = kAudioDevicePropertyScopeOutput;
    propertyAOPA.mSelector = kAudioDevicePropertyMute;
    AudioDeviceID outputDeviceId = [self getAudioDeviceId];
    if (outputDeviceId == kAudioObjectUnknown) {
        NSLog(@"Unknown device");
        return;
    }
    Boolean canSetVolume = NO;
    status = AudioHardwareServiceIsPropertySettable(outputDeviceId, &propertyAOPA, &canSetVolume);
    if (status || canSetVolume == NO)
    {
        NSLog(@"Device 0x%0x does not support volume control", outputDeviceId);
        return;
    }
    propertySize = sizeof(UInt32);
    UInt32 mute = 1;
    status = AudioHardwareServiceSetPropertyData(outputDeviceId, &propertyAOPA, 0, NULL, propertySize, &mute);
    if (status)
    {
        NSLog(@"Unable to set volume for device 0x%0x", outputDeviceId);
    }
}

- (void)setSysVolume:(Float32)newVolume
{
    if (newVolume < 0.0 || newVolume > 1.0) {
        NSLog(@"Requested volume out of range (%.2f)", newVolume);
        return ;
    }
    
    UInt32 propertySize = 0;
    OSStatus status = noErr;
    AudioObjectPropertyAddress propertyAOPA;
    propertyAOPA.mElement = kAudioObjectPropertyElementMaster;
    propertyAOPA.mScope = kAudioDevicePropertyScopeOutput;
    if (newVolume < 0.001) {
        NSLog(@"Requested mute");
        propertyAOPA.mSelector = kAudioDevicePropertyMute;
    } else {
        NSLog(@"Requested volume %.2f", newVolume);
        propertyAOPA.mSelector = kAudioHardwareServiceDeviceProperty_VirtualMasterVolume;
    }
    AudioDeviceID outputDeviceId = [self getAudioDeviceId];
    if (outputDeviceId == kAudioObjectUnknown) {
        NSLog(@"Unknown device");
        return;
    }
    Boolean canSetVolume = NO;
    status = AudioHardwareServiceIsPropertySettable(outputDeviceId, &propertyAOPA, &canSetVolume);
    if (status || canSetVolume == NO)
    {
        NSLog(@"Device 0x%0x does not support volume control", outputDeviceId);
        return;
    }
    if (propertyAOPA.mSelector == kAudioDevicePropertyMute)
    {
        propertySize = sizeof(UInt32);
        UInt32 mute = 1;
        status = AudioHardwareServiceSetPropertyData(outputDeviceId, &propertyAOPA, 0, NULL, propertySize, &mute);
    }
    else
    {
        propertySize = sizeof(Float32);
        status = AudioHardwareServiceSetPropertyData(outputDeviceId, &propertyAOPA, 0, NULL, propertySize, &newVolume);
        if (status)
        {
            NSLog(@"Unable to set volume for device 0x%0x", outputDeviceId);
        }
        // make sure we're not muted
        propertyAOPA.mSelector = kAudioDevicePropertyMute;
        propertySize = sizeof(UInt32);
        UInt32 mute = 0;
        if (!AudioHardwareServiceHasProperty(outputDeviceId, &propertyAOPA))
        {
            NSLog(@"Device 0x%0x does not support muting", outputDeviceId);
            return;
        }
        Boolean canSetMute = NO;
        status = AudioHardwareServiceIsPropertySettable(outputDeviceId, &propertyAOPA, &canSetMute);
        if (status || !canSetMute)
        {
            NSLog(@"Device 0x%0x does not support muting", outputDeviceId);
            return;
        }
        status = AudioHardwareServiceSetPropertyData(outputDeviceId, &propertyAOPA, 0, NULL, propertySize, &mute);
    }
    if (status)
    {
        NSLog(@"Unable to set volume for device 0x%0x", outputDeviceId);
    }
}

-(AudioDeviceID)getAudioDeviceId
{
    AudioDeviceID outputDeviceId = kAudioDeviceUnknown;
    
    OSStatus status = noErr;
    AudioObjectPropertyAddress propertyAOPA;
    propertyAOPA.mScope = kAudioObjectPropertyScopeGlobal;
    propertyAOPA.mElement = kAudioObjectPropertyElementMaster;
    propertyAOPA.mSelector = kAudioHardwarePropertyDefaultOutputDevice;
    
    if (!AudioHardwareServiceHasProperty(kAudioObjectSystemObject, &propertyAOPA))
    {
        printf("Cannot find default output device!\n");
        return outputDeviceId;
    }
    
    status = AudioHardwareServiceGetPropertyData(kAudioObjectSystemObject,  &propertyAOPA, 0, NULL, (UInt32[]){sizeof(AudioDeviceID)}, &outputDeviceId);
    if (status != 0) {
        printf("Cannot find default output device!");
    }
    return outputDeviceId;
}


- (void)updateRealtimeLabel
{
    ssInt +=1;
    if (ssInt >= 60) {
        mmInt += 1;
        ssInt -= 60;
        if (mmInt >=  60) {
            hhInt += 1;
            mmInt -= 60;
            if (hhInt >= 24) {
                hhInt = 0;
            }
        }
    }
    
    if(ssInt > 0 && ssInt % 4 == 0 )
    {
        
    }
    if (hhInt > 0) {
        self.timesLabel.stringValue = [NSString stringWithFormat:@"%02d:%02d:%02d",hhInt,mmInt,ssInt];
    }
    else
    {
        self.timesLabel.stringValue = [NSString stringWithFormat:@"%02d:%02d",mmInt,ssInt];
    }
}


- (void)delayCloseWindow
{
    dispatch_time_t time = dispatch_time(DISPATCH_TIME_NOW, NSEC_PER_SEC);
    dispatch_after(time, dispatch_get_main_queue(), ^{
        [self close];
        [self.delegate callDidHangup];
    });
    
}

- (void)sendHangUpCmd
{
    [[VoipEngine getInstance] hangUp:self.callId];
}


#pragma mark-----button
-(IBAction)hangup:(id)sender
{
    [self sendHangUpCmd];
    [self delayCloseWindow];
}

-(IBAction)closeWindow:(id)sender
{
    [self sendHangUpCmd];
    dispatch_time_t time = dispatch_time(DISPATCH_TIME_NOW, 0.5*NSEC_PER_SEC);
    dispatch_after(time, dispatch_get_main_queue(), ^{
        [self close];
        [self.delegate callDidHangup];
    });
}

- (IBAction)setMicMute:(id)sender
{
    [self setMute];
    //[[VoipEngine getInstance] setMicMute:YES];//设置为静音
}

- (IBAction)closeCamera:(id)sender
{
    [[VoipEngine getInstance] switchVideoMode:CAMERA_RECEIVE];//关闭
    //[[KCTFuncEngine getInstance] switchVideoMode:CAMERA_NORMAL];//打开
}

//手势
- (void)ss
{
    self.infoLabel.hidden = NO;
}

- (void)windowWillClose:(NSNotification *)notification
{
    
}

@end
