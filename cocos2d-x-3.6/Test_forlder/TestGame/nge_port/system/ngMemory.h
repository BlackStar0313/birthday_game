/*
 *  ngMemory.h
 *  template
 *
 *  Created by Yuhui Wang on 10/21/10.
 *  Copyright 2010 Anansi Mobile. All rights reserved.
 *
 */

#ifndef __NGMEMORY_H__
#define __NGMEMORY_H__

#include "../core/NGE_Types.h"

//#define NGE_MEMORY_USEAGE_CHECK

#ifdef __cplusplus
extern "C" {
#endif 

	
#ifndef NGE_MEMORY_USEAGE_CHECK
	
#define DMALLOC malloc
#define DFREE free
#define SAFE_FREE(p) \
    if (p) {         \
        DFREE(p);   \
        p = NULL;   \
    }
//	void* DMALLOC_EX(unsigned int n);
//	void  DFREE_EX(void *p);

#else // NGE_MEMORY_USEAGE_CHECK
		
	typedef struct _MemoryRecord {
		void* start;
		void* end;
		int index;
		const char* file;
		int line;
		_MemoryRecord* next;
	} MemoryRecord;
	
	void addRecord(MemoryRecord *r);
	MemoryRecord* findRecord(void *p);
	MemoryRecord* removeRecord(MemoryRecord* r);
	
	void ngMemoryDumpRecord(bool free);
	void ngMemoryUsageReport();

	void* DMALLOC_EX(unsigned int n, const char* file, int line);
	void  DFREE_EX(void *p, const char* file, int line);

	#define DMALLOC(size) DMALLOC_EX(size, __FILE__, __LINE__)
	#define DFREE(p) DFREE_EX(p, __FILE__, __LINE__)	

#endif // NGE_MEMORY_USEAGE_CHECK

#ifdef __cplusplus
}
#endif

#ifdef NGE_MEMORY_USEAGE_CHECK

void* operator new(size_t n, const char* file, int line);
void* operator new[](size_t n, const char* file, int line);
void operator delete(void* p, const char* file, int line);
void operator delete(void* p);

#define DNEW						   new(__FILE__, __LINE__)
#define DDELETE(object)                delete object
#define DNEWARR(t,s)                   new t[(s)]
#define DDELETEARR(object)             delete[] (object)

#else

//void* operator new(size_t n);
//void* operator new[](size_t n);
//void operator delete(void *p);

#define DNEW						   new
#define DDELETE(object)                delete object
#define DNEWARR(t,s)                   new t[(s)]
#define DDELETEARR(object)             delete[] (object)

#endif


#endif //__NGMEMORY_H__