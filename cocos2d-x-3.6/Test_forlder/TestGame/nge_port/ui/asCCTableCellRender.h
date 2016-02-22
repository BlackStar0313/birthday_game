//
//  asCCTableCellRender.h
//  pnk
//
//  Created by Rui Xie on 14-4-14.
//  Copyright (c) 2014 Anansi. All rights reserved.
//

#ifndef __nge_port_asCCTableCellRender_h__
#define __nge_port_asCCTableCellRender_h__

#include <cocos2d.h>
#include <cocos-ext.h>

USING_NS_CC;
USING_NS_CC_EXT;

#include <nsl/ngList.h>
#include <ui/asCCView.h>

NS_ASGUI_BEGIN

#pragma mark - asCCTableDataProvider
class asCCTableDataProvider {
public:
    asCCTableDataProvider() {}
    virtual ~asCCTableDataProvider() {}

public:
    virtual const void* GetDataAtIndex(int32 idx) const = 0;
    virtual int32 GetDataSize() const = 0;
};

class asCCTableDataListProvider
: public asCCTableDataProvider {
public:
    asCCTableDataListProvider(const ngList* list)
    : asCCTableDataProvider()
    , m_pList(list) {

    }
    virtual ~asCCTableDataListProvider() {}
public:
    virtual const void* GetDataAtIndex(int32 idx) const {
        if (m_pList == NULL) {
            return NULL;
        }

        if (idx < 0 && idx >= m_pList->Size()) {
            return NULL;
        }

        return ((ngList*)m_pList)->Get(idx);
    }

    virtual int32 GetDataSize() const {
        return m_pList->Size();
    }

private:
    const ngList* m_pList;
};

#pragma mark - asCCTableCell
class asCCTableViewCell
: public CCTableViewCell {

    virtual ~asCCTableViewCell() {
        SAFE_DELETE(m_pUICCView);
    }

private:
    asCCView* m_pUICCView;

    friend class asCCTableCellRender;
};

#pragma mark - asCCTableCellRender

/*! 提供table cell的数据和渲染结构。 */
class asCCTableCellRender
: public cocos2d::extension::CCTableViewDataSource
, public cocos2d::extension::CCTableViewDelegate
{
public:
#if 0
    asCCTableCellRender();
#endif

    explicit asCCTableCellRender(asCCViewCreateFunc func);
    virtual ~asCCTableCellRender();

public:
    /*! 设置数据源，provider必须是在堆上创建(new)，生命周期交由当前asCCTableCellRender管理。 */
    void SetData(asCCTableDataProvider* provider);
    /*! 设置点击事件listener */
    void SetTouchListener(asCCViewTouchListener* listener);

protected:
#pragma mark - cocos2d::extension::CCScrollViewDelegate
    virtual void scrollViewDidScroll(CCScrollView* view) {}
    virtual void scrollViewDidZoom(CCScrollView* view) {}

#pragma mark - cocos2d::extension::CCTableViewDelegate
    virtual void tableCellTouched(CCTableView* table, CCTableViewCell* cell);
    virtual void tableCellHighlight(CCTableView* table, CCTableViewCell* cell);
    virtual void tableCellUnhighlight(CCTableView* table, CCTableViewCell* cell);
    virtual void tableCellWillRecycle(CCTableView* table, CCTableViewCell* cell);

#pragma mark - ocos2d::extension::CCTableViewDataSource
    virtual CCSize tableCellSizeForIndex(CCTableView *table, unsigned int idx);
    virtual CCSize cellSizeForTable(CCTableView *table);
    virtual CCTableViewCell* tableCellAtIndex(CCTableView *table, unsigned int idx);
    virtual unsigned int numberOfCellsInTableView(CCTableView *table);

protected:
    /*! 在tableCellAtIndex()中调用，由子类继承并设置cell内容。 */
    virtual void SetCellValue(const void* data, asCCView* view) = 0;

private:
    const CCSize* GetCellSize();

private:
    asCCViewCreateFunc m_createFunc;
    asCCTableDataProvider* m_pDataProvider;
    asCCViewTouchListener* m_pTouchListener;

    CCSize* m_pCellSize;
    
};

#pragma mark - inline implement

inline void asCCTableCellRender::SetData(asCCTableDataProvider* provider) {
    NGASSERT(provider != NULL);
    SAFE_DELETE(m_pDataProvider);
    m_pDataProvider = provider;
}

inline void asCCTableCellRender::SetTouchListener(asGUI::asCCViewTouchListener* listener) {
    m_pTouchListener = listener;
}

inline CCSize asCCTableCellRender::tableCellSizeForIndex(CCTableView *table, unsigned int idx) {
    return cellSizeForTable(table);
}

inline CCSize asCCTableCellRender::cellSizeForTable(CCTableView *table) {
    return * GetCellSize();

}

inline unsigned int asCCTableCellRender::numberOfCellsInTableView(CCTableView *table) {
    if (m_pDataProvider == NULL) {
        return 0;
    } else {
        return m_pDataProvider->GetDataSize();
    }
}


NS_ASGUI_END

#endif
