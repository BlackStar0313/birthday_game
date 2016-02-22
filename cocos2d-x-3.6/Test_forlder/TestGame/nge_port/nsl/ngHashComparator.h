/*
*  ngHashComparator.h
*  NextGenEngine
*
*  Created by cooler on 10-5-17.
*  Copyright 2010 DreaminGame. All rights reserved.
*
*/

#ifndef __NGHASHCOMPARATOR_H__
#define __NGHASHCOMPARATOR_H__

#include "./ngComparator.h"

class ngHashComparator : public ngComparator
{
public:
	virtual uint32 HashCode(void* pObj) = 0;
};

#endif //__NGHASHCOMPARATOR_H__