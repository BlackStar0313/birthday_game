//
//  CFood.cpp
//  TestGame
//
//  Created by liuzhiwei on 15/5/13.
//
//

#include "CFood.h"
#include "CFoodManager.h"
CFood* CFood::create()
{
    CFood* pFood = new (std::nothrow) CFood();
    if (pFood && pFood->init()) {
        pFood->autorelease();
        return pFood;
    }
    CC_SAFE_DELETE(pFood);
    return nullptr;
}

CFood* CFood::create(const std::string &filename)
{
    CFood* pFood = new (std::nothrow) CFood();
    if (pFood && pFood->initWithFile(filename)) {
        pFood->autorelease();
        return pFood;
    }
    CC_SAFE_DELETE(pFood);
    return nullptr;
}

CFood::CFood()
{
    
}

CFood::~CFood()
{
    
}

bool CFood::init()
{
    if (!Sprite::init()) {
        return false;
    }
    
    this->AddBoxBody();
    this->setTag(TAG_FOOD);
    
    return true;
}

bool CFood::initWithFile(const std::string &filename)
{
    if (!Sprite::initWithFile(filename)) {
        return false;
    }
    
    this->AddBoxBody();
    this->setTag(TAG_FOOD);
    return true;
}

void CFood::Disappear()
{
    this->removeFromParent();
}

Size CFood::GetFoodSize()
{
    return this->getContentSize() * this->getScale();
}

void CFood::AddBoxBody()
{
    auto body = PhysicsBody::createCircle(this->getContentSize().width / 2 * this->getScale());
    this->setPhysicsBody(body);
    this->getPhysicsBody()->setDynamic(false);
    this->getPhysicsBody()->setContactTestBitmask(0xFFFFFFFF);
}