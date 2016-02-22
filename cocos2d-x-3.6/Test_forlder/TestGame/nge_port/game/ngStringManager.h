//
//  ngStringManager.h
//  NextGenEngine
//
//  Created by Chen Zhen on 13-6-20.
//  Copyright (c) 2013年 Anansi. All rights reserved.
//

#ifndef __NGSTRINGMANAGER_H__
#define __NGSTRINGMANAGER_H__

#include "../nsl/ngString.h"
#include "../nsl/ngArrayList.h"
#include "../core/ngSingleton.h"
#include "./ngStringLoader.h"

#define GET_STRING(index)	(ngStringManager::GetInstance()->GetString(index))
#define GET_CSTRING(index)  (ngStringManager::GetInstance()->GetCString(index))

/*!
 @class ngStringManager
 @abstract a wrapper(sub-class) of ngStringLoader, provider a singleton to manager all lang string.
 @discussion
 */

class ngStringManager
: public ngSingleton
, public ngStringLoader
{
public:
	static ngStringManager* GetInstance();
	static void ReleaseInstance(void *pObj);

protected:
	ngStringManager();
	virtual ~ngStringManager();
	static ngStringManager* m_pThis;
};

#endif