/*
 *  ngReachabilityCocoa.cpp
 *  template
 *
 *  Created by Yuhui Wang on 7/21/11.
 *  Copyright 2011 Anansi Mobile. All rights reserved.
 *
 */

#import "ngReachabilityCocoa.h"
#import "IZReachability.h"

@interface ReachabilityDelegate : NSObject  
{
	ngReachability* reach;
	IZReachability* reachability;
}

- (id)init:(ngReachability *)parent host:(NGCSTR)host;
- (void)start;
- (void)stop;
- (int)status;

@end

@implementation ReachabilityDelegate

- (id)init:(ngReachability *)parent host:(NGCSTR)host{
	if ((self = [super init]) != nil) {
		reach = parent;
		NSString* theHost = [NSString stringWithUTF8String:host];
		reachability = [[IZReachability reachabilityWithHostName:theHost] retain];
	}
	return self;
}

- (void)start {
	[[NSNotificationCenter defaultCenter] addObserver:self 
											 selector:@selector(reachabilityChanged:) 
												 name:kReachabilityChangedNotification 
											   object:nil];	
	[reachability startNotifer];
	NSLog(@"start reachability");
}

- (void)stop {
	[reachability stopNotifer];
	NSLog(@"stop reachability");
}

- (int)status {
	return [reachability currentReachabilityStatus];
}

- (void)reachabilityChanged:(NSNotification *)notification
{	
	NSLog(@"reachability status changed");
	if ((IZReachability *)[notification object] != reachability) {
		return;
	}	
	{
		NetworkStatus status = [reachability currentReachabilityStatus];
		reach->OnStatusChange((ngReachabilityListener::ReachabilityStatus)status);
	}
}

@end


static ReachabilityDelegate* delegate = NULL;
static ngReachabilityCocoa* sInstance = NULL;

ngReachability* ngReachability::GetInstance()
{
	if (sInstance == NULL) {
		sInstance = DNEW ngReachabilityCocoa();
	}
	return sInstance;
}

void ngReachability::ReleaseInstance(void *pObj)
{
	DELETE_OBJECT((ngReachabilityCocoa *)pObj, ngReachabilityCocoa);
}

ngReachabilityCocoa::ngReachabilityCocoa()
{
	if (delegate != nil) {
		[delegate release];
		delegate = nil;
	}
	delegate = [[ReachabilityDelegate alloc] init:this host:TEST_SITE];
	m_pListener = NULL;
}

void ngReachabilityCocoa::Start()
{
	[delegate start];
}

void ngReachabilityCocoa::Stop()
{
	[delegate stop];
}

int32 ngReachabilityCocoa::GetStatus() 
{
	return [delegate status];
}