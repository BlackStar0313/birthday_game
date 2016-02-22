/*
 *  ngInteger.h
 *  template
 *
 *  Created by Yuhui Wang on 7/18/10.
 *  Copyright 2010 Anansi Mobile. All rights reserved.
 *
 */

#ifndef __NGINTEGER_H__
#define __NGINTEGER_H__

#include "../core/NGE_Types.h"
#include "ngString.h"

class ngInteger {
public:
	ngInteger();
	ngInteger(int64 val);
	ngInteger(const ngStringV2& str);
	virtual ~ngInteger() {}
	
public:
	ngInteger& operator =(int64 val) { m_value = val; return *this; }
	boolean operator==(int64 val) const { return (boolean)(m_value == val); }
	boolean operator==( const ngInteger &r ) const { return (boolean)(m_value == r.IntValue()); }
		
public:
	static int64 ParseInt(const ngStringV2& str);
	int64 IntValue() const;
    void SetValue(int64 v) { m_value = v; }
	
protected:
	int64 m_value;
};


class ngIntegerComparator : public ngComparator 
{
public:
    virtual int32		Compare(void* a, void* b) {
        ngInteger* p1 = (ngInteger *)a;
        ngInteger* p2 = (ngInteger *)b;
#ifdef DEBUG        
        printf("compare %lld and %lld\n", p1->IntValue(), p2->IntValue());
#endif
        if (p1->IntValue() < p2->IntValue()) {
            return -1;
        }
        if (p1->IntValue() > p2->IntValue()) {
            return 1;
        }
        return 0;
    }
    virtual void		Destroy() {}
};



#endif //__NGINTEGER_H__