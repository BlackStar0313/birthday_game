/*
 *  ngHttpClientFactory.h
 *  template
 *
 *  Created by Yuhui Wang on 5/6/11.
 *  Copyright 2011 Anansi Mobile. All rights reserved.
 *
 */

#ifndef __NGHTTPCLIENTFACTORY_H__
#define __NGHTTPCLIENTFACTORY_H__

#include "./ngHttpClient.h"

typedef ngHttpClient* (*HttpClientProvider)(void);

class ngHttpClientFactory 
{
public:
	static ngHttpClient* CreateHttpClient();
    static void InitHttpClientProvider(HttpClientProvider provider);
    
private:
    static HttpClientProvider sHttpClientProvider;
};


#endif //__NGHTTPCLIENTFACTORY_H__