/*
 *  ngHttpSessionAndroid.cpp
 *  template
 *
 *  Created by Yuhui Wang on 10/8/10.
 *  Modified by chen on 2012-11-1.
 *  Copyright 2010 Anansi. All rights reserved.
 *
 */

#include "ngHttpSessionAndroid.h"
#include <ncal/ngHttpRequest.h>
#include <ncal/ngHttpResponse.h>
#include <ncal/ngHttpHeader.h>
#include <android/network/curl/ngCURL.h>

#include <android/ngStringHelper.h>
#include <utils/md5.h>

#pragma mark -

#define HTTP_REQUEST_MD5_SALT   "-idodo"

ngHttpSession* ngHttpSession::CreateSession() {
	ngHttpSession* pSession = DNEW(ngHttpSessionAndroid);
	return pSession;
}

ngHttpSessionAndroid::ngHttpSessionAndroid()
: m_pEncryptData(NULL) {

}

ngHttpSessionAndroid::~ngHttpSessionAndroid() {
    SAFE_DELETE(m_pEncryptData);
}

void ngHttpSessionAndroid::RequestInternal(ngStringV2& url) {

	// set request
	const char* pUrl = url.GetCString();
	
	NG_DEBUG_LOG("request internal %s", pUrl);

    // 1. method
	ngHttpRequest* pRequest = (ngHttpRequest *)m_pRequest;
    ngHttpRequest::HTTP_METHOD method = pRequest->GetMethod();
	
    ngCURL* pCURL = DNEW(ngCURL)(this);
    switch (method) {
		case ngHttpRequest::HTTP_GET:
            pCURL->AsyncGet(url);	
			break;
		case ngHttpRequest::HTTP_POST:
            pCURL->AsyncPost(url);
			break;
		case ngHttpRequest::HTTP_PUT:
            pCURL->AsyncPut(url);	
			break;
		case ngHttpRequest::HTTP_DELETE:
            NGASSERT(0);//no implement
			break;
		default:
            pCURL->AsyncGet(url);	
			break;
    }
}

void ngHttpSessionAndroid::CancelInternal() {
	
}

static ngStringV2* MakeHeader(NGCSTR key, NGCSTR value) {
    ngStringV2* header = DNEW(ngStringV2);
    header->Format("%s: %s", key, value);
    return header;
}

#pragma mark - ngCURLListener

void ngHttpSessionAndroid::MakeHeaderChunk(ngStringList& chunk) {
    
	ngHttpRequest* pRequest = (ngHttpRequest *)m_pRequest;

    chunk.Add(MakeHeader("Expect", ""));
    
    // user agent
    chunk.Add(MakeHeader("User-Agent", "Anansi/1.0"));
	
	ngHashMapIterator* pIterator = (ngHashMapIterator *)pRequest->GetHeader()->Iterator();
	while (pIterator->HasNext()) {
		ngHashMap::ngEntry* pEntry = pIterator->NextEntry();
		const char* pKey = (const char*)pEntry->GetKey();
		const char* pValue = (const char*)pEntry->GetValue();

        chunk.Add(MakeHeader(pKey, pValue));
	}
	pIterator->Destroy(); 

    
    uint8* pData = NULL;
    int32 dataSize = 0;
    if (IsDESEncrypt()) {
        if (m_pEncryptData) {
            pData = m_pEncryptData->GetBuffer();
            dataSize = m_pEncryptData->GetLimit();
        }
    } else {
        ngHttpRequest::HTTP_METHOD pMethod = pRequest->GetMethod();
        if (pMethod == ngHttpRequest::HTTP_POST || pMethod == ngHttpRequest::HTTP_PUT) {
            ngByteBuffer* pPostBuffer = pRequest->GetBuffer();
            pData = pPostBuffer->GetBuffer();
            dataSize = pPostBuffer->GetLimit();
        }
    }
    
    if (pData == NULL) return;

#if MD5_CHKSUM
		char digest[33] = {0,};
#if 0
    //ngStringV2::Format() have a bug if string len larger than 1024.
		ngStringV2 md5Str;
		md5Str.Format("%s%s", pData, HTTP_REQUEST_MD5_SALT);	//add salt.
        int32 ret = toMD5WithLen(digest, (NGCSTR)md5Str.GetBuffer(), md5Str.GetLength());
#else
        ngByteBuffer md5SrcBuffer;
        md5SrcBuffer.AppendBuffer((uint8*)pData, dataSize);
        md5SrcBuffer.AppendBuffer((uint8*)HTTP_REQUEST_MD5_SALT, strlen(HTTP_REQUEST_MD5_SALT));
		int32 ret = toMD5WithLen(digest, (NGCSTR)md5SrcBuffer.GetBuffer(), md5SrcBuffer.GetLimit());
#endif
		if (ret == 0) {
            ngStringV2* chunkStr = DNEW(ngStringV2);
			chunkStr->Format("%s: %s", "X-Game-Checksum", digest);
			chunk.Add(chunkStr);
            
			NG_DEBUG_LOG("md5sum: %s", chunkStr->GetCString());
		}
#endif
}

uint8* ngHttpSessionAndroid::GetHeaderData() {
	ngHttpRequest* pRequest = (ngHttpRequest *)m_pRequest;
	
	ngHttpRequest::HTTP_METHOD pMethod = pRequest->GetMethod();
	if (pMethod == ngHttpRequest::HTTP_POST || pMethod == ngHttpRequest::HTTP_PUT) {
		ngByteBuffer* pPostBuffer = pRequest->GetBuffer();

        if (IsDESEncrypt()) {
            uint8* pPostBytes = pPostBuffer->GetBuffer();
            ngStringV2* encryptData = ngStringHelper::encryptXOR((NGCSTR)pPostBytes, XORKEY);
            m_pEncryptData = DNEW(ngByteBuffer);
            m_pEncryptData->AppendBuffer(encryptData->GetBuffer(), encryptData->GetLength());
            SAFE_DELETE(encryptData);
            return m_pEncryptData->GetBuffer();
        } else {
            return pPostBuffer->GetBuffer();
        }
        
	}

    return NULL;
}

int32 ngHttpSessionAndroid::GetHeaderDataSize() {
   //this call should be after GetHeaderData();

    if (IsDESEncrypt()) {
        if (m_pEncryptData) {
            NG_DEBUG_LOG("[curl] GetHeaderDataSize: %d", m_pEncryptData->GetLimit());
            return m_pEncryptData->GetLimit();
        }
    } else {
        ngHttpRequest* pRequest = (ngHttpRequest *)m_pRequest;
        ngHttpRequest::HTTP_METHOD pMethod = pRequest->GetMethod();
        if (pMethod == ngHttpRequest::HTTP_POST || pMethod == ngHttpRequest::HTTP_PUT) {
            ngByteBuffer* pPostBuffer = pRequest->GetBuffer();
            return pPostBuffer->GetLimit();
        }
    }
    
    return 0;
}

boolean ngHttpSessionAndroid::IsResponseDone() {
    return GetResponse() != NULL;
}


void ngHttpSessionAndroid::OnCURLResponse(ngHttpResponse* response) {
    OnResponse(response);
}

void ngHttpSessionAndroid::OnCURLData(uint8* pData, uint32 nData) {
    OnData(pData, nData);
}

void ngHttpSessionAndroid::OnCURLFinish() {
    NG_DEBUG_LOG("[curl] request finished!~\n");
    /* notify request finish here. */
    //<<
	ngHttpRequest* pRequest = (ngHttpRequest *)m_pRequest;
	ngHttpRequest::HTTP_METHOD pMethod = pRequest->GetMethod();
	if (pMethod == ngHttpRequest::HTTP_POST || pMethod == ngHttpRequest::HTTP_PUT) {
        if (IsDESEncrypt()) {
            ngByteBuffer* pByteBuffer = ((ngHttpResponse*)m_pResponse)->GetBuffer();
            uint8* buffer = pByteBuffer->GetBuffer();
            
            ngStringV2* decryptData = ngStringHelper::decryptXOR((const char*)buffer, XORKEY);
            pByteBuffer->CopyBuffer(decryptData->GetBuffer(), decryptData->GetLimit());
            SAFE_DELETE(decryptData);
            NG_DEBUG_LOG("didFinishDecoding");
        }
    }
    
    OnFinish();
    //>>
    
}

void ngHttpSessionAndroid::OnCURLError(int32 errCode, NGCSTR errMsg) {
    NG_DEBUG_LOG("[curl] error: errorCode: %d!~", errCode);
    OnErrorStop(NGCONN_UNKNOWN);    //here temp use NGCONN_UNKNOWN. [zhen.chen]
}

