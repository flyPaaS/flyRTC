//
//  UCSRecordRequestManager.h
//  ucsvideosdk
//
//  Created by KCT   on 16/1/8.
//  Copyright © 2016年 ucs. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface UCSRecordRequestManager : NSObject

@property (retain,nonatomic)NSString * callId;
@property (retain,nonatomic)NSString * caller; //主叫号码
@property (retain,nonatomic)NSString * callee; //被叫号码
@property (retain,nonatomic)NSString * callMode;//呼叫模式
@property (retain,nonatomic)NSString * fileName; //文件地址

- (void)uploadRecordRequest;

- (void)stopRcordRequest;


@end
