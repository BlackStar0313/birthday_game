//
//  ngRectf.h
//  streaking
//
//  Created by Chen Zhen on 11-9-21.
//  Copyright 2011年 Anansi. All rights reserved.
//
#ifndef __NGRECTF_H__
#define __NGRECTF_H__

#include "../core/NGE_Types.h"
#include "./ngPointf.h"
#include "../math/ngMath.h"

class ngRectf
{
public:
	ngRectf();
	ngRectf( float x, float y, float dx, float dy );
	ngRectf( const ngRectf &r );
	
	void Clear();
	const ngRectf &Set( float x, float y, float dx, float dy );
	const ngRectf &SetEdges( float left, float top, float right, float bottom );
	const ngRectf &Set( const ngRectf &r );	
	
	float SetTop( float top );
	float SetBottom( float nottom );
	float SetLeft( float left );
	float SetRight( float right );
	
	float GetTop() const;
	float GetBottom() const;
	float GetLeft() const;
	float GetRight() const;
	float GetCenterX() const;
	float GetCenterY() const;
	
	float Area() const;
	
	void CenterX( float dx );
	void CenterY( float dy );
	void Center( float dx, float dy );
	
	void Move( float dx, float dy );
	void MoveUp( float padding );
	void MoveDown( float padding );
	void MoveLeft( float padding );
	void MoveRight( float padding );
	
	void Inset( float inset );
	void Inset( float insetX, float insetY );
	void Inset( float insetLeft, float insetTop, float insetRight, float insetBottom );
	
	void FitToRect( const ngRectf &r, float padding );
	void Combine( const ngRectf &r );
	void Subtract( const ngRectf &r );
	void Clip( const ngRectf &r );
	
	boolean Intersects( const ngRectf &r ) const;
	boolean Contains( const ngRectf &r ) const;
	boolean Contains( const ngPointf &p ) const;
	
	const ngRectf &operator=( const ngRectf &r );
	boolean operator==( const ngRectf &r ) const;
	boolean operator!=( const ngRectf &r ) const;
	
	float m_x;
	float m_y;
	float m_dx;
	float m_dy;
};


inline 
ngRectf::ngRectf()
{
	Clear();
}


inline 
ngRectf::ngRectf( float x, float y, float dx, float dy )
{
	Set( x, y, dx, dy );
}


inline 
ngRectf::ngRectf( const ngRectf &r )
{
	Set( r );
}


inline 
void ngRectf::Clear()
{
	m_x = m_y = m_dx = m_dy = 0;
}


inline 
const ngRectf &ngRectf::Set( float x, float y, float dx, float dy )
{
	m_x = x;
	m_y = y;
	m_dx = dx;
	m_dy = dy;
	
	return *this;
}


inline 
const ngRectf &ngRectf::Set( const ngRectf &r )
{
	m_x = r.m_x;
	m_y = r.m_y;
	m_dx = r.m_dx;
	m_dy = r.m_dy;
	
	return *this;
}


inline
const ngRectf &ngRectf::SetEdges( float left, float top, float right, float bottom )
{
    m_x = left;
    m_y = top;
    m_dx = right - left;
    m_dy = bottom - top;
	
    return *this;
}


inline
float ngRectf::SetTop( float top )
{
    m_dy += m_y - top;
    m_y = top;
    
	return top;
}


inline
float ngRectf::SetBottom( float bottom )
{
    m_dy = bottom - m_y;
	
    return bottom;
}


inline
float ngRectf::SetLeft( float left )
{
    m_dx += m_x - left;
    m_x = left;
	
	return left;
}     


inline
float ngRectf::SetRight( float right )
{
    m_dx = right - m_x;
	
    return right;
}


inline
float ngRectf::GetTop() const
{
    return m_y;
}


inline
float ngRectf::GetBottom() const
{
    return m_y + m_dy;
}


inline
float ngRectf::GetLeft() const
{
    return m_x;
}


inline
float ngRectf::GetRight() const
{
    return m_x + m_dx;
}

inline
float ngRectf::GetCenterX() const
{
    return m_x + ( m_dx  / 2 );
}

inline
float ngRectf::GetCenterY() const
{
    return m_y + ( m_dy / 2 );
}


inline
float ngRectf::Area() const
{
	return m_dx * m_dy;
}


inline 
void ngRectf::CenterX( float dx )
{
	m_x = ( dx - m_dx )  / 2;
}


inline
void ngRectf::CenterY( float dy )
{
	m_y = ( dy - m_dy )  / 2;
}


inline 
void ngRectf::Center( float dx, float dy )
{
	CenterX( dx );
	CenterY( dy );
}


inline 
void ngRectf::Move( float dx, float dy )
{
	m_x += dx;
	m_y += dy;
}


inline 
void ngRectf::MoveUp( float padding )
{
	m_y -= m_dy + padding;
}


inline 
void ngRectf::MoveDown( float padding )
{
	m_y += m_dy + padding;
}


inline 
void ngRectf::MoveLeft( float padding )
{
	m_x -= m_dx + padding;
}


inline 
void ngRectf::MoveRight( float padding )
{
	m_x += m_dx + padding;
}


inline 
void ngRectf::Inset( float inset )
{
	return Inset( inset, inset, inset, inset );
}


inline 
void ngRectf::Inset( float insetX, float insetY )
{
	Inset( insetX, insetY, insetX, insetY );
}


inline
void ngRectf::Inset( float insetLeft, float insetTop, float insetRight, float insetBottom )
{
	m_x += insetLeft;
    m_y += insetTop;
	m_dx = (float) ngMath::Max( 0, m_dx - ( insetLeft + insetRight ) );
	m_dy = (float) ngMath::Max( 0, m_dy - ( insetTop + insetBottom ) );
}


inline
const ngRectf &ngRectf::operator=( const ngRectf &r )
{
	m_x = r.m_x;
	m_y = r.m_y;
	m_dx = r.m_dx;
	m_dy = r.m_dy;
	
	return *this;
}


inline 
boolean ngRectf::operator==( const ngRectf &r ) const
{
    return ( m_x == r.m_x && m_y == r.m_y && m_dx == r.m_dx && m_dy == r.m_dy );
}


inline 
boolean ngRectf::operator!=( const ngRectf &r ) const
{
	return !( *this == r );
}


#endif //__NGRECTF_H__