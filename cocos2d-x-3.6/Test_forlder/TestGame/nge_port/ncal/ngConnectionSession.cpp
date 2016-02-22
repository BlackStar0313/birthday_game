/*
 *  ngConnectionSession.cpp
 *  NextGenEngine
 *
 *  Created by cooler on 10-5-19.
 *  Copyright 2010 DreaminGame. All rights reserved.
 *
 */

#include <ncal/ngConnectionSession.h>
#include <ncal/ngConnectionRequest.h>
#include <ncal/ngConnectionManager.h>

ngConnectionSession::ngConnectionSession()
: m_pListener(NULL)
, m_DESEncrypt(FALSE)
, m_pRequest(NULL) {
	m_status = SESSION_IDLE;

    ngConnectionManager::GetInstance()->OnNewConnectionSession(this);
}

ngConnectionSession::~ngConnectionSession() {
	SAFE_DELETE(m_pRequest);
}

void ngConnectionSession::Connect(ngStringV2& url)
{
	// change status
	m_status = SESSION_CONNECTING;

	// Platform request
	RequestInternal(url);
	
}

