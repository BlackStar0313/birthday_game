/*
 *  asIMEImpl.h
 *  NextGenEngine (in pnk)
 *
 *  Created by Chen Zhen on 14-6-24.
 *  Copyright (c) 2014年 Anansi Mobile. All rights reserved.
 */

#ifndef __asIMEImpl_h__
#define __asIMEImpl_h__

#include <core/NGE_Types.h>

class asIME;
#include <extension/IME/asIME.h>

/*!
 @class asIMEImpl
 @abstract 输入管理器平台实现接口
 @discussion
 */
class asIMEImpl {
private:
    /*! 创建实例，由平台实现，初始化失败返回NULL，必须使用pIME赋值给m_pIME。 */
    static asIMEImpl* CreateInstance(asIME* pIME);

protected:

    asIMEImpl();
    virtual ~asIMEImpl();

public:
    /* 触发回调事件。 */
    void OnShowKeyboard(float keyboardHeight);
    void OnHideKeyboard(const char* text);
    void OnTextChanged(const char* text);

private:

protected:

    /*! 显示输入法，通过asIME调用。 */
    virtual void ShowIME() = 0;
    /*! 隐藏输入法，通过asIME调用。 */
    virtual void HideIME() = 0;
    /*! 设置文本，通过asIME调用。 */
    virtual void SetText(const char* text) = 0;
    /*! 设置占位文本，通过asIME调用。 */
    virtual void SetPlaceHolder(const char* text) = 0;
    /*! 设置密码输入，通过asIME调用。 */
    virtual void SetSceretMode(boolean secret) = 0;
    /*! 设置键盘类型，通过asIME调用。 */
    virtual void SetKeyboardType(asIME::KeyboardType keyboardType) = 0;

    /* 实现接口 */

private:
    asIME* m_pIME;

    friend class asIME;
};

#endif	//__asIMEImpl_h__
