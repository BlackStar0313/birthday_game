//
//  ngRectf.cpp
//  streaking
//
//  Created by Chen Zhen on 11-9-21.
//  Copyright 2011年 Anansi. All rights reserved.
//

#include <nsl/ngRectf.h>

void ClipRegionToRegion( float start1, float size1, float& start2, float& size2 )
{
    if( start1 + size1 > start2 && start1 < start2 + size2 )
    {
        // save the size
        int initialSize2 = size2;
		
		// rectangles overlap, clip pRect to pClip
        if( start2 + size2 > start1 + size1 )
        {
            size2 = (start1 + size1) - start2;
        }
        if( start2 < start1 )
        {
            size2 -= start1 - start2;
            start2 = start1;
        }
		
        // don't allow the sizes to change sign
        if( (initialSize2 > 0 && size2 < 0)
		   || (initialSize2 < 0 && size2 > 0) )
        {
            size2 = 0;
        }
    }
    else
    {
        size2 = 0;
    }
}


void ngRectf::FitToRect( const ngRectf &r, float padding )
{
	// clamp to the right
	if( m_x + m_dx + padding > r.m_x + r.m_dx )
		m_x = ( r.m_x + r.m_dx ) - ( m_dx + padding );
	if( m_y + m_dy + padding > r.m_y + r.m_dy )
		m_y = ( r.m_y + r.m_dy ) - ( m_dy + padding );
	
	// clamp to the left
	if( m_x < r.m_x + padding )
		m_x = r.m_x + padding;
	if( m_y < r.m_y + padding )
		m_y = r.m_y + padding;
}


void ngRectf::Combine( const ngRectf &r )
{
	float x, y;
	float inputRightX, outputRightX;
	float inputBottomY, outputBottomY;
	
	// nothing to combine
	if( !r.m_dx || !r.m_dy )
		return;

#if 0
	if( !m_dx || !m_dy )
	{
		*this = r;
		return;
	}
#endif
	
	// adjust top left point
	if( r.m_x < m_x )
		x = r.m_x;
	else x = m_x;
	
	if( r.m_y < m_y )
		y = r.m_y;
	else y = m_y;
	
	inputRightX = r.m_x + r.m_dx;
	inputBottomY = r.m_y + r.m_dy;
	outputRightX = m_x + m_dx;
	outputBottomY = m_y + m_dy;
	
	if( inputRightX > outputRightX )
		outputRightX = inputRightX;
	if( inputBottomY > outputBottomY )
		outputBottomY = inputBottomY;
	
	// fix up output rect
	m_x = x;
	m_y = y;
	m_dx = outputRightX - m_x;
	m_dy = outputBottomY - m_y;
}


void ngRectf::Subtract( const ngRectf &r )
{
	if( r.m_x < m_x )
	{
		float diff = m_x - r.m_x;
		m_x += diff;
		m_dx -= diff;
	}
	else if( r.m_x > m_x )
	{
		m_dx -= r.m_dx;
	}
	else if( r.m_y < m_y )
	{
		float diff = m_y - r.m_y;
		m_y += diff;
		m_dy -= diff;
	}
	else if( r.m_y > m_y )
	{
		m_dy -= r.m_dy;
	}
}


void ngRectf::Clip( const ngRectf &r )
{
	ClipRegionToRegion( r.m_x, r.m_dx, m_x, m_dx );
	ClipRegionToRegion( r.m_y, r.m_dy, m_y, m_dy );
}


boolean ngRectf::Intersects( const ngRectf &r ) const
{
	if( ( m_x >= r.m_x && r.m_x + r.m_dx >= m_x ) || ( m_x < r.m_x && m_x + m_dx >= r.m_x ) )
	{
		if( ( m_y >= r.m_y && r.m_y + r.m_dy >= m_y ) || ( m_y < r.m_y && m_y + m_dy >= r.m_y ) )
			return TRUE;
	}
	
	return FALSE;
}

boolean ngRectf::Contains( const ngRectf &r ) const
{
	if( m_x <= r.m_x )
	{
		if( m_y <= r.m_y )
		{
			if( m_y + m_dy >= r.m_y + r.m_dy )
			{
				if( m_x + m_dx >= r.m_x + r.m_dx )
					return TRUE;
			}
		}
	}
	
	return FALSE;
}


boolean ngRectf::Contains( const ngPointf &p ) const
{
	return (p.m_x >= m_x && p.m_x <= m_x + m_dx && p.m_y >= m_y && p.m_y <= m_y + m_dy);
}
