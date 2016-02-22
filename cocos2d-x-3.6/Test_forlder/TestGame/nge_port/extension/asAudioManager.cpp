//
//  asAudioManager.cpp
//  money
//
//  Created by Rui Xie on 14-7-24.
//  Copyright (c) 2014年 Anansi Mobile. All rights reserved.
//

#include "asAudioManager.h"

#include "SimpleAudioEngine.h" // to include sound engine
#include "CGameSoundResource.h" // to include resource

using namespace CocosDenshion;

#pragma mark asAudioNode

enum AUDIO_TYPE {
    AUDIO_SOUND,
    AUDIO_MUSIC,
};

class asAudioNode {
    
public:
    asAudioNode(AUDIO_TYPE type, int32 audioId, int64 delayMS, boolean isLoop);
    ~asAudioNode();
    
    boolean isReadyToPlay(int64 deltaMS);
    AUDIO_TYPE getType() {return _audioType;}
    int32 getAudioId() {return _audioId;}
    boolean isLoop() {return _isLoop;}
    
    // for stop effect
    void SetEffectId(int32 effectId);
    int32 GetEffeceId();
    
private:
    int64 _delayMS;
    int32 _audioId;
    AUDIO_TYPE _audioType;
    boolean _isLoop;
    
    // for stop effect
    int32 _effectId;
    
    int64 _timer;
};

asAudioNode::asAudioNode(AUDIO_TYPE type, int32 audioId, int64 delayMS, boolean isLoop)
{
    _audioType = type;
    _audioId = audioId;
    _delayMS = delayMS;
    _isLoop = isLoop;
    _timer = 0;
    
    _effectId = -1;
}

asAudioNode::~asAudioNode()
{
    
}

boolean asAudioNode::isReadyToPlay(int64 deltaMS)
{
    _timer += deltaMS;
    if (_timer>_delayMS) {
        _timer = 0;
        return TRUE;
    } else {
        return FALSE;
    }
}

void asAudioNode::SetEffectId(int32 effectId)
{
    _effectId = effectId;
}

int32 asAudioNode::GetEffeceId()
{
    return _effectId;
}


#pragma mark asAudioManager

asAudioManager* asAudioManager::sThis = NULL;

asAudioManager* asAudioManager::GetInstance()
{
    if (!sThis) {
        asAudioManager::sThis = DNEW (asAudioManager);
    }
    
    return asAudioManager::sThis;
}

void asAudioManager::ReleaseInstance(void *pObj)
{
    
}

void asAudioManager::update(int64 deltaMS)
{
    ngIterator* pIter = _audioNodes.Iterator();
    while (pIter->HasNext()) {
        asAudioNode* pNode = (asAudioNode*)pIter->Next();
        if (pNode->isReadyToPlay(deltaMS)) {
            if (pNode->getType()==AUDIO_SOUND) {
                playSound(pNode->getAudioId(), 0, pNode->isLoop());
            } else {
                playMusic(pNode->getAudioId(), 0, pNode->isLoop());
            }
            
            _audioNodes.Remove(pNode);
            SAFE_DELETE(pNode);
        }
    }
    SAFE_DELETE(pIter);
}

boolean asAudioManager::playSound(int32 sid, int64 delayMS, boolean isLoop, boolean canReplace)
{
    if (sid < 0 || sid >= GAME_SOUND_NUM) {
        NGASSERT(FALSE);
        return false;
    }
    
    boolean ret = true;
    
    if (delayMS<=0) {
        if (canReplace) {
            /* 音效可以被覆盖 */
            ngIterator* pIter = _playingNodes.Iterator();
            while (pIter->HasNext()) {
                asAudioNode* pNode = (asAudioNode*)pIter->Next();
                if (pNode && pNode->getAudioId() == sid) {
                    SimpleAudioEngine::sharedEngine()->stopEffect(pNode->GetEffeceId());
                    
                    _playingNodes.Remove(pNode);
                    SAFE_DELETE(pNode);
                }
            }
            SAFE_DELETE(pIter);
            
            
            int32 effectId = SimpleAudioEngine::sharedEngine()->playEffect(GAME_SOUND[sid], isLoop);
            
            asAudioNode* pNode = DNEW asAudioNode(AUDIO_SOUND, sid, delayMS, isLoop);
            if (pNode) {
                pNode->SetEffectId(effectId);
                _playingNodes.InsertAtEnd(pNode);
            }
            
        } else {
            SimpleAudioEngine::sharedEngine()->playEffect(GAME_SOUND[sid], isLoop);
        }
    } else {
        asAudioNode* pNode = DNEW asAudioNode(AUDIO_SOUND, sid, delayMS, isLoop);
        if (pNode) {
            _audioNodes.InsertAtEnd(pNode);
        }
    }
    
    return ret;
}

void asAudioManager::playMusic(int32 mid, int64 delayMS, boolean isLoop)
{
    if (mid < 0 || mid >= GAME_MUSIC_NUM) {
        NGASSERT(FALSE);
        return;
    }
    
    if (delayMS<=0) {
        SimpleAudioEngine::sharedEngine()->playBackgroundMusic(GAME_MUSIC[mid], isLoop);
    } else {
        asAudioNode* pNode = DNEW asAudioNode(AUDIO_MUSIC, mid, delayMS, isLoop);
        if (pNode) {
            _audioNodes.InsertAtEnd(pNode);
        }
    }
}

void asAudioManager::setSoundVolume(float volume /* = 100 */)
{
    SimpleAudioEngine::sharedEngine()->setEffectsVolume(volume/100);
}

void asAudioManager::setMusicVolume(float volume /* = 100 */)
{
    SimpleAudioEngine::sharedEngine()->setBackgroundMusicVolume(volume/100);
}

void asAudioManager::stopSound(int32 sid)
{
    if (sid < 0 || sid >= GAME_SOUND_NUM) {
        NGASSERT(FALSE);
        return;
    }
    
    // caution!!
    /* 当前无法知道某个音效是否播放结束，目前使用stopAllEffects来停止所有音效 */
    SimpleAudioEngine::sharedEngine()->stopAllEffects();
}

void asAudioManager::stopMusic(int32 mid)
{
    if (mid < 0 || mid >= GAME_MUSIC_NUM) {
        NGASSERT(FALSE);
        return;
    }
    
    SimpleAudioEngine::sharedEngine()->stopBackgroundMusic();
}

void asAudioManager::preLoadAudioResource()
{
    /* pre-load sound resource */
    for (int i=0; i<GAME_MUSIC_NUM; i++) {
        CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadBackgroundMusic(GAME_MUSIC[i]);
    }
    
    for (int i=0; i<GAME_SOUND_NUM; i++) {
        CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect(GAME_SOUND[i]);
    }
}

void asAudioManager::pauseBGM()
{
    SimpleAudioEngine::sharedEngine()->pauseBackgroundMusic();
}

void asAudioManager::resumeBGM()
{
    SimpleAudioEngine::sharedEngine()->resumeBackgroundMusic();
}

asAudioManager::asAudioManager()
: ngSingleton(ReleaseInstance, this)
{
    
}

asAudioManager::~asAudioManager()
{
    LIST_CLEAR(&_audioNodes, asAudioNode);
    LIST_CLEAR(&_playingNodes, asAudioNode);
}

