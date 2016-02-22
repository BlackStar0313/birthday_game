#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
USING_NS_CC;

#include "CPlayerBall.h"

class HelloWorld : public cocos2d::Layer
{
public:
    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static cocos2d::Scene* createScene();

    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();
    
    // implement the "static create()" method manually
    CREATE_FUNC(HelloWorld);
    
    HelloWorld();
    
    void SetPhyWorld(PhysicsWorld* pWord) { m_world = pWord; }
    
    CPlayerBall* GetPlayerBall();
    
    bool onContactBegin(const cocos2d::PhysicsContact &contact);

private:
//    virtual bool onTouchBegan(Touch *touch, Event *unused_event);
//    virtual void onTouchMoved(Touch *touch, Event *unused_event);
//    virtual void onTouchEnded(Touch *touch, Event *unused_event);
//    virtual void onTouchCancelled(Touch *touch, Event *unused_event){};
    
    
    bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);
    void onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event);
    void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event);
    
    virtual void update(float delta);
    void UpdateCreateFood(float delta);
    
    virtual void onEnter();
    
    void StopSpriteBallAction();
    
    void CreateVideoPlayer();
private:
    
    CPlayerBall* m_playerBall;
    bool m_isClick;
    PhysicsWorld* m_world ;
};


inline CPlayerBall* HelloWorld::GetPlayerBall()
{
    return m_playerBall;
}

#endif // __HELLOWORLD_SCENE_H__
