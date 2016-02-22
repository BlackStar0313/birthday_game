/*
 *  ngHttpQueue.cpp
 *  template
 *
 *  Created by Yuhui Wang on 5/06/11.
 *  Copyright 2010 Anansi Mobile. All rights reserved.
 *
 */

#include <httpclient/ngHttpQueue.h>
#include <httpclient/ngHttpClient.h>
#include <httpclient/ngHttpClientFactory.h>

static ngHttpQueue* gHttpQueue = NULL;

ngHttpQueue* ngHttpQueue::GetInstance()
{
	if (gHttpQueue == NULL) {
		gHttpQueue = DNEW(ngHttpQueue);
	}
	return gHttpQueue;
}

void ngHttpQueue::ReleaseInstance(void *pObj)
{
	ngHttpQueue* pThis = (ngHttpQueue *)pObj;
	DELETE_OBJECT(pThis, ngHttpQueue);
}

ngHttpQueue::ngHttpQueue()
: ngSingleton(ReleaseInstance, this, 1), m_pause(false)
{
	
}

ngHttpQueue::~ngHttpQueue()
{
	Clear();
}

void ngHttpQueue::Clear()
{
	while (m_clients.Size() > 0) {
		ngHttpClient* pClient = (ngHttpClient *)m_clients.Poll();
		SAFE_DELETE(pClient);
	}
	m_clients.Clear();
}

boolean ngHttpQueue::HandleUpdate(int32 deltaMS)
{
	int32 size = m_clients.Size();
	while ((size > 0) && !m_pause) {
		ngHttpClient* pClient = (ngHttpClient *)m_clients.Poll();
		if (pClient->GetStatus() == ngHttpClient::CLIENT_FINISH) {
			SAFE_DELETE(pClient);
		}else {
			m_clients.Offer(pClient);
			pClient->HandleUpdate(deltaMS);
		}
		size --;
	}
	
	return TRUE;
}

ngHttpClient* ngHttpQueue::Take()
{
	ngHttpClient* pClient = ngHttpClientFactory::CreateHttpClient();
	m_clients.Offer(pClient);
	return pClient;
}

void ngHttpQueue::Deaf(ngHttpClientListener *pListener)
{
	int32 size = m_clients.Size();
	while (size > 0) {
		ngHttpClient* pClient = (ngHttpClient *)m_clients.Poll();
		if (pClient->GetListener() == pListener) {
			pClient->SetListener(NULL);
		}
		m_clients.Offer(pClient);
		size --;
	}	
}

void ngHttpQueue::setPause(boolean pause)
{
    m_pause = pause;
}
