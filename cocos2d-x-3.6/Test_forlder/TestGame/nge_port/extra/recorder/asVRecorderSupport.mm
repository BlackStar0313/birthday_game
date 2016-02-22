//
//  asKamcordSupport.cpp
//  cowboy1
//
//  Created by Rui Xie on 11/3/14.
//
//

#include "asVRecorderSupport.h"
#include "NGE_Macros.h"
#include "cocos2d.h"

USING_NS_CC;

#define ENABLE_VRECORD 1
#define USE_ASSCREEN_RECORDER 0

#if ENABLE_VRECORD
#if USE_ASSCREEN_RECORDER
#include "ASScreenRecorder.h"
#else
#include "RSScreenVideo.h"
#endif
#endif

asVRecorderSupport* asVRecorderSupport::m_pThis = NULL;

asVRecorderSupport::asVRecorderSupport()
{
    m_pListener = NULL;
}

asVRecorderSupport::~asVRecorderSupport()
{
    
}

asVRecorderSupport* asVRecorderSupport::GetInstance()
{
    if (!asVRecorderSupport::m_pThis) {
        asVRecorderSupport::m_pThis = DNEW asVRecorderSupport();
    }
    
    return asVRecorderSupport::m_pThis;
}

void asVRecorderSupport::ReleaseInstance()
{
    SAFE_DELETE(asVRecorderSupport::m_pThis);
}

void asVRecorderSupport::RegisterApp(NGCSTR appID, NGCSTR appSecret, NGCSTR appName)
{
#if ENABLE_VRECORD

    
#endif
}

void asVRecorderSupport::SetEventListener(asVRecorderListener *l)
{
    m_pListener = l;
}

void asVRecorderSupport::StartRecord()
{
#if ENABLE_VRECORD
    
#if USE_ASSCREEN_RECORDER
    if ([[ASScreenRecorder sharedInstance] isRecording]) {
        [[ASScreenRecorder sharedInstance] stopRecordingWithCompletion:^{
            NG_DEBUG_LOG("stop recording before recording\n");
        }];
    }
    
    [[ASScreenRecorder sharedInstance] startRecording];
    NG_DEBUG_LOG("start recording\n");
#else
    
    [[RSScreenVideo sharedInstance] startRecording];
#endif

#endif
}

void asVRecorderSupport::StopRecord()
{
#if ENABLE_VRECORD
#if USE_ASSCREEN_RECORDER
    if ([[ASScreenRecorder sharedInstance] isRecording]) {
        [[ASScreenRecorder sharedInstance] stopRecordingWithCompletion:^{
            NG_DEBUG_LOG("stop recording!!\n");
        }];
    }
#else
    NG_DEBUG_LOG("stop recording!!\n");

    
    [[RSScreenVideo sharedInstance] stopRecordingWithCompletion:^{
        NSLog(@"stop recording !!!!");
    }];
    
#endif
#endif
}

boolean asVRecorderSupport::IsRecording()
{
#if ENABLE_VRECORD
#if USE_ASSCREEN_RECORDER
    return [[ASScreenRecorder sharedInstance] isRecording];
#else
    return [[RSScreenVideo sharedInstance] isRecording];
#endif
#else
    return FALSE;
#endif
}