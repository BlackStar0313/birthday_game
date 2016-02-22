/*
 *  ngGameCenter.cpp
 *  template
 *
 *  Created by Yuhui Wang on 10/26/10.
 *  Copyright 2010 Anansi Mobile. All rights reserved.
 *
 */


#include "ngGameCenter.h"

ngGameCenter::ngGameCenter()
: ngSingleton(ReleaseInstance, this, 1)
{
}

ngGameCenter::~ngGameCenter()
{
}

void ngGameCenter::SetListener(ngGameCenterListener* pListener)
{
	m_pListener = pListener;
}

boolean ngGameCenter::IsAvailable() 
{
	return FALSE;
}

boolean ngGameCenter::Authenticate()
{
	return FALSE;
}

boolean ngGameCenter::IsAuthenticated()
{
    return FALSE;
}

boolean ngGameCenter::Friends()
{
	return FALSE;
}

void ngGameCenter::ReportScore(int64 score, NGCSTR category) {}
void ngGameCenter::SubmitAchievement(NGCSTR identifier, double percent) {}
void ngGameCenter::FindMatch(int32 minPlayers, int32 maxPlayers, int32 group) {}

#if defined (NGE_PLATFORM_METRO)
void ngGameCenter::ReleaseInstance(void *pObj) {}
#endif