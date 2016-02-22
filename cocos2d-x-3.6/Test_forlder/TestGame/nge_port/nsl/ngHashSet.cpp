#include <nsl/ngHashSet.h>
#include <nsl/ngHashSetIterator.h>
#include <nsl/ngHashComparator.h>
#include <nsl/ngBucket.h>
#include <core/NGE_Macros.h>

#define BUCKET_MAX_HEIGHT 4

static const int32 BUCKET_SIZE_LIST[] = {
	31, 97, 251, 499, 911, 2089, 4133, 8887
};

uint32 ngHashSet::BucketNo(void* pObj, int32 bucketCount, ngHashComparator* pComparator)
{
	NGASSERT(bucketCount > 0);
	if (pComparator == NULL) {
//		return (uint32)pObj % bucketCount;
        return 0;
	}else{
		return (uint32)pComparator->HashCode(pObj) % bucketCount;
	}
}

ngHashSet::ngHashSet()
: m_nBucketsCount(0)
, m_nSize(0)
, m_pBuckets(NULL)
, m_pComparator(NULL)
{
	m_pIterator = DNEW ngHashSetIterator(this);
}

void ngHashSet::Initialize(ngHashComparator* pComp /* = NULL */)
{
	m_pComparator = pComp;
}

ngHashSet::~ngHashSet()
{
	for (int i = 0;i < m_nBucketsCount;i ++) {
		SAFE_DELETE(m_pBuckets[i]);
	}
	SAFE_DELETE(m_pBuckets);
	SAFE_DELETE(m_pIterator);
	m_nBucketsCount = 0;
	m_nSize = 0;
}

int32 ngHashSet::Add(void* pObj, boolean* bAdded /* = NULL */)
{
	if (m_pBuckets == NULL)
	{
		m_nBucketsCount = BUCKET_SIZE_LIST[0];
		int32 nSize = sizeof(ngBucket*) * m_nBucketsCount;
		m_pBuckets = (ngBucket**) DMALLOC(nSize);
		if (m_pBuckets == NULL)
		{
			m_nBucketsCount = 0;
			return -1;
		}
		DMEMSET(m_pBuckets, 0, nSize);
	}

	uint32 nBucketNo = BucketNo(pObj, m_nBucketsCount, m_pComparator);
	ngBucket*& pBucket = m_pBuckets[nBucketNo];
	if (pBucket == NULL)
	{
		pBucket = ngBucket::CreateBucket(m_pComparator);
		if (pBucket == NULL)
			return -1;
	}

	boolean added;
	if (pBucket->Add(pObj, &added) != 0)
		return -1;

	if (bAdded != NULL)
		*bAdded = added;

	if (added)
	{
		m_nSize++;
		if (pBucket->m_nSize > BUCKET_MAX_HEIGHT)
			ExpandBuckets();
	}

	return 0;
}

uint32 ngHashSet::Size()
{
	return m_nSize;
}

boolean ngHashSet::IsEmpty()
{
	return m_nSize == 0;
}

ngIterator* ngHashSet::Iterator()
{
//	m_SAFE_DELETE(pIterator);
	return DNEW ngHashSetIterator(this);
}

void* ngHashSet::Contains(void* obj)
{
	if (m_nBucketsCount == 0)
		return NULL;

	int32 nBucketNo = BucketNo(obj, m_nBucketsCount, m_pComparator);
	ngBucket* pBucket = m_pBuckets[nBucketNo];
	if (pBucket == NULL)
		return NULL;
	else
		return pBucket->Contains(obj);
}

boolean ngHashSet::Clear()
{
	for (int i = 0; i < m_nBucketsCount; i++)
	{
		SAFE_DELETE(m_pBuckets[i]);
	}

	DFREE(m_pBuckets);
	m_pBuckets = NULL;
	m_nBucketsCount = 0;
	m_nSize = 0;

	return TRUE;
}

void* ngHashSet::Remove(void* obj, boolean* pRemoved)
{
	if (m_nBucketsCount == 0)
	{
		if (pRemoved)
			*pRemoved = FALSE;
		return NULL;
	}

	void* res = NULL;

	int32 nBucketNo = BucketNo(obj, m_nBucketsCount, m_pComparator);
	ngBucket*& pBucket = m_pBuckets[nBucketNo];

	if (pRemoved)
		*pRemoved = FALSE;

	if (pBucket != NULL)
	{
		res = pBucket->Remove(obj);
		if (res != NULL) {
			m_nSize --;
			if (pRemoved) {
				*pRemoved = TRUE;
			}
		}
	}

	return res;
}


int32 ngHashSet::ExpandBuckets()
{
	int i;
	int nNewBucketsCount = 0;
	int length = sizeof(BUCKET_SIZE_LIST) / sizeof(BUCKET_SIZE_LIST[0]);
	for (i = 0; i < length; i++)
	{
		if (BUCKET_SIZE_LIST[i] > m_nBucketsCount)
		{
			nNewBucketsCount = BUCKET_SIZE_LIST[i];
			break;
		}
	}

	if (nNewBucketsCount == 0)
		nNewBucketsCount = m_nBucketsCount * 2 - 1;

	ngBucket** pItems = (ngBucket**) DMALLOC(sizeof(ngBucket*) * nNewBucketsCount);
	if (pItems == NULL)
		return -1;

	for (i = 0; i < nNewBucketsCount; i++)
		pItems[i] = NULL;

	for (i = 0; i < m_nBucketsCount; i++)
	{
		ngBucket* pBucket = m_pBuckets[i];
		if (pBucket == NULL)
			continue;

		for (ngBucketNode* pNode = pBucket->m_pHead; pNode != NULL; pNode = pNode->next)
		{
			void* obj = pNode->data;
			uint32 nIndex = BucketNo(obj, nNewBucketsCount, m_pComparator);
			ngBucket*& pBucket2 = pItems[nIndex];
			if (pBucket2 == NULL)
				pBucket2 = ngBucket::CreateBucket(m_pComparator);
			boolean added;
			pBucket2->Add(obj, &added);
		}
	}

	for (i = 0; i < m_nBucketsCount; i++) {
		SAFE_DELETE(m_pBuckets[i]);
	}
	DFREE(m_pBuckets);
	m_pBuckets = pItems;
	m_nBucketsCount = nNewBucketsCount;
	return 0;
}

int32 ngHashSet::ShrinkBuckets()
{
	return -1;
}
