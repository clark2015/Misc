#ifndef ___SEM_H_____
#define ___SEM_H_____
#include <stdio.h>
#include <stdlib.h>

/*
2004		蓝琚成  建立文件
2007-07-04	蓝琚成  使用Trace宏代替printf
*/
#ifndef RelTrace
//	#define	RelTrace	puts
	#error "pls include ILogger.h, or define RelTrace as printf"
#endif

#ifndef SemDebugTrace
//	#define SemDebugTrace	(void*)0
	#define SemDebugTrace
#endif

#ifdef _WIN32
#include <windows.h>

class CSem
	{
	protected:
		HANDLE m_hSem;
	public:
		CSem()
			{
			m_hSem = CreateSemaphore(NULL,0,0x7FFFFFFE,NULL);
			if ( NULL == m_hSem )
				RelTrace("CSem::CSem() fail! lastError=%d,this=0x%X,handle=0x%X",GetLastError(),this,m_hSem);
			else
				SemDebugTrace("sem (this=0x%X,handle=0x%X) created",this,m_hSem);
			}
	
		~CSem()
			{
			if ( NULL != m_hSem )
				{
				if ( !CloseHandle(m_hSem) )
					RelTrace("CSem::CSem() : call CloseHandle() fail! lastError=%d,this=0x%X,handle=0x%X",GetLastError(),this,m_hSem);
				else
					SemDebugTrace("sem (this=0x%X,handle=0x%X) closed",this,m_hSem);
				}
			}
	
		inline void Up()
			{
			if ( !	ReleaseSemaphore(m_hSem,1,NULL) )
				RelTrace("CSem::Up() fail! lastError=%d,this=0x%X,handle=0x%X",GetLastError(),this,m_hSem);
			}
	
		inline void Down()
			{
			if ( WAIT_FAILED == WaitForSingleObject(m_hSem,INFINITE) )
				RelTrace("CSem::Down() fail! lastError=%d,this=0x%X,handle=0x%X",GetLastError(),this,m_hSem);
			}
	};

#else

#include <pthread.h>
#include <semaphore.h>
class CSem
	{
	protected:
		sem_t  m_sem;
	public:
		CSem()
			{
			if (0 != sem_init(&m_sem,0,0) )		//1) )
				RelTrace("不能初始化信号量!");
			}
		~CSem()
			{
			sem_destroy(&m_sem);
			}
		
		inline void Down()
			{
			sem_wait(&m_sem);
			}
			
		inline void Up()
			{
			sem_post(&m_sem);
			}
	};
#endif

#endif
