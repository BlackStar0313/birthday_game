//
//  ngCURLManager.h
//  pet_native_project_lib
//
//  Created by chen on 12-11-1.
//  Copyright (c) 2012年 Anansi. All rights reserved.
//

#ifndef __NGCURLMANAGER_H__
#define __NGCURLMANAGER_H__

#if defined (NGE_PLATFORM_ANDROID)

#include <nsl/ngQueue.h>
#include <core/ngSingleton.h>

class ngCURL;

/*!
 @class ngCURLManager
 @abstract manager all ngCURL async instance with a queue.
 @discussion
 */
class ngCURLManager : public ngSingleton {
private:
    ngCURLManager();
    ~ngCURLManager();

public:
    void Update(/*int32 deltaMS*/);

private:
    void OfferCURL(ngCURL* pCURL);

private:
    ngQueue m_curls;

#pragma mark - ngSingleton
public:
    static ngCURLManager* GetInstance();
    static void ReleaseInstance(void* pObj);
    
private:
    static ngCURLManager* m_pThis;
    
    friend class ngCURL;
};

#endif

#endif  //__NGCURLMANAGER_H__
