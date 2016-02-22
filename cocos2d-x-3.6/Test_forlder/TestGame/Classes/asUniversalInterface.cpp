//
//  asUniversalInterface.cpp
//  TestGame
//
//  Created by liuzhiwei on 15/5/19.
//
//

#include "asUniversalInterface.h"
//#include
asUniversalInterface::asUniversalInterface()
{

}

asUniversalInterface::~asUniversalInterface()
{

}

void asUniversalInterface::GenerateRelationShip(cocos2d::Layer *pParentLayer, cocos2d::PhysicsBody *pPlayerBody, cocos2d::PhysicsBody *pEatBody ,std::function<bool(PhysicsContact& )> callback)
{
    if (!pParentLayer || !pPlayerBody || !pEatBody) {
        assert("bind body error~~~~");
        return;
    }
    
    auto contactListener = EventListenerPhysicsContactWithBodies::create(pPlayerBody , pEatBody);
//    contactListener->onContactBegin = CC_CALLBACK_1(callback, pParentLayer);
    contactListener->onContactBegin = callback;
    
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    
    dispatcher->addEventListenerWithSceneGraphPriority(contactListener, pParentLayer);
    
}