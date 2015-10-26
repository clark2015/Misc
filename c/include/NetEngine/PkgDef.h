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

//下面这2个定义，用于方便直接在接收到的数据上构造新的数据包
#define PKG_HEAD_SPACE_LEN		12	//Recv()/OnRecv()得到的数据如果可写，则其前面至少还有 PKG_HEAD_SPACE_LEN 字节的空间可用
#define PKG_TAIL_SPACE_LEN		8	//Recv()/OnRecv()得到的数据如果可写，则其后面至少还有 PKG_TAIL_SPACE_LEN 字节的空间可用

#pragma pack(push,1)

//数据包头
struct STSocketHeader
	{
	ushort 	usDataLen;

	bool    IsSysPack()   const	{	return !!(usDataLen & 0x8000);	}
	uint16  RealDataLen() const {	return    usDataLen & 0x7FFF;	}
	};

//当STSocketHeader::usDataLen & 8000 != 0 时，是系统包
struct STPkgSysPkg
	{
	uint8   ucType;	//系统包类型
	};

enum SOCK_SYS_PKG_TYPE
	{
	SSPT_DELAY_CLOSE	= 1,
	SSPT_CRYPT_METHOD	= 2,
	};

#define SOCKET_CRYPT_METHOD_NORMAL	1	//一般的
#define SOCKET_CRYPT_METHOD_NO		2	//无加密

#define SOCKET_VER_MAIN		2
#define SOCKET_VER_SUB		0

//系统信息，连接建立时双方第一个发出及收到的包,无包头(STSocketHeader)
struct STPkgSysInfo
	{
	uint8 chLen;
	uint8 chVerMain;			//版本
	uint8 chVerSub;				//
	uint8 chByteOrder;			//字节顺序
	uint8 chDefCompressMethod;	//默认数据压缩方法
	uint8 chDefEncryptMethod ;	//默认数据加密方法
	uint8 chReserved;
	uint8 chCryptKeyLen;
	union
	{
		uint32	uiCryptKey[3];
		uint16  usCryptKey[6];
		uint8   chCryptKey[12];
	} CryptKey;
	//机器cpu类型
	//操作系统
	
	STPkgSysInfo()
		{
		chLen				= sizeof(STPkgSysInfo);
		chVerMain 			= SOCKET_VER_MAIN;
		chVerSub  			= SOCKET_VER_SUB;
		chByteOrder			= LOCAL_BYTEORDER;
		chDefCompressMethod = 0;	//无压缩
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


//OnConnectClosed() 的code参数，暂时放这里
enum SOCKET_CLOSE_CODE
{
	SOCKET_CLOSE_NORMAL		= 0,	//一般错误，例如read()/write()返回失败
	SOCKET_CLOSE_PEER_CLOSE	= 1,	//对方主动关闭
	SOCKET_CLOSE_PROTO_ERROR= 2,	//协议错误
};

#endif
