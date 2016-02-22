/*
 *  ngTimer.h
 *  template
 *
 *  Created by Yuhui Wang on 7/28/10.
 *  Copyright 2010 Anansi Mobile. All rights reserved.
 *
 */

#ifndef __NGSTOPWATCH_H__
#define __NGSTOPWATCH_H__

#include "../core/NGE_Types.h"
#include "./ngNOHashMap.h"
#include "../core/ngSingleton.h"
//#include <sys/times.h>

#define NOW_TIME_MS ngStopwatch::GetInstance()->GetNowTimeMS()
#define NOW_TIME_S  ngStopwatch::GetInstance()->GetNowTimeS()
#define NOW_TIME_DAY ngStopwatch::GetInstance()->GetNowTimeDay()

class ngStopwatchListener 
{
public:
	virtual void OnTimeAlarm(NGCSTR key, int32 param) = 0;
};

class ngStopwatch : public ngSingleton
{
public:
	static ngStopwatch* GetInstance();
	static void ReleaseInstance(void *pObj);
	
	virtual ~ngStopwatch();

private:
	ngStopwatch();
	
public:
	void		Start(int64 nowTimeMS);
	boolean		Register(NGCSTR key, int64 targetTimeMS, ngStopwatchListener* pListener, int32 param = 0);
	boolean		UnRegister(NGCSTR key);
	boolean		UnRegister(ngStopwatchListener* pListener);
	int64		GetRemainingTimeMS(NGCSTR key);	
	int64		GetStartTimeMS();
	int64		GetNowTimeMS();
    int64       GetNowTimeS();
    int32       GetNowTimeDay();
    int32       GetLocalTimeDay();
    int64       GetLocalTimeMS();
    int32       GetTimeDay(int64 seconds);
    struct tm*  GetLocalDate();
	boolean		HandleUpdate(int64 timeMS);
	
protected:
	static  ngStopwatch* m_pThis;
	int64	m_startTimeMS;
	int64	m_nowTimeMS;
	ngNOHashMap m_stopwatches;
};


#endif //__NGSTOPWATCH_H__