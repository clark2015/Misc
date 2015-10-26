#ifndef IASYN_PROC
#define IASYN_PROC
#include <stdio.h>
#include <stdlib.h>
#include "ILogger.h"
#include "IAsynObj.h"
/*
一个异步队列对象

2004  	   		蓝琚成				建立文件
2005-05-06		蓝琚成				使用Logger.dll
2005-07-10		蓝琚成	  v2		增加接口OnGetDesc()及对应处理
2006-07-14		蓝琚成	  v3		去掉IAsynObj一些多余的接口,去掉ar_ret,ar_retry,去掉dump
									增加IAsynProcQueue及取它的接口
2006-08-03		蓝琚成	  v3.1		增加AddExitThreadObject()接口
									增加SetPriority()接口
									IAsynProcQueue::GetSizes()去掉's'
2006-08-24		蓝琚成	  v3.2		增加GetThreadID()接口
2006-10-13		蓝琚成	  v4.0		简化接口
2006-11-14		蓝琚成	  v4.1		增加AddHead()
2007-01-05		蓝琚成	  v4.1.1	修正GetSize()的多线程安全问题
2007-07-24/25	蓝琚成	  v4.1.3	增加SetQueueName(),GetQueueName(),
									增加IsThreadExited(),IsExitObjectAdded(),
									释构时自动调AddExitThreadObject()，但还不能保证释构前线程已退出
									不升级主次版本以便跟以前兼容
2008-02-16		蓝琚成	  v4.1.4	Windows下为各个队列增加CrashDump支持，但这样它就依赖于dbgHelp.dll了
2008-06-02		蓝琚成	  v4.1.5	增加GetProcedCount(),不升级主次版本以便跟以前兼容
2009-11-19-23	蓝琚成	  v4.1.6	取主程序的ExceptionCustomize
2013-09-11		蓝琚成	  v4.1.8	linux下catch住异常，并继续运行
2013-10-11		蓝琚成	  v4.1.9	IAsynProcQueue也增加AddExitThreadObject()
2014-12-09				  v4.1.10	增加AddNOPObject()，调整了一下接口顺序

*/

#ifndef EXPORT_FUNC
	#error "Please include commInterfaceDef.h first";
#endif
//************************************************************************
#define  VER_ASYN_PROC_MAIN		4		//主版本增加是不兼容升级
#define  VER_ASYN_PROC_MIN		1		//次版本增加是兼容的升级(功能增加等)
#define  VER_ASYN_PROC_BUILD	10		//build版本增加是功能不变的bug修正升级
//************************************************************************

//单个队列，这样好处理一些
class IAsynProcQueue
	{
	public:
		virtual void		Release()					= 0;
		virtual bool		Add(IAsynObj *pAsyObj)		= 0;
		virtual bool		AddHead(IAsynObj *pAsyObj)	= 0;	//在队列头加入(这是无耻的插队行为)
		virtual int			GetSize() 					= 0;	//队列里有多少个未处理的对象
		virtual uint32		GetProcedCount()	const	= 0;	//已处理了多少个对象
		virtual void		SetQueueLong(long l) 		= 0;
		virtual long		GetQueueLong()				= 0;
		//用于调试时分辨各个线程/队列
		virtual void		SetQueueName(const char*s)	= 0;	
		virtual const char* GetQueueName()	const		= 0;
		//线程属性
		virtual void		SetPriority(int nPrio)		= 0;	
		virtual uint32		GetThreadID() const			= 0;
		//退出处理
		virtual void		AddExitThreadObject()		= 0;
		virtual bool		IsExitObjectAdded()	const	= 0;
		virtual bool		IsThreadExited()	const	= 0;
		virtual bool		AddNopObject()				= 0;
	};

//异步队列管理
class IAsynProcMagr
	{
	public:
		virtual void Release()								= 0;	
		virtual IAsynProcQueue*	GetQueue(int nQueue)		= 0;

		//与IAsynProcQueue的同名接口相同,只是批量处理
		virtual bool Add(IAsynObj *pAsyObj,int nQueue = -1)	= 0;	//nQueue = -1则随机加进一个队列
		virtual int  GetSizes(int pSizes[] = NULL) 			= 0;

		//对所有线程，让其处理完现时队列中的东西后退出
		//调用完个接口后，Add()将失败
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
	
			//检查版本
			IAsynProc_CheckVersion_Func CheckVersion;
			CheckVersion  =  (IAsynProc_CheckVersion_Func) GetProcAddress(m_hLib, "IAsynProc_CheckVersion");
			if ( NULL == CheckVersion )
				{
				RelTrace("function IAsynProc_CheckVersion() not found");
				goto out;
				}
			if ( ! CheckVersion(VER_ASYN_PROC_MAIN,VER_ASYN_PROC_MIN,VER_ASYN_PROC_BUILD) )
				goto out;
			//生成异步处理对象
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

	//重载一下以禁止外部调用
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
