/*
*  ngConnectionRequester.h
*  NextGenEngine
*
*  Created by cooler on 10-5-19.
*  Copyright 2010 DreaminGame. All rights reserved.
*
*/

#ifndef __NGCONNECTIONREQUESTER_H__
#define __NGCONNECTIONREQUESTER_H__

#include <core/NGE_NSL.h>
#include "./ngConnectionCommon.h"

class ngConnectionRequest {

public:
	void SetURI(NGCSTR uri) { m_uri = uri; }
	NGCSTR GetURI() { return m_uri.GetCString(); }
	void SetTimeout(int32 timeout) {m_timeout = timeout;}
	int32 GetTimeout() const { return m_timeout; }

protected:
	ngConnectionRequest() {}

public:
	virtual ~ngConnectionRequest() {}

protected:
	ngString m_uri;
	int32 m_timeout;
};


#endif //__NGCONNECTIONREQUESTER_H__