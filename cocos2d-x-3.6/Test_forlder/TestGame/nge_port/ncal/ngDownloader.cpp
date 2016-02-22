/*
 *  ngDownloader.cpp
 *  template
 *
 *  Created by Yuhui Wang on 7/30/10.
 *  Copyright 2010 Anansi Mobile. All rights reserved.
 *
 */


#include <ncal/ngDownloader.h>
#include <ncal/ngHttpSession.h>

ngDownloader::ngDownloader()
: m_pListener(NULL)
, m_pSession(NULL)
, m_nCurrentLength(0)
, m_nMaxLength(0) {

}

ngDownloader::~ngDownloader() {

}

int32 ngDownloader::Start(ngStringV2& url)
{
	m_pSession = ngHttpSession::CreateSession();
	m_pSession->SetConnectionListener(this);
	m_pSession->Connect(url);
	
	return NG_OK;
}
