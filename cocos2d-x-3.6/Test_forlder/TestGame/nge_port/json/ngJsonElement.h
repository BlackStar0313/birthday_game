/*
 *  ngJsonElement.h
 *  NextGenEngine
 *
 *  Created by Yuhui Wang on 5/26/10.
 *  Copyright 2010 DreaminGame. All rights reserved.
 *
 */

#ifndef __NGJSONELEMENT_H__
#define __NGJSONELEMENT_H__

#include <core/NGE_Types.h>
#include <core/NGE_NSL.h>
typedef struct json_object json_object;
//
//enum JSON_TYPE {
//	JSON_INVALID = -1,
//	JSON_NULL = 0,
//	JSON_STRING,
//	JSON_BOOLEAN,
//	JSON_INT,
//	JSON_DOUBLE,
//	JSON_HASH,
//	JSON_ARRAY,
//};

typedef enum _JSON_TYPE {
	JSON_INVALID = -1,
	JSON_NULL = 0,
	JSON_BOOLEAN,
	JSON_DOUBLE,
	JSON_INT,
	JSON_HASH,
	JSON_ARRAY,
	JSON_STRING
} JSON_TYPE;

class ngJsonNode  
{
public:
	ngJsonNode() {}
	virtual ~ngJsonNode() {}
	virtual void Destroy();
	
public:
	virtual JSON_TYPE GetType() = 0;
	virtual int32 Parse(json_object* pObj) = 0;
	virtual int32 ToString( ngStringV2& buff ) = 0;
};


class ngJsonArray : public ngJsonNode, public ngArrayList
{
public:
	ngJsonArray();
	virtual ~ngJsonArray();
	virtual void Destroy();

public:
	virtual JSON_TYPE GetType() { return JSON_ARRAY; }
	virtual int32 Parse(json_object* pObj);
	
	virtual int32 ToString( ngStringV2& buff );
	
protected:
	void Clear();
};

class ngJsonBoolean : public ngJsonNode, public ngBoolean
{
public:
	ngJsonBoolean() {}
	ngJsonBoolean(boolean val) { m_value = val; }
	virtual void Destroy();

	virtual JSON_TYPE GetType() { return JSON_BOOLEAN; }
	virtual int32 Parse(json_object* pObj);	
	virtual int32 ToString( ngStringV2& buff );	
};

class ngJsonDouble : public ngJsonNode , public ngDouble
{
public:
	ngJsonDouble() {}
	ngJsonDouble(double val) { m_value = val; }
	virtual void Destroy();

	virtual JSON_TYPE GetType() { return JSON_DOUBLE; }
	virtual int32 Parse(json_object* pObj);	
	virtual int32 ToString( ngStringV2& buff );
};

class ngJsonHash : public ngJsonNode , public ngNOHashMap
{
public:
	ngJsonHash();
	virtual ~ngJsonHash();
	virtual void Destroy();

public:
	virtual JSON_TYPE GetType() { return JSON_HASH; }
	virtual int32 Parse(json_object* pObj);
	void Set(NGCSTR key, NGCSTR value);
	void Set(NGCSTR key, NGWCSTR value);
	void Set(NGCSTR key, int64 value);
	void Set(NGCSTR key, int32 value);
	void Set(NGCSTR key, double value);
	void Set(NGCSTR key, void* value);		
	
	virtual int32 ToString( ngStringV2& buff );	
};


class ngJsonInteger : public ngJsonNode , public ngInteger
{
public:
	ngJsonInteger() {}
	ngJsonInteger(int64 val) { m_value = val; }
	virtual void Destroy();

	virtual JSON_TYPE GetType() { return JSON_INT; }
	virtual int32 Parse(json_object* pObj);
	virtual int32 ToString( ngStringV2& buff );
};

class ngJsonNull : public ngJsonNode
{
public:
	virtual void Destroy();
public:
	virtual JSON_TYPE GetType() { return JSON_NULL; }
	virtual int32 Parse(json_object* pObj);
	virtual int32 ToString( ngStringV2& buff ) { return NG_OK; }	
};

class ngJsonString : public ngJsonNode , public ngStringV2
{
public:
	ngJsonString();
	ngJsonString(NGCSTR str);
	ngJsonString(NGWCSTR str);
	virtual ~ngJsonString() {}
	virtual void Destroy();

	virtual JSON_TYPE GetType() { return JSON_STRING; }
	virtual int32 Parse(json_object* pObj);
	virtual int32 ToString( ngStringV2& buff );	
};


#endif //__NGJSONELEMENT_H__