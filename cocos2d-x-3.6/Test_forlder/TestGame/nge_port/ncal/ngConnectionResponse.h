/*
*  ngConnectionResponse.h
*  NextGenEngine
*
*  Created by cooler on 10-5-19.
*  Copyright 2010 DreaminGame. All rights reserved.
*
*/

#ifndef __NGCONNECTIONRESPONSE_H__
#define __NGCONNECTIONRESPONSE_H__

#include <core/NGE_NSL.h>

class ngConnectionResponse {
public:
	virtual int32 GetContentLength() = 0;
};


#endif //__NGCONNECTIONRESPONSE_H__