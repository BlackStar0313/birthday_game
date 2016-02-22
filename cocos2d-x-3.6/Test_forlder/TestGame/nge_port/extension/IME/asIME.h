/*
 *  asIME.h
 *  NextGenEngine (in pnk)
 *
 *  Created by Chen Zhen on 14-6-24.
 *  Copyright (c) 2014年 Anansi Mobile. All rights reserved.
 */

#ifndef __asIME_H__
#define __asIME_H__

#include "../../core/ngSingleton.h"
#include <nsl/ngLinkedList.h>

/*!
 @class asIMEEventListener
 @abstract 输入事件相应接口, 通过asIME::GetInstance()->AddEventListener()开始监听事件，析构或者显示调用asIME::GetInstance()->RemoveEventListener()停止响应。
 @discussion
 */
class asIMEEventListener {
protected:
    asIMEEventListener();
    virtual ~asIMEEventListener();

public:
    virtual void OnShowKeyboard(float keyboardHeight) {};
    virtual void OnHideKeyboard(const char* text) = 0;
    virtual void OnTextChange(const char* new_text) {};
};

class asIMEImpl;

/*!
 @class asIME
 @abstract 输入管理器，提供一个平台相关输入方式
 @discussion
 
        输入方式:
 
            iOS: 在键盘上附加一个输入框和确定按钮
            Android: 待定
            WP8: 待定
 */
class asIME
: protected ngSingleton {
public:
    typedef enum __E_KEYBOARD_TYPE__ {
        keyboardDefault = 0,    /* 默认键盘 */
        keyboardNum,            /* 数字键盘 */
    } KeyboardType;

public:

    /*! 显示键盘 */
    void ShowKeyboard();
    /*! 隐藏键盘 */
    void DismissKeyBoard();

    /*! 设置输入框的文本 */
    void SetText(const char* text);
    /*! 设置占位提示文本 */
    void SetPlaceHolder(const char* text);

    /*! 开启密码输入 */
    void SetSceretMode(boolean secret);

    /*! 设置键盘类型 */
    void SetKeyboardType(KeyboardType keyboardType);

public:
    /*! 添加listener */
    void AddEventListener(asIMEEventListener* l);
    /*! 移除listener */
    void RemoveEventListener(asIMEEventListener* l);
    /*! 清除所有listener */
    void RemoveAllEventListener();

private:
    asIMEImpl* m_pImpl;
    ngLinkedList m_eventListenerList;

    friend class asIMEImpl;

#pragma mark - ngSingleton

public:
    static asIME* GetInstance();
    static void ReleaseInstance(void* pObj);

private:
    asIME();
    virtual ~asIME();

private:
    static asIME* m_pThis;
};

#endif //__asIME_H__
