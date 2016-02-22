/*
 *  asHttpSessionLocal.cpp
 *  pnk
 *
 *  Created by Rui Xie 2015-04-11.
 *  Copyright 2015 Anansi Mobile. All rights reserved.
 *
 */
#include <ncal/asHttpSessionLocal.h>
#include <ncal/ngHttpRequest.h>
#include <json/ngJsonRoot.h>

ngHttpSession* asHttpSessionLocal::CreateSession()
{
    ngHttpSession* pSession = DNEW asHttpSessionLocal();
    return pSession;
}

asHttpSessionLocal::asHttpSessionLocal()
{
    
}

asHttpSessionLocal::~asHttpSessionLocal()
{
    
}

void asHttpSessionLocal::RequestInternal(ngStringV2 &url)
{
    /*
     1. create response
     2. create response json data with local session handler result
     3. fill data
     4. finish response
     */
    
    // 1
    ngHttpResponse* pResponse = DNEW(ngHttpResponse);
    this->OnResponse(pResponse);
    
    // 2
    ngByteBuffer* pBuffer = ((ngHttpRequest*)GetRequest())->GetBuffer();
    ngString request;
    request.ngByteBuffer::AppendBuffer(pBuffer->GetBuffer(), pBuffer->GetLimit());
    ngJsonNode* reponse = GetSessionHandler()->HandleUrl(url, request);
    
    // 3
    ngString buff;
    reponse->ToString(buff);
    SAFE_DELETE(reponse);
    
    this->OnData(buff.GetBuffer(), buff.GetLength());

    // 4
    this->OnFinish();
}

void asHttpSessionLocal::CancelInternal()
{
    
}