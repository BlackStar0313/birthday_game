//
//  ngCURLManager.cpp
//  pet_native_project_lib
//
//  Created by chen on 12-11-1.
//  Copyright (c) 2012年 Anansi. All rights reserved.
//

#include "ngCURLManager.h"

#if defined (NGE_PLATFORM_ANDROID)

#include <android/network/curl/ngCURL.h>

ngCURLManager* ngCURLManager::m_pThis = NULL;

ngCURLManager* ngCURLManager::GetInstance() {
    if (m_pThis == NULL) {
        m_pThis = DNEW(ngCURLManager);
    }
    return m_pThis;
}

void ngCURLManager::ReleaseInstance(void* pObj) {
    ngCURLManager* pThis = (ngCURLManager*)pObj;
    DELETE_OBJECT(pThis, ngCURLManager);
}

ngCURLManager::ngCURLManager()
: ngSingleton(ReleaseInstance, this) {

}

ngCURLManager::~ngCURLManager() {
    LIST_CLEAR(&m_curls, ngCURL);
}

void ngCURLManager::Update(/*int32 deltaMS*/) {
    int32 size = m_curls.Size();
	while ((size > 0)) {
		ngCURL* pCURL = (ngCURL*)m_curls.Poll();
        pCURL->CommitMainQueue();
		if (pCURL->IsDone()) {
            pCURL->NotifyDone();
			pCURL->SetIdle();
            SAFE_DELETE(pCURL);
		} else {
			m_curls.Offer(pCURL);
		}
		size--;
	}
}

void ngCURLManager::OfferCURL(ngCURL* pCURL) {
    m_curls.Offer(pCURL);
}

#endif
