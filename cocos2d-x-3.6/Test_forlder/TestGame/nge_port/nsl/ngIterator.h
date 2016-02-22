/*
*  ngIterator.h
*  NextGenEngine
*
*  Created by cooler on 10-5-15.
*  Copyright 2010 DreaminGame. All rights reserved.
*
*/

#ifndef __NGITERATOR_H__
#define __NGITERATOR_H__

#include "../core/NGE_Types.h"

class ngIterator
{
public:
	virtual ~ngIterator() {}

public:
	virtual boolean HasNext() = 0;
	virtual void* Next() = 0;
	virtual void* Get() = 0;
	virtual void Destroy() = 0;
};

#endif //__NGITERATOR_H__