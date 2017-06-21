//
//  PhoneWindowController.m
//  KCT_VOIP_Demo
//
//  Created by KCMac on 2017/5/22.
//  Copyright © 2017年 flypass. All rights reserved.
//

#import "PhoneWindowController.h"

@interface PhoneWindowController ()<NSTableViewDelegate,NSTableViewDataSource,VoipEngineDelegate>
{
    NSInteger _currentSelecctIndex;
}


@property(nonatomic,weak) IBOutlet NSTableView *tableView;
@property(nonatomic,weak) IBOutlet NSView *remoteBackgroundView;
@property(nonatomic,weak) IBOutlet NSView *localBackgroundView;
@property(nonatomic,strong) NSOpenGLView *videoLocationView;
@property(nonatomic,strong) NSOpenGLView *videoRemoteView;

@property(nonatomic,strong) NSMutableArray *cellArrays;

@end

@implementation PhoneWindowController


- (void)windowWillLoad {
    [super windowWillLoad];
    _cellArrays = [NSMutableArray new];
    _currentSelecctIndex = -1;
}


- (void)windowDidLoad {
    [super windowDidLoad];
    self.window.title = @"呼叫界面";
    
    CGRect locationRect = self.localBackgroundView.bounds;

    self.videoLocationView = [[VoipEngine getInstance] allocCameraViewWithFrame:locationRect];
    self.localBackgroundView.layer.backgroundColor = [NSColor blackColor].CGColor;
    [self.localBackgroundView addSubview:self.videoLocationView];
    
    
    CGRect remoteRect = self.remoteBackgroundView.bounds;
    self.videoRemoteView = [[VoipEngine getInstance] allocCameraViewWithFrame:remoteRect];
    self.remoteBackgroundView.layer.backgroundColor = [NSColor blackColor].CGColor;
    [self.remoteBackgroundView addSubview:self.videoRemoteView];
    
    [[VoipEngine getInstance] initCameraConfig:self.videoLocationView withRemoteVideoView:self.videoRemoteView];
    
    [[VoipEngine getInstance] setUIDelegate:self];
    [[VoipEngine getInstance] setVideoCodecWithIndex:0 audioCodecIndex:0];
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
}

#pragma mark---private--
- (void)selectAction:(id)sender
{
    NSButton *button = (NSButton *)sender;
    NSInteger index = button.tag;
    _currentSelecctIndex = index;
    [self changeState:index];
}

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


#pragma mark----button
-(IBAction)callPhone:(id)sender
{
    if (_currentSelecctIndex == -1) {
        NSAlert *alert = [NSAlert alertWithMessageText:@"提示" defaultButton:@"确定" alternateButton:nil otherButton:nil informativeTextWithFormat:@"请选择一个体验账号!"];
        NSUInteger action = [alert runModal];
        return;
    }
    
    NSDictionary *uidDict;
    uidDict = [self.sourceArray objectAtIndex:_currentSelecctIndex];
    NSString *client_number = [uidDict objectForKey:@"client_number"];
    NSLog(@"call number is :%@",client_number);
    
    [[VoipEngine getInstance] dial:KCTCallType_VideoPhone andCalled:client_number andUserdata:@"视频通话"];
}

-(IBAction)answer:(id)sender
{
    
}

-(IBAction)hangup:(id)sender
{
    
}


#pragma  mark - 通话代理

//tip 展示回调
- (void)showNetWorkState:(NSString *)networkStateStr networkDes:(NSString *)networkDes
{
    NSLog(@"--networkstate %@   -- %@",networkStateStr,networkDes);
}

- (void)showNetWorkDetail:(NSString *)networkDetail
{
    NSLog(@"detail %@",networkDetail);
}


//来电信息
-(void)incomingCallID:(NSString*)callid caller:(NSString*)caller phone:(NSString*)phone name:(NSString*)name callStatus:(int)status callType:(NSInteger)calltype
{
    NSLog(@"----来电了");
}

//通话状态回调  信令 交互
-(void)responseVoipManagerStatus:(KCTCallStatus)event callID:(NSString*)callid data:(KCTReason *)data withVideoflag:(int)videoflag
{
    NSLog(@"通话状态 %d",event);
}





@end
