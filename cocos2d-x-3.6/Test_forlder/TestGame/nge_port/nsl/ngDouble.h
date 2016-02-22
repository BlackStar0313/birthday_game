/*
 *  ngDouble.h
 *  template
 *
 *  Created by Yuhui Wang on 7/18/10.
 *  Copyright 2010 Anansi Mobile. All rights reserved.
 *
 */

#ifndef __NGDOUBLE_H__
#define __NGDOUBLE_H__

#include <core/NGE_Types.h>
#include <math/ngMath.h>
#include <nsl/ngString.h>

class ngDouble
{
public:
	ngDouble();
	ngDouble(double val);
	ngDouble(const ngStringV2& str);
	virtual ~ngDouble() {}
	
public:
	ngDouble& operator =(double val) { m_value = val; return *this; }
	boolean operator==(double val) { return (ngMath::Abs(m_value - val) < 0.00001); }

public:
	static double ParseDouble(const ngStringV2& str);
	double DoubleValue();
	
protected:
	double m_value;	
};


#endif //__NGDOUBLE_H__