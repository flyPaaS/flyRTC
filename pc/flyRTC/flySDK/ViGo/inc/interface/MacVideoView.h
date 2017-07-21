//
//  MacVideoView.h
//  vigo-lib
//
//  Created by KCMac on 2017/5/27.
//  Copyright © 2017年 国领电讯. All rights reserved.
//

#if !TARGET_OS_IPHONE

#import <Cocoa/Cocoa.h>

@interface MacVideoView
// 从本地数据库获取用户账号
+ (NSOpenGLView *) allocAndInitWithFrame:(CGRect)frame;
+ (void) enableRender:(BOOL)enable;
@end

#endif
