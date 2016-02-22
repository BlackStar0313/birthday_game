/*
*  ngListIterator.h
*  NextGenEngine
*
*  Created by cooler on 10-5-15.
*  Copyright 2010 DreaminGame. All rights reserved.
*
*/

#ifndef __NGLISTITERATOR_H__
#define __NGLISTITERATOR_H__

#include "./ngIterator.h"

class ngListIterator : public ngIterator
{
public:
	virtual int32 Index() = 0;
	virtual void  Set(void* pObj) = 0;
	virtual int32 InsertBefore(void* pObj) = 0;
	virtual int32 InsertAfter(void* pObj) = 0;
	virtual void* Remove() = 0;
};


#endif //__NGLISTITERATOR_H__