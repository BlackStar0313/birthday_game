/*
 *  ngHttpClientListener.h
 *  template
 *
 *  Created by Yuhui Wang on 5/06/11.
 *  Copyright 2010 Anansi Mobile. All rights reserved.
 *
 */

#ifndef __NGHTTPCLIENTLISTENER_H__
#define __NGHTTPCLIENTLISTENER_H__

#include "./ngHttpClient.h"

class ngHttpClientListener 
{
public:
	virtual void OnReceiveResponse(int32 messageId, void* pData) = 0;
	virtual void OnReceiveError(NGCSTR errorCode, NGCSTR errorMessage, void* pData = NULL) = 0;
	virtual void OnNetworkError(int32 nErrorCode) = 0;
};


#endif //__NGHTTPCLIENTLISTENER_H__