#include <nsl/ngHashMap.h>
#include <nsl/ngHashSet.h>
#include <nsl/ngCollection.h>
#include <nsl/ngHashMapIterator.h>
#include <nsl/ngComparator.h>
#include <nsl/ngLinkedList.h>

ngHashMap::ngHashMap()
: m_pSet(NULL)
, m_pKeys(NULL)
{
	m_pSet = DNEW(ngHashSet);
	m_pKeys = DNEW(ngLinkedList);
	
	Initialize();
}

ngHashMap::~ngHashMap()
{
	RemoveAll();
	
	SAFE_DELETE(m_pSet);
	SAFE_DELETE(m_pKeys);
}
void ngHashMap::Initialize(ngHashComparator* pComp)
{
	m_pSet->Initialize(pComp);
}

int32 ngHashMap::Put(void* pKey, void* pValue)
{
	ngMap::ngEntry e(pKey);
	ngMap::ngEntry* pEntry = (ngMap::ngEntry *)m_pSet->Contains(&e);
	if (pEntry != NULL) {
		pEntry->SetValue(pValue);
	}else{
		ngMap::ngEntry* pEntry = ngMap::ngEntry::CreateEntry(pKey);
		if (pEntry == NULL) {
			return NG_ERROR_NOMEM;
		}
		pEntry->SetValue(pValue);
		if (m_pSet->Add(pEntry) != NG_OK) {
			return NG_ERROR_FAILTURE;
		}

		// save the keys into a collection
		if (m_pKeys->Contains(pKey) == FALSE) {
			m_pKeys->Add(pKey);
		}
	}
	return NG_OK;
}

void* ngHashMap::Get(void* key)
{
	ngMap::ngEntry e(key);
	ngMap::ngEntry* pEntry = (ngMap::ngEntry *)m_pSet->Contains(&e);
	if (pEntry != NULL) {
		return pEntry->GetValue();
	}
	return NULL;
}

void* ngHashMap::Remove(void* key)
{
	ngMap::ngEntry e(key);
	ngMap::ngEntry* pEntry = (ngMap::ngEntry *)m_pSet->Contains(&e);
	if (pEntry != NULL) {
		m_pKeys->Remove(key);
		m_pSet->Remove(pEntry);		
		SAFE_DELETE(pEntry);
	}
	return NULL;
}

void ngHashMap::RemoveAll()
{
	ngIterator* pIterator = m_pSet->Iterator();
	while (pIterator->HasNext()) {
		ngMap::ngEntry* pEntry = (ngMap::ngEntry *)pIterator->Next();
		SAFE_DELETE(pEntry);
	}
	SAFE_DELETE(pIterator);
	m_pSet->Clear();
	m_pKeys->Clear();
}

boolean ngHashMap::ContainsKey(void* key)
{
	ngMap::ngEntry e(key);
	return (m_pSet->Contains(&e) != NULL);
}

ngIterator* ngHashMap::Iterator()
{
	return DNEW ngHashMapIterator(this);
}

boolean ngHashMap::ContainsValue(void* pValue, ngComparator* pComp)
{
	ngHashSetIterator it(m_pSet);
	if (pComp == NULL)
	{
		while (it.HasNext())
		{
			if (((ngMap::ngEntry*)it.Next())->GetValue() == pValue)
				return TRUE;
		}
	}
	else
	{
		while (it.HasNext())
		{
			if (pComp->Equals(((ngMap::ngEntry*)it.Next())->GetValue(), pValue))
				return TRUE;
		}
	}

	return FALSE;
}

ngCollection* ngHashMap::Keys()
{
	return m_pKeys;
}

uint32 ngHashMap::Size()
{
	return m_pSet->Size();
}