/*
*  ngSring.h
*  NextGenEngine
*
*  Created by cooler on 10-5-19.
*  Copyright 2010 DreaminGame. All rights reserved.
*
*/

#ifndef __NGSTRING_H__
#define __NGSTRING_H__

#if 1

#include "./ngStringV2.h"

typedef ngStringV2 ngString;

#else

/*
 * deprecated by Chen Zhen on 2014-01-23.
 * 
 * use
 *
 *  typedef ngStringV2 ngString;
 */

#include "../../headers/NGE_Core.h"
#include "../../headers/NGE_IO.h"

class ngString
{
public:
	ngString();
	ngString(const char* str);
	ngString(const wchar* str);
	ngString(const ngString& str);
	virtual ~ngString();

	void			Load(ngFileReader* file);
	int32			GetCharPtr(char* pData, int32 nData);
	const wchar*	ToWCharPtr();
	int32			GetLength();

	ngString		Trim();
	int32			IndexOf(const char* str, int32 fromIndex = 0);
	int32			IndexOf(const wchar* str, int32 fromIndex = 0);
	int32			IndexOf(char ch, int32 fromIndex = 0);
	int32			IndexOf(wchar ch, int32 fromIndex = 0);

	ngString		SubString(int32 beginIndex, int32 endIndex);

	boolean			HasPrefix(const wchar* str);
	boolean			HasPrefix(const char* str);

	ngString&		operator= (const ngString& str2);
	ngString&		operator= (const wchar* str2);
	ngString&		operator= (const char* str2);
	ngString&		operator+= (const ngString& str2);
	ngString&		operator+= (const wchar* str2);
	ngString&		operator+= (const char* str2);

	wchar			operator[](int32 idx);

	friend boolean	operator==(const ngString& str1, const ngString& str2);
	friend boolean	operator==(const ngString& str1, const char* str2);
	friend boolean	operator==(const ngString& str1, const wchar* str2);
	friend boolean	operator==(const char* str1, const ngString& str2);
	friend boolean	operator==(const wchar* str1, const ngString& str2);
	
	friend boolean	operator!=(const ngString& str1, const ngString& str2);
	friend boolean	operator!=(const ngString& str1, const char* str2);
	friend boolean	operator!=(const ngString& str1, const wchar* str2);
	friend boolean	operator!=(const char* str1, const ngString& str2);
	friend boolean	operator!=(const wchar* str1, const ngString& str2);
	
	friend ngString	operator+(const ngString& str1, const ngString& str2);
	friend ngString	operator+(const ngString& str1, const wchar* str2);
	friend ngString	operator+(const wchar* str1, const ngString& str2);
	friend ngString	operator+(const ngString& str1, const char* str2);
	friend ngString	operator+(const char* str1, const ngString& str2);

protected:
	void Free();
	void Concatenate(const wchar* str);
	void Concatenate(const char* str);
	boolean CompareCharPtr(const wchar* str1, const char* str2);
	boolean CompareCharPtr(const char* str1, const wchar* str2);
	boolean CompareWCharPtr(const wchar* str1, const wchar* str2);

	wchar*	m_str;
	int32	m_length;
};
#endif

#endif //__NGSTRING_H__
