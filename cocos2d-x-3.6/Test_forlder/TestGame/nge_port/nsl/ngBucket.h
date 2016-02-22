/*
*  ngBucket.h
*  NextGenEngine
*
*  Created by cooler on 10-5-17.
*  Copyright 2010 DreaminGame. All rights reserved.
*
*/

#ifndef __NGBUCKET_H__
#define __NGBUCKET_H__

#include "../core/NGE_Types.h"

struct ngBucketNode
{
	void* data;
	ngBucketNode* next;
};

class ngHashComparator;
class ngHashSet;
class ngBucket
{
	friend class ngHashSet;
	friend class ngHashSetIterator;
public:
	static ngBucket* CreateBucket(ngHashComparator* pHashComp = NULL);
	virtual ~ngBucket();
	
public:
	virtual int32 Add(void* pObj, boolean* bAdded = NULL);
	virtual void* Remove(void* pObj);
	virtual void* Contains(void* pObj);
	

protected:
	ngBucket(ngHashComparator* pHashComp);

protected:
	ngBucketNode* m_pHead;
	int32 m_nSize;
	ngHashComparator* m_pComparator;

};

#endif //__NGBUCKET_H__
