/*
 *  ngSingleton.cpp
 *  template
 *
 *  Created by Yuhui Wang on 10/22/10.
 *  Copyright 2010 Anansi Mobile. All rights reserved.
 *
 */


#include <core/ngSingleton.h>
#include <core/NGE_Defs.h>
#include <core/NGE_Macros.h>

typedef struct _ExitCallbackNode {
	EXIT_CALLBACK_FN callback;
	void* obj;
	_ExitCallbackNode* next;
} ExitCallbackNode;

static ExitCallbackNode* gExitCallbackNode = NULL;
static ExitCallbackNode* gSecondaryExitCallbackNode = NULL;

ngSingleton::ngSingleton(EXIT_CALLBACK_FN callback, void* pObj, int32 order)
{
	AddReleaseCallback(callback, pObj, order);
}

int32 ngSingleton::AddReleaseCallback(EXIT_CALLBACK_FN callback, void* pObj, int32 order)
{
	if (callback == NULL) {
		return NG_ERROR_FAILTURE;
	}
	ExitCallbackNode* pNode = (ExitCallbackNode *)DMALLOC(sizeof(ExitCallbackNode));
	if (pNode == NULL) {
		return NG_ERROR_FAILTURE;
	}
	pNode->obj = pObj;
	pNode->callback = callback;
	
	if (order == 0) {
		pNode->next = gExitCallbackNode;
		gExitCallbackNode = pNode;
	}else{
		pNode->next = gSecondaryExitCallbackNode;
		gSecondaryExitCallbackNode = pNode;
	}
	
	return NG_OK;
}

int32 ngSingleton::ReleaseSingletons()
{
	ExitCallbackNode* pNode = gExitCallbackNode;
	while (pNode) {
		gExitCallbackNode = gExitCallbackNode->next;
		pNode->callback(pNode->obj);
		SAFE_DELETE(pNode);
		pNode = gExitCallbackNode;
	}
	
	pNode = gSecondaryExitCallbackNode;
	while (pNode) {
		gSecondaryExitCallbackNode = gSecondaryExitCallbackNode->next;
		pNode->callback(pNode->obj);
		SAFE_DELETE(pNode);
		pNode = gSecondaryExitCallbackNode;
	}
	
	return NG_OK;
}