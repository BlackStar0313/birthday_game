
#include <nsl/ngStringV2.h>
#include <nsl/nslUtils.h>
#include <nsl/ngHashMap.h>
#include <utils/base64.h>

#include <nsl/ngArrayList.h>

int32 ngStringV2Comparator::Compare(void* a, void* b)
{
	ngStringV2* p1 = (ngStringV2 *)a;
	ngStringV2* p2 = (ngStringV2 *)b;
	return p1->Compare(*p2);
}

uint32 ngStringV2Comparator::HashCode(void* pObj)
{
	ngHashMap::ngEntry* pEntry = (ngHashMap::ngEntry *)pObj;
	ngStringV2* p1 = (ngStringV2 *)pEntry->GetKey();
	const char* str = p1->GetCString();
	return strHashCode(str);
}

ngStringV2::ngStringV2()
{
	m_nFormat = ASCII;
	m_nLength = 0;
}

ngStringV2::ngStringV2(const ngStringV2& another)
{
	uint8* buffer = ((ngByteBuffer&)another).GetBuffer();
	int32 length = ((ngByteBuffer&)another).GetLimit();
	AssignString((const uint8*)buffer, length);
	
	m_nFormat = another.GetFormat();
}

ngStringV2::ngStringV2(NGCSTR str)
{
	AssignString(str);
}

ngStringV2::ngStringV2(NGCSTR str, int32 length)
{
	AssignString(str,length);
}

ngStringV2::ngStringV2(NGWCSTR str)
{
	AssignString(str);
}

ngStringV2::ngStringV2(NGWCSTR str, int32 length)
{
	AssignString(str,length);
}

ngStringV2::ngStringV2(int64 val) 
{
	NGCHAR strTmp[20];
	sprintf(strTmp, "%lld", val);
	AssignString(strTmp);
}

ngStringV2::ngStringV2(double val)
{
	NGCHAR strTmp[20];
	sprintf(strTmp, "%f", val);
	AssignString(strTmp);
}

ngStringV2::~ngStringV2()
{
	Clear();
}

void ngStringV2::Clear()
{
	DeleteBuffer();
	m_nLength = 0;
}

NGCHAR ngStringV2::CharAt(int32 index)
{
	NGASSERT( m_nFormat == ASCII);
	return (NGCHAR)Get(index);
}

NGWCHAR ngStringV2::WcharAt(int32 index)
{
	NGASSERT( m_nFormat == WCS );
	uint8 lo = Get(index * 2);
	uint8 hi = Get(index * 2 + 1);
	return (NGWCHAR)TOWCHAR(hi,lo);
}

int32 ngStringV2::SetCharAt(NGCHAR ch, int32 index)
{
	return Set(index, (uint8)ch);
}

int32 ngStringV2::SetWcharAt(NGWCHAR ch, int32 index)
{
	uint8 hi = WCHAR_HI(ch);
	uint8 lo = WCHAR_LO(ch);

	// we assume it saves as [lo][hi] little endian
	if (Set(index * 2, lo) == NG_OK && Set(index * 2 + 1, hi) == NG_OK) {
		return NG_OK;
	}
	return NG_ERROR_FAILTURE;
}

int32 ngStringV2::GetCharacterAt(int32 index)
{
	if (m_nFormat == ASCII)
		return CharAt(index);
	else
		return WcharAt(index);
}

void ngStringV2::ToLower(NGSTR str)
{
	while (*str) {
		*str = nslToLower(*str);
		str ++;
	}
}

void ngStringV2::ToUpper(NGSTR str)
{
	while (*str) {
		*str = nslToUpper(*str);
		str ++;
	}
}

void ngStringV2::ToLower()
{
	NGASSERT( m_nFormat == ASCII );
	ToLower((NGSTR)m_pBuffer); 
}

void ngStringV2::ToUpper()
{
	NGASSERT( m_nFormat == WCS );
	ToUpper((NGSTR)m_pBuffer);
}

int32 ngStringV2::Compare(ngStringV2& targetStr)
{
	NGASSERT(m_nFormat == targetStr.GetFormat());
	if (m_nFormat == ASCII) {
		return Compare(targetStr.GetCString());
	}else if (m_nFormat == WCS) {
		return Compare(targetStr.GetWcs());
	}
	// can't be here
	NGASSERT(0);
	return -1;
}

int32 ngStringV2::Compare(NGCSTR targetStr)
{
	NGASSERT( m_nFormat == ASCII );
	if (targetStr == NULL && m_pBuffer == NULL) {
		return 0;
	}
	if (targetStr == NULL || m_pBuffer == NULL) {
		return 1;
	}
	return strcmp((NGCSTR)m_pBuffer, targetStr);
}

int32 ngStringV2::Compare(NGWCSTR targetStr)
{
	NGASSERT( m_nFormat == WCS );
	if (targetStr == NULL && m_pBuffer == NULL) {
		return 0;
	}
	if (targetStr == NULL || m_pBuffer == NULL) {
		return 1;
	}
	return wcscmp((NGWCSTR)m_pBuffer, targetStr);
}

int32 ngStringV2::ICompare(ngStringV2& targetStr)
{
	NGASSERT( m_nFormat == ASCII );
	return ICompare(targetStr.GetCString());
}

int32 ngStringV2::ICompare(NGCSTR targetStr) const
{
	NGASSERT( m_nFormat == ASCII );
	if (targetStr == NULL && m_pBuffer == NULL) {
		return 0;
	}
	if (targetStr == NULL || m_pBuffer == NULL) {
		return 1;
	}

    //here need a cross-platform strcasecmp, temp use strcmp
	//return ng_strcasecmp((NGCSTR)m_pBuffer, targetStr);
    return strcmp((NGCSTR)m_pBuffer, targetStr);
}

ngStringV2& ngStringV2::Append(NGCHAR ch)
{
	NGASSERT(m_nFormat == ASCII);
	NGCHAR strTemp[2];
	strTemp[0] = ch;
	strTemp[1] = NULL;
	AppendString(strTemp);

	return *this;
}

ngStringV2& ngStringV2::Append(NGWCHAR ch)
{
	NGASSERT(m_nFormat == WCS);
	NGWCHAR strTemp[2];
	strTemp[0] = ch;
	strTemp[1] = NULL;
	AppendString(strTemp);

	return *this;
}

int32 ngStringV2::Find(ngStringV2& targetStr, int32 offset /* = 0 */)
{
	if (m_nFormat == WCS) {
		return Find(targetStr.GetWcs(), offset);
	} else {
		return Find(targetStr.GetCString(), offset);
	}
}

int32 ngStringV2::Find(NGCSTR targetStr, int32 offset /* = 0 */)
{
	return Find((NGSTR)m_pBuffer, targetStr, offset);
}

int32 ngStringV2::Find(NGWCSTR targetStr, int32 offset /* = 0 */)
{
	return Find((NGWCSTR)m_pBuffer, targetStr, offset);
}

int32 ngStringV2::Find(NGCSTR sourceStr, NGCSTR targetStr, int32 offset)
{
	if (NULL == sourceStr || NULL == targetStr || offset >= strlen(sourceStr)) {
		return -1;
	}

	NGSTR result = strstr((NGSTR)(sourceStr + offset), targetStr);
	return (NULL == result) ? -1 : (int32)(result - (NGSTR)sourceStr);
}

int32 ngStringV2::Find(NGWCSTR sourceStr, NGWCSTR targetStr, int32 offset /* = 0 */)
{
	if (NULL == sourceStr || NULL == targetStr || offset >= wcslen(sourceStr)) {
		return -1;
	}

	NGWSTR result = wcsstr((NGWSTR)(sourceStr + offset), targetStr);
	return (NULL == result) ? -1 : (int32)(result - (NGWSTR)sourceStr);
}

void ngStringV2::SetBuffer(NGSTR buffer, int32 length)
{
	CopyBuffer((const uint8*)buffer, STRBYTELEN(length));
}

void ngStringV2::SetBuffer(NGWSTR buffer, int32 length)
{
	CopyBuffer((const uint8*)buffer, WCSBYTELEN(length));
}

uint32 ngStringV2::GetLength()
{
	return m_nLength;
}

void ngStringV2::SetLength(int32 length)
{
	int32 len = length;
	if (m_nFormat == WCS) {
		len = len * 2;
	}
	SetCapacity(len + 4);
	m_nLength = length;
}

void ngStringV2::AssignString(NGCSTR str, int32 length /* = -1 */)
{
	m_nFormat = ASCII;
	
    if (length == 0) return;
    
	if (str == NULL || str[0] == 0) {
		Clear();
		return;
	}
	
	if (length == -1) {
		m_nLength = strlen(str);
	}else{
		m_nLength = length;
	}

	if (CopyBuffer((const uint8 *)str,STRBYTELEN(m_nLength)) != NG_OK) {
		Clear();
	}
}

void ngStringV2::AssignString(NGWCSTR str, int32 length /* = -1 */)
{
	m_nFormat = WCS;
	
	if (str == NULL || str[0] == 0) {
		Clear();
		return;
	}
	
	if (length == -1) {
		m_nLength = wcslen(str);
	}else{
		m_nLength = length;
	}

	if (CopyBuffer((const uint8 *)str,WCSBYTELEN(m_nLength)) != NG_OK) {
		Clear();
	}
}

void ngStringV2::AssignString(const uint8* str, int32 length /* = -1 */)
{
	m_nFormat = UNKNOWN;
	
	if (str == NULL || str[0] == 0) {
		Clear();
		return;
	}
	if (length == -1) {
		Clear();
		return;
	}

	if (CopyBuffer(str,STRBYTELEN(length)) != NG_OK) {
		Clear();
		return;
	}

	m_nLength = length;
}


int32 ngStringV2::AppendString(NGCSTR str, int32 length /* = -1 */)
{
	if (str == NULL || str[0] == 0)  {
		return NG_ERROR_FAILTURE;
	}
	if (-1 == length) {
		length = strlen(str);
	}

	if (ConvertToASCII() == NG_ERROR_FAILTURE) {
		return NG_ERROR_FAILTURE;
	}

	SetLimit(m_nLength);
	AppendBuffer((uint8 *)str, STRBYTELEN(length));
	m_nLength += length;
	SetLimit(m_nLength);

	return NG_OK;
}

int32 ngStringV2::AppendString(NGWCSTR str, int32 length /* = -1 */)
{
	if (str == NULL || str[0] == 0)  {
		return NG_ERROR_FAILTURE;
	}

	if (-1 == length) {
		length = wcslen(str);
	}

	if (ConvertToWcs() == NG_ERROR_FAILTURE) {
		return NG_ERROR_FAILTURE;
	}

	SetLimit(WCSBYTELEN(m_nLength));
	AppendBuffer((uint8 *)str, WCSBYTELEN(length));
	m_nLength += length;
	SetLimit(WCSBYTELEN(m_nLength));

	return NG_OK;
}

ngStringV2& ngStringV2::operator =(const ngStringV2& another)
{
	FORMAT fmt = another.GetFormat();
	SetFormat(fmt);
	if (fmt == ASCII) {
		AssignString(another.GetCString());
	}else if (fmt == WCS) {
		AssignString(another.GetWcs());
	}
	return *this;
}

ngStringV2& ngStringV2::operator +=(const ngStringV2& another)
{
	NGASSERT(m_nFormat == another.GetFormat());
	if (m_nFormat == ASCII) {
		AppendString(another.GetCString());
	}else if (m_nFormat == WCS) {
		AppendString(another.GetWcs());
	}
	return *this;
}

ngStringV2& ngStringV2::operator +=(int64 val)
{
	NGCHAR strTmp[24];
	sprintf(strTmp, "%lld", val);
	AppendString(strTmp);
	return *this;
}

ngStringV2& ngStringV2::operator +=(double val)
{
	NGCHAR strTmp[20];
	sprintf(strTmp, "%f", val);
	AppendString(strTmp);
	return *this;
}

int32 ngStringV2::CopyBuffer(const uint8 *src, int32 length) {

	if (length == 0) {
		return NG_OK;
	}
    
	if (ngByteBuffer::CopyBuffer(src, length) != NG_OK) {
		return NG_ERROR_FAILTURE;
	}
    
    m_nLength = m_nLimit;

	return NG_OK;
}

int32 ngStringV2::AppendBuffer(const uint8*src, int32 length) {

    if (length == 0) {
        return NG_OK;
    }

	if (ngByteBuffer::AppendBuffer((uint8*)src, length) != NG_OK) {
		return NG_ERROR_FAILTURE;
	}
	
//    m_nLength = m_nLimit;
    
	return NG_OK;
}

int32 ngStringV2::CalcCapacity(int32 length) const {
    return ngByteBuffer::CalcCapacity(length + 4);  //4 more bytes for '\0' terminal protection.
}

int32 ngStringV2::ConvertToASCII()
{
	if (m_nFormat == ASCII) {
		return NG_OK;
	}
	if (m_nFormat == UNKNOWN) {
		m_nFormat = ASCII;
		return NG_OK;
	}
	NGWCSTR pWcs = (NGWCSTR)m_pBuffer;
	NGSTR pBuff = NULL;
	uint32 utflen = WcsToUtf8(pWcs, pBuff);
	CopyBuffer((uint8 *)pBuff, STRBYTELEN(utflen));
	SAFE_DELETE(pBuff);
	m_nFormat = ASCII;
	return NG_OK;
}

int32 ngStringV2::ConvertToWcs()
{
	if (m_nFormat == WCS) {
		return NG_OK;
	}
	if (m_nFormat == UNKNOWN) {
		m_nFormat = WCS;
		return NG_OK;
	}	
	NGCSTR pStr = (NGCSTR)m_pBuffer;
	NGWSTR pWcs = NULL;
	uint32 wlen = Utf8ToWcs(pStr, pWcs);
	CopyBuffer((uint8 *)pWcs, WCSBYTELEN(wlen));
	SAFE_DELETE(pWcs);
	m_nFormat = WCS;
	return NG_OK;
}

void ngStringV2::Trim()
{
	TrimLeft();
	TrimRight();
}

void ngStringV2::TrimLeft()
{
	// TODO
	NGASSERT(0);
}

void ngStringV2::TrimRight()
{
	// TODO
	NGASSERT(0);
}

int32 ngStringV2::Replace(NGCSTR fromStr, NGCSTR toStr)
{
	if (m_pBuffer == NULL) {
		return 0;
	}
	
	int32 replen = strlen(toStr);
	int32 patlen = strlen(fromStr);
	int32 orilen = strlen((NGCSTR)m_pBuffer);
	
	int32 patcnt = 0;
	NGCSTR oriptr;
	NGCSTR patloc;
	
	for(oriptr = (NGCSTR)m_pBuffer; patloc = strstr(oriptr, fromStr); oriptr = patloc + patlen) {
		patcnt ++;
	}
	
	int32 retlen = orilen + patcnt * (replen - patlen);
	NGSTR returned = (NGSTR)DMALLOC(sizeof(NGCHAR) * retlen + 1);
	if (returned != NULL) {
		NGSTR retptr = returned;
		for (oriptr = (NGCSTR)m_pBuffer; patloc = strstr(oriptr, fromStr); oriptr = patloc + patlen)
		{
			int32 skplen = patloc - oriptr;
			// copy the section until the occurence of the pattern
			strncpy(retptr, oriptr, skplen);
			retptr += skplen;
			// copy the replacement 
			strncpy(retptr, toStr, replen);
			retptr += replen;
		}
		// copy the rest of the string.
		strcpy(retptr, oriptr);
	}
	AssignString(returned);
    
    SAFE_FREE(returned);
	
	return m_nLength;
}

int32 ngStringV2::Replace(NGSTR buffer, NGCHAR fromChar, NGCHAR toChar)
{
	// TODO
	NGASSERT(0);
	return -1;
}

#define TEMP_BUFFER_SIZE 1024
void ngStringV2::Format(NGCSTR strFormat, ...)
{
	va_list argptr;
    va_start(argptr, strFormat);
    Format(strFormat, argptr);
    va_end(argptr);
}

void ngStringV2::Format(NGCSTR strFormat, va_list argptr)
{
	char strTemp[TEMP_BUFFER_SIZE];
	char *p = strTemp;
	int32 count = TEMP_BUFFER_SIZE;
#ifdef WP8
	memset(strTemp,0,TEMP_BUFFER_SIZE);
#endif
	int32 result = -1;
	while (result < 0) {
        
		result = vsnprintf(p, count - 1, strFormat, argptr);
        
        /* 
         * ???
         * here if result string len > 1024(var result > 1024), it will be truncated to a string with len 1022(count - 1).
         * reference: http://en.cppreference.com/w/c/io/vfprintf
         * so I temp add a assert below.
         * should support long string?
         * (same problem if copy method ngStringV2::Format(NGCSTR strFormat, va_list argptr)).
         *
         * [zhen.chen]
         *
         */
//        NGASSERT(result >= 0 && result < count - 1);

#if 0
		if (result < 0) {
			if (p != strTemp) {
				SAFE_DELETE_ARRAY(p);
			}
			count = count + TEMP_BUFFER_SIZE;
			p = (char*)DMALLOC(count);
		}
#else
        //temp fixed with below code
        if (result < 0) {
            //vsnprintf failed, nothing copied!~
            NGASSERT(0);
            break;//will not retry.
        } else if (result > count - 1) {
            //temp buffer is not enought.
            count = result + 2;
            p = (char*)DMALLOC(count);
            p[count - 1] = '\0';
            
            result = -1;    //temp compatible value.
        } else {
            //copy done!~
            break;
        }
#endif
	}
    
	m_nFormat = ASCII;
	CopyBuffer((const uint8*)p, result);
	m_nLength = result;
    
	if (p != strTemp) {
		SAFE_FREE(p);
	}
}

void ngStringV2::Format(NGWCSTR strFormat, ...)
{
	va_list argptr;
	wchar strTemp[TEMP_BUFFER_SIZE];
	wchar *p = strTemp;
	int32 count = TEMP_BUFFER_SIZE;

	int32 result = -1;
	while (result < 0) {

		va_start(argptr, strFormat);
		result = vswprintf(p, count - 1, strFormat, argptr);
		va_end(argptr);

        //temp fixed with below code
        if (result < 0) {
            //vsnprintf failed, nothing copied!~
            NGASSERT(0);
            break;//will not retry.
        } else if (result > count - 1) {
            //temp buffer is not enought.
            count = result + 2;
            p = (wchar*)DMALLOC(count * sizeof(wchar));
            p[count - 1] = '\0';
            
            result = -1;    //temp compatible value.
        } else {
            //copy done!~
            break;
        }
	}

	m_nFormat = WCS;
	CopyBuffer((const uint8*)p, WCSBYTELEN(result));
	m_nLength = result;

	if (p != strTemp) {
		SAFE_FREE(p);
	}
}

NGCSTR ngStringV2::GetCString(boolean bConvertIfNeeded /* = TRUE */)
{
	if (m_nFormat == WCS && bConvertIfNeeded == TRUE) {
		ConvertToASCII();
	}
	
	if (m_nLength > 0)
	{
		return (NGCSTR)GetBuffer();
	}
	else
	{
		return "";
	}
}

NGCSTR ngStringV2::GetCString() const
{
	return (NGCSTR)GetBuffer();
}

NGWCSTR ngStringV2::GetWcs(boolean bConvertIfNeeded /* = TRUE */)
{
	if (m_nFormat == ASCII && bConvertIfNeeded == TRUE) {
		ConvertToWcs();
	}
	return (NGWCSTR)GetBuffer();
}

NGWCSTR ngStringV2::GetWcs() const
{
	return (NGWCSTR)GetBuffer();
}


int32 ngStringV2::GetInt() const
{
	NGASSERT( m_nFormat == ASCII );
	return atoi((NGCSTR)m_pBuffer);
}

int64 ngStringV2::GetInt64() const {
	NGASSERT( m_nFormat == ASCII );
#ifdef WP8
	return _atoi64((NGCSTR)m_pBuffer);
#else
	return atoll((NGCSTR)m_pBuffer);
#endif // WP8

	
}

double ngStringV2::GetFloat() const
{
	NGASSERT( m_nFormat == ASCII );
	return atof((NGCSTR)m_pBuffer);
}

int32 ngStringV2::SetFormat(FORMAT format)
{
	if (m_nFormat == format) {
		return NG_OK;
	}
	if (format == ASCII && m_nFormat == WCS) {
		return ConvertToASCII();
	}
	if (format == WCS && m_nFormat == ASCII) {
		return ConvertToWcs();
	}
	return NG_ERROR_FAILTURE;
}

ngStringV2* ngStringV2::Mid(int32 start, int32 len)
{
	if (start < 0) {
		start = 0;
	}
	if (m_nFormat == WCS) {
		start = WCSBYTELEN(start);
		len = WCSBYTELEN(len);
	}
	uint8* pBuffer = m_pBuffer + start;
	ngStringV2* pString = DNEW ngStringV2(m_nFormat);
	pString->AppendBuffer(pBuffer, len);
	pString->m_nLength = len;
	return pString;
}

ngStringV2* ngStringV2::Left(int32 len)
{
	return Mid(0, len);
}

ngStringV2* ngStringV2::Right(int32 len)
{
	int32 start = m_nLength - len > 0 ? m_nLength - len : 0;
	return Mid(start, len);
}


ngStringV2* ngStringV2::Base64Encode(ngByteBuffer* buffer)
{
    int32 len = 0;
    char* c = NewBase64Encode((const void *)buffer->GetBuffer(), buffer->GetLimit(), FALSE, (size_t *)&len);
    ngStringV2* pNewString = DNEW ngStringV2(c);
    
    SAFE_FREE(c);
    return pNewString;
}

ngByteBuffer* ngStringV2::Base64Decode()
{
    int32 len = 0;
    void * b = NewBase64Decode(GetCString(), GetLength(), (size_t *)&len);
    ngByteBuffer* pBuffer = DNEW ngByteBuffer();
    pBuffer->CopyBuffer((const uint8*)b, len);

    SAFE_FREE(b);
    return pBuffer;
}

ngStringList* ngStringV2::Split(NGCSTR sep) {

    //TODO: new interface, need test. [chen.zhen] @ 2012-07-03
    
    if (m_nLength <= 0) {
        return NULL;
    }
    
    int32 sepLen = strlen(sep);

    ngStringList* pSplitList = DNEW(ngStringList);
    
    int32 infoPosBegin = 0;
    int32 infoPosEnd   = 0;
    
    ngStringV2* pSubStr = NULL;
    
    while (TRUE) {
        infoPosEnd = Find(sep, infoPosBegin);
        
        if (infoPosEnd != -1) {
            pSubStr = Mid(infoPosBegin, infoPosEnd - infoPosBegin);
            infoPosBegin = infoPosEnd + sepLen;
        } else if (infoPosBegin != m_nLength + sepLen) {
            pSubStr = Mid(infoPosBegin, GetLength() - infoPosBegin);
            infoPosBegin = m_nLength + sepLen;
        } else {
            break;
        }
        
        NGASSERT(pSubStr);

        pSplitList->Add(pSubStr);        
    }
    
    return pSplitList;

}

boolean ngStringV2::HasPrefix(const char* str) {
	return (Find(str) == 0);
}
