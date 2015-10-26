#ifndef _IBLOCKING_CLIENT_SOCKET_H_
#define _IBLOCKING_CLIENT_SOCKET_H_

/*
2010-08-13  alan   1.00	建立工程

*/

#include "localByteOrder.h"
#include "../commInterfaceDef.h"
//**********************************************************************
#define  VER_BIO_CLIENT_SOCKET_MAIN		1		//主版本增加是不兼容升级
#define  VER_BIO_CLIENT_SOCKET_MIN		0		//次版本增加是兼容的升级(功能增加等)
#define  VER_BIO_CLIENT_SOCKET_BUILD	0		//build版本增加是功能不变的bug修正升级
//**********************************************************************

class IBlockingClientSocket
{
public:
	virtual	void Release()		= 0;
	
	//连接,返回NULL表示成功，否则返回出错原因
	//bKeepSilent=true则对网络错误不trace详细信息(对程序错误仍会trace)
	virtual const char* Connect(const char*pIpAddr,ushort usPort,bool bKeepSilent = false,bool bForceNoCrypt = false)	= 0;

	//断开连接
	virtual void		CloseConnect()							= 0;
	
	//接收,返回0表示连接关闭,buff长度应>=MAX_PKG_DATA_LEN
	virtual const char*	Recv(		void*pBuff,int &nLen)		= 0;
	
	//发送,返回false表示连接关闭
	virtual const char* Send (const void*pBuff,int  nLen)		= 0;
	virtual	const char* SendV(const void*pBuff[],const int pnLen[],int nCount,const void*pAppendHead = 0,int nAddHeadCount = 0)	= 0;
	
	//取数据流量(仅指socket层的)
	virtual uint64  GetFluxOut()	const	= 0;
	virtual uint64  GetFluxIn()  	const	= 0;
};

//**********************************************************************

#ifdef _DEBUG
	#define BIOCLIENTSOCKET_DLL_NAME	"./BClientSocketD"DLL_EXT
#else
	#define BIOCLIENTSOCKET_DLL_NAME	"./BClientSocket"DLL_EXT
#endif

class CBlockingClientSocketLoader : 
		public InterfaceLoader<IBlockingClientSocket,VER_BIO_CLIENT_SOCKET_MAIN,VER_BIO_CLIENT_SOCKET_MIN,VER_BIO_CLIENT_SOCKET_BUILD>
	{
	protected:
		virtual const char *GetDllFileName()	{	return BIOCLIENTSOCKET_DLL_NAME;		}
		virtual const char *GetProcName()		{	return "CreateBlockingClientSocket";	}
	};

#endif
