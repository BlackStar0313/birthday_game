/*
 *  ngHttpResponse.h
 *  NextGenEngine
 *
 *  Created by cooler on 10-5-19.
 *  Copyright 2010 DreaminGame. All rights reserved.
 *
 */

#include <ncal/ngHttpResponse.h>
#include <ncal/ngHttpHeader.h>

ngHttpResponse::ngHttpResponse() {
	// TODO: comparator if need
	m_header.Initialize();
}

ngHttpResponse::~ngHttpResponse() {
}

void ngHttpResponse::SetContentType(NGCSTR contentType) {
	m_header.Put(HTTP_CONTENT_TYPE, contentType);
}

NGCSTR ngHttpResponse::GetContentType() {
	return m_header.Get(HTTP_CONTENT_TYPE);
}

void ngHttpResponse::SetCookie(NGCSTR cookie) {
	m_header.Put(HTTP_COOKIE, cookie);
}

NGCSTR ngHttpResponse::GetCookie() {
	return m_header.Get(HTTP_COOKIE);
}

void ngHttpResponse::SetContentLength(int32 len) {
	char buff[16] = {'\0'};
	sprintf(buff, "%hd", len);
	m_header.Put(HTTP_CONTENT_LENGTH, (NGCSTR)buff);
}

int32 ngHttpResponse::GetContentLength() {
	int32 len = 0;
	NGCSTR strLen = m_header.Get(HTTP_CONTENT_LENGTH);
	if (strLen != NULL) {
		sscanf(strLen, "%d", &len);
	}
	return len;
}

NGCSTR ngHttpResponse::GetSuggestedFilename() {
	return m_filename;
}

void ngHttpResponse::SetSuggestedFilename(NGCSTR filename) {
	m_filename = filename;
}