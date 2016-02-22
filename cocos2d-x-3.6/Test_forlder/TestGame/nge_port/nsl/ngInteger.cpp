/*
 *  ngInteger.cpp
 *  template
 *
 *  Created by Yuhui Wang on 7/18/10.
 *  Copyright 2010 Anansi Mobile. All rights reserved.
 *
 */

#include <nsl/ngInteger.h>

ngInteger::ngInteger()
: m_value(0)
{
}

ngInteger::ngInteger(int64 val)
: m_value(val)
{
}

ngInteger::ngInteger(const ngStringV2& str)
{
	m_value = str.GetInt();
}

int64 ngInteger::ParseInt(const ngStringV2& str)
{
	return str.GetInt();
}

int64 ngInteger::IntValue() const
{
	return m_value;
}