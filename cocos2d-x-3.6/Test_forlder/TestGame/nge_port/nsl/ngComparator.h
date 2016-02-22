/*
*  ngComparator.h
*  NextGenEngine
*
*  Created by cooler on 10-5-15.
*  Copyright 2010 DreaminGame. All rights reserved.
*
*/

#ifndef __NGCOMPARATOR_H__
#define __NGCOMPARATOR_H__

#include "../core/NGE_Types.h"

class ngComparator
{
public:
	virtual ~ngComparator() {}

public:
	virtual int32		Compare(void* a, void* b) = 0;
	virtual boolean		Equals(void* a, void* b) { return Compare(a, b) == 0; }
	virtual void		Destroy() = 0;
    
#ifdef DEBUG
    static int incr;
#endif
};


#endif //__NGCOMPARATOR_H__