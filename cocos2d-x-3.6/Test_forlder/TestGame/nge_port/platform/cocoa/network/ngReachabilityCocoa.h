/*
 *  ngReachabilityCocoa.h
 *  template
 *
 *  Created by Yuhui Wang on 7/21/11.
 *  Copyright 2011 Anansi Mobile. All rights reserved.
 *
 */


#ifndef __NGREACHABILITYCOCOA_H__
#define __NGREACHABILITYCOCOA_H__

#include "ngReachability.h"

class ngReachabilityCocoa : public ngReachability
{	
public:
	static ngReachability* GetInstance();
	
	virtual void Start();
	virtual void Stop();
	virtual int32 GetStatus();
	
public:
	ngReachabilityCocoa();
};



#endif //__NGREACHABILITYCOCOA_H__