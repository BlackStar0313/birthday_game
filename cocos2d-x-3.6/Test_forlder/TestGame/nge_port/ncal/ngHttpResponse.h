/*
*  ngHttpResponse.h
*  NextGenEngine
*
*  Created by cooler on 10-5-19.
*  Copyright 2010 DreaminGame. All rights reserved.
*
*/

#ifndef __NGHTTPRESPONSE_H__
#define __NGHTTPRESPONSE_H__

#include "./ngConnectionResponse.h"
#include "../nsl/ngByteBuffer.h"
#include "./ngHttpHeader.h"

/*!
 @class ngHttpResponse
 @abstract http响应类，由ngHttpSession
 @discussion
 */
class ngHttpResponse
: public ngConnectionResponse {
public:
	ngHttpResponse();
	virtual ~ngHttpResponse();
	
public:
	void SetStatusCode(int32 code) {m_nStatusCode = code;}
	int32 GetStatusCode() { return m_nStatusCode; }

	void SetStatusText(NGCSTR text) { m_strStatusText = text; }
//	NGCSTR GetStatusText() { return m_strStatusText.ToCharPtr(); }

	void SetContentType(NGCSTR contentType);
	NGCSTR GetContentType();

	void SetContentLength(int32 len);
	virtual int32 GetContentLength();

	void SetCookie(NGCSTR cookie);
	NGCSTR GetCookie(); 
	
	NGCSTR GetSuggestedFilename();
	void SetSuggestedFilename(NGCSTR filename);
	
	ngHttpHeader* GetHeader() { return &m_header; }	
	ngByteBuffer* GetBuffer() { return &m_buffer; }

protected:
	int32 m_nStatusCode;
	ngStringV2 m_strStatusText;
	ngHttpHeader m_header;
	ngStringV2 m_filename;
	ngByteBuffer m_buffer;
};

#endif //__NGHTTPRESPONSE_H__