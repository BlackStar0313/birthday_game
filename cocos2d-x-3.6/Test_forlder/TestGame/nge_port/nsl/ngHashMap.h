/*
*  ngHashMap.h
*  NextGenEngine
*
*  Created by cooler on 10-5-17.
*  Copyright 2010 DreaminGame. All rights reserved.
*
*/

#ifndef __NGHASHMAP_H__
#define __NGHASHMAP_H__

#include "./ngMap.h"
#include "../core/NGE_Defs.h"

class ngHashSet;
class ngCollection;
class ngHashComparator;

class ngHashMap : public ngMap
{
	friend class ngHashMapIterator;
public:
	ngHashMap();
	virtual ~ngHashMap();
public:
	virtual void Initialize(ngHashComparator* pComp = NULL);
	virtual int32 Put(void* key, void* value);
	virtual void* Get(void* key);
	virtual void*  Remove(void* key);
	virtual void  RemoveAll();
	virtual ngIterator* Iterator();
	virtual boolean ContainsKey(void* pKey);
	virtual boolean ContainsValue(void* pValue, ngComparator* pComp);
	virtual ngCollection* Keys();
	virtual ngHashSet* GetEntrySet() { return m_pSet; }
	virtual uint32 Size();

protected:
	ngHashSet* m_pSet;
	ngCollection* m_pKeys;
};

#endif //__NGHASHMAP_H__