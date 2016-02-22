//
//  asMoveBallAction.cpp
//  TestGame
//
//  Created by liuzhiwei on 15/5/11.
//
//

#include "asMoveBallAction.h"


asMoveBallAction::asMoveBallAction()
: m_moveSpeed(0)
, m_moveDestination(Vec2(0, 0))
, m_isDone(false)
{

}

asMoveBallAction::~asMoveBallAction()
{

}

asMoveBallAction* asMoveBallAction::create(const cocos2d::Vec2 &moveDestination, const float moveSpeed)
{
    asMoveBallAction* pMoveAction = new (std::nothrow) asMoveBallAction();
    pMoveAction->setTag(MOVE_ACTION_TAG);
    if (pMoveAction) {
        if (pMoveAction->initWithPos(moveDestination, moveSpeed)) {
            pMoveAction->autorelease();
        }
        else {
            delete pMoveAction;
            pMoveAction = nullptr;
        }
    }
    
    return pMoveAction;
}

void asMoveBallAction::startWithTarget(cocos2d::Node *target)
{
    Action::startWithTarget(target);
    m_previousPosition = m_startPosition = target->getPosition3D();
}

bool asMoveBallAction::initWithPos(const cocos2d::Vec2 &moveDestination, const float moveSpeed)
{
    m_moveDestination = moveDestination;
    m_moveSpeed = moveSpeed;
    
    return true;
}

void asMoveBallAction::update(float time)
{
    Vec3 currentPos = _target->getPosition3D();
    Vec3 diff = currentPos - m_previousPosition;
    m_previousPosition = m_previousPosition + diff;
    
    
    Sprite* ptarget = (Sprite* ) _target;
    Size targetSize = ptarget->getContentSize();
    Rect targetRect(m_previousPosition.x - targetSize.width / 4 , m_previousPosition.y - targetSize.height / 4, targetSize.width / 2 , targetSize.height / 2);
    
    if (targetRect.containsPoint(m_moveDestination)) {
        return;
    }
    
    
    /* 三角形等比定律: 斜边比斜边等于对边比对边 */
    float bigDest = sqrtf(pow(fabsf(m_startPosition.x - m_moveDestination.x) , 2) + pow(fabsf(m_startPosition.y - m_moveDestination.y) , 2));
    
    float smallDest = bigDest - m_moveSpeed;
    
    float widthStart = fabsf(m_startPosition.x - m_moveDestination.x) ;
    float heightStart = fabsf(m_startPosition.y - m_moveDestination.y) ;
    
    
    float widthMove = widthStart - smallDest / bigDest * widthStart;
    float heightMove = heightStart - smallDest / bigDest * heightStart;
    
    
    if (m_moveDestination.x > m_startPosition.x && m_moveDestination.y > m_startPosition.y) { // move to right-top
        
    }
    else if(m_moveDestination.x > m_startPosition.x && m_moveDestination.y < m_startPosition.y) { // move to right-bottom
        heightMove *= -1;
    }
    else if(m_moveDestination.x < m_startPosition.x && m_moveDestination.y > m_startPosition.y) { // move to left-top
        widthMove *= -1;
    }
    else if(m_moveDestination.x < m_startPosition.x && m_moveDestination.y < m_startPosition.y) { // move to left-bottom
        widthMove *= -1;
        heightMove *= -1;
    }
    
    CCLOG("~~fuck log is  width is : %f , height is : %f , dest is : %f \n ", widthMove ,heightMove , bigDest);
    
    Vec3 newPos =  m_previousPosition + Vec3(widthMove, heightMove , 0);
    _target->setPosition3D(newPos);
    m_previousPosition = newPos;
}

void asMoveBallAction::step(float dt)
{
    this->update(dt);
}

bool asMoveBallAction::isDone() const
{
    return  m_isDone;
}