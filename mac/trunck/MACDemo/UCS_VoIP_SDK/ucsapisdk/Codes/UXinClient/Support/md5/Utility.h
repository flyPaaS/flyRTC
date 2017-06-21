//
//  UxinUtility.h
//  UXinClient
//
//  Created by Liam on 11-7-22.
//  Copyright 2011 UXIN. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface UxinUtility : NSObject {

}

+(UxinUtility*)shareUtility;

- (NSString *) md5String:(NSString *)str ;
- (NSString *) md5Data:(NSData *)data;
- (NSString *) getShaString:(NSString *)source;
@end
