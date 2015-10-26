#ifndef _SPEND_TIME_CHECKER_H_
#define _SPEND_TIME_CHECKER_H_
#include <stdio.h>
#include <stdlib.h>
#include "portable_hash_map.h"
#include <algorithm>
#ifdef _WIN32
	#include <Mmsystem.h>
	#pragma comment(lib,"Winmm.lib")
#else
	#include <sys/time.h>
#endif

#ifndef DEFAULT_WARNING_INV
	#ifdef _DEBUG
		#define DEFAULT_WARNING_INV	5
	#else
		#define DEFAULT_WARNING_INV	50
	#endif
#endif

#ifndef ENABLE_CHECK_SPEND_TIME
	#ifdef _WIN32
		#define ENABLE_CHECK_SPEND_TIME	1
	#else
		#define ENABLE_CHECK_SPEND_TIME	0
	#endif
#endif
//************************************************************************************

class CSpendTimeRecorder
{
	struct StSpendTime
	{
		uint32 ulSpend;
		uint32 ulCallCount;
	};
protected:
	typedef	hash_map<const char*,StSpendTime>	RECORD_MAP;	//hash_map的查找速度是map的5倍
	RECORD_MAP	m_map;
public:	
	void AddSpendTime(const char*pFuncName,uint32 ulSpend)
	{
		RECORD_MAP::iterator it = m_map.find(pFuncName);
		if ( it == m_map.end() )
		{
			StSpendTime st;
			st.ulSpend		= ulSpend;
			st.ulCallCount	= 1;
			m_map.insert(RECORD_MAP::value_type(pFuncName,st));
		}
		else
		{
			(*it).second.ulSpend	 += ulSpend;
			(*it).second.ulCallCount ++;
		}
	}

	virtual void TraceRecord() const
	{
#ifndef SORT_TRACE_SPEND_TIME
		RECORD_MAP::const_iterator _F = m_map.begin();
		RECORD_MAP::const_iterator _L = m_map.end();
		//		  000000001 00000001 00000001 : CUserMagr::OnDBRet_LoginResult()
	//	RelTrace("   总耗时 调用次数 平均耗时   函数");
		RelTrace(" Time.tot  callCnt Time.Avg   func");
		RelTrace("--------------------------------------------------------------");
		for ( ; _F != _L; ++_F )
		{
			const char*pName =  (*_F).first;
			if ( pName == NULL )
				pName = "NULL";
			RelTrace("%9.u %8.u %8.u : %s",
							(*_F).second.ulSpend,
							(*_F).second.ulCallCount,
							(*_F).second.ulCallCount == 0 ? 0 : ((*_F).second.ulSpend/(*_F).second.ulCallCount),
							pName);
		}
#else
		struct StSortItem
		{
			const char*pStr;
			const StSpendTime*pItem;
			bool operator < (const StSortItem &a)	const	{	return strcmp(this->pStr,a.pStr) < 0;	}
		};
		std::vector<StSortItem>	tmp;
		{
			RECORD_MAP::const_iterator _F = m_map.begin();
			RECORD_MAP::const_iterator _L = m_map.end();
			for ( ; _F != _L; ++_F )
			{
				StSortItem item = {	_F->first,&_F->second	};
				tmp.push_back(item);
			}
			std::sort(tmp.begin(),tmp.end());
		}
		std::vector<StSortItem>::const_iterator f = tmp.begin();
		std::vector<StSortItem>::const_iterator l = tmp.end();
		//		  000000001 00000001 00000001 : CUserMagr::OnDBRet_LoginResult()
	//	RelTrace("   总耗时 调用次数 平均耗时   函数");
		RelTrace(" Time.tot  callCnt Time.Avg   func");
		RelTrace("--------------------------------------------------------------");
		for ( ; f != l; ++f)
		{
			const char*pName =  (*f).pStr;
			if ( pName == NULL )
				pName = "NULL";
			RelTrace("%9.u %8.u %8.u : %s",
							(*f).pItem->ulSpend,
							(*f).pItem->ulCallCount,
							(*f).pItem->ulCallCount == 0 ? 0 : ((*f).pItem->ulSpend/(*f).pItem->ulCallCount),
							pName);
		}
#endif
	}
};
//************************************************************************************
template <class Printer>
class tmplSpendTimeCheckerBase
{
#if ENABLE_CHECK_SPEND_TIME
	const char *m_pFuncName;
	uint32 m_ulWarningTime;
#ifdef _WIN32
	uint32 m_ulBegin;
#else
	struct timeval m_tv_begin;
#endif
#endif


public:
	tmplSpendTimeCheckerBase(const char*pFuncName,uint32 ulWarningTime = DEFAULT_WARNING_INV)
	{
#if ENABLE_CHECK_SPEND_TIME
		m_pFuncName		= pFuncName;
		m_ulWarningTime = ulWarningTime;
	#ifdef _WIN32
		m_ulBegin = timeGetTime();	//之前记得timeBeginPeriod(1)
	#else
		gettimeofday(&m_tv_begin,NULL);
	#endif
#endif
	}

	void CheckSpendTime()
	{
#if ENABLE_CHECK_SPEND_TIME
		uint32 ulSpend;
	#ifdef _WIN32
		ulSpend =  timeGetTime() - m_ulBegin;	
	#else
		struct timeval tv_end;
		gettimeofday(&tv_end,NULL);
		long s = tv_end.tv_sec - m_tv_begin.tv_sec;
		if ( s < 0 )
		{
			RelTrace("CheckSpendTime() : modify system time？");
			ulSpend = 0;
		}
		else
		{
			long us = 1000000 * s +  tv_end.tv_usec - m_tv_begin.tv_usec;
			ulSpend = us/1000;
		}
	#endif
		if ( ulSpend >= m_ulWarningTime ) 
			m_Printer(m_pFuncName,ulSpend);
	
	#ifdef SPEND_TIME_RECORDER
		SPEND_TIME_RECORDER.AddSpendTime(m_pFuncName,ulSpend);
	#endif
#endif
	}
	Printer	m_Printer;
};

template <class Printer>
class tmplSpendTimeChecker : public tmplSpendTimeCheckerBase<Printer>
{
public:
	tmplSpendTimeChecker(const char*pFuncName,uint32 ulWarningTime = DEFAULT_WARNING_INV)
		: tmplSpendTimeCheckerBase<Printer>(pFuncName,ulWarningTime) {}

	~tmplSpendTimeChecker()
	{
		this->CheckSpendTime();
	}
};
//************************************************************************************
struct _PrintSpendTime
{
	void operator() (const char *pFuncName,uint32 ulMs)
	{
	//	RelTrace("%s耗时%u ms",pFuncName,ulMs);
		RelTrace("%s spended %u ms",pFuncName,ulMs);
	}
};

struct _ConsolePrintSpendTime
{
	void operator() (const char *pFuncName,uint32 ulMs)
	{
	//	printf("%s耗时%u ms\n",pFuncName,ulMs);
		printf("%s spended %u ms\n",pFuncName,ulMs);
	}
};

template <class CmdType = unsigned char>
struct _PrintCmdSpendTime
{
	_PrintCmdSpendTime()	{	m_Cmd = -1;	}
	CmdType m_Cmd;
	void operator() (const char *pFuncName,uint32 ulMs)
	{
	//	RelTrace("%s耗时%u ms,命令=%u",pFuncName,ulMs,uint32(m_Cmd));
		RelTrace("%s spended %u ms,cmd=%u",pFuncName,ulMs,uint32(m_Cmd));
	}
};

struct _PrintStrSpendTime
{
	_PrintStrSpendTime()	{	m_pStr = NULL;	}
	const char*m_pStr;
	void operator() (const char *pFuncName,uint32 ulMs)
	{
		RelTrace("%s spended %u ms,parameter=%s",pFuncName,ulMs,m_pStr);
	}
};

//************************************************************************************
typedef tmplSpendTimeChecker<_PrintSpendTime>						CheckSpendTime;
typedef tmplSpendTimeChecker<_PrintCmdSpendTime<unsigned char>  >	CheckCmdSpendTime;
typedef tmplSpendTimeChecker<_PrintCmdSpendTime<unsigned char>  >	CheckCmdSpendTimeC;
typedef tmplSpendTimeChecker<_PrintCmdSpendTime<unsigned short> >	CheckCmdSpendTimeS;
typedef tmplSpendTimeChecker<_PrintCmdSpendTime<unsigned int>   >	CheckCmdSpendTimeI;
typedef tmplSpendTimeChecker<_PrintStrSpendTime>					CheckStrSpendTime;

typedef tmplSpendTimeChecker<_ConsolePrintSpendTime>				ConsoleCheckSpendTime;
//************************************************************************************
/*	没啥鸟用
#ifdef _WIN32
#define __func__	__FUNCDNAME__
//#define __func__	__FUNCTION__
#endif

#define	CheckFuncSpendTime()	CheckSpendTime _checker(__func__)
*/
//************************************************************************************

#endif
