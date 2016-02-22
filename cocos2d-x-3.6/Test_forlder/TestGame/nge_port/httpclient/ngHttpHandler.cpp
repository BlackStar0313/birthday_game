/*
 *  ngHttpHandler.cpp
 *  engine
 *
 *  Created by Yuhui Wang on 5/06/2011.
 *  Copyright 2011 Anansi Mobile. All rights reserved.
 *
 */

#include <httpclient/ngHttpHandler.h>

ngHttpHandler::ngHttpHandler() {
}

ngHttpHandler::~ngHttpHandler() {
	ngHttpQueue::GetInstance()->Deaf(this);
}

ngHttpClient* ngHttpHandler::GetHttpClient() {
	NG_DEBUG_LOG("[ngHttpHandler] %s", __FUNCTION__);
	ngHttpClient* pClient = ngHttpQueue::GetInstance()->Take();
	pClient->SetListener(this);
	return pClient;
}

void ngHttpHandler::OnReceiveError(NGCSTR errorCode, NGCSTR errorMessage, void *pData) {
    NG_DEBUG_LOG("[ngHttpHandler] %s, error[%s]: %s", __FUNCTION__, errorCode, errorMessage);
}

void ngHttpHandler::OnNetworkError(int32 nErrorCode) {
    NG_DEBUG_LOG("[ngHttpHandler] %s, error[%d]", __FUNCTION__, nErrorCode);
}

void ngHttpHandler::ShowErrorDialog(NGCSTR errorCode, NGCSTR errorMessage) {

}