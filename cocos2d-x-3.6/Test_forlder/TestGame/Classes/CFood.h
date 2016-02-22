//
//  CFood.h
//  TestGame
//
//  Created by liuzhiwei on 15/5/13.
//
//

#ifndef __TestGame__CFood__
#define __TestGame__CFood__

#include "cocos2d.h"
USING_NS_CC;


class CFood : public Sprite
{
public:
    CFood();
    ~CFood();
    
    static CFood* create();
    static CFood* create(const std::string& filename);
    
    virtual bool init() override;
    virtual bool initWithFile(const std::string& filename);
    
    void Disappear();
    
    Size GetFoodSize();
    
    void AddBoxBody();
    
};
#endif /* defined(__TestGame__CFood__) */
