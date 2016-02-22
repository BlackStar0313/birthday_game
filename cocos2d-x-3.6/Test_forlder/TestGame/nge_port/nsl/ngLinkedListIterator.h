/*
*  ngLinkedListIterator.h
*  NextGenEngine
*
*  Created by cooler on 10-5-15.
*  Copyright 2010 DreaminGame. All rights reserved.
*
*/

#ifndef __NGLINKEDLISTITERATOR_H__
#define __NGLINKEDLISTITERATOR_H__

#include "./ngListIterator.h"
#include "./ngLinkedList.h"

class ngLinkedListIterator : public ngListIterator
{
	friend class ngLinkedList;
public:
	ngLinkedListIterator(const ngLinkedList* pList, int32 index = 0);
	ngLinkedListIterator(const ngLinkedList* pList, ngLinkedListNode* pNode);
	virtual ~ngLinkedListIterator();
public:
	// ngIterator
	virtual boolean HasNext();
	virtual void* Next();
	virtual void* Get();
	virtual void Destroy();

	// ngListIterator
	virtual int32 Index();
	virtual void  Set(void* pObj);
	virtual int32 InsertBefore(void* pObj);
	virtual int32 InsertAfter(void* pObj);
	virtual void* Remove();

protected:
	ngLinkedList* m_pList;
	ngLinkedListNode* m_pNode;
	int32 m_nIndex;
	int32 m_nListSize;
};

#endif //__NGLINKEDLISTITERATOR_H__