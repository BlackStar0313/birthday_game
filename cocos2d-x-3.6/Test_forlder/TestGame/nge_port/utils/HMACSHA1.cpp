//
//  HMACSHA1.c
//  pet_native_project_lib
//
//  Created by chen on 12-10-30.
//  Copyright (c) 2012年 Anansi. All rights reserved.
//

#include <utils/HMACSHA1.h>

#define SHA_BLOCKSIZE   64

#if defined (NGE_PLATFORM_ANDROID)

void HMAC_SHA1(const u_char* d, int ld, const u_char* k, int lk, char* ret) {
    SHA1_CTX ictx, octx;
	u_char isha[SHA1_DIGEST_LENGTH], osha[SHA1_DIGEST_LENGTH];
	u_char key[SHA1_DIGEST_LENGTH];
	u_char buf[SHA_BLOCKSIZE];
	int i;

	if (lk > SHA_BLOCKSIZE) {
        
        SHA1_CTX tctx;

        SHA1Init(&tctx);
        SHA1Update(&tctx, k, lk);
        SHA1Final(key, &tctx);
        
        k = key;
        lk = SHA1_DIGEST_LENGTH;
    }
    
    /**** Inner Digest ****/
    
	SHA1Init(&ictx);
    
	/* Pad the key for inner digest */
	for (i = 0; i < lk; ++i) buf[i] = k[i] ^ 0x36;
	for (i = lk; i < SHA_BLOCKSIZE; ++i) buf[i] = 0x36;
    
	SHA1Update(&ictx, buf, SHA_BLOCKSIZE);
	SHA1Update(&ictx, d, ld);
    
	SHA1Final(isha, &ictx);
    
    /**** Outter Digest ****/
    
	SHA1Init(&octx);
    
    /* Pad the key for outter digest */
    
	for (i = 0; i < lk; ++i) buf[i] = k[i] ^ 0x5C;
	for (i = lk; i < SHA_BLOCKSIZE; ++i) buf[i] = 0x5C;
    
	SHA1Update(&octx, buf, SHA_BLOCKSIZE);
	SHA1Update(&octx, isha, SHA1_DIGEST_LENGTH);
    
	SHA1Final(osha, &octx);
    
    for (int i = 0; i < SHA1_DIGEST_LENGTH; i++) {
        ret[i] = osha[i];
    }
}

#elif defined (NGE_PLATFORM_COCOA)
void HMAC_SHA1(const u_char* d, int ld, const u_char* k, int lk, char* ret) {
    CCHmac(kCCHmacAlgSHA1, k, lk, d, ld, ret);
}

#endif