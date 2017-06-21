//
//  UCSASIInputStream.h
//  Part of UCSASIHTTPRequest -> http://allseeing-i.com/UCSASIHTTPRequest
//
//  Created by Ben Copsey on 10/08/2009.
//  Copyright 2009 All-Seeing Interactive. All rights reserved.
//

#import <Foundation/Foundation.h>

@class UCSASIHTTPRequest;

// This is a wrapper for NSInputStream that pretends to be an NSInputStream itself
// Subclassing NSInputStream seems to be tricky, and may involve overriding undocumented methods, so we'll cheat instead.
// It is used by UCSASIHTTPRequest whenever we have a request body, and handles measuring and throttling the bandwidth used for uploading

@interface UCSASIInputStream : NSObject {
	NSInputStream *stream;
	UCSASIHTTPRequest *request;
}
+ (id)inputStreamWithFileAtPath:(NSString *)path request:(UCSASIHTTPRequest *)request;
+ (id)inputStreamWithData:(NSData *)data request:(UCSASIHTTPRequest *)request;

@property (retain, nonatomic) NSInputStream *stream;
@property (assign, nonatomic) UCSASIHTTPRequest *request;
@end
