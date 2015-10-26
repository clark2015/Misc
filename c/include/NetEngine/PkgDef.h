#ifndef  _PKG_DEF_H
#define  _PKG_DEF_H

#ifdef _WIN32
//	#include <Winsock2.h>
#else
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <sys/select.h>
	#include <sys/socket.h>
	#include <arpa/inet.h>
	#include <unistd.h>
#endif

#include <sys/types.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include "localByteOrder.h"

typedef unsigned int   uint32;
typedef unsigned short uint16;
typedef unsigned char  uint8;

#define MAX_PKG_DATA_LEN		4080
#define MAX_PKG_HEADER_LEN		16
#define MAX_PKG_LEN				(MAX_PKG_HEADER_LEN + MAX_PKG_DATA_LEN)

//������2�����壬���ڷ���ֱ���ڽ��յ��������Ϲ����µ����ݰ�
#define PKG_HEAD_SPACE_LEN		12	//Recv()/OnRecv()�õ������������д������ǰ�����ٻ��� PKG_HEAD_SPACE_LEN �ֽڵĿռ����
#define PKG_TAIL_SPACE_LEN		8	//Recv()/OnRecv()�õ������������д������������ٻ��� PKG_TAIL_SPACE_LEN �ֽڵĿռ����

#pragma pack(push,1)

//���ݰ�ͷ
struct STSocketHeader
	{
	ushort 	usDataLen;

	bool    IsSysPack()   const	{	return !!(usDataLen & 0x8000);	}
	uint16  RealDataLen() const {	return    usDataLen & 0x7FFF;	}
	};

//��STSocketHeader::usDataLen & 8000 != 0 ʱ����ϵͳ��
struct STPkgSysPkg
	{
	uint8   ucType;	//ϵͳ������
	};

enum SOCK_SYS_PKG_TYPE
	{
	SSPT_DELAY_CLOSE	= 1,
	SSPT_CRYPT_METHOD	= 2,
	};

#define SOCKET_CRYPT_METHOD_NORMAL	1	//һ���
#define SOCKET_CRYPT_METHOD_NO		2	//�޼���

#define SOCKET_VER_MAIN		2
#define SOCKET_VER_SUB		0

//ϵͳ��Ϣ�����ӽ���ʱ˫����һ���������յ��İ�,�ް�ͷ(STSocketHeader)
struct STPkgSysInfo
	{
	uint8 chLen;
	uint8 chVerMain;			//�汾
	uint8 chVerSub;				//
	uint8 chByteOrder;			//�ֽ�˳��
	uint8 chDefCompressMethod;	//Ĭ������ѹ������
	uint8 chDefEncryptMethod ;	//Ĭ�����ݼ��ܷ���
	uint8 chReserved;
	uint8 chCryptKeyLen;
	union
	{
		uint32	uiCryptKey[3];
		uint16  usCryptKey[6];
		uint8   chCryptKey[12];
	} CryptKey;
	//����cpu����
	//����ϵͳ
	
	STPkgSysInfo()
		{
		chLen				= sizeof(STPkgSysInfo);
		chVerMain 			= SOCKET_VER_MAIN;
		chVerSub  			= SOCKET_VER_SUB;
		chByteOrder			= LOCAL_BYTEORDER;
		chDefCompressMethod = 0;	//��ѹ��
		chDefEncryptMethod  = SOCKET_CRYPT_METHOD_NORMAL;
		chReserved			= 0;
		chCryptKeyLen		= 0;
		memset(&CryptKey,0,sizeof(CryptKey));
		}
	
	void TraceMe()	const
		{
		Trace("chLen=%d",chLen);
		Trace("chVerMain=%d",chVerMain);
		Trace("chVerSub=%d",chVerSub);
		Trace("chByteOrder=%d",chByteOrder);
		Trace("chDefCompressMethod=%d",chDefCompressMethod);
		Trace("chDefEncryptMethod=%d",chDefEncryptMethod);
		Trace("CryptKey: %2.2X %2.2X %2.2X %2.2X %2.2X %2.2X %2.2X %2.2X %2.2X %2.2X %2.2X %2.2X",
				CryptKey.chCryptKey[0],CryptKey.chCryptKey[1],CryptKey.chCryptKey[2],
				CryptKey.chCryptKey[3],CryptKey.chCryptKey[4],CryptKey.chCryptKey[5],
				CryptKey.chCryptKey[6],CryptKey.chCryptKey[7],CryptKey.chCryptKey[8],
				CryptKey.chCryptKey[9],CryptKey.chCryptKey[10],CryptKey.chCryptKey[11]);
		}
	};
/*
struct STPkgCryptMethod
	{
	uint8	ucMethod;
	char	key[12];
	STPkgCryptMethod()
		{
		memset(this,0,sizeof(this));
		ucMethod = SOCKET_CRYPT_METHOD_NO;
		}
	};
*/
#pragma pack(pop)


//OnConnectClosed() ��code��������ʱ������
enum SOCKET_CLOSE_CODE
{
	SOCKET_CLOSE_NORMAL		= 0,	//һ���������read()/write()����ʧ��
	SOCKET_CLOSE_PEER_CLOSE	= 1,	//�Է������ر�
	SOCKET_CLOSE_PROTO_ERROR= 2,	//Э�����
};

#endif
