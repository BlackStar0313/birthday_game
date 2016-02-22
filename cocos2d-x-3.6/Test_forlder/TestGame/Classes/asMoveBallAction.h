//
//  asMoveBallAction.h
//  TestGame
//
//  Created by liuzhiwei on 15/5/11.
//
//

#ifndef __TestGame__asMoveBallAction__
#define __TestGame__asMoveBallAction__

#include "cocos2d.h"
USING_NS_CC;

#define MOVE_ACTION_TAG 10000

class asMoveBallAction : public Action
{
public:
    
    asMoveBallAction();
    ~asMoveBallAction();
    
    static asMoveBallAction* create(const Vec2& moveDestination , const float moveSpeed);
    
    virtual void startWithTarget(Node *target) override;
    
    virtual void update(float time) override;
    
    virtual void step(float dt);
    
    virtual bool isDone() const  override;

    void setIsActionDone(bool isDone);

private:
    bool initWithPos(const Vec2& moveDestination , const float moveSpeed);
    
    
    float m_moveSpeed;
    Vec2 m_moveDestination;
    Vec3 m_startPosition;
    Vec3 m_previousPosition;
    bool m_isDone;
};

inline void asMoveBallAction::setIsActionDone(bool isDone)
{
    m_isDone = isDone;
}

#endif /* defined(__TestGame__asMoveBallAction__) */
