//
//  UXinHttpUploadFileManager.h
//  UXinClient
//
//  Created by Liam on 12-10-11.
//  Copyright (c) 2012年 UXIN. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "IMRecordItem.h"

// UXinHttpUploadFileManagerDelegate
@protocol UXinHttpUploadFileManagerDelegate <NSObject>
@optional
// 网络请求成功，收到返回
// nResult      : 返回值
// identifier   : 唯一标识
- (void) onUXinHttpUploadFileManagerDidFinishLoading:(NSDictionary *) nResult withIdentifier:(id) identifier;
// 网络请求出现错误
// error        : 错误信息
// identifier   : 唯一标识
- (void) onUXinHttpUploadFileManagerDidFailWithError:(NSError *) error withIdentifier:(id) identifier;
// 进度回调
// newProgress : 进度值
// identifier  : 唯一标识
- (void) onUXinHttpUploadFileManagerSetProgress:(float) newProgress withIdentifier:(id) identifier;
@end


@interface UXinHttpUploadFileManager : NSObject
{
    id                      m_delegate;
    
    NSMutableArray          *m_arrayASIFormDataRequest;     // 请求列表
    NSMutableArray          *m_arrayIdentifier;             // 请求唯一标识
}


- (id) init;

// 设置相关参数，并且发起请求
- (void) startWithURL:(NSString *) strURL withDelegate:(id) delegate withIdentifier:(IMRecordItem *) identifier withFile:(NSArray *) arrayFileName forKey:(NSString *) key;

@end
