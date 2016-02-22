/*
 *  ngBoolean.cpp
 *  template
 *
 *  Created by Yuhui Wang on 7/18/10.
 *  Copyright 2010 Anansi Mobile. All rights reserved.
 *
 */

#include <nsl/ngBoolean.h>

ngBoolean::ngBoolean()
: m_value(FALSE)
{
}

ngBoolean::ngBoolean(boolean val)
: m_value(val)
{
}

ngBoolean::ngBoolean(const ngStringV2& str)
{
	m_value = ParseBoolean(str);	
}

boolean ngBoolean::ParseBoolean(const ngStringV2& str)
{
	if (str.ICompare("true") == 0 || str.ICompare("yes") == 0 || str.ICompare("1") == 0) {
		return TRUE;
	}
	return FALSE;
}

boolean ngBoolean::BooleanValue()
{
	return m_value;
}