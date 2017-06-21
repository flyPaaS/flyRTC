//
//  UXinHttpUploadFileManager.m
//  UXinClient
//
//  Created by Liam on 12-10-11.
//  Copyright (c) 2012年 UXIN. All rights reserved.
//

#import "UXinHttpUploadFileManager.h"
#import "UXinCustomASIFormDataRequest.h"
#import "Signature/Signature.h"
#import "SecurityURLAdaptor.h"

@implementation UXinHttpUploadFileManager


- (id) init
{
    self = [super init];
    
    if (self)
    {
        m_arrayASIFormDataRequest = nil;
        m_arrayASIFormDataRequest = [[NSMutableArray alloc] init];
        m_arrayIdentifier = nil;
        m_arrayIdentifier = [[NSMutableArray alloc] init];
    }
    
    return self;
}

- (void) dealloc
{
    UXIN_MEM_RELEASE(m_arrayASIFormDataRequest);
    UXIN_MEM_RELEASE(m_arrayIdentifier);
    
    [super dealloc];
}

// 设置相关参数，并且发起请求
- (void) startWithURL:(NSString *) strURL withDelegate:(id) delegate withIdentifier:(IMRecordItem *) identifier withFile:(NSArray *) arrayFileName forKey:(NSString *) key
{
    NSURL *url = [NSURL URLWithString:strURL];
    url = [[SecurityURLAdaptor sharedAdaptor] convert:url];
    
    UXinCustomASIFormDataRequest *pRequest = [[UXinCustomASIFormDataRequest alloc] initWithURL:url];
    [pRequest setDelegate:self];
    [pRequest setProgressDelegrate:self];
    [pRequest setUseCookiePersistence:YES];
    [pRequest setTimeOutSeconds:360.0f];
    [pRequest addRequestHeader:@"Cookie" value:[UserDefaultManager getCookieString]];
    pRequest.identifier = identifier;
    
    [pRequest addRequestHeader:@"ac" value:[UserDefaultManager GetAC]];
    char ch[100] = {0};
    Security((unsigned char*)&ch[0]);
    [pRequest addRequestHeader:@"SecurityFlag" value:[NSString stringWithCString:ch encoding:NSUTF8StringEncoding]];
    
    BOOL bHasFile = NO;
    for (int i = 0; i < [arrayFileName count]; ++i)
    {
        NSString *strFileName = [arrayFileName objectAtIndex:i];
        if (nil != strFileName && [strFileName length] > 0)
        {
            [pRequest addFile:strFileName forKey:key];
            bHasFile = YES;
        }
    }
    
    if (bHasFile)
    {
        @synchronized(self)
        {
            [m_arrayASIFormDataRequest addObject:pRequest];
            if (identifier)
            {
                [m_arrayIdentifier addObject:[identifier retain]];
            }
        }
        m_delegate = delegate;
        NSLog(@"UXinHttpUploadFile url = %@", strURL);
        [pRequest startAsynchronous];
    }
    else
    {
        NSLog(@"No File need be send.");
        [pRequest release];
        if (nil != m_delegate && [m_delegate respondsToSelector:@selector(onUXinHttpUploadFileManagerDidFailWithError:withIdentifier:)])
        {
            [m_delegate onUXinHttpUploadFileManagerDidFailWithError:nil withIdentifier:identifier];
        }
    }
}

#pragma mark-
#pragma mark ASIHTTPRequestDelegate methods
// 请求结束，获取 Response 数据
- (void) requestFinished:(ASIHTTPRequest *) request
{
    NSData *responseData = [request responseData];
    
    if (nil == responseData)
    {
        return;
    }
    
    int nRet = HTTP_REQUEST_SERVER_ERROR;
    
    NSMutableDictionary *returnDic = [NSMutableDictionary dictionary];
    NSString *jsonValue = [[NSString alloc] initWithData:responseData encoding:NSUTF8StringEncoding];
    NSLog(@"UXinHttpUploadFileManager jsonValue = %@", jsonValue);
    [returnDic setDictionary:[jsonValue JSONValue]];
    [jsonValue release];
    
    if ([returnDic count] > 0)
    {
        for (int i = 0; i < [m_arrayASIFormDataRequest count]; ++i)
        {
            if (request == [m_arrayASIFormDataRequest objectAtIndex:i])
            {
                id idRet = [returnDic objectForKey:@"result"];
                if (nil == idRet)
                {
                    if (nil != m_delegate && [m_delegate respondsToSelector:@selector(onUXinHttpUploadFileManagerDidFailWithError:withIdentifier:)])
                    {
                        id tempIdentifier = nil;
                        if (i < [m_arrayIdentifier count])
                        {
                            tempIdentifier = [m_arrayIdentifier objectAtIndex:i];
                        }
                        [m_delegate onUXinHttpUploadFileManagerDidFailWithError:nil withIdentifier:tempIdentifier];
                        
                        @synchronized(self)
                        {
                            [[m_arrayASIFormDataRequest objectAtIndex:i] release];
                            [m_arrayASIFormDataRequest removeObjectAtIndex:i];
                            if (i < [m_arrayIdentifier count])
                            {
                                [[m_arrayIdentifier objectAtIndex:i] release];
                                [m_arrayIdentifier removeObjectAtIndex:i];
                            }
                        }
                    }
                    break;
                }
                else
                {
                    nRet = [[returnDic objectForKey:@"result"] intValue];
                    if (nil != m_delegate && [m_delegate respondsToSelector:@selector(onUXinHttpUploadFileManagerDidFinishLoading:withIdentifier:)])
                    {
                        id tempIdentifier = nil;
                        if (i < [m_arrayIdentifier count])
                        {
                            tempIdentifier = [m_arrayIdentifier objectAtIndex:i];
                        }
                        [m_delegate onUXinHttpUploadFileManagerDidFinishLoading:returnDic withIdentifier:tempIdentifier];
                        
                        @synchronized(self)
                        {
                            [[m_arrayASIFormDataRequest objectAtIndex:i] release];
                            [m_arrayASIFormDataRequest removeObjectAtIndex:i];
                            if (i < [m_arrayIdentifier count])
                            {
                                [[m_arrayIdentifier objectAtIndex:i] release];
                                [m_arrayIdentifier removeObjectAtIndex:i];
                            }
                        }
                    }
                    break;
                }
            }
        }
    }
    else
    {
        for (int i = 0; i < [m_arrayASIFormDataRequest count]; ++i)
        {
            if (request == [m_arrayASIFormDataRequest objectAtIndex:i])
            {
                if (nil != m_delegate && [m_delegate respondsToSelector:@selector(onUXinHttpUploadFileManagerDidFailWithError:withIdentifier:)])
                {
                    id tempIdentifier = nil;
                    if (i < [m_arrayIdentifier count])
                    {
                        tempIdentifier = [m_arrayIdentifier objectAtIndex:i];
                    }
                    [m_delegate onUXinHttpUploadFileManagerDidFailWithError:nil withIdentifier:tempIdentifier];
                    
                    @synchronized(self)
                    {
                        [[m_arrayASIFormDataRequest objectAtIndex:i] release];
                        [m_arrayASIFormDataRequest removeObjectAtIndex:i];
                        if (i < [m_arrayIdentifier count])
                        {
                            [[m_arrayIdentifier objectAtIndex:i] release];
                            [m_arrayIdentifier removeObjectAtIndex:i];
                        }
                    }
                }
                break;
            }
        }
    }
}

// 请求失败，获取 error
- (void) requestFailed:(ASIHTTPRequest *) request
{
    NSError *error = [request error];
    
    NSLog(@"%@", [error description]);
    
    for (int i = 0; i < [m_arrayASIFormDataRequest count]; ++i)
    {
        if (request == [m_arrayASIFormDataRequest objectAtIndex:i])
        {
            if (nil != m_delegate && [m_delegate respondsToSelector:@selector(onUXinHttpUploadFileManagerDidFailWithError:withIdentifier:)])
            {
                [m_delegate onUXinHttpUploadFileManagerDidFailWithError:error withIdentifier:[m_arrayIdentifier objectAtIndex:i]];

                @synchronized(self)
                {
                    [[m_arrayASIFormDataRequest objectAtIndex:i] release];
                    [m_arrayASIFormDataRequest removeObjectAtIndex:i];
                    [[m_arrayIdentifier objectAtIndex:i] release];
                    [m_arrayIdentifier removeObjectAtIndex:i];
                }
            }
            break;
        }
    }
}

#pragma mark-
#pragma mark UXinCustomASIFormDataRequestDelegate methods
// 进度回调
// request      : self
// newProgress  : 进度值
// identifier   : 唯一标识
- (void) onSetProgress:(ASIHTTPRequest *)request withProgress:(float) newProgress withIdentifier:(id) identifier
{
    if (nil == identifier)
    {
        return;
    }
    
    id tempIdentifier = [identifier retain];
    if (nil != m_delegate && [m_delegate respondsToSelector:@selector(onUXinHttpUploadFileManagerSetProgress:withIdentifier:)])
    {
        [m_delegate onUXinHttpUploadFileManagerSetProgress:newProgress withIdentifier:tempIdentifier];
    }
    [tempIdentifier release];
}

@end
