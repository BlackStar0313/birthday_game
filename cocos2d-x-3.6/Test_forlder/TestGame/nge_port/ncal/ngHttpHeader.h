/*
*  ngHttpHeader.h
*  NextGenEngine
*
*  Created by cooler on 10-5-19.
*  Copyright 2010 DreaminGame. All rights reserved.
*
*/

#ifndef __NGHTTPHEADER_H__
#define __NGHTTPHEADER_H__

#define HTTP_CONTENT_TYPE		"Content-Type"
#define HTTP_CONTENT_LENGTH		"Content-Length"
#define HTTP_COOKIE				"Cookie"

#include "../nsl/ngNVHashMap.h"

class ngHttpHeader : public ngNVHashMap
{

};

#endif //__NGHTTPHEADER_H__