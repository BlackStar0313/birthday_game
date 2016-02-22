/*
*  ngHashSetIterator.h
*  NextGenEngine
*
*  Created by cooler on 10-5-17.
*  Copyright 2010 DreaminGame. All rights reserved.
*
*/

#ifndef __NGHASHSETITERATOR_H__
#define __NGHASHSETITERATOR_H__

#include "./ngIterator.h"

class ngHashSet;
struct ngBucketNode;
class ngHashSetIterator : public ngIterator
{
	friend class ngHashSet;

public:
	ngHashSetIterator(const ngHashSet* pSet);

public:
	virtual boolean HasNext();
	virtual void* Next();
	virtual void* Get();
	virtual void Destroy();

private:
	void MoveToNextBucket();

private:
	int32 m_nBucketNo;
	ngBucketNode* m_pNode;
	const ngHashSet* m_pSet;
};

#endif //__NGHASHSETITERATOR_H__
