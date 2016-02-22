/*
 *  asIMEImplCocoa.h
 *  NextGenEngine (in pnk)
 *
 *  Created by Chen Zhen on 14-6-24.
 *  Copyright (c) 2014年 Anansi Mobile. All rights reserved.
 */

#ifndef __asIMEImplCocoa_h__
#define __asIMEImplCocoa_h__

#include "../impl/asIMEImpl.h"
#import <UIKit/UIKit.h> //UITextField

@class asIMEDelegate;

/*! asIMEImpl的iOS实现。 */
class asIMEImplCocoa
: public asIMEImpl {

public:
    /*! 响应键盘开启 */
    void NotifyShowKeyboard(float keyboardHeight);
    /*! 响应键盘关闭 */
    void NotifyHideKeyboard();
    /*! 响应文本变化 */
    void NotifyTextChanged();

private:
    asIMEImplCocoa();
    virtual ~asIMEImplCocoa();

    void InitKeyboard();

protected:
    virtual void ShowIME();
    virtual void HideIME();
    virtual void SetText(const char* text);
    virtual void SetPlaceHolder(const char* text);
    virtual void SetSceretMode(boolean secret);
    virtual void SetKeyboardType(asIME::KeyboardType keyboardType);

private:
    UITextField* m_pPlatformTextField;
    UITextField* m_pToolbarTextField;

    asIMEDelegate* m_pIMEDelegate;
    
    float m_keyboardHeight;

    friend class asIMEImpl;
};

#endif	//__asIMEImplCocoa_h__
