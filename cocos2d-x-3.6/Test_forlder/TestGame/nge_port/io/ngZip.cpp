//
//  ngZip.cpp
//  pet
//
//  Created by chen on 12-8-2.
//  Copyright (c) 2012年 Anansi. All rights reserved.
//

#include <io/ngZip.h>
#include <core/NGE_Macros.h>

#include <core/ngDevice.h>
#include <io/ngFileWriter.h>
#include <nsl/ngByteBuffer.h>
#include <nsl/ngLinkedList.h>

ngZip::ngZip() {
    
}

ngZip::~ngZip() {
    CloseZipFile();
}

boolean ngZip::OpenZipFile(NGCSTR filename, NGCSTR zipPwd) {

    NG_DEBUG_LOG("open zip file!~\n");
    
    m_zipPwd = zipPwd;
    
    ngDevice::GetInstance()->GetFilePath(m_zipFile, filename);
	if(!m_zipFile.IsEmpty()) {
        m_unzFile = unzOpen(m_zipFile.GetCString());
        NGASSERT(m_unzFile);
        unz_global_info globalInfo = {0};
		if(unzGetGlobalInfo(m_unzFile, &globalInfo) == UNZ_OK) {
            NG_DEBUG_LOG("[zip] %ld entries in %s\n", globalInfo.number_entry, m_zipFile.GetCString());
            return TRUE;
		}
	}    
    return FALSE;
}

boolean ngZip::Unzip(ngLinkedList* files) {
    
    return UnzipTo("./", files);

}

#define UNZIP_BUFFER_LEN    4096

boolean ngZip::UnzipTo(NGCSTR path, ngLinkedList* files) {

	boolean success = TRUE;
    
    ngStringV2 strUnZipPath = path;
	
    int32 ret = unzGoToFirstFile(m_unzFile);

    uint8 buffer[UNZIP_BUFFER_LEN] = {0};
    
    ngFileWriter fw;

	if( ret != UNZ_OK ) {
		NG_DEBUG_LOG("[zip] unzip failed!~\n");
	}
    
	do{
		if(m_zipPwd.IsEmpty()) {
			ret = unzOpenCurrentFile(m_unzFile);
		} else {
			ret = unzOpenCurrentFilePassword(m_unzFile, m_zipPwd.GetCString());
        }

		if(ret != UNZ_OK) {
            NG_DEBUG_LOG("[zip] Error occurs while open file!~\n");
			success = FALSE;
			unzCloseCurrentFile(m_unzFile);
			break;
		}
        
		// reading data and write to file
		int32 read;
		unz_file_info fileInfo ={0};
		ret = unzGetCurrentFileInfo(m_unzFile, &fileInfo, NULL, 0, NULL, 0, NULL, 0);
		if(ret != UNZ_OK)
		{
			NG_DEBUG_LOG("[zip] Error occurs while getting file info!~\n");
			success = FALSE;
			unzCloseCurrentFile(m_unzFile);
			break;
		}

		char* filename = (char*) malloc(fileInfo.size_filename + 1);
		unzGetCurrentFileInfo(m_unzFile, &fileInfo, filename, fileInfo.size_filename + 1, NULL, 0, NULL, 0);
		filename[fileInfo.size_filename] = '\0';

		ngStringV2 strPath = filename;
		boolean isDirectory = FALSE;
		if(filename[fileInfo.size_filename-1] == '/' || filename[fileInfo.size_filename-1] == '\\') {
			isDirectory = TRUE;
		}
        free(filename);

		if(strPath.Find("\\") || strPath.Find("/")) {
            /* contains a path */
			strPath.Replace("\\", "/");
		}
		ngStringV2 fullUnZipPath = strUnZipPath + strPath;
        
		if(isDirectory) {
#if 1 //port to cocos2dx
            NGASSERT(0);
#else
            fw.MkDir(fullUnZipPath);
            unzCloseCurrentFile(m_unzFile);
            ret = unzGoToNextFile(m_unzFile);
#endif
            continue;
        }

        ngByteBuffer byteBuffer;
        
        if (fw.Open(fullUnZipPath)) {
            
            if (files) {
                files->Add(DNEW(ngStringV2)(fullUnZipPath));
            }
            
            while (TRUE) {
                read = unzReadCurrentFile(m_unzFile, buffer, UNZIP_BUFFER_LEN);
                if (read > 0) {
                    byteBuffer.AppendBuffer(buffer, read);
                } else if (read == 0) {
                    break;
                } else {
                    //error: read < 0
                    NG_DEBUG_LOG("[zip] Failed to reading zip file!~\n");
                    NGASSERT(0);    //file must be damaged.
                }
            }
            fw.Write(&byteBuffer);
            fw.Close();
        }

		unzCloseCurrentFile(m_unzFile);
		ret = unzGoToNextFile(m_unzFile);

	} while (ret == UNZ_OK && UNZ_OK != UNZ_END_OF_LIST_OF_FILE);

	return success;
}

void ngZip::CloseZipFile() {
	m_zipPwd.Clear();
    m_zipFile.Clear();
	if(m_unzFile) {
		unzClose(m_unzFile);
        m_unzFile = NULL;
    }
}

