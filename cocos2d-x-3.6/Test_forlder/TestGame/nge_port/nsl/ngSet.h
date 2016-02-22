/*
*  ngSet.h
*  NextGenEngine
*
*  Created by cooler on 10-5-17.
*  Copyright 2010 DreaminGame. All rights reserved.
*
*/

#ifndef __NGSET_H__
#define __NGSET_H__

#include "../core/NGE_Types.h"

class ngIterator;
class ngSet
{
public:
	virtual int32 Add(void* pObj, boolean* bAdded = NULL) = 0;
	virtual void* Remove(void* pObj, boolean* pRemoved = NULL) = 0;
	virtual void* Contains(void* pObj) = 0;
	virtual ngIterator* Iterator() = 0;
	virtual boolean IsEmpty() = 0;
	virtual uint32 Size() = 0;
	virtual boolean Clear() = 0;
};

#endif //__NGSET_H__