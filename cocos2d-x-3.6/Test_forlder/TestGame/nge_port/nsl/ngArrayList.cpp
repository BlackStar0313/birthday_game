#include <nsl/ngArrayList.h>
#include <nsl/ngArrayListIterator.h>
#include <nsl/ngComparator.h>
#include <core/NGE_Macros.h>

static int32 nextCapacity(int32 capacity)
{
	if (capacity == 0)
		return 8;	
	else if (capacity < 1024)
		return capacity * 2;
	else
		return (int32) (capacity * 1.6);
}

ngArrayList::ngArrayList()
{
	m_pItems = NULL;
	m_nSize = 0;
	m_nCapacity = 0;
	//m_pIterator = DNEW ngArrayListIterator(this);
}

ngArrayList::ngArrayList(int32 capacity)
{
	NGASSERT( capacity <= 128 * 1024 );		// we assume that the array's maximum size is 128K
	m_nCapacity = capacity;
	m_pItems = (void **)DMALLOC(sizeof(void *) * capacity);
	NGASSERT( m_pItems );
	m_nSize = 0;
}

ngArrayList::~ngArrayList()
{
	DFREE(m_pItems);
	//SAFE_DELETE(m_pIterator);
}

int32 ngArrayList::Add(void* pObj)
{
	return InsertAtEnd(pObj);
}

uint32 ngArrayList::Size() const
{
	return m_nSize;
}

boolean ngArrayList::IsEmpty() const
{
	return (m_nSize <= 0);
}

ngIterator* ngArrayList::Iterator(int32 index /* = 0 */) const 
{
	return DNEW ngArrayListIterator(this, index);
}

void ngArrayList::Clear()
{
	int32 size = m_nSize;
	for (int32 i = 0; i < size; i ++) {
		RemoveByIndex(0);
	}
	m_nSize = 0;
}

void* ngArrayList::Remove(void* obj)
{
	if (m_nSize == 0)
		return NULL;

	void** pItems = m_pItems;
	void** pEnd = m_pItems + m_nSize;

	while (true)
	{
		if (*pItems == obj)
			break;	// got it.

		if (++pItems == pEnd)
			return NULL;	// can't find it.
	}

	void** pItems2 = pItems++;
	while (pItems != pEnd)
	{
		if (*pItems != obj)
			*pItems2++ = *pItems;
		*pItems++;
	}

	m_nSize -= pItems - pItems2;
	
	return obj;
}


boolean ngArrayList::Contains(void* pObj, ngComparator* pComp /* = NULL */)
{
	int32 index = Find(pObj,0,pComp);
	return (index >= 0);
}



void* ngArrayList::First()
{
	return m_pItems[0];
}

void* ngArrayList::Last()
{
	return m_pItems[m_nSize - 1];
}


void* ngArrayList::Get(int32 index)
{
	NGASSERT(index >= 0 && index < m_nSize);
	return m_pItems[index];
}

boolean ngArrayList::Set(int32 index, void* obj)
{
	NGASSERT(index >= 0 && index < m_nSize);
	m_pItems[index] = obj;
	return TRUE;
}

int32 ngArrayList::InsertBefore(void* pObj, void* pRefObj)
{
	NGASSERT(0);
	return -1;
}

int32 ngArrayList::InsertBefore(uint32 index, void* pObj)
{
	NGASSERT(index >= 0 && index < m_nSize);
	return InsertBeforeInternal(index, pObj);
}

int32 ngArrayList::InsertAfter(void* pObj, void* pRefObj)
{
	NGASSERT(0);
	return -1;
}

int32 ngArrayList::InsertAtEnd(void* pObj)
{
	if (m_nCapacity == m_nSize)
	{
		// not enough capacity 
		int32 nCapacity = nextCapacity(m_nCapacity);
		void** pItems = (void**) DMALLOC(sizeof(void*) * nCapacity);
		if (pItems == NULL)
			return -1;

		DMEMCPY(pItems, m_pItems, sizeof(void*) * m_nSize);
		m_nCapacity = nCapacity;
		DFREE(m_pItems);
		m_pItems = pItems;
	}

	m_pItems[m_nSize++] = pObj;
	return 0;
}

int32 ngArrayList::InsertAtHead(void* pObj)
{
	return InsertBeforeInternal(0, pObj);
}

void* ngArrayList::RemoveByIndex(uint32 index)
{
	NGASSERT(index >= 0 && index < m_nSize);
	
	void* pObj = Get(index);

	// necessary to shrink
	if (m_nSize > 8 && m_nCapacity / m_nSize >= 2)
	{
		int32 nNewCapacity = m_nSize + 8; 
		void** pItems = (void**) DMALLOC(sizeof(void*) * nNewCapacity);
		if (pItems == NULL)
		{
			// failed to malloc, so can't shrink
			DMEMMOVE(m_pItems + index, m_pItems + index + 1, sizeof(void *) * (m_nSize - index - 1));
		}
		else
		{
			if (index > 0)
				DMEMCPY(pItems, m_pItems, sizeof(void*) * index);
			if (index < m_nSize - 1)
				DMEMMOVE(pItems + index, m_pItems + index + 1, sizeof(void*) * (m_nSize - index - 1));
			DFREE(m_pItems);
			m_pItems = pItems;
			m_nCapacity = nNewCapacity;
		}
	}
	else
	{
		// don't shrink
		DMEMMOVE(m_pItems + index, m_pItems + index + 1, sizeof(void *) * (m_nSize - index - 1));
	}
	m_nSize--;
	
	return pObj;
}

int32 ngArrayList::Find(void* pObj, int32 start /* = 0 */, ngComparator* pComp /* = NULL */)
{
	if (m_nSize == 0) {
		return -1;
	}

	// if start less than 0, start from right.
	if (start < 0) {
		start = m_nSize + start;
	}

	NGASSERT( 0 <= start && m_nSize > start);

	if (pComp == NULL) {
		for (int32 i = start;i < m_nSize;i ++) {
			if (pObj == m_pItems[i]) {
				return i;
			}
		}
	}else{
		for (int32 i = start;i < m_nSize;i ++) {
			if (pComp->Equals(pObj, m_pItems[i])) {
				return i;
			}
		}
	}
	return -1;
}

int32 ngArrayList::ReverseFind(void* pObj, int32 start /* = -1 */, ngComparator* pComp /* = NULL */)
{
	if (m_nSize == 0) {
		return -1;
	}

	// if start less than 0, start from right.
	if (start < 0) {
		start = m_nSize + start;
	}

	NGASSERT( 0 <= start && m_nSize > start);

	if (pComp == NULL) {
		for (int32 i = start;i > 0;i --) {
			if (pObj == m_pItems[i]) {
				return i;
			}
		}
	}else{
		for (int32 i = start;i > 0;i --) {
			if (pComp->Equals(pObj, m_pItems[i])) {
				return i;
			}
		}
	}
	return -1;
}

int32 ngArrayList::Sort(ngComparator* pComp /* = NULL */)
{
	NGASSERT(0);
	return -1;
}

int32 ngArrayList::SortedInsert(void* pObj, ngComparator* pComp /* = NULL */)
{
	NGASSERT(0);
	return -1;
}

int32 ngArrayList::Resize(int32 capacity)
{
	void** p = (void**) DMALLOC(sizeof(void*) * capacity);
	if (p == NULL)
		return -1;

	DMEMCPY(p, (const void*) m_pItems, sizeof(void*) * capacity);
	if (capacity > m_nSize)
		DMEMCPY(p + capacity, 0, capacity - m_nSize);

	DFREE(m_pItems);
	m_pItems = p;
	m_nCapacity = capacity;
	m_nSize = capacity;
	return 0;
}

int32 ngArrayList::InsertBeforeInternal(int32 index, void* pObj)
{
	// whether the capacity is enough
	if (m_nCapacity > m_nSize)
	{
		// is enough
		DMEMMOVE(m_pItems + index + 1, m_pItems + index, sizeof(void*) * (m_nSize - index));
	}
	else
	{
		// not enough
		int32 nCapacity = nextCapacity(m_nCapacity);
		void** pItems = (void**) DMALLOC(sizeof(void*) * nCapacity);
		if (pItems == NULL)
			return -1;

		if (index > 0)
			DMEMCPY(pItems, m_pItems, sizeof(void*) * index);

		if (index < m_nSize)
			DMEMCPY(pItems + index + 1, m_pItems + index, sizeof(void*) * (m_nSize - index));

		m_nCapacity = nCapacity;
		DFREE(m_pItems);
		m_pItems = pItems;
	}

	m_pItems[index] = pObj;
	m_nSize++;
	return 0;
}

