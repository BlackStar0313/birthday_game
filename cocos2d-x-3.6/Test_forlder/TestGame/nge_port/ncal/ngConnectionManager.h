/*
*  ngConnectionManager.h
*  NextGenEngine
*
*  Created by cooler on 10-5-19.
*  Copyright 2010 DreaminGame. All rights reserved.
*
*/

#ifndef __NGCONNECTIONMANAGER_H__
#define __NGCONNECTIONMANAGER_H__

#include <core/NGE_NSL.h>
#include "../core/ngSingleton.h"

class ngConnectionSession;

/*!
 @class ngConnectionManager
 @abstract 维护所有ngConnectionSession的更新和释放
 @discussion
 */
class ngConnectionManager : public ngSingleton {
public:

    /*! 更新接口，遍历检查所有session的状态，释放已经不再使用的session。 */
	virtual void HandleUpdate(int32 deltaMS);

protected:
    /*! 初始化接口，在构造完成之后调用，什么都没做。 */
	virtual void Initialize();

private:
    /*!
     @function OnNewConnectionSession
     @param pSession session对象
     @abstract 将新的ngConnectionSession对象存入队列
     @discussion 只在ngConnectionSession的构造方法中调用
     */
    void OnNewConnectionSession(ngConnectionSession* pSession);

private:
    ngLinkedList m_sessions;

    friend class ngConnectionSession;

#pragma mark - ngSingleton
public:
	static ngConnectionManager* GetInstance();
	static void ReleaseInstance(void *pObj);

protected:
	ngConnectionManager();
	virtual ~ngConnectionManager();

protected:
	static ngConnectionManager* m_pThis;
};

#endif //__NGHTTPMANAGER_H__