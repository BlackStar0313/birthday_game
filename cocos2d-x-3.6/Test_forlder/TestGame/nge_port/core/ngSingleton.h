/*
 *  ngSingleton.h
 *  template
 *
 *  Created by Yuhui Wang on 10/22/10.
 *  Copyright 2010 Anansi Mobile. All rights reserved.
 *
 */

#ifndef __NGSINGLETON_H__
#define __NGSINGLETON_H__

#include "./NGE_Types.h"

typedef void (*EXIT_CALLBACK_FN)(void *);

class ngSingleton 
{
public:
	ngSingleton(EXIT_CALLBACK_FN callback, void* pObj, int32 order = 0);
public:
	virtual int32 AddReleaseCallback(EXIT_CALLBACK_FN callback, void* pObj, int32 order);
	static  int32 ReleaseSingletons();
};


#endif //__NGSINGLETON_H__