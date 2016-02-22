/*
*  ngNVHashMap.h
*  NextGenEngine
*
*  Created by cooler on 10-5-17.
*  Copyright 2010 DreaminGame. All rights reserved.
*
*/

#ifndef __NGNVHASHMAP_H__
#define __NGNVHASHMAP_H__

#include "./ngHashComparator.h"
#include "./ngHashMap.h"
#include "./ngString.h"

class ngNVHashComparator : public ngHashComparator
{
	virtual int32		Compare(void* a, void* b);
	virtual void		Destroy() {}
	virtual uint32		HashCode(void* pObj);
};

class ngNVHashMap : public ngHashMap
{
	friend class ngHashMapIterator;
public:
	ngNVHashMap();
	virtual ~ngNVHashMap();
public:
	virtual void Initialize(ngHashComparator* pComp = NULL);
	virtual int32 Put(ngString& key, ngString& val);
	virtual int32 Put(const char* key, const char* val);
    virtual int32 Put(const char* key, int64 val);
	virtual const char* Get(ngString& key);
	virtual const char* Get(const char* key);
	virtual void* Remove(ngString& key);
	virtual void* Remove(const char* key);
	virtual void RemoveAll();
	virtual boolean ContainsKey(ngString& key);
	virtual boolean ContainsKey(const char* key);

protected:
	ngHashComparator* m_pComparator;
};

#endif //__NGHASHMAP_H__