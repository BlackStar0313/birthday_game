//
//  ngHttpClientFactory.cpp
//  lib_nge_project
//
//  Created by chen on 12-12-11.
//  Copyright (c) 2012年 Anansi. All rights reserved.
//

#include <httpclient/ngHttpClientFactory.h>

HttpClientProvider ngHttpClientFactory::sHttpClientProvider = NULL;

ngHttpClient* ngHttpClientFactory::CreateHttpClient()
{
	if (sHttpClientProvider) {
        return sHttpClientProvider();
    }
    
    NGASSERT(0);
    
    return DNEW(ngHttpClient);
}

void ngHttpClientFactory::InitHttpClientProvider(HttpClientProvider provider) {
    sHttpClientProvider = provider;
}
