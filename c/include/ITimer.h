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

static const CLASSUID	IF_UID_ITIMER  ("ITimer接口");
static const uint32		IF_VER_ITIMER  = INTERFACE_VER(VER_TIMER_MAIN,VER_TIMER_SUB);
//****************************************************************
/*
    时间轴,管理定时器的接口.在一个程序的主程序中设一定时器,或建个线程,定时
调用Pulse(),接口将检查是否有定时器超时了.
    注意:
    1.此接口假定所有代码运行在同一线程中(即不是多线程安全的)
    2.Timer间隔必须 >= 10 ms 且 <= 20天

修改历史:
日期		修改人	版本	修改	

2007-12-09	蓝琚成	4.0.0	支持IUnknownEx2查询接口
							增加SetTimerDesc()
							增加GetTimeOutSinkVersion()
							增加计算OnTimer次数的功能
2007-12-20	蓝琚成	5.0.0	增加GetInterval()
							增加有ulInterval参数的Pulse()
2008-02-15	蓝琚成	5.0.1	SetTimer的参数bResetCountIfExist改为bResetIfExist,即所有信息都重置
2008-03-11	蓝琚成	5.1.0	增加SetName()/GetName()
2010-08-16	        5.2.0	把对CULong64的依赖变为unsigned long long 
2014-10-30	        5.3.0	所有ulong改为uint32
							去掉GetInterval()
							增加PulseLoop()
2015-12             6.0.0   完全重写了，现在回调时间都精确到10ms之内了，速度也提高了很多
							OnTimer()不再返回值
							去掉一些没什么用的接口



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
	//时间脉冲
	virtual void		Pulse(uint32 uiInterval)	= 0;
	
	//注意uiTimerID与pSink一起才是唯一的
	//uiStartMS是多久后第一次调用，如果=0则uiInvMS后
	//uiInvMS和uiStartMS必须>=10ms
	virtual bool		SetTimer		(uint uiTimerID, ITimeOutSink *pSink, uint32 uiInvMS,uint32 uiStartMS = 0,const char*pDesc = 0)	= 0;
	virtual bool		KillTimer		(uint uiTimerID, ITimeOutSink *pSink)				= 0;
	virtual bool		IsTimerSeted	(uint uiTimerID, const	ITimeOutSink *pSink) const	= 0;
	virtual uint		GetCalledCount	(uint uiTimerID, const	ITimeOutSink *pSink) const	= 0;	//注意时间太久这个值会回绕,timer不存在则返回0

	//取共运行了多久
	virtual uint32		GetTickCount()		const 	= 0;
	virtual uint64		GetTickCount64()	const 	= 0;

	//设置名字,在有Trace时将显示
	virtual void		SetName(const char*p)		= 0;
	virtual const char* GetName()			const	= 0;

	//一个循环，每uiMS就在pQueue中调用一次Pulse(uiMS)
	virtual void		PulseLoop(class IAsynProcQueue*pQueue,uint32 uiMS,bool &bExitFlag)	= 0;

	//调试用
	virtual long		DebugCmd(uint32 uiCmd,uint32 uiParam0 = 0,uint32 uiParam1 = 0)		= 0;
	enum
	{
		MIN_INTERVAL	= 10,				//timer最小间隔,10ms
		MAX_INTERVAL	= 20*24*60*60*1000,	//timer最大间隔,20天

		DEBUG_CMD_CHECK_SAME_THREAD	= 0,
		DEBUG_CMD_TRACE				= 1,
		DEBUG_CMD_TRACE_LIST		= 2,	//可能会比较多，别乱用
		DEBUG_CMD_POOL_INFO			= 3,
	};
};

class CTimeLineLoader : public InterfaceLoader<ITimeLine,VER_TIMER_MAIN,VER_TIMER_SUB,VER_TIMER_BUILD>
{
	const char*	m_pProcName;
protected:
	virtual const char *GetDllFileName()	{	return "./TimeLine"DLL_EXT;	}
	virtual const char *GetProcName()		{	return m_pProcName;	}

	//重载一下以禁止外部调用
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

//旧的接口就不要用啦
#ifdef OLD_TIMER_H__
class ITimeOutSink
{
public:
	enum	{	TIMER_OUT_SINK_VER	= 2	};
	virtual uint32	GetTimeOutSinkVersion()	const {	return TIMER_OUT_SINK_VER;	}

	virtual bool	OnTimer(uint32 uiTimerID,int nTick)	= 0;	//返回false则删除
};

class ITimeAxis : public IUnknownEx2
{
public:
	//时间脉冲,如果没有ulInterval参数则自己调用GetInterval()
	virtual void	Pulse(uint32 ulInterval)	= 0;
	virtual void	Pulse()	= 0;
	
	//注意uiTimerID与pSink一起才是唯一的
	virtual bool	SetTimer	(uint32 uiTimerID, ITimeOutSink *pSink, uint32 ulMillSec,		    bool bResetIfExist = true) = 0;
	virtual bool	SetTimerDesc(uint32 uiTimerID, ITimeOutSink *pSink, uint32 ulMillSec,int nTick, bool bResetIfExist = true) = 0;	//nTick降序
	virtual bool	KillTimer	(uint32 uiTimerID, ITimeOutSink *pSink)	= 0;
	virtual bool	KillAllTimerForSink(ITimeOutSink *pSink)				= 0;	//未实现

	//下面2个要遍历timer列表，不建议在服务器使用
	virtual int		GetTimerTick(uint32 uiTimerID, const ITimeOutSink *pSink) const = 0;	
	virtual bool	IsTimerSeted(uint32 uiTimerID, const ITimeOutSink *pSink) const = 0;	

	//取共运行了多久
	virtual	uint32	GetTickCount()		const	= 0;
	virtual uint64	GetTickCount64()	const	= 0;

	//设置名字,在有Trace时将显示
	virtual void		SetName(const char*p)	= 0;
	virtual const char* GetName()	const		= 0;

	//一个循环，每uiMS就在pQueue中调用一次Pulse(uiMS)
	virtual void		PulseLoop(class IAsynProcQueue*pQueue,uint32 uiMS,bool &bExitFlag)	= 0;

	//调试用
	virtual long	DebugCmd(uint32 ulCmd,uint32 ulParam0 = 0,uint32 ulParam1 = 0)	= 0;
	enum
	{
		DEBUG_CMD_CHECK_SAME_THREAD	= 0,
		DEBUG_CMD_TRACE				= 1,
	};
};

	//取本次与上次调用此函数之间的时间间隔(单位ms),第一次则取得与对象建立时间之间的间隔
	//得到的结果用于给Pulse()
//	virtual uint32 GetInterval()	= 0;
//****************************************************************

//#ifdef _DEBUG
//	#define szITimerAxisDLL  "./TimerD"DLL_EXT		//因为此dll引出一个全局对象，还是只有一个dll好
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
