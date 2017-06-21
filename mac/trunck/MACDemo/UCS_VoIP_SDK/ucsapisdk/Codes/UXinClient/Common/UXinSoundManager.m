//
//  UXinSoundManager.m
//  UXinClient
//
//  Created by Liam on 13-11-19.
//  Copyright (c) 2013年 UXIN CO. All rights reserved.
//

#import "UXinSoundManager.h"

#import <AudioToolbox/AudioToolbox.h>
#import <AVFoundation/AVFoundation.h>
#import <AVFoundation/AVCaptureDevice.h>
#import <AVFoundation/AVMediaFormat.h>
#import <MediaPlayer/MediaPlayer.h>


static UXinSoundManager *s_pUXinSoundManager = nil;

@implementation UXinSoundManager

+ (UXinSoundManager *) sharedInstance
{
    if (nil == s_pUXinSoundManager)
    {
        s_pUXinSoundManager = [[UXinSoundManager alloc] init];

    }
    
    return s_pUXinSoundManager;
}

// 启动和暂停扬声器
// bOpen = TRUE启动扬声器,否则关闭扬声器
- (void) LoudSpeaker:(BOOL) bOpen
{
/*
    NSLog(@"LoudSpeaker = %d", bOpen);
    [self switchToSpeaker:bOpen];
    return;
    float nSystemVersion = 10.0;
    if( nSystemVersion < 7.0 )
    {
        static BOOL bAlreadyInit = NO;
        if( !bAlreadyInit )
        {
            bAlreadyInit = YES;
            OSStatus nInit = AudioSessionInitialize(NULL, NULL, NULL, NULL);
            NSLog(@"PublicFunc LoudSpeaker AudioSessionInitialize -- nInit = %ld", nInit);
        }

        AudioSessionSetActive(true);

        UInt32 cat = kAudioSessionCategory_PlayAndRecord;
        //    OSStatus nSetCategory = AudioSessionSetProperty(kAudioSessionProperty_AudioCategory, sizeof(cat), &cat);
        //NSLog(@"PublicFunc LoudSpeaker AudioSessionSetProperty kAudioSessionCategory_PlayAndRecord -- nSetCategory = %ld", nSetCategory);

        NSUInteger route = bOpen?kAudioSessionOverrideAudioRoute_Speaker:kAudioSessionOverrideAudioRoute_None;
        AudioSessionSetProperty(kAudioSessionProperty_OverrideAudioRoute, sizeof(route), &route);
    }
    else
    {
        UInt32 route = bOpen ? AVAudioSessionPortOverrideSpeaker : AVAudioSessionPortOverrideNone;
        NSError* outError = nil;

        BOOL bSet = [[AVAudioSession sharedInstance] overrideOutputAudioPort:route error:&outError];
        NSLog(@"iOS7.overrideOutputAudioPort :%d ", bSet?1:0);
    }
*/
}

//  获取系统音量
- (float) getSystemVolume
{
//    MPMusicPlayerController *musicPlayer = [MPMusicPlayerController applicationMusicPlayer];
//    NSLog(@"musicPlayer.volume = %f", musicPlayer.volume);
//    return musicPlayer.volume;
    return 0.68;
}

// 设置系统音量
- (void) setSystemVolume:(float) systemVolume
{
//    MPMusicPlayerController *musicPlayer = [MPMusicPlayerController applicationMusicPlayer];
//    musicPlayer.volume = systemVolume;
}





- (void) switchToSpeaker:(BOOL)bToSpeaker
{
    /*
    NSLog(@"%s %d", __PRETTY_FUNCTION__, bToSpeaker);
    float nSystemVersion = [[[UIDevice currentDevice] systemVersion] floatValue];
    if( nSystemVersion < 7.0 )
    {
//        OSStatus nInit = AudioSessionInitialize(NULL, NULL, NULL, NULL);
//        OSStatus result = AudioSessionSetActive(true);
        OSStatus nSetSpeaker = noErr;
        if( bToSpeaker )
        {
            UInt32 doChangeDefaultRoute = 1;
            nSetSpeaker = AudioSessionSetProperty(kAudioSessionProperty_OverrideCategoryDefaultToSpeaker,
                                                  sizeof(doChangeDefaultRoute),
                                                  &doChangeDefaultRoute);
        }
        else
        {
            UInt32 doChangeDefaultRoute = 0;
            nSetSpeaker = AudioSessionSetProperty(kAudioSessionProperty_OverrideCategoryDefaultToSpeaker,
                                                  sizeof(doChangeDefaultRoute),
                                                  &doChangeDefaultRoute);
        }

        {
            UInt32 route = kAudioSessionOverrideAudioRoute_None;
            if( bToSpeaker )
            {
                route = kAudioSessionOverrideAudioRoute_Speaker;
            }
            else
            {
                route = kAudioSessionOverrideAudioRoute_None;
            }
//            OSStatus nSetSpeaker = AudioSessionSetProperty(kAudioSessionProperty_OverrideAudioRoute, sizeof(route), &route);
            //NSLog( @"Record nSetSpeaker:%ld route:%08lx", nSetSpeaker, route );
        }
    }
    else
    {
        AVAudioSession* pSession = [AVAudioSession sharedInstance];
        NSError* setError = nil;
        BOOL bSet = YES;

        bSet = [pSession setActive:YES error:&setError];

        if( bToSpeaker )
        {
            bSet = [pSession setCategory:AVAudioSessionCategoryPlayAndRecord
                             withOptions:AVAudioSessionCategoryOptionDefaultToSpeaker
                                   error:&setError];

            [pSession overrideOutputAudioPort:AVAudioSessionPortOverrideSpeaker error:&setError];
        }
        else
        {
            bSet = [pSession setCategory:AVAudioSessionCategoryPlayAndRecord
                             withOptions:0
                                   error:&setError];
            [pSession overrideOutputAudioPort:AVAudioSessionPortOverrideNone error:&setError];
        }
    }
     */
}



@end
