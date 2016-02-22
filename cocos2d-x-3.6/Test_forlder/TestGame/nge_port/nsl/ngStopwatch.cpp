/*
 *  ngStopwatch.cpp
 *  template
 *
 *  Created by Yuhui Wang on 7/28/10.
 *  Copyright 2010 Anansi Mobile. All rights reserved.
 *
 */


#include <nsl/ngStopwatch.h>
#include <nsl/ngHashMapIterator.h>

class ngStopwatchNode 
{
public:
	ngStopwatchNode(ngStopwatch* pParent, NGCSTR key, int64 targetTimeMS, ngStopwatchListener* pListener, int32 param);
public:
	void	Visit(int32 timeMS);
	int64	GetRemainingTimeMS();
	boolean GetAvailable() { return m_bAvailable; }
	ngStopwatchListener* GetCaller() { return m_pCaller; }
protected:
	boolean	m_bAvailable;
	ngStopwatch* m_pParent;
	ngStringV2 m_key;
	int64	m_remainingTimeMS;
	ngStopwatchListener*	m_pCaller;
	int32	m_param;
};


ngStopwatchNode::ngStopwatchNode(ngStopwatch* pParent, NGCSTR key, int64 targetTimeMS, ngStopwatchListener* pListener, int32 param)
{
	m_pParent = pParent;
	m_bAvailable = TRUE;
	m_key = key;
	m_remainingTimeMS = targetTimeMS - pParent->GetNowTimeMS();
	m_pCaller = pListener;
	m_param = param;
}

void ngStopwatchNode::Visit(int32 timeMS)
{
	m_remainingTimeMS -= timeMS;
	if (m_remainingTimeMS <= 0) {
		m_pCaller->OnTimeAlarm(m_key.GetCString(), m_param);
		m_bAvailable = FALSE;
	}
}

int64 ngStopwatchNode::GetRemainingTimeMS()
{
	return m_remainingTimeMS;
}

ngStopwatch* ngStopwatch::m_pThis = NULL;

ngStopwatch* ngStopwatch::GetInstance()
{
	if (m_pThis == NULL) {
		m_pThis = DNEW(ngStopwatch);
	}
	return m_pThis;
}

void ngStopwatch::ReleaseInstance(void *pObj)
{
	ngStopwatch* pThis = (ngStopwatch *)pObj;
	DELETE_OBJECT(pThis, ngStopwatch);
}

ngStopwatch::ngStopwatch()
: ngSingleton(ReleaseInstance, this, 1)
, m_startTimeMS(0)
, m_nowTimeMS(0)
{
	m_stopwatches.Initialize();
}

ngStopwatch::~ngStopwatch()
{
	ngHashMapIterator* pIterator = (ngHashMapIterator *)m_stopwatches.Iterator();
	while (pIterator->HasNext()) {
		ngMap::ngEntry* pEntry = pIterator->NextEntry();
		ngStopwatchNode* pNode = (ngStopwatchNode *)pEntry->GetValue();
		SAFE_DELETE(pNode);
	}
	SAFE_DELETE(pIterator);
	m_stopwatches.RemoveAll();
}

void ngStopwatch::Start(int64 nowTimeMS)
{
	m_startTimeMS = nowTimeMS;
	m_nowTimeMS = nowTimeMS;
}

boolean ngStopwatch::Register(NGCSTR key, int64 targetTimeMS, ngStopwatchListener* pListener, int32 param)
{
	if (targetTimeMS <= m_nowTimeMS) {
		return FALSE;
	}
	ngStopwatchNode* pNode = (ngStopwatchNode *)m_stopwatches.Get(key);
	if (pNode != NULL) {
		SAFE_DELETE(pNode);
	}
	pNode = DNEW ngStopwatchNode(this, key, targetTimeMS, pListener, param);
	m_stopwatches.Put(key, pNode);
	
	return TRUE;
}

boolean ngStopwatch::UnRegister(NGCSTR key)
{
	ngStopwatchNode* pNode = (ngStopwatchNode *)m_stopwatches.Get(key);
	
	if (pNode == NULL)
		return FALSE;
	
	SAFE_DELETE(pNode);
	if (m_stopwatches.Remove(key) != NULL) {
		return TRUE;
	}
	
	return FALSE;
}

boolean ngStopwatch::UnRegister(ngStopwatchListener* pListener)
{
	ngHashMapIterator* pIterator = (ngHashMapIterator *)m_stopwatches.Iterator();
	while (pIterator->HasNext()) {
		ngMap::ngEntry* pEntry = pIterator->NextEntry();
		ngStopwatchNode* pNode = (ngStopwatchNode *)pEntry->GetValue();
		if (pNode->GetCaller() == pListener) {
			NGCSTR key = (NGCSTR)pEntry->GetKey();
			m_stopwatches.Remove(key);
			SAFE_DELETE(pNode);
		}		
	}
	SAFE_DELETE(pIterator);
	return TRUE;
}

int64 ngStopwatch::GetRemainingTimeMS(NGCSTR key)
{
	ngStopwatchNode* pNode = (ngStopwatchNode *)m_stopwatches.Get(key);
	if (pNode != NULL) {
		return pNode->GetRemainingTimeMS();
	}
	return -1;
}

int64 ngStopwatch::GetStartTimeMS()
{
	return m_startTimeMS;
}

int64 ngStopwatch::GetNowTimeMS()
{
	return m_nowTimeMS;
}

int64 ngStopwatch::GetNowTimeS()
{
    return m_nowTimeMS / 1000;
}

int32 ngStopwatch::GetNowTimeDay()
{
    int64 second = GetNowTimeS();
    return GetTimeDay(second);
}

int32 ngStopwatch::GetLocalTimeDay()
{
    int64 ms = GetLocalTimeMS();
    return GetTimeDay(ms/1000);
}


#if defined  (_WIN32) || defined (_WIN64)
#include <time.h>
#include <windows.h>

struct timeval {
        long    tv_sec;         /* seconds */
        long    tv_usec;        /* and microseconds */
};


int gettimeofday(struct timeval *tp, void *tzp)
{
    time_t clock;
    struct tm tm;
    SYSTEMTIME wtm;

    GetLocalTime(&wtm);
    tm.tm_year     = wtm.wYear - 1900;
    tm.tm_mon     = wtm.wMonth - 1;
    tm.tm_mday     = wtm.wDay;
    tm.tm_hour     = wtm.wHour;
    tm.tm_min     = wtm.wMinute;
    tm.tm_sec     = wtm.wSecond;
    tm. tm_isdst    = -1;
    clock = mktime(&tm);
    tp->tv_sec = clock;
    tp->tv_usec = wtm.wMilliseconds * 1000;

    return (0);
}
#else
#include <sys/time.h>
#endif

int64 ngStopwatch::GetLocalTimeMS()
{
    timeval tm;
    gettimeofday(&tm, NULL);
    return (int64)tm.tv_sec * 1000 + tm.tv_usec / 1000;
}

int32 ngStopwatch::GetTimeDay(int64 seconds)
{
    return seconds / (60 * 60 * 24);
}

struct tm*  ngStopwatch::GetLocalDate()
{
    time_t currentTime;
    currentTime = time(NULL);
    return localtime(&currentTime);
}

boolean ngStopwatch::HandleUpdate(int64 timeMS)
{
	m_nowTimeMS += timeMS;
	ngHashMapIterator* pIterator = (ngHashMapIterator *)m_stopwatches.Iterator();
	while (pIterator->HasNext()) {
		ngMap::ngEntry* pEntry = pIterator->NextEntry();
		ngStopwatchNode* pNode = (ngStopwatchNode *)pEntry->GetValue();
		if (pNode->GetAvailable() == FALSE) {
			m_stopwatches.Remove((NGCSTR)pEntry->GetKey());
			SAFE_DELETE(pNode);
		}else{			
			pNode->Visit(timeMS);
		}
	}
	SAFE_DELETE(pIterator);
	return TRUE;
}

