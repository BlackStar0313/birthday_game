//
//  asAudioManager.h
//  money
//
//  Created by Rui Xie on 14-7-24.
//  Copyright (c) 2014年 Anansi Mobile. All rights reserved.
//

#ifndef __money__asAudioManager__
#define __money__asAudioManager__

#include <core/ngSingleton.h>
#include <core/NGE_NSL.h>
#include <nsl/ngLinkedList.h>
//#include "jsonMacro.h"

#include "asGamePreferences.h"


class asAudioManager;

class asAudioManager : public ngSingleton {
    
public:
    static asAudioManager* GetInstance();
    static void ReleaseInstance(void *pObj);
    
    void update(int64 deltaMS);
    
    boolean playSound(int32 sid, int64 delayMS=0, boolean isLoop=FALSE, boolean canReplace=FALSE);
    void playMusic(int32 mid, int64 delayMS=0, boolean isLoop=TRUE);
    
    void setSoundVolume(float volume = 100);
    void setMusicVolume(float volume = 100);
    
    // caution!!
    /* 当前无法知道某个音效是否播放结束，目前使用stopAllEffects来停止所有音效 */
    void stopSound(int32 sid);
    void stopMusic(int32 mid);
    
    void preLoadAudioResource();
    void pauseBGM();
    void resumeBGM();
    
private:
    /* 链表中记录当前需要播放的音乐效果 */
    ngLinkedList _audioNodes;
    
    ngLinkedList _playingNodes;
    
private:
    asAudioManager();
    virtual ~asAudioManager();
    
    static asAudioManager* sThis;
};

#endif /* defined(__money__asAudioManager__) */
