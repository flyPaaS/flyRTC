//
//  MacVideoView.m
//  vigo-lib
//
//  Created by KCMac on 2017/5/27.
//  Copyright © 2017年 国领电讯. All rights reserved.
//

#if !TARGET_OS_IPHONE

#import "MacVideoView.h"
#include "webrtc/modules/video_render/mac/cocoa_render_view.h"


@implementation MacVideoView

+ (NSOpenGLView *) allocAndInitWithFrame:(CGRect)frame
{
    CocoaRenderView *view = nil;
    view = [[CocoaRenderView alloc] initWithFrame:frame];
    return view;
}


+ (void) enableRender:(BOOL)enable
{
    
}


@end

#endif
