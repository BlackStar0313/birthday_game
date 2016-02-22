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

#import "IZURLConnection.h"

NSString *IZURLConnectionConnectionDidStartNotification = @"IZURLConnectionConnectionDidStartNotification";
NSString *IZURLConnectionConnectionDidFinishNotification = @"IZURLConnectionConnectionDidFinishNotification";

#define kDefaultAutoRetryAttemptLimit 3

@implementation IZURLConnection

@synthesize delegate;
@synthesize loading;
@synthesize autoRetryAttempt;
@synthesize autoRetryAttemptLimit;
@synthesize request;

#pragma mark -
#pragma mark Setters

- (void)setLoading:(BOOL)mode
{
	if (self.loading != mode)
	{
		[self willChangeValueForKey:@"loading"];
		loading = mode;
		[self didChangeValueForKey:@"loading"];
	}
}

- (void)setRequest:(NSURLRequest *)aRequest
{
	if (self.request != aRequest)
	{
		[self willChangeValueForKey:@"request"];
		[request release];
		request = [aRequest retain];
		[self didChangeValueForKey:@"request"];
	}
}

- (void)setAutoRetryAttempt:(NSInteger)count
{
	if (self.autoRetryAttempt != count)
	{
		[self willChangeValueForKey:@"autoRetryAttempt"];
		autoRetryAttempt = count;
		[self didChangeValueForKey:@"autoRetryAttempt"];		
	}
}

#pragma mark -
#pragma mark NSURLConnection interface

#pragma mark Preflighting a Request

+ (BOOL)canHandleRequest:(NSURLRequest *)aRequest
{
	return [NSURLConnection canHandleRequest:aRequest];
}

#pragma mark Loading Data Synchronously

+ (NSData *)sendSynchronousRequest:(NSURLRequest *)aRequest returningResponse:(NSURLResponse **)response error:(NSError **)error
{
	[[NSNotificationCenter defaultCenter] performSelectorOnMainThread:@selector(postNotification:) 
														   withObject:[NSNotification notificationWithName:IZURLConnectionConnectionDidStartNotification object:self] 
														waitUntilDone:NO];
	
	NSData *data = [NSURLConnection sendSynchronousRequest:aRequest returningResponse:response error:error];
	
	[[NSNotificationCenter defaultCenter] performSelectorOnMainThread:@selector(postNotification:) 
														   withObject:[NSNotification notificationWithName:IZURLConnectionConnectionDidFinishNotification object:self] 
														waitUntilDone:NO];
	
	return data;
}

#pragma mark Loading Data Asynchronously

+ (id)connectionWithRequest:(NSURLRequest *)aRequest delegate:(id)aDelegate
{
	return [[[self alloc] initWithRequest:aRequest delegate:aDelegate] autorelease];
}

- (id)initWithRequest:(NSURLRequest *)aRequest delegate:(id)aDelegate
{
	return [self initWithRequest:aRequest delegate:aDelegate startImmediately:YES];	
}

- (id)initWithRequest:(NSURLRequest *)aRequest delegate:(id)aDelegate startImmediately:(BOOL)startImmediately
{
	if (self = [super init])
	{
		self.delegate = aDelegate;
		self.request = aRequest;
		self.loading = NO;
		self.autoRetryAttempt = 0;
		self.autoRetryAttemptLimit = kDefaultAutoRetryAttemptLimit;
		
		// Start the connection
		if (startImmediately)
			[self start];
	}
	
	return self;	
}

- (void)start
{
	if (self.loading == NO)
	{	
		self.loading = YES;
		
		[[NSNotificationCenter defaultCenter] performSelectorOnMainThread:@selector(postNotification:) 
															   withObject:[NSNotification notificationWithName:IZURLConnectionConnectionDidStartNotification object:self] 
															waitUntilDone:NO];		
		
		_shouldRetryWhenReachable = NO; // Reset to default state
		_connection = [[NSURLConnection alloc] initWithRequest:self.request delegate:self];
		
		if (_connection == nil)
		{
			self.loading = NO;
			
			[[NSNotificationCenter defaultCenter] performSelectorOnMainThread:@selector(postNotification:) 
																   withObject:[NSNotification notificationWithName:IZURLConnectionConnectionDidFinishNotification object:self] 
																waitUntilDone:NO];
			
			if ([self.delegate respondsToSelector:@selector(connection:didFailWithError:)])
				[self.delegate connection:(NSURLConnection *)self didFailWithError:nil];
		}
	}
}

#pragma mark Stopping a Connection

- (void)cancel
{
//	self.autoRetryAttempt = 0;
	
	if (_hostReachability)
	{
		[_hostReachability stopNotifer];
		[_hostReachability release]; _hostReachability = nil;
	}	
	
	if (self.loading == YES)
	{
		self.loading = NO;
		
		[[NSNotificationCenter defaultCenter] performSelectorOnMainThread:@selector(postNotification:) 
															   withObject:[NSNotification notificationWithName:IZURLConnectionConnectionDidFinishNotification object:self] 
															waitUntilDone:NO];
		
		[_connection cancel];
		[_connection release]; _connection = nil;		
	}
}

#pragma mark -
#pragma mark NSURLConnection delegate methods

#pragma mark Connection Authentication 

- (BOOL)connection:(NSURLConnection *)connection canAuthenticateAgainstProtectionSpace:(NSURLProtectionSpace *)protectionSpace
{
	if ([self.delegate respondsToSelector:@selector(connection:canAuthenticateAgainstProtectionSpace:)])
		return [self.delegate connection:(NSURLConnection *)self canAuthenticateAgainstProtectionSpace:protectionSpace];
	
	return NO; // Framework Default
}

- (void)connection:(NSURLConnection *)connection didCancelAuthenticationChallenge:(NSURLAuthenticationChallenge *)challenge
{
	if ([self.delegate respondsToSelector:@selector(connection:didCancelAuthenticationChallenge:)])
		[self.delegate connection:(NSURLConnection *)self didCancelAuthenticationChallenge:challenge];	
}

- (void)connection:(NSURLConnection *)connection didReceiveAuthenticationChallenge:(NSURLAuthenticationChallenge *)challenge
{
	if ([self.delegate respondsToSelector:@selector(connection:didReceiveAuthenticationChallenge:)])
		[self.delegate connection:(NSURLConnection *)self didReceiveAuthenticationChallenge:challenge];	
}

- (BOOL)connectionShouldUseCredentialStorage:(NSURLConnection *)connection
{
	if ([self.delegate respondsToSelector:@selector(connectionShouldUseCredentialStorage:)])
		return [self.delegate connectionShouldUseCredentialStorage:(NSURLConnection *)self];
	
	return YES; // Framework Default
}

#pragma mark Connection Data and Responses

- (NSCachedURLResponse *)connection:(NSURLConnection *)connection willCacheResponse:(NSCachedURLResponse *)cachedResponse
{
	if ([self.delegate respondsToSelector:@selector(connection:willCacheResponse:)])
		return [self.delegate connection:(NSURLConnection *)self willCacheResponse:cachedResponse];
	
	return cachedResponse;
}

- (void)connection:(NSURLConnection *)connection didReceiveResponse:(NSURLResponse *)response
{
	if ([response isKindOfClass:[NSHTTPURLResponse class]]) {
		NSHTTPURLResponse* pHttpResponse = (NSHTTPURLResponse *)response;
		NSInteger statusCode = [pHttpResponse statusCode];
		if (statusCode >= 500) {
			NSLog(@"[IZURL] got non-200 code: %d", statusCode);
			self.autoRetryAttempt++;
			if (self.autoRetryAttempt <= self.autoRetryAttemptLimit)
			{
				NSLog(@"[IZURL] will start retry %d nd time", self.autoRetryAttempt);
				[self cancel];
				[self start];
				return; // Notice that we return here and do not let the delegate to be notified that we failed	
			}
		}
	}
	
	if ([self.delegate respondsToSelector:@selector(connection:didReceiveResponse:)]) {
		[self.delegate connection:(NSURLConnection *)self didReceiveResponse:response];
	}
}

- (void)connection:(NSURLConnection *)connection didReceiveData:(NSData *)data
{
	if ([self.delegate respondsToSelector:@selector(connection:didReceiveData:)])
		[self.delegate connection:(NSURLConnection *)self didReceiveData:data];
}

- (void)connection:(NSURLConnection *)connection didSendBodyData:(NSInteger)bytesWritten totalBytesWritten:(NSInteger)totalBytesWritten totalBytesExpectedToWrite:(NSInteger)totalBytesExpectedToWrite
{
	if ([self.delegate respondsToSelector:@selector(connection:didSendBodyData:totalBytesWritten:totalBytesExpectedToWrite:)])
		[self.delegate connection:(NSURLConnection *)self didSendBodyData:bytesWritten totalBytesWritten:totalBytesWritten totalBytesExpectedToWrite:totalBytesExpectedToWrite];
}

- (NSURLRequest *)connection:(NSURLConnection *)connection willSendRequest:(NSURLRequest *)aRequest redirectResponse:(NSURLResponse *)redirectResponse
{
	NSURLRequest *ret = aRequest;
	
	if ([self.delegate respondsToSelector:@selector(connection:willSendRequest:redirectResponse:)])
		ret = [self.delegate connection:(NSURLConnection *)self willSendRequest:aRequest redirectResponse:redirectResponse];
	
	if ([self.request.URL.host isEqual:ret.URL.host] == NO && _hostReachability)
	{
		// We need to change reachability
		[_hostReachability stopNotifer];
		[_hostReachability release];
		
		_hostReachability = [[IZReachability reachabilityWithHostName:ret.URL.host] retain];
		[_hostReachability startNotifer];
	}
	
	self.request = ret;
	
	return ret;
}

#pragma mark Connection Completion

- (void)connection:(NSURLConnection *)connection didFailWithError:(NSError *)error
{	
		NSLog(@"[IZURL]Connection failed! Error - %@ %@",
			  [error localizedDescription],
			  [[error userInfo] objectForKey:NSErrorFailingURLStringKey]);	
	
	[_connection release]; _connection = nil;
	[[NSNotificationCenter defaultCenter] performSelectorOnMainThread:@selector(postNotification:) 
														   withObject:[NSNotification notificationWithName:IZURLConnectionConnectionDidFinishNotification object:self] 
														waitUntilDone:NO];
	self.loading = NO;
	
	if ([error domain] == NSURLErrorDomain)
	{
		if ([error code] == NSURLErrorNotConnectedToInternet)
		{
			NSLog(@"[IZURL]NSURLErrorNotConnectedToInternet, start checking reachability");
			
			if (_hostReachability == nil)
			{
				_hostReachability = [[IZReachability reachabilityWithHostName:self.request.URL.host] retain];
				[[NSNotificationCenter defaultCenter] addObserver:self 
														 selector:@selector(reachabilityChanged:) 
															 name:kReachabilityChangedNotification 
														   object:nil];				
				[_hostReachability startNotifer];			
			}			
			
			_shouldRetryWhenReachable = YES;
			
			return;	// Notice that we return here and do not let the delegate to be notified that we failed	
		}
		
		// These are transient errors. Usually when there's a switch between 3G, EDGE and WiFi network connections -- Issue retry now.
		else if ([error code] == NSURLErrorCannotFindHost || 
				 [error code] == NSURLErrorCannotConnectToHost ||
				 [error code] == NSURLErrorNetworkConnectionLost ||
				 [error code] == NSURLErrorTimedOut)
		{
			NSLog(@"[IZURL]%@", [error localizedDescription]);
			self.autoRetryAttempt++;
			if (self.autoRetryAttempt <= self.autoRetryAttemptLimit)
			{
				NSLog(@"[IZURL] will start retry %d nd time", self.autoRetryAttempt);
				[self start];
				return; // Notice that we return here and do not let the delegate to be notified that we failed	
			}
		}		
	}
	
	if ([self.delegate respondsToSelector:@selector(connection:didFailWithError:)])
		[self.delegate connection:(NSURLConnection *)self didFailWithError:error];
}

- (void)connectionDidFinishLoading:(NSURLConnection *)connection
{
	[_connection release]; _connection = nil;
	[[NSNotificationCenter defaultCenter] performSelectorOnMainThread:@selector(postNotification:) 
														   withObject:[NSNotification notificationWithName:IZURLConnectionConnectionDidFinishNotification object:self] 
														waitUntilDone:NO];
	self.loading = NO;
	
	if (_hostReachability)
	{
		[_hostReachability stopNotifer];
		[_hostReachability release]; _hostReachability = nil;
	}
	
	if ([self.delegate respondsToSelector:@selector(connectionDidFinishLoading:)]) {
		[self.delegate connectionDidFinishLoading:(NSURLConnection *)self];	
	}
}

#pragma mark -
#pragma mark Reachability

- (void)reachabilityChanged:(NSNotification *)notification
{
	NSLog(@"[IZURL]self.loading: %d _shouldRetryWhenReachable: %d", self.loading, _shouldRetryWhenReachable);
	
	if ((IZReachability *)[notification object] != _hostReachability)
		return;
	
	if (_shouldRetryWhenReachable == YES)
	{
		NetworkStatus status = [_hostReachability currentReachabilityStatus];
		if (status != NotReachable)
		{
			NSLog(@"[IZURL]%@ has become reachable! Retrying...", [self.request.URL host]);
			[self start];
		}
	}
}

#pragma mark -

- (void)dealloc
{
	//NSLog(@"[IZURL]dealloc");
	[[NSNotificationCenter defaultCenter] removeObserver:self];
	
	[self cancel];
	
	self.request = nil;
	
	self.delegate = nil;
	
	[super dealloc];
}

@end
