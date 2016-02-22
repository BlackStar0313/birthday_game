/*
 *  ngReachability.cpp
 *  template
 *
 *  Created by Yuhui Wang on 7/21/11.
 *  Copyright 2011 Anansi Mobile. All rights reserved.
 *
 */
#include "ngReachability.h"

ngReachability::ngReachability()
: ngSingleton(ReleaseInstance, this, 1)
{
	m_pListener = NULL;
}

void ngReachability::SetListener(ngReachabilityListener* pListener)
{
	m_pListener = pListener;
}

void ngReachability::SetPulsationInterval(int32 timeMS)
{
	m_interval = timeMS;
}


void ngReachability::OnStatusChange(ngReachabilityListener::ReachabilityStatus status) 
{
	if (m_pListener) {
		m_pListener->OnStatusChange(status);
	}
}