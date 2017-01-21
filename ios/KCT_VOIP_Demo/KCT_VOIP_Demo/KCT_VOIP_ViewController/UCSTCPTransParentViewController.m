//
//  UCSTCPTransParentViewController.m
//  UCS_IM_Demo
//
//  Created by 云之讯 on 16/4/19.
//  Copyright © 2016年 Barry. All rights reserved.
//

#import "UCSTCPTransParentViewController.h"
//#import "MessageModel.h"
#import "UCChatViewCell.h"
#import "MessageModelManager.h"

@interface UCSTCPTransParentViewController ()<UITableViewDataSource,UITableViewDelegate>


@property (strong,nonatomic) UITextField * numberField; // 接收者输入框，WLS，2016-04-19
@property (strong,nonatomic) UITextField * messageField; // 内容输入框，WLS，2016-04-19


@property (strong,nonatomic) UITableView * contentTableView; // 接收到透传数据的列表视图，WLS，2016-04-19
@property (strong,nonatomic) NSMutableArray * contentArray; // 接收到透传数据的数组，WLS，2016-04-19


@end

@implementation UCSTCPTransParentViewController

- (void)dealloc{
    
    [[NSNotificationCenter defaultCenter] removeObserver:self name:UCSNotiTCPTransParent object:nil];
    
}

- (void)makeContentView{
    


    /**
     @author WLS, 16-04-19 16:04:30

     接收者输入框
     */
    self.numberField = [[UITextField alloc] initWithFrame:CGRectMake(10, 5, CurrentWidth-20, 30)];
    self.numberField.font = [UIFont systemFontOfSize:14];
    self.numberField.placeholder = @"请输入接收人账号";
    self.numberField.text = @"";
    self.numberField.clearButtonMode = UITextFieldViewModeAlways;
    self.numberField.layer.borderColor = [UIColor blackColor].CGColor;
    self.numberField.layer.borderWidth = 1;
    [self.view addSubview:self.numberField];
    
    
    /**
     @author WLS, 16-04-19 16:04:30
     
     内容输入框
     */
    self.messageField = [[UITextField alloc] initWithFrame:CGRectMake(10, 5+GetViewHeight(self.numberField)+GetViewY(self.numberField), CurrentWidth-20, 30)];
    self.messageField.font = [UIFont systemFontOfSize:14];
    self.messageField.placeholder = @"请输入透传内容";
    self.messageField.text = @"";
    self.messageField.clearButtonMode = UITextFieldViewModeAlways;
    self.messageField.layer.borderColor = [UIColor blackColor].CGColor;
    self.messageField.layer.borderWidth = 1;
    [self.view addSubview:self.messageField];

    

    /**
     @author WLS, 16-04-19 19:04:44
     
     发送按钮
     */
    UIButton * sendButton = [UIButton buttonWithType:UIButtonTypeCustom];
    sendButton = [UIButton buttonWithType:UIButtonTypeCustom];
    sendButton.frame = CGRectMake(10, GetViewY(self.messageField)+GetViewHeight(self.messageField)+5, 60, 30);
    [sendButton setTitle:@"立即发送" forState:UIControlStateNormal];
    sendButton.titleLabel.font = [UIFont systemFontOfSize:14];
    [sendButton setTitleColor:[UIColor whiteColor] forState:UIControlStateNormal];
    [sendButton setTitleColor:[UIColor grayColor] forState:UIControlStateHighlighted];
    sendButton.backgroundColor = RGBColor(65, 180, 139);
    sendButton.layer.cornerRadius = 2;
    [sendButton addTarget:self action:@selector(sendButtonClick:) forControlEvents:UIControlEventTouchUpInside];
    [self.view addSubview:sendButton];
    
    
    self.contentTableView = [[UITableView alloc] initWithFrame:CGRectMake(10, GetViewY(sendButton)+GetViewHeight(sendButton)+5, CurrentWidth-20, CurrentHeight - 70 - (GetViewY(sendButton)+GetViewHeight(sendButton)+5))];
    self.contentTableView.delegate = self;
    self.contentTableView.dataSource = self;
    self.contentTableView.layer.borderWidth = 1;
    self.contentTableView.layer.borderColor = [UIColor blackColor].CGColor;
    self.contentTableView.separatorStyle = UITableViewCellSeparatorStyleNone;
    self.contentTableView.backgroundColor = [UIColor clearColor];
    [self.view addSubview:self.contentTableView];
    
    //收键盘手势
    UITapGestureRecognizer * tapGesture = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(tapGestureClick)];
    [self.contentTableView addGestureRecognizer:tapGesture];

}

//收键盘
- (void)tapGestureClick{
    [self.view endEditing:NO];
}



- (void)touchesBegan:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event{
    [self.view endEditing:YES];
}

- (void)viewDidLoad {
    [super viewDidLoad];
    
    self.title = @"tcp透传测试";
    
    
    self.contentArray = [[NSMutableArray alloc] init];
    
    [self makeContentView];
    
    
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(receiveTCPTransParentInfo:) name:UCSNotiTCPTransParent object:nil];
    
//    MessageModel *model = [MessageModelManager modelWithMessage:message];

    
    // Do any additional setup after loading the view.
}



#pragma mark - uitableviewdelegate

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section{
    
    return self.contentArray.count;
}

- (CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath{
    
    NSObject *obj = [self.contentArray objectAtIndex:indexPath.row];
    if ([obj isKindOfClass:[NSString class]]) {
        return 40;
    }
    else{
        MessageModel * model = (MessageModel*) obj;
        //        if (model.type == MessageBodyType_notification) {
        //            return [UCChatNotificationCell tableView:tableView heightForRowAtIndexPath:indexPath withObject:(MessageModel *)model];
        //        }else{
        //          return [UCChatViewCell tableView:tableView heightForRowAtIndexPath:indexPath withObject:(MessageModel *)obj];
        //        }
        return model.modelHeight;
    }

    
}


- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath{
    
    if (indexPath.row < [self.contentArray count]) {
        
        id obj = [self.contentArray objectAtIndex:indexPath.row];

        MessageModel *model = (MessageModel *)obj;
        
    
        UCChatViewCell *cell = (UCChatViewCell *)[tableView dequeueReusableCellWithIdentifier:model.messageIdentifier];
        if (cell == nil) {
            cell = [[UCChatViewCell alloc] initWithMessageModel:model reuseIdentifier:model.messageIdentifier];
            cell.backgroundColor = [UIColor clearColor];
            cell.selectionStyle = UITableViewCellSelectionStyleNone;
        }
        cell.messageModel = model;
        return cell;
    }
    
    return nil;
}


#pragma mark - 收到透传数据之后

- (void)receiveTCPTransParentInfo:(NSNotification *) note {
    
    UCSTCPTransParent * msgEntity = (UCSTCPTransParent *)note.object;

    
    UCSTextMsg * msg = [UCSTextMsg initWithContent:msgEntity.cmdString];
    
    
    UCSMessage * message = [[UCSMessage alloc] init];
    message.conversationType = UCS_IM_DISCUSSIONCHAT;
    message.messageType = UCS_IM_TEXT;
    message.receiveId = @"";
    message.messageId = msgEntity.requestIdentifier.integerValue;
    message.messageDirection = MessageDirection_RECEIVE;
    message.senderUserId = msgEntity.senderUserId;
    message.senderNickName = msgEntity.senderUserId;
    message.receivedStatus = ReceivedStatus_READ;
    message.sentStatus = SendStatus_success;
    message.time = 0;
    message.objectName = @"";
    message.extra = @"";
    message.content = msg;
    
    MessageModel *model = [MessageModelManager modelWithMessage:message];
    
    [self.contentArray addObject:model];
    
    [self.contentTableView reloadData];

}


/**
 @author WLS, 16-04-19 19:04:52
 
 透传数据发送
 */
- (void)sendButtonClick:(UIButton *)button{
    
    [self.view endEditing:YES];
    
    if ([self.messageField.text isEqualToString:@""] ||
        [self.numberField.text isEqualToString:@""]) {
        
        
        [self showAlertView:@"内容或者接收人账号不能为空"];
        return;
    }
    
    if (self.messageField.text.length>256) {
        [self showAlertView:@"字数太长"];
        return;
    }
    
    UCSTCPTransParentRequest *request = [UCSTCPTransParentRequest initWithCmdString:self.messageField.text receiveId:self.numberField.text];
    
    
    [[UCSTcpClient sharedTcpClientManager] sendTransParentData:request success:^(UCSTCPTransParentRequest *request) {
       
        NSLog(@"发送成功");
        
        [self showAlertView:@"发送成功"];
        
    } failure:^(UCSTCPTransParentRequest *request, UCSError *error) {
        
        NSLog(@"发送失败：%@",error.errorDescription);
        
        [self showAlertView:[NSString stringWithFormat:@"发送失败%d：%@",error.code,error.errorDescription]];
    }];
    
    
}

- (void)showAlertView:(NSString *)message{
    
    
    UIAlertView * alertView = [[UIAlertView alloc] initWithTitle:@"提示" message:message delegate:nil cancelButtonTitle:@"返回" otherButtonTitles:nil, nil];
    
    [alertView show];
    
}


- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

/*
#pragma mark - Navigation

// In a storyboard-based application, you will often want to do a little preparation before navigation
- (void)prepareForSegue:(UIStoryboardSegue *)segue sender:(id)sender {
    // Get the new view controller using [segue destinationViewController].
    // Pass the selected object to the new view controller.
}
*/

@end
