/*
 *  asCCTableCellRender.cpp
 *  pnk
 *
 *  Created by Chen Zhen on 14-4-28.
 *  Copyright (c) 2014年 Anansi Mobile. All rights reserved.
 */

#include "asCCTableCellRender.h"

USING_NS_ASGUI;

#if 0
asCCTableCellRender::asCCTableCellRender()
: m_createFunc(NULL)
, m_pDataProvider(NULL)
, m_pCellSize(NULL)
, m_pTouchListener(NULL) {

}
#endif

asCCTableCellRender::asCCTableCellRender(asCCViewCreateFunc func)
: m_createFunc(func)
, m_pDataProvider(NULL)
, m_pCellSize(NULL)
, m_pTouchListener(NULL) {

}

asCCTableCellRender::~asCCTableCellRender() {
    SAFE_DELETE(m_pDataProvider);
}

#pragma mark - cocos2d::extension::CCTableViewDelegate
void asCCTableCellRender::tableCellTouched(CCTableView* table, CCTableViewCell* cell) {

}

void asCCTableCellRender::tableCellHighlight(CCTableView* table, CCTableViewCell* cell) {

}

void asCCTableCellRender::tableCellUnhighlight(CCTableView* table, CCTableViewCell* cell) {

}

void asCCTableCellRender::tableCellWillRecycle(CCTableView* table, CCTableViewCell* cell) {

}

#pragma mark - ocos2d::extension::CCTableViewDataSource

CCTableViewCell* asCCTableCellRender::tableCellAtIndex(CCTableView *table, unsigned int idx) {

    asCCTableViewCell* cell = (asCCTableViewCell*) table->dequeueCell();
    if (cell == NULL) {
        cell = DNEW(asCCTableViewCell);
        cell->autorelease();

        NGASSERT(m_createFunc != NULL);
        asCCView* view = m_createFunc(cell, m_pTouchListener,0);
        cell->m_pUICCView = view;

    }

    const void* pData = NULL;
    if (m_pDataProvider != NULL) {
        pData = m_pDataProvider->GetDataAtIndex(idx);
    }

    SetCellValue(pData, cell->m_pUICCView);

    return cell;
}

const CCSize* asCCTableCellRender::GetCellSize() {
    if (m_pCellSize == NULL) {
        asCCTableViewCell* cell = DNEW(asCCTableViewCell);

        NGASSERT(m_createFunc != NULL);
        asCCView* view = m_createFunc(cell, NULL,0);
        cell->m_pUICCView = view;

        UIWidget* cellLayer = (UIWidget*) ((UIWidget*) view->getRootNode()->getChildren()->objectAtIndex(0))->getChildren()->objectAtIndex(0);
        m_pCellSize = DNEW(CCSize)(cellLayer->getContentSize().width,
                                   cellLayer->getContentSize().height);

        SAFE_DELETE(cell);
    }

    return m_pCellSize;
}

