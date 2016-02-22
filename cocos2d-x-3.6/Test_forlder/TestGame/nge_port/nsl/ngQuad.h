/*
 *  ngQuad.h
 *  template
 *
 *  Created by ppcfan on 10-8-4.
 *  Copyright 2010 Anansi Mobile. All rights reserved.
 *
 */

#ifndef __NGQUAD_H__
#define __NGQUAD_H__

#include "../core/NGE_Defs.h"
#include "./ngPoint.h"
#include "./ngPointf.h"

class ngQuad
{
public:
	ngQuad();
	ngQuad( int16 x0, int16 y0, int16 x1, int16 y1, int16 x2, int16 y2, int16 x3, int16 y3 );
	ngQuad( ngPoint& p0, ngPoint& p1, ngPoint& p2, ngPoint& p3);
	ngQuad( ngPointf& p0, ngPointf& p1, ngPointf& p2, ngPointf& p3);
	ngQuad( const ngQuad &r );
    ngQuad& operator=(const ngQuad& r);

	const ngQuad &Set( int16 x0, int16 y0, int16 x1, int16 y1, int16 x2, int16 y2, int16 x3, int16 y3 );
	const ngQuad &Set( ngQuad &r );
	
	void Move(int32 dx, int32 dy);
	
	boolean Contains( const ngPoint &p ) const;
	
	ngPoint* GetLeft();
	ngPoint* GetRight();
	ngPoint* GetTop();
	ngPoint* GetBottom();
        
    ngPoint m_p0;
	ngPoint m_p1;
	ngPoint m_p2;
	ngPoint m_p3;
    
private:
	double	triangleArea(const ngPoint& a, const ngPoint& b, const ngPoint& c) const;
    void    UpdateBounds();

    
    ngPoint* m_pLeft;
    ngPoint* m_pRight;
    ngPoint* m_pTop;
    ngPoint* m_pBottom;

};

#endif