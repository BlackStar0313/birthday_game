/*
*  nslUtils.h
*  NextGenEngine
*
*  Created by cooler on 10-5-19.
*  Copyright 2010 DreaminGame. All rights reserved.
*
*/

#ifndef __NSLUTILS_H__
#define __NSLUTILS_H__

#include "../core/NGE_Types.h"
#include "../nsl/ngString.h"

extern uint8 s_to_lower_map[256];
extern uint8 s_to_upper_map[256];

#define nslToLower(ch)	(s_to_lower_map[(uint8) ch])
#define nslToUpper(ch)	(s_to_upper_map[(uint8) ch])


uint32				strHashCode(const char* pStr);
uint32				strIHashCode(const char* pStr);

uint32				Utf8ToWcs(const char *pInStr, wchar*& pOutWStr);
uint32				WcsToUtf8(const wchar *pInWStr, char*& pOutStr);

uint32              Utf8ToWcs( const uint8 *pIn, uint32 utflen, wchar *szDest, uint32 buflen );
uint32              WcsToUtf8( const wchar *szIn, uint32 wcslen, uint8 *pDest, uint32 buflen );

uint32              GetUtfLen( const wchar *szIn, uint32 wcslen );
uint32              GetWcsLen( const uint8 *pIn, uint32 utflen );

char*				GetAsciiFromString(ngString& str);

void				Swap(int32& a, int32& b);
void                Swap(float& a, float& b);

#if 0 //port to cocos2dx
//deprecated in cocos2dx port

int32				ComposeTransform(int32 first, int32 second);

void				TranslatePosWithTransformAndAlign(int32 transform, float& x, float& y, int32 w, int32 h, int32 align);
void                TranslatePosWithTransformAndAlign(int32 transform, int32& x, int32& y, int32 w, int32 h, int32 align);
void				TranslatePosWithAlign(float& x, float& y, float width, float height, int32 align);
void				TranslatePosWithAlign(int32& x, int32& y, int32 width, int32 height, int32 align);

#endif

int32				HashBytes( const char* begin, const char* end);

#endif //__NSLUTILS_H__