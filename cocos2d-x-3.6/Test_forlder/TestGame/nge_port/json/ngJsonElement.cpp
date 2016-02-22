#include <json/ngJsonElement.h>
#include <json/ngJsonFactory.h>
#include <nsl/ngHashMapIterator.h>
#include <json/json-c/json.h>

//#define DEBUG_JSON_ELEMENT

/************************************************************************/
/* ngJsonNode                                                           */
/************************************************************************/

void ngJsonNode::Destroy()
{
	//DELETE_OBJECT(this, ngJsonNode);	
}

/************************************************************************/
/* ngJsonArray                                                          */
/************************************************************************/

ngJsonArray::ngJsonArray()
{
	
}

ngJsonArray::~ngJsonArray()
{
	Clear();
}

void ngJsonArray::Clear()
{
	int32 size = Size();
	for (int i = 0;i < size;i ++) {
		ngJsonNode* pNode = (ngJsonNode *)Get(i);
		pNode->Destroy();
	}		
}

void ngJsonArray::Destroy()
{
	DELETE_OBJECT(this, ngJsonArray);
}

int32 ngJsonArray::Parse(json_object* pObj)
{
	Clear();
	int32 size = json_object_array_length(pObj);
	
	for (int i = 0;i < size; i++) {
		json_object* pItem = json_object_array_get_idx(pObj,i);
		int type = JSON_NULL;
		if (pItem) {
			type = json_object_get_type(pItem);	
		}
		ngJsonNode* pNode = ngJsonFactory::CreateJsonNode(type);
		pNode->Parse(pItem);
#ifdef DEBUG_JSON_ELEMENT
		NGDBGPRINTF("[array] [%d]0x%x\n", pNode->GetType(), pNode);
#endif
		InsertAtEnd(pNode);
	}

	return NG_OK;
}

int32 ngJsonArray::ToString( ngStringV2& buff )
{
	buff += "[";
	int32 size = Size();
	for (int i = 0;i < size;i ++) {
		if (i != 0) {
			buff += ",";
		}
		ngJsonNode* pNode = (ngJsonNode *)Get(i);
		pNode->ToString(buff);
	}
	buff += "]";
	return NG_OK;
}

/************************************************************************/
/* ngJsonHash                                                           */
/************************************************************************/

ngJsonHash::ngJsonHash()
{
	Initialize();
}

//ngJsonHash::ngJsonHash(NGCSTR name)
//{
//	m_hash.Initialize();
//
//}
//
//ngJsonHash::ngJsonHash(NGWCSTR name)
//{
//	m_hash.Initialize();
//	SetName(name);
//}

ngJsonHash::~ngJsonHash()
{
#if 1
	ngHashMapIterator* pIterator = (ngHashMapIterator *)Iterator();
	while (pIterator->HasNext()) {
		ngHashMap::ngEntry* pEntry = pIterator->NextEntry();		
		ngJsonNode* pVal = (ngJsonNode *)pEntry->GetValue();
		pVal->Destroy();
	}
	SAFE_DELETE(pIterator);	
#endif
}

void ngJsonHash::Destroy()
{
	DELETE_OBJECT(this, ngJsonHash);
}

int32 ngJsonHash::Parse(json_object* pObj)
{
	json_object_object_foreach(pObj, key, val) {
//		printf("\t%s: %s\n", key, json_object_to_json_string(val));		
		if (val == NULL) {
			continue;
		}
		int type = json_object_get_type(val);
		ngJsonNode* pVal = ngJsonFactory::CreateJsonNode(type);
		pVal->Parse(val);
#ifdef DEBUG_JSON_ELEMENT
		NGDBGPRINTF("[hash] key=%s val=[%d]0x%x\n", key, pVal->GetType(), pVal);
#endif
		Set((const char *)key, pVal);
	}

	return NG_OK;
}

#if 1
void ngJsonHash::Set(NGCSTR key, NGCSTR value)
{
	ngJsonString* pValue = DNEW ngJsonString(value);
	Put(key, pValue);
}

void ngJsonHash::Set(NGCSTR key, NGWCSTR value)
{
	ngJsonString* pValue = DNEW ngJsonString(value);
	Put(key, pValue);
}

void ngJsonHash::Set(NGCSTR key, int64 value)
{
	ngJsonInteger* pValue = DNEW ngJsonInteger(value);
	Put(key, pValue);
}

void ngJsonHash::Set(NGCSTR key, int32 value)
{
	ngJsonInteger* pValue = DNEW ngJsonInteger(value);
	Put(key, pValue);
}

void ngJsonHash::Set(NGCSTR key, double value)
{
	ngJsonDouble* pValue = DNEW ngJsonDouble(value);
	Put(key, pValue);
}
#endif

void ngJsonHash::Set(NGCSTR key, void* value)
{
	Put(key, value);
}


//void* ngJsonHash::Get(NGCSTR key)
//{
//	void* pValue = Get(key);
//	return pValue;
//}

int32 ngJsonHash::ToString( ngStringV2& buff )
{
	buff += "{";
	
	boolean bFirst = TRUE;
	ngHashMapIterator* pIterator = (ngHashMapIterator *)Iterator();
	while (pIterator->HasNext()) {
		if (bFirst == FALSE) {
			buff += ",";
		}else {
			bFirst = FALSE;
		}
		ngHashMap::ngEntry* pEntry = pIterator->NextEntry();
		const char* pKey = (const char*)pEntry->GetKey();
		ngJsonNode* pValue = (ngJsonNode *)pEntry->GetValue();
		
		buff += "\"";
		buff += pKey;
		buff += "\"";
		buff += ":";
		pValue->ToString(buff);
	}
	SAFE_DELETE(pIterator); 
	
	buff += "}";
	
	return NG_OK;
}

/************************************************************************/
/* ngJsonDouble                                                         */
/************************************************************************/

int32 ngJsonDouble::Parse(json_object* pObj)
{
	double val = json_object_get_double(pObj);
	m_value = val;
#ifdef DEBUG_JSON_ELEMENT
	NGDBGPRINTF("[double] %f\n", val);
#endif
	return NG_OK;
}

int32 ngJsonDouble::ToString( ngStringV2& buff )
{
	buff += (double)m_value;
	return NG_OK;
}

void ngJsonDouble::Destroy()
{
	DELETE_OBJECT(this, ngJsonDouble);
}

/************************************************************************/
/* ngJsonInteger                                                        */
/************************************************************************/

int32 ngJsonInteger::Parse(json_object* pObj)
{
	int64 val = json_object_get_int(pObj);
	m_value = val;
#ifdef DEBUG_JSON_ELEMENT
	NGDBGPRINTF("[int] %lld\n", val);
#endif
	return NG_OK;
}

int32 ngJsonInteger::ToString( ngStringV2& buff )
{
	buff += m_value;
	return NG_OK;
}

void ngJsonInteger::Destroy()
{
	DELETE_OBJECT(this, ngJsonInteger);
}


/************************************************************************/
/* ngJsonBoolean                                                        */
/************************************************************************/

int32 ngJsonBoolean::Parse(json_object* pObj)
{
	boolean val = json_object_get_boolean(pObj);
	m_value = val;
#ifdef DEBUG_JSON_ELEMENT
	NGDBGPRINTF("[boolean] %d\n", val);
#endif
	return NG_OK;
}

int32 ngJsonBoolean::ToString( ngStringV2& buff )
{
	buff += (int64)m_value;
	return NG_OK;
}

void ngJsonBoolean::Destroy()
{
	DELETE_OBJECT(this, ngJsonBoolean);
}


/************************************************************************/
/* ngJsonNull                                                           */
/************************************************************************/

int32 ngJsonNull::Parse(json_object* pObj)
{
#ifdef DEBUG_JSON_ELEMENT
	NGDBGPRINTF("null");
#endif

	return NG_OK;
}

void ngJsonNull::Destroy()
{
	DELETE_OBJECT(this, ngJsonNull);
}

/************************************************************************/
/* ngJsonString                                                         */
/************************************************************************/

ngJsonString::ngJsonString()
{	
	
}

ngJsonString::ngJsonString(NGWCSTR val) 
{
	AssignString(val);
}

ngJsonString::ngJsonString(NGCSTR val)
{
	AssignString(val);
}

void ngJsonString::Destroy()
{
	DELETE_OBJECT(this, ngJsonString);
}

#if 0
void ngJsonString::SetValue(NGCSTR val)
{
	m_val = val;
}

void ngJsonString::SetValue(NGWCSTR val)
{
	m_val = val;
}
#endif

int32 ngJsonString::Parse(json_object* pObj)
{
	NGCSTR val = json_object_get_string(pObj);
	AssignString(val);
#ifdef DEBUG_JSON_ELEMENT
	NGDBGPRINTF("[string] %s\n", GetCString());
#endif
	return NG_OK;
}

int32 ngJsonString::ToString( ngStringV2& buff )
{
	buff += "\"";
	Replace("\\", "\\\\");
	Replace("\"", "\\\"");
	buff += GetCString(TRUE);
	buff += "\"";
	return NG_OK;
}
