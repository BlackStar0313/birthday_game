/*
*  ngHashSet.h
*  NextGenEngine
*
*  Created by cooler on 10-5-17.
*  Copyright 2010 DreaminGame. All rights reserved.
*
*/

#ifndef __NGHASHSET_H__
#define __NGHASHSET_H__

#include "./ngSet.h"

class ngBucket;
class ngHashComparator;
class ngHashSet : public ngSet
{
	friend class ngHashSetIterator;
public:
	ngHashSet();
	~ngHashSet();

public:
	virtual void Initialize(ngHashComparator* pComp = NULL);
	virtual int32 Add(void* pObj, boolean* bAdded = NULL);
	virtual void* Remove(void* pObj, boolean* pRemoved = NULL);
	virtual void* Contains(void* pObj);
	virtual ngIterator* Iterator();
	virtual boolean IsEmpty();
	virtual uint32 Size();
	virtual boolean Clear();

protected:
	static uint32 BucketNo(void* pObj, int32 bucketCount, ngHashComparator* pComparator = NULL);
	int32 ExpandBuckets();
	int32 ShrinkBuckets();

protected:
	ngIterator* m_pIterator;
	ngBucket** m_pBuckets;
	int32 m_nBucketsCount;		// count for buckets
	int32 m_nSize;				// total elements
	ngHashComparator* m_pComparator;
	
};

#endif //__NGHASHSET_H__
