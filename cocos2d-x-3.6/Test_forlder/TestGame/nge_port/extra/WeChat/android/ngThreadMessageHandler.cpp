//
//  ngThreadMessageHandler.cpp
//  NextGenEngine
//
//  Created by chen on 13-2-19.
//  Copyright (c) 2013年 Anansi. All rights reserved.
//

#include "ngThreadMessageHandler.h"
#include <headers/NGE_Core.h>

ngThreadMessage::ngThreadMessage() {
    sem_init(&sem, 0, 0);
}

ngThreadMessage::~ngThreadMessage() {
    sem_destroy(&sem);
}

void ngThreadMessageHandler::QueueMessage(ngThreadMessage* pMsg) {
    pthread_mutex_lock(&mutex);
    m_msgs.Add(pMsg);
    pthread_mutex_unlock(&mutex);
    
//    sem_wait(&(pMsg->sem));
//    SAFE_DELETE(pMsg);
}

void ngThreadMessageHandler::HandleMessage() {
    pthread_mutex_lock(&mutex);
    
    if (m_msgs.IsEmpty()) {
        pthread_mutex_unlock(&mutex);
        return;
    }

    ngLinkedList copyMsgs;
    ngIterator* itr = m_msgs.Iterator();
    while (itr->HasNext()) {
        copyMsgs.Add(itr->Next());
    }
    itr->Destroy();
    m_msgs.Clear();
    
    pthread_mutex_unlock(&mutex);
    
    ngIterator* copyItr = copyMsgs.Iterator();
    while (copyItr->HasNext()) {
        ngThreadMessage* pMsg = (ngThreadMessage*)copyItr->Next();
        pMsg->HandleMessageData();
        SAFE_DELETE(pMsg);
//        sem_post(&(pMsg->sem));
    }
    copyItr->Destroy();
}

#pragma mark - ngSingleton

ngThreadMessageHandler* ngThreadMessageHandler::GetInstance() {
    if (m_pThis == NULL) {
        m_pThis = DNEW(ngThreadMessageHandler);
    }
    return m_pThis;
}

void ngThreadMessageHandler::ReleaseInstance(void* pObj) {
    ngThreadMessageHandler* pThis = (ngThreadMessageHandler*)pObj;
    DELETE_OBJECT(pThis, ngThreadMessageHandler);
}

ngThreadMessageHandler::ngThreadMessageHandler()
: ngSingleton(ReleaseInstance, this) {
    pthread_mutex_init(&mutex, NULL);
}

ngThreadMessageHandler::~ngThreadMessageHandler() {
    pthread_mutex_destroy(&mutex);
    LIST_CLEAR(&m_msgs, ngThreadMessage);
}

ngThreadMessageHandler* ngThreadMessageHandler::m_pThis = NULL;
