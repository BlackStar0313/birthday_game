/*
 *  ngGameCenterCocoa.h
 *  template
 *
 *  Created by Yuhui Wang on 10/26/10.
 *  Copyright 2010 Anansi Mobile. All rights reserved.
 *
 */

#ifndef __NGGAMECENTERCOCOA_H__
#define __NGGAMECENTERCOCOA_H__

#include "ngGameCenter.h"

class ngGameCenterCocoa : public ngGameCenter 
{
public:
	ngGameCenterCocoa();
	virtual ~ngGameCenterCocoa();
public:
	virtual boolean IsAvailable();
	virtual boolean Authenticate();
    virtual boolean IsAuthenticated();
	virtual boolean Friends();
	
	virtual void OnAuthenticate(ngGameCenterUserData *pUserData);
	virtual void OnError(int32 errorCode);
	virtual void OnFriends(NGCSTR* ppFriends, int32 nFriends);
    virtual void OnMatchStarted();
    virtual void OnMatchEnded();
    virtual void OnMatchDidReceiveData(NGCSTR playerId, int32 dataLength, const void* data);
	
	virtual void ReportScore(int64 score, NGCSTR category);
	virtual void ShowLeaderBoard();
	virtual void SubmitAchievement(NGCSTR identifier, double percent);
    virtual void FindMatch(int32 minPlayers, int32 maxPlayers, int32 group = -1);
    virtual void SendMessage(int32 size, uint8* data);
};


#endif //__NGGAMECENTERCOCOA_H__