#include <nsl/ngArrayListIterator.h>
#include <nsl/ngArrayList.h>
#include <core/NGE_Macros.h>

ngArrayListIterator::ngArrayListIterator(const ngArrayList* pList, int32 index /* = 0 */)
: m_pList((ngArrayList*)pList)
, m_nIndex(index)
{
	
}

boolean ngArrayListIterator::HasNext()
{
	return (m_pList->Size() > m_nIndex);
}

void* ngArrayListIterator::Next()
{
	NGASSERT( m_pList->Size() > m_nIndex );
	return m_pList->Get(m_nIndex ++);
}

void* ngArrayListIterator::Get()
{
	NGASSERT( m_pList->Size() > m_nIndex );
	return m_pList->Get(m_nIndex);
}

int32 ngArrayListIterator::Index()
{
	return m_nIndex;
}

void ngArrayListIterator::Set(void* pObj)
{
	NGASSERT( m_pList->Size() > m_nIndex );
	m_pList->m_pItems[m_nIndex] = pObj;
}

void ngArrayListIterator::Destroy()
{
	m_nIndex = 0;
	DELETE_OBJECT(this, ngArrayListIterator);
}

int32 ngArrayListIterator::InsertBefore(void* pObj)
{
	int32 res = m_pList->InsertBefore(m_nIndex, pObj);
	if (res != 0)
		return res;
	m_nIndex++;
	return 0;
}

int32 ngArrayListIterator::InsertAfter(void* pObj)
{
	int32 res = m_pList->InsertBefore(m_nIndex + 1, pObj);
	if (res != 0)
		return res;
	return 0;
}

void* ngArrayListIterator::Remove()
{
	return m_pList->RemoveByIndex(m_nIndex);
}
