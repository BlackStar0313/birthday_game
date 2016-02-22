//
//  ngThreadMessageHandler.h
//  NextGenEngine
//
//  Created by chen on 13-2-19.
//  Copyright (c) 2013年 Anansi. All rights reserved.
//

#ifndef __NGTHREADMESSAGEHANDLER_H__
#define __NGTHREADMESSAGEHANDLER_H__

#include <core/ngSingleton.h>
#include <nsl/ngLinkedList.h>

#include "pthread.h"
#include "semaphore.h"

class ngThreadMessage {
public:
    ngThreadMessage();
    virtual ~ngThreadMessage();

protected:
    virtual void HandleMessageData() = 0;
    
private:
    sem_t sem;
    
    friend class ngThreadMessageHandler;
};

class ngThreadMessageHandler : public ngSingleton {
public:
    void QueueMessage(ngThreadMessage* pMsg);

private:
    void HandleMessage();
    
private:
    ngLinkedList m_msgs;
    
    pthread_mutex_t mutex;
    
#pragma mark - ngSingleton
public:
    static ngThreadMessageHandler* GetInstance();
    static void ReleaseInstance(void* pObj);
    
private:
    ngThreadMessageHandler();
    virtual ~ngThreadMessageHandler();

private:
    static ngThreadMessageHandler* m_pThis;
    
    friend class ngApplet;
};

#endif  //__NGTHREADMESSAGEHANDLER_H__
