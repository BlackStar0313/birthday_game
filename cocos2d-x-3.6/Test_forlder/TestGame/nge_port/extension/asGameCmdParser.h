/*
 *  asGameCmdParser.h
 *  pnk
 *
 *  Created by Chen Zhen on 14-6-4.
 *  Copyright (c) 2014年 Anansi Mobile. All rights reserved.
 */

#ifndef __asGameCmdParser_h__
#define __asGameCmdParser_h__

#include "../core/ngSingleton.h"
#include "../nsl/ngString.h"
#include "../nsl/ngNOHashMap.h"

/*! 命令回调接口，argc为参数个数，argv为参数数组，argv在调用完成后由asGameCmdParser清理，返回0表示处理成功，非0值表示处理命令失败。 */
typedef int (*asGameCmdCallback)(int argc, char* argv[]);

#pragma mark - asGameCmdParser

/*!
 @class asGameCmdParser
 @abstract 命令解析类，单例，接受输入解析命令并分发
 @discussion 所有命令，启动码，参数，都只接受 字母/数字/下划线 组合。
 
    命令格式：
 
        /<pwd> <cmd> [arg1] [arg2] ...
 
        其中:

            pwd: 通过SetCmdPwd()设置，默认为cmd。
            cmd: 通过RegisterCmd()注册的命令，arg1等参数通过callback接口传回。

 */
class asGameCmdParser
: public ngSingleton {

public:

    /*! 设置命令启动码，如果不设置，使用默认启动码"cmd" */
    void SetCmdPwd(NGCSTR pwd);

    /*!
     @function RegisterCmd
     @param cmdStr 命令名
     @param cmdCallback 命令回调
     @result 注册成功返回TRUE。
     @abstract 注册命令
     @discussion 已存在cmdStr命令，会覆盖旧命令
     */
    int32 RegisterCmd(const char* cmdStr, asGameCmdCallback cmdCallback);

    /*!
     @function ParserCmd
     @param cmdStr 命令文本
     @result 返回NG_OK
     @abstract 解析并分发命令
     @discussion
     */
    int32 ParserCmd(NGCSTR cmdStr);

private:

    ngString m_cmdPwd;
    ngNOHashMap m_cmdMap;

#pragma mark - ngSingleton

public:

    static asGameCmdParser* GetInstance();
    static void ReleaseInstance(void* pObj);

private:
    asGameCmdParser();
    /*virtual */~asGameCmdParser();

private:
    static asGameCmdParser* m_pThis;

};

#pragma mark - inline implement


#endif	//__asGameCmdParser_h__
