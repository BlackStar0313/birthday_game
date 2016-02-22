/*
 
 File: GameCenterManager.m
 Abstract: Basic introduction to GameCenter
 
 Version: 1.0
 
 Disclaimer: IMPORTANT:  This Apple software is supplied to you by Apple Inc.
 ("Apple") in consideration of your agreement to the following terms, and your
 use, installation, modification or redistribution of this Apple software
 constitutes acceptance of these terms.  If you do not agree with these terms,
 please do not use, install, modify or redistribute this Apple software.
 
 In consideration of your agreement to abide by the following terms, and subject
 to these terms, Apple grants you a personal, non-exclusive license, under
 Apple's copyrights in this original Apple software (the "Apple Software"), to
 use, reproduce, modify and redistribute the Apple Software, with or without
 modifications, in source and/or binary forms; provided that if you redistribute
 the Apple Software in its entirety and without modifications, you must retain
 this notice and the following text and disclaimers in all such redistributions
 of the Apple Software.
 Neither the name, trademarks, service marks or logos of Apple Inc. may be used
 to endorse or promote products derived from the Apple Software without specific
 prior written permission from Apple.  Except as expressly stated in this notice,
 no other rights or licenses, express or implied, are granted by Apple herein,
 including but not limited to any patent rights that may be infringed by your
 derivative works or by other works in which the Apple Software may be
 incorporated.
 
 The Apple Software is provided by Apple on an "AS IS" basis.  APPLE MAKES NO
 WARRANTIES, EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION THE IMPLIED
 WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 PURPOSE, REGARDING THE APPLE SOFTWARE OR ITS USE AND OPERATION ALONE OR IN
 COMBINATION WITH YOUR PRODUCTS.
 
 IN NO EVENT SHALL APPLE BE LIABLE FOR ANY SPECIAL, INDIRECT, INCIDENTAL OR
 CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 ARISING IN ANY WAY OUT OF THE USE, REPRODUCTION, MODIFICATION AND/OR
 DISTRIBUTION OF THE APPLE SOFTWARE, HOWEVER CAUSED AND WHETHER UNDER THEORY OF
 CONTRACT, TORT (INCLUDING NEGLIGENCE), STRICT LIABILITY OR OTHERWISE, EVEN IF
 APPLE HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 
 Copyright (C) 2010 Apple Inc. All Rights Reserved.
 
 */

#import "GameCenterManager.h"
#import <GameKit/GameKit.h>
#import "ngGameCenterCocoa.h"
#import <core/NGE_Macros.h>

@implementation GameCenterManager

@synthesize earnedAchievementCache;
@synthesize currentLeaderBoard;
@synthesize delegate;
@synthesize match;
@synthesize playersDict;

- (id) init
{
	self = [super init];
	if(self!= NULL)
	{
		earnedAchievementCache= NULL;
		delegate = self;
	}
	return self;
}

- (void) dealloc
{
	self.earnedAchievementCache= NULL;
	[super dealloc];
}

- (void)setCocoa:(ngGameCenterCocoa *)coco {
	cocoa = coco;
}

// NOTE:  GameCenter does not guarantee that callback blocks will be execute on the main thread. 
// As such, your application needs to be very careful in how it handles references to view
// controllers.  If a view controller is referenced in a block that executes on a secondary queue,
// that view controller may be released (and dealloc'd) outside the main queue.  This is true
// even if the actual block is scheduled on the main thread.  In concrete terms, this code
// snippet is not safe, even though viewController is dispatching to the main queue:
//
//	[object doSomethingWithCallback:  ^()
//	{
//		dispatch_async(dispatch_get_main_queue(), ^(void)
//		{
//			[viewController doSomething];
//		});
//	}];
//
// UIKit view controllers should only be accessed on the main thread, so the snippet above may
// lead to subtle and hard to trace bugs.  Many solutions to this problem exist.  In this sample,
// I'm bottlenecking everything through  "callDelegateOnMainThread" which calls "callDelegate". 
// Because "callDelegate" is the only method to access the delegate, I can ensure that delegate
// is not visible in any of my block callbacks.

- (void) callDelegate: (SEL) selector withArg: (id) arg error: (NSError*) err
{
	assert([NSThread isMainThread]);
	if([delegate respondsToSelector: selector])
	{
		if(arg != NULL)
		{
			[delegate performSelector: selector withObject: arg withObject: err];
		}
		else
		{
			[delegate performSelector: selector withObject: err];
		}
	}
	else
	{
		NSLog(@"Missed Method");
	}
}


- (void) callDelegateOnMainThread: (SEL) selector withArg: (id) arg error: (NSError*) err
{
	dispatch_async(dispatch_get_main_queue(), ^(void)
	{
	   [self callDelegate: selector withArg: arg error: err];
	});
}

+ (BOOL) isGameCenterAvailable
{
	// check for presence of GKLocalPlayer API
	Class gcClass = (NSClassFromString(@"GKLocalPlayer"));
	
	// check if the device is running iOS 4.1 or later
	NSString *reqSysVer = @"4.1";
	NSString *currSysVer = [[UIDevice currentDevice] systemVersion];
	BOOL osVersionSupported = ([currSysVer compare:reqSysVer options:NSNumericSearch] != NSOrderedAscending);
	
	return (gcClass && osVersionSupported);
}

+ (BOOL) isAuthenticated
{
    return [GKLocalPlayer localPlayer].isAuthenticated;
}

- (void) authenticateLocalUser
{
	//if([GKLocalPlayer localPlayer].authenticated == NO)
	{
		[[GKLocalPlayer localPlayer] authenticateWithCompletionHandler:^(NSError *error) 
		{
			[self callDelegateOnMainThread: @selector(processGameCenterAuth:error:) withArg:[GKLocalPlayer localPlayer] error: error];
		}];
	}
}

- (void) loadFriends
{
	if ([GKLocalPlayer localPlayer].authenticated == YES)  {
		[[GKLocalPlayer localPlayer] loadFriendsWithCompletionHandler:^(NSArray* friends, NSError *error) {
			[self callDelegateOnMainThread: @selector(processGameCenterFriends:error:) withArg:friends error:error];
		}];
	}
}

- (void) reloadHighScoresForCategory: (NSString*) category
{
	GKLeaderboard* leaderBoard= [[[GKLeaderboard alloc] init] autorelease];
	leaderBoard.category= category;
	leaderBoard.timeScope= GKLeaderboardTimeScopeAllTime;
	leaderBoard.range= NSMakeRange(1, 1);
	
	[leaderBoard loadScoresWithCompletionHandler:  ^(NSArray *scores, NSError *error)
	{
		[self callDelegateOnMainThread: @selector(reloadScoresComplete:error:) withArg: leaderBoard error: error];
	}];
}

- (void) reportScore: (int64_t) score forCategory: (NSString*) category 
{
	GKScore *scoreReporter = [[[GKScore alloc] initWithCategory:category] autorelease];	
	scoreReporter.value = score;
	[scoreReporter reportScoreWithCompletionHandler: ^(NSError *error) 
	 {
		 [self callDelegateOnMainThread: @selector(scoreReported:) withArg: NULL error: error];
	 }];
}

- (void) showLeaderboard
{
	GKLeaderboardViewController *leaderboardController = [[GKLeaderboardViewController alloc] init];
    if (leaderboardController != nil)
    {
        /* ruix : use temp view controller to show leaderboard */
        //<<
        tempController = [[UIViewController alloc] init];
        UIApplication* app = [UIApplication sharedApplication];
        UIWindow* topWindow = [app keyWindow];
        if (!topWindow) {
            topWindow = [[app windows] objectAtIndex:0];
        }
        
        [topWindow addSubview:tempController.view];
        leaderboardController.leaderboardDelegate = self;
        [tempController presentModalViewController:leaderboardController animated:YES];
        //>>
        
//		UIViewController* viewController = [NextGenEngine sharedInstance].glViewController;
//		leaderboardController.category = self.currentLeaderBoard;
//		leaderboardController.leaderboardDelegate = self;
//        [viewController presentModalViewController: leaderboardController animated: YES];
    }
}

- (void) submitAchievement: (NSString*) identifier percentComplete: (double) percentComplete
{
	//GameCenter check for duplicate achievements when the achievement is submitted, but if you only want to report 
	// new achievements to the user, then you need to check if it's been earned 
	// before you submit.  Otherwise you'll end up with a race condition between loadAchievementsWithCompletionHandler
	// and reportAchievementWithCompletionHandler.  To avoid this, we fetch the current achievement list once,
	// then cache it and keep it updated with any new achievements.
	if(self.earnedAchievementCache == NULL)
	{
		[GKAchievement loadAchievementsWithCompletionHandler: ^(NSArray *scores, NSError *error)
		{
			if(error == NULL)
			{
				NSMutableDictionary* tempCache= [NSMutableDictionary dictionaryWithCapacity: [scores count]];
				for (GKAchievement* score in tempCache)
				{
					[tempCache setObject: score forKey: score.identifier];
				}
				self.earnedAchievementCache= tempCache;
				[self submitAchievement: identifier percentComplete: percentComplete];
			}
			else
			{
				//Something broke loading the achievement list.  Error out, and we'll try again the next time achievements submit.
				NSLog(@"error when submitting score, code = %d", [error code]);
				if ([self respondsToSelector:@selector(achievementSubmitted:error:)]) {
					[self callDelegateOnMainThread: @selector(achievementSubmitted:error:) withArg: NULL error: error];
				}
			}

		}];
	}
	else
	{
		 //Search the list for the ID we're using...
		GKAchievement* achievement= [self.earnedAchievementCache objectForKey: identifier];
		if(achievement != NULL)
		{
			if((achievement.percentComplete >= 100.0) || (achievement.percentComplete >= percentComplete))
			{
				//Achievement has already been earned so we're done.
				achievement= NULL;
			}
			achievement.percentComplete= percentComplete;
		}
		else
		{
			achievement= [[[GKAchievement alloc] initWithIdentifier: identifier] autorelease];
			achievement.percentComplete= percentComplete;
			//Add achievement to achievement cache...
			[self.earnedAchievementCache setObject: achievement forKey: achievement.identifier];
		}
		if(achievement!= NULL)
		{
			//Submit the Achievement...
			[achievement reportAchievementWithCompletionHandler: ^(NSError *error)
			{
				 [self callDelegateOnMainThread: @selector(achievementSubmitted:error:) withArg: achievement error: error];
			}];
		}
	}
}

- (void) resetAchievements
{
	self.earnedAchievementCache= NULL;
	[GKAchievement resetAchievementsWithCompletionHandler: ^(NSError *error) 
	{
		 [self callDelegateOnMainThread: @selector(achievementResetResult:) withArg: NULL error: error];
	}];
}

- (void) mapPlayerIDtoPlayer: (NSString*) playerID
{
	[GKPlayer loadPlayersForIdentifiers: [NSArray arrayWithObject: playerID] withCompletionHandler:^(NSArray *playerArray, NSError *error)
	{
		GKPlayer* player= NULL;
		for (GKPlayer* tempPlayer in playerArray)
		{
			if([tempPlayer.playerID isEqualToString: playerID])
			{
				player= tempPlayer;
				break;
			}
		}
		[self callDelegateOnMainThread: @selector(mappedPlayerIDToPlayer:error:) withArg: player error: error];
	}];
	
}

- (void)findMatchWithMinPlayers:(int)minPlayers maxPlayers:(int)maxPlayers group:(int)group
{
    GKMatchRequest *request = [[[GKMatchRequest alloc] init] autorelease]; 
    request.minPlayers = minPlayers; 
    request.maxPlayers = maxPlayers;
    if (group >= 0) {
        request.playerGroup = group;
    }
    [[GKMatchmaker sharedMatchmaker] findMatchForRequest:request withCompletionHandler:^(GKMatch *theMatch, NSError *error) {
        if (error) {
            NSLog(@"find match error :%@\n", [error localizedDescription]);
        } else if (theMatch != nil) {
            self.match = theMatch; // Use a retaining property to retain the match
            match.delegate = self; 
            if (!matchStarted && match.expectedPlayerCount == 0) {
                matchStarted = YES; // Insert application-specific code to begin the match.
            }
        }
    }];
}

- (void)sendMessage:(NSData *)data
{
    NSError* error;
    BOOL success = [self.match sendDataToAllPlayers:data withDataMode:GKMatchSendDataReliable error:&error];
    if (!success) {
        NSLog(@"Error sending init packet");
        matchStarted = NO;
        cocoa->OnMatchEnded();
    }
}

- (void) processGameCenterAuth: (GKLocalPlayer *)player error:(NSError*) error
{
	if ([error code] > 0) {
		cocoa->OnError([error code]);
	} else {
		ngGameCenterUserData* pUserData = DNEW ngGameCenterUserData();
		pUserData->playerID = [[player playerID] UTF8String];
		pUserData->playerAlias = [[player alias] UTF8String];
		
		cocoa->OnAuthenticate(pUserData);
	}
}

- (void) processGameCenterFriends: (NSArray *)friends error:(NSError *)error
{
	int32 nFriends = [friends count];
	NSLog(@"loaded %d friends", nFriends);
	NGCSTR* ppFriends = DNEWARR(NGCSTR, nFriends);
	for (int i = 0; i < nFriends; i ++) {
		NSString* player = (NSString *)[friends objectAtIndex:i];
		ppFriends[i] = [player UTF8String];
		NSLog(@"player %d is %@", i, player);
	}
	cocoa->OnFriends(ppFriends, nFriends);
}

- (void) scoreReported: (NSError*) error 
{
	if([error code] > 0) {
		cocoa->OnError([error code]);
	}else{
		// TODO: when score reported..
	}
}

- (void)leaderboardViewControllerDidFinish:(GKLeaderboardViewController *)viewController
{
//	UIViewController* vc = [NextGenEngine sharedInstance].glViewController;
//    [vc dismissModalViewControllerAnimated:YES];
    
    /* ruix : use temp view controller to show leaderboard */
    //<<
    [tempController dismissModalViewControllerAnimated:YES];
    [tempController.view removeFromSuperview];
    tempController = nil;
    //>>
}


- (void)lookupPlayers {
    
    NSLog(@"Looking up %d players...", match.playerIDs.count);
    [GKPlayer loadPlayersForIdentifiers:match.playerIDs withCompletionHandler:^(NSArray *players, NSError *error) {
        
        if (error != nil) {
            NSLog(@"Error retrieving player info: %@", error.localizedDescription);
            matchStarted = NO;
            cocoa->OnMatchEnded();
        } else {
            
            // Populate players dict
            self.playersDict = [NSMutableDictionary dictionaryWithCapacity:players.count];
            for (GKPlayer *player in players) {
                NSLog(@"Found player: %@", player.alias);
                [playersDict setObject:player forKey:player.playerID];
            }
            
            // Notify delegate match can begin
            matchStarted = YES;
            cocoa->OnMatchStarted();
            
        }
    }];
    
}

#pragma mark GKMatchDelegate

// The match received data sent from the player.
- (void)match:(GKMatch *)theMatch didReceiveData:(NSData *)data fromPlayer:(NSString *)playerID {
    
    if (match != theMatch) return;
    
    cocoa->OnMatchDidReceiveData([playerID UTF8String], [data length], [data bytes]);
}

// The player state changed (eg. connected or disconnected)
- (void)match:(GKMatch *)theMatch player:(NSString *)playerID didChangeState:(GKPlayerConnectionState)state {
    
    if (match != theMatch) return;
    
    switch (state) {
        case GKPlayerStateConnected: 
            // handle a new player connection.
            NSLog(@"Player connected!");
            
            if (!matchStarted && theMatch.expectedPlayerCount == 0) {
                NSLog(@"Ready to start match!");
                [self lookupPlayers];
            }
            
            break; 
        case GKPlayerStateDisconnected:
            // a player just disconnected. 
            NSLog(@"Player disconnected!");
            matchStarted = NO;
            cocoa->OnMatchEnded();
            break;
    }                 
    
}

// The match was unable to connect with the player due to an error.
- (void)match:(GKMatch *)theMatch connectionWithPlayerFailed:(NSString *)playerID withError:(NSError *)error {
    
    if (match != theMatch) return;
    
    NSLog(@"Failed to connect to player with error: %@", error.localizedDescription);
    matchStarted = NO;
    cocoa->OnMatchEnded();
}

// The match was unable to be established with any players due to an error.
- (void)match:(GKMatch *)theMatch didFailWithError:(NSError *)error {
    
    if (match != theMatch) return;
    
    NSLog(@"Match failed with error: %@", error.localizedDescription);
    matchStarted = NO;
    cocoa->OnMatchEnded();
}

@end
