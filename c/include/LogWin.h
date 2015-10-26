#ifndef _LOG_WIN_H_
#define _LOG_WIN_H_
/*
2005-04-21	蓝琚成	建立文件,基本完成,未测试
2005-04-22	蓝琚成	增加parent参数(未实现),增加LogLn(),ShowWin()
2006-07		蓝琚成	限制内容大小,Log太多时把前面的截掉
					改为最后的Log在最上面
					初始化位置在左上角

建立一个log窗口,上有一个RichEdit,用于记录消息
*/

#include "ILogger.h"
#include <Windows.h>

class ILogWin
{
public:	
	virtual void Release() = 0;
	virtual void Log  (const char *pMod,const char *pMsg,const char *pTime) = 0;
	virtual void LogLn(const char *pMod,const char *pMsg,const char *pTime) = 0;
	virtual void ShowWin(bool b)		= 0;
};

class CLogWinPlusIn : public ILogTargetPlusIn
	{
	protected:		
		HMODULE    m_hMod;
		ILogWin *  m_pLogWin;
		typedef ILogWin *(*_CreateLogWinObject)(HWND hParent,DWORD dwMianVer ,DWORD dwMin ,DWORD dwBuildVer);

		bool	m_bShow;
	public:
		bool Create(HWND hParent = NULL,bool bShowWnd = true)
		{
			Clean();
			m_hMod = LoadLibrary(".\\LogWin.dll");
			if (!m_hMod )
				return false;
			_CreateLogWinObject clwo = (_CreateLogWinObject )GetProcAddress(m_hMod,"CreateLogWinObject");
			if ( !clwo )
				{
				Clean();
				return false;
				}
			m_pLogWin = clwo(hParent,1,0,0);
			if ( !m_pLogWin )
				{
				Clean();
				return false;
				}
			else
				{
				if ( bShowWnd )
					{
					m_pLogWin->ShowWin(true);
					m_bShow   = true;
					}
				}
			return true;
		}
		bool IsValid()	const {	return m_pLogWin != NULL;	}
		void Clean()
			{
			if ( m_pLogWin )
				{
				m_pLogWin->Release();
				m_pLogWin = NULL;
				}
			if ( m_hMod )
				{
				FreeLibrary(m_hMod);
				m_hMod = NULL;
				}
			m_bShow   = false;
			}
		void ShowWin(bool b)
			{
			if ( m_pLogWin )
				{
				m_pLogWin->ShowWin(b);
				m_bShow   = b;
				}
			}
		bool  IsWndShow() const {	return m_bShow;	}
		CLogWinPlusIn()
			{
			m_hMod = NULL;
			m_pLogWin = NULL;
			m_bShow   = false;
			}
		~CLogWinPlusIn()
			{
			Clean();			
			}
		virtual void OnLog  (const char *pModName,const char *pMsg,const char *pTime = NULL)
			{
			if ( m_pLogWin )	m_pLogWin->Log(pModName,pMsg,pTime);
			}
		virtual void OnLogLn(const char *pModName,const char *pMsg,const char *pTime = NULL)
			{
			if ( m_pLogWin )	m_pLogWin->LogLn(pModName,pMsg,pTime);
			}
	};

#endif
