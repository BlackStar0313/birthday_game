/*
*  ngHashMapIterator.h
*  NextGenEngine
*
*  Created by cooler on 10-5-17.
*  Copyright 2010 DreaminGame. All rights reserved.
*
*/

#ifndef __NGHASHMAPITERATOR_H__
#define __NGHASHMAPITERATOR_H__

#include "./ngHashSetIterator.h"
#include "./ngMap.h"

class ngHashMap;
class ngHashMapIterator : public ngHashSetIterator
{
public:
	ngHashMapIterator(ngHashMap* pMap);
	virtual ~ngHashMapIterator();
	virtual void Destroy();
	
public:
	ngMap::ngEntry* GetEntry() { return (ngMap::ngEntry *) Get(); }
	ngMap::ngEntry* NextEntry() { return (ngMap::ngEntry *) Next(); }
};

#endif //__NGHASHMAPITERATOR_H__