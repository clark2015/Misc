#ifndef Cmd_Player_Sociality_h
#define Cmd_Player_Sociality_h

//社交服务器消息
enum
{
//	PLAYER_SEND_BAG				= 2,		//背包类
//	PLAYER_SEND_GUILD			= 3,		//帮会信息
//	PLAYER_SEND_USER_RELATION	= 4,		//玩家关系
//	PLAYER_SEND_GUIDE			= 5,		//新手引导,暂无用，留着
//	PLAYER_SEND_TEAM			= 6,		//组队
//	PLAYER_SEND_RANKING_LIST	= 7,		//排行榜
//	PLAYER_SEND_MAIL			= 8,		//邮件
	PLAYER_SEND_QUERY_USER_FUZZY= 10,		//模糊查找玩家

//	PLAYER_RECV_ANS_OWNER		= 101,		//返回属性
//	PLAYER_RECV_GUILD			= 102,		//帮会信息
//	PLAYER_RECV_USER_RELATION	= 103,		//玩家关系
//	PLAYER_RECV_GUIDE			= 104,		//新手引导
//	PLAYER_RECV_TEAM			= 105,		//组队
//	PLAYER_RECV_RANKING_LIST	= 106,		//排行榜
//	PLAYER_RECV_MAIL			= 107,		//邮件
	PLAYER_RECV_QUERY_USER_FUZZY= 110,		//模糊查找玩家


	//聊天 PLAYER_MAIN_CHAT,PLAYER_MAIN_CHAT_GS子命令码
	PLAYER_SEND_CHAT			= 1,		//聊天
	PLAYER_RECV_CHAT			= 100,		//聊天
};

#pragma pack(push,1)

struct StPlayerSend_QueryUserFuzzy
{
	uint16	usCount;	//最多查多少个
	char	name[1];
};

struct StPlayerRecv_QueryUserFuzzy
{
	uint64	ullActorID;
	uint16	usLevel;
	uint8	ucProfession;	//职业
	char	szName[1];
};

struct StPlayer_Chat		//懒得分成2个了
{
	enum
	{
		FLAGS_SYS		= 1,		//是系统发的，还是玩家客户端发的
		FLAGS_NPC		= 2,		//说话人是不是NPC
	};
	uint8		ucChannel;		//聊天频道
	uint8		ucSubChannel;
	bool		bIsGM;
	uint8		ucReserved;	
	uint64		ullToActorID;
	uint64		ullFromActorID;	//系统消息是0
	union
	{
		char		szToActorName	[ACTOR_NAME_LEN];	//私聊时用于指定对象
		char		szFromActorName	[ACTOR_NAME_LEN];	//谁发的,私聊外的其它通道用
	};
	uint8		ucFlags;
	uint64		ullGuildID;		//帮会ID,或队伍ID
	uint32		uiVipRank;		//vip等级
	uint32		nReserved;		//保留
	uint16		usOffSet;		//本字段到szMsg之间的偏移，现在一般是4 ushort(VPlanLevel) + ushort(msg_size)
	char		szMsg[1];		//聊天信息
};

#pragma pack(pop)

#endif
