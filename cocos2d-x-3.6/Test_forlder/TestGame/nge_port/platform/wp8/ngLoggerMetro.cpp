/*
 *  ngLoggerMetro.cpp
 *  template
 *
 *  Created by Yuhui Wang on 10/10/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "ngLoggerMetro.h"
#include <Windows.h>

#include <stdio.h>
#include <varargs.h>
#include <string>
#include <metro/UtilsMetro.h>
#include <nsl/ngString.h>

#define BUFFER_SIZE (2 * 1024 * 1024)

void ngLoggerMetro::Log(NGCSTR format, ...) {
	va_list ap;
	va_start(ap, format);
    Log(format, ap);
	va_end(ap);
}

void ngLoggerMetro::Log_NoLine(NGCSTR format, va_list args)
{
	DoLog(0, format, args);
}

void ngLoggerMetro::Log(NGCSTR format, va_list args)
{
	DoLog(1, format, args);
}
void ngLoggerMetro::DoLog(int iAddLine, NGCSTR format, va_list args)
{
// release wp8 app store not allow OutputDebugStringA
// vsnprintf may call GetModuleHandleW (not allowed)
#if defined( NGE_PLATFORM_METRO ) && defined( DEBUG )

	static char *pBuf = NULL;
	
	int iSize = BUFFER_SIZE / 4;

	if(format == NULL || format[0] == 0)
	{
		return ;
	}

	if(pBuf == NULL)
	{
		pBuf = (char *)malloc(BUFFER_SIZE);
	}

	if(pBuf == NULL) { return ; }

	pBuf[0] = 0;
	memset(pBuf, 0, 10);

	memset(pBuf + BUFFER_SIZE - 20, 0, 10);
	pBuf[BUFFER_SIZE - 1] = 0;

	memset(pBuf, 0, BUFFER_SIZE);

	int iBytesWriten = vsnprintf(pBuf, iSize, format, args);
	// trucated string
	if(iBytesWriten == iSize)
	{
		pBuf[iBytesWriten] = 0;
	}

	//vsnprintf_s(szBuf, iSize, iSize, format, args);

	memset(pBuf + BUFFER_SIZE - 20, 0, 10);
	pBuf[BUFFER_SIZE - 1] = 0;

	std::wstring dbgstr = CCUtf8ToUnicode(pBuf);
	const wchar_t *pWChar = dbgstr.c_str();
	if(pWChar == NULL)
	{
		return ;
	}

	int iCopyBlock = 1024;
	int iLen = wcslen(pWChar);
	if(iLen > iCopyBlock)
	{
		int iCopyLen = 0;
		for( int iIndex = 0; iIndex < iLen; )
		{
			iCopyLen = iCopyBlock;
			if(iIndex + iCopyLen > iLen)
			{
				iCopyLen = iLen - iIndex;
			}

			int iWOffset = iIndex;
			memmove(pBuf, &pWChar[iWOffset], iCopyLen * sizeof(wchar));
			((wchar *)pBuf)[iCopyLen] = 0;

			OutputDebugStringW((wchar *)pBuf);

			iIndex = iIndex + iCopyLen;
		}

		if(iAddLine == 1)
		{
			OutputDebugStringW(L"\n");
		}
	}
	else
	{
		OutputDebugStringW(dbgstr.c_str());

		if(iAddLine == 1)
		{
			OutputDebugStringW(L"\n");
		}
	}

    SAFE_FREE(pBuf);

#endif // NGE_PLATFORM_METRO && DEBUG 

}
