/*
 *  ngDouble.cpp
 *  template
 *
 *  Created by Yuhui Wang on 7/18/10.
 *  Copyright 2010 Anansi Mobile. All rights reserved.
 *
 */


#include <nsl/ngDouble.h>

ngDouble::ngDouble()
: m_value(0.0)
{
}

ngDouble::ngDouble(double val)
: m_value(val)
{
}

ngDouble::ngDouble(const ngStringV2& str)
{
	m_value = str.GetFloat();
}

double ngDouble::ParseDouble(const ngStringV2& str)
{
	return str.GetFloat();
}

double ngDouble::DoubleValue()
{
	return m_value;
}