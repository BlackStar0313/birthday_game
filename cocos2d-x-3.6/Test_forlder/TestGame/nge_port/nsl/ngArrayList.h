/*
*  ngArrayList.h
*  NextGenEngine
*
*  Created by cooler on 10-5-15.
*  Copyright 2010 DreaminGame. All rights reserved.
*
*/

#ifndef __NGARRAYLIST_H__
#define __NGARRAYLIST_H__

#include "./ngList.h"

class ngArrayListIterator;
class ngArrayList : public ngList
{
	friend class ngArrayListIterator;
public:
	ngArrayList();
	ngArrayList(int32 capacity);
	virtual ~ngArrayList();

public:
	// ngCollection
	virtual int32	Add(void* pObj);
	virtual uint32	Size() const;
	virtual boolean	IsEmpty() const;
	virtual ngIterator* Iterator(int32 index = 0) const;
	virtual boolean	Contains(void* pObj, ngComparator* pComp = NULL);
	virtual void	Clear();
	virtual void*	Remove(void* pObj);

	// ngList
	virtual void* First();
	virtual void* Last();
	virtual void* Get(int32 index);
	virtual boolean  Set(int32 index, void* pObj);

	virtual int32 InsertBefore(uint32 index, void* pObj);
	virtual int32 InsertBefore(void* pObj, void* pRefObj);
	virtual int32 InsertAfter(void* pObj, void* pRefObj);
	virtual int32 InsertAtEnd(void* pObj);
	virtual int32 InsertAtHead(void* pObj);

	virtual void* RemoveByIndex(uint32 index);

	virtual int32 Find(void* pObj, int32 start = 0, ngComparator* pComp = NULL);
	virtual int32 ReverseFind(void* pObj, int32 start = -1, ngComparator* pComp = NULL);

	// if comparator is null, we sort the objects using there address.s
	virtual int32 Sort(ngComparator* pComp = NULL);
	virtual int32 SortedInsert(void* pObj, ngComparator* pComp = NULL);

public:
	void*& operator[] (int32 index) const { return m_pItems[index]; }

protected:
	int32 Resize( int32 capacity );
	int32 InsertBeforeInternal(int32 index, void* pObj);

protected:
	//ngIterator* m_pIterator;
	void** m_pItems;
	int32 m_nSize;
	int32 m_nCapacity;
};

#endif //__NGARRAYLIST_H__