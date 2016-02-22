/*
 *  ngGameCenter.h
 *  template
 *
 *  Created by Yuhui Wang on 10/26/10.
 *  Copyright 2010 Anansi Mobile. All rights reserved.
 *
 */

#ifndef __NGGAMECENTER_H__
#define __NGGAMECENTER_H__

#include "NGE_Types.h"
#include "ngSingleton.h"

typedef struct _ngGameCenterUserData {
	NGCSTR playerID;
	NGCSTR playerAlias;
} ngGameCenterUserData;

class ngGameCenterListener 
{
public:
	virtual void OnGameCenterAuthenticate(ngGameCenterUserData* pUserData) = 0;
	virtual void OnGameCenterError(int32 errorCode) = 0;
	virtual void OnGameCenterFriends(NGCSTR* ppFriends, int32 nFriends) = 0;
    
#pragma multiplayer
    virtual void OnGameCenterMatchStarted() {}
    virtual void OnGameCenterMatchEnded() {}
    virtual void OnGameCenterMatchDidReceiveData(NGCSTR playerId, int32 dataLength, const void* data) {}
};

class ngGameCenter : ngSingleton
{
public:
	ngGameCenter();
	virtual ~ngGameCenter();
public:
	static ngGameCenter* GetInstance();
	static void ReleaseInstance(void *pObj);
	virtual void SetListener(ngGameCenterListener* pListener);
	
	virtual boolean IsAvailable();
	virtual boolean Authenticate();
    virtual boolean IsAuthenticated();
	virtual boolean Friends();
	
	virtual void ReportScore(int64 score, NGCSTR category);
	virtual void ShowLeaderBoard() {}
	virtual void SubmitAchievement(NGCSTR identifier, double percent);
    
    virtual void FindMatch(int32 minPlayers, int32 maxPlayers, int32 group = -1);
    virtual void SendMessage(int32 size, uint8* data) {}
	
protected:
	ngGameCenterListener* m_pListener;
};


#endif //__NGGAMECENTER_H__