/*
 *  asCCView.cpp
 *  pnk
 *
 *  Created by Rui Xie on 14-4-2.
 *  Copyright (c) 2014年 Anansi. All rights reserved.
 */

#include "asCCView.h"
#include "asGUIReaderHelper.h"
#include "CPressHandler.h"
#include "asWidgetActionHelper.h"
#include "CPlayer.h"

USING_NS_ASGUI;

#pragma mark - asCCViewTouchListener

asCCViewTouchListener::asCCViewTouchListener() {
    
}

asCCViewTouchListener::~asCCViewTouchListener() {
    
}

NS_ASGUI_BEGIN

#pragma mark - asCCViewTouchDispatcher

/*! 用于响应UI控件点击并分发事件。 */
class asCCViewTouchDispatcher
: cocos2d::CCObject {

private:
    asCCViewTouchDispatcher(asCCView* handler)
    : m_pHandler(handler) {

    }

    void clickCallback(CCObject* sender, TouchEventType ttype) {
        m_pHandler->clickCallback(sender, ttype);
    }

    friend class asCCView;

private:
    asCCView* m_pHandler;
};

NS_ASGUI_END

#pragma mark - asCCView

asCCView::asCCView(cocos2d::CCNode* owner)
: _touchListener(NULL)
, m_pOwnerLayer(owner)
, m_pTouchDispatcher(DNEW(asCCViewTouchDispatcher)(this))
, m_pRootNode(NULL)
, m_userData(NULL)
{
//    NGASSERT(m_pOwnerLayer != NULL);
    
    m_ZOrder = 0;
}

asCCView::~asCCView() {
    SAFE_DELETE(m_pTouchDispatcher);
    m_pGuideList.Clear();
}

void asCCView::SetTouchListener(asCCViewTouchListener *l) {
    _touchListener = l;
}

void asCCView::SetUserData(void *data)
{
    m_userData = data;
}

void* asCCView::GetUserData()
{
    return m_userData;
}

boolean asCCView::Init() {
    cocos2d::gui::TouchGroup* pTouchGroup = cocos2d::gui::TouchGroup::create();
    
#if defined (__asGUIReaderHelper_h__)
	pTouchGroup->addWidget(asGUIReaderHelper::LoadJsonUIFile(GetUIJsonFileName(), this));
#else
	pTouchGroup->addWidget(cocos2d::extension::GUIReader::shareReader()->widgetFromJsonFile(GetUIJsonFileName(), this));
#endif
    m_pRootNode = pTouchGroup;

	bindCallBack();
	bindText();
    bindGuide();

    if (m_pOwnerLayer) {
        m_pOwnerLayer->addChild(pTouchGroup);
    }

    return TRUE;
}

boolean asCCView::InitAsCell(int32 ZOrder) {
#if defined (__asGUIReaderHelper_h__)
    cocos2d::gui::Widget* pRootWidget = (asGUIReaderHelper::LoadJsonUIFile(GetUIJsonFileName(), this));
#else
    cocos2d::gui::Widget* pRootWidget = (cocos2d::extension::GUIReader::shareReader()->widgetFromJsonFile(GetUIJsonFileName(), this));
#endif
    m_pRootNode = pRootWidget;

	bindCallBack();
	bindText();
    bindGuide();

    if (m_pOwnerLayer) {
        m_pOwnerLayer->addChild(pRootWidget , ZOrder);
    }
    
    m_ZOrder = ZOrder;

    return TRUE;
}

boolean asCCView::cloneRootNode(asGUI::asCCView *asView)
{
    if (1) {
        
        m_ZOrder = asView->m_ZOrder;
        
        m_pTouchDispatcher = DNEW(asCCViewTouchDispatcher)(this);
        m_pOwnerLayer = asView->m_pOwnerLayer;
        _touchListener = asView->_touchListener;
        
        // handle cell case
        cocos2d::gui::Widget* pRootWidget = dynamic_cast<cocos2d::gui::Widget*>(asView->getRootNode());
        if (pRootWidget) {
            m_pRootNode = pRootWidget->clone(this);
            
            if (m_pOwnerLayer) {
                m_pOwnerLayer->addChild(m_pRootNode, asView->m_ZOrder);
            }
            
        // handle normal case
        } else {
            cocos2d::gui::TouchGroup* pTouchGroup = cocos2d::gui::TouchGroup::create();
            
            cocos2d::gui::TouchGroup* touchgroup = dynamic_cast<cocos2d::gui::TouchGroup*>(asView->getRootNode());
            
            NGASSERT(touchgroup);
            
            cocos2d::gui::Widget* wid = dynamic_cast<cocos2d::gui::Widget*>(touchgroup->getRootWidget()->getChildren()->objectAtIndex(0));
            
            NGASSERT(wid);
            
            pTouchGroup->addWidget(wid->clone(this));
            
            m_pRootNode = pTouchGroup;
            
            if (m_pOwnerLayer) {
                m_pOwnerLayer->addChild(pTouchGroup);
            }
        }
        
        bindCallBack();
        bindText();
        bindGuide();

        return TRUE;
    }
}

void asCCView::afterBindUIElement(cocos2d::gui::Widget *wid)
{
    // handle widget actions here
    
    //button actioin
    Button* pBtn = dynamic_cast<Button*>(wid);
    if (pBtn && pBtn->getTag()>=UI_EFFECT_OFFSET) {
        // CHECK LATER
        pBtn->enableShadow(true);
        pBtn->setActionFunc((actionFunc)&asWidgetActionHelper::doBtnPressAction, (actionFunc)&asWidgetActionHelper::doBtnReleaseAction);
    }
    
    //lable action
    Label* pLabel = dynamic_cast<Label*>(wid);
    if (pLabel && pLabel->getTag()>=UI_EFFECT_OFFSET) {
        // CHECK LATER
        pLabel->disableShadow(true);
    }
    if (pLabel && pLabel->getTag()>=UI_RICH_LEBAL) {
        pLabel->createRichTextRenderer();
    }
    
    // more actions
}

void asCCView::BindCallBack(cocos2d::gui::UIWidget *widget) {
	widget->addTouchEventListener(m_pTouchDispatcher, toucheventselector(asCCViewTouchDispatcher::clickCallback));
}

void asCCView::clickCallback(CCObject* pSender, TouchEventType touchType) {
    cocos2d::gui::UIWidget* pWidget = (cocos2d::gui::UIWidget*) pSender;
    
    CPressHandler::GetInstance()->HandleTouch(touchType, pWidget, pWidget->getTag());
    if(!CPressHandler::GetInstance()->TriggerPress() && _touchListener){
        
        /* 添加引导点击更新状态判断[tianhua.cai][guide] */
        // <<
        if ( touchType == TOUCH_EVENT_ENDED ) {
            GuideManagerInstance->updateLuaGuideStatus(pWidget->getTag(), pWidget->GetisUseSecondTag());
        }
        // >>
        
        _touchListener->HandleTouch(touchType, pWidget, pWidget->getTag());
    }
}

void asCCView::BindToGuideList(cocos2d::gui::UIWidget *widget)
{
    m_pGuideList.Add(widget);
}

cocos2d::gui::UIWidget* asCCView::getWidgetByGuideTag(int32 uiTag)
{
    for ( int32 i = 0; i < m_pGuideList.Size(); i++) {
        cocos2d::gui::UIWidget* pItem = (cocos2d::gui::UIWidget*)m_pGuideList.Get(i);
        if ( pItem && pItem->getTag() == uiTag) {
            return pItem;
        }
    }
    return NULL;
}