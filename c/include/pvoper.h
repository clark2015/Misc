#ifndef _CPV_OPER_H
#define _CPV_OPER_H

/*
2004		��袳�  �����ļ�
2004-11-02	��袳�  ����CLockBlock��
2004-01-30	��袳�  ʹCLockBlock���������Ҫʱ�ż���
2006-09-22  ��袳�  ����CLock��
2007-02-26  ��袳�  ��ǰ��PV�������������ˣ�����
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

/*	����CLockBlock���ڶԴ�����ڵĲ�������
example:

CLock g_Lock;

{
	CLockBlock(&g_Lock);
	.....	//����Ҫ�����Ĳ���,�����ڶദreturn�����ص������ǽ���
}
*/
class CLockBlock
	{
	protected:		
		CLock	*m_pLock;
	public:
		CLockBlock(CLock *p)	{	Lock(p); 							}
		~CLockBlock()			{	if (m_pLock)	m_pLock->Unlock();	}
		//ʹ��������Ҫʱ�ż���
		CLockBlock()			{	m_pLock = NULL;						}
		void Lock(CLock *p)		{	m_pLock = p;	m_pLock->Lock();	}
	};
//*************************************************
#endif
