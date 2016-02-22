//
//  asUniversalInterface.h
//  TestGame
//
//  Created by liuzhiwei on 15/5/19.
//
//

#ifndef __TestGame__asUniversalInterface__
#define __TestGame__asUniversalInterface__

#include "asMoveBallAction.h"
#include "CFood.h"
#include "CFoodManager.h"

USING_NS_CC;

#define FoodManager CFoodManager::GetInstance()

class asUniversalInterface{
public:
    asUniversalInterface();
    ~asUniversalInterface();
    
    static void GenerateRelationShip(Layer* pParentLayer , PhysicsBody* pPlayerBody , PhysicsBody* pEatBody , std::function<bool(PhysicsContact& )> callback);
};


#endif /* defined(__TestGame__asUniversalInterface__) */
