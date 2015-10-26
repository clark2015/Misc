#ifndef	_ICLIENT_SOCK_H
#define	_ICLIENT_SOCK_H
/*
2007-04-28  alan   6.00	增加Proxy(socks4/5)功能
						整理代码
2007-05-03  alan   6.10	实现http代理
2008-2/3月  alan   6.11	不用ACE,直接用Windows API重新实现了
						实现GetFluxOut()/GetFluxIn()
						加入CrashDump
2010-08-11  alan   6.12	修正一个memory leak的bug
	  至19				增加支持NullEncrypt
						GetFluxIn/Out(),返回ulonglong
2014-10-24		   7.00 OnError()去掉,OnClose()改名OnConnectClosed();
*/

#include "localByteOrder.h"
#include "../commInterfaceDef.h"
#include "Proxy.h"
//**********************************************************************
#define  VER_CLIENT_SOCKET_MAIN		7		//主版本增加是不兼容升级
#define  VER_CLIENT_SOCKET_MIN		0		//次版本增加是兼容的升级(功能增加等)
#define  VER_CLIENT_SOCKET_BUILD	0		//build版本增加是功能不变的bug修正升级
//**********************************************************************
class IClientSocketSink;
class IClientSocket
	{
	public:
		virtual	void Release()		= 0;
		virtual bool Connect(const char*pIpAddr,ushort usPort,
			IClientSocketSink *pSink,IAsynProcQueue *pAsynQueue,const STProxyInfo * pProxy = 0)	= 0;
	};
//**********************************************************************
class IClientSocketSinkHelper
	{
	public:
		//发送数据,pHead一般不用管
		virtual	void	Send(const void	*pBuff,int nLen)  = 0;
		//关闭连接,既不要再接收数据,也不再发送数据
		virtual	void	CloseConnect(uint32 ulReserved=0) = 0;

		virtual	char*	GetLocalIP(char szIP[16])	const = 0;
		virtual	uint32	GetLocalIP()				const = 0;
		virtual	uint16	GetLocalPort()				const = 0;

		virtual bool	IsPeerInfoReady()			const = 0;

		//取数据流量(仅指socket层的)
		virtual uint64  GetFluxOut()				const = 0;
		virtual uint64  GetFluxIn()  				const = 0;

		//取字节顺序
		virtual	enBYTE_ORDER GetPeerByteOrder()		const = 0;
		virtual	enBYTE_ORDER GetLocalByteOrder()	const = 0;
	};

typedef IClientSocketSinkHelper ICSSHelper;
//**********************************************************************

class IClientSocketSink
	{
	public:
	//	1.在OnConnectOK()之前调用,如果连接出错,直接OnConnectError()
	//	2.如果通信过程出错要调用OnError(),先会调用OnGetHelper(NULL)
	//	3.正常的结束及OnConnectError()前都不会调用OnGetHelper(NULL)
		virtual void OnGetHelper(ICSSHelper *pHelper)	= 0;

		//连接错误/OK
		virtual	void OnConnectError(const char *pMsg)	= 0;
		virtual	void OnConnectOK()						= 0;

		//对方的信息(如密钥，字节序等)已收到，这时才可以发送信息
		virtual void OnPeerInfoReady()					= 0;

		//收到数据
		virtual	void OnRecv(const char * pBuf,int nLen)	= 0;
		
		//连接已关闭,既不能再收数据,也不能再发数据,自己调CloseConnect()不会回调这个
		//code请看 SOCKET_CLOSE_CODE,pReason可能会为空
		virtual	void OnConnectClosed(uint code,const char*pReason)	= 0;		
	};
	
/*	
		virtual	void OnClose()							= 0;

		//出错,在调用它之前,OnGetHelper(NULL)会被调用,之后OnClose()会被调用
		virtual	void OnError(const char *pMsg)			= 0;
*/
//**********************************************************************

#ifdef _DEBUG
	#define _CSK_DLL_NAME	"./ClientSocketD"DLL_EXT
#else
	#define _CSK_DLL_NAME	"./ClientSocket"DLL_EXT
#endif

class CClientSocketLoader
	{
	typedef bool (*IClientSocket_CheckVersion_Func)(int verMain,int verMin,int verBuild);
	typedef IClientSocket * (*IClientSocket_Create_Func)();

	protected:
		HMODULE			m_hLib;
		IClientSocket*	m_pSock;	
	public:
		CClientSocketLoader()	
			{
			m_pSock = NULL;
			m_hLib  = NULL;
			}
		~CClientSocketLoader()
			{
			Close();
			}
		void Close()
			{
			if ( m_pSock )
				{
				m_pSock->Release();
				m_pSock = NULL;
				}
			
			if ( m_hLib )
				{
				dlclose(m_hLib);
				m_hLib = NULL;
				}
			}
		bool Create(char szErrorMsg[] = 0 ,int nLen = 0)
			{
			if ( !m_hLib )
				m_hLib = dlopen (_CSK_DLL_NAME,RTLD_LAZY);
			if ( NULL == m_hLib )
				{
				if ( szErrorMsg && nLen > 0 )
					lstrcpyn(szErrorMsg,"无法打开动态库"_CSK_DLL_NAME,nLen);
				return false;
				}

			//检查版本
			IClientSocket_CheckVersion_Func CheckVersion;
			CheckVersion  =  (IClientSocket_CheckVersion_Func) dlsym(m_hLib, "IClientSocket_CheckVersion");
			if ( NULL == CheckVersion )
				{
				if ( szErrorMsg && nLen > 0 )
					lstrcpyn(szErrorMsg,"函数IClientSocket_CheckVersion()找不到",nLen);
				goto out;
				}
			if ( ! CheckVersion(VER_CLIENT_SOCKET_MAIN,VER_CLIENT_SOCKET_MIN,VER_CLIENT_SOCKET_BUILD) )
				{
				if ( szErrorMsg  && nLen > 0 )
					lstrcpyn(szErrorMsg,"版本检查失败",nLen);
				goto out;
				}
			//生成异步处理对象
			IClientSocket_Create_Func createfunc;
			createfunc  =  (IClientSocket_Create_Func )dlsym(m_hLib, "IClientSocket_Create");
			if ( NULL == createfunc )
				{
				if ( szErrorMsg && nLen > 0 )
					lstrcpyn(szErrorMsg,"函数IClientSocket_Create()找不到",nLen);
				goto out;
				}
			m_pSock = createfunc();
			if ( NULL == m_pSock )
				{
				if ( szErrorMsg  && nLen > 0 )
					lstrcpyn(szErrorMsg,"IServerSocket_Create()返回NULL",nLen);
				}

		out:
			if ( NULL == m_pSock )
				{
				dlclose(m_hLib);
				m_hLib = NULL;
				}
			
			return m_pSock != NULL;
			}
		bool  IsValid()	const						{	return m_pSock != NULL;	}
			  IClientSocket * operator ->()			{	return m_pSock;			}
		const IClientSocket * operator ->()	  const	{	return m_pSock;			}
			  IClientSocket * GetInterface()		{	return m_pSock;			}
		const IClientSocket * GetInterface()  const	{	return m_pSock;			}
			  IClientSocket * GetClientSock()		{	return m_pSock;			}
		const IClientSocket * GetClientSock() const	{	return m_pSock;			}
	};

//**********************************************************************

#ifdef _WIN32
//把socket消息附加到窗口
class IAttachWndSocketSink : public IClientSocketSink
	{
	public : 
		virtual void Release()	= 0;
	};
	
class CAttachWndSocketSinkLoader
	{
	protected:
		IAttachWndSocketSink*	m_pSink;
		HMODULE 				m_hLib;
		typedef IAttachWndSocketSink* (*_CreateAttachWndSink)(HWND hParentWnd,IClientSocketSink *pSink);

	public:
		CAttachWndSocketSinkLoader()	{	m_pSink = NULL;	m_hLib = NULL;	}
		~CAttachWndSocketSinkLoader()	{	Close();	}

		void Close()
			{
			if ( m_pSink)	{	m_pSink->Release();	m_pSink = NULL;		}
			if ( m_hLib )	{	dlclose(m_hLib);	m_hLib  = NULL;		}
			}
		bool Create(HWND hParentWnd,IClientSocketSink *pSink)
			{
			if ( NULL == pSink )	return false;
			Close();
			
			m_hLib = dlopen (_CSK_DLL_NAME,RTLD_LAZY);
			if ( NULL == m_hLib )
				return false;

			_CreateAttachWndSink proc;
			proc =  (_CreateAttachWndSink)dlsym(m_hLib, "CreateAttachWndSink");
			if ( NULL == proc )
				{
				Close();
				return false;
				}
			m_pSink = proc(hParentWnd,pSink);
			if ( NULL == m_pSink )
				Close();
			return NULL != m_pSink;
			}
		bool IsValid() const				{	return NULL != m_pSink;	}
		IClientSocketSink*	GetSink()		{	return m_pSink;			}
		IClientSocketSink*	GetInterface()	{	return m_pSink;			}
	};

/*
使用CWndCDumpSocketSinkLoader之前:
1.在文件里增加
	CrashDump/ExceptionHandler.cpp/h
	CrashDump/GetWinVer.cpp/h
	CrashDump/MinVersion.cpp/h
2.加入如下代码
	#pragma comment (lib , "../Lib/CrashDump/dbghelp.lib")
	#include "../CrashDump/ExceptionHandler.h"
	#include "../Include/IAsynProc.h"
	#include "../Include/NetEngine/IClientSocket.h"
*/
#ifdef EXCEPTIONHANDLER_H

#define CLIENT_SOCKET_CORE_DUMP_TRY			__try{
#define CLIENT_SOCKET_CORE_DUMP_CATCH(func)	}__except(RecordExceptionInfo(GetExceptionInformation(), func)){}

class CWndCDumpSocketSinkLoader : public CAttachWndSocketSinkLoader
	{
	struct CConv : public IClientSocketSink
		{
		IClientSocketSink* m_pSink;
		
		virtual void OnGetHelper(ICSSHelper *pHelper)
			{
			CLIENT_SOCKET_CORE_DUMP_TRY
				m_pSink->OnGetHelper(pHelper);
			CLIENT_SOCKET_CORE_DUMP_CATCH("CWndCDumpSocketSinkLoader::CConv::OnGetHelper()")
			}
		virtual	void OnConnectError(const char *pMsg)
			{
			CLIENT_SOCKET_CORE_DUMP_TRY
				m_pSink->OnConnectError(pMsg);
			CLIENT_SOCKET_CORE_DUMP_CATCH("CWndCDumpSocketSinkLoader::CConv::OnConnectError()")
			}
		virtual	void OnConnectOK()
			{
			CLIENT_SOCKET_CORE_DUMP_TRY
				m_pSink->OnConnectOK();
			CLIENT_SOCKET_CORE_DUMP_CATCH("CWndCDumpSocketSinkLoader::CConv::OnConnectOK()")
			}
		virtual void OnPeerInfoReady()
			{
			CLIENT_SOCKET_CORE_DUMP_TRY
				m_pSink->OnPeerInfoReady();
			CLIENT_SOCKET_CORE_DUMP_CATCH("CWndCDumpSocketSinkLoader::CConv::OnPeerInfoReady()")
			}
		virtual	void OnRecv(const char * pBuf,int nLen)
			{
			CLIENT_SOCKET_CORE_DUMP_TRY
				m_pSink->OnRecv(pBuf,nLen);
			CLIENT_SOCKET_CORE_DUMP_CATCH("CWndCDumpSocketSinkLoader::CConv::OnRecv()")
			}
		virtual	void OnClose()
			{
			CLIENT_SOCKET_CORE_DUMP_TRY
				m_pSink->OnClose();
			CLIENT_SOCKET_CORE_DUMP_CATCH("CWndCDumpSocketSinkLoader::CConv::OnClose()")
			}
		virtual	void OnError(const char *pMsg)
			{
			CLIENT_SOCKET_CORE_DUMP_TRY
				m_pSink->OnError(pMsg);
			CLIENT_SOCKET_CORE_DUMP_CATCH("CWndCDumpSocketSinkLoader::CConv::OnError()")
			}
		};

		CConv m_Conv;
	public:
		bool Create(HWND hParentWnd,IClientSocketSink *pSink)
			{
			m_Conv.m_pSink = pSink;
			return CAttachWndSocketSinkLoader::Create(hParentWnd,&m_Conv);
			}
	};
#endif	//EXCEPTIONHANDLER_H
#endif	//_WIN32
//**********************************************************************

#define  VER_SOCKET_MAGR_MAIN	1		//主版本增加是不兼容升级
#define  VER_SOCKET_MAGR_MIN	0		//次版本增加是兼容的升级(功能增加等)
#define  VER_SOCKET_MAGR_BUILD	0		//build版本增加是功能不变的bug修正升级

class ISocketMagr
	{
	public:
		virtual void Release()	{}	//do nothing

		virtual bool Init(void * = NULL)	= 0;
		virtual bool End (void * = NULL)	= 0;
	};

class CSocketMagrLoader : 
		public InterfaceLoader<ISocketMagr,VER_SOCKET_MAGR_MAIN,VER_SOCKET_MAGR_MIN,VER_SOCKET_MAGR_BUILD>
	{
	protected:
		virtual const char *GetDllFileName()	{	return _CSK_DLL_NAME;		}
		virtual const char *GetProcName()		{	return "GetSocketMagr";	}
	};

class CGlobeSocketMagr
	{
		CSocketMagrLoader	m_Loader;
	public:
		CGlobeSocketMagr()	
			{	
			if (m_Loader.Create())
				{
				if ( !m_Loader->Init() )
					m_Loader.Clean();
				}
			}
		~CGlobeSocketMagr()	
			{
			if (m_Loader.IsValid())	
				{
				m_Loader->End();
				m_Loader.Clean();
				}
			}
	};

#endif
