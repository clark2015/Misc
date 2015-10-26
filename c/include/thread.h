#ifndef _MY_THREAD_H_
#define _MY_THREAD_H_

#ifdef _WIN32
	#include <windows.h>
	#include <process.h>
	#ifndef _MT
		#pragma message ("你应该在编译选项中加上/MD或/MT,以使用多线程库,否则_beginthreadex()会找不到.")
	#endif
#else
	#include <pthread.h>
#endif
#include "DLTypeDefs.h"
#include "pvoper.h"
#include "commtools.h"
#include <string.h>
/*
注意
1.对象的销毁不会导致线程的结束(不是不想,但我不知道怎么做啊:( )
2.线程结束时会访问本对象(m_bRuning),如果这时对象已不可用,则会出问题
*/
//***********************************************************************
enum 
{
#ifdef _WIN32
		MY_INVALID_THREAD_ID	=  0	//看GetThreadID()的返回可知是对的
#else
		MY_INVALID_THREAD_ID	=  0	//不一定对，pthread并没有这么一个定义，但Linux下是用进程模拟线程的，所以一般不会=0
#endif
};
//***********************************************************************
class CThread
	{
		enum{	THREAD_NAME_LENTGH = 24	};
	protected:
		bool	m_bRuning;
		uint32	m_ulThreadID;
		char	m_szThreadName[THREAD_NAME_LENTGH];
		char*	m_pStackThreadName;	//因为有时非法时本对象已不在，所以要放一个copy在线程堆栈
		CLock	m_Lock;

	public:
		CThread()	
			{	
			m_bRuning = false;	
			m_ulThreadID = MY_INVALID_THREAD_ID;	
			memset(m_szThreadName,0,sizeof(m_szThreadName));	
			m_pStackThreadName = NULL;
			}
			
		//建立
		bool Create()
			{
			if ( m_bRuning )
				return false;
			m_bRuning = true;
			bool bRet = false;
		#ifdef  _WIN32
			unsigned tid;
			bRet = 0 != _beginthreadex(NULL,0,StaticThreadProc,this,0,&tid);
			if ( bRet )
				m_ulThreadID = (uint32)tid;
		#else
			pthread_t  td;
			bRet = 0 == pthread_create(&td,NULL,StaticThreadProc,this);
			if ( bRet )
			{
				pthread_detach(td);
				m_ulThreadID = (uint32)td;
			}
		#endif
			if ( !bRet )
				m_bRuning = false;
			return bRet;
			}
		
		//线程名字
		void SetMyName(const char*p)
			{
			CLockBlock lock(&m_Lock);
			if ( NULL == p )
				{
				memset(m_szThreadName,0,sizeof(m_szThreadName));
				}
			else
				{
				lstrcpyn(m_szThreadName,p,sizeof(m_szThreadName));
				}
			if ( m_pStackThreadName )
				lstrcpyn(m_pStackThreadName,m_szThreadName,sizeof(m_szThreadName));
			}
		const char* GetMyName()	const
			{
			return m_szThreadName;
			}
		
		//线程ID
		uint32 GetMyID() const	//取线程id
			{
			return m_bRuning ? m_ulThreadID : MY_INVALID_THREAD_ID;
			}

		bool  IsRuning() const	{	return m_bRuning;	}
	protected:
		virtual void ThreadProc() = 0;
		virtual void OnThreadExited()	{}	//可以用来delete this;
	
	private:
	#ifdef _WIN32
		static unsigned WINAPI StaticThreadProc(LPVOID p)
	#else
		static void *StaticThreadProc(void *p)
	#endif
			{
			char szThreadName[THREAD_NAME_LENTGH];
			CThread *pThis = (CThread*)p;
				{
				CLockBlock lock(&pThis->m_Lock);
				pThis->m_pStackThreadName = szThreadName;
				memcpy(szThreadName,pThis->m_szThreadName,sizeof(szThreadName));
				}
			pThis->ThreadProc();
			pThis->m_bRuning = false;
				{
				CLockBlock lock(&pThis->m_Lock);
				pThis->m_pStackThreadName = NULL;	//要在OnThreadExited()之前，因为它可能会delete this
				}
			pThis->OnThreadExited();
			return 0;
			}
	};

//***********************************************************************

#endif
