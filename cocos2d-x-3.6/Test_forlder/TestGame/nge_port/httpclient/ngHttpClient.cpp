/*
 *  ngHttpClient.cpp
 *  template
 *
 *  Created by Yuhui Wang on 5/06/11.
 *  Copyright 2010 Anansi Mobile. All rights reserved.
 *
 */

#include <httpclient/ngHttpClient.h>
#include <json/ngJsonFactory.h>
#include <ncal/ngHttpSession.h>

#include <ncal/asHttpSessionLocal.h>

#include <ncal/ngHttpRequest.h>
#include <ncal/ngHttpResponse.h>
#include <httpclient/ngHttpClientListener.h>
#include <core/ngDevice.h>

NGCSTR ngHttpClient::m_gameId = "ANANSI_GAME";
int32  ngHttpClient::m_gameVersion = 0;
int32  ngHttpClient::m_platform = 0;
int32  ngHttpClient::m_serverIdx = 0;

ngHttpClient::ngHttpClient()
: m_pSession(NULL)
, m_status(CLIENT_IDLE)
, m_pListener(NULL)
{
}

ngHttpClient::~ngHttpClient()
{
	SetListener(NULL);
}

void ngHttpClient::SetListener(ngHttpClientListener *pListener)
{
	m_pListener = pListener;
}

void ngHttpClient::SetGameId(NGCSTR gameId) 
{
	m_gameId = gameId;
}

void ngHttpClient::SetGameVersion(int32 version)
{
    m_gameVersion = version;
}

void ngHttpClient::SetGamePlatform(int32 platform)
{
    m_platform = platform;
}

void ngHttpClient::SetServerIdx(int32 serverIdx)
{
    m_serverIdx = serverIdx;
}

boolean ngHttpClient::HandleUpdate(int32 deltaMS)
{
	if (m_pSession && m_pSession->GetStatus() == SESSION_FINISH) {
		ngHttpResponse* pResp = (ngHttpResponse *)m_pSession->GetResponse();
		ngByteBuffer* pByteBuffer = pResp->GetBuffer();
		const char* pBuff = (const char*)pByteBuffer->GetBuffer();

		NG_DEBUG_LOG("response buffer: %s\n", pBuff);

		int32 result = m_jsonRoot.Parse(pBuff);
		
		if (result == NG_OK && m_jsonRoot.GetType() == JSON_HASH) {
			ngJsonHash* pHash = (ngJsonHash *)m_jsonRoot.GetNode();
			ngJsonString* pError = (ngJsonString *)pHash->Get("error");
			ngJsonInteger* pTimestamp = (ngJsonInteger *)pHash->Get("timestamp");
			int64 timestamp = pTimestamp->IntValue();
			if (pError && pError->ICompare("0") == 0) {
				boolean parsable = TRUE;
				
				if (parsable == TRUE) {
					int32 rc = ParseResponse(timestamp, pHash->Get("data"));				
					if (rc == NG_TOBECONTINUE) {
						m_status = CLIENT_ACTIVE;
						return FALSE;
					}
					if (m_pListener != NULL && rc == NG_OK) {
						m_pListener->OnReceiveResponse(m_reqType, pHash->Get("data"));						
					}
				}
			}else {				
				ngJsonString* pErrorMessage = (ngJsonString *)pHash->Get("errorMessage");				
				ngJsonNode* pData = NULL;
				if (pHash->ContainsKey("data")) {
					pData = (ngJsonNode *)pHash->Get("data");
				}
                int32 rc = ParseError( pError->GetCString(TRUE), pErrorMessage->GetCString(), pData);
                if( rc != NG_ERROR_FINISH_HANDLE ){
                
                    if (m_pListener != NULL) {
                        m_pListener->OnReceiveError(pError->GetCString(TRUE), pErrorMessage->GetCString(), pData);
                    }else {
                        ShowErrorDialog();
                    }
                }

			}
		} else {
			NG_DEBUG_LOG("[ngHttpClient][Json Root Error] can't parse json");
			if (m_pListener != NULL) {
				m_pListener->OnReceiveError("Can't load response", "", NULL);
			}else {
				ShowErrorDialog();
			}
			
		}
		m_pSession->SetUseless();
		m_status = CLIENT_FINISH;					
	}else if (m_pSession && m_pSession->GetStatus() == SESSION_ERROR) {
		if( m_pListener != NULL) {
			m_pListener->OnNetworkError(m_pSession->GetErrorCode());
		}else{
			ShowErrorDialog();
		}
		m_pSession->SetUseless();
		m_status = CLIENT_FINISH;					
	}
	
	return TRUE;
}

int32 ngHttpClient::ParseResponse(int64 timestamp, void *pData)
{
	NG_DEBUG_LOG("[ngHttpClient] ParseResponse, type=%d", m_reqType);

		
	return NG_OK;
}

int32 ngHttpClient::ParseError( NGCSTR errorCode, NGCSTR errorMessage, void *pData){
    return NG_OK;
}


int32 ngHttpClient::PutURL(ngStringV2& url, ngJsonHash* pReqHash, boolean useLocalServer /* = FALSE */)
{
	m_status = CLIENT_ACTIVE;
	
	NG_DEBUG_LOG("ngHttpClient::PutURL, %s\n", url.GetCString());
	
	ngJsonRoot* pReqBody = CreateReqBody(pReqHash);	
	ngStringV2* pReqStr = pReqBody->ToString();
    
    NG_DEBUG_LOG("req str %s\n", pReqStr->GetCString());
	
    /* 是否使用本地模拟服务器 */
    if (useLocalServer) {
        m_pSession = asHttpSessionLocal::CreateSession();
    } else {
        m_pSession = ngHttpSession::CreateSession();
    }

    /* 启用md5校验 */
    m_pSession->SetMD5Check(TRUE);
    /* 启用数据加密 */
    m_pSession->SetDESEncrypt(TRUE);

	ngHttpRequest* pRequest = (ngHttpRequest* )m_pSession->GetRequest();
	pRequest->SetMethod(ngHttpRequest::HTTP_PUT);
	ngByteBuffer* pByteBuffer = pRequest->GetBuffer();
	pByteBuffer->AppendBuffer(pReqStr->GetBuffer(), pReqStr->GetLength());
    
	m_pSession->Connect(url);	
		
	SAFE_DELETE(pReqBody);
	
	return NG_OK;
}

ngJsonRoot* ngHttpClient::CreateReqBody(ngJsonHash* bodyHash)
{
	ngJsonRoot* requestJson = DNEW(ngJsonRoot);
	requestJson->SetType(JSON_HASH);
	
	ngJsonHash* pHash = (ngJsonHash *)requestJson->GetNode();
	pHash->Set("header", GetHeader());
	pHash->Set("command", bodyHash);
	
	return requestJson;
	
}

ngJsonHash* ngHttpClient::GetHeader()
{
	ngJsonHash* pReqHeader = DNEW ngJsonHash();
	pReqHeader->Set("deviceFamily", ngDevice::GetInstance()->GetDeviceFamily());
	pReqHeader->Set("deviceType", ngDevice::GetInstance()->GetDeviceType());
	pReqHeader->Set("deviceVersion", ngDevice::GetInstance()->GetDeviceVersion());
	pReqHeader->Set("deviceId", ngDevice::GetInstance()->GetDeviceID());
	pReqHeader->Set("locale", ngDevice::GetInstance()->CurrentLocaleLanguage());
	return pReqHeader;
}

void ngHttpClient::ShowErrorDialog()
{
	NG_DEBUG_LOG("[ngHttpClient] show error dialog");
#if 0 //port to cocs2dx
	ngAlertDialog* pAlert = ngAlertDialog::GetInstance();
	pAlert->SetTitle("Error when communicating with server");
	pAlert->SetContent("We are sorry, but we got an error when connecting to server, please come back again later");
	pAlert->Show("OK", NULL);
#endif
}
