#include <json/ngJsonRoot.h>
#include <json/ngJsonElement.h>
#include <json/ngJsonFactory.h>
#include <json/json-c/json.h>

ngJsonRoot::ngJsonRoot()
: m_pNode(NULL)
{

}

ngJsonRoot::~ngJsonRoot()
{
	if (m_pNode) {
		SAFE_DELETE(m_pNode);
	}
}

void ngJsonRoot::SetType(JSON_TYPE type)
{
	if (m_pNode) {
		m_pNode->Destroy();
	}
	m_pNode = ngJsonFactory::CreateJsonNode(type);
}

void ngJsonRoot::SetNode(ngJsonNode* pNode)
{
	m_pNode = pNode;
}

int32 ngJsonRoot::Parse(NGCSTR raw)
{
	// if it's not a json format, return with error.
	if (strstr(raw, "{") != raw && strstr(raw, "[") != raw) {
		return NG_ERROR_FAILTURE;
	}
	
	json_object *new_obj;

	new_obj = json_tokener_parse(raw);
//	printf("new_obj.to_string()=%s\n", json_object_to_json_string(new_obj));

	int type = json_object_get_type(new_obj);

	if (m_pNode) {
		SAFE_DELETE(m_pNode);
	}
	m_pNode = ngJsonFactory::CreateJsonNode(type);
	m_pNode->Parse(new_obj);

	json_object_put(new_obj);

	return NG_OK;
}

ngStringV2* ngJsonRoot::ToString()
{
	m_stringValue.Clear();
	m_pNode->ToString(m_stringValue);
	return &m_stringValue;
}