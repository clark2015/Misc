#ifndef __DL_ITIMER_H__
#define __DL_ITIMER_H__
#include "commInterfaceDef.h"
#include <assert.h>
#include "IUnknownEx.h"
#include "DLTypeDefs.h"
#ifdef _WIN32
	#include <Mmsystem.h>
	#pragma comment(lib,"Winmm")
#else
	#include <sys/time.h>
#endif

#define VER_TIMER_MAIN		6
#define VER_TIMER_SUB		0
#define VER_TIMER_BUILD		0

static const CLASSUID	IF_UID_ITIMER  ("ITimer�ӿ�");
static const uint32		IF_VER_ITIMER  = INTERFACE_VER(VER_TIMER_MAIN,VER_TIMER_SUB);
//****************************************************************
/*
    ʱ����,����ʱ���Ľӿ�.��һ�����������������һ��ʱ��,�򽨸��߳�,��ʱ
����Pulse(),�ӿڽ�����Ƿ��ж�ʱ����ʱ��.
    ע��:
    1.�˽ӿڼٶ����д���������ͬһ�߳���(�����Ƕ��̰߳�ȫ��)
    2.Timer������� >= 10 ms �� <= 20��

�޸���ʷ:
����		�޸���	�汾	�޸�	

2007-12-09	��袳�	4.0.0	֧��IUnknownEx2��ѯ�ӿ�
							����SetTimerDesc()
							����GetTimeOutSinkVersion()
							���Ӽ���OnTimer�����Ĺ���
2007-12-20	��袳�	5.0.0	����GetInterval()
							������ulInterval������Pulse()
2008-02-15	��袳�	5.0.1	SetTimer�Ĳ���bResetCountIfExist��ΪbResetIfExist,��������Ϣ������
2008-03-11	��袳�	5.1.0	����SetName()/GetName()
2010-08-16	        5.2.0	�Ѷ�CULong64��������Ϊunsigned long long 
2014-10-30	        5.3.0	����ulong��Ϊuint32
							ȥ��GetInterval()
							����PulseLoop()
2015-12             6.0.0   ��ȫ��д�ˣ����ڻص�ʱ�䶼��ȷ��10ms֮���ˣ��ٶ�Ҳ����˺ܶ�
							OnTimer()���ٷ���ֵ
							ȥ��һЩûʲô�õĽӿ�



*/

#ifndef OLD_TIMER_H__

class ITimeLine;

class ITimeOutSink
{
public:
	enum	{	TIMER_OUT_SINK_VER	= 3	};
	virtual uint32	GetTimeOutSinkVersion()	const {	return TIMER_OUT_SINK_VER;	}

	virtual void	OnTimer(ITimeLine*pTimeLine,uint uiTimerID)	= 0;
};

class ITimeLine : public IUnknownEx2
{
public:
	//ʱ������
	virtual void		Pulse(uint32 uiInterval)	= 0;
	
	//ע��uiTimerID��pSinkһ�����Ψһ��
	//uiStartMS�Ƕ�ú��һ�ε��ã����=0��uiInvMS��
	//uiInvMS��uiStartMS����>=10ms
	virtual bool		SetTimer		(uint uiTimerID, ITimeOutSink *pSink, uint32 uiInvMS,uint32 uiStartMS = 0,const char*pDesc = 0)	= 0;
	virtual bool		KillTimer		(uint uiTimerID, ITimeOutSink *pSink)				= 0;
	virtual bool		IsTimerSeted	(uint uiTimerID, const	ITimeOutSink *pSink) const	= 0;
	virtual uint		GetCalledCount	(uint uiTimerID, const	ITimeOutSink *pSink) const	= 0;	//ע��ʱ��̫�����ֵ�����,timer�������򷵻�0

	//ȡ�������˶��
	virtual uint32		GetTickCount()		const 	= 0;
	virtual uint64		GetTickCount64()	const 	= 0;

	//��������,����Traceʱ����ʾ
	virtual void		SetName(const char*p)		= 0;
	virtual const char* GetName()			const	= 0;

	//һ��ѭ����ÿuiMS����pQueue�е���һ��Pulse(uiMS)
	virtual void		PulseLoop(class IAsynProcQueue*pQueue,uint32 uiMS,bool &bExitFlag)	= 0;

	//������
	virtual long		DebugCmd(uint32 uiCmd,uint32 uiParam0 = 0,uint32 uiParam1 = 0)		= 0;
	enum
	{
		MIN_INTERVAL	= 10,				//timer��С���,10ms
		MAX_INTERVAL	= 20*24*60*60*1000,	//timer�����,20��

		DEBUG_CMD_CHECK_SAME_THREAD	= 0,
		DEBUG_CMD_TRACE				= 1,
		DEBUG_CMD_TRACE_LIST		= 2,	//���ܻ�Ƚ϶࣬������
		DEBUG_CMD_POOL_INFO			= 3,
	};
};

class CTimeLineLoader : public InterfaceLoader<ITimeLine,VER_TIMER_MAIN,VER_TIMER_SUB,VER_TIMER_BUILD>
{
	const char*	m_pProcName;
protected:
	virtual const char *GetDllFileName()	{	return "./TimeLine"DLL_EXT;	}
	virtual const char *GetProcName()		{	return m_pProcName;	}

	//����һ���Խ�ֹ�ⲿ����
	bool Create(char szErrorMsg[] = 0 ,int nLen = 0)
	{
		return InterfaceLoader<ITimeLine,VER_TIMER_MAIN,VER_TIMER_SUB,VER_TIMER_BUILD>::Create(szErrorMsg,nLen);
	}
public:
	bool GetSysTimeLine	(char szErrorMsg[] = 0 ,int nLen = 0)	{	m_pProcName = "GetSysTimeLine";	return Create(szErrorMsg,nLen);	}
	bool NewTimeLine	(char szErrorMsg[] = 0 ,int nLen = 0)	{	m_pProcName = "NewTimeLine";	return Create(szErrorMsg,nLen);	}
};

inline ITimeLine * GetSysTimeLine(
						#ifdef _DEBUG
								bool bCheckSameThread = true
						#endif
								)
{
	static CTimeLineLoader Loader;
	static bool  bFirst = true;
	if ( bFirst )
	{
		bFirst = false;
		Loader.GetSysTimeLine();
	}

#ifdef _DEBUG	
	if ( bCheckSameThread )
	{
		uint32 uiCurrThreadID = GetCurrentThreadId();
		uint32 uiOld = 0;
		assert(Loader->DebugCmd(ITimeLine::DEBUG_CMD_CHECK_SAME_THREAD,long(&uiOld)));
	}
#endif

	return Loader.GetInterface();
}

#ifdef _DEBUG
	#define GetSysTimeLineConst()	GetSysTimeLine(false)
#else
	#define GetSysTimeLineConst()	GetSysTimeLine()
#endif

#endif

//�ɵĽӿھͲ�Ҫ����
#ifdef OLD_TIMER_H__
class ITimeOutSink
{
public:
	enum	{	TIMER_OUT_SINK_VER	= 2	};
	virtual uint32	GetTimeOutSinkVersion()	const {	return TIMER_OUT_SINK_VER;	}

	virtual bool	OnTimer(uint32 uiTimerID,int nTick)	= 0;	//����false��ɾ��
};

class ITimeAxis : public IUnknownEx2
{
public:
	//ʱ������,���û��ulInterval�������Լ�����GetInterval()
	virtual void	Pulse(uint32 ulInterval)	= 0;
	virtual void	Pulse()	= 0;
	
	//ע��uiTimerID��pSinkһ�����Ψһ��
	virtual bool	SetTimer	(uint32 uiTimerID, ITimeOutSink *pSink, uint32 ulMillSec,		    bool bResetIfExist = true) = 0;
	virtual bool	SetTimerDesc(uint32 uiTimerID, ITimeOutSink *pSink, uint32 ulMillSec,int nTick, bool bResetIfExist = true) = 0;	//nTick����
	virtual bool	KillTimer	(uint32 uiTimerID, ITimeOutSink *pSink)	= 0;
	virtual bool	KillAllTimerForSink(ITimeOutSink *pSink)				= 0;	//δʵ��

	//����2��Ҫ����timer�б��������ڷ�����ʹ��
	virtual int		GetTimerTick(uint32 uiTimerID, const ITimeOutSink *pSink) const = 0;	
	virtual bool	IsTimerSeted(uint32 uiTimerID, const ITimeOutSink *pSink) const = 0;	

	//ȡ�������˶��
	virtual	uint32	GetTickCount()		const	= 0;
	virtual uint64	GetTickCount64()	const	= 0;

	//��������,����Traceʱ����ʾ
	virtual void		SetName(const char*p)	= 0;
	virtual const char* GetName()	const		= 0;

	//һ��ѭ����ÿuiMS����pQueue�е���һ��Pulse(uiMS)
	virtual void		PulseLoop(class IAsynProcQueue*pQueue,uint32 uiMS,bool &bExitFlag)	= 0;

	//������
	virtual long	DebugCmd(uint32 ulCmd,uint32 ulParam0 = 0,uint32 ulParam1 = 0)	= 0;
	enum
	{
		DEBUG_CMD_CHECK_SAME_THREAD	= 0,
		DEBUG_CMD_TRACE				= 1,
	};
};

	//ȡ�������ϴε��ô˺���֮���ʱ����(��λms),��һ����ȡ���������ʱ��֮��ļ��
	//�õ��Ľ�����ڸ�Pulse()
//	virtual uint32 GetInterval()	= 0;
//****************************************************************

//#ifdef _DEBUG
//	#define szITimerAxisDLL  "./TimerD"DLL_EXT		//��Ϊ��dll����һ��ȫ�ֶ��󣬻���ֻ��һ��dll��
//#else
	#define szITimerAxisDLL  "./Timer"DLL_EXT
//#endif

class CITimeAxisLoader
{
	typedef IUnknownEx2* (*func_GetSysITimeAxis)(uint32 ulMainVer,uint32 ulSubVer,uint32 ulBuildVer);
	ITimeAxis * m_pTimeAxis;
	HMODULE		m_hDll;

	bool Create(const char*pFunctionName)
	{
		if ( m_pTimeAxis )
			return true;

		if ( 0 == pFunctionName || 0 == pFunctionName[0] )
			return false;

		m_hDll = LoadLibrary(szITimerAxisDLL);
		if ( 0 == m_hDll )
			return false;

		func_GetSysITimeAxis func = (func_GetSysITimeAxis)GetProcAddress(m_hDll,pFunctionName);
		if ( func )
		{
			IUnknownEx2*p = func(VER_TIMER_MAIN,VER_TIMER_SUB,VER_TIMER_BUILD);
			if ( p )
			{
				m_pTimeAxis = (ITimeAxis*)p->QueryInterface(IF_UID_ITIMER,IF_VER_ITIMER);
				if ( !m_pTimeAxis )
					p->Release();
			}
		}

		if ( !func || ! m_pTimeAxis )
		{
			FreeLibrary(m_hDll);
			m_hDll = 0;
			return false;
		}
		return true;
	}

public:
	CITimeAxisLoader()	{	m_pTimeAxis = 0;	m_hDll = 0;	}
	~CITimeAxisLoader()	{	Close();						}
	
	bool GetSysITimeAxis()	{	return Create("GetSysITimeAxis");	}
	bool NewITimeAxis()		{	return Create("NewITimeAxis");		}

	void Close()
	{
		if ( m_pTimeAxis )
		{
			m_pTimeAxis->Release();
			m_pTimeAxis = 0;
		}
		if ( m_hDll )
		{
			FreeLibrary(m_hDll);
			m_hDll = 0;
		}
	}
	
	const	ITimeAxis* GetITimeAxis()	const	{	return m_pTimeAxis;		}
			ITimeAxis* GetITimeAxis()			{	return m_pTimeAxis;		}
	const	ITimeAxis* GetInterface()	const	{	return m_pTimeAxis;		}
			ITimeAxis* GetInterface()			{	return m_pTimeAxis;		}
	const	ITimeAxis* operator ->()	const	{	return m_pTimeAxis;		}
			ITimeAxis* operator ->()			{	return m_pTimeAxis;		}
	bool	IsValid()					const	{	return 0 != m_pTimeAxis;}
};
//****************************************************************

inline ITimeAxis * GetITimeAxis_Old(
						#ifdef _DEBUG
								bool bCheckSameThread = true
						#endif
								)
{
	static CITimeAxisLoader Loader;
	static bool  bFirst = true;
	if ( bFirst )
	{
		bFirst = false;
		Loader.GetSysITimeAxis();
	}

#ifdef _DEBUG	
	if ( bCheckSameThread )
	{
		uint32 ulCurrThreadID = GetCurrentThreadId();
		uint32 ulOld = 0;
		assert(Loader->DebugCmd(ITimeAxis::DEBUG_CMD_CHECK_SAME_THREAD,long(&ulOld)));
	}
#endif

	return Loader.GetITimeAxis();
}
#ifdef _DEBUG
	#define GetITimeAxisConst_Old()	GetITimeAxis_Old(false)
#else
	#define GetITimeAxisConst_Old()	GetITimeAxis_Old()
#endif
//****************************************************************

inline uint32 MyGetTickCount_Old()
{
	static uint32 ul;
	ITimeAxis * pAxis = GetITimeAxisConst_Old();
	if ( !pAxis )
		return ul;
	return pAxis->GetTickCount();
}

inline uint64 MyGetTickCount64_Old()
{
	static ITimeAxis * pAxis = NULL;
    
    if ( pAxis ) return pAxis->GetTickCount64();

    pAxis = GetITimeAxisConst_Old();
	if ( !pAxis )
		return 0;

	return pAxis->GetTickCount64();
}

//****************************************************************
#endif

#endif
