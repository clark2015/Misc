#ifndef _IBLOCKING_CLIENT_SOCKET_H_
#define _IBLOCKING_CLIENT_SOCKET_H_

/*
2010-08-13  alan   1.00	��������

*/

#include "localByteOrder.h"
#include "../commInterfaceDef.h"
//**********************************************************************
#define  VER_BIO_CLIENT_SOCKET_MAIN		1		//���汾�����ǲ���������
#define  VER_BIO_CLIENT_SOCKET_MIN		0		//�ΰ汾�����Ǽ��ݵ�����(�������ӵ�)
#define  VER_BIO_CLIENT_SOCKET_BUILD	0		//build�汾�����ǹ��ܲ����bug��������
//**********************************************************************

class IBlockingClientSocket
{
public:
	virtual	void Release()		= 0;
	
	//����,����NULL��ʾ�ɹ������򷵻س���ԭ��
	//bKeepSilent=true����������trace��ϸ��Ϣ(�Գ�������Ի�trace)
	virtual const char* Connect(const char*pIpAddr,ushort usPort,bool bKeepSilent = false,bool bForceNoCrypt = false)	= 0;

	//�Ͽ�����
	virtual void		CloseConnect()							= 0;
	
	//����,����0��ʾ���ӹر�,buff����Ӧ>=MAX_PKG_DATA_LEN
	virtual const char*	Recv(		void*pBuff,int &nLen)		= 0;
	
	//����,����false��ʾ���ӹر�
	virtual const char* Send (const void*pBuff,int  nLen)		= 0;
	virtual	const char* SendV(const void*pBuff[],const int pnLen[],int nCount,const void*pAppendHead = 0,int nAddHeadCount = 0)	= 0;
	
	//ȡ��������(��ָsocket���)
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
