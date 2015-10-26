#ifndef LOG_2_EXIST_WIN_H_
#define LOG_2_EXIST_WIN_H_
#include "pvoper.h"
#include <assert.h>
#ifdef __AFX_H__

class CLog2ExistWin : public ILogTargetPlugin
	{
	protected:
		bool	m_bDiscardLog;
		bool	m_bStopShow;
		bool	m_bAppendHead;	//新信息加在头还是尾
		CString m_str;
		CWnd*	m_pWnd;
	public:
		CLog2ExistWin()	
			{	
			m_pWnd = NULL;	
			m_bDiscardLog	= false;
			m_bStopShow		= false;
			m_bAppendHead	= true;
			}

		void SetWnd(CWnd*p)
			{
			assert(p);
			m_pWnd = p;
			}

		void SetAppendHead(bool b)	
			{
			m_bAppendHead = b;
			}

		void ClearTrace()
			{
			m_str = "";
			SetWndText();
			}
		void DiscardLog(bool bDiscard)	{	m_bDiscardLog = bDiscard;	}
		void StopShow(bool bStop)		
			{	
			m_bStopShow = bStop;
			if ( m_bStopShow && m_pWnd )
				SetWndText();
			}
		bool IsDiscard() const {	return m_bDiscardLog;	}
		bool IsStop()	 const {	return m_bStopShow;		}

		//Log处理
		virtual void OnLog  (const char *pModName,const char *pMsg,const char *pTime )
			{
			ASSERT(pMsg);
			if ( m_bDiscardLog )
				return;

			if ( pMsg )
				MyLog(pModName,CString(pMsg)	  ,pTime );
			}
		virtual void OnLogLn(const char *pModName,const char *pMsg,const char *pTime )
			{
			ASSERT(pMsg);
			if ( m_bDiscardLog )
				return;

			if ( pMsg )
				MyLog(pModName,CString(pMsg)+ "\n",pTime );
			}
	protected:
		void MyLog(const char *pModName,const CString &sMsg,const char *pTime )
			{
			ASSERT( m_pWnd );
			if ( !m_pWnd || !IsWindow(m_pWnd->m_hWnd) )	return;

			CString strTime;
			if ( !pTime || pTime[0] == 0 )
				;
			else
				strTime = CString("[") + CString(pTime) + CString("]");

			CString strMod;
			if ( ! pModName || pModName[0] == 0)
				;
			else
				strMod = CString("[") + pModName + "]";

			const int MAX_LEN = 400*100;
			if ( m_bAppendHead )
				{
				m_str = strTime + strMod + " " + sMsg + m_str;
				if  (m_str.GetLength() > MAX_LEN )
					m_str = m_str.Left(MAX_LEN - 20*100);
				}
			else
				{
				m_str = m_str + strTime + strMod + " " + sMsg;
				if  (m_str.GetLength() > MAX_LEN )
					m_str = m_str.Right(MAX_LEN - 20*100 );
				}
			if ( !m_bStopShow )
				SetWndText();
			}
		virtual void SetWndText()
			{
			if ( m_pWnd )
				m_pWnd->SetWindowText(m_str);
			}
	};

#endif
//********************************************************************************
#ifdef __AFXCMN_H__

/*
以上的CLog2ExistWin在多线程时有死锁的隐患，过程是：
1.界面线程正要调用Trace()
2.逻辑(或其它)线程取得执行权，并执行Trace(),于是它获得了ILogger里的锁
3.逻辑线程Trace过程中，要SendMessageText(WM_SETTEXT)给窗口，从而界面线程获得执行权
4.界面线程必须执行完当前的代码才能处理WM_SETTEXT，但当前代码正要Trace，却无法获取ILogger的锁,于是死锁

下面的2个类CLogRichEdit_NoDielock和CLog2ExistWin_NoDielock合作处理。它要Trace时只是把Trace的内容
记录在一个CString里，并设置一个Timer来定时把CString的内容更新到界面上。
*/
class CLogRichEdit_NoDielock : public CRichEditCtrl
	{
		bool	m_bFirstCall;
		CString m_sPendingStr;
		CLock	m_Lock;
	public:
		enum	{	MAGIC_NNM = 1234509876,TIMER_ID = 12345 	};
		ulong m_ulMagicNum;
		CLogRichEdit_NoDielock()
			{
			m_ulMagicNum = MAGIC_NNM;
			m_bFirstCall = true;
			}

		void SetWndText(const CString str)
			{
			CLockBlock lock(&m_Lock);
			m_sPendingStr = str; 
			}

		void SetUpdateInterval(ulong ulMs)
			{
			assert( ulMs > 0 );
			if  ( ulMs > 0 )
				{
				KillTimer(TIMER_ID);
				SetTimer(TIMER_ID,ulMs,NULL);
				}
			}

		virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
			{
			if ( m_bFirstCall )
				{
				m_bFirstCall = false;
				LRESULT ret = CRichEditCtrl::WindowProc(message, wParam, lParam);
				SetTimer(TIMER_ID,500,NULL);
				return ret;
				}

			if ( message == WM_TIMER && wParam == TIMER_ID )
				{
			//	if ( m_sPendingStr != "" )
					{
					CLockBlock lock(&m_Lock);
				//	if ( m_sPendingStr != "" )
					if ( m_sPendingStr.GetLength() > 0 )
						{
						SetWindowText(m_sPendingStr);
						m_sPendingStr = "";
						}
					}
				}

			return CRichEditCtrl::WindowProc(message, wParam, lParam);
			}
	};

class CLog2ExistWin_NoDielock : public CLog2ExistWin
	{
	public:
		void SetWnd(CWnd*p)
			{
			CLogRichEdit_NoDielock* p1 = (CLogRichEdit_NoDielock*)p;
			assert(p && p1->m_ulMagicNum == CLogRichEdit_NoDielock::MAGIC_NNM);
			m_pWnd = p;
			}
	protected:
		virtual void SetWndText()
			{
			if ( NULL == m_pWnd )
				return;
			((CLogRichEdit_NoDielock*)m_pWnd)->SetWndText(m_str);
			}
	};
//********************************************************************************

class CLogRichEdit : public CRichEditCtrl
	{
	int	m_nHeight;
	int m_nLineHeight;

	int		m_nSetTextTimes;
	DWORD	m_dwLastScrollTime;

	enum 
		{
		WM_SCORE_ME	=	WM_USER + 1000	,
		MIN_SCROLL_TIME	= 2000
		};

	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
		{
		if ( message == WM_SETTEXT )
			{
			LRESULT lRet = CRichEditCtrl::WindowProc(message, wParam, lParam);

			m_nSetTextTimes++;
			if ( m_nSetTextTimes > m_nHeight/2 )	//加了半个窗口的行数
				{
				if ( GetTickCount() - m_dwLastScrollTime > MIN_SCROLL_TIME )
					{
					m_nSetTextTimes    = -2000000000;
					PostMessage(WM_SCORE_ME,0,0);
					}
				}
		
			return lRet;
			}
		else if ( message == WM_SCORE_ME)
			{
			ScoreIt(true);
			return 0;
			}
		switch (message)
			{
			case WM_SIZE:
				m_nHeight = HIWORD(lParam)/m_nLineHeight;
				if ( m_nHeight <= 0 )
					m_nHeight = 1;
			default:
				return CRichEditCtrl::WindowProc(message, wParam, lParam);
			}
		}

	void ScoreIt(bool bHalf)
		{
		m_nSetTextTimes    = 0;
		m_dwLastScrollTime = GetTickCount();
		int nFirst = GetFirstVisibleLine();
		int nCount = GetLineCount();
		if ( nFirst + m_nHeight < nCount )
			{
			LineScroll(nCount-nFirst-m_nHeight-1);	//+  bHalf ? m_nHeight/2 : 0);
			if ( bHalf )
				LineScroll(m_nHeight/2);
			}
		}

public:
	CLogRichEdit()	
		{	
		m_nHeight		= 10;		//随便
		m_nLineHeight	= 16;	

		m_nSetTextTimes		= 0;
		m_dwLastScrollTime	= GetTickCount() - MIN_SCROLL_TIME;
		}	

	void SetLightHeight(int nH)
		{
		ASSERT(nH >0 );
		m_nLineHeight  = nH;
		}
	};

#endif

#endif

