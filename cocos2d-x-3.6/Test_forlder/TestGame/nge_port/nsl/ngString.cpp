
#if 0

/* deprecated by Chen Zhen on 2014-01-23. */

#include <nsl/ngString.h>
#include <nslUtils.h>

ngString::ngString()
{
	m_str = NULL;
	m_length = 0;
}

ngString::ngString(const ngString& str)
{
	m_str = NULL;
	m_length = 0;

	Concatenate(str.m_str);
}

ngString::ngString(const char* str)
{
	m_str = NULL;
	m_length = 0;

	Concatenate(str);
}

ngString::ngString(const wchar* str)
{
	m_str = NULL;
	m_length = 0;
	
	Concatenate(str);
}

ngString::~ngString()
{
	Free();
}

void ngString::Free()
{
	m_length = 0;
	SAFE_DELETE_ARRAY(m_str);
}

void ngString::Load(ngFileReader* file)
{
	int32 utflen = file->ReadInt16BE();
	char* buffer = DNEWARR(char,utflen + 1);
	for (int i = 0; i < utflen; i++)
	{
		buffer[i] = file->ReadUInt8();
	}
	buffer[utflen] = 0;
	
	m_length = Utf8ToWcs(buffer, m_str);
	SAFE_DELETE_ARRAY(buffer);
}

int32 ngString::GetCharPtr(char* pData, int32 nData)
{
	char* buffer = NULL;
	int32 buflen = WcsToUtf8(m_str, buffer) + 1;
	if (pData)
	{
		if (nData < buflen)
		{
			buffer[nData - 1] = 0;
			buflen = nData;
		}
		
		strcpy(pData, buffer);
	}
	SAFE_DELETE_ARRAY(buffer);
	return buflen;
}

const wchar* ngString::ToWCharPtr()
{
	return m_str;
}

int32 ngString::GetLength()
{
	return m_length;
}

void ngString::Concatenate(const wchar* str)
{
	if (str)
	{
		int32 strLen = wcslen(str);
		int32 newLen = strLen + m_length;
		wchar* newStr = DNEWARR(wchar, newLen + 1);
		
		*newStr = 0;
		
		if (m_str)
		{
			wcscpy(newStr, m_str);
			Free();
		}
		
		m_str = newStr;
		wcscat(m_str, str);
		
		m_length = newLen;
		m_str[m_length] = 0;
	}
}

void ngString::Concatenate(const char* str)
{
	if (str)
	{
		wchar* buffer = NULL;
		int32 strLen = Utf8ToWcs(str, buffer);
		int32 newLen = strLen + m_length;
		wchar* newStr = DNEWARR(wchar, newLen + 1);
		
		*newStr = 0;
		
		if (m_str)
		{
			wcscpy(newStr, m_str);
			Free();
		}
		
		m_str = newStr;
		wcscat(m_str, buffer);
		
		m_length = newLen;
		m_str[m_length] = 0;
		
		SAFE_DELETE_ARRAY(buffer);
	}
}

ngString& ngString::operator = (const ngString& str2)
{
	Free();
	Concatenate(str2.m_str);
	return *this;
}

ngString& ngString::operator = (const wchar* str)
{
	Free();
	Concatenate(str);
	return *this;
}

ngString& ngString::operator = (const char* str)
{
	Free();
	Concatenate(str);
	return *this;
}

ngString& ngString::operator +=(const ngString &str2)
{
	Concatenate(str2.m_str);
	return *this;
}

ngString& ngString::operator +=(const wchar* str2)
{
	Concatenate(str2);
	return *this;
}

ngString& ngString::operator +=(const char* str2)
{
	Concatenate(str2);
	return *this;
}

wchar ngString::operator [](int32 idx)
{
	if (idx >= 0 && idx < m_length)
		return m_str[idx];
	else
		return 0;
}

boolean CompareWCharPtr(const wchar* str1, const wchar* str2)
{
	return (wcscmp( str1, str2 ) ? FALSE : TRUE);
}

boolean CompareCharPtr(const wchar* str1, const char* str2)
{	
	wchar* buffer = NULL;
	Utf8ToWcs(str2, buffer);
	boolean isEqual = wcscmp( str1, buffer ) ? FALSE : TRUE;
	SAFE_DELETE_ARRAY(buffer);
	
	return isEqual;
}

boolean CompareCharPtr(const char* str1, const wchar* str2)
{
	return CompareCharPtr(str2, str1);
}

boolean operator==(const ngString& str1, const ngString& str2)
{
	return CompareWCharPtr(str1.m_str, str2.m_str);
}

boolean operator==(const ngString& str1, const wchar* str2)
{
	return CompareWCharPtr(str1.m_str, str2);
}

boolean operator==(const ngString& str1, const char* str2)
{
	return CompareCharPtr(str1.m_str, str2);
}

boolean operator==(const wchar* str1, const ngString& str2)
{
	return CompareWCharPtr(str1, str2.m_str);
}

boolean operator==(const char* str1, const ngString& str2)
{
	return CompareCharPtr(str1, str2.m_str);
}

boolean operator!=(const ngString& str1, const ngString& str2)
{
	return !CompareWCharPtr(str1.m_str, str2.m_str);
}

boolean operator!=(const ngString& str1, const wchar* str2)
{
	return !CompareWCharPtr(str1.m_str, str2);
}

boolean operator!=(const ngString& str1, const char* str2)
{
	return !CompareCharPtr(str1.m_str, str2);
}

boolean operator!=(const wchar* str1, const ngString& str2)
{
	return !CompareWCharPtr(str1, str2.m_str);
}

boolean operator!=(const char* str1, const ngString& str2)
{
	return !CompareCharPtr(str1, str2.m_str);
}

ngString ngString::Trim()
{
	int first = 0;
	int last = m_length - 1;

	while
		(
		first < m_length
		&&	
		(
		m_str[first] == '\t'
		|| m_str[first] == '\n' 
		|| m_str[first] == '\f' 
		|| m_str[first] == '\r' 
		|| m_str[first] == ' '
		)
		)
	{
		first++;
	}

	while
		(
		last >= 0
		&&	
		(
		m_str[last] == '\t' 
		|| m_str[last] == '\n' 
		|| m_str[last] == '\f' 
		|| m_str[last] == '\r' 
		|| m_str[last] == ' '
		)
		)
	{
		last--;
	}

	return SubString( first, last );
}

int32 ngString::IndexOf(const wchar* str, int32 fromIndex /* = 0 */)
{
	int32 strLen = wcslen( str );
	
	if( strLen == 0 && fromIndex <= m_length )
		return fromIndex;
	
	int32 i = fromIndex;
	int32 thisLen = m_length - strLen;
	
	while( i < thisLen )
	{
		boolean match = TRUE;
		const wchar* a = str;
		const wchar* b = m_str + i;
		
		for( int j = 0; j < strLen; ++j )
		{
			if( *a != *b )
			{
				match = FALSE;
				if ( j == 0 )
					i++;
				else
					i += j;
				break;
			}
			a++;
			b++;
		}
		
		if( match == TRUE )
			return i;
	}
	
	return -1;
}

int32 ngString::IndexOf(const char* str, int32 fromIndex /* = 0 */)
{	
	wchar* buffer = NULL;
	Utf8ToWcs(str, buffer);
	int32 index = IndexOf(buffer, fromIndex);
	SAFE_DELETE_ARRAY(buffer);
	
	return index;
}

int32 ngString::IndexOf(wchar ch, int32 fromIndex /* = 0 */)
{
	int32 i = fromIndex;
	
	while (i < m_length)
	{
		if (ch == *(m_str + i))
		{
			return i;
		}
	}
	
	return -1;
}

int32 ngString::IndexOf(char ch, int32 fromIndex /* = 0 */)
{
	return IndexOf((wchar)ch, fromIndex);
}

boolean ngString::HasPrefix(const wchar* str)
{
	return (IndexOf(str) == 0);
}

boolean ngString::HasPrefix(const char* str)
{
	return (IndexOf(str) == 0);
}

ngString ngString::SubString(int32 beginIndex, int32 endIndex)
{
	if( m_length > 0 )
	{
		if( beginIndex	< 0 )
			beginIndex	= 0;
		if( endIndex	< 0 )
			endIndex	= 0;
		
		int32 span = endIndex - beginIndex;
		int32 dir	= ( span < 0 )? -1 : 1;
		int32 spanLimit = span * dir + 1;
		int32 spanCount = 0;
		int32 size = 0;
		const wchar* currSrc = m_str + beginIndex;
		wchar* subStr = DNEWARR(wchar, spanLimit + 1);
		wchar* currDest = subStr;
		
		while( spanCount <	spanLimit )
		{
			if( ( currSrc <= ( m_str + m_length ) )	 &&  ( currSrc >= m_str ) )
			{
				*currDest = *currSrc;
				currDest++;
				size++;
			}
			
			currSrc	+= dir;
			spanCount++;
		}
		
		currDest = subStr + size;
		*currDest = NULL;
		
		ngString subString( subStr );
		
		DDELETEARR(subStr);
		
		return ngString( subString );
	}
	else
	{
		return ngString( "" );
	}
}

#endif