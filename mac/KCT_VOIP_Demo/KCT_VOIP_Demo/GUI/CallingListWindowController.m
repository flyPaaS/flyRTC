//
//  CallingListWindowController.m
//  KCT_VOIP_Demo
//
//  Created by KCMac on 2017/5/22.
//  Copyright © 2017年 flypass. All rights reserved.
//

#import "CallingListWindowController.h"

@interface CallingListWindowController ()<NSTableViewDelegate,NSTableViewDataSource,CalledListDelegate>
{
    NSInteger _segmentIndex;
    NSInteger _currentSelecctIndex;
    BOOL _isLoading;
}

@property(nonatomic,weak) IBOutlet NSSegmentedControl *segmentCtl;
@property(nonatomic,weak) IBOutlet NSTableView *tableView;
@property(nonatomic,weak) IBOutlet NSProgressIndicator *progessIndicator;
@property(nonatomic,weak) IBOutlet NSTextField *loadText;
@property(nonatomic,strong) CalledListWindowController *phoneWin;
@end

@implementation CallingListWindowController

- (void)windowWillLoad {
    [super windowWillLoad];
    _currentSelecctIndex = -1;
    _isLoading = NO;
}

- (void)windowDidLoad {
    [super windowDidLoad];
    
    self.window.title = @"请选择体验账号";
    self.segmentCtl.integerValue = 0;
    self.progessIndicator.hidden = YES;
    self.loadText.hidden = YES;
    self.loadText.stringValue = @"正在登录...";
    _segmentIndex = 0;
    
    // Implement this method to handle any initialization after your window controller's window has been loaded from its nib file.
}


#pragma mark-----TableView

- (NSInteger)numberOfRowsInTableView:(NSTableView *)tableView
{
    if (_segmentIndex == 0)
    {
        return self.SRArrays.count;
    }
    
    return self.CanArrays.count;
}

- (NSView *)tableView:(NSTableView *)tableView viewForTableColumn:(NSTableColumn *)tableColumn row:(NSInteger)row
{
    int mod = row%6;
    NSDictionary *uidDict;
    if (_segmentIndex ==0) {
        uidDict = [self.SRArrays objectAtIndex:row];
    } else {
        uidDict = [self.CanArrays objectAtIndex:row];
    }
    NSString *client_number = [uidDict objectForKey:@"client_number"];
    
    ClientTableCellView *cell = [tableView makeViewWithIdentifier:@"customCell" owner:self];
    
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
    NSInteger currentRow = [self.tableView selectedRow];
    _currentSelecctIndex = currentRow;
    [self changeState:currentRow];
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
    [self.tableView enumerateAvailableRowViewsUsingBlock:^(NSTableRowView *rowView, NSInteger row) {
        
        ClientTableCellView *cell = [rowView viewAtColumn:0];
        if (index == row) {
            [cell.button setImage:[NSImage imageNamed:@"checkBtn"]];
        } else {
            [cell.button setImage:[NSImage imageNamed:@"unCheckBtn"]];
        }
        
    }];
}

#pragma mark---button--
-(IBAction)buttonPress:(id)sender
{
    if (_isLoading) {
        return;
    }
    
    if (_currentSelecctIndex == -1) {
        NSAlert *alert = [NSAlert alertWithMessageText:@"提示" defaultButton:@"确定" alternateButton:nil otherButton:nil informativeTextWithFormat:@"请选择一个体验账号!"];
        NSUInteger action = [alert runModal];
        NSLog(@"alert select index :%ld",action);
        return;
    }
    _isLoading = YES;
    NSDictionary *uidDic = nil;
    if (_segmentIndex == 0) {
        uidDic = [self.SRArrays objectAtIndex:_currentSelecctIndex];
    } else {
        uidDic = [self.CanArrays objectAtIndex:_currentSelecctIndex];
    }
    self.loadText.hidden = NO;
    self.progessIndicator.hidden = NO;
    NSString *uid = [uidDic objectForKey:@"client_number"];
    NSString *pwd = [uidDic objectForKey:@"client_pwd"];
    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
    NSString *main_account = [defaults objectForKey:kAccountSid];
    NSString *main_token = [defaults objectForKey:kAccountToken];
    
    __weak __typeof(self)weakSelf = self;
    [[KCTTcpClient sharedTcpClientManager] connect:main_account accountToken:main_token clientNumber:uid clientPwd:pwd success:^(NSString *userId) {
        
        BOOL isIpv6 = [[KCTTcpClient sharedTcpClientManager] isSocketIPV6];
        if (isIpv6)
        {
            //[[KCTFuncEngine getInstance] setSocketIpv6:isIpv6];
        }
        
        dispatch_async(dispatch_get_main_queue(), ^{
            weakSelf.progessIndicator.hidden = YES;
            weakSelf.loadText.stringValue = @"登录成功!";
            weakSelf.loadText.hidden = YES;
            _isLoading = NO;
            
            NSMutableArray *temp;
            if (_segmentIndex == 0) {
                temp = [NSMutableArray arrayWithArray:self.SRArrays];
            } else {
                temp = [NSMutableArray arrayWithArray:self.CanArrays];
            }
            [temp removeObjectAtIndex:_currentSelecctIndex];
            if (!self.phoneWin) {
                _phoneWin = [[CalledListWindowController alloc] initWithWindowNibName:@"CalledListWindowController"];
                _phoneWin.delegate = self;
            }
            
            _phoneWin.sourceArray = [NSArray arrayWithArray:temp];
            [_phoneWin.window center];
            [_phoneWin.window orderFront:nil];
            [weakSelf.window orderOut:nil];
        });
        
    } failure:^(KCTError *error) {
        weakSelf.progessIndicator.hidden = NO;
        weakSelf.loadText.stringValue = @"登录失败!";
    }];
}

- (IBAction)changeValue:(id)sender
{
    NSSegmentedControl *contrl = (NSSegmentedControl *)sender;
    _segmentIndex = contrl.integerValue;
    _currentSelecctIndex = -1;
    [self.tableView reloadData];
    
}


#pragma mark---delegate
- (void)windowBack
{
    self.phoneWin = nil;
}

- (void)showSelfWindow
{
    [self showWindow:NSApp];
}


@end
