/*
*  ngMap.h
*  NextGenEngine
*
*  Created by cooler on 10-5-16.
*  Copyright 2010 DreaminGame. All rights reserved.
*
*/

#ifndef __NGMAP_H__
#define __NGMAP_H__

#include "../core/NGE_Types.h"
#include "../core/NGE_Macros.h"

class ngCollection;
class ngComparator;
class ngIterator;

class ngMap
{
public:
	virtual int32 Put(void* key, void* value) = 0;
	virtual void* Get(void* key) = 0;
	virtual void* Remove(void* key) = 0;
	virtual void  RemoveAll() = 0;
	virtual ngIterator* Iterator() = 0;
	virtual boolean ContainsKey(void* key) = 0;
	virtual boolean ContainsValue(void* pValue, ngComparator* pComp) = 0;
	virtual ngCollection* Keys() = 0;
	virtual uint32 Size() = 0;

public:
	class ngEntry 
	{
	public:
		static ngEntry* CreateEntry(void* pKey) { ngEntry* pEntry = DNEW ngEntry(pKey); return pEntry; }
		ngEntry(void* pKey) { m_pKey = pKey; m_pValue = NULL;}
		virtual ~ngEntry() {}
	public:
		void* GetKey() { return m_pKey; }
		void* GetValue() { return m_pValue; }
		void  SetValue(void* pValue) { m_pValue = pValue; }

	protected:
		void* m_pKey;
		void* m_pValue;
	};
};

#endif //__NGMAP_H__
