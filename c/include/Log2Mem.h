#ifndef _LOG_2_MEM_H_
#define _LOG_2_MEM_H_
/*
2008-03		蓝琚成	建立文件
*/
#include "ILogger.h"

template <int MAX_LOG_COUNT = 50,int LEN_PER_LOG = 120,bool bLogTime = 1>
struct CLog2MemPluginTmpl : public ILogTargetPlugin
{
	char	m_szLogs[MAX_LOG_COUNT][LEN_PER_LOG];
	int		m_nCurrent;
	char	m_szModName[64];	//只记录这个模块的Log,不设置则记录全部

	virtual void OnLog  (const char *pModName,const char *pMsg,const char *pTime )	{	DoLog(pModName,pMsg,pTime);	}
	virtual void OnLogLn(const char *pModName,const char *pMsg,const char *pTime )	{	DoLog(pModName,pMsg,pTime);	}

	void DoLog(const char *pModName,const char *pMsg,const char *pTime )
	{
		if ( m_szModName[0] && strcmp(m_szModName,pModName) )
			return;

		if ( bLogTime && pTime )
			_snprintf(m_szLogs[m_nCurrent],LEN_PER_LOG,"[%s][%s]%s",pTime,pModName,pMsg);
		else
			_snprintf(m_szLogs[m_nCurrent],LEN_PER_LOG,"[%s]%s",		  pModName,pMsg);
		m_szLogs[m_nCurrent][LEN_PER_LOG-1] = 0;	//需要吗？

		m_nCurrent ++ ;
		if ( m_nCurrent >= MAX_LOG_COUNT )
			m_nCurrent = 0;
	}

	CLog2MemPluginTmpl()
	{
		memset(m_szModName,	0,sizeof(m_szModName));
		memset(m_szLogs,	0,sizeof(m_szLogs));
		m_nCurrent = 0;
	}
};

template <int MAX_LOG_COUNT = 50,int LEN_PER_LOG = 120,bool bLogTime = 1>
struct CAutoLog2MemTmpl
{
	CLog2MemPluginTmpl<MAX_LOG_COUNT,LEN_PER_LOG,bLogTime>	m_Plugin;

	CAutoLog2MemTmpl(const char *pModName = NULL)
	{
		if ( pModName && pModName[0] )
			lstrcpyn(m_Plugin.m_szModName,pModName,sizeof(m_Plugin.m_szModName));
		g_Loger.GetSysTarget()->AddPlugin(&m_Plugin);
	}
	~CAutoLog2MemTmpl()
	{
		g_Loger.GetSysTarget()->DelPlugin(&m_Plugin);
	}
};

//一般放在线程主函数中
#define	TRACE_ON_STACK_PARAM(LogCount,LenPerLog,bLogTime,szModName)	CAutoLog2MemTmpl<LogCount,LenPerLog,bLogTime>		_RelTraceOnStackParam(szModName)
#define	TRACE_ON_STACK()											CAutoLog2MemTmpl<>									_RelTraceOnStack

#ifndef _DEBUG
	#define DBG_TRACE_ON_STACK_PARAM(LogCount,LenPerLog,bLogTime,szModName)
	#define DBG_TRACE_ON_STACK()
#else
	#define DBG_TRACE_ON_STACK_PARAM(LogCount,LenPerLog,bLogTime,szModName)	CAutoLog2MemTmpl<LogCount,LenPerLog,bLogTime>	_DbgTraceOnStackParam(szModName)
	#define DBG_TRACE_ON_STACK()											CAutoLog2MemTmpl<>								_DbgTraceOnStack
#endif

#endif
