//
//  CPlayerBall.h
//  TestGame
//
//  Created by liuzhiwei on 15/5/13.
//
//

#ifndef __TestGame__CPlayerBall__
#define __TestGame__CPlayerBall__

#include "cocos2d.h"
#include "CBaseBall.h"
USING_NS_CC;

/*
 * @brife:CPlayerBall 玩家类
 */
class CPlayerBall : public CBaseBall
{
public:
    CPlayerBall();
    ~CPlayerBall();
    
    static CPlayerBall* create();
    static CPlayerBall* create(const std::string& filename);
    
    virtual bool init() override;
    virtual bool initWithFile(const std::string& filename);
    
    virtual void EatFood();
};
#endif /* defined(__TestGame__CPlayerBall__) */
