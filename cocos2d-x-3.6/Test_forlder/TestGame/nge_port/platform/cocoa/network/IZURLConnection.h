/*
 Copyright (C) 2009 Zsombor Szabó, IZE Ltd.. All rights reserved.
 
 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:
 
 * Redistributions of source code must retain the above copyright notice, this
 list of conditions and the following disclaimer.
 
 * Redistributions in binary form must reproduce the above copyright notice,
 this list of conditions and the following disclaimer in the documentation
 and/or other materials provided with the distribution.
 
 * Neither the name of the author nor the names of its contributors may be used
 to endorse or promote products derived from this software without specific
 prior written permission.
 
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE
 FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#import "IZReachability.h"

extern NSString *IZURLConnectionConnectionDidStartNotification;
extern NSString *IZURLConnectionConnectionDidFinishNotification;

@interface IZURLConnection : NSObject 
{
	BOOL						loading;
	BOOL						_shouldRetryWhenReachable;
	
	NSInteger					autoRetryAttemptLimit;
	NSInteger					autoRetryAttempt;
	
	NSURLRequest				*request;
	NSURLConnection				*_connection;
	IZReachability				*_hostReachability;
	
	id							delegate;
}

// Is YES if there is actual loading. If no Internet connection is present this will be NO when connection breaks. But set to yes again if it will be retried upon completion.
@property (nonatomic, readonly, assign, getter=isLoading) BOOL loading; 

// In which auto retry cycle is it currently in.
@property (nonatomic, readonly, assign) NSInteger autoRetryAttempt; 

// How many times should it auto retry upon failure. Default 2
@property (nonatomic, assign) NSInteger autoRetryAttemptLimit; 

// Convenience property to be able to check the request anytime.
@property (nonatomic, readonly) NSURLRequest *request; 

// Receives NSURLConnection delegate methods
@property (nonatomic, assign) id delegate;

#pragma mark -
#pragma mark NSURLConnection interface

#pragma mark Preflighting a Request

+ (BOOL)canHandleRequest:(NSURLRequest *)aRequest;

#pragma mark Loading Data Synchronously

+ (NSData *)sendSynchronousRequest:(NSURLRequest *)aRequest returningResponse:(NSURLResponse **)response error:(NSError **)error;

#pragma mark Loading Data Asynchronously

+ (id)connectionWithRequest:(NSURLRequest *)aRequest delegate:(id)aDelegate;

// This calls the designated initializer with startImmediately YES
- (id)initWithRequest:(NSURLRequest *)aRequest delegate:(id)aDelegate;

// Designated initializer
- (id)initWithRequest:(NSURLRequest *)aRequest delegate:(id)aDelegate startImmediately:(BOOL)startImmediately;

// Starts the connection.
// Does nothing if already loading. Cancel first.
// Unlike NSURLConnection you can start a connection multiple times.
- (void)start;

#pragma mark Stopping a Connection

// Cancels the connection. 
// Does nothing if already cancelled.
// Resets autoRetryAttempt to 0
- (void)cancel;

@end
