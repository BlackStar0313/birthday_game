/*
 *  ngRect.h
 *  template
 *
 *  Created by Yuhui Wang on 7/3/10.
 *  Copyright 2010 Anansi Mobile. All rights reserved.
 *
 */

#ifndef __NGRECT_H__
#define __NGRECT_H__

#include "../core/NGE_Types.h"

#include "./ngPoint.h"
#include "./ngRectf.h"

class ngRect
{
public:
	ngRect();
	ngRect( int16 x, int16 y, int16 dx, int16 dy );
	ngRect( const ngRect &r );
    ngRect( const ngRectf &r );
	
	void Clear();
	const ngRect &Set( int16 x, int16 y, int16 dx, int16 dy );
	const ngRect &SetEdges( int16 left, int16 top, int16 right, int16 bottom );
	const ngRect &Set( const ngRect &r );	
	const ngRect &Set( const ngRectf &r );	
	
	int SetTop( int top );
	int SetBottom( int nottom );
	int SetLeft( int left );
	int SetRight( int right );
	
	int GetTop() const;
	int GetBottom() const;
	int GetLeft() const;
	int GetRight() const;
	int GetCenterX() const;
	int GetCenterY() const;
	
	int Area() const;
	
	void CenterX( int16 dx );
	void CenterY( int16 dy );
	void Center( int16 dx, int16 dy );
	
	void Move( int16 dx, int16 dy );
	void MoveUp( int16 padding );
	void MoveDown( int16 padding );
	void MoveLeft( int16 padding );
	void MoveRight( int16 padding );
	
	void Inset( int16 inset );
	void Inset( int16 insetX, int16 insetY );
	void Inset( int16 insetLeft, int16 insetTop, int16 insetRight, int16 insetBottom );
	
	void FitToRect( const ngRect &r, int16 padding );
	void Combine( const ngRect &r );
	void Subtract( const ngRect &r );
	void Clip( const ngRect &r );
	
	boolean Intersects( const ngRect &r ) const;
	boolean Contains( const ngRect &r ) const;
	boolean Contains( const ngPoint &p ) const;
    boolean Contains( const int32 &x, const int32 &y) const;
	
	const ngRect &operator=( const ngRect &r );
	boolean operator==( const ngRect &r ) const;
	boolean operator!=( const ngRect &r ) const;
	
	int16 m_x;
	int16 m_y;
	int16 m_dx;
	int16 m_dy;
};


inline 
ngRect::ngRect()
{
	Clear();
}


inline 
ngRect::ngRect( int16 x, int16 y, int16 dx, int16 dy )
{
	Set( x, y, dx, dy );
}


inline 
ngRect::ngRect( const ngRect &r )
{
	Set( r );
}

inline 
ngRect::ngRect( const ngRectf &r )
{
	Set( r );
}

inline 
void ngRect::Clear()
{
	m_x = m_y = m_dx = m_dy = 0;
}


inline 
const ngRect &ngRect::Set( int16 x, int16 y, int16 dx, int16 dy )
{
	m_x = x;
	m_y = y;
	m_dx = dx;
	m_dy = dy;
	
	return *this;
}


inline 
const ngRect &ngRect::Set( const ngRect &r )
{
	m_x = r.m_x;
	m_y = r.m_y;
	m_dx = r.m_dx;
	m_dy = r.m_dy;
	
	return *this;
}

inline 
const ngRect &ngRect::Set( const ngRectf &r )
{
	m_x = r.m_x;
	m_y = r.m_y;
	m_dx = r.m_dx;
	m_dy = r.m_dy;
	
	return *this;
}

inline
const ngRect &ngRect::SetEdges( int16 left, int16 top, int16 right, int16 bottom )
{
    m_x = left;
    m_y = top;
    m_dx = right - left;
    m_dy = bottom - top;
	
    return *this;
}

inline
int ngRect::SetTop( int top )
{
    m_dy += m_y - top;
    m_y = top;
    
	return top;
}


inline
int ngRect::SetBottom( int bottom )
{
    m_dy = bottom - m_y;
	
    return bottom;
}


inline
int ngRect::SetLeft( int left )
{
    m_dx += m_x - left;
    m_x = left;
	
	return left;
}     


inline
int ngRect::SetRight( int right )
{
    m_dx = right - m_x;
	
    return right;
}


inline
int ngRect::GetTop() const
{
    return m_y;
}


inline
int ngRect::GetBottom() const
{
    return m_y + m_dy;
}


inline
int ngRect::GetLeft() const
{
    return m_x;
}


inline
int ngRect::GetRight() const
{
    return m_x + m_dx;
}

inline
int ngRect::GetCenterX() const
{
    return m_x + ( m_dx >> 1 );
}

inline
int ngRect::GetCenterY() const
{
    return m_y + ( m_dy >> 1 );
}


inline
int ngRect::Area() const
{
	return m_dx * m_dy;
}


inline 
void ngRect::CenterX( int16 dx )
{
	m_x = ( dx - m_dx ) >> 1;
}


inline
void ngRect::CenterY( int16 dy )
{
	m_y = ( dy - m_dy ) >> 1;
}


inline 
void ngRect::Center( int16 dx, int16 dy )
{
	CenterX( dx );
	CenterY( dy );
}


inline 
void ngRect::Move( int16 dx, int16 dy )
{
	m_x += dx;
	m_y += dy;
}


inline 
void ngRect::MoveUp( int16 padding )
{
	m_y -= m_dy + padding;
}


inline 
void ngRect::MoveDown( int16 padding )
{
	m_y += m_dy + padding;
}


inline 
void ngRect::MoveLeft( int16 padding )
{
	m_x -= m_dx + padding;
}


inline 
void ngRect::MoveRight( int16 padding )
{
	m_x += m_dx + padding;
}


inline 
void ngRect::Inset( int16 inset )
{
	return Inset( inset, inset, inset, inset );
}


inline 
void ngRect::Inset( int16 insetX, int16 insetY )
{
	Inset( insetX, insetY, insetX, insetY );
}


inline
void ngRect::Inset( int16 insetLeft, int16 insetTop, int16 insetRight, int16 insetBottom )
{
	m_x += insetLeft;
    m_y += insetTop;
	m_dx = (int16) ngMath::Max( 0, m_dx - ( insetLeft + insetRight ) );
	m_dy = (int16) ngMath::Max( 0, m_dy - ( insetTop + insetBottom ) );
}


inline
const ngRect &ngRect::operator=( const ngRect &r )
{
	m_x = r.m_x;
	m_y = r.m_y;
	m_dx = r.m_dx;
	m_dy = r.m_dy;
	
	return *this;
}


inline 
boolean ngRect::operator==( const ngRect &r ) const
{
    return ( m_x == r.m_x && m_y == r.m_y && m_dx == r.m_dx && m_dy == r.m_dy );
}


inline 
boolean ngRect::operator!=( const ngRect &r ) const
{
	return !( *this == r );
}


#endif //__NGRECT_H__