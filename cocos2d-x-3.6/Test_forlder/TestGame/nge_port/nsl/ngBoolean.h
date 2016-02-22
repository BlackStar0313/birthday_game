/*
 *  ngBoolean.h
 *  template
 *
 *  Created by Yuhui Wang on 7/18/10.
 *  Copyright 2010 Anansi Mobile. All rights reserved.
 *
 */

#ifndef __NGBOOLEAN_H__
#define __NGBOOLEAN_H__

#include "../core/NGE_Types.h"
#include "../nsl/ngString.h"

class ngBoolean 
{
public:
	ngBoolean();
	ngBoolean(boolean val);
	ngBoolean(const ngStringV2& str);
	virtual ~ngBoolean() {}
	
public:
	ngBoolean& operator =(boolean val) { m_value = val; return *this; }
	boolean operator==(boolean val) { return (boolean)(m_value == val); }

public:
	static boolean ParseBoolean(const ngStringV2& str);
	boolean BooleanValue();
	
protected:
	boolean m_value;
};


#endif //__NGBOOLEAN_H__