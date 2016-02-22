/*
 *  ngJsonFactory.h
 *  NextGenEngine
 *
 *  Created by Yuhui Wang on 5/26/10.
 *  Copyright 2010 DreaminGame. All rights reserved.
 *
 */


#ifndef __NGJSONFACTORY_H__
#define __NGJSONFACTORY_H__

class ngJsonNode;
class ngJsonFactory 
{
public:
	static ngJsonNode* CreateJsonNode(int type);
};

#endif //__NGJSONFACTORY_H__