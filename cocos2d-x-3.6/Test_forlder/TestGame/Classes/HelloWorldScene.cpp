#include "HelloWorldScene.h"
#include "asMoveBallAction.h"
#include "CFood.h"
#include "CFoodManager.h"
#include "asUniversalInterface.h"
#include "UIVideoPlayer.h"
#include "CocosGUI.h"

USING_NS_CC;

#define MOVE_SPEED 2.0f

Scene* HelloWorld::createScene()
{
    // 'scene' is an autorelease object
//    auto scene = Scene::create();
    auto scene = Scene::createWithPhysics();
    
    //画物理世界多边形边缘
//    scene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);
    
    // 'layer' is an autorelease object
    auto layer = HelloWorld::create();

    layer->SetPhyWorld(scene->getPhysicsWorld());
    
    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

HelloWorld::HelloWorld()
: m_playerBall(NULL)
, m_isClick(false)
{
    
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }
    
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    
    
    //init ball's position
    m_playerBall = CPlayerBall::create("btn-a-0.png");
    m_playerBall->setPosition(Vec2(origin.x + visibleSize.width / 2,
                            origin.y + visibleSize.height / 2 ));
    m_playerBall->setScale(0.5);
    this->addChild(m_playerBall);
    
    //old touch way
//    this->setTouchEnabled(true);
//    this->setTouchMode(Touch::DispatchMode::ONE_BY_ONE);
    
    //set touch
    auto touchListener = EventListenerTouchOneByOne::create();
    touchListener->onTouchBegan = CC_CALLBACK_2(HelloWorld::onTouchBegan, this);
    touchListener->onTouchMoved = CC_CALLBACK_2(HelloWorld::onTouchMoved, this);
    touchListener->onTouchEnded = CC_CALLBACK_2(HelloWorld::onTouchEnded, this);
    
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);
    
    //test code :
    CFood* pFood = CFood::create("btn-b-0.png");
    pFood->setScale(0.2);
    pFood->setPosition(Vec2(origin.x + visibleSize.width / 4,
                            origin.y + visibleSize.height / 4));
    this->addChild(pFood);
//    GenerateRelationshipBody(pFood);
    FoodManager->AddFood(pFood);
    
    asUniversalInterface::GenerateRelationShip(this, m_playerBall->getPhysicsBody(), pFood->getPhysicsBody(), CC_CALLBACK_1(HelloWorld::onContactBegin, this));
    
    this->scheduleUpdate();
    this->schedule(CC_SCHEDULE_SELECTOR(HelloWorld::UpdateCreateFood), 2.0f);
    
    return true;
}

//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< touch
bool HelloWorld::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *unused_event)
{
    if (!m_playerBall) {
        return false;
    }
    Vec2 pos = touch->getLocation();
    
//    m_spriteBall->stopAllActions();
//    
//    auto moveTo = CCMoveTo::create(1.0f , pos);
//    m_spriteBall->runAction(moveTo);
//    m_spriteBall->stopAllActions();
    StopSpriteBallAction();
    
//    int moveDest = m_isClick == true ? -2 : 2 ;
//    int moveDest = 2;
    m_isClick = m_isClick == true ? false : true;
    auto moveTo = asMoveBallAction::create(pos, m_playerBall->GetSpeed());
    
    m_playerBall->runAction(moveTo);
    
    return true;
}

void HelloWorld::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *unused_event)
{
    if (!m_playerBall) {
        return ;
    }
//    m_spriteBall->stopAllActions();
    StopSpriteBallAction();

    Vec2 pos = touch->getLocation();
    
//    int moveDest = 2;
    
    auto moveTo = asMoveBallAction::create(pos, m_playerBall->GetSpeed());
    
    m_playerBall->runAction(moveTo);
}

void HelloWorld::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *unused_event)
{
//    m_spriteBall->stopAllActions();
    StopSpriteBallAction();
}
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>



void HelloWorld::StopSpriteBallAction()
{
    if (m_playerBall) {
        Action* pAction =  m_playerBall->getActionByTag(MOVE_ACTION_TAG);
        if (pAction) {
            m_playerBall->stopAction(pAction);
        }
    }
}

void HelloWorld::update(float delta)
{
//    FoodManager->CheckIsAddFood(m_playerBall);
}

void HelloWorld::UpdateCreateFood(float delta)
{
    Size size = Director::getInstance()->getVisibleSize();
    FoodManager->RandomCreateFood(size , this );
}

void HelloWorld::onEnter()
{
    Layer::onEnter();
    
}

bool HelloWorld::onContactBegin(const cocos2d::PhysicsContact &contact)
{
    auto spriteA = (CFood* )contact.getShapeA()->getBody()->getNode();
    auto spriteB = (CFood* )contact.getShapeB()->getBody()->getNode();
    
    if (spriteA && spriteA->getTag() == TAG_FOOD) {
        FoodManager->EatFood(m_playerBall, spriteA);
    }
    
    if (spriteB && spriteB->getTag() == TAG_FOOD) {
        FoodManager->EatFood(m_playerBall, spriteB);
    }
    
    CreateVideoPlayer();
    
    return TRUE;
}

void HelloWorld::CreateVideoPlayer()
{
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    Size visibleSize = Director::getInstance()->getVisibleSize();
    if (m_playerBall && m_playerBall->getContentSize().width * m_playerBall->getScaleX() > visibleSize.width/2) {
        m_playerBall->removeFromParent();
        m_playerBall = NULL;
        FoodManager->ClearAllFood();
        this->unscheduleAllCallbacks();

        
        //test vedio
        auto videoPlayer = cocos2d::experimental::ui::VideoPlayer::create();
        videoPlayer->setPosition(Vec2(origin.x + visibleSize.width / 2,
                                      origin.y + visibleSize.height / 2 ));
        videoPlayer->setFileName("test.mp4");
        videoPlayer->setContentSize(visibleSize);
        videoPlayer->setKeepAspectRatioEnabled(true);
        videoPlayer->HideLoadingBar();
        this->addChild(videoPlayer);
        videoPlayer->play();
    }
}