/*
 *  ngHttpQueue.h
 *  template
 *
 *  Created by Yuhui Wang on 5/06/11.
 *  Copyright 2010 Anansi Mobile. All rights reserved.
 *
 */

#ifndef __NGHTTPQUEUE_H__
#define __NGHTTPQUEUE_H__

#include "../nsl/ngQueue.h"
#include "../core/ngSingleton.h"

class ngHttpClient;
class ngHttpClientListener;

class ngHttpQueue : public ngSingleton
{
public:
	static ngHttpQueue* GetInstance();
	static void ReleaseInstance(void *pObj);
	
	virtual ~ngHttpQueue();
	
public:
	virtual boolean HandleUpdate(int32 deltaMS);
	
	ngHttpClient* Take();
	void Deaf(ngHttpClientListener* pListener);
    void setPause(boolean pause);
	
private:
	void Clear();
	ngHttpQueue();
	
protected:
	ngQueue m_clients;
    
private:
    boolean m_pause;
};


#endif //__NGHTTPQUEUE_H__