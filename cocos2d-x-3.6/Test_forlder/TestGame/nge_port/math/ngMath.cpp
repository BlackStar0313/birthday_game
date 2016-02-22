/*
 *  ngMath.cpp
 *  NextGenEngine
 *
 *  Created by ppcfan on 09-4-2.
 *  Copyright 2009 DreaminGame. All rights reserved.
 *
 */

#include <math/ngMath.h>
#include <nsl/ngRandom.h>

int32 ngMath::Rand(int32 range)
{
    if (range == 0) return 0;

	int32 ret = ngRandom::GetInstance()->GetRand(range); 
	return ret;
}

float ngMath::InvSqrt(float x)
{
    union
    {
        int intPart;
        float floatPart;
    } convertor;
    
    convertor.floatPart = x;
    convertor.intPart = 0x5f3759df - (convertor.intPart >> 1);
    return convertor.floatPart*(1.5f - 0.4999f*x*convertor.floatPart*convertor.floatPart);
}

float ngMath::Sqrt(float value)
{
    return sqrtf(value);
}

ngPointf ngMath::RotatePointByAngle(ngPointf p, ngPointf pivot, float angle) {
    ngPointf r(p.m_x - pivot.m_x, p.m_y - pivot.m_y);
    float t = r.m_x;
    double cosa = cosf(angle), sina = sinf(angle);
    r.m_x = t * cosa - r.m_y * sina;
    r.m_y = t * sina + r.m_y * cosa;
    
    r.Set(r.m_x + pivot.m_x, r.m_y + pivot.m_y);

    return r;
}

float ngMath::Anglef(ngPointf sub) {
    if (sub.m_x == 0.0 && sub.m_y == 0.0) {
        return 0;
    }
    
    if (sub.m_x == 0.0) {
        return sub.m_y > 0 ? M_PI / 2 : -M_PI / 2;
    }
    
    if (sub.m_y == 0.0 && sub.m_x < 0) {
        return -M_PI;
    }
    
    float angle = atan(sub.m_y / sub.m_x);
    
    return sub.m_x < 0 ? angle + M_PI : angle;
}