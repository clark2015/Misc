#ifndef _CMD_PLAYER_Action_H_
#define _CMD_PLAYER_Action_H_

//PLAYER_MAIN_ACTOR_ACTION ���������뼰�ṹ
enum
{
										// �ṹ						����
	PLAYER_SEND_TRANSPORT_RECVED= 8,	//							�ͻ����յ� PLAYER_RECV_TRANSPORT ��Ҫ���������Ϣ���������յ���֮ǰ�����Կͻ��˵��˶���Ϣ
	PLAYER_SEND_CHANGE_LINE		= 9,	//uint16					Ҫ����
	PLAYER_SEND_GET_LINE_INFO	= 10,	//������					ȡ��·��Ϣ

	PLAYER_RECV_LEAVE			= 102,	//							����뿪��ֻ�����˳�������ʱ���ͣ��뿪�������򲻻�
	PLAYER_RECV_OBJECT_DEL		= 109,	//StPlayerRecv_ObjectDel	��Ʒ�ӵ�ͼ��ɾ��
	PLAYER_RECV_TRANSPORT		= 116,	//StPlayerRecv_Transport	���͵��µĵ�ͼ������
	PLAYER_RECV_LINE_INFO		= 117,	//struct{lineid,usercount}[] ��·��Ϣ,����,Ԫ����һ��16λ��·��,һ��16λ�û��� 
	PLAYER_RECV_SAFE_AREA		= 118,	//bool						����/�뿪��ȫ����

	//***********************************************************************************
	//PLAYER_MAIN_MISC����������
	PLAYER_SEND_PING_ANS		= 2,	//��						Ping��Ӧ
	PLAYER_SEND_A_LOG			= 3,	//							�ͻ���Ҫ�������log,���ǵ�a_log��
	PLAYER_SEND_POINT_LOG		= 4,	//										
	PLAYER_SEND_CDKEY			= 6,	//							cdkey
	
	PLAYER_RECV_PING			= 101,	//StPlayerRecv_Ping			Ping
	PLAYER_RECV_CDKEY			= 107,
	PLAYER_RECV_KICK_REASON		= 108,	//һ��uint16��ԭ��			��ұ��ߵ�ԭ��,1=����
};

#pragma pack(push,1)
struct a_log
{
    uint64  guid;
    uint64  logTime;
    uint32  logtype;
    char    clientOs[65];
    char    flashVer[65];
    char    clientVer[65];
    char    msg[1801];
};
#pragma pack(pop)
//sizeof(a_log) == 2016
//lua lua/dbcache/_print_c_struct.lua a_log.lua

#pragma pack(push,1)
struct point_log
{
    uint64  id;
    uint32  log_time;
    uint32  connect_time;
    char    auth[151];
    uint32  etype;
    char    msg[257];
    char    net[33];
    char    account_name[16];
};
#pragma pack(pop)
//sizeof(point_log) == 461+16
//lua lua/dbcache/_print_c_struct.lua point_log.lua

inline
const char* ClientALogToDB(char*p,int nLen,a_log&log)
{
	//type
	const uint*pp = (const uint*)p;
	int readed = 4;
	log.logtype = *pp;
	//clientos
	const char*pStr = (const char*)(p+readed);
	int nStrLen = strlen(pStr);
	lstrcpyn(log.clientOs,pStr,sizeof(log.clientOs));
	readed += nStrLen+1;
	if ( readed >= nLen )
	{
		return "Format Error 1";
	}
	//flashVer
	pStr = (const char*)(p+readed);
	nStrLen = strlen(pStr);
	lstrcpyn(log.flashVer,pStr,sizeof(log.flashVer));
	readed += nStrLen+1;
	if ( readed >= nLen )
	{
		return "Format Error 2";
	}
	//clientVer
	pStr = (const char*)(p+readed);
	nStrLen = strlen(pStr);
	lstrcpyn(log.clientVer,pStr,sizeof(log.clientVer));
	readed += nStrLen+1;
	if ( readed >= nLen )
	{
		return "Format Error 3";
	}
	//msg
	pStr = (const char*)(p+readed);
	nStrLen = strlen(pStr);
	lstrcpyn(log.msg,pStr,sizeof(log.msg));
	readed += nStrLen+1;

	return NULL;
}

inline
const char* ClientPointLogToDB(char*p,int nLen,point_log&log)
{
	int readed = 0;
	//auth
	lstrcpyn(log.auth,p,sizeof(log.auth));
	readed += strlen(p) + 1;
	if ( readed >= nLen )
	{
		return "Format Error 1";
	}
	//etype;
	uint*pui = (uint*)(p + readed);
	log.etype = *pui;
	readed += 4;
	if ( readed >= nLen )
	{
		return "Format Error 2";
	}
	//msg
	lstrcpyn(log.msg,p+readed,sizeof(log.msg));
	readed += strlen(p+readed) + 1;
	if ( readed >= nLen )
	{
		return "Format Error 3";
	}
	//net
	lstrcpyn(log.net,p+readed,sizeof(log.net));
	readed += strlen(p+readed) + 1;
	//account_name
	if ( readed >= nLen )
	{
		log.account_name[0] = 0;
		RelTrace("Warning��ClientPointLogToDB() :no account_name field");
	}
	else
		lstrcpyn(log.account_name,p+readed,sizeof(log.account_name));

	return NULL;
}

#pragma pack(push,1)

struct StPlayerRecv_ObjectDel
{
	uint64	ullGUIDs[1];
};

struct StPlayerRecv_Ping
{
	uint32	ulNetLag;	//������ʱ����λms������ʱ��/2
	bool	bNeedAns;
	uint32	now;		//=time(NULL)
};

struct StPlayerRecv_Birth
{
	uint16	usCount;
	uint16	usStructLen;
	typedef StPlayerBirthItem StItem;
	
	StItem	item[1];
};

const uint32 PLAYER_BIRTH_ITEM_MAX_SIZE = sizeof(StPlayerBirthItem) + 2 + 4;

struct StPlayerRecv_Leave
{
	uint64 ullActorID;
};

struct StPlayerRecv_Transport
{
	uint16	usPosX;
	uint16	usPosY;
	uint32	ulClientMapID;
	char	szInstanceName[MAP_NAME_LEN];	//������
	uchar	ucReserved0;	
	bool	bFight;							//�Ƿ�ս����ͼ
	uchar	ucInstanceType;					//0:���õ�ͼ,1:��ͨ����,2:������
	uint32	ulReserved0;	
	uint16	usLine;							//��ǰ��·
	uint32	ulServerMapID;
	uint32	ulInstanceSerialID;				//����ϵ��
	char	szClientExtendInfo[1];			//0��β����Ϣ�ַ���,һ����npc��Ϣ
};

#pragma pack(pop)

#endif
