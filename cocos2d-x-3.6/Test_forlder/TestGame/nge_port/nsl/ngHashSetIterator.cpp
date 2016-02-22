#include <nsl/ngHashSetIterator.h>
#include <nsl/ngBucket.h>
#include <nsl/ngHashSet.h>
#include <core/NGE_Macros.h>

ngHashSetIterator::ngHashSetIterator(const ngHashSet* pSet)
{
	m_pSet = pSet;
	m_pNode = NULL;
	m_nBucketNo = -1;
	MoveToNextBucket();
}

boolean ngHashSetIterator::HasNext()
{
	return m_pNode != NULL;
}

void* ngHashSetIterator::Next()
{
	NGASSERT(m_pNode != NULL);
	void* obj = m_pNode->data;
	m_pNode = m_pNode->next;
	if (m_pNode == NULL)
		MoveToNextBucket();
	return obj;
}

void* ngHashSetIterator::Get()
{
	NGASSERT(m_pNode != NULL);
	return m_pNode->data;
}

void ngHashSetIterator::Destroy()
{
	m_pNode = NULL;
	m_nBucketNo = -1;
	MoveToNextBucket();
	DELETE_OBJECT(this, ngHashSetIterator);
}

void ngHashSetIterator::MoveToNextBucket()
{
	if (m_pNode == NULL)
		m_nBucketNo++;

	int length = m_pSet->m_nBucketsCount;
	while (m_nBucketNo < length)
	{
		ngBucket* pBucket = m_pSet->m_pBuckets[m_nBucketNo];
		if (pBucket == NULL || pBucket->m_pHead == NULL)
		{
			m_nBucketNo++;
			continue;
		}

		m_pNode = pBucket->m_pHead;
		break;
	}
}