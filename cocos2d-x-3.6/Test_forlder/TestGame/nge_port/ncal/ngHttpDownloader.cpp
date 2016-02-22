/*
 *  ngHttpDownloader.cpp
 *  template
 *
 *  Created by Yuhui Wang on 7/30/10.
 *  Copyright 2010 Anansi Mobile. All rights reserved.
 *
 */

#include <ncal/ngHttpDownloader.h>
#include <ncal/ngHttpSession.h>
#include <ncal/ngHttpResponse.h>
#include <core/ngDevice.h>

ngHttpDownloader::ngHttpDownloader()
: m_saveTempFile(FALSE)
, m_pTempFile(NULL) {
    
}

ngHttpDownloader::~ngHttpDownloader() {
    if (m_saveTempFile && m_pTempFile) {
        fclose(m_pTempFile);
    }
}

void ngHttpDownloader::InitHttpSession() {
	m_pSession = ngHttpSession::CreateSession();
	m_pSession->SetConnectionListener(this);

    //here m_pSession must be a ngHttpSession.  [zhen.chen]
    ((ngHttpSession*)m_pSession)->SetFileBuffered(m_saveTempFile);
}

int32 ngHttpDownloader::StartWithExistSession(ngStringV2& url) {
    NGASSERT(m_pSession);
	m_pSession->Connect(url);
    
    return NG_OK;
}

int32 ngHttpDownloader::Start(ngStringV2& url)
{
	m_url = url;
	m_pSession = ngHttpSession::CreateSession();
	m_pSession->SetConnectionListener(this);

    //here m_pSession must be a ngHttpSession.  [zhen.chen]
    ((ngHttpSession*)m_pSession)->SetFileBuffered(m_saveTempFile);

	m_pSession->Connect(url);
	
	return NG_OK;
	
}

void ngHttpDownloader::OnConnect()
{
}

void ngHttpDownloader::OnResponse(ngConnectionResponse* pResponse)
{
	m_nMaxLength = pResponse->GetContentLength();
	m_nCurrentLength = 0;
    
    if (m_saveTempFile) {
        ngStringV2 tempFileName;
        tempFileName.Format("__%s.temp", GetFileName());
        ngStringV2 tempFilePath;
        ngDevice::GetInstance()->GetFileWritePath(tempFilePath, tempFileName);
        
        m_pTempFile = fopen(tempFilePath.GetCString(), "wb");
        NGASSERT(m_pTempFile);
    }
}

void ngHttpDownloader::OnData(uint8 *pData, uint32 nData)
{
	m_nCurrentLength += nData;
    if (m_saveTempFile && m_pTempFile) {
        //just write all data.
        fwrite(pData, nData, 1, m_pTempFile);
    }
	if (m_nMaxLength >= m_nCurrentLength) {
		int32 percent = (float)m_nCurrentLength * 100 / m_nMaxLength;   //here use (float) cast to avoid int32 overflow.
		NG_DEBUG_LOG("downloader ondata: len=%d, max=%d, percent=%d\n", m_nCurrentLength, m_nMaxLength, percent);
        if (m_pListener) {
            m_pListener->OnDownloadUpdate( percent );
        }
	}
}

void ngHttpDownloader::OnErrorStop(NG_CONNECTION_ERROR nErrorCode)
{
    if (m_saveTempFile && m_pTempFile) {
        fclose(m_pTempFile);
    }
    if (m_pListener) {
        m_pListener->OnDownloadFailed();
    }
}

void ngHttpDownloader::OnFinish()
{
    if (m_saveTempFile && m_pTempFile) {
        fclose(m_pTempFile);
        ngStringV2 tempFileName;
        tempFileName.Format("__%s.temp", GetFileName());
        if (m_pListener) {
            m_pListener->OnDownloadComplete(NULL, tempFileName.GetCString(), m_url.GetCString());
        }
        return;
    }
	ngHttpSession* pSession = (ngHttpSession *)m_pSession;
	ngByteBuffer* pBuffer = ((ngHttpResponse *)(pSession->GetResponse()))->GetBuffer();
	
    if (m_pListener) {
        m_pListener->OnDownloadComplete(pBuffer, GetFileName(), m_url.GetCString());
    }

    m_pSession->SetUseless();
}

NGCSTR ngHttpDownloader::GetFileName()
{
#ifdef NGE_PLATFORM_ANDROID
	const char* pUrl = m_url.GetCString();
	const char* index = NULL;
	const char* idx = strstr(pUrl, "/");
	while (idx != NULL) {
		index = idx;
		idx = strstr(idx + 1, "/");
	}	
	return (NGCSTR)(index + 1);
#else
	ngHttpSession* pSession = (ngHttpSession *)m_pSession;
	ngHttpResponse* pResponse = (ngHttpResponse *)(pSession->GetResponse());
    if (pResponse == NULL) {
        return NULL;
    }
	return pResponse->GetSuggestedFilename();
#endif
}
