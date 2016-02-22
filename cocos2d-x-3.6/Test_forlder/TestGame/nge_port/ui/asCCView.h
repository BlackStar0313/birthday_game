/*
 *  asCCView.h
 *  pnk
 *
 *  Created by Rui Xie on 14-4-2.
 *  Copyright (c) 2014 Anansi. All rights reserved.
 */

#ifndef __nge_port__asCCView__
#define __nge_port__asCCView__

#include <cocos2d.h>
#include <cocos-ext.h>

#include "../game/ngStringManager.h"

USING_NS_CC;
using namespace cocos2d::gui;

#define UI_EFFECT_OFFSET 1000000
#define UI_RICH_LEBAL 2000000


#define NS_ASGUI_BEGIN  namespace asGUI {
#define NS_ASGUI_END    }
#define USING_NS_ASGUI  using namespace asGUI

#define CREATE_ASCCVIEW(__TYPE__)                                   \
static asCCView* Create(                                            \
                        cocos2d::CCNode* __owner__,                 \
                        asCCViewTouchListener* __touchlistener__,   \
                        int __ZOrder__                              \
                        ) {                                         \
    __TYPE__* pRet = DNEW(__TYPE__)(__owner__);                     \
    if (pRet && pRet->Init()) {                                     \
        pRet->SetTouchListener(__touchlistener__);                  \
        return pRet;                                                \
    } else {                                                        \
        SAFE_DELETE(pRet);                                          \
        return NULL;                                                \
    }                                                               \
}                                                                   \
                                                                    \
static asCCView* CreateAsCell(                                      \
                        cocos2d::CCNode* __owner__,                 \
                        asCCViewTouchListener* __touchlistener__,   \
                        int __ZOrder__                           \
                        ) {                                         \
    __TYPE__* pRet = DNEW(__TYPE__)(__owner__);                     \
    if (pRet && pRet->InitAsCell(__ZOrder__)) {                     \
        pRet->SetTouchListener(__touchlistener__);                  \
        return pRet;                                                \
    } else {                                                        \
        SAFE_DELETE(pRet);                                          \
        return NULL;                                                \
    }                                                               \
}                                                                   \
                                                                    \
virtual asCCView* Clone() {                                                 \
    __TYPE__* pRet = DNEW(__TYPE__)(NULL);                          \
    if (pRet && pRet->cloneRootNode(this)) {                        \
        return pRet;                                                \
    } else {                                                        \
        SAFE_DELETE(pRet);                                          \
        return NULL;                                                \
    }                                                               \
}


#define ASCCVIEW_CREATE(__type__, __owner__, __touchlistener__, __ZOrder__) (asGUI::__type__*) asGUI::__type__::Create((__owner__), (__touchlistener__), (__ZOrder__))
#define ASCCVIEW_CREATE_AS_CELL(__type__, __owner__, __touchlistener__, __ZOrder__) (asGUI::__type__*) asGUI::__type__::CreateAsCell((__owner__), (__touchlistener__),(__ZOrder__))
//#define ASCCVIEW_CREATE_AS_CELL_WITH_ZORDER(__type__, __owner__, __touchlistener__, __ZOrder__) (asGUI::__type__*) asGUI::__type__::    ((__owner__), (__touchlistener__),(__ZOrder__))

#define INVALID_ASCCVIEW_OBJ_TAG (-1)

NS_ASGUI_BEGIN

class asCCViewTouchListener {
public:
    asCCViewTouchListener();
    virtual ~asCCViewTouchListener();
    
    virtual void HandleTouch(TouchEventType touchType, CCObject* pSender, int32 tag = INVALID_ASCCVIEW_OBJ_TAG) = 0;
};

/*! 用于响应UI控件点击并分发事件。 */
class asCCViewTouchDispatcher;

/*!
 @class asCCView
 @abstract 导出CocoStudio的UI数据的基类，处理点击事件的分发、语言文本转换、控件变量绑定。
 @discussion
 */
class asCCView {
public:
    asCCView(cocos2d::CCNode* owner);
    virtual ~asCCView();

    cocos2d::CCNode* getRootNode();
    
    virtual asCCView* Clone() = 0;

    virtual void bindUIElement(cocos2d::gui::Widget* wid, const char* widName) = 0;
    virtual void afterBindUIElement(cocos2d::gui::Widget* wid);
    
    /* 通过uiTag来获取当前view中是否有对应的引导控件 */
    cocos2d::gui::UIWidget* getWidgetByGuideTag(int32 uiTag);

protected:
    /*! 初始化方法，载入UI资源 -> 调用绑定接口 -> 添加到owner */
    boolean Init();
    /*! 初始化方法，载入UI资源 -> 调用绑定接口 -> 添加到owner，去掉TouchGroup层，m_pRootLayer是根节点控件。 */
    boolean InitAsCell(int32 ZOrder);
    
    boolean cloneRootNode(asCCView* asView);

    virtual NGCSTR GetUIJsonFileName() const = 0;
    virtual void bindCallBack() = 0;
    virtual void bindText() = 0;
    virtual void bindGuide(){};

    void clickCallback(CCObject* pSender, TouchEventType touchType);
    /*! 在bindCallBack中调用，用于绑定点击事件到m_pTouchDispatcher。 */
    void BindCallBack(cocos2d::gui::UIWidget* widget);

    void SetTouchListener(asCCViewTouchListener* l);
    
    void SetUserData(void* data);
    void* GetUserData();
    
    /*! 绑定到当前view中的引导列表中[tianhua.cai][guide] */
    void BindToGuideList(cocos2d::gui::UIWidget* widget);
    
private:
    void* m_userData;
    cocos2d::CCNode* m_pOwnerLayer;
    asCCViewTouchListener* _touchListener;
    asCCViewTouchDispatcher* m_pTouchDispatcher;

    cocos2d::CCNode* m_pRootNode;
    
    int32 m_ZOrder;

    friend class asCCViewTouchDispatcher;
    
    /* 存储引导列表 */
    ngLinkedList m_pGuideList;
};

#pragma mark - inline implement
inline cocos2d::CCNode* asCCView::getRootNode() {
    return m_pRootNode;
}

NS_ASGUI_END

typedef asGUI::asCCView* (*asCCViewCreateFunc)(cocos2d::CCNode*, asGUI::asCCViewTouchListener*, int);

#endif /* defined(__pnk__asCCView__) */
