#include <json/ngJsonFactory.h>
#include <json/ngJsonElement.h>
#include <json/json-c/json.h>

ngJsonNode* ngJsonFactory::CreateJsonNode(int type)
{
	ngJsonNode* pNode = NULL;

	switch(type) {
		case json_type_null:
			pNode = DNEW(ngJsonNull);
			break;
		case json_type_boolean:
			pNode = DNEW(ngJsonBoolean);
			break;
		case json_type_int:
			pNode = DNEW(ngJsonInteger);
			break;
		case json_type_double:
			pNode = DNEW(ngJsonDouble);
			break;
		case json_type_string:
			pNode = DNEW(ngJsonString);
			break;
		case json_type_array:
			pNode = DNEW(ngJsonArray);
			break;
		case json_type_object:
			pNode = DNEW(ngJsonHash);
			break;
	}	

	return pNode;
}