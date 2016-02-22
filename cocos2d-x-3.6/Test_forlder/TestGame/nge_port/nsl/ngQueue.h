/*
 *  ngQueue.h
 *  Next Gen Engine
 *
 *  Created by Yuhui Wang on 6/25/10.
 *  Copyright 2010 DreaminGame. All rights reserved.
 *
 */

#ifndef __NGQUEUE_H__
#define __NGQUEUE_H__

#include "./ngLinkedList.h"

class ngQueue : public ngLinkedList
{
public:
	ngQueue();
	virtual ~ngQueue();
	
public:
	virtual boolean IsEmpty() const;
	virtual int32 Offer(void* item);
	virtual void* Poll();
	virtual void* Peek();
};


#endif //__NGQUEUE_H__