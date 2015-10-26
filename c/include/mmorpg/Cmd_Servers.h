#ifndef CMD_SERVERS_H
#define CMD_SERVERS_H
#include "Cmd_Player_PlatformInfo.h"

//������֮�����Ϣ
enum
{
	//ͨ��
	SERVER_MODULE_CHAT					= 3,	//����ģ��ķ���������Ϣ
    SERVER_MODULE_QUEST                 = 4,    //����ģ��ķ���������Ϣ
    SERVER_MODULE_LUA                   = 5,    //Luaģ��ķ���������Ϣ
    SERVER_MODULE_LUA_INSTANCE          = 6,    //Luaģ��ķ���������Ϣ
//	SERVER_MODULE_TRADE                 = 7,    //����ģ��ķ���������Ϣ
	SERVER_MODULE_CROSS_SUBSTRIBE		= 8,	//�������
	COMM_SERVER_BD_CACHE				= 9,	//��ͨ������֮������ݻ������


	//���ݿ��������Ϣ
	DB_SERVER_RECV_SYNC					= 103,
	DB_SERVER_RECV_NEW_GOODID_RANGE		= 104,
	DB_SERVER_RECV_LOGOUT				= 106,
	DB_SERVER_RECV_CACHE				= 108,
	DB_SERVER_RECV_QUERY_BY_NAME		= 109,
	DB_SERVER_RECV_CDKEY				= 112,
	DB_SERVER_RECV_HTTP_PROC			= 113,

	DB_SERVER_SEND_NEW_GOODID_RANGE		= 204,
	DB_SERVER_SEND_CACHE				= 207,
	DB_SERVER_SEND_QUERY_BY_NAME		= 210,
	DB_SERVER_SEND_CDKEY				= 213,
	DB_SERVER_SEND_HTTP_PROC			= 214,

	//AI��������Ϣ

	//�罻��������Ϣ
//	SOCIALITY_SERVER_RECV_CHAT_INFO		= 2000,
//	SOCIALITY_SERVER_SEND_CHAT_INFO		= 2001,

	//��Ϸ������������Ϣ
	GAMENODE_SERVER_RECV_KICK			= 3000,		//����ĳ��
	GAMENODE_SERVER_RECV_CHANGE_PLAYER	= 3001,		//����������Ҫ��ӽ𡢼Ӿ����

	GAMENODE_SERVER_SEND_CHANGE_PLAYER	= 3100,		//�ӽ��Ӧ
};

//ͨ����Ϣ

#pragma pack(push,1)
#ifdef _WIN32
	#pragma warning(disable:4200)
#endif

#define MAX_CLIENT_VAR_LEN	16		//�Ժ�������������	512
struct StCommServer_TransmitUser_Public
{
	uint64		ullActorID;
	uint16		usViewWidth;	//���Ӵ��ڵĸ߿�
	uint16		usViewHeight;	//
	uint32		ulNetLag;
	uint32		ulLoginSID;
	uint32		ulAccountRight;
	uint64		ullGroupID;		//ͬһ��ID���ᴫ����ͬ�ĵ�ͼȥ
	uint32		ulConnectTimeOS;	//����ʱ�䣬�������治������
	char		szClientVar[MAX_CLIENT_VAR_LEN];
	StPlayerPlatformInfo_V2		PlatformInfo;
};

#include "tables/actor_merge.h"
struct StCommServer_TransmitUser_FromDB_V2
{
	enum	{	MAGIC_NO = 0x77497749	};
	uint32		ulMagicNo;
	StCommServer_TransmitUser_Public	Public;

	bool		bTransport;	//true=��һ���߼����������͵���һ���߼�������,false=��¼���һ�δ��ͣ�����ѡ��ɫ���ٵ�¼
	uchar		ucReserved0;
	//��Ȼ��Ϸ�߼�����ͨ�����Բ鵽�����罻�ͷַ�������ҲҪ�õ���
	actor_merge	ActorInfo;
};

struct StAccountKey
{
	char		szPlatform[PLATFORM_NAME_LEN];
	char		szAccountName[ACCOUNT_NAME_LEN];
};

struct StCommServer_TransmitUser_ToDB
{
	enum	{	MAGIC_NO = 0x66366666	};
	uint32			ulMagicNo;
	StCommServer_TransmitUser_Public	Public;

	StAccountKey	AccountKey;
	uint16			usToX;	
	uint16			usToY;
	uint32			ulToMapID;
	uint64			ullToInstanceID;
	bool			bIsAdult;
	char			szToServerName[SERVER_NAME_LEN];
};

struct StGameNodeServer_Kick
{
	uint64	ullActorID;
};

struct StGameNodeServer_ChangePlayer
{
	uint64		ullActorID;
	uint64		ullActorID2;						//��ؽ�ɫID	log��
	char		szActorName [ACTOR_NAME_LEN];		//����Log
	char		szActorName2[ACTOR_NAME_LEN];		//����Log
	uint32		uiLogReason;
	int64		llSilver;							//Ǯ�����������ɸ�
	int64		llGold;								//Ǯ�����������ɸ�
	int64		llExp;								//����
	int64		llPow;								//����
	uint64		llReserved0;
	uint64		llReserved1;
	uint64		llReserved2;
	uint64		llReserved3;
	
	//������ģ���Զ����
	char	  szModuleName[32];			//ģ����
	char	  szDesc[32];				//��Ҫ����
	uint16	  usReasonCode;				//ģ���Զ����ԭ��
	uint64	  ullInnerID;				//ģ���Զ���,��������ǻ�ID
};

struct StGameNodeServer_ChangePlayer_Ans
{
	uint16		usErrorCode;		//0==�ɹ�,����δ��
	char		szReason[1];

	enum
	{
		ERROR_CODE_SUCCESS				= 0,
		ERROR_CODE_INNER				= 1,	//�ڲ�����
		ERROR_CODE_SOME_FIELD_UNSUPPORT = 2,	//��Щ�ֶε��޸Ĳ�֧��
		ERROR_CODE_PLAYER_NOT_FOUND		= 3,	//�Ҳ������
	};
};

struct StCommServer_QueryOwnerByName
{
	uint16		usOwnerType;
	char		szName[48];
	uint32		ulQueryID;
	uint32		ulSessionID;
};

#pragma pack(pop)

#endif
