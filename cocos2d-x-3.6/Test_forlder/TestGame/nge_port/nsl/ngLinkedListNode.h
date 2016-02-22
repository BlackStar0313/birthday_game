/*
*  ngLinkedListNode.h
*  NextGenEngine
*
*  Created by cooler on 10-5-15.
*  Copyright 2010 DreaminGame. All rights reserved.
*
*/

#ifndef __NGLINKEDLISTNODE_H__
#define __NGLINKEDLISTNODE_H__

struct ngLinkedListNode
{
	ngLinkedListNode();
	ngLinkedListNode* prev;
	ngLinkedListNode* next;
	void* data;
};

#endif //__NGLINKEDLISTNODE_H__