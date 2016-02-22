//
//  ngPointf.h
//  streaking
//
//  Created by Chen Zhen on 11-9-21.
//  Copyright 2011年 Anansi. All rights reserved.
//

#ifndef __NGPOINTF_H__
#define __NGPOINTF_H__

#include <math.h>

class ngPointf 
{
public:
	ngPointf():m_x(0), m_y(0) {}
	ngPointf(float x, float y): m_x(x), m_y(y) {}
	ngPointf(const ngPointf& another) { m_x = another.m_x; m_y = another.m_y; }
	ngPointf& operator= (const ngPointf& pt) { m_x = pt.m_x; m_y = pt.m_y; return *this; }
	virtual ~ngPointf() {}
	
public:
	boolean operator==( const ngPointf& p ) const;
	boolean operator!=( const ngPointf& p ) const;		
	
public:
	void Set(float x,float y) { m_x = x;m_y = y; }
	float Distance(const ngPointf& another) { return sqrt((float)((m_x - another.m_x) * (m_x - another.m_x) + (m_y - another.m_y) * (m_y - another.m_y))); }
	
public:
	float m_x;
	float m_y;	
};

inline 
boolean ngPointf::operator==( const ngPointf &p ) const
{
    return ( m_x == p.m_x && m_y == p.m_y );
}


inline 
boolean ngPointf::operator!=( const ngPointf &p ) const
{
	return !( *this == p );
}

#endif //__NGPOINTF_H__