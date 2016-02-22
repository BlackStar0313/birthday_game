#include <nsl/ngLinkedList.h>
#include <nsl/ngComparator.h>
#include <nsl/ngLinkedListNode.h>
#include <nsl/ngLinkedListIterator.h>
#include <nsl/ngReverseLinkedListIterator.h>
#include <core/NGE_Macros.h>
#include <core/NGE_Defs.h>

ngLinkedListNode::ngLinkedListNode()
: prev(NULL)
, next(NULL)
, data(NULL)
{
}


ngLinkedList::ngLinkedList()
: m_pHead(NULL)
, m_pTail(NULL)
, m_nSize(0)
{
//	m_pIterator = DNEW ngLinkedListIterator(this);
}

ngLinkedList::ngLinkedList(const ngLinkedList& src)
: m_pHead(NULL)
, m_pTail(NULL)
, m_nSize(0)
{
    LINKEDLIST_COPY(*this, src);
}

ngLinkedList::~ngLinkedList()
{
	Clear();
//	SAFE_DELETE(m_pIterator);
}

void ngLinkedList::Clear()
{
	ngLinkedListNode* pNode = m_pHead;
	while (pNode) {
		ngLinkedListNode* pNext = pNode->next;
		SAFE_DELETE(pNode);
		pNode = pNext;
	}

	m_pHead = NULL;
	m_pTail = NULL;
	m_nSize = 0;
}

int32 ngLinkedList::Add(void* pObj)
{
	return InsertAtEnd(pObj);
}

uint32 ngLinkedList::Size() const
{
	return m_nSize;
}


boolean ngLinkedList::IsEmpty() const
{
	return (m_nSize == 0);
}

ngIterator* ngLinkedList::Iterator(int32 index /*= 0*/) const {
	return DNEW ngLinkedListIterator(this,index);
}

ngIterator* ngLinkedList::ReverseIterator() const {
	return DNEW ngReverseLinkedListIterator(this);
}

boolean ngLinkedList::Contains(void* pObj, ngComparator* pComp /* = NULL */)
{
	int32 index = Find(pObj, 0, pComp);
	return (index >= 0);
}

void* ngLinkedList::First()
{
	if (m_pHead == NULL) {
		return NULL;
	}
	return m_pHead->data;
}

void* ngLinkedList::Remove(void* pObj) 
{
	ngLinkedListNode* pNode = m_pHead;
	while (pNode != NULL) {
		if (pNode->data == pObj) {
			RemoveInternal(pNode);
			return pObj;
		}

		pNode = pNode->next;
	}
	
	return NULL;
}

void* ngLinkedList::Last()
{
	if (m_pHead == NULL) {
		return NULL;
	}
	return m_pTail->data;
}

void* ngLinkedList::Get(int32 index)
{
	if (index < 0 || index >= m_nSize) {
		NGASSERT( 0	);
		return NULL;
	}

	ngLinkedListNode* pNode = MoveTo(index);
	if (pNode != NULL) {
		return pNode->data;
	}
	return NULL;
}

boolean ngLinkedList::Set(int32 index, void* pObj)
{
	if (index < 0 || index >= m_nSize) {
		NGASSERT( 0 );
		return FALSE;
	}

	ngLinkedListNode* pNode = MoveTo(index);
	pNode->data = pObj;

	return TRUE;
}

int32 ngLinkedList::InsertBefore(void* pObj, void* pRefObj)
{
	int32 offset = Find(pRefObj);
	ngLinkedListNode* pNode = MoveTo(offset);
	return InsertBeforeInternal(pNode,pObj);
}

int32 ngLinkedList::InsertBefore(uint32 index, void* pObj)
{
	ngLinkedListNode* pNode = MoveTo(index);
	return InsertBeforeInternal(pNode, pObj);
}

int32 ngLinkedList::InsertAfter(void* pObj, void* pRefObj)
{
	int32 offset = Find(pRefObj);
	ngLinkedListNode* pNode = MoveTo(offset);
	return InsertAfterInternal(pNode, pObj);
}

int32 ngLinkedList::InsertAtEnd(void* pObj)
{
	return InsertAfterInternal(m_pTail, pObj);
}

int32 ngLinkedList::InsertAtHead(void* pObj)
{
	return InsertBeforeInternal(m_pHead, pObj);
}

void* ngLinkedList::RemoveByIndex(uint32 index)
{
	ngLinkedListNode* pNode = MoveTo(index);
	void* pObj = pNode->data;
	RemoveInternal(pNode);
	return pObj;
}

int32 ngLinkedList::Find(void* pObj, int32 start /* = 0 */, ngComparator* pComp /* = NULL */)
{
	if (m_nSize <= 0) {
		return -1;
	}

	ngLinkedListNode* pNode = MoveTo(start);
	if (pComp == NULL) {
		while (pNode) {
			if (pNode->data == pObj) {
				return start;
			}

			start ++;
			pNode = pNode->next;
		}
	}else{
		while(pNode) {
			if (pComp->Equals(pObj, pNode->data)) {
				return start;
			}

			pNode = pNode->next;
			start ++;
		}
	}

	return -1;
}

int32 ngLinkedList::ReverseFind(void* pObj, int32 start /* = -1 */, ngComparator* pComp /* = NULL */)
{
	if (m_nSize <= 0) {
		return -1;
	}

	ngLinkedListNode* pNode = MoveTo(start);
	if (pComp == NULL) {
		while (pNode) {
			if (pNode->data == pObj) {
				return start;
			}

			start --;
			pNode = pNode->prev;
		}
	}else{
		while(pNode) {
			if (pComp->Equals(pObj, pNode->data)) {
				return start;
			}

			pNode = pNode->prev;
			start --;
		}
	}

	return -1;
}

int32 ngLinkedList::Sort(ngComparator* pComp /* = NULL */)
{
	if (m_nSize <= 1)
		return -1;
	
#if 1
	ngLinkedListNode* pHead = m_pTail;
	ngLinkedListNode* pTail = m_pTail;
	m_pTail = m_pTail->prev;
	m_pTail->next = NULL;
	
	pHead->prev = pTail->prev = NULL;
	pHead->next = pTail->next = NULL;
	
	// insert sort
	int32 i = m_nSize - 1;
	if (pComp)
	{
		while (i-- > 0)
		{
			ngLinkedListNode* p = m_pTail;
			m_pTail = m_pTail->prev;
			if (m_pTail)
				m_pTail->next = NULL;

			ngLinkedListNode* pNode = pHead;

			do {
				if (pNode == NULL)
				{
					pTail->next = p;
					p->prev = pTail;
					p->next = NULL;
					pTail = p;
					break;
				}
				else
				{
					if (pComp->Compare(p->data, pNode->data) < 0)
					{
						ngLinkedListNode* prev = pNode->prev;
						if (prev)
							prev->next = p;
						p->next = pNode;
						p->prev = prev;
						pNode->prev = p;
						if (pHead == pNode) {
							pHead = p;
						}
						break;
					}
				}

				pNode = pNode->next;

			} while (1);
		}
	}
	else
	{
		while (i-- > 0)
		{
			ngLinkedListNode* p = m_pTail;
			m_pTail = m_pTail->prev;
			if (m_pTail)
				m_pTail->next = NULL;

			ngLinkedListNode* pNode = pHead;

			do {
				if (pNode == NULL)
				{
					pTail->next = p;
					p->prev = pTail;
					p->next = NULL;
					pTail = p;
					break;
				}
				else
				{
					if (p->data < pNode->data)
					{
						ngLinkedListNode* prev = pNode->prev;
						if (prev)
							prev->next = p;
						p->next = pNode;
						p->prev = prev;
						pNode->prev = p;
						if (pHead == pNode) {
							pHead = p;
						}
						break;
					}
				}

				pNode = pNode->next;

			} while (1);
		}
	}
	
	m_pTail = pTail;
	m_pHead = pHead;
#else
    QSort(0, Size() - 1, pComp);
#endif
	
	return NG_OK;
}

int32 ngLinkedList::SortedInsert(void* pObj, ngComparator* pComp /* = NULL */)
{
	ngLinkedListNode* new_node = DNEW(ngLinkedListNode);
	if (new_node == NULL)
		return -1;

	int32 i = 0;
	new_node->data = pObj;
	if (pComp)
	{
		ngLinkedListNode* pNode = m_pHead;

		do {
			if (pNode == NULL)
			{
				m_pTail->next = new_node;
				new_node->prev = m_pTail;
				new_node->next = NULL;
				break;
			}
			else
			{
				if (pComp->Compare(pObj, pNode->data) < 0)
				{
					ngLinkedListNode* prev = pNode->prev;
					if (prev)
						prev->next = new_node;
					new_node->next = pNode;
					new_node->prev = prev;
					pNode->prev = new_node;
					break;
				}
			}

			pNode = pNode->next;
			i++;

		} while (1);
	}
	else
	{
		ngLinkedListNode* pNode = m_pHead;

		do {
			if (pNode == NULL)
			{
				m_pTail->next = new_node;
				new_node->prev = m_pTail;
				new_node->next = NULL;
				break;
			}
			else
			{
				if (pObj < pNode->data)
				{
					ngLinkedListNode* prev = pNode->prev;
					if (prev)
						prev->next = new_node;
					new_node->next = pNode;
					new_node->prev = prev;
					pNode->prev = new_node;
					break;
				}
			}

			pNode = pNode->next;

		} while (1);
	}

	return i;
}

void ngLinkedList::Swap(int32 i, int32 j)
{
    ngLinkedListNode* pNode = MoveTo(i);
    ngLinkedListNode* pNext = MoveTo(j);
	void* pTemp = pNode->data;
    pNode->data = pNext->data;
    pNext->data = pTemp;
}

int32 ngLinkedList::Partition(int32 begin, int32 end, ngComparator* pComp)
{    
    int32 inc = 0;
    
    ngLinkedListNode* pNode = MoveTo(begin);
    void* value = pNode->data;
    int32 index = begin, ii = begin, jj = end;
    
    while (ii <= jj) {
        ngLinkedListNode* pJ = MoveTo(jj);
        while (pComp->Compare(pJ->data, value) >= 0 && jj >= index) {
            jj --;
            inc ++;
            pJ = MoveTo(jj);
        }
        if (jj >= index) {
            Set(index, pJ->data);
            index = jj;
        }
        ngLinkedListNode* pI = MoveTo(ii);
        while (pComp->Compare(pI->data, value) <= 0 && ii <= index) {
            ii ++;
            inc ++;
            pI = MoveTo(ii);
        }
        if (ii <= index) {
            Set(index, pI->data);
            index = ii;
        }
    }
    Set(index, value);
    
    printf("compare %d times in %d - %d, index = %d\n", inc, begin, end, index);
    
    return index;                
}

void ngLinkedList::QSort(int32 begin, int32 end, ngComparator* pComp) {
    if (end > begin) {
        int index = Partition(begin, end, pComp);
        if (index - begin > 1) {
            QSort(begin, index - 1, pComp);
        }
        if (end - index > 1) {
            QSort(index + 1,  end,  pComp);
        }
    }
}

int32 ngLinkedList::QSort(ngComparator* pComp)
{
    QSort(0, Size() - 1, pComp);
    return NG_OK;
}

int32 ngLinkedList::InsertAfterInternal(ngLinkedListNode* pNode, void* pObj)
{
	ngLinkedListNode* newNode = DNEW ngLinkedListNode();
	if (newNode == NULL) {
		return NG_ERROR_NOMEM;
	}
	newNode->data = pObj;

	// Add to the head
	if (pNode == NULL) {
		newNode->prev = NULL;
		newNode->next = m_pHead;
		if (m_pHead == NULL) {
			m_pTail = newNode;
		}else{
			m_pHead->prev = newNode;
		}
		m_pHead = newNode;
	}else{
		ngLinkedListNode* nextNode = pNode->next;
		pNode->next = newNode;
		newNode->prev = pNode;
		newNode->next = nextNode;
		if (nextNode == NULL) {
			m_pTail = newNode;
		}else{
			nextNode->prev = newNode;
		}
	}

	m_nSize ++;
	return NG_OK;
}


int32 ngLinkedList::InsertBeforeInternal(ngLinkedListNode* pNode, void* pObj)
{
	ngLinkedListNode* newNode = DNEW(ngLinkedListNode);
	if (newNode == NULL) {
		return NG_ERROR_NOMEM;
	}

	newNode->data = pObj;
	// Add to the tail.
	if (pNode == NULL) {
		newNode->prev = m_pTail;
		newNode->next = NULL;
		if (m_pTail == NULL) {
			m_pHead = newNode;
		}else{
			m_pTail->next = newNode;
		}
		m_pTail = newNode;
	}else{
		ngLinkedListNode* prevNode = pNode->prev;
		newNode->next = pNode;
		newNode->prev = prevNode;
		pNode->prev = newNode;
		if (prevNode == NULL) {
			m_pHead = newNode;
		}else{
			prevNode->next = newNode;
		}
	}
	m_nSize ++;
	return NG_OK;
}

ngLinkedListNode* ngLinkedList::MoveTo(int32 offset)
{
	if (offset <= 0) {
		return m_pHead;
	}

	if (offset >= m_nSize - 1) {
		return m_pTail;
	}

	if (offset == 1) {
		return m_pHead->next;
	}

	if (offset == m_nSize - 2) {
		return m_pTail->prev;
	}

	int32 pos = 0;
	if (offset * 2 < m_nSize) {
		// if offset nears the head, start find from the head.
		ngLinkedListNode* pNode = m_pHead;
		pos = 0;
		while (pNode) {
			if (pos == offset) {
				return pNode;
			}
			
			pNode = pNode->next;
			pos ++;
		}
	}else{
		// if offset nears the tail, start find from the tail.
		ngLinkedListNode* pNode = m_pTail;
		pos = m_nSize - 1;
		while (pNode) {
			if (pos == offset) {
				return pNode;
			}

			pNode = pNode->prev;
			pos --;
		}
	}
	return NULL;
}

ngLinkedListNode* ngLinkedList::RemoveInternal(ngLinkedListNode* pNode, void** ppObj /* = NULL */)
{
	ngLinkedListNode* prevNode = pNode->prev;
	ngLinkedListNode* nextNode = pNode->next;

	if (prevNode != NULL) {
		prevNode->next = nextNode;
	}
	if (nextNode != NULL) {
		nextNode->prev = prevNode;
	}
	if (pNode == m_pHead) {
		m_pHead = nextNode;
	}
	if (pNode == m_pTail) {
		m_pTail = prevNode;
	}

	m_nSize --;

	if (ppObj) {
		*ppObj = pNode->data;
	}

	SAFE_DELETE( pNode );

	// return the next node
	return nextNode;
}