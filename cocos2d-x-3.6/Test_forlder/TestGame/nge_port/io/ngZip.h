//
//  ngZip.h
//  pet
//
//  Created by chen on 12-8-2.
//  Copyright (c) 2012年 Anansi. All rights reserved.
//

#ifndef __NGZIP_H__
#define __NGZIP_H__

#include "../core/NGE_Defs.h"
#include "../nsl/ngStringV2.h"

#include <io/minizip/zip.h>
#include <io/minizip/unzip.h>

class ngLinkedList;

class ngZip {
public:
    ngZip();
    virtual ~ngZip();
    
    boolean OpenZipFile(NGCSTR filename, NGCSTR zipPwd = NULL);
    boolean Unzip(ngLinkedList* files = NULL);
    boolean UnzipTo(NGCSTR path, ngLinkedList* files = NULL);
    
    void CloseZipFile();
    
    /* TODO: zip interface. */
    
private:
    unzFile m_unzFile;

    ngStringV2 m_zipFile;
    ngStringV2 m_targetPath;
    ngStringV2 m_zipPwd;

};

#endif  //__NGZIP_H__
