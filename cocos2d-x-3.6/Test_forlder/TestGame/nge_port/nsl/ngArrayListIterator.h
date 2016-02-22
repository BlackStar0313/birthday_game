/*
*  ngArrayListIterator.h
*  NextGenEngine
*
*  Created by cooler on 10-5-15.
*  Copyright 2010 DreaminGame. All rights reserved.
*
*/

#ifndef __NGARRAYLISTITERATOR_H__
#define __NGARRAYLISTITERATOR_H__

#include "./ngListIterator.h"
#include "./ngArrayList.h"

class ngArrayListIterator : public ngListIterator
{
	friend class ngArrayList;
public:
	ngArrayListIterator(const ngArrayList* pList, int32 index = 0);

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
	ngArrayList* m_pList;
	int32 m_nIndex;
};

#endif //__NGARRAYLISTITERATOR_H__