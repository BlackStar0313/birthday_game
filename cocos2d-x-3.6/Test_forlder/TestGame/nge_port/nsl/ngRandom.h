/*
 *  ngRandom.h
 *  NGE
 *
 *  Created by Yuhui Wang on 12/07/10.
 *  Copyright 2010 DreaminGame. All rights reserved.
 *
 */

#ifndef __NGRANDOM_H__
#define __NGRANDOM_H__

#include "../core/NGE_Types.h"
#include "../core/ngSingleton.h"

/* Period parameters */  
#define RANDGEN_N 624
#define RANDGEN_M 397
#define RANDGEN_MATRIX_A 0x9908b0dfUL   /* constant vector a */
#define RANDGEN_UPPER_MASK 0x80000000UL /* most significant w-r bits */
#define RANDGEN_LOWER_MASK 0x7fffffffUL /* least significant r bits */


class ngRandom : public ngSingleton
{
public:
	virtual                  ~ngRandom();
	static					ngRandom* GetInstance();
	static					void ReleaseInstance(void *pObj);

	/* initializes mt[N] with a seed */
	void                     Seed( uint32 seed );

	/* generates a random number on [0,0xffffffff]-interval */
	uint32                   Generate();

	/* generates a random number on [0,upperLimit)-interval */
	uint32                   GetRand( uint32 upperLimit );
    /* generates a random number on [lowerlimit,upperLimit]-interval */
	uint32                   GetRandRange( int32 lowerlimit, uint32 upperLimit );

protected:
	                         ngRandom();

	int                      m_mti;           /* mti==N+1 means mt[N] is not initialized */
	uint32                   m_mag01[2];      /* mag01[x] = x * MATRIX_A  for x=0,1 */
	uint32                   m_mt[RANDGEN_N];         /* the array for the state vector  */

private:
	// disable default copy constructor and assignment operator
	                         ngRandom( const ngRandom &source ); 
	ngRandom &               operator=( const ngRandom &source );
};



#endif	// #ifndef __NGRANDOM_H__
