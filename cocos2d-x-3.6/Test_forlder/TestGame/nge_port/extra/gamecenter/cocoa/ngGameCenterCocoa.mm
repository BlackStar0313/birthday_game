/*
 *  ngGameCenterCocoa.mm
 *  template
 *
 *  Created by Yuhui Wang on 10/26/10.
 *  Copyright 2010 Anansi Mobile. All rights reserved.
 *
 */


#include "ngGameCenterCocoa.h"
#include "NGE_Macros.h"
#import <UIKit/UIKit.h>
#import <GameKit/GameKit.h>
#import "GameCenterManager.h"
#include <nsl/ngStringV2.h>
#include "TargetConditionals.h"

static ngGameCenterCocoa* m_pThis = NULL;
static GameCenterManager* manager;
ngGameCenter* ngGameCenter::GetInstance() 
{
	if (m_pThis == NULL) {
		m_pThis = DNEW ngGameCenterCocoa();
	}
	return m_pThis;
}

void ngGameCenter::ReleaseInstance(void *pObj)
{
	DELETE_OBJECT((ngGameCenterCocoa *)pObj, ngGameCenterCocoa);
}

ngGameCenterCocoa::ngGameCenterCocoa()
{
	if (manager != nil) {
		[manager release];
		manager = nil;
	}
	manager = [[GameCenterManager alloc] init];	
	[manager setCocoa:this];
	m_pListener = NULL;
}

ngGameCenterCocoa::~ngGameCenterCocoa()
{
}

boolean ngGameCenterCocoa::IsAvailable()
{
	return [GameCenterManager isGameCenterAvailable];
}

boolean ngGameCenterCocoa::Authenticate()
{
#if !TARGET_IPHONE_SIMULATOR
	[manager authenticateLocalUser];
#endif
	return TRUE;
}

boolean ngGameCenterCocoa::IsAuthenticated()
{
    return [GameCenterManager isAuthenticated];
}

boolean ngGameCenterCocoa::Friends()
{
	[manager loadFriends];
	return TRUE;
}

void ngGameCenterCocoa::OnAuthenticate(ngGameCenterUserData* pUserData)
{
	if (m_pListener != NULL) {
		m_pListener->OnGameCenterAuthenticate(pUserData);
	}
	SAFE_DELETE(pUserData);
}

void ngGameCenterCocoa::OnError(int32 errorCode)
{
	if (m_pListener != NULL) {
		m_pListener->OnGameCenterError(errorCode);
	}
//#ifdef DEBUG	
//	ngStringV2 code((int64)errorCode);
//	ngAlertDialog::GetInstance()->SetContent(code.GetCString());
//	ngAlertDialog::GetInstance()->SetTitle("GameCenter Error");
//	ngAlertDialog::GetInstance()->Show();
//#endif
}

void ngGameCenterCocoa::OnFriends(NGCSTR* ppFriends, int32 nFriends)
{
	if (m_pListener != NULL) {
		m_pListener->OnGameCenterFriends(ppFriends, nFriends);
	}
	SAFE_DELETE(ppFriends);
}

void ngGameCenterCocoa::ReportScore(int64 score, NGCSTR category)
{
	NSString* strCategory = [NSString stringWithUTF8String:category];
	manager.currentLeaderBoard = strCategory;
	[manager reportScore:score forCategory:strCategory];
}

void ngGameCenterCocoa::SubmitAchievement(NGCSTR identifier, double percent)
{
	NSString* strId = [NSString stringWithUTF8String:identifier];
	[manager submitAchievement:strId percentComplete:percent];
}

void ngGameCenterCocoa::ShowLeaderBoard()
{
	[manager showLeaderboard];
}

void ngGameCenterCocoa::FindMatch(int32 minPlayers, int32 maxPlayers, int32 group) 
{
    [manager findMatchWithMinPlayers:minPlayers maxPlayers:maxPlayers group:group];
}

void ngGameCenterCocoa::SendMessage(int32 size, uint8 *data)
{
    NSData *theData = [NSData dataWithBytes:(const void *)data length:size];
    [manager sendMessage:theData];
}

void ngGameCenterCocoa::OnMatchStarted()
{
    if (m_pListener != NULL) {
        m_pListener->OnGameCenterMatchStarted();
    }
}

void ngGameCenterCocoa::OnMatchEnded()
{
    if (m_pListener != NULL) {
        m_pListener->OnGameCenterMatchEnded();
    }
}

void ngGameCenterCocoa::OnMatchDidReceiveData(NGCSTR playerId, int32 dataLength, const void *data)
{
    if (m_pListener != NULL) {
        m_pListener->OnGameCenterMatchDidReceiveData(playerId, dataLength, data);
    }
}

