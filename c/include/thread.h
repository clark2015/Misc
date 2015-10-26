#ifndef _MY_THREAD_H_
#define _MY_THREAD_H_

#ifdef _WIN32
	#include <windows.h>
	#include <process.h>
	#ifndef _MT
		#pragma message ("��Ӧ���ڱ���ѡ���м���/MD��/MT,��ʹ�ö��߳̿�,����_beginthreadex()���Ҳ���.")
	#endif
#else
	#include <pthread.h>
#endif
#include "DLTypeDefs.h"
#include "pvoper.h"
#include "commtools.h"
#include <string.h>
/*
ע��
1.��������ٲ��ᵼ���̵߳Ľ���(���ǲ���,���Ҳ�֪����ô����:( )
2.�߳̽���ʱ����ʱ�����(m_bRuning),�����ʱ�����Ѳ�����,��������
*/
//***********************************************************************
enum 
{
#ifdef _WIN32
		MY_INVALID_THREAD_ID	=  0	//��GetThreadID()�ķ��ؿ�֪�ǶԵ�
#else
		MY_INVALID_THREAD_ID	=  0	//��һ���ԣ�pthread��û����ôһ�����壬��Linux�����ý���ģ���̵߳ģ�����һ�㲻��=0
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
		char*	m_pStackThreadName;	//��Ϊ��ʱ�Ƿ�ʱ�������Ѳ��ڣ�����Ҫ��һ��copy���̶߳�ջ
		CLock	m_Lock;

	public:
		CThread()	
			{	
			m_bRuning = false;	
			m_ulThreadID = MY_INVALID_THREAD_ID;	
			memset(m_szThreadName,0,sizeof(m_szThreadName));	
			m_pStackThreadName = NULL;
			}
			
		//����
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
		
		//�߳�����
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
		
		//�߳�ID
		uint32 GetMyID() const	//ȡ�߳�id
			{
			return m_bRuning ? m_ulThreadID : MY_INVALID_THREAD_ID;
			}

		bool  IsRuning() const	{	return m_bRuning;	}
	protected:
		virtual void ThreadProc() = 0;
		virtual void OnThreadExited()	{}	//��������delete this;
	
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
				pThis->m_pStackThreadName = NULL;	//Ҫ��OnThreadExited()֮ǰ����Ϊ�����ܻ�delete this
				}
			pThis->OnThreadExited();
			return 0;
			}
	};

//***********************************************************************

#endif
