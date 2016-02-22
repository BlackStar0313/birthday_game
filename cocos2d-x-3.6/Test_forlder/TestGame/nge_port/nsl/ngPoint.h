/*
 *  ngPoint.h
 *  template
 *
 *  Created by Yuhui Wang on 6/27/10.
 *  Copyright 2010 DreaminGame. All rights reserved.
 *
 */

#ifndef __NGPOINT_H__
#define __NGPOINT_H__

#include <math.h>

class ngPoint 
{
public:
	ngPoint():m_x(0), m_y(0) {}
	ngPoint(int32 x, int32 y): m_x(x), m_y(y) {}
	ngPoint(const ngPoint& another) { m_x = another.m_x; m_y = another.m_y; }
	ngPoint& operator= (const ngPoint& pt) { m_x = pt.m_x; m_y = pt.m_y; return *this; }
	virtual ~ngPoint() {}
	
public:
	boolean operator==( const ngPoint& p ) const;
	boolean operator!=( const ngPoint& p ) const;		
	
public:
	void Set(int32 x,int32 y) { m_x = x;m_y = y; }
	int32 Distance(const ngPoint& another) { return sqrt((float)((m_x - another.m_x) * (m_x - another.m_x) + (m_y - another.m_y) * (m_y - another.m_y))); }
	
public:
	int32 m_x;
	int32 m_y;	
};

inline 
boolean ngPoint::operator==( const ngPoint &p ) const
{
    return ( m_x == p.m_x && m_y == p.m_y );
}


inline 
boolean ngPoint::operator!=( const ngPoint &p ) const
{
	return !( *this == p );
}

#endif //__NGPOINT_H__