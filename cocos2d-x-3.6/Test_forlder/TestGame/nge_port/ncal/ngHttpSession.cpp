/*
 *  ngHttpSession.cpp
 *  NextGenEngine
 *
 *  Created by cooler on 10-5-22.
 *  Copyright 2010 DreaminGame. All rights reserved.
 *
 */

#include <ncal/ngHttpSession.h>
#include <ncal/ngHttpRequest.h>

ngString ngHttpSession::m_XorKey = "";
ngString ngHttpSession::m_tailKey = "";

ngHttpSession::ngHttpSession()
: m_pResponse(NULL)
, m_fileBuffered(FALSE)
{

}

ngHttpSession::~ngHttpSession()
{
	SAFE_DELETE(m_pResponse);
}

void ngHttpSession::OnConnect()
{
	// do something.
	m_status = SESSION_CONNECTED;
	m_pListener->OnConnect();
}

void ngHttpSession::OnResponse(ngConnectionResponse* pResponse)
{
	if (m_pResponse == NULL) {	
		m_pResponse = (ngHttpResponse*)pResponse;
		int32 nDataLength = m_pResponse->GetContentLength();
        
        if (!m_fileBuffered) {
            ngByteBuffer* pBuffer = m_pResponse->GetBuffer();
            pBuffer->SetCapacity(nDataLength);
        }

		m_status = SESSION_RESPONSE;
		if (m_pListener) {
			m_pListener->OnResponse(pResponse);
		}
	}
}

void ngHttpSession::OnData(uint8 *pData, uint32 nData) {

	NG_DEBUG_LOG("ngHttpSession::OnData, len = %d, response=0x%llX", nData, (int64)m_pResponse);
	m_status = SESSION_RECEIVING;

    if (!m_fileBuffered) {
        ngByteBuffer* pBuffer = m_pResponse->GetBuffer();
        pBuffer->AppendBuffer(pData, nData);
    }
	if (m_pListener) {
		m_pListener->OnData(pData, nData);
	}
}

void ngHttpSession::OnErrorStop(NG_CONNECTION_ERROR nErrorCode)
{
	NG_DEBUG_LOG("ngHttpSession::OnErrorStop %d", nErrorCode);
	
	if (m_pResponse != NULL) {
		ngByteBuffer* pBuffer = m_pResponse->GetBuffer();
		pBuffer->DeleteBuffer();
	}

	m_nErrorCode = nErrorCode;
	m_status = SESSION_ERROR;
	if (m_pListener) {
		m_pListener->OnErrorStop(nErrorCode);
	}
}

void ngHttpSession::OnFinish() {
	NG_DEBUG_LOG("ngHttpSession::OnFinish");	
	m_status = SESSION_FINISH;
	if (m_pListener) {	
		m_pListener->OnFinish();
	}
}

ngConnectionRequest* ngHttpSession::CreateRequest() {
    return DNEW(ngHttpRequest);
}
