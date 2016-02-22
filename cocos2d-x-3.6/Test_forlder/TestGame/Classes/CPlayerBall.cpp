//
//  CPlayerBall.cpp
//  TestGame
//
//  Created by liuzhiwei on 15/5/13.
//
//

#include "CPlayerBall.h"
#include "CFoodManager.h"

#define INIT_SPEED 2                /* 每个玩家的初始速度 */
#define BALL_GROW_SCALE  1.3f       /* 玩家吃掉食物长大的比例 */
#define BALL_GROW_DURATION 0.5f     /* 长大的动画的时间 */

CPlayerBall* CPlayerBall::create()
{
    CPlayerBall* pPlayer = new (std::nothrow) CPlayerBall();
    if (pPlayer && pPlayer->init()) {
        pPlayer->autorelease();
        return pPlayer;
    }
    CC_SAFE_DELETE(pPlayer);
    return nullptr;
}

CPlayerBall* CPlayerBall::create(const std::string &filename)
{
    CPlayerBall* pPlayer = new (std::nothrow) CPlayerBall();
    if (pPlayer && pPlayer->initWithFile(filename)) {
        pPlayer->autorelease();
        return pPlayer;
    }
    CC_SAFE_DELETE(pPlayer);
    return nullptr;
}

CPlayerBall::CPlayerBall()
{
    
}

CPlayerBall::~CPlayerBall()
{
    
}

bool CPlayerBall::init()
{
    if (!CBaseBall::init()) {
        return false;
    }
    
    this->AddBoxBody();
    this->setTag(TAG_PLAYER);
    this->SetSpeed(INIT_SPEED);
    
    return true;
}

bool CPlayerBall::initWithFile(const std::string &filename)
{
    if (!CBaseBall::initWithFile(filename)) {
        return false;
    }
    
    this->AddBoxBody();
    this->setTag(TAG_PLAYER);
    this->SetSpeed(INIT_SPEED);

    return true;
}

void CPlayerBall::EatFood()
{
    this->runAction(CCScaleTo::create(BALL_GROW_DURATION, this->getScale() * BALL_GROW_SCALE));
    
    //TODO: need to add real speed  here .[Blackstar]
    float speed = 0;
    speed = this->getScale() < 1 ? INIT_SPEED * this->getScale() : INIT_SPEED / this->getScale();
    this->SetSpeed(speed);
}