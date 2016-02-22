//
//  ngFileManager.h
//  islands
//
//  Created by Yuhui Wang on 11/15/11.
//  Copyright (c) 2011 Anansi. All rights reserved.
//

#ifndef __NGFILEMANAGER_H__
#define __NGFILEMANAGER_H__

#include <core/NGE_Types.h>
#include <core/NGE_Macros.h>
#include <nsl/ngString.h>
#include <core/ngSingleton.h>

#if defined (NGE_PLATFORM_METRO)
//impl just necessary in metro.
class ngFileManagerImpl;
#endif

class ngFileManager : public ngSingleton
{
public:
    static ngFileManager* GetInstance();
	static void	ReleaseInstance(void* pObj);
    
    virtual ~ngFileManager() {}
	
	int32 MoveFile(NGCSTR from, NGCSTR to);
    //use to move file with path.
	int32 MoveLocalFile(NGCSTR from, NGCSTR to);
    int32 RemoveFile(NGCSTR filename);
    void* GetFileData(NGCSTR filename, long* pSize = NULL) const; //file name must a full path.

    /*!
     @function MkDir
     @param dir 可写入路径下的相对路径
     @result 创建成功或目录已存在时返回 NG_OK, 否则返回 NG_ERROR_IO。
     @abstract 创建目录
     @discussion
     */
    int32 MkDir(NGCSTR dir);
    
private:
    ngFileManager();
    static ngFileManager* m_pThis;

#if defined (NGE_PLATFORM_METRO)
    ngFileManagerImpl* m_pImpl;
#endif
};

#if defined (NGE_PLATFORM_METRO)
//temp here. [zhen.chen]
class ngFileManagerImpl
{
protected:
    ngFileManagerImpl() {}
    virtual ~ngFileManagerImpl() {}

protected:
    static ngFileManagerImpl* GetInstance();

protected:
    virtual int32 MoveFile(NGCSTR from, NGCSTR to) = 0;
	virtual int32 MoveLocalFile(NGCSTR from, NGCSTR to) = 0;
    virtual int32 RemoveFile(NGCSTR filename) = 0;
    virtual void* GetFileData(NGCSTR filename, long* pSize = NULL) const = 0;
 
    friend class ngFileManager;
};
#endif

#endif  //__NGFILEMANAGER_H__
