/*
 *  NGETypes.h
 *  NextGenEngine
 *
 *  Created by ppcfan on 09-3-10.
 *  Copyright 2009 DreaminGame. All rights reserved.
 *
 */
#ifndef __NGETYPES_H__
#define __NGETYPES_H__

/*
#if not ((defined NGE_PLATFORM_WIN32) && (defined NGE_PLATFORM_BADA))
#include <stdint.h>
#endif
*/
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <wchar.h>

#undef boolean
typedef unsigned char boolean;
#undef FALSE
#undef TRUE
#define TRUE        ((boolean)1)
#define FALSE       ((boolean)0)

typedef wchar_t wchar;

/*
#if (defined NGE_PLATFORM_WIN32) || (defined NGE_PLATFORM_BADA)
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;

typedef char int8_t;
typedef short int16_t;
typedef int int32_t;
typedef long long int64_t;
#endif //(defined NGE_PLATFORM_WIN32) || (defined NGE_PLATFORM_BADA)

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;
*/

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;
typedef unsigned long long uint64;

typedef signed char int8;
typedef signed short int16;
typedef signed int int32;
typedef signed long long int64;

typedef unsigned char u_char;

typedef int32 EventId;

typedef char NGCHAR;
typedef wchar NGWCHAR;
typedef const char* NGCSTR;
typedef char* NGSTR;
typedef wchar* NGWSTR;
typedef const wchar* NGWCSTR;
typedef void* NGPVOID;
typedef const void* NGCPVOID;

#if !defined(MAX_UINT8)
#define MAX_UINT8    ((uint8) 0xff)
#endif	// #if !defined(MAX_UINT8)

#if !defined(MAX_UINT16)
#define MAX_UINT16   ((uint16) 0xffff)
#endif	// #if !defined(MAX_UINT8)

#if !defined(MAX_UINT32)
#define MAX_UINT32   ((uint32) 0xffffffff)
#endif	// #if !defined(MAX_UINT8)

#if !defined(MAX_INT8)
#define MAX_INT8     ((int8) 0x7f)
#endif	// #if !defined(MAX_INT8)

#if !defined(MAX_INT16)
#define MAX_INT16    ((int16) 0x7fff)
#endif	// #if !defined(MAX_INT8)

#if !defined(MAX_INT32)
#define MAX_INT32    ((int32) 0x7fffffff)
#endif	// #if !defined(MAX_INT8)

#endif
