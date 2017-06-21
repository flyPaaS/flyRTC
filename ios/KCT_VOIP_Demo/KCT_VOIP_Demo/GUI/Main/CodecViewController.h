//
//  CodecViewController.h
//  SoftphoneDemo
//
//  Created by KCMac on 2017/5/19.
//
//

#import <UIKit/UIKit.h>

@protocol CodecViewControllerDelegate <NSObject>

- (void)selectVideoCodec:(NSInteger)videoCodecIndex audio:(NSInteger)audioCodecIndex;

@end

@interface CodecViewController : UIViewController

@property(nonatomic,assign)id<CodecViewControllerDelegate>delegate;
@property(assign,nonatomic) NSInteger videoIndex;
@property(assign,nonatomic) NSInteger audioIndex;


@end
