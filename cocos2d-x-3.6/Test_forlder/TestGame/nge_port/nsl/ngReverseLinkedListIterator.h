/*
 *  ngReverseLinkedListIterator.h
 *  Next Gen Engine
 *
 *  Created by Yuhui Wang on 7/4/10.
 *  Copyright 2010 DreaminGame. All rights reserved.
 *
 */


#ifndef __NGREVERSELINKEDLISTITERATOR_H__
#define __NGREVERSELINKEDLISTITERATOR_H__

#include "./ngListIterator.h"
#include "./ngLinkedList.h"

class ngReverseLinkedListIterator : public ngListIterator
{
	friend class ngLinkedList;
public:
	ngReverseLinkedListIterator(const ngLinkedList* pList);
	ngReverseLinkedListIterator(const ngLinkedList* pList, ngLinkedListNode* pNode);
	
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

#endif //__NGREVERSELINKEDLISTITERATOR_H__