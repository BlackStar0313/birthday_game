//
//  ngStringLoader.h
//  NextGenEngine
//
//  Created by Chen Zhen on 13-6-20.
//  Copyright (c) 2013年 Anansi. All rights reserved.
//

#ifndef __NGSTRINGLOADER_H__
#define __NGSTRINGLOADER_H__

#include "../core/NGE_Types.h"

class ngArrayList;
class ngStringV2;

/*!
 @class ngStringLoader
 @abstract provider a ngStringV2 list, load all data from local file.
 @discussion
 */
class ngStringLoader {
public:

    ngStringLoader();
    virtual ~ngStringLoader();

public:
    /*!
     @function Load
     @param localeStringFileName local file name.
     @abstract load string list from local file.
     @discussion file default format is simple, just string list with UTF-8 encode. <br/>
     if you have a different format, implement a sub-class from ngStringLoader and override the method,
     and do not forget Call Free() to release old data before load new data.
     @result TRUE if load success, otherwise FALSE.
     */
    virtual boolean Load(const char* localeStringFileName);

    /*!
     @function Free
     @abstract release string list
     @discussion if re call Load(), old string list will be free.
     */
	void Free();

    /* getter */
    //<<
	ngStringV2* GetString(int32 index);
    NGCSTR      GetCString(int32 index);
    //>>

protected:
	ngArrayList*	m_pStrings;
	int32			m_stringsNum;
};

#endif	//__NGSTRINGLOADER_H__
