//
//  UxinUtility.m
//  UXinClient
//
//  Created by Liam on 11-7-22.
//  Copyright 2011 UXIN. All rights reserved.
//

#import "Utility.h"
#import <CommonCrypto/CommonCryptor.h>
#import <CommonCrypto/CommonDigest.h>


@implementation UxinUtility

static UxinUtility *instance = NULL;

+(UxinUtility*)shareUtility{
    
    if (instance) {
        return instance;
    }else {
        instance=[[self alloc] init];
        return  instance;
    }
    
    
}

- (NSString *) md5String:(NSString *)str {
    const char *cStr = [str UTF8String];
    unsigned char result[16];
    CC_MD5( cStr, strlen(cStr), result );
    return [[NSString stringWithFormat:
             @"%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X",
             result[0], result[1], result[2], result[3], 
             result[4], result[5], result[6], result[7],
             result[8], result[9], result[10], result[11],
             result[12], result[13], result[14], result[15]
             ] lowercaseString];
}
- (NSString *) md5Data:(NSData *)data 
{
    unsigned char result[16];
    CC_MD5(data.bytes, data.length, result ); // This is the md5 call
    return [[NSString stringWithFormat:
             @"%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",
             result[0], result[1], result[2], result[3], 
             result[4], result[5], result[6], result[7],
             result[8], result[9], result[10], result[11],
             result[12], result[13], result[14], result[15]
             ] lowercaseString];  
}
//sha
-(NSString *) getShaString:(NSString *)source{
    assert(source != nil);
    const char * cstr = [source cStringUsingEncoding:NSUTF8StringEncoding];
    NSData * data = [NSData dataWithBytes:cstr length:[source length]];
    uint8_t digest[CC_SHA1_DIGEST_LENGTH]={0};
    CC_SHA1([data bytes], [data length], digest);
    NSMutableString * output = [NSMutableString stringWithCapacity:CC_SHA1_DIGEST_LENGTH*2];
    for (int i=0; i<CC_SHA1_DIGEST_LENGTH; ++i) {
        [output appendFormat:@"%02X",digest[i]];
    }
    return output;
}


-(void) dealloc{
    if (instance) {
        [instance release];
        instance  = nil;
    }
    [super dealloc];
}

@end

