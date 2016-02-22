/*
 *  asGameCmdParser.cpp
 *  pnk
 *
 *  Created by Chen Zhen on 14-6-4.
 *  Copyright (c) 2014年 Anansi Mobile. All rights reserved.
 */

#include "asGameCmdParser.h"
#include <core/NGE_Macros.h>

/*! 是否空文本，str为NULL或len等于0返回TRUE. */
static boolean _isStrEmpty(NGCSTR str) {
    if (str && strlen(str) > 0) {
        return FALSE;
    }
    return TRUE;
}

/*! 验证文本，字母/数字/下划线 组合。如果str为NULL或空，返回TRUE. */
static boolean _isStrVaild(NGCSTR str) {
    if (str == NULL || strlen(str) <= 0) {
        return TRUE;
    }

    uint32 len = strlen(str);
    for (uint32 idx = 0; idx < len; idx++) {

        if ((str[idx] >> 7 & 0x1) == 0x1) {
            //mulit-byte utf-8.
            return FALSE;
        }

        if ((str[idx] >= 48 && str[idx] <= 57)      /* 数字 */
            || (str[idx] >= 65 && str[idx] <= 90)   /* 大写字母 */
            || (str[idx] >= 97 && str[idx] <= 122)  /* 小写字母 */
            || str[idx] == 95                       /* 下划线 */
            || str[idx] == 45                       /* 连字符 */
            ) {
            continue;
        } else {
            return FALSE;
        }
    }

    return TRUE;
}

/*! 搜索分隔符，没有找到返回-1. */
static int32 _findSeparator(NGCHAR sep, NGCSTR str) {
    if (str == NULL) {
        return -1;
    }

    for (uint32 i = 0; str[i] != 0x0; i++) {
        if (str[i] == sep) {
            return i;
        }
    }

    return -1;
}

/*! 分隔文本，最大支持128段。 */
static int32 _splitStr(NGCHAR sep, NGCSTR str, NGSTR*& output) {
    char* buffer[128] = { NULL };
    int32 count = 0;

    uint32 lastWordStart = 0;
    uint32 i = 0;
    for (; str[i] != 0x0; i++) {
        if (str[i] == sep) {
            if (i == lastWordStart) {
                lastWordStart = i + 1;
            } else {
                char* item = DNEWARR(char, i - lastWordStart + 1);
                memcpy(item, str + lastWordStart, i - lastWordStart);
                item[i - lastWordStart] = 0x0;
                lastWordStart = i + 1;

                buffer[count] = item;
                count++;

                if (count >= 128) {
                    //ignore more str.
                    break;
                }
            }
        }
    }

    if (i > lastWordStart) {
        char* item = DNEWARR(char, i - lastWordStart + 1);
        memcpy(item, str + lastWordStart, i - lastWordStart);
        item[i - lastWordStart] = 0x0;

        buffer[count] = item;
        count++;

    }

    if (count > 0) {
        output = DNEWARR(char*, count);
        memcpy(output, buffer, count * sizeof(char*));
    }

    return count;
}

static void _freeCmd(int argc, char* argv[]) {
    /* 清理参数 */
    //<<
    for (int32 i = 0; i < argc; i++) {
        SAFE_DELETE_ARRAY(argv[i]);
    }
    SAFE_DELETE_ARRAY(argv);
    //>>
}

void asGameCmdParser::SetCmdPwd(NGCSTR pwd) {
    if (!_isStrEmpty(pwd) && _isStrVaild(pwd)) {
        m_cmdPwd = pwd;
        NG_DEBUG_LOG("[CMD] now cmd pwd is: \"%s\"", m_cmdPwd.GetCString());
    }
}

int32 asGameCmdParser::RegisterCmd(const char* cmdStr, asGameCmdCallback cmdCallback) {

    if (_isStrEmpty(cmdStr)) {
        NG_DEBUG_LOG("[CMD] ignore empty cmd str.");
        return NG_ERROR_FAILTURE;
    }

    if (!_isStrVaild(cmdStr)) {
        NG_DEBUG_LOG("[CMD] ignore invalid cmd str: \"%s\".", cmdStr);
        return NG_ERROR_FAILTURE;
    }

    m_cmdMap.Put(cmdStr, (void*) cmdCallback);

    return NG_OK;
}

int32 asGameCmdParser::ParserCmd(NGCSTR cmdStr) {

    if (_isStrEmpty(cmdStr)) {
        //empty str
        return NG_OK;
    }

    if (cmdStr[0] != 47 /* '/' */) {
        //not start with '/'
        return NG_OK;
    }

    cmdStr += 1; //skip '/'

    int32 cmdStrLen = strlen(cmdStr);

    if (cmdStrLen <= m_cmdPwd.GetLength()) {
        //not a vaild cmd str.
        return NG_OK;
    }

    for (uint32 i = 0; i < m_cmdPwd.GetLength(); i++) {
        if (cmdStr[i] != m_cmdPwd.GetCString()[i]) {
            //not a vaild cmd str.
            return NG_OK;
        }
    }

    cmdStr += m_cmdPwd.GetLength();
    cmdStrLen -= m_cmdPwd.GetLength();

    if (cmdStrLen <= 1) {
        //empty cmd.
        return NG_OK;
    }

    //skip a space.
    cmdStr += 1;
    cmdStrLen -= 1;

    /* 解析命令 */
    int32 argc = 0;
    char** argv = NULL;
    argc = _splitStr(32 /* space */, cmdStr, argv);

    if (argc == 0) {
        //not valid cmd.
        return NG_OK;
    }

    asGameCmdCallback cmdCallback = (asGameCmdCallback) m_cmdMap.Get(argv[0]);
    if (cmdCallback == NULL) {
        //no callback found.
        _freeCmd(argc, argv);
        return NG_OK;
    }

    /* 跳过第1个参数(命令名)。 */
    cmdCallback(argc - 1, argc <= 1 ? NULL : (argv + 1));

    _freeCmd(argc, argv);

    return NG_OK;
}

#pragma mark - ngSingleton

asGameCmdParser* asGameCmdParser::GetInstance() {
    if (m_pThis == NULL) {
        m_pThis = DNEW(asGameCmdParser);
    }

    return m_pThis;
}

void asGameCmdParser::ReleaseInstance(void* pObj) {
    asGameCmdParser* pThis = (asGameCmdParser*) pObj;
    DELETE_OBJECT(pThis, asGameCmdParser);
}

asGameCmdParser::asGameCmdParser()
: ngSingleton(ReleaseInstance, this)
, m_cmdPwd("cmd") {

}

asGameCmdParser::~asGameCmdParser() {

}

asGameCmdParser* asGameCmdParser::m_pThis = NULL;
