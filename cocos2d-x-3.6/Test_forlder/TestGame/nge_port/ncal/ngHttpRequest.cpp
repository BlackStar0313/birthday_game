/*
 *  ngHttpRequester.h
 *  NextGenEngine
 *
 *  Created by cooler on 10-5-19.
 *  Copyright 2010 DreaminGame. All rights reserved.
 *
 */

#include <ncal/ngHttpRequest.h>

ngHttpRequest::ngHttpRequest()
: m_method(HTTP_GET) {
	// TODO: comparator here if need.
	m_header.Initialize();
}

ngHttpRequest::~ngHttpRequest() {

}

int32 ngHttpRequest::SetHeader(NGCSTR pKey, NGCSTR pValue) {
	m_header.Put(pKey,pValue);
	return NG_OK;
}

NGCSTR ngHttpRequest::GetHeader(NGCSTR pKey) {
	return m_header.Get(pKey); 
}

void ngHttpRequest::SetContentType(NGCSTR contentType) {
	SetHeader(HTTP_CONTENT_TYPE, contentType);
}

NGCSTR ngHttpRequest::GetContentType() {
	return GetHeader(HTTP_CONTENT_TYPE);
}

void ngHttpRequest::SetCookie(NGCSTR cookie) {
	SetHeader(HTTP_COOKIE, cookie);
}

NGCSTR ngHttpRequest::GetCookie() {
	return GetHeader(HTTP_COOKIE);
}
