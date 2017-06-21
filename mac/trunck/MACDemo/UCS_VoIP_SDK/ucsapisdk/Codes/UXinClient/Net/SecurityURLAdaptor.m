//
//  SecurityURLAdaptor.m
//  UXinClient
//
//  Created by FengXing on 13-8-21.
//  Copyright (c) 2013年 UXIN CO. All rights reserved.
//

#import "SecurityURLAdaptor.h"

static SecurityURLAdaptor *s_sharedAdaptor = nil;

@implementation SecurityURLAdaptor
{
    // 全路径过滤
    NSArray *_securityAbsolutePath;
    NSArray *_legacyAbsolutePath;
    
    // PWD计算过滤
    NSArray *_securityPwdPath;
    NSArray *_legacyPwdPath;
}


+ (id)sharedAdaptor
{
	@synchronized(self) {
		if (s_sharedAdaptor == nil) {
			s_sharedAdaptor = [[self alloc] init];
		}
	}
	return s_sharedAdaptor;
}

+ (id)allocWithZone:(NSZone *)zone 
{
    @synchronized(self) {
        if (s_sharedAdaptor == nil) {
            s_sharedAdaptor = [super allocWithZone:zone];
            return s_sharedAdaptor;
        }
        return nil;
    }
}

- (id)init
{
    if (self = [super init]) {
        // 不是所有的接口都需要用签名
        // 在这里填写是一定使用的需要签名的接口，比如原来没有带sign
        _securityAbsolutePath = @[@"/msg/getchannelmsg", @"/action_report"];
        // 在这里填写是一定不使用的需要签名的接口，比如sign的计算规则不同
        _legacyAbsolutePath = @[@"/getnum", @"reportcontrol.action", @"/v2/user/getinfo", @"/v2/report/media_report"];
        
        _securityPwdPath = @[];
        _legacyPwdPath = @[@"/login"];
        [_securityAbsolutePath retain];
        [_legacyAbsolutePath retain];
        [_securityPwdPath retain];
        [_legacyPwdPath retain];
    }
    return self;
}

- (id)copyWithZone:(NSZone *)zone
{
	return self;
}

#if !__has_feature(objc_arc)
- (oneway void)release
{

}

- (id)retain
{
	return self;
}

- (id)autorelease
{
	return self;
}

- (NSUInteger)retainCount
{
	return UINT_MAX;
}
#endif

#pragma mark -
- (NSURL*)convert:(NSURL*)url
{
	NSString *query = [url query];
    NSMutableArray *orderKeys = [[[NSMutableArray alloc] init] autorelease];
	NSMutableDictionary *params = [[[NSMutableDictionary alloc] init] autorelease];
	for (NSString *param in [query componentsSeparatedByString:@"&"]) {
	   NSArray *elts = [param componentsSeparatedByString:@"="];
	   if([elts count] < 2) continue;
	   [params setObject:[elts objectAtIndex:1] forKey:[elts objectAtIndex:0]];
        [orderKeys addObject:[elts objectAtIndex:0]];
	}
    
    // 参数修改
    if ([params objectForKey:@"securityver"]) {
        return url;
    } else {
        [params setObject:@"1" forKey:@"securityver"];
        [orderKeys addObject:@"securityver"];
    }
    
    NSString *strUid = [UserDefaultManager GetUserID];
    
    // 必传参数
    // 注册不需要uid,uid没有也不传
    if (![[url absoluteString] hasSuffix:@"reg?"] && nil != strUid && [strUid isKindOfClass:[NSString class]] && [strUid length] > 0)
    {
        [params setObject:[UserDefaultManager GetUserID] forKey:@"uid"];
        [orderKeys addObject:@"uid"];
    }

    [params setObject:kProductName forKey:@"p"];
    [orderKeys addObject:@"p"];
    [params setObject:kVersion forKey:@"v"];
    [orderKeys addObject:@"v"];
    [params setObject:kPhoneType forKey:@"pv"];
    [orderKeys addObject:@"pv"];
    [params setObject:[PublicFunc GetInviteID] forKey:@"u"];
    [orderKeys addObject:@"u"];
    

    if ([params objectForKey:@"sn"] == nil) {
        [params setObject:[PublicFunc genRandomSn] forKey:@"sn"];
        [orderKeys insertObject:@"sn" atIndex:0];
    }
    
    NSString *sign = [self calcNewSign:params];
	// make new url
	NSMutableString *newQuery = [[[NSMutableString alloc] init] autorelease];
	for (id key in orderKeys) {
        if ([key isEqualToString:@"sign"])
            [newQuery appendString:[NSString stringWithFormat:@"%@=%@&", key, sign, nil]];
        else
            [newQuery appendString:[NSString stringWithFormat:@"%@=%@&", key, params[key], nil]];
	}
    if ([params objectForKey:@"sign"] == nil) {
        [newQuery appendString:[NSString stringWithFormat:@"sign=%@&", sign, nil]];
    }

    [newQuery deleteCharactersInRange:NSMakeRange([newQuery length]-1, 1)];
	NSMutableString *newUrl = [[[NSMutableString alloc] initWithString:[url absoluteString]] autorelease];
	NSRange queryRange = [newUrl rangeOfString:query];
	[newUrl replaceCharactersInRange:queryRange withString:newQuery];
	return [NSURL URLWithString:newUrl];
}

- (NSString *)calcNewSign:(NSDictionary *)params
{
    NSMutableString *chiper = [[[NSMutableString alloc] init] autorelease];
	for (NSString *key in [[params allKeys] sortedArrayUsingSelector:@selector(caseInsensitiveCompare:)]) {
		if ([key isEqualToString:@"sign"]) continue; // rewrite this
        NSAssert([params[key] isKindOfClass:[NSString class]], @"Request Must Be String");
		[chiper appendString:params[key]];
	}
    
	NSString *chiper2 = [chiper stringByReplacingPercentEscapesUsingEncoding:NSUTF8StringEncoding];
	uint8_t sign[100] = {0};
    
    const unsigned char* t = (const uint8_t*)[chiper2 cStringUsingEncoding:NSUTF8StringEncoding];
    Signature(t,strlen((const char* )t), sign);
    return [NSString stringWithCString:(char*)sign encoding:NSUTF8StringEncoding];
}

- (BOOL)isSupport:(NSURL *)url
{
    NSString *abs = [[url pathComponents] componentsJoinedByString:@"/"];
    if ([abs length] == 0) {
        return NO;
    }
    
    if ([_legacyAbsolutePath containsObject:[abs substringFromIndex:1]])
    	return NO;
    
    if ([_securityAbsolutePath containsObject:[abs substringFromIndex:1]])
    	return YES;

    NSString *query = [url query];
    if (query == nil || [query rangeOfString:@"sign="].location == NSNotFound)
	    return NO;
	return YES;
}

- (BOOL)isSupportPwd:(NSURL *)url
{
    NSString *abs = [[url pathComponents] componentsJoinedByString:@"/"];
    if ([abs length] == 0) {
        return NO;
    }
    
    if ([_legacyPwdPath containsObject:[abs substringFromIndex:1]])
    	return NO;
    
    if ([_securityPwdPath containsObject:[abs substringFromIndex:1]])
    	return YES;
    
    NSString *query = [url query];
    if (query == nil || [query rangeOfString:@"pwd="].location == NSNotFound)
	    return NO;
	return YES;
}
@end
