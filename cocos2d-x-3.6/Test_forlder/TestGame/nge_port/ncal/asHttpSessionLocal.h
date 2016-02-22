/*
 *  asHttpSessionLocal.h
 *  pnk
 *
 *  Created by Rui Xie 2015-04-11.
 *  Copyright 2015 Anansi Mobile. All rights reserved.
 *
 */

#ifndef __pnk_asHttpSessionLocal_h__
#define __pnk_asHttpSessionLocal_h__

#include <ncal/ngHttpSession.h>

class ngJsonNode;

class asHttpSessionLocalHandler {
    
public:
    asHttpSessionLocalHandler() {};
    virtual ~asHttpSessionLocalHandler() {};
    
    virtual ngJsonNode* HandleUrl(ngString& url, ngString& request) = 0;
};

/* keep only one copy of local session handler */
static asHttpSessionLocalHandler* sLocalSessionHandler = NULL;

class asHttpSessionLocal : public ngHttpSession {
    
public:
    static ngHttpSession* CreateSession(); /* create self */
    
    static asHttpSessionLocalHandler* GetSessionHandler();
    
public:
    asHttpSessionLocal();
    virtual ~asHttpSessionLocal();
    
protected:
    virtual void RequestInternal(ngStringV2& url);
    virtual void CancelInternal();
};
#endif //__pnk_asHttpSessionLocal_h__