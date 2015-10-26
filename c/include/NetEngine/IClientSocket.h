#ifndef	_ICLIENT_SOCK_H
#define	_ICLIENT_SOCK_H
/*
2007-04-28  alan   6.00	����Proxy(socks4/5)����
						�������
2007-05-03  alan   6.10	ʵ��http����
2008-2/3��  alan   6.11	����ACE,ֱ����Windows API����ʵ����
						ʵ��GetFluxOut()/GetFluxIn()
						����CrashDump
2010-08-11  alan   6.12	����һ��memory leak��bug
	  ��19				����֧��NullEncrypt
						GetFluxIn/Out(),����ulonglong
2014-10-24		   7.00 OnError()ȥ��,OnClose()����OnConnectClosed();
*/

#include "localByteOrder.h"
#include "../commInterfaceDef.h"
#include "Proxy.h"
//**********************************************************************
#define  VER_CLIENT_SOCKET_MAIN		7		//���汾�����ǲ���������
#define  VER_CLIENT_SOCKET_MIN		0		//�ΰ汾�����Ǽ��ݵ�����(�������ӵ�)
#define  VER_CLIENT_SOCKET_BUILD	0		//build�汾�����ǹ��ܲ����bug��������
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
		//��������,pHeadһ�㲻�ù�
		virtual	void	Send(const void	*pBuff,int nLen)  = 0;
		//�ر�����,�Ȳ�Ҫ�ٽ�������,Ҳ���ٷ�������
		virtual	void	CloseConnect(uint32 ulReserved=0) = 0;

		virtual	char*	GetLocalIP(char szIP[16])	const = 0;
		virtual	uint32	GetLocalIP()				const = 0;
		virtual	uint16	GetLocalPort()				const = 0;

		virtual bool	IsPeerInfoReady()			const = 0;

		//ȡ��������(��ָsocket���)
		virtual uint64  GetFluxOut()				const = 0;
		virtual uint64  GetFluxIn()  				const = 0;

		//ȡ�ֽ�˳��
		virtual	enBYTE_ORDER GetPeerByteOrder()		const = 0;
		virtual	enBYTE_ORDER GetLocalByteOrder()	const = 0;
	};

typedef IClientSocketSinkHelper ICSSHelper;
//**********************************************************************

class IClientSocketSink
	{
	public:
	//	1.��OnConnectOK()֮ǰ����,������ӳ���,ֱ��OnConnectError()
	//	2.���ͨ�Ź��̳���Ҫ����OnError(),�Ȼ����OnGetHelper(NULL)
	//	3.�����Ľ�����OnConnectError()ǰ���������OnGetHelper(NULL)
		virtual void OnGetHelper(ICSSHelper *pHelper)	= 0;

		//���Ӵ���/OK
		virtual	void OnConnectError(const char *pMsg)	= 0;
		virtual	void OnConnectOK()						= 0;

		//�Է�����Ϣ(����Կ���ֽ����)���յ�����ʱ�ſ��Է�����Ϣ
		virtual void OnPeerInfoReady()					= 0;

		//�յ�����
		virtual	void OnRecv(const char * pBuf,int nLen)	= 0;
		
		//�����ѹر�,�Ȳ�����������,Ҳ�����ٷ�����,�Լ���CloseConnect()����ص����
		//code�뿴 SOCKET_CLOSE_CODE,pReason���ܻ�Ϊ��
		virtual	void OnConnectClosed(uint code,const char*pReason)	= 0;		
	};
	
/*	
		virtual	void OnClose()							= 0;

		//����,�ڵ�����֮ǰ,OnGetHelper(NULL)�ᱻ����,֮��OnClose()�ᱻ����
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
					lstrcpyn(szErrorMsg,"�޷��򿪶�̬��"_CSK_DLL_NAME,nLen);
				return false;
				}

			//���汾
			IClientSocket_CheckVersion_Func CheckVersion;
			CheckVersion  =  (IClientSocket_CheckVersion_Func) dlsym(m_hLib, "IClientSocket_CheckVersion");
			if ( NULL == CheckVersion )
				{
				if ( szErrorMsg && nLen > 0 )
					lstrcpyn(szErrorMsg,"����IClientSocket_CheckVersion()�Ҳ���",nLen);
				goto out;
				}
			if ( ! CheckVersion(VER_CLIENT_SOCKET_MAIN,VER_CLIENT_SOCKET_MIN,VER_CLIENT_SOCKET_BUILD) )
				{
				if ( szErrorMsg  && nLen > 0 )
					lstrcpyn(szErrorMsg,"�汾���ʧ��",nLen);
				goto out;
				}
			//�����첽�������
			IClientSocket_Create_Func createfunc;
			createfunc  =  (IClientSocket_Create_Func )dlsym(m_hLib, "IClientSocket_Create");
			if ( NULL == createfunc )
				{
				if ( szErrorMsg && nLen > 0 )
					lstrcpyn(szErrorMsg,"����IClientSocket_Create()�Ҳ���",nLen);
				goto out;
				}
			m_pSock = createfunc();
			if ( NULL == m_pSock )
				{
				if ( szErrorMsg  && nLen > 0 )
					lstrcpyn(szErrorMsg,"IServerSocket_Create()����NULL",nLen);
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
//��socket��Ϣ���ӵ�����
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
ʹ��CWndCDumpSocketSinkLoader֮ǰ:
1.���ļ�������
	CrashDump/ExceptionHandler.cpp/h
	CrashDump/GetWinVer.cpp/h
	CrashDump/MinVersion.cpp/h
2.�������´���
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

#define  VER_SOCKET_MAGR_MAIN	1		//���汾�����ǲ���������
#define  VER_SOCKET_MAGR_MIN	0		//�ΰ汾�����Ǽ��ݵ�����(�������ӵ�)
#define  VER_SOCKET_MAGR_BUILD	0		//build�汾�����ǹ��ܲ����bug��������

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
