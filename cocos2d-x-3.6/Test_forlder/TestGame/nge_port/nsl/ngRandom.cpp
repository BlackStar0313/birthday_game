
/* 
   A C-program for MT19937, with initialization improved 2002/1/26.
   Coded by Takuji Nishimura and Makoto Matsumoto.

   Before using, initialize the state by using init_genrand(seed)  
   or init_by_array(init_key, key_length).

   Copyright (C) 1997 - 2002, Makoto Matsumoto and Takuji Nishimura,
   All rights reserved.                          

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:

     1. Redistributions of source code must retain the above copyright
        notice, this list of conditions and the following disclaimer.

     2. Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution.

     3. The names of its contributors may not be used to endorse or promote 
        products derived from this software without specific prior written 
        permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


   Any feedback is very welcome.
   http://www.math.sci.hiroshima-u.ac.jp/~m-mat/MT/emt.html
   email: m-mat @ math.sci.hiroshima-u.ac.jp (remove space)
*/

#include <nsl/ngRandom.h>
#include <core/NGE_Macros.h>

static ngRandom* gRandom = NULL;

ngRandom* ngRandom::GetInstance()
{
	if (gRandom == NULL) {
		gRandom = DNEW ngRandom();
	}
	return gRandom;
}

void ngRandom::ReleaseInstance(void *pObj)
{
	ngRandom* pThis = (ngRandom *)pObj;
	DELETE_OBJECT(pThis, ngRandom);
}

ngRandom::ngRandom()
: ngSingleton( ReleaseInstance, this )
{
	// initialize
	m_mti = RANDGEN_N + 1;
	m_mag01[0] = 0x0UL;
	m_mag01[1] = RANDGEN_MATRIX_A;

	// seed 
	Seed( time(NULL) );
}

	
ngRandom::~ngRandom()
{
}


/* initializes mt[N] with a seed */
void ngRandom::Seed( uint32 seed )
{
	int i;

    m_mt[0] = seed & 0xffffffffUL;
    for ( i = 1; i < RANDGEN_N; i++ ) 
	{
        m_mt[i] = 
			(1812433253UL * ( m_mt[i - 1] ^ (m_mt[i - 1] >> 30)) + i ); 
        /* See Knuth TAOCP Vol2. 3rd Ed. P.106 for multiplier. */
        /* In the previous versions, MSBs of the seed affect   */
        /* only MSBs of the array mt[].                        */
        /* 2002/01/09 modified by Makoto Matsumoto             */
        m_mt[i] &= 0xffffffffUL;
        /* for >32 bit machines */
    }
   m_mti = RANDGEN_N;
}


/* generates a random number on [0,0xffffffff]-interval */
uint32 ngRandom::Generate()
{
    uint32 y;

	/* generate N words at one time */
    if(m_mti >= RANDGEN_N) 
	{ 
        int kk;

        if(m_mti == RANDGEN_N+1)   /* if RNGSeed() has not been called, */
            Seed( time(NULL) ); /* a default initial seed is used   */

        for (kk=0;kk<RANDGEN_N-RANDGEN_M;kk++) {
            y = (m_mt[kk]&RANDGEN_UPPER_MASK)|(m_mt[kk+1]&RANDGEN_LOWER_MASK);
            m_mt[kk] = m_mt[kk+RANDGEN_M] ^ (y >> 1) ^ m_mag01[y & 0x1UL];
        }
        for (;kk<RANDGEN_N-1;kk++) {
            y = (m_mt[kk]&RANDGEN_UPPER_MASK)|(m_mt[kk+1]&RANDGEN_LOWER_MASK);
            m_mt[kk] = m_mt[kk+(RANDGEN_M-RANDGEN_N)] ^ (y >> 1) ^ m_mag01[y & 0x1UL];
        }
        y = (m_mt[RANDGEN_N-1]&RANDGEN_UPPER_MASK)|(m_mt[0]&RANDGEN_LOWER_MASK);
        m_mt[RANDGEN_N-1] = m_mt[RANDGEN_M-1] ^ (y >> 1) ^ m_mag01[y & 0x1UL];

        m_mti = 0;
    }
  
    y = m_mt[m_mti++];

    /* Tempering */
    y ^= (y >> 11);
    y ^= (y << 7) & 0x9d2c5680UL;
    y ^= (y << 15) & 0xefc60000UL;
    y ^= (y >> 18);

    return y;
}


/* generates a random number on [0,upperLimit)-interval */
uint32 ngRandom::GetRand( uint32 upperLimit )
{
	return ( Generate() % upperLimit ); 
}


/* generates a random number on [lowerlimit,upperLimit]-interval */
uint32 ngRandom::GetRandRange( int32 lowerlimit, uint32 upperLimit )
{
	return ( lowerlimit + Generate() % (1+upperLimit-lowerlimit) ); 
}
