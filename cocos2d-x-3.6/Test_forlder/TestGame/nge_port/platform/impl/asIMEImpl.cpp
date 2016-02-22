/*
 *  asIMEImpl.cpp
 *  pnk
 *
 *  Created by Chen Zhen on 14-6-25.
 *  Copyright (c) 2014年 Anansi Mobile. All rights reserved.
 */

#include "asIMEImpl.h"
#include <extension/IME/asIME.h>
#include <core/NGE_Macros.h>

asIMEImpl::asIMEImpl()
: m_pIME(NULL) {

}

asIMEImpl::~asIMEImpl() {

}

void asIMEImpl::OnShowKeyboard(float keyboardHeight)
{
    if (m_pIME == NULL) {
        return;
    }
    
    static boolean running = false;
    if (running) {
        NG_DEBUG_LOG("[asIME] %s running!~", __FUNCTION__);
        return;
    }
    running = TRUE;
    
    NG_DEBUG_LOG("[asIME] show with keyboardHeight: %2f", keyboardHeight);
    
    ngLinkedList linstenerList(m_pIME->m_eventListenerList);
    ngIterator* itr = linstenerList.Iterator();
    while (itr->HasNext()) {
        asIMEEventListener* l = (asIMEEventListener*) itr->Next();
        l->OnShowKeyboard(keyboardHeight);
    }
    SAFE_DELETE(itr);
    
    running = FALSE;
}

void asIMEImpl::OnHideKeyboard(const char* text) {
    if (m_pIME == NULL) {
        return;
    }

    static boolean running = false;
    if (running) {
        NG_DEBUG_LOG("[asIME] %s running!~", __FUNCTION__);
        return;
    }
    running = TRUE;

    NG_DEBUG_LOG("[asIME] done with text: %s", text);

    ngLinkedList linstenerList(m_pIME->m_eventListenerList);
    ngIterator* itr = linstenerList.Iterator();
    while (itr->HasNext()) {
        asIMEEventListener* l = (asIMEEventListener*) itr->Next();
        l->OnHideKeyboard(text);
    }
    SAFE_DELETE(itr);

    running = FALSE;
}

void asIMEImpl::OnTextChanged(const char* text) {
    if (m_pIME == NULL) {
        return;
    }

    if (text == NULL) {
        text = ""; /* protected code. */
    }

    NG_DEBUG_LOG("[asIME] text: %s", text);

    ngLinkedList linstenerList(m_pIME->m_eventListenerList);
    ngIterator* itr = linstenerList.Iterator();
    while (itr->HasNext()) {
        asIMEEventListener* l = (asIMEEventListener*) itr->Next();
        l->OnTextChange(text);
    }
    SAFE_DELETE(itr);
}
