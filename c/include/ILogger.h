#ifndef _ILOGGER_H_
#define _ILOGGER_H_
#include <stdio.h>
#include <stdlib.h>
#include "commInterfaceDef.h"
/*
一个灵活的,多线程安全的log类
注意:
    由于内部使用了sprintf系列函数,请注意Log的数据不要太长,并且要注
意不要使用客户发过来的参数作为第一个Log参数,否则用户定制的参数很容
易造成成功的攻击.
								--蓝琚成
*/
/*
2004  	   		蓝琚成    		建立文件
2004-10-7  		蓝琚成    		可以用,有待完善
2005-04-21 		蓝琚成    		实现plugin的功能
2005-04-22/23 	蓝琚成    		修正plugin功能的bug,并测试
						  		完善Loader类
2005-04-24 		蓝琚成    		实现时间的显示
						  		完善Loader类
2005-05-25		蓝琚成	  		增加RelAssert宏
2006-08-31		蓝琚成	  v1.02	修正Trace浮点数时R6002的错误
								砍掉本文件一些无用的东西
								Trace build版本warning检查sub版本
2006-12-05		蓝琚成	  v1.10	实现SetOutputFileName()
2007-07-27/30	蓝琚成	  v1.11	增加SetEnableQueue(),GetEnableQueue()接口，未实现.
								不升级主次版本以便跟外面的游戏兼容
								砍掉本文件一些无用的东西
2008-03-11		蓝琚成	  v1.12	增加DirectLog(),GetModName()接口
2008-12-01		蓝琚成	  v1.13	增加GetLogTarget(),GetWithTime()接口
2010-08-02		蓝琚成	  v1.14	增加ILogFilter接口
2013-01-08		蓝琚成	  v1.15 修正拼写错误

*/

#define MAX_LOG_MOD_NAME_LEN	64
#define MAX_LOG_MSG_LEN			1024
//************************************************************
class ILogTargetPlugin	//Log目标的插件接口
	{
	public:
		//一般说来只要实现这2个接口即可
		//我会确保pModName和pMsg不为NULL,但可能会为空串
		virtual void OnLog  (const char *pModName,const char *pMsg,const char *pTime = NULL)  = 0;	
		virtual void OnLogLn(const char *pModName,const char *pMsg,const char *pTime = NULL)  = 0;
		
		//你可能会想在这里delete this
		virtual void OnAttach()	{}	//调用AddPlugin()时回调
		virtual void OnDetach()	{}	//调用DelPlugin()时回调
	};

class ILogFilter	//Log过滤器
	{
	public:
		//返回false则否决
		virtual bool OnLog  (const char *pModName,const char *pMsg,const char *pTime)  = 0;	
		virtual bool OnLogLn(const char *pModName,const char *pMsg,const char *pTime)  = 0;
	};
//************************************************************
class ILogTarget	//ILogger的输出目标
	{
	public:
		virtual void Release()							= 0;
		//可以同时设置多个Log的目标,即可同时log到文件和RichEdit
	#ifdef _WIN32
		class CWebBrowser2;
		//hWnd为NULL则不Log到RichEdit
		virtual void SetRichEditHwnd(HWND *hWnd)		= 0;
		virtual void SetRichEditHwnd(CWebBrowser2 *pIE)	= 0;
		
		virtual void SetListViewHwnd(const char *pMod,HWND *hWnd) = 0;
		virtual void SetRemoteListViewHwnd(const char *pMod,const char *pWinCaption/*,UINT nCtlID*/) = 0;
	#else
		//如fdFile为-1则不log,0为标准输出?
		virtual void SetOutputFd(int fdFile /*= 0 */)		= 0;
	#endif
	
		virtual void SetOutputStream(FILE *)	= 0;	//Win 32下输出到stdout有问题,不懂为什么
		
		//pName为空则不输出到文件,返回文件是否能打开(不管能否打开，以后Log时都会重试)
		virtual bool SetOutputFileName(const char *pName,bool bCleanOld = false)	= 0;
		
		virtual void EnableStdout(bool b) 	= 0;
	
		//增加/删除Log输出插件
		virtual void AddPlugin(ILogTargetPlugin *p) 	= 0;
		virtual void DelPlugin(ILogTargetPlugin *p) 	= 0;

		//设置时间格式,未实现
		virtual void SetTimeFormat(ulong ulReserved0,ulong ulReserved1,ulong ulReserved2)	= 0;

		//是否把Log内容入队，另开一个线程来Log
		virtual void SetEnableQueue(bool bEnable)	= 0;
		virtual bool GetEnableQueue()	const		= 0;

		//增加/删除Log输出过滤器
		virtual void AddFilter(ILogFilter *p) 	= 0;
		virtual void DelFilter(ILogFilter *p) 	= 0;
	};

//************************************************************
class ILogger
	{
	public:
		virtual void Release()						= 0;
		//更改Log到哪里，刚建立时默认Log到系统目标
		virtual void SetLogTarget(ILogTarget *p) 	= 0;
		
		//在Log前加的名称,为空则不加模块名
		virtual void SetModName(const char *pMod) 	= 0;
		//如果b为True,则调用Log和LogLn相当于调用tLog和tLogLn
		virtual void SetWithTime(bool b = true)		= 0;
		//参数是rgb颜色
	//	virtual void SetLogColor(ulong ulColor)		= 0;
		//*****************Log函数*****************
		virtual void Log(const char *, ...)			= 0;
		virtual void LogLn(const char *, ...)		= 0;

		//强制Log时加时间
		virtual void tLog(const char *, ...)		= 0;
		virtual void tLogLn(const char *, ...)		= 0;

		//强制Log时不加时间
		virtual void ntLog(const char *, ...)		= 0;
		virtual void ntLogLn(const char *, ...)		= 0;

		//直接Log字符串,不用sprintf之类的分析
		virtual void DirectLog(const char*p,bool bLn)					= 0;
		virtual void DirectLog(const char*p,bool bLn,bool bWithTime)	= 0;

		//取模块名等信息
		virtual const char*GetModName()		const	= 0;
		virtual ILogTarget* GetLogTarget()	const	= 0;
		virtual bool GetWithTime()			const	= 0;
	};

//************************************************************************
#define  VER_LOGGER_MAIN	1		//主版本增加是不兼容升级
#define  VER_LOGGER_MIN		1		//次版本增加是兼容的升级(功能增加等)
#define  VER_LOGGER_BUILD	5		//build版本增加是功能不变的bug修正升级
//************************************************************************
#ifdef _WIN32
#define ILOGGER_DLL_NAME 	"./logger.dll"
#else
#define ILOGGER_DLL_NAME 	"./logger.so"
#endif

class CILoggerLoader
	{
	typedef void 		(*func_GetSysDefLogger)(ILogger **, ILogTarget **);
	typedef bool 		(*func_ILogger_CheckVersion)(int verMain,int verMin,int verBuild);
	typedef ILogTarget* (*func_CreateLogTarget)();
	typedef ILogger *   (*func_CreateLogger)();

	protected:
		HMODULE 			 m_hMod;
		
		ILogger*			 m_pSysLogger;	//一个公用的Logger对象,是logger.dll中的一个全局变量,其模块信息永远为"Sys",SetModName()和SetWithTime()和SetLogTarget()对其不起作用,不能Release()
		ILogTarget*			 m_pSysTarget;
		ILogger*			 m_pOwnLogger;
		ILogTarget*			 m_pOwnTarget;
		func_CreateLogTarget m_pCreateLogTarget;
		func_CreateLogger    m_pCreateLogger;
		
	public:
		CILoggerLoader()
			{
			m_hMod 				= NULL;
			m_pSysLogger 		= NULL;
			m_pSysTarget		= NULL;
			m_pOwnLogger		= NULL;
			m_pOwnTarget		= NULL;
			m_pCreateLogTarget 	= NULL;
			m_pCreateLogger	 	= NULL;
			}
		~CILoggerLoader()
			{
			if ( m_pOwnLogger )
				m_pOwnLogger->Release();
			if ( m_pOwnTarget )
				m_pOwnTarget->Release();
				
			if ( NULL != m_hMod )
				FreeLibrary(m_hMod);
			}
	
		bool LoadDll()
			{
			m_hMod = LoadLibrary(ILOGGER_DLL_NAME);
			if ( NULL == m_hMod )
				return false;

			func_ILogger_CheckVersion  pChkVer = (func_ILogger_CheckVersion)GetProcAddress(m_hMod,"ILogger_CheckVersion");
			func_GetSysDefLogger       pGetSys = (func_GetSysDefLogger)	 	GetProcAddress(m_hMod,"GetSysDefLogger");
			func_CreateLogTarget 	   pCTarget= (func_CreateLogTarget)	 	GetProcAddress(m_hMod,"CreateLogTarget");  
			func_CreateLogger    	   pCLogger= (func_CreateLogger)		GetProcAddress(m_hMod,"CreateLogger");

			if ( ! pChkVer || !pGetSys || !pCTarget || !pCLogger )
				return false;
				
			if ( !pChkVer(VER_LOGGER_MAIN,VER_LOGGER_MIN,VER_LOGGER_BUILD) )
				return false;
			
			pGetSys(&m_pSysLogger,&m_pSysTarget);

			m_pCreateLogTarget = pCTarget;
			m_pCreateLogger    = pCLogger;
			return true;
			}
		
		bool CreateOwnLogger()
			{
			if ( m_pOwnLogger )
				return true;
			if ( m_pCreateLogTarget )
				{
				m_pOwnLogger = m_pCreateLogger();
				return NULL != m_pOwnLogger;
				}
			else
				return false;
			}
			
		bool CreateOwnTarget()
			{
			if ( m_pOwnTarget )
				return true;
			if ( m_pCreateLogTarget )
				{
				m_pOwnTarget = m_pCreateLogTarget();
				return NULL != m_pOwnTarget;
				}
			else
				return false;
			}
		
		ILogger * 	 GetSysLogger()		{	return m_pSysLogger;	}
		ILogTarget * GetSysTarget()		{	return m_pSysTarget;	}
		ILogger * 	 GetOwnLogger()		{	return m_pOwnLogger;	}
		ILogTarget * GetOwnTarget()		{	return m_pOwnTarget;	}
		//有自建的就返回自建的,否则返回系统的
		ILogger * 	 GetDefLogger()		{	return m_pOwnLogger ? m_pOwnLogger : m_pSysLogger;	}
		ILogTarget * GetDefTarget()		{	return m_pOwnTarget ? m_pOwnTarget : m_pSysTarget;	}
		ILogger * 	 operator->()		{	return m_pOwnLogger ? m_pOwnLogger : m_pSysLogger;	}
		ILogger * 	 operator->() const	{	return m_pOwnLogger ? m_pOwnLogger : m_pSysLogger;	}
		
		bool IsValid()
			{
			return 	m_pSysLogger &&
					m_pSysTarget &&
					m_pCreateLogTarget &&
					m_pCreateLogger;
			}
	};
//************************************************************************
#ifdef _WIN32
#define ShowLoadLoggerErr(x)	MessageBox(NULL,x,"错误",0);
#else
#define ShowLoadLoggerErr(x)	puts(x);
#endif

extern CILoggerLoader  g_Loger;

#define INIT_GLOBAL_LOGGER(szModName,bShowTime)	\
CILoggerLoader  g_Loger;			\
struct _c_globe_logger_loader_		\
	{								\
	_c_globe_logger_loader_()		\
		{							\
		if ( g_Loger.LoadDll() )	\
			{						\
			g_Loger.CreateOwnLogger();			\
			const char *pModName = szModName;	\
			if ( pModName  )					\
				g_Loger->SetModName(pModName);	\
			g_Loger->SetWithTime(bShowTime);	\
			}						\
		else						\
			{						\
			ShowLoadLoggerErr("初始化全局Logger失败!请关闭程序并退出!");	\
			}						\
		}							\
	};								\
static _c_globe_logger_loader_	_static_logger_loader_;		//2个变量的初始化顺序好象没有保证.....

#define INIT_GLOBE_LOGGER(szModName,bShowTime)	INIT_GLOBAL_LOGGER(szModName,bShowTime)		//兼容旧代码

#ifndef DbgTrace
	#ifdef _DEBUG
		#define DbgTrace g_Loger->LogLn
	#else
		#define DbgTrace 
	#endif
#endif

#ifndef Trace
	#define Trace 	 g_Loger->LogLn
#endif	

#ifndef RelTrace
	#define RelTrace g_Loger->LogLn
#endif

#define _NOT_IMPLEMENTED_(x)  RelTrace("错误:试图运行未实现的代码[%s],此代码位于文件%s的第%d行",x,__FILE__,__LINE__)

#endif
