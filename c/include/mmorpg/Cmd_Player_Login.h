#ifndef _CMD_PLAYER_LOGIN_H_
#define _CMD_PLAYER_LOGIN_H_
#include "../DLTypeDefs.h"
#include "mmorpgdefs.h"
#include "Cmd_Player_PlatformInfo.h"

enum
{
										// 结构						解释
	//登录子命令码
	PLAYER_RECV_LOGIN_SUCC		= 1,
	PLAYER_RECV_LOGIN_FAIL		= 2,
	PLAYER_RECV_ALL_ACTOR		= 3,	//StPlayerRecv_AllActor		下发所有角色信息
	PLAYER_RECV_NEW_ACTOR_RES	= 4,	//StPlayerRecv_NewActorResult新建角色的结果
	PLAYER_RECV_DEL_ACTOR_RES	= 5,	//		删除角色的结果
	PLAYER_RECV_PING_DB			= 6,	//
	PLAYER_RECV_RAND_NAME		= 7,	//StPlayerRecv_RandName		返回随机角色名
	PLAYER_RECV_LOGIN_TOKEN		= 8,	//32字节字符串+0结尾		令牌信息

	PLAYER_SEND_NEW_ACTOR		= 100,	//StPlayerSend_NewActor		新建角色
	PLAYER_SEND_DEL_ACTOR		= 101,	//StPlayerSendRecv_GUID		删除角色
	PLAYER_SEND_USE_ACTOR		= 102,	//StPlayerSend_UseActor		使用这个actor ID登录
	PLAYER_SEND_PING_DB			= 103,
	PLAYER_SEND_RAND_NAME		= 105,	//StPlayerSend_RandName		取随机名字				
};

#pragma pack(push,1)

struct StPlayerSend_RandName
{
	uint8	sex;
};

struct StPlayerRecv_RandName
{
	char szRandName[ACTOR_NAME_LEN];
};

struct StPlayerSend_UseActor
{
	uint64	ullActorID;
	uint16	usViewWidth;	//可视窗口的高宽,现在没什么用了
	uint16	usViewHeight;	//
};

struct StPlayerRecv_AllActor
{
	uint16	usCount;
	uint16	usStructLen;
	struct	StItem
	{
		uint64		ullActorID;
		char		szActorName[ACTOR_NAME_LEN];
		uint8		ucLevel;
		uint8		profession;	//职业
		uint8		sex;
		uint8		cFive;		//五行
		uint16		usBanneret;
		uint16		usPosX;
		uint16		usPosY;
		uint32		ulMapID;
		char		szMapName[MAP_NAME_LEN];
		bool		bNeverLogin;
	};
	StItem item[1];

	uint32	GetSize()	const	{	return 4 + usCount*sizeof(StItem);	 }
};

struct StPlayerSend_NewActor
{
	char		szActorName[ACTOR_NAME_LEN];
	uint8		profession;	//职业
	uint8		sex;
	uint8		five;		//五行
	uint16		usFaceID;	//脸谱
};

struct StPlayerRecv_NewActorResult
{
	uint64		ullActorID;	//0表示失败
	uint16		usCode;		//暂时无用
	char		szReason[1];//如果成功，这个就是StPlayerSend_NewActor
};

struct StPlayerRecv_DelActorResult
{
	uint64		ullActorID;	//这是请求删除的ID
	uint16		usCode;		//0表示成功
	char		szReason[1];
};

//登录消息
struct StPlayerLogin
{
	enum
	{
		MAGIC_NO = 0x24568735
	};

	uint32		ulMagicNo;
	uint64		ullAccountID;	//ullActorID;
	uint16		usViewWidth;	//可视窗口的高宽
	uint16		usViewHeight;	//
	char		szUserPass[USER_PASS_LEN];
};

struct StPlayerLogin_V2
{
	enum
	{
		MAGIC_NO = 0x24568735
	};

	uint32		ulMagicNo;
	uint64		ullAccountID;	//ullActorID;
	uint16		usViewWidth;	//可视窗口的高宽
	uint16		usViewHeight;	//
	char		szUserPass[USER_PASS_LEN];
	uint32		uiServerID;
};

struct StPlayerBirthItem
{
	enum
	{
		STATUS_DIE			= 1,	//已经死了，这是尸体

		FLAGS_CREATE		= 1,	//1=NPC或物品创建，或玩家登录,0=对象进入视野
		FLAGS_HAS_Z			= 64,	//有z坐标，无则为0
		FLAGS_HAS_SPEED_MULT= 128,	//速度倍数,无则为1
	};
	uint64		ullActorID;
	char		szActorName[ACTOR_NAME_LEN];
	float		orientation;	//方向
	uint8		ucState;
	uint8		profession;		//职业
	char		sex;
	uint16		usPosX;
	uint16		usPosY;
	uint16		usFlags;
};

struct StPlayerLoginResult
{
	StPlayerBirthItem	item;
	struct other
	{
		uint64			ullReserved;					//ullAccountID;
		char			szInstanceName[MAP_NAME_LEN];	//副本名
	//	uint64			ullReserved0;					
		uint32			uiFunctionOpended3;				//已开启的功能
		uint32			ulReserved0;
		uint16			usViewWidth;					//可视窗口的高宽
		uint16			usViewHeight;					//
		uint32			ulClientMapID;					//地图ID
		bool			bFirstLogin;					//第一次登录
		bool			bFight;							//是否战斗地图
		uint8			ucInstanceType;					//0:公用地图,1:普通副本
		uint32			ulShowCloth;					//外观
		uint32			ulShowWeapon;					//外观
		uint32          ulShowWeaponEffect;				//外观
		uint16			usLine;							//当前线路
		uint32			ulServerMapID;
		uint32			ulInstanceSerialID;				//副本系列
		uint32			uiFunctionOpended1;				//已开启的功能
		uint32			uiFunctionOpended2;				//已开启的功能
		uint32			now;							//=time(NULL)
		char			szClientExtendInfo[1];			//0结尾的客户端信息，npc信息字符串
	};
	other o;
};
#pragma pack(pop)

#endif
