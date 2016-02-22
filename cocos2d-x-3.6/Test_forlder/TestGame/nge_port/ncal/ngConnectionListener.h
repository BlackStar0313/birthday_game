/*
*  ngConnectionListener.h
*  NextGenEngine
*
*  Created by cooler on 10-5-19.
*  Copyright 2010 DreaminGame. All rights reserved.
*
*/

#ifndef __NGCONNECTIONLISTENER_H__
#define __NGCONNECTIONLISTENER_H__

#include "./ngConnectionCommon.h"

class ngConnectionResponse;

class ngConnectionListener
{
public:
	virtual void OnConnect() = 0;
	virtual void OnResponse(ngConnectionResponse* pResponse) = 0;
	virtual void OnData(uint8 *pData, uint32 nData) = 0;
	virtual void OnErrorStop(NG_CONNECTION_ERROR nErrorCode) = 0;
	virtual void OnFinish() = 0;
};

#endif //__NGCONNECTIONLISTENER_H__