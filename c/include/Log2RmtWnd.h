#ifndef _LOG_2_RMT_WND_H_
#define _LOG_2_RMT_WND_H_

#include "ILogger.h"

struct _LOG_WND_REMOTE_DATA
{
	BOOL bNewLine;
	char szDataTime[24];
	char szMod[MAX_LOG_MOD_NAME_LEN];
    char szMsg[MAX_LOG_MSG_LEN];
};

#define _LOG_RMT_WND_MAGIC_NO  7777

const char *g_Log2RmtWndWndName = "dl_trace_2_win_recv_window_";

class CLog2RmtWnd : public ILogTargetPlusIn
{
protected:
	HWND m_hLocalWnd;
    HWND m_hRemoteWnd;
    char m_szModOnly[MAX_LOG_MOD_NAME_LEN];
protected:
	bool DoLog2RmtWnd(const char *pMsg,const char*pMod= NULL,const char *pTime = NULL,BOOL bNewLine = false)   
    {   
		if  ( !IsWindow(m_hRemoteWnd) )	
			m_hRemoteWnd = ::FindWindow(NULL,g_Log2RmtWndWndName);   

		if ( NULL == m_hRemoteWnd )   
			return false;  
		
		if  ( NULL == pMod )
			pMod = "";

		if  ( NULL == pTime)
			pTime = "";

		_LOG_WND_REMOTE_DATA forsend;   
		forsend.bNewLine	= bNewLine;
        lstrcpyn(forsend.szMod,		pMod,	sizeof(forsend.szMod));   
        lstrcpyn(forsend.szMsg,		pMsg,	sizeof(forsend.szMsg));   
		lstrcpyn(forsend.szDataTime,pTime,	sizeof(forsend.szDataTime));   

        COPYDATASTRUCT d;   
        d.dwData = _LOG_RMT_WND_MAGIC_NO;   
        d.cbData = sizeof(forsend);   
        d.lpData = &forsend;   

		return FALSE != ::SendMessage(m_hRemoteWnd,WM_COPYDATA,LONG(m_hLocalWnd),LONG(&d));   
	}
        
	virtual void OnLog  (const char *pModName,const char *pMsg,const char *pTime = NULL)
	{
		if ( m_szModOnly[0] && strcmp(m_szModOnly,pModName) )
			return;
		DoLog2RmtWnd(pMsg,pModName,pTime);
	}
	virtual void OnLogLn(const char *pModName,const char *pMsg,const char *pTime = NULL)
	{
		if ( m_szModOnly[0] && strcmp(m_szModOnly,pModName) )
			return;
		DoLog2RmtWnd(pMsg,pModName,pTime,true);
	}

public:
	CLog2RmtWnd()       
	{       
		m_hLocalWnd  = NULL;	
		m_hRemoteWnd = NULL;       
		memset(m_szModOnly,0,sizeof(m_szModOnly));
	}       

	void SetLocalWnd(HWND hLocalWnd)	//这个不调用也行
	{       
		m_hLocalWnd = hLocalWnd;	
	}       

	void SetModName(const char *pMod)	//只Log这个模块
	{	
		if ( pMod )
			lstrcpyn(m_szModOnly,pMod,sizeof(m_szModOnly));	
		else
			m_szModOnly[0] = 0;
	}       

	void TraceIt(LPCSTR lpszFormat, ...)       
	{       
		va_list args;	
		va_start(args, lpszFormat);	

		char szBuffer[1024];	
		_vsnprintf(szBuffer, 1024, lpszFormat, args);	

		va_end(args);	

		DoLog2RmtWnd(szBuffer);	
	}       
};

struct CAutoLog2RmtWnd
{
	CLog2RmtWnd	m_Plusin;

	CAutoLog2RmtWnd(const char *pModName = NULL)
	{
		if ( pModName && pModName[0] )
			m_Plusin.SetModName(pModName);
		g_Loger.GetSysTarget()->AddPlusIn(&m_Plusin);
	}
	~CAutoLog2RmtWnd()
	{
		g_Loger.GetSysTarget()->DelPlusIn(&m_Plusin);
	}
};

#ifndef _DEBUG
	#define DBG_TRACE_ON_RMT_WND(szModOnly)
#else
	#define DBG_TRACE_ON_RMT_WND(szModOnly)	CAutoLog2RmtWnd	_DbgTraceOnRmtWnd(szModOnly)
#endif

#endif
