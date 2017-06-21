//
//  CodecWindowController.m
//  KCT_VOIP_Demo
//
//  Created by KCMac on 2017/6/9.
//  Copyright © 2017年 flypass. All rights reserved.
//

#import "CodecWindowController.h"


@interface CodecWindowController ()

@property (nonatomic,weak) IBOutlet NSPopUpButton *videoPopUp;
@property (nonatomic,weak) IBOutlet NSPopUpButton *audioPopUp;

@end

@implementation CodecWindowController

- (void)windowDidLoad {
    [super windowDidLoad];
    
    self.window.title = @"Codec 设置";
    // Implement this method to handle any initialization after your window controller's window has been loaded from its nib file.
}


#pragma mark----button click

- (IBAction)setting:(id)sender
{
    NSInteger videoIndex = [self.videoPopUp  indexOfSelectedItem];
    NSInteger audioIndex = [self.audioPopUp indexOfSelectedItem];
    if (videoIndex%2 == 1) {
        videoIndex = videoIndex-1;
    }
    NSLog(@"video index :%ld",(long)videoIndex);
    NSLog(@"audio index :%ld",(long)audioIndex);
    
    if (self.delegate) {
        [self.delegate selectVideoCodec:videoIndex audio:audioIndex];
    }
    [self close];
}

@end
