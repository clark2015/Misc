#ifndef _DL_PORTS_H
#define _DL_PORTS_H

#ifdef _WIN32
#include <windows.h>
#include <direct.h>
#include <Mmsystem.h>
typedef DWORD pthread_t;
#else
#include <limits.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pthread.h>
#endif
#include <assert.h>
#include <stdlib.h>
#include <time.h>

#ifndef ASSERT	//gcc里assert不是按 _DEBUG 宏来决定，统一用 ASSERT好了
	#ifndef _DEBUG
	//	#define ASSERT ((void)0)
		#define ASSERT(x)	 
	#else
		#define ASSERT assert
	#endif
#endif

//取类组大小
#define lengthof(x)	(sizeof(x)/sizeof((x)[0]))

#ifndef __max	//vc是定义在 stdlib.h里
#define __max(a,b)  (((a) > (b)) ? (a) : (b))
#define __min(a,b)  (((a) < (b)) ? (a) : (b))
#endif

#ifndef MAX_PATH	//在WinDef.h
	#define MAX_PATH	PATH_MAX
#endif

//****************************************************************

#ifndef _WIN32
	//Linux下的lstrcpyn实现
	inline char *lstrcpyn(char *pDes,const char *pSrc,int nLen)
	{
		assert(pDes && pSrc && nLen >0 );
	//	strncpy(pDes,pSrc,nLen-1);
	//	pDes[nLen-1] = 0;	

		int i;
		for (i=0; i<nLen; i++)
		{
			pDes[i] = pSrc[i];
			if ( ! pDes[i] )
				break;
		}
		if ( i == nLen && nLen != 0 )
			pDes[i-1] = 0;

		return pDes;
	}

	//wsprintf优点是速度快，缺点是只能在windows下用，且不支持浮点数，且输出不能大于1K
	#define	wsprintf	sprintf		
	
	inline int dl_mkdir(const char *pathname, int mode = 0755)	//01755)
	{
		return mkdir(pathname,mode);
	}
	
	inline pthread_t GetCurrentThreadId()	
		{	return pthread_self();	}
		
	inline bool IsThreadSame(pthread_t ulID1,pthread_t ulID2)
		{	return pthread_equal(pthread_t(ulID1),pthread_t(ulID2));	}

	#define Sleep(x)	usleep((x)*1000)
	
	inline
	bool PtInRect(const RECT *lprc,POINT pt){
		if ( !lprc )
			return false;
		return 	pt.x >= lprc->left && pt.x < lprc->right &&
				pt.y >= lprc->top  && pt.y < lprc->bottom;
	}
	
#else
	#define sleep(x)	Sleep((x)*1000)
	
	inline int dl_mkdir(const char *pathname, int mode = 0755)	//01755)	
	{
		return _mkdir(pathname);
	}
	
	inline struct tm *localtime_r(const time_t *timep, struct tm *result)
	{
	#if _MSC_VER >= 1600
		localtime_s(result,timep);
	#else
		#ifndef _MT
			assert(!"use /MT /MD pls");
		#endif
		const struct tm*p = localtime(timep);
		*result = *p;
	#endif 
		return result;
	}	
	
	inline bool IsThreadSame(pthread_t ulID1,pthread_t ulID2)
		{	return ulID1 == ulID2;	}

//	#define index		strchr			4.3BSD; marked as LEGACY in POSIX.1-2001.  POSIX.1-2008 removes the specifications of index() and rindex(), recommending strchr(3) and strrchr(3) instead.
//	#define rindex		strrchr
	#define strncasecmp _strnicmp
	#define strcasecmp	_stricmp
#endif
//****************************************************************

inline uint32 OSGetTickCount()
{
//	timeGetTime() 是 GetTickCount() 耗时的2倍多一点
//	gettimeofday()是 GetTickCount() 耗时的几十倍
#ifdef _WIN32
//	return GetTickCount();
	return timeGetTime();
#else
	struct timeval now;
	gettimeofday(&now,0);
	return now.tv_sec * 1000 + now.tv_usec/1000;
#endif
}
//****************************************************************
#ifndef _WIN32
#define ATTRIB_HIDDEN	__attribute__((visibility("hidden")))
#else
#define ATTRIB_HIDDEN	
#endif
//****************************************************************
#ifndef _WIN32
inline	uint32 InterlockedIncrement(uint32 volatile *lpAddend)	{	return __sync_add_and_fetch(lpAddend,1);	}
inline  uint64 InterlockedIncrement(uint64 volatile *lpAddend)	{	return __sync_add_and_fetch(lpAddend,1);	}
inline	uint32 InterlockedDecrement(uint32 volatile *lpAddend)	{	return __sync_sub_and_fetch(lpAddend,1);	}
inline	uint64 InterlockedDecrement(uint64 volatile *lpAddend)	{	return __sync_sub_and_fetch(lpAddend,1);	}
inline  void   MemoryBarrier()		{	__sync_synchronize();	}
#endif

inline  void   __sync_synchronize()	{	MemoryBarrier();		}

#endif
