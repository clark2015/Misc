#ifndef CMD_SERVERS_H
#define CMD_SERVERS_H
#include "Cmd_Player_PlatformInfo.h"

//服务器之间的消息
enum
{
	//通用
	SERVER_MODULE_CHAT					= 3,	//聊天模块的服务器间消息
    SERVER_MODULE_QUEST                 = 4,    //任务模块的服务器间消息
    SERVER_MODULE_LUA                   = 5,    //Lua模块的服务器间消息
    SERVER_MODULE_LUA_INSTANCE          = 6,    //Lua模块的服务器间消息
//	SERVER_MODULE_TRADE                 = 7,    //交易模块的服务器间消息
	SERVER_MODULE_CROSS_SUBSTRIBE		= 8,	//跨服订阅
	COMM_SERVER_BD_CACHE				= 9,	//普通服务器之间的数据缓存操作


	//数据库服务器消息
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

	//AI服务器消息

	//社交服务器消息
//	SOCIALITY_SERVER_RECV_CHAT_INFO		= 2000,
//	SOCIALITY_SERVER_SEND_CHAT_INFO		= 2001,

	//游戏结点服务器的消息
	GAMENODE_SERVER_RECV_KICK			= 3000,		//踢走某人
	GAMENODE_SERVER_RECV_CHANGE_PLAYER	= 3001,		//其它服务器要求加金、加经验等

	GAMENODE_SERVER_SEND_CHANGE_PLAYER	= 3100,		//加金回应
};

//通用消息

#pragma pack(push,1)
#ifdef _WIN32
	#pragma warning(disable:4200)
#endif

#define MAX_CLIENT_VAR_LEN	16		//以后有用再增大了	512
struct StCommServer_TransmitUser_Public
{
	uint64		ullActorID;
	uint16		usViewWidth;	//可视窗口的高宽
	uint16		usViewHeight;	//
	uint32		ulNetLag;
	uint32		ulLoginSID;
	uint32		ulAccountRight;
	uint64		ullGroupID;		//同一个ID将会传到相同的地图去
	uint32		ulConnectTimeOS;	//连接时间，断线重玩不会重置
	char		szClientVar[MAX_CLIENT_VAR_LEN];
	StPlayerPlatformInfo_V2		PlatformInfo;
};

#include "tables/actor_merge.h"
struct StCommServer_TransmitUser_FromDB_V2
{
	enum	{	MAGIC_NO = 0x77497749	};
	uint32		ulMagicNo;
	StCommServer_TransmitUser_Public	Public;

	bool		bTransport;	//true=从一个逻辑服务器传送到另一个逻辑服务器,false=登录后第一次传送，或重选角色后再登录
	uchar		ucReserved0;
	//虽然游戏逻辑可以通过属性查到，但社交和分发服务器也要用到它
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
	uint64		ullActorID2;						//相关角色ID	log用
	char		szActorName [ACTOR_NAME_LEN];		//方便Log
	char		szActorName2[ACTOR_NAME_LEN];		//方便Log
	uint32		uiLogReason;
	int64		llSilver;							//钱币数，可正可负
	int64		llGold;								//钱币数，可正可负
	int64		llExp;								//经验
	int64		llPow;								//功力
	uint64		llReserved0;
	uint64		llReserved1;
	uint64		llReserved2;
	uint64		llReserved3;
	
	//下面是模块自定义的
	char	  szModuleName[32];			//模块名
	char	  szDesc[32];				//简要描述
	uint16	  usReasonCode;				//模块自定义的原因
	uint64	  ullInnerID;				//模块自定义,例如可以是获奖ID
};

struct StGameNodeServer_ChangePlayer_Ans
{
	uint16		usErrorCode;		//0==成功,其它未定
	char		szReason[1];

	enum
	{
		ERROR_CODE_SUCCESS				= 0,
		ERROR_CODE_INNER				= 1,	//内部错误
		ERROR_CODE_SOME_FIELD_UNSUPPORT = 2,	//有些字段的修改不支持
		ERROR_CODE_PLAYER_NOT_FOUND		= 3,	//找不到玩家
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
