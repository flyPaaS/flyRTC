//
//  LoginWindowController.m
//  KCT_VOIP_Demo
//
//  Created by KCMac on 2017/5/22.
//  Copyright © 2017年 flypass. All rights reserved.
//

#import "LoginWindowController.h"

@interface LoginWindowController ()
{
    BOOL _isLogining;
}
@property(nonatomic,strong)IBOutlet NSButton *loginBtn;
@property(nonatomic,strong)IBOutlet NSTextField *useText;
@property(nonatomic,strong)IBOutlet NSTextField *passText;
@property(nonatomic,strong)IBOutlet NSProgressIndicator *progressIndicatior;
@property(nonatomic,strong)IBOutlet NSTextField *textView;
@property(nonatomic,strong)CallingListWindowController *callingWin;

@end

@implementation LoginWindowController

- (void)windowDidLoad {
    [super windowDidLoad];
    
    self.window.title = @"登录";
    self.progressIndicatior.hidden = YES;
    self.textView.hidden = YES;
    self.textView.stringValue = @"正在登录...";
    
    _isLogining = NO;
    _callingWin = [[CallingListWindowController alloc] initWithWindowNibName:@"CallingListWindowController"];
    // Implement this method to handle any initialization after your window controller's window has been loaded from its nib file.
}


- (IBAction)loginPeress:(id)sender
{
    if (_isLogining)
    {
        return;
    }
    self.progressIndicatior.hidden = NO;
    self.textView.hidden = NO;
    [self.progressIndicatior startAnimation:nil];
    
    NSString *useId = self.useText.stringValue;
    NSString *pass = self.passText.stringValue;
    if ([useId isEqualToString:@""] || [pass isEqualToString:@""]) {
        NSAlert *alert = [NSAlert alertWithMessageText:@"提示" defaultButton:@"确定" alternateButton:nil otherButton:nil informativeTextWithFormat:@"请输入用户名和密码!"];
        NSUInteger action = [alert runModal];
        NSLog(@"---action :%d",action);
        return;
    }
    
    __weak __typeof(self)weakSelf = self;
    __block NSArray *SRArrays = nil;
    __block NSArray *canArrays = nil;
    NSMutableDictionary *dic = [[NSMutableDictionary alloc] init];
    
    dispatch_group_t group = dispatch_group_create();
    dispatch_queue_t queue = dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0);
    dispatch_group_async(group, queue, ^{
        __block BOOL isRun = YES;
        [[HttpRequestEngine engineInstance] newLogin:useId token:pass successBlock:^(NSDictionary *responseDict) {
            isRun = NO;
            NSDictionary *newRespDic = [responseDict objectForKey:@"resp"];
            NSNumber *nRespCode = [newRespDic objectForKey:@"respCode"];
            if ([nRespCode intValue] == 0 && nRespCode != nil) {
                NSArray *newArray = [newRespDic objectForKey:@"client"];
                canArrays = newArray;
            }
            
        } failBlock:nil];
        while (isRun) {
            [[NSRunLoop currentRunLoop] runMode:NSDefaultRunLoopMode beforeDate:[NSDate distantFuture]];
        }
    });
        
    dispatch_group_async(group, queue, ^{
        __block BOOL isRun = YES;
        [[HttpRequestEngine engineInstance] login:useId token:pass successBlock:^(NSDictionary *responseDict) {
            isRun = NO;
            NSDictionary *respDic = [responseDict objectForKey:@"resp"];
            NSNumber *nRespCode = [respDic objectForKey:@"respCode"];
            if ([nRespCode intValue] == 0 && nRespCode != nil) {
                NSArray *array = [respDic objectForKey:@"client"];
                SRArrays = array;
                NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
                [defaults setObject:[respDic objectForKey:@"sid"] forKey:kAccountSid];
                [defaults setObject:[respDic objectForKey:@"token"] forKey:kAccountToken];
                [defaults synchronize];
            }
        } failBlock:nil];
        while (isRun) {
            [[NSRunLoop currentRunLoop] runMode:NSDefaultRunLoopMode beforeDate:[NSDate distantFuture]];
        }
    });
    
    dispatch_group_notify(group, queue, ^{
        
        dispatch_async(dispatch_get_main_queue(), ^{
            _isLogining = NO;
            weakSelf.progressIndicatior.hidden = YES;
            weakSelf.textView.hidden = YES;
            [weakSelf.progressIndicatior stopAnimation:nil];
            weakSelf.callingWin.SRArrays = SRArrays;
            weakSelf.callingWin.CanArrays = canArrays;
            [weakSelf.callingWin.window center];
            [weakSelf.callingWin.window orderFront:nil];
            
            [weakSelf.window orderOut:nil];
        });
    });
}

#pragma mark---window delegate
- (BOOL)windowShouldZoom:(NSWindow *)window toFrame:(NSRect)newFrame
{
    return NO;
}


@end
