#ifndef _CPV_OPER_H
#define _CPV_OPER_H

/*
2004		蓝琚成  建立文件
2004-11-02	蓝琚成  增加CLockBlock类
2004-01-30	蓝琚成  使CLockBlock类可以在需要时才加锁
2006-09-22  蓝琚成  增加CLock类
2007-02-26  蓝琚成  以前把PV操作的语义搞错了，修正
*/
//*************************************************
#include "sem.h"
class CPVOper : public CSem
	{
	public:
		void P()	{	Down();	}
		void V()	{	Up();	}
	};
//*************************************************

#ifdef _WIN32

#include <Windows.h>
class CLock
	{
	protected:	
		CRITICAL_SECTION m_section;
		
	public:
		CLock()				{	InitializeCriticalSection(&m_section);	}
		~CLock()			{	DeleteCriticalSection(&m_section);		}

		inline void Lock()	{	EnterCriticalSection(&m_section);		}
		inline void Unlock(){	LeaveCriticalSection(&m_section);		}
		//	TryEnterCriticalSection		
	};

#else

#include <pthread.h>

class CLock
	{
	protected:	
		pthread_mutex_t m_mut;
	public:
		CLock()
			{
			pthread_mutex_t tmp = PTHREAD_MUTEX_INITIALIZER;
			m_mut = tmp;
			}

		inline void Lock()		{	pthread_mutex_lock(&m_mut);		}
		inline void Unlock()	{	pthread_mutex_unlock(&m_mut);	}
	};

#endif	//_WIN32
//*************************************************

/*	下面CLockBlock用于对代码块内的操作加锁
example:

CLock g_Lock;

{
	CLockBlock(&g_Lock);
	.....	//做需要加锁的操作,并可在多处return而不必担心忘记解锁
}
*/
class CLockBlock
	{
	protected:		
		CLock	*m_pLock;
	public:
		CLockBlock(CLock *p)	{	Lock(p); 							}
		~CLockBlock()			{	if (m_pLock)	m_pLock->Unlock();	}
		//使可以在需要时才加锁
		CLockBlock()			{	m_pLock = NULL;						}
		void Lock(CLock *p)		{	m_pLock = p;	m_pLock->Lock();	}
	};
//*************************************************
#endif
