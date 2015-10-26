#ifndef	_ISERVER_SOCK_H
#define	_ISERVER_SOCK_H

/*
2006-02-05  蓝琚成   v2.0 -> v3.0
					 		IServerSocketSinkHelper::Send()增加pHead参数
2006-07-01           v3.0 -> v4.0	
							CloseConnect()增加delay参数
2006-12-20           6.0    非法操作的bug修正
							接口调整
2007-02-09           7.0    增加SendV()
							这几天，用ACE_Proactor代替了原来的select reactor
2007-04-28           7.01   
2007-06-04           7.02   用时间轴代替ACE的timer,以避免Handler被delete了Timer还在的问题
2007-06-11           8.00	增加OnOpenSuccess()
							BeginServer() 改为 OpenSocket(),CloseServer()改为CloseSocket()
2008-2/3月           9.00	直接用Windows API重写了,不再用ACE,同样是用IO完成端口,Linux版有需要再实现
							增加DebugCmd(),无内容
							实现GetFluxOut()/GetFluxIn()
							删除ConnectProbe()/OnProbeReturn()/OnProbeError()
							加入CrashDump
2010-08-11至19       9.01	增加Connect(),不改主次版本以便兼容
							增加支持NullEncrypt
							前段时间加了手持设备的支持，没改版本
							OnRecv()的pBuff参数去掉const修饰
							GetFluxIn/Out(),返回ulonglong
2010-12-13			 10.0	SendV()的长度数组加const		
							SendV()增加AppendHead参数，以支持前置服务器
							CloseConnect()增加pReason参数
2011-10-03			 10.01	修正服务器主动连接一个非法操作的bug
							修正服务器收到SysPack时没有调用OnClose()的问题
2011-10-22			 10.02	端口传进-1则随机监听一个本地端口
2013-06				 10.02  实现linux版本
2014-10-24			 11.00  OnError()去掉,OnClose()改名OnConnectClosed();

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
		//OpenSocket()成功,如果返回false,则OpenSocket()也会返回false
		virtual bool OnOpenSuccess()				=0;
		//新客户端连上来了
		virtual IServerSocketSink* OnNewClient()	=0;	//注意这个不是在异步队列里回调的
	};

class IServerSocket
	{
	public:
		//释放
		virtual	void Release()	 = 0;
		
		//打开服务器,成功则回调OnOpenSuccess(),否则返回false
		virtual	bool OpenSocket(ushort usPort,INewClientSink*pSink,IAsynProcQueue *pAsynQueue)	= 0;

		//关闭服务器
		virtual	bool CloseSocket()	 = 0;

		//调试用
		virtual long DebugCmd(uint32 ulCmd,uint32 ulParam0 = 0,uint32 ulParam1 = 0)				= 0;

		//建立对外的连接,必须在OpenSocket后才能调用
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
		//发送数据
		virtual	void	Send (const void* pBuff,		int  nLen			  )	= 0;
		virtual	void	SendV(const void* pBuff[],const int pnLen[],int nCount,const void*pAppendHead = 0,int nAddHeadCount = 0)	= 0;

		//注意Ready后才能发数据
		virtual bool	IsPeerInfoReady()			const	= 0;

		//关闭连接,既不要再接收数据,也不再发送数据
		//ulDelayMs表示延时时间，以便socket层把之前的数据发完
		//pReason有时可能方便客户端分析
		virtual	void	CloseConnect(uint32 ulDelayMs = 0,const char*pReason = 0)	= 0;
		
		//取地址
		virtual	char *	GetPeerIP(char  szIP[16])		const	= 0;
		virtual	uint32	GetPeerIP()						const	= 0;
		virtual	char *	GetLocalIP(char szIP[16])		const	= 0;
		virtual	uint32	GetLocalIP()					const	= 0;
		virtual	uint16	GetPeerPort()					const	= 0;
		virtual bool	GetLocalAddr(sockaddr_in& Out)	const	= 0;
		virtual bool	GetPeerAddr(sockaddr_in& Out)	const	= 0;

		//取数据流量(仅指socket层的)
		virtual uint64  GetFluxOut()					const	= 0;
		virtual uint64  GetFluxIn()						const	= 0;

		//取字节顺序
		virtual	enBYTE_ORDER GetPeerByteOrder()			const	= 0;
		virtual	enBYTE_ORDER GetLocalByteOrder()		const	= 0;
	};

typedef IServerSocketSinkHelper ISSSHelper;
//**********************************************************************
class IServerSocketSink
	{
	public:
	//	1.在发送系统信息成功后调用,但也可能发送出错,这时直接到第2条
	//	2.如果通信过程出错要调用OnError(),先会调用OnGetHelper(NULL)
	//	3.正常的结束不会调用OnGetHelper(NULL)
		virtual void OnGetHelper(ISSSHelper *pHelper)	= 0;

		//对方的信息(如密钥，字节序等)已收到，这时才可以发送信息
		virtual void OnPeerInfoReady()					= 0;
	
		//收到数据
	//	virtual	void OnRecv(const char* pBuff,int nSize)= 0;
		virtual	void OnRecv(	  char* pBuff,int nSize)= 0;	//pBuff没有const修饰，可能有点危险，但使程序可以原地修改消息并转发，对性能有好处

		//关闭连接了,保证在所有数据处理完后才到达,自己调CloseConnect()不会回调它
		//code请看 SOCKET_CLOSE_CODE,pReason可能会为空
		virtual	void OnConnectClosed(uint code,const char*pReason)	= 0;
	};

class IServerSocketClientSink : public IServerSocketSink
	{
	public:
		//连接错误 or OK
		virtual void OnConnectError(const char *pMsg)	= 0;
		virtual void OnConnectOK()						= 0;
	};

/*
		virtual	void OnClose()	 						= 0;			

		//出错,不要再在里面使用包括CloseConnect()在内的任何IServerSocketSinkHelper调用
		//在调用它之前,OnGetHelper(NULL)会被调用,之后OnClose()会被调用
		virtual	void OnError(const char *pMsg)	  		= 0;
		*/
//**********************************************************************
#define  VER_SERVER_SOCKET_MAIN		11		//主版本增加是不兼容升级
#define  VER_SERVER_SOCKET_SUB		0		//次版本增加是兼容的升级(功能增加等)
#define  VER_SERVER_SOCKET_BUILD	0		//build版本增加是功能不变的bug修正升级

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
					lstrcpyn(szErrorMsg,"不能打开动态库"S_SOCK_DLL_NAME,nErrLen);
				return false;
				}

			//检查版本
			IServerSocket_CheckVersion_Func CheckVersion;
			CheckVersion  =  (IServerSocket_CheckVersion_Func) GetProcAddress(m_hLib, "IServerSocket_CheckVersion");
			if (CheckVersion == NULL)
				{
				if ( szErrorMsg  && nErrLen > 0 )
					lstrcpyn(szErrorMsg,"函数IServerSocket_CheckVersion()找不到",nErrLen);
				goto out;
				}
			if ( ! CheckVersion(VER_SERVER_SOCKET_MAIN,VER_SERVER_SOCKET_SUB,VER_SERVER_SOCKET_BUILD) )
				{
				if ( szErrorMsg  && nErrLen > 0 )
					lstrcpyn(szErrorMsg,"版本检查失败",nErrLen);
				goto out;
				}
			//生成异步处理对象
			IServerSocket_Create_Func createfunc;
			createfunc  =  (IServerSocket_Create_Func )GetProcAddress(m_hLib, "IServerSocket_Create");
			if (CheckVersion == NULL)
				{
				if ( szErrorMsg  && nErrLen > 0 )
					lstrcpyn(szErrorMsg,"函数IServerSocket_Create()找不到",nErrLen);
				goto out;
				}
			m_pSock = createfunc();
			if ( NULL == m_pSock )
				{
				if ( szErrorMsg  && nErrLen > 0 )
					lstrcpyn(szErrorMsg,"IServerSocket_Create()返回NULL",nErrLen);
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
