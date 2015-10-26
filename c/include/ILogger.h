#ifndef _ILOGGER_H_
#define _ILOGGER_H_
#include <stdio.h>
#include <stdlib.h>
#include "commInterfaceDef.h"
/*
һ������,���̰߳�ȫ��log��
ע��:
    �����ڲ�ʹ����sprintfϵ�к���,��ע��Log�����ݲ�Ҫ̫��,����Ҫע
�ⲻҪʹ�ÿͻ��������Ĳ�����Ϊ��һ��Log����,�����û����ƵĲ�������
����ɳɹ��Ĺ���.
								--��袳�
*/
/*
2004  	   		��袳�    		�����ļ�
2004-10-7  		��袳�    		������,�д�����
2005-04-21 		��袳�    		ʵ��plugin�Ĺ���
2005-04-22/23 	��袳�    		����plugin���ܵ�bug,������
						  		����Loader��
2005-04-24 		��袳�    		ʵ��ʱ�����ʾ
						  		����Loader��
2005-05-25		��袳�	  		����RelAssert��
2006-08-31		��袳�	  v1.02	����Trace������ʱR6002�Ĵ���
								�������ļ�һЩ���õĶ���
								Trace build�汾warning���sub�汾
2006-12-05		��袳�	  v1.10	ʵ��SetOutputFileName()
2007-07-27/30	��袳�	  v1.11	����SetEnableQueue(),GetEnableQueue()�ӿڣ�δʵ��.
								���������ΰ汾�Ա���������Ϸ����
								�������ļ�һЩ���õĶ���
2008-03-11		��袳�	  v1.12	����DirectLog(),GetModName()�ӿ�
2008-12-01		��袳�	  v1.13	����GetLogTarget(),GetWithTime()�ӿ�
2010-08-02		��袳�	  v1.14	����ILogFilter�ӿ�
2013-01-08		��袳�	  v1.15 ����ƴд����

*/

#define MAX_LOG_MOD_NAME_LEN	64
#define MAX_LOG_MSG_LEN			1024
//************************************************************
class ILogTargetPlugin	//LogĿ��Ĳ���ӿ�
	{
	public:
		//һ��˵��ֻҪʵ����2���ӿڼ���
		//�һ�ȷ��pModName��pMsg��ΪNULL,�����ܻ�Ϊ�մ�
		virtual void OnLog  (const char *pModName,const char *pMsg,const char *pTime = NULL)  = 0;	
		virtual void OnLogLn(const char *pModName,const char *pMsg,const char *pTime = NULL)  = 0;
		
		//����ܻ���������delete this
		virtual void OnAttach()	{}	//����AddPlugin()ʱ�ص�
		virtual void OnDetach()	{}	//����DelPlugin()ʱ�ص�
	};

class ILogFilter	//Log������
	{
	public:
		//����false����
		virtual bool OnLog  (const char *pModName,const char *pMsg,const char *pTime)  = 0;	
		virtual bool OnLogLn(const char *pModName,const char *pMsg,const char *pTime)  = 0;
	};
//************************************************************
class ILogTarget	//ILogger�����Ŀ��
	{
	public:
		virtual void Release()							= 0;
		//����ͬʱ���ö��Log��Ŀ��,����ͬʱlog���ļ���RichEdit
	#ifdef _WIN32
		class CWebBrowser2;
		//hWndΪNULL��Log��RichEdit
		virtual void SetRichEditHwnd(HWND *hWnd)		= 0;
		virtual void SetRichEditHwnd(CWebBrowser2 *pIE)	= 0;
		
		virtual void SetListViewHwnd(const char *pMod,HWND *hWnd) = 0;
		virtual void SetRemoteListViewHwnd(const char *pMod,const char *pWinCaption/*,UINT nCtlID*/) = 0;
	#else
		//��fdFileΪ-1��log,0Ϊ��׼���?
		virtual void SetOutputFd(int fdFile /*= 0 */)		= 0;
	#endif
	
		virtual void SetOutputStream(FILE *)	= 0;	//Win 32�������stdout������,����Ϊʲô
		
		//pNameΪ����������ļ�,�����ļ��Ƿ��ܴ�(�����ܷ�򿪣��Ժ�Logʱ��������)
		virtual bool SetOutputFileName(const char *pName,bool bCleanOld = false)	= 0;
		
		virtual void EnableStdout(bool b) 	= 0;
	
		//����/ɾ��Log������
		virtual void AddPlugin(ILogTargetPlugin *p) 	= 0;
		virtual void DelPlugin(ILogTargetPlugin *p) 	= 0;

		//����ʱ���ʽ,δʵ��
		virtual void SetTimeFormat(ulong ulReserved0,ulong ulReserved1,ulong ulReserved2)	= 0;

		//�Ƿ��Log������ӣ���һ���߳���Log
		virtual void SetEnableQueue(bool bEnable)	= 0;
		virtual bool GetEnableQueue()	const		= 0;

		//����/ɾ��Log���������
		virtual void AddFilter(ILogFilter *p) 	= 0;
		virtual void DelFilter(ILogFilter *p) 	= 0;
	};

//************************************************************
class ILogger
	{
	public:
		virtual void Release()						= 0;
		//����Log������ս���ʱĬ��Log��ϵͳĿ��
		virtual void SetLogTarget(ILogTarget *p) 	= 0;
		
		//��Logǰ�ӵ�����,Ϊ���򲻼�ģ����
		virtual void SetModName(const char *pMod) 	= 0;
		//���bΪTrue,�����Log��LogLn�൱�ڵ���tLog��tLogLn
		virtual void SetWithTime(bool b = true)		= 0;
		//������rgb��ɫ
	//	virtual void SetLogColor(ulong ulColor)		= 0;
		//*****************Log����*****************
		virtual void Log(const char *, ...)			= 0;
		virtual void LogLn(const char *, ...)		= 0;

		//ǿ��Logʱ��ʱ��
		virtual void tLog(const char *, ...)		= 0;
		virtual void tLogLn(const char *, ...)		= 0;

		//ǿ��Logʱ����ʱ��
		virtual void ntLog(const char *, ...)		= 0;
		virtual void ntLogLn(const char *, ...)		= 0;

		//ֱ��Log�ַ���,����sprintf֮��ķ���
		virtual void DirectLog(const char*p,bool bLn)					= 0;
		virtual void DirectLog(const char*p,bool bLn,bool bWithTime)	= 0;

		//ȡģ��������Ϣ
		virtual const char*GetModName()		const	= 0;
		virtual ILogTarget* GetLogTarget()	const	= 0;
		virtual bool GetWithTime()			const	= 0;
	};

//************************************************************************
#define  VER_LOGGER_MAIN	1		//���汾�����ǲ���������
#define  VER_LOGGER_MIN		1		//�ΰ汾�����Ǽ��ݵ�����(�������ӵ�)
#define  VER_LOGGER_BUILD	5		//build�汾�����ǹ��ܲ����bug��������
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
		
		ILogger*			 m_pSysLogger;	//һ�����õ�Logger����,��logger.dll�е�һ��ȫ�ֱ���,��ģ����Ϣ��ԶΪ"Sys",SetModName()��SetWithTime()��SetLogTarget()���䲻������,����Release()
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
		//���Խ��ľͷ����Խ���,���򷵻�ϵͳ��
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
#define ShowLoadLoggerErr(x)	MessageBox(NULL,x,"����",0);
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
			ShowLoadLoggerErr("��ʼ��ȫ��Loggerʧ��!��رճ����˳�!");	\
			}						\
		}							\
	};								\
static _c_globe_logger_loader_	_static_logger_loader_;		//2�������ĳ�ʼ��˳�����û�б�֤.....

#define INIT_GLOBE_LOGGER(szModName,bShowTime)	INIT_GLOBAL_LOGGER(szModName,bShowTime)		//���ݾɴ���

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

#define _NOT_IMPLEMENTED_(x)  RelTrace("����:��ͼ����δʵ�ֵĴ���[%s],�˴���λ���ļ�%s�ĵ�%d��",x,__FILE__,__LINE__)

#endif
