/*
 *  ngKeyFactory.h
 *  template
 *
 *  Created by Yuhui Wang on 10/14/10.
 *  Copyright 2010 Anansi Mobile. All rights reserved.
 *
 */

#ifndef __NGKEYFACTORY_H__
#define __NGKEYFACTORY_H__

#include "../core/NGE_Types.h"

class ngKey 
{
public:
	ngKey();
	ngKey(uint32 clazz, uint32 idd);
	virtual ~ngKey();
public:
	uint32 GetClass() const;
	uint32 GetId() const;
	boolean Equals(const ngKey& k);
	
private:
	uint32 m_class;
	uint32 m_id;
};

class ngKeyFactory 
{
public:
//	static uint32 StringToHash(NGCSTR str, boolean lower = FALSE);
	static ngKey* StringToKey(NGCSTR str);
	static NGSTR KeyToString(ngKey* key);
};



#endif //__NGKEYFACTORY_H__