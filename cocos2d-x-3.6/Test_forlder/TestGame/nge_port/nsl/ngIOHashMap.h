/*
 *  ngIOHashMap.h
 *  template
 *
 *  Created by Yuhui Wang on 9/2/10.
 *  Copyright 2010 Anansi Mobile. All rights reserved.
 *
 */

#ifndef __NGIOHASHMAP_H__
#define __NGIOHASHMAP_H__

#include "./ngHashComparator.h"
#include "./ngHashMap.h"
#include "./ngStringV2.h"

class ngIOHashComparator : public ngHashComparator
{
	virtual int32		Compare(void* a, void* b);
	virtual void		Destroy() {}
	virtual uint32		HashCode(void* pObj);
};

class ngIOHashMap : public ngHashMap
{
	friend class ngHashMapIterator;
public:
	ngIOHashMap();
	virtual ~ngIOHashMap();
public:
	virtual void Initialize(ngHashComparator* pComp = NULL);
	virtual int32 Put(int32 key, void* val);
	virtual void* Get(int32 key);
	virtual void* Remove(int32 key);
	virtual void  RemoveAll();
	virtual boolean ContainsKey(int32 key);
	
protected:
	ngHashComparator* m_pComparator;
};

#endif //__NGIOHASHMAP_H__