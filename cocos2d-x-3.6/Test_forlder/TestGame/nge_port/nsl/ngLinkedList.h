/*
*  ngLinkedList.h
*  NextGenEngine
*
*  Created by cooler on 10-5-15.
*  Copyright 2010 DreaminGame. All rights reserved.
*
*/

#ifndef __NGLINKEDLIST_H__
#define __NGLINKEDLIST_H__

#include "./ngList.h"

#define LINKEDLIST_COPY(tgt, src)                       \
    do {                                                \
        (tgt).Clear();                                  \
        ngIterator* _itrCopy = (src).Iterator();        \
        while (_itrCopy->HasNext()) {                   \
            (tgt).Add(_itrCopy->Next());                \
        }                                               \
        _itrCopy->Destroy();                            \
    } while(0);


struct ngLinkedListNode;

class ngLinkedList : public ngList
{
	friend class ngLinkedListIterator;
	friend class ngReverseLinkedListIterator;
public:
	ngLinkedList();
    /*explicit */ngLinkedList(const ngLinkedList& src);
	virtual ~ngLinkedList();

public:
	// ngCollection
	virtual int32	Add(void* pObj);
	virtual uint32	Size() const;
	virtual boolean	IsEmpty() const;
	virtual ngIterator* Iterator(int32 index = 0) const;
	virtual ngIterator* ReverseIterator() const;
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
    virtual int32 QSort(ngComparator* pComp = NULL);

protected:
	int32 InsertBeforeInternal(ngLinkedListNode* pNode, void* pObj);
	int32 InsertAfterInternal(ngLinkedListNode* pNode, void* pObj);
	ngLinkedListNode* RemoveInternal(ngLinkedListNode* pNode, void** ppObj = NULL);
	ngLinkedListNode* MoveTo(int32 offset);
    
    void Swap(int32 i , int32 j);
    int32 Partition(int32 begin, int32 end, ngComparator* pComp = NULL);
    void QSort(int32 begin, int32 end, ngComparator* pComp = NULL);

protected:
//	ngIterator* m_pIterator;
	ngLinkedListNode* m_pHead;
	ngLinkedListNode* m_pTail;
	uint32 m_nSize;
};


#endif //__NGLINKEDLIST_H__