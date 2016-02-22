//
//  CBaseBall.cpp
//  TestGame
//
//  Created by liuzhiwei on 15/5/15.
//
//

#include "CBaseBall.h"

#define BALL_OFF_SET 3

CBaseBall::CBaseBall()
: m_speed(0)
{
    
}

CBaseBall::~CBaseBall()
{
    
}

bool CBaseBall::init()
{
    if (!Sprite::init()) {
        return false;
    }
    return true;
}

bool CBaseBall::initWithFile(const std::string &filename)
{
    if (!Sprite::initWithFile(filename)) {
        return false;
    }
    return true;
}

void CBaseBall::AddBoxBody()
{
    auto body = PhysicsBody::createCircle(this->getContentSize().width / 2 - BALL_OFF_SET);
    this->setPhysicsBody(body);
    this->getPhysicsBody()->setContactTestBitmask(0xFFFFFFFF);
    this->getPhysicsBody()->setGravityEnable(false);
}