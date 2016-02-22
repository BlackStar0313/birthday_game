/*
 *  ngStack.cpp
 *  template
 *
 *  Created by Yuhui Wang on 6/25/10.
 *  Copyright 2010 Anansi Mobile. All rights reserved.
 *
 */

#include <nsl/ngStack.h>

ngStack::ngStack()
{
}

ngStack::~ngStack()
{
}

boolean ngStack::IsEmpty() const
{
	return ngLinkedList::IsEmpty();
}

int32 ngStack::Push(void *item)
{
	return InsertAtEnd(item);
}

void* ngStack::Pop()
{
	void* pItem = Last();
	if (pItem != NULL) {
		Remove(pItem);
		return pItem;
	}
	return NULL;
}

void* ngStack::Peek()
{
	return Last();
}