//
//  CFoodManager.cpp
//  TestGame
//
//  Created by liuzhiwei on 15/5/14.
//
//

#include "CFoodManager.h"
#include "NGE_Macros.h"
#include "asUniversalInterface.h"
#include "HelloWorldScene.h"

#define CARETE_FOOD_INTERVEL 4.0f

CFoodManager* CFoodManager::m_pThis = NULL;

CFoodManager* CFoodManager::GetInstance()
{
    if (m_pThis == NULL) {
        m_pThis = new (std::nothrow) CFoodManager();
    }
    
    return m_pThis;
}

void CFoodManager::ReleaseInstance(void *pObj)
{
    CFoodManager* pFood = (CFoodManager* ) pObj;
    
    NGASSERT(pFood == pObj);
    
    DELETE_OBJECT(m_pThis, CFoodManager);
    
    m_pThis = NULL;
}

CFoodManager::CFoodManager()
//: m_pFoodList(new list<CFood*>)
{

}

CFoodManager::~CFoodManager()
{
//    LIST_CLEAR(m_pFoodList, CFood);
//    delete m_pFoodList;
//    list<CFood* >::iterator IteFoodList = m_pFoodList->begin();
}


void CFoodManager::AddFood(CFood *pFood)
{
    //TODO:  需要对加入的食物进行处理
//    m_pFoodList->Add(pFood);
    m_foodList.push_back(pFood);
}

void CFoodManager::ClearAllFood()
{
    list<CFood*>::iterator itor;
    for (itor = m_foodList.begin(); itor != m_foodList.end() ; ) {
        CFood* pFood = (CFood*) *itor;
        itor = m_foodList.erase(itor);
//        m_foodList.remove(*itor);
        pFood->Disappear();
    }
//    m_foodList.clear();
}

/* 通过rect的包含来进行碰撞检测 */
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
void CFoodManager::CheckIsAddFood(CPlayerBall *pPlayer)
{
    //TODO: 需要对纳入的判断食物点进行优化 (没用stl)
//    for (int32 i = 0 ; i < m_pFoodList->Size(); i++) {
//        CFood* pFood = (CFood*) m_pFoodList->Get(i);
//        if (CheckContain(pPlayer, pFood)) {
//            pPlayer->EatFood();
//            
//            m_pFoodList->Remove(pFood);
//            pFood->Disappear();
//        }
//    }
}

bool CFoodManager::CheckContain(CPlayerBall *pPlayer, CFood *pFood)
{
    Rect FoodRect(pFood->getPositionX() , pFood->getPositionY() , pFood->GetFoodSize().width / 2, pFood->GetFoodSize().height / 2);
    
    if (pPlayer->getBoundingBox().intersectsRect(FoodRect)) {
        return true;
    }
    
    return false;
}
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

/* 通过cocos2d 中 physics 做碰撞检测 */
void CFoodManager::EatFood(CPlayerBall *pPlayer ,CFood *pFood)
{
    pPlayer->EatFood();
    m_foodList.remove(pFood);
    pFood->Disappear();
}

void CFoodManager::RandomCreateFood(cocos2d::Size size , Layer* pParentLayer )
{
    auto pLayer = dynamic_cast<HelloWorld* >(pParentLayer);
    
    float posX = 0;
    float posY = 0;
    
    do{
        posX = CCRANDOM_0_1() * size.width;
        posY = CCRANDOM_0_1() * size.height;
        
        CFood* pFood = CFood::create("btn-b-0.png");
        
        if (IsCanCreateFood(posX, posY, pFood, size)) {
            pFood->setScale(0.2);
            pFood->setPosition(Vec2(posX,
                                    posY));
            pParentLayer->addChild(pFood);
            FoodManager->AddFood(pFood);
            
            asUniversalInterface::GenerateRelationShip(pParentLayer, pLayer->GetPlayerBall()->getPhysicsBody(), pFood->getPhysicsBody(), CC_CALLBACK_1(HelloWorld::onContactBegin, pLayer));
            break;
        }
        
    }while (1);
    

}

bool CFoodManager::IsCanCreateFood(float posX, float posY, CFood *pFood , Size size)
{
    if (posX - pFood->GetFoodSize().width / 2 < 0 ||
        posX + pFood->GetFoodSize().width / 2 > size.width ||
        posY - pFood->GetFoodSize().height / 2 < 0 ||
        posY + pFood->GetFoodSize().height / 2 > size.height) {
        return false;
    }
    
    return true;
}