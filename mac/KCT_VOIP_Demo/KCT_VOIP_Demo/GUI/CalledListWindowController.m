//
//  CalledListWindowController.m
//  KCT_VOIP_Demo
//
//  Created by KCMac on 2017/5/22.
//  Copyright © 2017年 flypass. All rights reserved.
//

#import "CalledListWindowController.h"
#import <AVFoundation/AVFoundation.h>

@interface CalledListWindowController ()<NSTableViewDelegate,NSTableViewDataSource,VoipEngineDelegate,VideoDelegate,CodecSelectDelegate>
{
    NSInteger _currentSelecctIndex;
}

@property(nonatomic,weak) IBOutlet NSTableView *tableView;

@property(nonatomic,strong) NSMutableArray *cellArrays;
@property(nonatomic,strong) VideoWindowController *videoWinCtl;
@property (nonatomic,strong)AVAudioPlayer *player;
@property(nonatomic,strong) CodecWindowController *codecWinCtl;

@end

@implementation CalledListWindowController


- (void)windowWillLoad {
    [super windowWillLoad];
    _cellArrays = [NSMutableArray new];
    _currentSelecctIndex = -1;
}


- (void)windowDidLoad {
    [super windowDidLoad];
    self.window.title = @"呼叫界面";
    
    [[VoipEngine getInstance] setUIDelegate:self];
    [[VoipEngine getInstance] setVideoCodecWithIndex:0 audioCodecIndex:0];
    
    NSString *path = [[NSBundle mainBundle] pathForResource:@"push" ofType:@"wav"];
    NSURL *url = [NSURL fileURLWithPath:path];
    _player = [[AVAudioPlayer alloc] initWithContentsOfURL:url error:nil];
    _player.volume = 1.0;
    _player.numberOfLoops = -1;//循环播放
    // Implement this method to handle any initialization after your window controller's window has been loaded from its nib file.
}


#pragma mark-----TableView

- (NSInteger)numberOfRowsInTableView:(NSTableView *)tableView
{
    return self.sourceArray.count;
}

- (NSView *)tableView:(NSTableView *)tableView viewForTableColumn:(NSTableColumn *)tableColumn row:(NSInteger)row
{
    int mod = row%6;
    NSDictionary *uidDict;
    uidDict = [self.sourceArray objectAtIndex:row];
    NSString *client_number = [uidDict objectForKey:@"client_number"];
    
    ClientTableCellView *cell = [tableView makeViewWithIdentifier:@"customCell" owner:self];
    
    
    if (![self.sourceArray containsObject:cell]) {
        [self.cellArrays addObject:cell];
    }
    
    
    NSString *imageName = [NSString stringWithFormat:@"head%d_small.png",mod+1];
    cell.textFieldT.stringValue = client_number;
    cell.imageViewT.image = [NSImage imageNamed:imageName];
    [cell.button setAction:@selector(selectAction:)];
    cell.button.tag = row;
    if (_currentSelecctIndex == row) {
        [cell.button setImage:[NSImage imageNamed:@"checkBtn"]];
    }else {
        [cell.button setImage:[NSImage imageNamed:@"unCheckBtn"]];
    }
    
    return cell;
}

- (void)tableViewSelectionDidChange:(NSNotification *)notification
{
    NSInteger row = [self.tableView selectedRow];
    _currentSelecctIndex = row;
    [self changeState:row];
    [self callPhone];
}


#pragma mark----button event
- (void)selectAction:(id)sender
{
    NSButton *button = (NSButton *)sender;
    NSInteger index = button.tag;
    _currentSelecctIndex = index;
    [self changeState:index];
    [self callPhone];
}

- (IBAction)back:(id)sender
{
    [self.delegate showSelfWindow];
    [[KCTTcpClient sharedTcpClientManager] login_uninitWithFlag:YES];
    [self.window close];
    [self.delegate windowBack];
}


- (IBAction)codec:(id)sender
{
    if (!self.codecWinCtl) {
        _codecWinCtl = [[CodecWindowController alloc] initWithWindowNibName:@"CodecWindowController"];
    }
    
    self.codecWinCtl.delegate = self;
    [self.codecWinCtl showWindow:nil];
    [self.codecWinCtl.window center];
}


#pragma mark---private--
- (void)changeState:(NSInteger)index
{
    for (int i = 0; i<self.cellArrays.count; i++) {
        ClientTableCellView *cell = [self.cellArrays objectAtIndex:i];
        if (index == i) {
            [cell.button setImage:[NSImage imageNamed:@"checkBtn"]];
        } else {
            [cell.button setImage:[NSImage imageNamed:@"unCheckBtn"]];
        }
    }
}


- (void)callPhone
{
    NSDictionary *uidDict;
    uidDict = [self.sourceArray objectAtIndex:_currentSelecctIndex];
    NSString *client_number = [uidDict objectForKey:@"client_number"];
    NSLog(@"call number is :%@",client_number);
    
    if (!_videoWinCtl) {
        _videoWinCtl = [[VideoWindowController alloc] initWithWindowNibName:@"VideoWindowController"];
        _videoWinCtl.delegate = self;
        _videoWinCtl.callId = client_number;
    }
    
    [self.videoWinCtl.window center];
    [self.videoWinCtl showWindow:nil];
    [self.videoWinCtl callPhone:client_number];
}



#pragma mark----VideoDelegate

- (void)callDidHangup
{
    self.videoWinCtl = nil;
}


#pragma mark-----codec delegate

- (void)selectVideoCodec:(NSInteger)videoCodecIndex audio:(NSInteger)audioCodecIndex
{
    [[VoipEngine getInstance] setVideoCodecWithIndex:videoCodecIndex audioCodecIndex:audioCodecIndex];
}


#pragma  mark - 通话代理

//tip 展示回调
- (void)showNetWorkState:(NSString *)networkStateStr networkDes:(NSString *)networkDes
{
    [self.videoWinCtl networkStatusChange:[networkStateStr integerValue] networdDes:networkDes];
}

- (void)showNetWorkDetail:(NSString *)networkDetail
{
    NSLog(@"detail %@",networkDetail);
}


//来电信息
-(void)incomingCallID:(NSString*)callid caller:(NSString*)caller phone:(NSString*)phone name:(NSString*)name callStatus:(int)status callType:(NSInteger)calltype
{
    [self.player play];
    NSLog(@"----来电了");
    NSAlert *incomintAlert = [NSAlert alertWithMessageText:caller defaultButton:@"接听" alternateButton:@"拒绝" otherButton:nil informativeTextWithFormat:@"对方正邀请你加入视频电话"];
    NSInteger index = [incomintAlert runModal];
    NSLog(@"=----index :%d",index);
    if (index == 1) {
        if (!_videoWinCtl) {
            _videoWinCtl = [[VideoWindowController alloc] initWithWindowNibName:@"VideoWindowController"];
            _videoWinCtl.callId = caller;
            _videoWinCtl.delegate = self;
        }
        self.videoWinCtl.callId = caller;
        [self.videoWinCtl showWindow:nil];
        [self.videoWinCtl.window center];
        [[VoipEngine getInstance] answer:caller];
    } else {
        [[VoipEngine getInstance] hangUp:caller];
    }
    //self.callId = callid;
    
}

//通话状态回调  信令 交互
-(void)responseVoipManagerStatus:(KCTCallStatus)event callID:(NSString*)callid data:(KCTReason *)data withVideoflag:(int)videoflag
{
    if (event == KCTCallStatus_Answered || event == KCTCallStatus_Released) {
        [self.player stop];
    }
    [_videoWinCtl responseVoipManagerStatus:event callID:callid data:data withVideoflag:videoflag];
}





@end
