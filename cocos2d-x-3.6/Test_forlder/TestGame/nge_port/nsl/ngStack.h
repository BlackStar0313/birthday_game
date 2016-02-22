/*
 *  ngStack.h
 *  Next Game Engine
 *
 *  Created by Yuhui Wang on 6/25/10.
 *  Copyright 2010 DreaminGame. All rights reserved.
 *
 */


#ifndef __NGSTACK_H__
#define __NGSTACK_H__

#include "./ngLinkedList.h"

class ngStack : public ngLinkedList
{
public:
	ngStack();
	virtual ~ngStack();
	
public:
	virtual boolean IsEmpty() const;
	virtual int32	Push(void *item);
	virtual void*	Pop();
	virtual void*	Peek();
};


#endif //__NGSTACK_H__

