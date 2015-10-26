#ifndef FRONT_SERVER_MSG_H__
#define FRONT_SERVER_MSG_H__
#include "DLTypeDefs.h"

enum
{
	FS_F2S_LOGIN		= 1,
	FS_F2S_USER_ENTER	= 2,
	FS_F2S_USER_LEAVE	= 3,
	FS_F2S_USER_MSG		= 4,
	FS_F2S_PING			= 5,

	FS_S2F_LOGIN_ANS	= 100,
	FS_S2F_MSG			= 101,
	FS_S2F_KICK			= 102,
	FS_S2F_PING			= 103,
};

#define FRONT_SERVER_MAGIC_STR		"^.^iAmFrOntSrV^_^"

#pragma pack(1)
struct StFrontServer_F2S_Login
{
	enum
	{
		MAGIC_NO	= 0x19283746,
		MAGIC_NO1	= 0xafdecbaf,
		MAGIC_NO2	= 0x0733eac2,
	};
	uint32	ulMagicNum;
	char	szMagicStr[20];
	uint32	ulMagicNum1;
	uint32	pid;
	char	szUserName[20];
	char	szUserPass[20];
	uint32	ulReserved[20];
	uint32	ulMagicNum2;
	uint32	ulIPV4Count;
	uint32	ulIPV6Count;
	uint32	ulDNSCount;
	struct IPV4_PORT
	{
		uint32  ulIP;
		ushort usPort;
	};
	IPV4_PORT ipv4[1];
};

struct StFrontServer_S2F_LoginAns
{
	enum
	{
		MAGIC_NO	= 0x84726435,
		MAGIC_NO1	= 0xacef7821,
	};
	uint32	ulMagicNum;
	char	szServerName[64];
	uint32	ulMagicNum1;
};

struct StFrontServer_F2S_UserEnter
{
	uint32	ulVersion;		//�Ա���չ���Ժ���Է�������������Ϣ
	ushort	usFixHeadLen;	//���ṹ�Ĵ�С == sizeof(StFrontServer_F2S_UserEnter)
	ushort	usUserDataLen;	//������û����ݵĴ�С
	uint32	ulUserID;
	uint32	ulUserIP;
	ushort	usUserPort;
	ushort	usFrontServerCount;
	struct  StFrontServerAddr	//�û������ӵ�ǰ�û�IP��ַ���������ǰ�û�Ƕ�׵Ļ����������⣬ȡ�������һ��
	{
		uint32	ulIP;
		ushort	usPort;
	};
	StFrontServerAddr FrontServerAddr[1];
};

struct StFrontServer_F2S_UserLeave
{
	uint32	ulUserID;
};

struct StFrontServer_F2S_UserMsg
{
	uint32	ulUserID;
	char	msg[1];
};

struct StFrontServer_S2F_Msg
{
	ushort  usCount;
	uint32	ulUserID[1];
//	char	msg[1];
};

struct StFrontServer_S2F_Kick
{
	uint32	ulUserID;
	uint32	ulDelayMS;
	char	szReason[1];
};

struct StFrontServer_Ping
{
	uint32	SerialNum;
	uint32	ulPingTime;
	bool	bActive;		//���������ǻ�Ӧping
};
#pragma pack(8)


//�����������ͻ������������û����ݳ���,     - ������ -�û��� 4���û�ID��С,�ο� StFrontServer_S2F_Msg 
#define FS_MAX_PKG_DATA_LEN	 (MAX_PKG_DATA_LEN	- 2     - 2      - 4*sizeof(uint32))	//=4080 - 2 - 2 - 16 = 4060

#endif
