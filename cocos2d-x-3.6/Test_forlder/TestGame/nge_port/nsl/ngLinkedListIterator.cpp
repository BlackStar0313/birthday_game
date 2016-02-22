#include <nsl/ngLinkedListIterator.h>
#include <nsl/ngLinkedListNode.h>
#include <core/NGE_Macros.h>

ngLinkedListIterator::ngLinkedListIterator(const ngLinkedList *pList, int32 index /* = 0 */)
: m_pList((ngLinkedList *)pList)
, m_nIndex(index)
{
	m_pNode = m_pList->MoveTo(index);
    
#if 0
    /* iteratorList & __debug_iterator define in NGE_Defs.h, use this code to debug iterator memory leaks. */
    if (iteratorList) {
        iteratorList->Add(this);
        NG_DEBUG_LOG("[Iterator] iterator list size: %d\n", iteratorList->Size());
    }
    if (__debug_iterator) {
//        NG_DEBUG_LOG("[Iterator] New Iterator, list: 0x%X(%d)\n", (int32)pList, pList->Size());
        NG_DEBUG_LOG("[Iterator] New Iterator");
    }
    
#endif
}


ngLinkedListIterator::ngLinkedListIterator(const ngLinkedList* pList, ngLinkedListNode* pNode)
: m_pList((ngLinkedList *)pList)
, m_pNode(pNode)
{
	m_nIndex = m_pList->Find(pNode);
}

ngLinkedListIterator::~ngLinkedListIterator()
{
#if 0
    if (iteratorList) {
        iteratorList->Remove(this);
        NG_DEBUG_LOG("[Iterator] iterator list size: %d\n", iteratorList->Size());
    }
    if (__debug_iterator) {
//        NG_DEBUG_LOG("[Iterator] Destroy Iterator, list: 0x%X(%d)\n", (int32)m_pList, m_pList->Size());
        NG_DEBUG_LOG("[Iterator] Destroy Iterator\n");
    }
#endif
}

boolean ngLinkedListIterator::HasNext()
{
	return (m_pNode != NULL);
}

void* ngLinkedListIterator::Next()
{
	NGASSERT( m_pNode != NULL );
	void* pObj = m_pNode->data;
	m_pNode = m_pNode->next;
	m_nIndex ++;
	return pObj;
}

void* ngLinkedListIterator::Get()
{
	NGASSERT( m_pNode != NULL );
	void* pObj = m_pNode->data;
	return pObj;
}

void ngLinkedListIterator::Destroy()
{
	DELETE_OBJECT(this, ngLinkedListIterator);
}

int32 ngLinkedListIterator::Index()
{
	return m_nIndex;
}

void ngLinkedListIterator::Set(void* pObj)
{
	m_pNode->data = pObj;
}

int32 ngLinkedListIterator::InsertBefore(void* pObj)
{
	int32 res = m_pList->InsertBeforeInternal(m_pNode, pObj);
	m_nIndex ++;
	return res;
}

int32 ngLinkedListIterator::InsertAfter(void* pObj)
{
	int32 res = m_pList->InsertAfterInternal(m_pNode, pObj);
	return res;
}

void* ngLinkedListIterator::Remove()
{
	ngLinkedListNode* pNode = m_pNode->next;
	void* pObj = m_pNode->data;
	m_pNode = m_pList->RemoveInternal(m_pNode);
	m_pNode = pNode;
	return pObj;
}

