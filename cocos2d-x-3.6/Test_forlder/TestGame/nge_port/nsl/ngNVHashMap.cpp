#include <nsl/ngNVHashMap.h>
#include <nsl/nslUtils.h>
#include <nsl/ngIterator.h>
#include <nsl/ngHashSet.h>
#include <nsl/ngCollection.h>

int32 ngNVHashComparator::Compare(void* a, void* b)
{
	ngHashMap::ngEntry* p1 = (ngHashMap::ngEntry *)a;
	ngHashMap::ngEntry* p2 = (ngHashMap::ngEntry *)b;
	
	const char* sa = (const char*)p1->GetKey();
	const char* sb = (const char*)p2->GetKey();
	return strcmp(sa,sb);
}

uint32 ngNVHashComparator::HashCode(void* pObj)
{
	ngHashMap::ngEntry* p1 = (ngHashMap::ngEntry *)pObj;
	return strHashCode((const char*)p1->GetKey());
}

ngNVHashMap::ngNVHashMap()
{
	m_pComparator = DNEW ngNVHashComparator;
	
	Initialize();
}

ngNVHashMap::~ngNVHashMap()
{
	ngIterator* pIterator = Iterator();
	while (pIterator->HasNext()) {
		ngMap::ngEntry* pEntry = (ngMap::ngEntry *)pIterator->Next();
		NGCSTR k = (NGCSTR)pEntry->GetKey();
		NGCSTR v = (NGCSTR)pEntry->GetValue();
		SAFE_DELETE(k);
		SAFE_DELETE(v);
	}
	SAFE_DELETE(pIterator);
	
	SAFE_DELETE(m_pComparator);
}

void ngNVHashMap::Initialize(ngHashComparator* pComp /* = NULL */)
{
	if (pComp == NULL) {
		pComp = m_pComparator;
	}
	ngHashMap::Initialize(pComp);
}

int32 ngNVHashMap::Put(ngString& key, ngString& val)
{
	char* pKey = GetAsciiFromString(key);
	char* pValue = GetAsciiFromString(val);
	
	int32 ret = Put(pKey, pValue);
	
	SAFE_DELETE(pKey);
	SAFE_DELETE(pValue);
	
	return ret;
}

int32 ngNVHashMap::Put(const char *key, const char *val)
{
	char* mKey = STRDUP(key);
	char* mVal = STRDUP(val);

	return ngHashMap::Put(mKey,mVal);
}

int32 ngNVHashMap::Put(const char* key, int64 val)
{
    ngStringV2 intVal(val);
    
    return Put(key, intVal.GetCString());
}

const char* ngNVHashMap::Get(ngString& key)
{
	char* pKey = GetAsciiFromString(key);
	const char* ret = Get(pKey);
	SAFE_DELETE(pKey);
	return ret;
}

const char* ngNVHashMap::Get(const char *key)
{
	return (const char*)ngHashMap::Get((void*)key);
}

void* ngNVHashMap::Remove(ngString& key)
{
	char* pKey = GetAsciiFromString(key);
	Remove(pKey);
	SAFE_DELETE(pKey);
	return NULL;
}

void* ngNVHashMap::Remove(const char* key)
{
	ngMap::ngEntry e((void*)key);
	ngMap::ngEntry* pEntry = (ngMap::ngEntry *)m_pSet->Contains(&e);
	if (pEntry != NULL) {
		m_pKeys->Remove((void*)key);
		m_pSet->Remove(pEntry);
		
		NGCSTR k = (NGCSTR)pEntry->GetKey();
		NGCSTR v = (NGCSTR)pEntry->GetValue();
		SAFE_DELETE(k);
		SAFE_DELETE(v);
		SAFE_DELETE(pEntry);
	}
	return NULL;
}

void ngNVHashMap::RemoveAll()
{
	ngIterator* pIterator = m_pSet->Iterator();
	while (pIterator->HasNext()) {
		ngMap::ngEntry* pEntry = (ngMap::ngEntry *)pIterator->Next();
		NGCSTR k = (NGCSTR)pEntry->GetKey();
		NGCSTR v = (NGCSTR)pEntry->GetValue();
		SAFE_DELETE(k);
		SAFE_DELETE(v);
		SAFE_DELETE(pEntry);
	}
	SAFE_DELETE(pIterator);
	m_pSet->Clear();
	m_pKeys->Clear();
}

boolean ngNVHashMap::ContainsKey(ngString& key)
{
	char* pKey = GetAsciiFromString(key);
	boolean ret = ContainsKey(pKey);
	SAFE_DELETE(pKey);
	return ret;
}

boolean ngNVHashMap::ContainsKey(const char* key)
{
	return ngHashMap::ContainsKey((void *)key);
}
