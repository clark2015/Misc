#ifndef IASYN_PROC
#define IASYN_PROC
#include <stdio.h>
#include <stdlib.h>
#include "ILogger.h"
#include "IAsynObj.h"
/*
һ���첽���ж���

2004  	   		��袳�				�����ļ�
2005-05-06		��袳�				ʹ��Logger.dll
2005-07-10		��袳�	  v2		���ӽӿ�OnGetDesc()����Ӧ����
2006-07-14		��袳�	  v3		ȥ��IAsynObjһЩ����Ľӿ�,ȥ��ar_ret,ar_retry,ȥ��dump
									����IAsynProcQueue��ȡ���Ľӿ�
2006-08-03		��袳�	  v3.1		����AddExitThreadObject()�ӿ�
									����SetPriority()�ӿ�
									IAsynProcQueue::GetSizes()ȥ��'s'
2006-08-24		��袳�	  v3.2		����GetThreadID()�ӿ�
2006-10-13		��袳�	  v4.0		�򻯽ӿ�
2006-11-14		��袳�	  v4.1		����AddHead()
2007-01-05		��袳�	  v4.1.1	����GetSize()�Ķ��̰߳�ȫ����
2007-07-24/25	��袳�	  v4.1.3	����SetQueueName(),GetQueueName(),
									����IsThreadExited(),IsExitObjectAdded(),
									�͹�ʱ�Զ���AddExitThreadObject()���������ܱ�֤�͹�ǰ�߳����˳�
									���������ΰ汾�Ա����ǰ����
2008-02-16		��袳�	  v4.1.4	Windows��Ϊ������������CrashDump֧�֣�����������������dbgHelp.dll��
2008-06-02		��袳�	  v4.1.5	����GetProcedCount(),���������ΰ汾�Ա����ǰ����
2009-11-19-23	��袳�	  v4.1.6	ȡ�������ExceptionCustomize
2013-09-11		��袳�	  v4.1.8	linux��catchס�쳣������������
2013-10-11		��袳�	  v4.1.9	IAsynProcQueueҲ����AddExitThreadObject()
2014-12-09				  v4.1.10	����AddNOPObject()��������һ�½ӿ�˳��

*/

#ifndef EXPORT_FUNC
	#error "Please include commInterfaceDef.h first";
#endif
//************************************************************************
#define  VER_ASYN_PROC_MAIN		4		//���汾�����ǲ���������
#define  VER_ASYN_PROC_MIN		1		//�ΰ汾�����Ǽ��ݵ�����(�������ӵ�)
#define  VER_ASYN_PROC_BUILD	10		//build�汾�����ǹ��ܲ����bug��������
//************************************************************************

//�������У������ô���һЩ
class IAsynProcQueue
	{
	public:
		virtual void		Release()					= 0;
		virtual bool		Add(IAsynObj *pAsyObj)		= 0;
		virtual bool		AddHead(IAsynObj *pAsyObj)	= 0;	//�ڶ���ͷ����(�����޳ܵĲ����Ϊ)
		virtual int			GetSize() 					= 0;	//�������ж��ٸ�δ����Ķ���
		virtual uint32		GetProcedCount()	const	= 0;	//�Ѵ����˶��ٸ�����
		virtual void		SetQueueLong(long l) 		= 0;
		virtual long		GetQueueLong()				= 0;
		//���ڵ���ʱ�ֱ�����߳�/����
		virtual void		SetQueueName(const char*s)	= 0;	
		virtual const char* GetQueueName()	const		= 0;
		//�߳�����
		virtual void		SetPriority(int nPrio)		= 0;	
		virtual uint32		GetThreadID() const			= 0;
		//�˳�����
		virtual void		AddExitThreadObject()		= 0;
		virtual bool		IsExitObjectAdded()	const	= 0;
		virtual bool		IsThreadExited()	const	= 0;
		virtual bool		AddNopObject()				= 0;
	};

//�첽���й���
class IAsynProcMagr
	{
	public:
		virtual void Release()								= 0;	
		virtual IAsynProcQueue*	GetQueue(int nQueue)		= 0;

		//��IAsynProcQueue��ͬ���ӿ���ͬ,ֻ����������
		virtual bool Add(IAsynObj *pAsyObj,int nQueue = -1)	= 0;	//nQueue = -1������ӽ�һ������
		virtual int  GetSizes(int pSizes[] = NULL) 			= 0;

		//�������̣߳����䴦������ʱ�����еĶ������˳�
		//��������ӿں�Add()��ʧ��
		virtual void AddExitThreadObject(uint32 ulReserved = -1)	= 0;
	};
typedef IAsynProcMagr	IAsynProc;

//************************************************************************

#ifdef _DEBUG
	#define ASYNPROC_DLL_NAME "./AsynProcD"DLL_EXT
#else
	#define ASYNPROC_DLL_NAME "./AsynProc"DLL_EXT
#endif

class CAsynProcLoader
	{
	typedef bool 		(*IAsynProc_CheckVersion_Func)(int verMain,int verMin,int verBuild);
	typedef IAsynProc * (*IAsynProc_Create_Func)(int nQueues);

	public:
		CAsynProcLoader()	
			{
			m_pProc = NULL;
			m_hLib  = NULL;
			}
		~CAsynProcLoader()
			{
			Close();
			}
		void Close()
			{
			if ( m_pProc )
				{
				m_pProc->Release();
				m_pProc  = NULL;
				}
			
			if ( m_hLib )
				{
				FreeLibrary(m_hLib);
				m_hLib  = NULL;
				}
			}
		bool Create(int nQueues)
			{
			m_hLib = LoadLibrary (ASYNPROC_DLL_NAME);
			if ( NULL == m_hLib )
				{
				RelTrace("Can not LoadLibrary %s",ASYNPROC_DLL_NAME);
				return false;
				}
	
			//���汾
			IAsynProc_CheckVersion_Func CheckVersion;
			CheckVersion  =  (IAsynProc_CheckVersion_Func) GetProcAddress(m_hLib, "IAsynProc_CheckVersion");
			if ( NULL == CheckVersion )
				{
				RelTrace("function IAsynProc_CheckVersion() not found");
				goto out;
				}
			if ( ! CheckVersion(VER_ASYN_PROC_MAIN,VER_ASYN_PROC_MIN,VER_ASYN_PROC_BUILD) )
				goto out;
			//�����첽�������
			IAsynProc_Create_Func createfunc;
			createfunc  =  (IAsynProc_Create_Func )GetProcAddress(m_hLib, "IAsynProc_Create");
			if ( NULL == createfunc )
				{
				RelTrace("function IAsynProc_Create() not found");
				goto out;
				}
			m_pProc = createfunc(nQueues);
				
		out:
			if ( NULL == m_pProc )
				{
				FreeLibrary(m_hLib);
				m_hLib = NULL;
				}
			
			return m_pProc != NULL;
			}
		
			  IAsynProc* operator ->()			{	return m_pProc;			}
		const IAsynProc* operator ->()	const	{	return m_pProc;			}
			  IAsynProc* GetInterface()			{	return m_pProc;			}
		const IAsynProc* GetInterface()	const	{	return m_pProc;			}
			  IAsynProc* GetAsynProc()			{	return m_pProc;			}
		const IAsynProc* GetAsynProc()	const	{	return m_pProc;			}
		bool  IsValid()					const	{	return m_pProc != NULL;	}
		
	protected:
		HMODULE 	m_hLib;
		IAsynProc*	m_pProc;	
	};
//************************************************************************

#include "commInterfaceDef.h"

class CAsynQueueLoader : public InterfaceLoader<IAsynProcQueue,VER_ASYN_PROC_MAIN,VER_ASYN_PROC_MIN,VER_ASYN_PROC_BUILD>
{
	const char*	m_pProcName;
protected:
	virtual const char *GetDllFileName()	{	return ASYNPROC_DLL_NAME;	}
	virtual const char *GetProcName()		{	return m_pProcName;	}

	//����һ���Խ�ֹ�ⲿ����
	bool Create(char szErrorMsg[] = 0 ,int nLen = 0)
	{
		return InterfaceLoader<IAsynProcQueue,VER_ASYN_PROC_MAIN,VER_ASYN_PROC_MIN,VER_ASYN_PROC_BUILD>::Create(szErrorMsg,nLen);
	}
public:
	bool Create2(bool bDoubleQueue = false)
	{
		if ( bDoubleQueue )
			m_pProcName = "CreateAsynQueue2";
		else
			m_pProcName = "CreateAsynQueue";
		char szErrorMsg[128];
		bool b = Create(szErrorMsg,sizeof(szErrorMsg));
		if ( !b )
			RelTrace("%s",szErrorMsg);
		return b;
	}
};


#endif
