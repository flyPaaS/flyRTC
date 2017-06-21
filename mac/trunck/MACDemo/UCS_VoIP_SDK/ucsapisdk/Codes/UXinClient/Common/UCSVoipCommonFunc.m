//
//  UCSVoipCommonFunc.m
//  ucsvideosdk
//
//  Created by KCT   on 16/5/13.
//  Copyright © 2016年 ucs. All rights reserved.
//

#import "UCSVoipCommonFunc.h"


#ifndef _EXECINFO_H_
#define _EXECINFO_H_ 1
#include <sys/cdefs.h>
#include <Availability.h>
__BEGIN_DECLS int backtrace(void**,int)
__OSX_AVAILABLE_STARTING(__MAC_10_5, __IPHONE_2_0); char** backtrace_symbols(void* const*,int)
__OSX_AVAILABLE_STARTING(__MAC_10_5, __IPHONE_2_0); void backtrace_symbols_fd(void* const*,int,int)
__OSX_AVAILABLE_STARTING(__MAC_10_5, __IPHONE_2_0);
__END_DECLS
#endif

#define MAX_CALLSTACK_DEPTH	(64)

@implementation UCSVoipCommonFunc

+ (NSArray *)ucsVoipGetcallstack:(NSUInteger)depth{
    NSMutableArray * array = [[[NSMutableArray alloc] init] autorelease] ;
    void * stacks[MAX_CALLSTACK_DEPTH] = { 0 };
    depth = backtrace( stacks, (depth > MAX_CALLSTACK_DEPTH) ? MAX_CALLSTACK_DEPTH : depth );
    if ( depth > 1 ) {
        char ** symbols = backtrace_symbols( stacks, depth );
        if ( symbols ) {
            for ( int i = 0; i < (depth - 1); ++i ) {
                NSString * symbol = [NSString stringWithUTF8String:(const char *)symbols[1 + i]];
                
                if ( 0 == [symbol length] ){
                    
                    continue;
                }
                
                [array addObject:symbol];
                
                //                NSRange range1 = [symbol rangeOfString:@"["];
                //                NSRange range2 = [symbol rangeOfString:@"]"];
                //
                //
                //                if ( range1.length > 0 && range2.length > 0 ) {
                //                    NSRange range3;
                //                    range3.location = range1.location;
                //                    range3.length = range2.location + range2.length - range1.location;
                //                    [array addObject:[symbol substringWithRange:range3]];
                //
                //                } else {
                //                    [array addObject:symbol];
                //                }
                
            }
            free( symbols ); 
        } 
    } 
    return array;
}


@end
