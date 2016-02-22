/*
 *  ngIOHashMap.cpp
 *  template
 *
 *  Created by Yuhui Wang on 9/2/10.
 *  Copyright 2010 Anansi Mobile. All rights reserved.
 *
 */


#include <nsl/ngIOHashMap.h>
#include <nsl/nslUtils.h>
#include <nsl/ngHashSet.h>
#include <nsl/ngInteger.h>
#include "ngHashMapIterator.h"
#include "ngCollection.h"

int32 ngIOHashComparator::Compare(void* a, void* b)
{
	ngHashMap::ngEntry* p1 = (ngHashMap::ngEntry *)a;
	ngHashMap::ngEntry* p2 = (ngHashMap::ngEntry *)b;
	
	ngInteger* sa = (ngInteger *)p1->GetKey();
	ngInteger* sb = (ngInteger *)p2->GetKey();
	return !(sa->IntValue() == sb->IntValue());
}

uint32 ngIOHashComparator::HashCode(void* pObj)
{
	ngHashMap::ngEntry* p1 = (ngHashMap::ngEntry *)pObj;
	ngInteger* pKey = (ngInteger *)p1->GetKey();
	return pKey->IntValue();
}

ngIOHashMap::ngIOHashMap()
{
	m_pComparator = DNEW(ngIOHashComparator);
	
	Initialize();
}

void ngIOHashMap::Initialize(ngHashComparator* pComp /* = NULL */)
{
	if (pComp == NULL) {
		ngHashMap::Initialize(m_pComparator);	
	}else {
		ngHashMap::Initialize(pComp);
	}
}

ngIOHashMap::~ngIOHashMap()
{
	RemoveAll();
	SAFE_DELETE(m_pComparator);
}

int32 ngIOHashMap::Put(int32 key, void* val)
{
	ngInteger* pKey = DNEW(ngInteger)(key);
	int32 ret = ngHashMap::Put(pKey, val);
	
	return ret;
}


void* ngIOHashMap::Get(int32 key)
{
	ngInteger intKey(key);
	void* ret = ngHashMap::Get(&intKey);
	return ret;
}

void* ngIOHashMap::Remove(int32 key)
{	
	ngInteger intKey(key);
	void* ret = ngHashMap::Remove(&intKey);
	return ret;
}

void ngIOHashMap::RemoveAll()
{
	ngHashMapIterator* pIterator  = (ngHashMapIterator *)Iterator();
	while (pIterator->HasNext()) {
		ngMap::ngEntry* pEntry = (ngMap::ngEntry *)pIterator->NextEntry();
		ngInteger* pKey = (ngInteger *)pEntry->GetKey();
		SAFE_DELETE(pKey);
		SAFE_DELETE(pEntry);
	}
	SAFE_DELETE(pIterator);
	
	m_pSet->Clear();
	m_pKeys->Clear();
}

boolean ngIOHashMap::ContainsKey(int32 key)
{
	ngInteger* pKey = DNEW(ngInteger)(key);	
	boolean ret = ngHashMap::ContainsKey(pKey);
	SAFE_DELETE(pKey);
	return ret;
}