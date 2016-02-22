#include <nsl/ngNOHashMap.h>
#include <nsl/nslUtils.h>
#include <nsl/ngHashSet.h>
#include <nsl/ngIterator.h>
#include <nsl/ngHashSet.h>
#include <nsl/ngCollection.h>

int32 ngNOHashComparator::Compare(void* a, void* b)
{
	ngHashMap::ngEntry* p1 = (ngHashMap::ngEntry *)a;
	ngHashMap::ngEntry* p2 = (ngHashMap::ngEntry *)b;

	const char* sa = (const char*)p1->GetKey();
	const char* sb = (const char*)p2->GetKey();
	return strcmp(sa,sb);
}

uint32 ngNOHashComparator::HashCode(void* pObj)
{
	ngHashMap::ngEntry* p1 = (ngHashMap::ngEntry *)pObj;
	return strHashCode((const char*)p1->GetKey());
}

ngNOHashMap::ngNOHashMap()
{
	m_pComparator = DNEW(ngNOHashComparator);
	
	Initialize();
}

void ngNOHashMap::Initialize(ngHashComparator* pComp /* = NULL */)
{
	if (pComp == NULL) {
		ngHashMap::Initialize(m_pComparator);	
	}else {
		ngHashMap::Initialize(pComp);
	}

}

ngNOHashMap::~ngNOHashMap()
{
	ngIterator* pIterator = Iterator();
	while (pIterator->HasNext()) {
		ngMap::ngEntry* pEntry = (ngMap::ngEntry *)pIterator->Next();
		NGCSTR k = (NGCSTR)pEntry->GetKey();
		SAFE_DELETE(k);
	}
	SAFE_DELETE(pIterator);
	
	SAFE_DELETE(m_pComparator);
}

int32 ngNOHashMap::Put(ngStringV2& key, void* val)
{
	const char* pKey = key.GetCString();
	int32 ret = Put(pKey, val);

	return ret;
}

int32 ngNOHashMap::Put(const char *key, void* val)
{
	NGCSTR mKey = key;
	if (ContainsKey(key) == FALSE) {
		mKey = STRDUP(key);		
	}
	return ngHashMap::Put((void *)mKey,val);
}

void* ngNOHashMap::Get(ngStringV2& key)
{
	const char* pKey = key.GetCString();
	void* ret = Get(pKey);
	return ret;
}

void* ngNOHashMap::Get(const char *key)
{
	return ngHashMap::Get((void*)key);
}

void* ngNOHashMap::Remove(ngStringV2& key)
{
	const char* pKey = key.GetCString();
	void* ret = Remove(pKey);
	return ret;
}

void* ngNOHashMap::Remove(const char* key)
{
	ngMap::ngEntry e((void*)key);
	ngMap::ngEntry* pEntry = (ngMap::ngEntry *)m_pSet->Contains(&e);
	if (pEntry != NULL) {
		m_pKeys->Remove((void*)key);
		m_pSet->Remove(pEntry);
		
		void* v = pEntry->GetValue();
		
		NGCSTR k = (NGCSTR)pEntry->GetKey();
		SAFE_DELETE(k);
		SAFE_DELETE(pEntry);
		
		return v;
	}
	return NULL;
}

void ngNOHashMap::RemoveAll()
{
	ngIterator* pIterator = m_pSet->Iterator();
	while (pIterator->HasNext()) {
		ngMap::ngEntry* pEntry = (ngMap::ngEntry *)pIterator->Next();
		NGCSTR k = (NGCSTR)pEntry->GetKey();
		SAFE_DELETE(k);
		SAFE_DELETE(pEntry);
	}
	SAFE_DELETE(pIterator);
	m_pSet->Clear();
	m_pKeys->Clear();
}

boolean ngNOHashMap::ContainsKey(ngStringV2& key)
{
	const char* pKey = key.GetCString();
	boolean ret = ContainsKey(pKey);
	return ret;
}

boolean ngNOHashMap::ContainsKey(const char* key)
{
	return ngHashMap::ContainsKey((void *)key);
}
