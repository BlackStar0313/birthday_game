//
//  NextGenEngine.m
//  NextGenEngine
//
//  Created by ppcfan on 10-9-19.
//  Copyright 2010 Anansi Mobile. All rights reserved.
//

#import "NextGenEngine.h"


static NextGenEngine* engineInstance = nil;

@implementation NextGenEngine

@synthesize glViewController;
@synthesize glView;

+ (NextGenEngine*)sharedInstance
{
	if (engineInstance == nil)
	{
		engineInstance = [[NextGenEngine alloc] init];
	}
	
	return engineInstance;
}

- (BOOL)textFieldShouldReturn:(UITextField *)textField
{
	[textField resignFirstResponder];
	return NO;
}

- (void)registerPushNotification
{
    [[UIApplication sharedApplication]
     registerForRemoteNotificationTypes:(UIRemoteNotificationType)(UIRemoteNotificationTypeBadge | UIRemoteNotificationTypeSound | UIRemoteNotificationTypeAlert)];
}


@end
