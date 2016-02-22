/*
 *  ngHttpHandler.h
 *  template
 *
 *  Created by Yuhui Wang on 5/06/11.
 *  Copyright 2010 Anansi Mobile. All rights reserved.
 *
 */

#ifndef __NGHTTPHANDLER_H__
#define __NGHTTPHANDLER_H__

#include "./ngHttpClient.h"
#include "./ngHttpClientListener.h"
#include "./ngHttpQueue.h"

class ngHttpHandler : public ngHttpClientListener
{
public:
	ngHttpHandler();
	virtual ~ngHttpHandler();
	
	ngHttpClient* GetHttpClient();
	
public:
	virtual void OnReceiveResponse(int32 messageId, void* pData) {}
	virtual void OnReceiveError(NGCSTR errorCode, NGCSTR errorMessage, void *pData = NULL);
	virtual void OnNetworkError(int32 nErrorCode);
	
protected:
	void ShowErrorDialog(NGCSTR errorCode, NGCSTR errorMessage);
};

#endif //__NGHTTPHANDLER_H__