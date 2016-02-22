/*
 *  ngQuad.cpp
 *  template
 *
 *  Created by ppcfan on 10-8-4.
 *  Copyright 2010 Anansi Mobile. All rights reserved.
 *
 */

#include <nsl/ngQuad.h>
#include <math/ngMath.h>

ngQuad::ngQuad()
: m_p0(0, 0)
, m_p1(0, 0)
, m_p2(0, 0)
, m_p3(0, 0)
{
    UpdateBounds();
}

ngQuad::ngQuad( int16 x0, int16 y0, int16 x1, int16 y1, int16 x2, int16 y2, int16 x3, int16 y3 )
: m_p0(x0, y0)
, m_p1(x1, y1)
, m_p2(x2, y2)
, m_p3(x3, y3)
{
    UpdateBounds();
}

ngQuad::ngQuad( ngPoint& p0, ngPoint& p1, ngPoint& p2, ngPoint& p3)
: m_p0(p0)
, m_p1(p1)
, m_p2(p2)
, m_p3(p3)
{
    UpdateBounds();
}

ngQuad::ngQuad( ngPointf& p0, ngPointf& p1, ngPointf& p2, ngPointf& p3) {
    
    m_p0.Set(p0.m_x, p0.m_y);
    m_p1.Set(p1.m_x, p1.m_y);
    m_p2.Set(p2.m_x, p2.m_y);
    m_p3.Set(p3.m_x, p3.m_y);
    
    UpdateBounds();
}

ngQuad::ngQuad( const ngQuad &r )
{
	m_p0 = r.m_p0;
	m_p1 = r.m_p1;
	m_p2 = r.m_p2;
	m_p3 = r.m_p3;
    
    UpdateBounds();
}

ngQuad& ngQuad::operator=(const ngQuad& r) {
	m_p0 = r.m_p0;
	m_p1 = r.m_p1;
	m_p2 = r.m_p2;
	m_p3 = r.m_p3;
    
    UpdateBounds();

    return *this;
}

void ngQuad::UpdateBounds()
{
    // left
    ngPoint* point1 = m_p0.m_x < m_p1.m_x ? &m_p0 : &m_p1;
	ngPoint* point2 = m_p2.m_x < m_p3.m_x ? &m_p2 : &m_p3;
	
	m_pLeft = (point1->m_x < point2->m_x ? point1 : point2);
    
    // right
    point1 = m_p0.m_x > m_p1.m_x ? &m_p0 : &m_p1;
	point2 = m_p2.m_x > m_p3.m_x ? &m_p2 : &m_p3;
	
	m_pRight = (point1->m_x > point2->m_x ? point1 : point2);
    
    //top
    point1 = m_p0.m_y < m_p1.m_y ? &m_p0 : &m_p1;
	point2 = m_p2.m_y < m_p3.m_y ? &m_p2 : &m_p3;
	
	m_pTop = (point1->m_y < point2->m_y ? point1 : point2);
    
    //bottom
    point1 = m_p0.m_y > m_p1.m_y ? &m_p0 : &m_p1;
	point2 = m_p2.m_y > m_p3.m_y ? &m_p2 : &m_p3;
	
	m_pBottom = (point1->m_y > point2->m_y ? point1 : point2);
    
}

ngPoint* ngQuad::GetLeft()
{
	return m_pLeft;
}
ngPoint* ngQuad::GetRight()
{
    return m_pRight;
}

ngPoint* ngQuad::GetTop()
{
    return m_pTop;
}

ngPoint* ngQuad::GetBottom()
{
	return m_pBottom;
}

boolean ngQuad::Contains( const ngPoint &p ) const {
	if (m_p0 == m_p1 && m_p1 == m_p2 && m_p2 == m_p3 && m_p3 == m_p0)
	{
		return m_p0 == p;
	}
    
    //quad is a line
    if (m_p0.m_x == m_p1.m_x  && m_p1.m_x == m_p2.m_x && m_p2.m_x == m_p3.m_x) {
        if (p.m_x != m_p0.m_x) {
            return FALSE;
        }
        
        if (p.m_y < m_pTop->m_y || p.m_y > m_pBottom->m_y) {
            return FALSE;
        } 
            
        return TRUE;
    }
    
    if (m_p0.m_y == m_p1.m_y && m_p1.m_y == m_p2.m_y && m_p2.m_y == m_p3.m_y) {
        if (p.m_y != m_p0.m_y) {
            return FALSE;
        }
        
        if (p.m_x < m_pLeft->m_x || p.m_x > m_pRight->m_x) {
            return FALSE;
        } 
            
        return TRUE;
    }
	
	double areaTriangles = triangleArea(m_p0, m_p1, p) + triangleArea(m_p1, m_p2, p) + triangleArea(m_p2, m_p3, p) + triangleArea(m_p3, m_p0, p);
	double areaQuad = triangleArea(m_p0, m_p1, m_p2) + triangleArea(m_p2, m_p3, m_p0);
	
	return areaTriangles == areaQuad;
}

double ngQuad::triangleArea(const ngPoint& a, const ngPoint& b, const ngPoint& c) const {  
	return (ngMath::Abs((a.m_x * b.m_y + b.m_x * c.m_y + c.m_x * a.m_y - b.m_x * a.m_y - c.m_x * b.m_y - a.m_x * c.m_y) / 2.0));  
}

const ngQuad& ngQuad::Set( int16 x0, int16 y0, int16 x1, int16 y1, int16 x2, int16 y2, int16 x3, int16 y3 )
{
	m_p0.Set(x0, y0);
	m_p1.Set(x1, y1);
	m_p2.Set(x2, y2);
	m_p3.Set(x3, y3);
    
    UpdateBounds();
	
	return *this;
}

const ngQuad& ngQuad::Set( ngQuad &r )
{
	m_p0 = r.m_p0;
	m_p1 = r.m_p1;
	m_p2 = r.m_p2;
	m_p3 = r.m_p3;
    
    UpdateBounds();
	
	return *this;
}

void ngQuad::Move(int32 dx, int32 dy)
{
	m_p0.m_x += dx;
	m_p0.m_y += dy;
	m_p1.m_x += dx;
	m_p1.m_y += dy;
	m_p2.m_x += dx;
	m_p2.m_y += dy;
	m_p3.m_x += dx;
	m_p3.m_y += dy;
    
    UpdateBounds();
}