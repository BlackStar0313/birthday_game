/*
 *  ngQueue.cpp
 *  template
 *
 *  Created by Yuhui Wang on 6/25/10.
 *  Copyright 2010 DreaminGame. All rights reserved.
 *
 */


#include <nsl/ngQueue.h>

ngQueue::ngQueue()
{
}

ngQueue::~ngQueue()
{
}

boolean ngQueue::IsEmpty() const
{
	return ngLinkedList::IsEmpty();
}

int32 ngQueue::Offer(void *item)
{
	return InsertAtEnd(item);
}

void* ngQueue::Poll()
{
	void* pItem = First();
	if (pItem != NULL) {
		Remove(pItem);
		return pItem;
	}
	return NULL;
}

void* ngQueue::Peek()
{
	return First();
}