//
//  CBaseBall.h
//  TestGame
//
//  Created by liuzhiwei on 15/5/15.
//
//

#ifndef __TestGame__CBaseBall__
#define __TestGame__CBaseBall__

#include "cocos2d.h"
USING_NS_CC;


/*
 * @brife: CBaseBall 玩家和敌人的基类，
 */
class CBaseBall : public Sprite
{
public:
    CBaseBall();
    ~CBaseBall();
    
    virtual bool init() override;
    virtual bool initWithFile(const std::string& filename);
    
    virtual void EatFood() = 0;
    
    void AddBoxBody();
    
    float GetSpeed();
    void  SetSpeed(float speed);
private:
    float m_speed;
};



inline float CBaseBall::GetSpeed()
{
    return m_speed;
}

inline void CBaseBall::SetSpeed(float speed)
{
    m_speed = speed;
}

#endif /* defined(__TestGame__CBaseBall__) */
