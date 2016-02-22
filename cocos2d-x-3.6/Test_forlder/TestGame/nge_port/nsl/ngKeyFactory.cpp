/*
 *  ngKeyFactory.cpp
 *  template
 *
 *  Created by Yuhui Wang on 10/14/10.
 *  Copyright 2010 Anansi Mobile. All rights reserved.
 *
 */


#include <nsl/ngKeyFactory.h>
#include <string.h>
#include <core/NGE_Macros.h>
#include <nsl/nslUtils.h>

#ifndef WP8
static char* strrev ( char * str )
{
    char *start = str;
    char *left = str;
    char ch;
    while (*str++) /* find end of  string */
        ;
    str -= 2;
    while (left < str)
    {
        ch = *left;
        *left++ = *str;
        *str-- = ch;
    }
    return(start);
}
#endif // !WP8

#if defined (WP8)
#	define strrev _strrev
#endif

const NGCHAR ALPHABET[] = "ABCDEFGHIJKLMNOPQRSTUVWYZabcdefghijklmnopqrstuvwxyz0123456789-_";
const int32 BASE = strlen(ALPHABET);
const static int32 STRING_MAX_SIZE = 24;
const NGCHAR SYMBOL = 'X';

ngKey::ngKey()
: m_class(0)
, m_id(0)
{
}

ngKey::ngKey(uint32 clazz, uint32 idd)
: m_class(clazz)
, m_id(idd)
{
}

ngKey::~ngKey()
{
}

uint32 ngKey::GetClass() const
{
	return m_class;
}

uint32 ngKey::GetId() const
{
	return m_id;
}

boolean ngKey::Equals(const ngKey& k)
{
	return (m_class == k.GetClass() && m_id == k.GetId());
}

int32 IntegerToString(uint32 n, NGSTR* s)
{
	NGSTR str = *s;
	NGSTR p = str;
	int32 r;
	while (TRUE) {
		r = n % BASE;
		n = n / BASE;
		*p = ALPHABET[r];
		if (n == 0) {
			break;
		}
		p ++;
	}						
	str = strrev(str);
	return strlen(str);
}

uint32 StringToInteger(NGCSTR str, boolean lower = FALSE)
{
	uint32 n = 0;
	int32 len = strlen(str);
	NGCSTR p = str;
	for (int i = 0;i < len;i ++) {
		NGSTR pos = (NGSTR) strchr(ALPHABET, *p);
		if (pos == NULL) {
			break;
		}
		n = n * BASE + (pos - ALPHABET);
		p ++;
	}
	return n;
}


ngKey* ngKeyFactory::StringToKey(NGCSTR str)
{
	NGSTR s = (NGSTR) strchr(str, SYMBOL);
	s ++;
	uint32 clazz = StringToInteger(str);
	uint32 idd = StringToInteger(s);
	return DNEW(ngKey)(clazz, idd);
}

NGSTR ngKeyFactory::KeyToString(ngKey* key)
{
	NGSTR str = (NGSTR)DMALLOC(sizeof(NGCHAR) * STRING_MAX_SIZE);
	memset(str, 0, STRING_MAX_SIZE);
	NGSTR p = str;
	int32 len = IntegerToString(key->GetClass(), &str);
	p += len;
	*p = SYMBOL;
	p ++;
	len = IntegerToString(key->GetId(), &p);
	return str;	
}

//uint32 ngKeyFactory::StringToHash(NGCSTR str, boolean lower)
//{
//    if( lower )
//        return strIHashCode( str );
//    else
//        return strHashCode( str );
//}