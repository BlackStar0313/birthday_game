/*
*  ngCollection.h
*  NextGenEngine
*
*  Created by cooler on 10-5-15.
*  Copyright 2010 DreaminGame. All rights reserved.
*
*/


#ifndef __NG_COLLECTION_H__
#define __NG_COLLECTION_H__

#include "./ngIterator.h"

class ngComparator;

class ngCollection
{
public:
	ngCollection() {}
	virtual ~ngCollection() {}

public:
	virtual int32	Add(void* pObj) = 0;
	virtual uint32	Size() const = 0;
	virtual boolean	IsEmpty() const = 0;
	virtual ngIterator* Iterator(int32 index = 0) const = 0;
	virtual boolean	Contains(void* pObj, ngComparator* pComp = NULL) = 0;
	virtual void	Clear() = 0;
	virtual void*	Remove(void* pObj) = 0;

};

#endif //__NG_COLLECTION_H__
