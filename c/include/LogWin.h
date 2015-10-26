#ifndef _LOG_WIN_H_
#define _LOG_WIN_H_
/*
2005-04-21	��袳�	�����ļ�,�������,δ����
2005-04-22	��袳�	����parent����(δʵ��),����LogLn(),ShowWin()
2006-07		��袳�	�������ݴ�С,Log̫��ʱ��ǰ��Ľص�
					��Ϊ����Log��������
					��ʼ��λ�������Ͻ�

����һ��log����,����һ��RichEdit,���ڼ�¼��Ϣ
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
