/*
 *  asIME.cpp
 *  NextGenEngine (in pnk)
 *
 *  Created by Chen Zhen on 14-6-24.
 *  Copyright (c) 2014年 Anansi Mobile. All rights reserved.
 */

#include "asIME.h"
#include <core/NGE_Macros.h>
#include <platform/impl/asIMEImpl.h>

asIMEEventListener::asIMEEventListener() {

}

asIMEEventListener::~asIMEEventListener() {
    asIME::GetInstance()->RemoveEventListener(this);
}

void asIME::ShowKeyboard() {

    if (m_pImpl == NULL) {
        return;
    }

    m_pImpl->ShowIME();
}

void asIME::DismissKeyBoard() {

    if (m_pImpl == NULL) {
        return;
    }

    m_pImpl->HideIME();
}

void asIME::SetText(const char* text) {

    if (m_pImpl == NULL) {
        return;
    }

    m_pImpl->SetText(text);
}

void asIME::SetPlaceHolder(const char* text) {

    if (m_pImpl == NULL) {
        return;
    }

    m_pImpl->SetPlaceHolder(text);
}

void asIME::SetSceretMode(boolean secret) {

    if (m_pImpl == NULL) {
        return;
    }

    m_pImpl->SetSceretMode(secret);
}

void asIME::SetKeyboardType(KeyboardType keyboardType) {

    if (m_pImpl == NULL) {
        return;
    }

    m_pImpl->SetKeyboardType(keyboardType);
}

void asIME::AddEventListener(asIMEEventListener* l) {
#if 0
    /* 暂时认为asIME同时只应该存在一个listener。 */
    RemoveAllEventListener();
#else
    if (l == NULL) {
        return;
    }

    if (m_eventListenerList.Contains(l)) {
        return;
    }
#endif
    m_eventListenerList.Add(l);
}

void asIME::RemoveEventListener(asIMEEventListener* l) {
    if (l == NULL) {
        return;
    }

    m_eventListenerList.Remove(l);
}

void asIME::RemoveAllEventListener() {
    m_eventListenerList.Clear();
}

#pragma mark - ngSingleton

asIME* asIME::GetInstance() {
    if (m_pThis == NULL) {
        m_pThis = DNEW(asIME);
    }
    return m_pThis;
}

void asIME::ReleaseInstance(void* pObj) {
    asIME* pThis = (asIME*) pObj;
    NGASSERT(pThis == m_pThis);
    SAFE_DELETE(m_pThis);
}

asIME::asIME()
: ngSingleton(ReleaseInstance, this) {
    /* 初始话平台实现，初始化失败返回NULL。 */
    m_pImpl = asIMEImpl::CreateInstance(this);
}

asIME::~asIME() {
    SAFE_DELETE(m_pImpl);
}

asIME* asIME::m_pThis = NULL;
