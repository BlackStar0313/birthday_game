/*
*  ngNOHashMap.h
*  NextGenEngine
*
*  Created by cooler on 10-5-27.
*  Copyright 2010 DreaminGame. All rights reserved.
*
*/

#ifndef __NGNOHASHMAP_H__
#define __NGNOHASHMAP_H__

#include "./ngHashComparator.h"
#include "./ngHashMap.h"
#include "./ngStringV2.h"

class ngNOHashComparator : public ngHashComparator
{
	virtual int32		Compare(void* a, void* b);
	virtual void		Destroy() {}
	virtual uint32		HashCode(void* pObj);
};

class ngNOHashMap : public ngHashMap
{
	friend class ngHashMapIterator;
public:
	ngNOHashMap();
	virtual ~ngNOHashMap();
public:
	virtual void Initialize(ngHashComparator* pComp = NULL);
	virtual int32 Put(ngStringV2& key, void* val);
	virtual int32 Put(const char* key, void* val);
	virtual void* Get(ngStringV2& key);
	virtual void* Get(const char* key);
	virtual void* Remove(ngStringV2& key);
	virtual void* Remove(const char* key);
	virtual void RemoveAll();
	virtual boolean ContainsKey(ngStringV2& key);
	virtual boolean ContainsKey(const char* key);
	
protected:
	ngHashComparator* m_pComparator;
};

#endif //__NGNOHASHMAP_H__