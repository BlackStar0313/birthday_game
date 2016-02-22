//
//  CFoodManager.h
//  TestGame
//
//  Created by liuzhiwei on 15/5/14.
//
//

#ifndef __TestGame__CFoodManager__
#define __TestGame__CFoodManager__

#include "cocos2d.h"
#include "ngComparator.h"
//#include "ngLinkedList.h"
#include "CFood.h"
#include "CPlayerBall.h"

USING_NS_CC;
using namespace std;
enum OBJECT_TAG{
    TAG_FOOD = 1,
    TAG_ENEMY ,
    TAG_PLAYER,
};

class CFoodManager {
public:
    CFoodManager();
    ~CFoodManager();
    
    static CFoodManager* GetInstance();
    static void ReleaseInstance(void* pObj);
    
    void AddFood(CFood* pFood);
    void CheckIsAddFood(CPlayerBall* pPlayer);
    void EatFood(CPlayerBall *pPlayer ,CFood* pFood);
    void RandomCreateFood(Size size , Layer* pParentLayer);
    void ClearAllFood();
private:
    bool CheckContain(CPlayerBall* pPlayer , CFood* pFood);
    bool IsCanCreateFood(float posX , float posY , CFood* pFood , Size size);
    
private:
    static CFoodManager* m_pThis;
    
    list<CFood*> m_foodList;
};

#endif /* defined(__TestGame__CFoodManager__) */
