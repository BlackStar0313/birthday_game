//
//  ngStringManager.cpp
//  NextGenEngine
//
//  Created by Chen Zhen on 13-6-20.
//  Copyright (c) 2013年 Anansi. All rights reserved.
//

#include <game/ngStringManager.h>

ngStringManager* ngStringManager::m_pThis = NULL;

ngStringManager* ngStringManager::GetInstance() {
	if (m_pThis == NULL) {
		m_pThis = DNEW(ngStringManager);
	}
	
	return m_pThis;
}

ngStringManager::ngStringManager()
: ngSingleton(ReleaseInstance, this) {

}

ngStringManager::~ngStringManager() {
}

void ngStringManager::ReleaseInstance(void *pObj) {
	ngStringManager* pThis = (ngStringManager *)pObj;
	DELETE_OBJECT(pThis, ngStringManager);
}