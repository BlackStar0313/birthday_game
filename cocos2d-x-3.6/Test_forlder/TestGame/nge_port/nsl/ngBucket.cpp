#include <nsl/ngBucket.h>
#include <nsl/ngHashComparator.h>
#include "../core/NGE_Macros.h"

ngBucket* ngBucket::CreateBucket(ngHashComparator* pHashComp /* = NULL */)
{
	ngBucket* pBucket = DNEW ngBucket(pHashComp);
	return pBucket;
}

ngBucket::ngBucket(ngHashComparator* pHashComp)
: m_pHead(NULL)
, m_nSize(0)
, m_pComparator(pHashComp)
{
	
}

ngBucket::~ngBucket()
{
	ngBucketNode* pNode = m_pHead;
	while(pNode) {
		ngBucketNode* pFree = pNode->next;
		SAFE_DELETE(pNode);
		pNode = pFree;
	}
	m_nSize = 0;
}

int32 ngBucket::Add(void *pObj, boolean *bAdded /* = 0 */)
{
	ngBucketNode* pLastNode = NULL;
	*bAdded = FALSE;
	ngBucketNode* pNode = m_pHead;
	while(pNode) {
		if (m_pComparator == NULL) {
			if (pNode->data == pObj) {
				return 0;
			}
		}else{
			if (m_pComparator->Equals(pObj, pNode->data)) {
				return 0;
			}
		}

		pLastNode = pNode;
		pNode = pNode->next;
	}

	pNode = DNEW(ngBucketNode);
	if (pNode == NULL) {
		return -1;
	}
	pNode->data = pObj;
	pNode->next = NULL;

	if (pLastNode == NULL) {
		m_pHead = pNode;
	}else{
		pLastNode->next = pNode;
	}

	m_nSize ++;
	*bAdded = TRUE;

	return 0;
}

void* ngBucket::Remove(void* pObj)
{
	ngBucketNode* pPrev = NULL;
	boolean bEqual = FALSE;
	ngBucketNode* pNode = m_pHead;
	while (pNode) {
		bEqual = (m_pComparator == NULL) ? (pNode->data == pObj) : (m_pComparator->Equals(pObj,pNode->data));
		if (bEqual) {
			ngBucketNode* pNext = pNode->next;
			if (pPrev == NULL) {
				m_pHead = pNext;
			}else{
				pPrev->next = pNext;
			}
			void* ret = pNode->data;
			SAFE_DELETE(pNode);
			m_nSize --;
			return ret;
		}
		pPrev = pNode;
		pNode = pNode->next;
	}
	return NULL;
}

void* ngBucket::Contains(void* pObj)
{
	boolean bEqual = FALSE;
	ngBucketNode* pNode = m_pHead;
	while (pNode) {
		bEqual = (m_pComparator == NULL) ? (pNode->data == pObj) : (m_pComparator->Equals(pObj,pNode->data));
		if (bEqual) {
			return pNode->data;
		}
		pNode = pNode->next;
	}
	return NULL;
}