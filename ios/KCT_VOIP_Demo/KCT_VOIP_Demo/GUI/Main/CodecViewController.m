//
//  CodecViewController.m
//  SoftphoneDemo
//
//  Created by KCMac on 2017/5/19.
//
//

#import "CodecViewController.h"

@interface CodecViewController ()<UITableViewDataSource,UITableViewDelegate>
{
}
@property(strong,nonatomic) IBOutlet UITableView *tableView;
@property(strong,nonatomic) NSArray *audioCodecArray;
@property(strong,nonatomic) NSArray *videoCodecArray;
@property(strong,nonatomic) NSMutableArray *videoCellArray;
@property(strong,nonatomic) NSMutableArray *audioCellArray;


@end


#define kCellIdentifier @"codecCell"


@implementation CodecViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    self.title = @"Codec Setting";
    self.videoCellArray = [NSMutableArray new];
    self.audioCellArray = [NSMutableArray new];
    
    
    //self.audioCodecArray = @[@"OPUS",@"AMR",@"SILKWB",@"SILK",@"G729",@"OPUSNB",@"ILBC",@"AAC"];
    self.audioCodecArray = @[@"OPUS",@"AAC"];
    self.videoCodecArray = @[@"h264-sw",@"h264-hw",@"h265-sw",@"h265-hw"];
    self.tableView.contentInset = UIEdgeInsetsMake(-35, 0, 0, 0);
    
    UIColor *color = RGB(17, 121, 255);
    UIButton *rightButton = [UIButton buttonWithType:UIButtonTypeCustom];
    rightButton.frame = CGRectMake(0, 0, 80, 30);
    [rightButton setTitleColor:color forState:UIControlStateNormal];
    [rightButton setTitleColor:[UIColor blueColor] forState:UIControlStateHighlighted];
    [rightButton setTitle:@"设置" forState:UIControlStateNormal];
    [rightButton addTarget:self action:@selector(saveSelect:) forControlEvents:(UIControlEventTouchUpInside)];
    UIBarButtonItem *rItem = [[UIBarButtonItem alloc] initWithCustomView:rightButton];
    self.navigationItem.rightBarButtonItem = rItem;
    
    //self.tableView.backgroundColor = [UIColor redColor];
    [self.tableView registerClass:[UITableViewCell class] forCellReuseIdentifier:@"codec"];
    // Do any additional setup after loading the view from its nib.
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView
{
    return 2;
}

- (nullable NSString *)tableView:(UITableView *)tableView titleForHeaderInSection:(NSInteger)section
{
    if (section == 0)
    {
        return @"Video Codec";
    }
    return @"Audio Codec";
}

//- (NSString *)tableView:(UITableView *)tableView titleForFooterInSection:(NSInteger)section
//{
//    if (section == 0)
//    {
//        return @"选择视频的编码格式";
//    }
//    return @"选择音频的编码格式";
//}

- (UIView *)tableView:(UITableView *)tableView viewForFooterInSection:(NSInteger)section
{
    UILabel *label = [[UILabel alloc] initWithFrame:CGRectMake(0, 0, 320, 24)];
    label.text = @"";
    return label;
}


- (CGFloat)tableView:(UITableView *)tableView heightForFooterInSection:(NSInteger)section
{
    return 45;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {
    if (section == 0)
    {
        return self.videoCodecArray.count;
    }
    
    return self.audioCodecArray.count;
}

- (void)tableView:(UITableView *)aTableView didSelectRowAtIndexPath:(NSIndexPath *)anIndexPath
{
    if (anIndexPath.section == 0) {
        _videoIndex = anIndexPath.row;
        for (int i = 0; i<self.videoCellArray.count; i++) {
            UITableViewCell *cell = (UITableViewCell *)[self.videoCellArray objectAtIndex:i];
            cell.accessoryType = UITableViewCellAccessoryNone;
        }
        UITableViewCell *cell = [aTableView cellForRowAtIndexPath:anIndexPath];
         cell.accessoryType = UITableViewCellAccessoryCheckmark;
    }
    else {
        _audioIndex = anIndexPath.row;
        for (int i = 0; i<self.audioCellArray.count; i++) {
            UITableViewCell *cell = (UITableViewCell *)[self.audioCellArray objectAtIndex:i];
            cell.accessoryType = UITableViewCellAccessoryNone;
        }
        UITableViewCell *cell = [aTableView cellForRowAtIndexPath:anIndexPath];
        cell.accessoryType = UITableViewCellAccessoryCheckmark;
    }
}

- (UITableViewCell *)tableView:(UITableView *)aTableView cellForRowAtIndexPath:(NSIndexPath *)anIndexPath
{

    UITableViewCell *cell = [aTableView dequeueReusableCellWithIdentifier:@"codec" forIndexPath:anIndexPath];
    
    if (anIndexPath.section == 0)
    {
        NSString *title = [self.videoCodecArray objectAtIndex:anIndexPath.row];
        cell.textLabel.text = [NSString stringWithFormat:@"   %@",title];
        if (anIndexPath.row == _videoIndex) {
            cell.accessoryType = UITableViewCellAccessoryCheckmark;
        } else
        {
            cell.accessoryType = UITableViewCellAccessoryNone;
        }
        if (![self.videoCellArray containsObject:cell])
        {
            [self.videoCellArray addObject:cell];
        }
    }
    else
    {
        NSString *title = [self.audioCodecArray objectAtIndex:anIndexPath.row];
        cell.textLabel.text = [NSString stringWithFormat:@"   %@",title];
        if (anIndexPath.row == _audioIndex) {
            cell.accessoryType = UITableViewCellAccessoryCheckmark;
        } else
        {
            cell.accessoryType = UITableViewCellAccessoryNone;
        }
        if (![self.audioCellArray containsObject:cell])
        {
            [self.audioCellArray addObject:cell];
        }
    }
    
    return cell;
}

- (IBAction)saveSelect:(id)sender
{
    if (self.delegate) {
        [self.delegate selectVideoCodec:_videoIndex audio:_audioIndex];
    }
    [self.navigationController popViewControllerAnimated:YES];
}

- (IBAction)close:(id)sender
{
    [self.navigationController popViewControllerAnimated:YES];
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
