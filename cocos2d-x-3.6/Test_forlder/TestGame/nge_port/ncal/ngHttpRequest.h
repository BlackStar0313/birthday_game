/*
*  ngHttpRequester.h
*  NextGenEngine
*
*  Created by cooler on 10-5-19.
*  Copyright 2010 DreaminGame. All rights reserved.
*
*/

#ifndef __NGHTTPREQUESTER_H__
#define __NGHTTPREQUESTER_H__

#include "./ngConnectionRequest.h"
#include "./ngHttpHeader.h"
#include "../nsl/ngByteBuffer.h"

class ngHttpRequest
: public ngConnectionRequest {
public:
	ngHttpRequest();
	virtual ~ngHttpRequest();
	
public:
	enum HTTP_METHOD {
		HTTP_GET = 0,
		HTTP_POST,
		HTTP_PUT,
		HTTP_DELETE
	};	
	
public:
	void SetGetMethod() { SetMethod(HTTP_GET); }
	void SetPostMethod() { SetMethod(HTTP_POST); }
	void SetPutMethod() { SetMethod(HTTP_POST); }

	void SetMethod(HTTP_METHOD method) { m_method = method;}
	HTTP_METHOD GetMethod() const { return m_method; }

	void SetContentType(NGCSTR contentType);
	NGCSTR GetContentType();
	
	void SetCookie(NGCSTR cookie);
	NGCSTR GetCookie();
	
	ngHttpHeader* GetHeader() { return &m_header; }
	ngByteBuffer* GetBuffer() { return &m_buffer; }

//protected:
public:
	virtual int32 SetHeader(NGCSTR pKey, NGCSTR pValue);
	virtual NGCSTR GetHeader(NGCSTR pKey);

protected:
	HTTP_METHOD m_method;
	ngHttpHeader m_header;
	ngByteBuffer m_buffer;
};


#endif //__NGHTTPREQUESTER_H__