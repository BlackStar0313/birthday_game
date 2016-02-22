/*
 *  ngHttpSessionCocoa.h
 *  NextGenEngine
 *
 *  Created by Yuhui Wang on 5/22/10.
 *  Copyright 2010 DreaminGame. All rights reserved.
 *
 */

#ifndef __NGHTTPSESSIONCOCOA_H__
#define __NGHTTPSESSIONCOCOA_H__

#import "ngHttpSession.h"
#import "ngStringV2.h"
#import <UIKit/UIKit.h>
#import "IZURLConnection.h"

class ngHttpSessionCocoa;

@interface HttpSessionDelegate : NSObject
{
	ngHttpSessionCocoa* m_pParent;
}

- (id)initWithParent:(ngHttpSessionCocoa *)parent;

@end


class ngHttpSessionCocoa : public ngHttpSession 
{
public:
	ngHttpSessionCocoa();
	~ngHttpSessionCocoa();
protected:
	virtual void RequestInternal(ngStringV2& url);
	virtual void CancelInternal();
	
protected:
	IZURLConnection* m_pConnection;
	HttpSessionDelegate* m_pSessionDelegate;
};


#endif //__NGHTTPSESSIONCOCOA_H__