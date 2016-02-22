/*
 *  ngReachability.h
 *  template
 *
 *  Created by Yuhui Wang on 7/21/11.
 *  Copyright 2011 Anansi Mobile. All rights reserved.
 *
 */

#ifndef __NGREACHABILITY_H__
#define __NGREACHABILITY_H__


#include "NGE_Types.h"
#include "ngStringV2.h"
#include "ngSingleton.h"

#define TEST_SITE "www.apple.com"
//#define TEST_SITE "www.baidu.com"

class ngReachabilityListener 
{
public:
	enum ReachabilityStatus {
		RSNotReachable = 0,
		RSReachableViaWiFi,
		RSReachableViaWWAN
	};
public:
	virtual void OnStatusChange(ReachabilityStatus status) = 0;
};

class ngReachability : ngSingleton
{
public:
	static ngReachability* GetInstance();
	static void ReleaseInstance(void *pObj);
	
	virtual void Start() = 0;
	virtual void Stop() = 0;
	virtual int32 GetStatus() = 0;
	
	void SetListener(ngReachabilityListener* pListener);
	void SetPulsationInterval(int32 timeMS);
	
public:
	void OnStatusChange(ngReachabilityListener::ReachabilityStatus status);
	
protected:
	ngReachability();
	
protected:
	int32 m_interval;
	ngReachabilityListener* m_pListener;
};




#endif //__NGREACHABILITY_H__