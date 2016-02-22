/*
*  ngList.h
*  NextGenEngine
*
*  Created by cooler on 10-5-15.
*  Copyright 2010 DreaminGame. All rights reserved.
*
*/

#ifndef __NGLIST_H__
#define __NGLIST_H__

#include "./ngCollection.h"

class ngComparator;

class ngList : public ngCollection
{
public:
	ngList() : ngCollection() {}
	virtual ~ngList() {}

public:
	virtual void* First() = 0;
	virtual void* Last() = 0;
	virtual void* Get(int32 index) = 0;
	virtual boolean  Set(int32 index, void* pObj) = 0;

	virtual int32 InsertBefore(uint32 index, void* pObj) = 0;
	virtual int32 InsertBefore(void* pObj, void* pRefObj) = 0;
	virtual int32 InsertAfter(void* pObj, void* pRefObj) = 0;
	virtual int32 InsertAtEnd(void* pObj) = 0;
	virtual int32 InsertAtHead(void* pObj) = 0;
	
	virtual void* RemoveByIndex(uint32 index) = 0;

	virtual int32 Find(void* pObj, int32 start = 0, ngComparator* pComp = NULL) = 0;
	virtual int32 ReverseFind(void* pObj, int32 start = -1, ngComparator* pComp = NULL) = 0;

	// if comparator is null, we sort the objects using there address.s
	virtual int32 Sort(ngComparator* pComp = NULL) = 0;
	virtual int32 SortedInsert(void* pObj, ngComparator* pComp = NULL) = 0;

};
#endif //__NGLIST_H__
