//
//  asKamcordSupport.h
//  cowboy1
//
//  Created by Rui Xie on 11/3/14.
//
//

#ifndef __cowboy1__asVRecorderSupport__
#define __cowboy1__asVRecorderSupport__


#include <core/NGE_Defs.h>

class asVRecorderListener {
    
public:
    asVRecorderListener() {};
    virtual ~asVRecorderListener(){}
    
    virtual void OnRecordDone() = 0;
    virtual void OnRecordFailed(int32 errorCode) = 0;
    virtual void OnShareDone() = 0;
    virtual void OnShareFailed(int32 errorCode) = 0;
};


class asVRecorderSupport {
    
public:
    static asVRecorderSupport* GetInstance();
    static void ReleaseInstance();
    
    void RegisterApp(NGCSTR appID, NGCSTR appSecret, NGCSTR appName);
    void SetEventListener(asVRecorderListener* l);
    
    void StartRecord();
    void StopRecord();
    
    // test code
//    void captureFrame();
    
    boolean IsRecording();
    
private:
    asVRecorderSupport();
    ~asVRecorderSupport();
    
    asVRecorderListener* m_pListener;
    
    static asVRecorderSupport* m_pThis;
};


#endif /* defined(__cowboy1__asVRecorderSupport__) */
