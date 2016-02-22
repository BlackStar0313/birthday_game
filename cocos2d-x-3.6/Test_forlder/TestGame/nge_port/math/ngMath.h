/*
 *  ngMath.h
 *  NextGenEngine
 *
 *  Created by ppcfan on 09-4-2.
 *  Copyright 2009 DreaminGame. All rights reserved.
 *
 */

#ifndef __CMATH_H__
#define __CMATH_H__

#include "../core/NGE_Types.h"
#include "../nsl/ngPointf.h"

#ifndef M_PI
#define M_PI 3.14159
#endif

class ngMath {
public:
	static int32 Abs(int32 value) { return value >= 0 ? value : -value; }
	static int32 Max(int32 lvalue, int32 rvalue) { return lvalue > rvalue ? lvalue : rvalue; }
	static int32 Min(int32 lvalue, int32 rvalue) { return lvalue < rvalue ? lvalue : rvalue; }
	static int32 Square(int32 value) { return value * value; }
    static float InvSqrt(float value);
    static float Sqrt(float value);
	static int32 Rand(int32 range) ;
	
	static double Abs(double value) { return value >= 0.0 ? value : -value; }
    
    /*!
     @function RotatePointByAngle
     @param p 被旋转点
     @param pivot 旋转中心点
     @param angle 旋转角度
     @abstract 将p点绕pivot旋转角度angle.
     @discussion
     @result 旋转后的点
     */
//    static ngPointf RotatePointByAngle(const ngPointf& p, const ngPointf& pivot, float angle);
    static ngPointf RotatePointByAngle(ngPointf p, ngPointf pivot, float angle);
    
    static float Anglef(ngPointf sub);
};

#endif