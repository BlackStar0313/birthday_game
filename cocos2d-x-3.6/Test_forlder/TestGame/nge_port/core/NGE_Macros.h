/*
 *  NGEMacros.h
 *  NextGenEngine
 *
 *  Created by ppcfan on 09-3-10.
 *  Copyright 2009 DreaminGame. All rights reserved.
 *
 */

#ifndef __NGEMACROS_H__
#define __NGEMACROS_H__

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "../system/ngMemory.h"

using namespace std;
// macros to enable memory tracking (compiled out)
//#define DMALLOC(size)                  malloc(size)
//#define DFREE(ptr)                     free(ptr)
//
//#define DNEW						   new
//#define DDELETE(object)                delete (object)
//#define DNEWARR(t,s)                   new t[(s)]
//#define DDELETEARR(object)             delete[] (object)

#pragma mark - GNUC __attribute__

#if !defined(__GNUC__)
#   define __attribute__(x)  //nothing here
#endif

#define NGE_DEPRECATED  __attribute__((__deprecated__))
#define NGE_UNUSED  __attribute__((__unused__))
#define NGE_STR_FORMAT(format_index, args_index)    __attribute__((__format__(__printf__, format_index, args_index)))

#pragma mark -

#define DMEMCPY						   memcpy
#define DMEMSET						   memset
#define DMEMMOVE					   memmove

// extern c
#define EXTERNC_START					#ifdef __cplusplus \
										extern "C" { \
										#endif
#define EXTERNC_END						#ifdef __cplusplus \
										} \
										#endif

/*
 *	Safe delete
 */
#define SAFE_DELETE(p)       if (p) { DDELETE(p); (p) = NULL; }
#ifdef NGE_MEMORY_USAGE_CHECK
#define DELETE_OBJECT(object, clazz) \
		{ \
			if(object) { \
				(object)->~clazz(); \
				DFREE_EX(object, __FILE__, __LINE__); \
			} \
		}
#else
#define DELETE_OBJECT(object, clazz) \
		{ \
			if(object) { \
				(object)->~clazz(); \
				free(object); \
			} \
		}
#endif

#define LIST_CLEAR(list, clazz) \
        {   \
            ngIterator* pIter = (list)->Iterator(); \
            while (pIter->HasNext()) {  \
                clazz* pType = (clazz *)pIter->Next(); \
                SAFE_DELETE(pType); \
            }   \
            pIter->Destroy();   \
            (list)->Clear();    \
        }

#define ARRAY_LIST_CLEAR(_array, type) \
    {   \
        for (int32 _i = 0; _i < _array.Size(); _i++) {  \
            type* pType = (type*)_array[_i]; \
            SAFE_DELETE(pType); \
        }   \
        _array.Clear(); \
    }

#define SAFE_DELETE_ARRAY(p) if (p) { DDELETEARR(p); (p) = NULL; }

#define INIT_ARRAY(p, t, v, l)  memset(p, v, l * sizeof(t))

#define INIT_ARRAY_FOR(p, v, l)             \
    for (int32 _i = 0; _i < l; _i++) {      \
        p[_i] = v;                          \
    }


#define EQUALS(left, right)			(left == right)


// String utility
#ifdef NGE_PLATFORM_METRO
#	define STRDUP	_strdup
#	define strdup	_strdup
#else
#	define STRDUP	strdup
#endif
#define WCSBYTELEN(len)		(len * sizeof(wchar) / sizeof(uint8))
#define STRBYTELEN(len)		(len * sizeof(char) / sizeof(uint8))
#define BYTEWCSLEN(len)		(len * sizeof(uint8) / sizeof(wchar))
#define BYTESTRLEN(len)		(len * sizeof(uint8) / sizeof(char))
#define ASSERTWCSLEN(len)	NGASSERT( (len * sizeof(uint8)) % sizeof(wchar) == 0 )
#define TOWCHAR(hi,lo)		((hi << 8) + lo)
#define WCHAR_HI(wch)			(uint8)((wch >> 8) & 0xFF)
#define WCHAR_LO(wch)			(uint8)(wch & 0xFF)

// these probably won't work with big-endian devices
#define RGB_GET_BLUE(c)					( (int32) ( ( c ) & 0xff ) )
#define RGB_GET_GREEN(c)				( (int32) ( ( c >> 8 ) & 0xff ) )
#define RGB_GET_RED(c)					( (int32) ( ( c >> 16 ) & 0xff ) )
#define RGB_GET_ALPHA(c)                ( (int32) ( ( c >> 24 ) & 0xff ) )
#define MAKE_ARGB(a, r, g, b)			( (int32) ( ( ( a & 0xFF ) << 24 ) | ( ( r & 0xFF ) << 16 ) | ( ( g & 0xFF ) << 8 ) | ( ( b & 0xFF ) ) ) )
#define MAKE_RGB_COLOR(r, g, b)         ( (int32) ( ( ( r & 0xFF ) << 16 ) | ( ( g & 0xFF ) << 8 ) | ( ( b & 0xFF ) ) ) )

//debug macro
#if defined (DEBUG_LOG)
#   include <core/ngLogger.h>
#   if defined (NGE_PLATFORM_COCOA) || defined (NGE_PLATFORM_ANDROID)
#		define NG_DEBUG_LOG(fmt, args...) ngLogger::Log(fmt, ##args)
#	elif defined (NGE_PLATFORM_METRO)
#		define NG_DEBUG_LOG(fmt,...) ngLogger::Log(fmt,__VA_ARGS__)
#   else
#       define NG_DEBUG_LOG(fmt, args...) //unknown platform, do nothing here.
#   endif
#else
#   define NG_DEBUG_LOG(fmt, ...)
#endif

#ifdef DEBUG
#ifdef NGE_PLATFORM_METRO
#   define NGDBGPRINTF		printf
#   define NGASSERT(x)		{if(!(x)) { NG_DEBUG_LOG("%s, %s, %d", #x, __FILE__, __LINE__); abort(); }}
#else
#   define NGDBGPRINTF		printf
#   define NGASSERT(x)		{if(!(x)) { NG_DEBUG_LOG("%s, %s, %d", #x, __FILE__, __LINE__); abort(); }}
#endif
#else
#   define NGDBGPRINTF
#   define NGASSERT(x) 
#endif

#ifdef NGE_PLATFORM_METRO

#	ifndef M_PI
#		define M_PI 3.1415926
#	endif

#define snprintf(_dst, _limit, _fmt, ...) _snprintf_s((_dst), (_limit), (_limit) - 1, (_fmt), __VA_ARGS__)
#define  NG_BREAK_IF(cond)			if(cond) break;
#endif
#define DEGREES_TO_RADIANS(__ANGLE__) ((__ANGLE__) / 180.0 * M_PI)

#ifdef DEBUG
#   define  D_TIME(code)    \
                clock_t t_b, t_e;   \
                t_b = clock();      \
                do {                    \
                    code                \
                } while (0);            \
                t_e = clock();          \
                NG_DEBUG_LOG("[debug time] time : %ld\n", t_e - t_b);

#else
#   define  D_TIME(code)    code
#endif

#define ENABLE_MEM_CHEAT_PROTECT    1

#if ENABLE_MEM_CHEAT_PROTECT
#   if 1
/* simple way. */
#       define PROTECT_MEM_CHEAT_G(v)  ((v) - 58)
#       define PROTECT_MEM_CHEAT_V(v)  ((v) + 58)
//do {                            \
//      v = v + 58;               \
//} while (0);                    
#   else
/* more hard way, swip bit. */
/* tip: bug if bit is equal. */
#       define PROTECT_MEM_CHEAT_G(v)  (((v >> 8) & 0xFF) + (((v) & 0xFF) << 8) + ((v) & 0xFFFF0000))
#       define PROTECT_MEM_CHEAT_V(v)  (((v >> 8) & 0xFF) + (((v) & 0xFF) << 8) + ((v) & 0xFFFF0000))
#   endif
#else
#   define PROTECT_MEM_CHEAT_G(v)  (v)
#   define PROTECT_MEM_CHEAT_S(v)
#endif

#define NG_UNUSED(_var) (void)_var

/* time debug */
//<<
#ifdef DEBUG
#define time_debug_start        \
    clock_t _td_tb, _td_te;     \
    _td_tb = clock();

#define time_debug_log(_tag)    \
    _td_te = clock();           \
    NG_DEBUG_LOG("debug_log, time cost [%s]: %0.4f", _tag, (float)(_td_te - _td_tb) / CLOCKS_PER_SEC); \
    _td_tb = clock();
#else
#define time_debug_start

#define time_debug_log(_tag)

#endif
//>>

#endif //__NGEMACROS_H__