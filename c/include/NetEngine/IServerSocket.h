#ifndef	_ISERVER_SOCK_H
#define	_ISERVER_SOCK_H

/*
2006-02-05  ��袳�   v2.0 -> v3.0
					 		IServerSocketSinkHelper::Send()����pHead����
2006-07-01           v3.0 -> v4.0	
							CloseConnect()����delay����
2006-12-20           6.0    �Ƿ�������bug����
							�ӿڵ���
2007-02-09           7.0    ����SendV()
							�⼸�죬��ACE_Proactor������ԭ����select reactor
2007-04-28           7.01   
2007-06-04           7.02   ��ʱ�������ACE��timer,�Ա���Handler��delete��Timer���ڵ�����
2007-06-11           8.00	����OnOpenSuccess()
							BeginServer() ��Ϊ OpenSocket(),CloseServer()��ΪCloseSocket()
2008-2/3��           9.00	ֱ����Windows API��д��,������ACE,ͬ������IO��ɶ˿�,Linux������Ҫ��ʵ��
							����DebugCmd(),������
							ʵ��GetFluxOut()/GetFluxIn()
							ɾ��ConnectProbe()/OnProbeReturn()/OnProbeError()
							����CrashDump
2010-08-11��19       9.01	����Connect(),�������ΰ汾�Ա����
							����֧��NullEncrypt
							ǰ��ʱ������ֳ��豸��֧�֣�û�İ汾
							OnRecv()��pBuff����ȥ��const����
							GetFluxIn/Out(),����ulonglong
2010-12-13			 10.0	SendV()�ĳ��������const		
							SendV()����AppendHead��������֧��ǰ�÷�����
							CloseConnect()����pReason����
2011-10-03			 10.01	������������������һ���Ƿ�������bug
							�����������յ�SysPackʱû�е���OnClose()������
2011-10-22			 10.02	�˿ڴ���-1���������һ�����ض˿�
2013-06				 10.02  ʵ��linux�汾
2014-10-24			 11.00  OnError()ȥ��,OnClose()����OnConnectClosed();

*/

#include "../DLTypeDefs.h"
class IAsynProcQueue;
class IServerSocketSink;
class IServerSocketClientSink;
class IServerSocket;
struct sockaddr_in;
//**********************************************************************

class INewClientSink
	{
	public:	
		//OpenSocket()�ɹ�,�������false,��OpenSocket()Ҳ�᷵��false
		virtual bool OnOpenSuccess()				=0;
		//�¿ͻ�����������
		virtual IServerSocketSink* OnNewClient()	=0;	//ע������������첽������ص���
	};

class IServerSocket
	{
	public:
		//�ͷ�
		virtual	void Release()	 = 0;
		
		//�򿪷�����,�ɹ���ص�OnOpenSuccess(),���򷵻�false
		virtual	bool OpenSocket(ushort usPort,INewClientSink*pSink,IAsynProcQueue *pAsynQueue)	= 0;

		//�رշ�����
		virtual	bool CloseSocket()	 = 0;

		//������
		virtual long DebugCmd(uint32 ulCmd,uint32 ulParam0 = 0,uint32 ulParam1 = 0)				= 0;

		//�������������,������OpenSocket����ܵ���
		virtual bool Connect(const char*pIpAddr,ushort usPort,IServerSocketClientSink*pSink,bool bForceNoCrypt = false)	= 0;
		
		enum
		{
			DEBUG_CMD_NORMAL	= 0,DEBUG_CMD_POOL_INFO	= 1,DEBUG_CMD_CLEAR_POOL = 2
		};
	};

//**********************************************************************
#include "localByteOrder.h"

class IServerSocketSinkHelper
	{
	public:
		//��������
		virtual	void	Send (const void* pBuff,		int  nLen			  )	= 0;
		virtual	void	SendV(const void* pBuff[],const int pnLen[],int nCount,const void*pAppendHead = 0,int nAddHeadCount = 0)	= 0;

		//ע��Ready����ܷ�����
		virtual bool	IsPeerInfoReady()			const	= 0;

		//�ر�����,�Ȳ�Ҫ�ٽ�������,Ҳ���ٷ�������
		//ulDelayMs��ʾ��ʱʱ�䣬�Ա�socket���֮ǰ�����ݷ���
		//pReason��ʱ���ܷ���ͻ��˷���
		virtual	void	CloseConnect(uint32 ulDelayMs = 0,const char*pReason = 0)	= 0;
		
		//ȡ��ַ
		virtual	char *	GetPeerIP(char  szIP[16])		const	= 0;
		virtual	uint32	GetPeerIP()						const	= 0;
		virtual	char *	GetLocalIP(char szIP[16])		const	= 0;
		virtual	uint32	GetLocalIP()					const	= 0;
		virtual	uint16	GetPeerPort()					const	= 0;
		virtual bool	GetLocalAddr(sockaddr_in& Out)	const	= 0;
		virtual bool	GetPeerAddr(sockaddr_in& Out)	const	= 0;

		//ȡ��������(��ָsocket���)
		virtual uint64  GetFluxOut()					const	= 0;
		virtual uint64  GetFluxIn()						const	= 0;

		//ȡ�ֽ�˳��
		virtual	enBYTE_ORDER GetPeerByteOrder()			const	= 0;
		virtual	enBYTE_ORDER GetLocalByteOrder()		const	= 0;
	};

typedef IServerSocketSinkHelper ISSSHelper;
//**********************************************************************
class IServerSocketSink
	{
	public:
	//	1.�ڷ���ϵͳ��Ϣ�ɹ������,��Ҳ���ܷ��ͳ���,��ʱֱ�ӵ���2��
	//	2.���ͨ�Ź��̳���Ҫ����OnError(),�Ȼ����OnGetHelper(NULL)
	//	3.�����Ľ����������OnGetHelper(NULL)
		virtual void OnGetHelper(ISSSHelper *pHelper)	= 0;

		//�Է�����Ϣ(����Կ���ֽ����)���յ�����ʱ�ſ��Է�����Ϣ
		virtual void OnPeerInfoReady()					= 0;
	
		//�յ�����
	//	virtual	void OnRecv(const char* pBuff,int nSize)= 0;
		virtual	void OnRecv(	  char* pBuff,int nSize)= 0;	//pBuffû��const���Σ������е�Σ�գ���ʹ�������ԭ���޸���Ϣ��ת�����������кô�

		//�ر�������,��֤���������ݴ������ŵ���,�Լ���CloseConnect()����ص���
		//code�뿴 SOCKET_CLOSE_CODE,pReason���ܻ�Ϊ��
		virtual	void OnConnectClosed(uint code,const char*pReason)	= 0;
	};

class IServerSocketClientSink : public IServerSocketSink
	{
	public:
		//���Ӵ��� or OK
		virtual void OnConnectError(const char *pMsg)	= 0;
		virtual void OnConnectOK()						= 0;
	};

/*
		virtual	void OnClose()	 						= 0;			

		//����,��Ҫ��������ʹ�ð���CloseConnect()���ڵ��κ�IServerSocketSinkHelper����
		//�ڵ�����֮ǰ,OnGetHelper(NULL)�ᱻ����,֮��OnClose()�ᱻ����
		virtual	void OnError(const char *pMsg)	  		= 0;
		*/
//**********************************************************************
#define  VER_SERVER_SOCKET_MAIN		11		//���汾�����ǲ���������
#define  VER_SERVER_SOCKET_SUB		0		//�ΰ汾�����Ǽ��ݵ�����(�������ӵ�)
#define  VER_SERVER_SOCKET_BUILD	0		//build�汾�����ǹ��ܲ����bug��������

#ifdef _DEBUG
	#define S_SOCK_DLL_NAME	"./ServerSocketD"DLL_EXT
#else
	#define S_SOCK_DLL_NAME	"./ServerSocket"DLL_EXT
#endif
//**********************************************************************

class CServerSocketLoader
	{
	typedef bool (*IServerSocket_CheckVersion_Func)(int verMain,int verSub,int verBuild);
	typedef IServerSocket * (*IServerSocket_Create_Func)();

	public:
		CServerSocketLoader()	
			{
			m_pSock = NULL;
			m_hLib  = NULL;
			}
		~CServerSocketLoader()
			{
			Clean();
			}
		void Clean()
			{
			if ( m_pSock )
				m_pSock->Release();
			
			if ( m_hLib )
				FreeLibrary(m_hLib);
			}
		bool Create(char szErrorMsg[] = 0,int nErrLen = 0)
			{
			if ( szErrorMsg  && nErrLen > 0 )
				szErrorMsg[0] = 0;

			m_hLib = LoadLibrary(S_SOCK_DLL_NAME);
			if ( NULL == m_hLib )
				{
				if ( szErrorMsg  && nErrLen > 0 )
					lstrcpyn(szErrorMsg,"���ܴ򿪶�̬��"S_SOCK_DLL_NAME,nErrLen);
				return false;
				}

			//���汾
			IServerSocket_CheckVersion_Func CheckVersion;
			CheckVersion  =  (IServerSocket_CheckVersion_Func) GetProcAddress(m_hLib, "IServerSocket_CheckVersion");
			if (CheckVersion == NULL)
				{
				if ( szErrorMsg  && nErrLen > 0 )
					lstrcpyn(szErrorMsg,"����IServerSocket_CheckVersion()�Ҳ���",nErrLen);
				goto out;
				}
			if ( ! CheckVersion(VER_SERVER_SOCKET_MAIN,VER_SERVER_SOCKET_SUB,VER_SERVER_SOCKET_BUILD) )
				{
				if ( szErrorMsg  && nErrLen > 0 )
					lstrcpyn(szErrorMsg,"�汾���ʧ��",nErrLen);
				goto out;
				}
			//�����첽�������
			IServerSocket_Create_Func createfunc;
			createfunc  =  (IServerSocket_Create_Func )GetProcAddress(m_hLib, "IServerSocket_Create");
			if (CheckVersion == NULL)
				{
				if ( szErrorMsg  && nErrLen > 0 )
					lstrcpyn(szErrorMsg,"����IServerSocket_Create()�Ҳ���",nErrLen);
				goto out;
				}
			m_pSock = createfunc();
			if ( NULL == m_pSock )
				{
				if ( szErrorMsg  && nErrLen > 0 )
					lstrcpyn(szErrorMsg,"IServerSocket_Create()����NULL",nErrLen);
				}

		out:
			if ( NULL == m_pSock )
				{
				FreeLibrary(m_hLib);
				m_hLib = NULL;
				}
			
			return m_pSock != NULL;
			}
		
		bool  IsValid()	const						{	return m_pSock != NULL;	}
			  IServerSocket * operator ->()			{	return m_pSock;			}
		const IServerSocket * operator ->()	const	{	return m_pSock;			}
			  IServerSocket * GetInterface()		{	return m_pSock;			}
		const IServerSocket * GetInterface() const	{	return m_pSock;			}
			  IServerSocket * GetServerSock()		{	return m_pSock;			}
		const IServerSocket * GetServerSock() const	{	return m_pSock;			}
	protected:
		HMODULE m_hLib;
		IServerSocket	*m_pSock;	
	};
//**********************************************************************
#endif
