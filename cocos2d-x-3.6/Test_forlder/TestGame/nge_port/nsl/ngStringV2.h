/*
*  ngSringV2.h
*  NextGenEngine
*
*  Created by cooler on 10-5-25.
*  Copyright 2010 DreaminGame. All rights reserved.
*
*/

#ifndef __NGSTRINGV2_H__
#define __NGSTRINGV2_H__

#include "../nsl/ngByteBuffer.h"
#include "../nsl/ngHashComparator.h"
#include "../utils/base64.h"
#include "../core/NGE_Macros.h"

class ngStringV2Comparator : public ngHashComparator
{
	virtual int32		Compare(void* a, void* b);
	virtual void		Destroy() {}
	virtual uint32		HashCode(void* pObj);
};

class ngArrayList;
#if 0
class ngStringList : public ngArrayList {
public:
    ngStringList();
    virtual ~ngStringList();
};
#else
typedef ngArrayList ngStringList;
#endif

class ngStringV2 : public ngByteBuffer
{
public:
	enum FORMAT {
		UNKNOWN = 0,
		WCS,
		ASCII
	};
public:
	// ctor dtor
	ngStringV2();
	ngStringV2(FORMAT fmt):m_nFormat(fmt),m_nLength(0) {}
	ngStringV2(const ngStringV2& another);
	ngStringV2(NGCSTR str);
	ngStringV2(NGCSTR str, int32 length);
	ngStringV2(NGWCSTR wstr);
	ngStringV2(NGWCSTR wstr, int32 length);
	ngStringV2(int64 val);
	ngStringV2(double val);

	virtual ~ngStringV2();
public:
	// functions
	NGCHAR CharAt(int32 index);
	NGWCHAR WcharAt(int32 index);
    int32 SetCharAt(NGCHAR ch, int32 index);
	int32 SetWcharAt(NGWCHAR ch, int32 index);
	int32 GetCharacterAt(int32 index);

    void ToUpper();
    void ToLower();

    static void ToUpper(NGSTR str);
    static void ToLower(NGSTR str);

    int32 Compare(ngStringV2& targetStr);
    int32 Compare(NGCSTR targetStr);
	int32 Compare(NGWCSTR targetStr);

    /* ignore case */
    int32 ICompare(ngStringV2& targetStr);
    int32 ICompare(NGCSTR targetStr) const;

    ngStringV2& Append(NGCHAR ch);
	ngStringV2& Append(NGWCHAR ch);
    ngStringV2& Append(NGCSTR str) { AppendString(str); return *this; }
    ngStringV2& Append(NGCSTR str, int32 length) { AppendString(str, length); return *this; }
	ngStringV2& Append(NGWCSTR wstr) { AppendString(wstr); return *this; }
	ngStringV2& Append(NGWCSTR wstr, int32 length) { AppendString(wstr, length); return *this; }

    /**
     * returns the index of the first occurrence of targetStr in the string, 
     * or -1 if targetStr is not found in the string.
     */
    int32 Find(ngStringV2& targetStr, int32 offset = 0);
    int32 Find(NGCSTR targetStr, int32 offset = 0);
	int32 Find(NGWCSTR targetStr, int32 offset = 0);
    static int32 Find(NGCSTR sourceStr, NGCSTR targetStr, int32 offset = 0);
	static int32 Find(NGWCSTR sourceStr, NGWCSTR targetStr, int32 offset = 0);

    void SetBuffer(NGSTR buffer, int32 length);
	void SetBuffer(NGWSTR buffer, int32 length);

    uint32 GetLength();
    void SetLength(int32 length);
	boolean IsEmpty() const { return (m_nLength == 0); }

    void Trim();
    void TrimRight();
    void TrimLeft();
	
	
	
	void Clear();

    int32 Replace(NGCHAR fromChar, NGCHAR toChar) { return Replace((NGSTR)m_pBuffer, fromChar, toChar); }
    /* replace every fromStr in source string to toStr, 
    note: string size may changed. getCString() return may be different with previous */
    int32 Replace(NGCSTR fromStr, NGCSTR toStr);
	static int32 Replace(NGSTR buffer, NGCHAR fromChar, NGCHAR toChar);

    void Format(NGCSTR strFormat, ...) NGE_STR_FORMAT(2, 3);
    void Format(NGCSTR strFormat, va_list argptr)  NGE_STR_FORMAT(2, 0);
	void Format(NGWCSTR strFormat, ...);
 
    void AssignString(NGCSTR str, int32 length = -1);
	void AssignString(NGWCSTR wstr, int32 length = -1);
    int32 AppendString(NGCSTR str, int32 length = -1);
	int32 AppendString(NGWCSTR wstr, int32 length = -1);

	operator NGWSTR() { return (NGWSTR)GetWcs(TRUE); }
	operator NGWCSTR() { return (NGWCSTR)GetWcs(TRUE); }
	operator NGSTR() { return (NGSTR)GetCString(TRUE); }
	operator NGCSTR() { return (NGCSTR)GetCString(TRUE); }
	int32 GetInt() const;
    int64 GetInt64() const;
	double GetFloat() const;
	
	ngStringV2* Mid(int32 start, int32 len);
	ngStringV2* Left(int32 len);
	ngStringV2* Right(int32 len);
    
    static ngStringV2* Base64Encode(ngByteBuffer* buffer);
    ngByteBuffer* Base64Decode();

	boolean operator==(ngStringV2& otherStr) { return (boolean)(Compare(otherStr) == 0); }
	boolean operator==(NGCSTR str) { return (boolean)(Compare(str) == 0); }
	boolean operator==(NGWCSTR wstr) { return (boolean)(Compare(wstr) == 0); }
	boolean operator!=(ngStringV2& otherStr) { return (boolean)(Compare(otherStr) != 0); }
	boolean operator!=(NGCSTR str) { return (boolean)(Compare(str) != 0); }
	boolean operator!=(NGWCSTR wstr) { return (boolean)(Compare(wstr) != 0); }

	ngStringV2& operator =(NGSTR str) { AssignString(str); return *this; }
	ngStringV2& operator =(NGCSTR str) { AssignString(str); return *this; }
	ngStringV2& operator =(NGWSTR str) { AssignString(str); return *this; }
	ngStringV2& operator =(NGWCSTR str) { AssignString(str); return *this; }
	ngStringV2& operator =(const ngStringV2& otherStr);

	ngStringV2& operator +=(NGSTR str) { AppendString(str); return *this; }
	ngStringV2& operator +=(NGCSTR str) { AppendString((NGSTR)str); return *this; }
	ngStringV2& operator +=(NGWSTR str) { AppendString(str); return *this; }
	ngStringV2& operator +=(NGWCSTR str) { AppendString((NGWSTR)str); return *this; }
	ngStringV2& operator +=(const ngStringV2 &otherStr);
	ngStringV2& operator +=(int64 val);
	ngStringV2& operator +=(double val);
	
	ngStringV2 operator+(NGCSTR str) {ngStringV2 result = *this; result += str; return result; }
	ngStringV2 operator+(NGWCSTR str) {ngStringV2 result = *this; result += str; return result; }
	ngStringV2 operator+(ngStringV2& otherStr) { ngStringV2 result = *this; result += otherStr; return result; }

	ngStringV2& operator <<(NGCSTR str) { AppendString(str); return *this; }
	ngStringV2& operator <<(NGWCSTR str) { AppendString(str); return *this; }

	NGCSTR GetCString(boolean bConvertIfNeeded = TRUE);
	NGCSTR GetCString() const;
	NGWCSTR GetWcs(boolean bConvertIfNeeded = TRUE);
	NGWCSTR GetWcs() const;

    ngStringList* Split(NGCSTR sep);

    /*! 检查前缀 */
    boolean HasPrefix(NGCSTR prefix);

protected:
	
	/* ByteBuffer methods support */
	virtual int32 CopyBuffer(const uint8 *src, int32 length);
	virtual int32 AppendBuffer(const uint8*src, int32 length);

    virtual int32 CalcCapacity(int32 length) const;

public:
	// charset format
	FORMAT GetFormat() const { return m_nFormat; }
	int32 SetFormat(FORMAT format);

protected:
	void AssignString(const uint8* str, int32 length = -1);
	int32 ConvertToWcs();
	int32 ConvertToASCII();

protected:
	int32 m_nLength;
	FORMAT m_nFormat;
};


#endif //__NGSTRINGV2_H__
