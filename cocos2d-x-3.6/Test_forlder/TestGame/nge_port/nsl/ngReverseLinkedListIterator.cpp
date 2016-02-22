/*
 *  ngReverseLinkedListIterator.cpp
 *  template
 *
 *  Created by Yuhui Wang on 7/4/10.
 *  Copyright 2010 Anansi Mobile. All rights reserved.
 *
 */


#include <nsl/ngReverseLinkedListIterator.h>
#include <nsl/ngLinkedListNode.h>
#include <core/NGE_Macros.h>

ngReverseLinkedListIterator::ngReverseLinkedListIterator(const ngLinkedList *pList)
: m_pList((ngLinkedList *)pList)
{
	m_nIndex = m_pList->Size() - 1;
	m_pNode = (ngLinkedListNode *)m_pList->MoveTo(m_nIndex);
}


ngReverseLinkedListIterator::ngReverseLinkedListIterator(const ngLinkedList* pList, ngLinkedListNode* pNode)
: m_pList((ngLinkedList *)pList)
, m_pNode(pNode)
{
	m_nIndex = m_pList->Find(pNode);
}

boolean ngReverseLinkedListIterator::HasNext()
{
	return (m_pNode != NULL);
}

void* ngReverseLinkedListIterator::Next()
{
	NGASSERT( m_pNode != NULL );
	void* pObj = m_pNode->data;
	m_pNode = m_pNode->prev;
	m_nIndex --;
	return pObj;
}

void* ngReverseLinkedListIterator::Get()
{
	NGASSERT( m_pNode != NULL );
	void* pObj = m_pNode->data;
	return pObj;
}

void ngReverseLinkedListIterator::Destroy()
{
	m_nIndex = m_pList->Size() - 1;
	DELETE_OBJECT(this, ngReverseLinkedListIterator);
}

int32 ngReverseLinkedListIterator::Index()
{
	return m_nIndex;
}

void ngReverseLinkedListIterator::Set(void* pObj)
{
	m_pNode->data = pObj;
}

int32 ngReverseLinkedListIterator::InsertBefore(void* pObj)
{
	int32 res = m_pList->InsertBeforeInternal(m_pNode, pObj);
	m_nIndex ++;
	return res;
}

int32 ngReverseLinkedListIterator::InsertAfter(void* pObj)
{
	int32 res = m_pList->InsertAfterInternal(m_pNode, pObj);
	return res;
}

void* ngReverseLinkedListIterator::Remove()
{
//	void* pObj = m_pNode->data;
//	m_pList->RemoveInternal(m_pNode);
//	return pObj;
//	
	ngLinkedListNode* pNode = m_pNode->prev;
	void* pObj = m_pNode->data;
	m_pNode = m_pList->RemoveInternal(m_pNode);
	m_pNode = pNode;
	m_nIndex --;
	return pObj;
	
}

