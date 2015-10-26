#ifndef MMORPG_DEFS_H_
#define MMORPG_DEFS_H_
#include "../DLTypeDefs.h"
#include <assert.h>
#ifdef _WIN32
#include <Windows.h>
#endif
#include "../ports.h"
#include <string.h>	//for strlen

#define WORLD_NAME_LEN			17				//游戏世界名长度,包括结尾的0
#define SERVER_NAME_LEN			17				//服务器名长度,包括结尾的0
#define MAP_NAME_LEN			17				//地图/副本名长度,包括结尾的0
#define PLATFORM_NAME_LEN		21				//平台名长度,包括结尾的0
#define ACCOUNT_NAME_LEN		41				//帐号名长度,包括结尾的0
#define ACTOR_NAME_LEN			24				//角色名长度,包括结尾的0
#define NPC_NAME_LEN			24				//角色名长度,包括结尾的0
#define USER_PASS_LEN			16				//密码长度,包括结尾的0
#define	MAX_LOGIC_DATA_LEN		4000			//服务器与客户端之间可以发的最大的数据长度
#define MAX_SERVER_DATA_LEN		(5*1024*1024)	//服务器之间可以发的最大的数据长度(5M)
#define MAX_ACTOR_COUNT			30	//5			//帐号最多可以有多少个角色	//因为有合服操作，这个数字只能设大一点了

enum SERVER_TYPE
{
	SERVER_TYPE_INVALID			= 0,
	SERVER_TYPE_DISPATCH		= 1,		//分发服务器
	SERVER_TYPE_PLAYER			= 2,		//现在是每个玩家当成一个服务器
	SERVER_TYPE_DB				= 4,		//数据库服务器
	SERVER_TYPE_SOCIALITY		= 8,		//社交服务器，包括聊天,好友系统(好友上下线通知,加减好友...)，公会等信息
	SERVER_TYPE_SOCIALITY_1		= 0x10,		//暂无用
	SERVER_TYPE_SOCIALITY_2		= 0x20,		//暂无用
	SERVER_TYPE_SOCIALITY_3		= 0x40,		//暂无用
	SERVER_TYPE_GAME_NODE		= 0x80,		//游戏结点服务器,副本服务器,可以有多个
	SERVER_TYPE_GAME_NODE_CROSS	= 0x100,	//跨服
	SERVER_TYPE_GAME_NODE_ALL	= 0x200,	//全区全服
	SERVER_TYPE_GAME_NODE_ALL_1	= 0x400,	//
	SERVER_TYPE_GAME_NODE_ALL_2	= 0x800,	//
	SERVER_TYPE_GAME_NODE_ALL_3	= 0x1000,	//
	SERVER_TYPE_GAME_NODE_ALL_4	= 0x2000,	//
	SERVER_TYPE_GAME_NODE_ALL_5	= 0x3000,	//
	SERVER_TYPE_GAME_NODE_ALL_6	= 0x8000,	//
	SERVER_TYPE_GAME_NODE_ALL_7	= 0x10000,	//
	SERVER_TYPE_GAME_NODE_ALL_8	= 0x20000,	//
	SERVER_TYPE_GAME_NODE_ALL_9	= 0x40000,	//
	SERVER_TYPE_GAME_NODE_ALL_10= 0x80000,	//
	SERVER_TYPE_GAME_NODE_ALL_11= 0x100000,	//
	SERVER_TYPE_GAME_NODE_ALL_12= 0x200000,	//
	SERVER_TYPE_GAME_NODE_ALL_13= 0x400000,	//
	SERVER_TYPE_GAME_NODE_ALL_14= 0x800000,	//
	SERVER_TYPE_GAME_NODE_ALL_15= 0x1000000,//
	SERVER_TYPE_GAME_NODE_ALL_16= 0x2000000,//
	SERVER_TYPE_GAME_NODE_ALL_17= 0x4000000,//
	SERVER_TYPE_GAME_NODE_ALL_18= 0x8000000,//
	SERVER_TYPE_GAME_NODE_ALL_19= 0x10000000,//

	SERVER_TYPE_GAME_NODE_ALL_MIN	=	SERVER_TYPE_GAME_NODE_ALL,
	SERVER_TYPE_GAME_NODE_ALL_MAX	=	SERVER_TYPE_GAME_NODE_ALL_19,
	SERVER_TYPE_GAME_NODE_ALL_MASK  = 	SERVER_TYPE_GAME_NODE_ALL   |
										SERVER_TYPE_GAME_NODE_ALL_1	|
										SERVER_TYPE_GAME_NODE_ALL_2	|
										SERVER_TYPE_GAME_NODE_ALL_3	|
										SERVER_TYPE_GAME_NODE_ALL_4	|
										SERVER_TYPE_GAME_NODE_ALL_5	|
										SERVER_TYPE_GAME_NODE_ALL_6	|
										SERVER_TYPE_GAME_NODE_ALL_7	|
										SERVER_TYPE_GAME_NODE_ALL_8	|
										SERVER_TYPE_GAME_NODE_ALL_9	|
										SERVER_TYPE_GAME_NODE_ALL_10|
										SERVER_TYPE_GAME_NODE_ALL_11|
										SERVER_TYPE_GAME_NODE_ALL_12|
										SERVER_TYPE_GAME_NODE_ALL_13|
										SERVER_TYPE_GAME_NODE_ALL_14|
										SERVER_TYPE_GAME_NODE_ALL_15|
										SERVER_TYPE_GAME_NODE_ALL_16|
										SERVER_TYPE_GAME_NODE_ALL_17|
										SERVER_TYPE_GAME_NODE_ALL_18|
										SERVER_TYPE_GAME_NODE_ALL_19,

	SERVER_TYPE_GAME_NODE_MASK		=	SERVER_TYPE_GAME_NODE | SERVER_TYPE_GAME_NODE_CROSS | SERVER_TYPE_GAME_NODE_ALL_MASK,
};

inline 
char* ServerTypeStr(uint32 ulServerType,char buff[1024])	//1024 = 32个*32(strByte)
{
	//特殊情形
	if ( SERVER_TYPE_INVALID == ulServerType )
	{
		lstrcpyn(buff,"SERVER_TYPE_INVALID",32);
		return buff;
	}
	if ( -1 == ulServerType )
	{
		lstrcpyn(buff,"全部",32);
		return buff;
	}
	if ( SERVER_TYPE_GAME_NODE_MASK == ulServerType )
	{
		lstrcpyn(buff,"SERVER_TYPE_GAME_NODE_MASK",32);
		return buff;
	}
	
	//一个个加
	char*p = buff;
	if ( ulServerType & SERVER_TYPE_DISPATCH	)		{	if ( p != buff ) {	*p=',';	p++; }	lstrcpyn(p,"SERVER_TYPE_DISPATCH",	32);		p += strlen(p);	}
	if ( ulServerType & SERVER_TYPE_PLAYER		)		{	if ( p != buff ) {	*p=',';	p++; }	lstrcpyn(p,"SERVER_TYPE_PLAYER",	32);		p += strlen(p);	}
	if ( ulServerType & SERVER_TYPE_DB			)		{	if ( p != buff ) {	*p=',';	p++; }	lstrcpyn(p,"SERVER_TYPE_DB",		32);		p += strlen(p);	}
	if ( ulServerType & SERVER_TYPE_SOCIALITY	)		{	if ( p != buff ) {	*p=',';	p++; }	lstrcpyn(p,"SERVER_TYPE_SOCIALITY",	32);		p += strlen(p);	}
	if ( ulServerType & SERVER_TYPE_GAME_NODE	)		{	if ( p != buff ) {	*p=',';	p++; }	lstrcpyn(p,"SERVER_TYPE_GAME_NODE",	32);		p += strlen(p);	}
	if ( ulServerType & SERVER_TYPE_GAME_NODE_CROSS)	{	if ( p != buff ) {	*p=',';	p++; }	lstrcpyn(p,"SERVER_TYPE_GAME_NODE_CROSS",32);	p += strlen(p);	}
	if ( ulServerType & SERVER_TYPE_GAME_NODE_ALL   )	{	if ( p != buff ) {	*p=',';	p++; }	lstrcpyn(p,"SERVER_TYPE_GAME_NODE_ALL",	 32);	p += strlen(p);	}
	if ( ulServerType & SERVER_TYPE_GAME_NODE_ALL_1 )	{	if ( p != buff ) {	*p=',';	p++; }	lstrcpyn(p,"SERVER_TYPE_GAME_NODE_ALL_1",32);	p += strlen(p);	}
	if ( ulServerType & SERVER_TYPE_GAME_NODE_ALL_2 )	{	if ( p != buff ) {	*p=',';	p++; }	lstrcpyn(p,"SERVER_TYPE_GAME_NODE_ALL_2",32);	p += strlen(p);	}
	if ( ulServerType & SERVER_TYPE_GAME_NODE_ALL_3 )	{	if ( p != buff ) {	*p=',';	p++; }	lstrcpyn(p,"SERVER_TYPE_GAME_NODE_ALL_3",32);	p += strlen(p);	}
	if ( ulServerType & SERVER_TYPE_GAME_NODE_ALL_4 )	{	if ( p != buff ) {	*p=',';	p++; }	lstrcpyn(p,"SERVER_TYPE_GAME_NODE_ALL_4",32);	p += strlen(p);	}
	if ( ulServerType & SERVER_TYPE_GAME_NODE_ALL_5 )	{	if ( p != buff ) {	*p=',';	p++; }	lstrcpyn(p,"SERVER_TYPE_GAME_NODE_ALL_5",32);	p += strlen(p);	}
	if ( ulServerType & SERVER_TYPE_GAME_NODE_ALL_6 )	{	if ( p != buff ) {	*p=',';	p++; }	lstrcpyn(p,"SERVER_TYPE_GAME_NODE_ALL_6",32);	p += strlen(p);	}
	if ( ulServerType & SERVER_TYPE_GAME_NODE_ALL_7 )	{	if ( p != buff ) {	*p=',';	p++; }	lstrcpyn(p,"SERVER_TYPE_GAME_NODE_ALL_7",32);	p += strlen(p);	}
	if ( ulServerType & SERVER_TYPE_GAME_NODE_ALL_8 )	{	if ( p != buff ) {	*p=',';	p++; }	lstrcpyn(p,"SERVER_TYPE_GAME_NODE_ALL_8",32);	p += strlen(p);	}
	if ( ulServerType & SERVER_TYPE_GAME_NODE_ALL_9 )	{	if ( p != buff ) {	*p=',';	p++; }	lstrcpyn(p,"SERVER_TYPE_GAME_NODE_ALL_9",32);	p += strlen(p);	}
	if ( ulServerType & SERVER_TYPE_GAME_NODE_ALL_10)	{	if ( p != buff ) {	*p=',';	p++; }	lstrcpyn(p,"SERVER_TYPE_GAME_NODE_ALL_10",32);	p += strlen(p);	}
	if ( ulServerType & SERVER_TYPE_GAME_NODE_ALL_11)	{	if ( p != buff ) {	*p=',';	p++; }	lstrcpyn(p,"SERVER_TYPE_GAME_NODE_ALL_11",32);	p += strlen(p);	}
	if ( ulServerType & SERVER_TYPE_GAME_NODE_ALL_12)	{	if ( p != buff ) {	*p=',';	p++; }	lstrcpyn(p,"SERVER_TYPE_GAME_NODE_ALL_12",32);	p += strlen(p);	}
	if ( ulServerType & SERVER_TYPE_GAME_NODE_ALL_13)	{	if ( p != buff ) {	*p=',';	p++; }	lstrcpyn(p,"SERVER_TYPE_GAME_NODE_ALL_13",32);	p += strlen(p);	}
	if ( ulServerType & SERVER_TYPE_GAME_NODE_ALL_14)	{	if ( p != buff ) {	*p=',';	p++; }	lstrcpyn(p,"SERVER_TYPE_GAME_NODE_ALL_14",32);	p += strlen(p);	}
	if ( ulServerType & SERVER_TYPE_GAME_NODE_ALL_15)	{	if ( p != buff ) {	*p=',';	p++; }	lstrcpyn(p,"SERVER_TYPE_GAME_NODE_ALL_15",32);	p += strlen(p);	}
	if ( ulServerType & SERVER_TYPE_GAME_NODE_ALL_16)	{	if ( p != buff ) {	*p=',';	p++; }	lstrcpyn(p,"SERVER_TYPE_GAME_NODE_ALL_16",32);	p += strlen(p);	}
	if ( ulServerType & SERVER_TYPE_GAME_NODE_ALL_17)	{	if ( p != buff ) {	*p=',';	p++; }	lstrcpyn(p,"SERVER_TYPE_GAME_NODE_ALL_17",32);	p += strlen(p);	}
	if ( ulServerType & SERVER_TYPE_GAME_NODE_ALL_18)	{	if ( p != buff ) {	*p=',';	p++; }	lstrcpyn(p,"SERVER_TYPE_GAME_NODE_ALL_18",32);	p += strlen(p);	}
	if ( ulServerType & SERVER_TYPE_GAME_NODE_ALL_19)	{	if ( p != buff ) {	*p=',';	p++; }	lstrcpyn(p,"SERVER_TYPE_GAME_NODE_ALL_19",32);	p += strlen(p);	}

	uint32 x =	SERVER_TYPE_DISPATCH		|
				SERVER_TYPE_PLAYER			|
				SERVER_TYPE_DB				|
				SERVER_TYPE_SOCIALITY		|
				SERVER_TYPE_GAME_NODE		|
				SERVER_TYPE_GAME_NODE_CROSS	|
				SERVER_TYPE_GAME_NODE_ALL	|
				SERVER_TYPE_GAME_NODE_ALL_1	|
				SERVER_TYPE_GAME_NODE_ALL_2	|
				SERVER_TYPE_GAME_NODE_ALL_3	|
				SERVER_TYPE_GAME_NODE_ALL_4	|
				SERVER_TYPE_GAME_NODE_ALL_5	|
				SERVER_TYPE_GAME_NODE_ALL_6	|
				SERVER_TYPE_GAME_NODE_ALL_7	|
				SERVER_TYPE_GAME_NODE_ALL_8	|
				SERVER_TYPE_GAME_NODE_ALL_9	|
				SERVER_TYPE_GAME_NODE_ALL_10|
				SERVER_TYPE_GAME_NODE_ALL_11|
				SERVER_TYPE_GAME_NODE_ALL_12|
				SERVER_TYPE_GAME_NODE_ALL_13|
				SERVER_TYPE_GAME_NODE_ALL_14|
				SERVER_TYPE_GAME_NODE_ALL_15|
				SERVER_TYPE_GAME_NODE_ALL_16|
				SERVER_TYPE_GAME_NODE_ALL_17|
				SERVER_TYPE_GAME_NODE_ALL_18|
				SERVER_TYPE_GAME_NODE_ALL_19;
				;
	if  ( (ulServerType & (~x)) != 0 )				{	if ( p != buff ) {	*p=',';	p++; }	lstrcpyn(p,"未知的服务器类型",		32);	}	
	return buff;
}

#pragma pack(push,1)

struct StServerName
{
	union NAME
	{
		struct StSortItem
		{
			uint64 sn1;
			uint64 sn2;
		}SortItem;
		struct StName
		{
			char szName[SERVER_NAME_LEN];
		}RealName;
	}m_Name;

	const char*c_str()	const	{	return m_Name.RealName.szName;	}
	void SetName(const char*pName)
	{
		assert(pName && pName[0]);
		memset(this,0,sizeof(*this));
		lstrcpyn(m_Name.RealName.szName,pName,sizeof(m_Name.RealName.szName));
	}
	void SetEmpty()				{	memset(this,0,sizeof(*this));	}
	void SetZeroEnd()			{			m_Name.RealName.szName[sizeof(m_Name.RealName.szName)-1] =  0;	}
	bool IsZeroEnd()	const	{	return	m_Name.RealName.szName[sizeof(m_Name.RealName.szName)-1] == 0;	}

	StServerName(const char*pName)
	{
		#if _MSC_VER >= 1600
		static_assert(sizeof(m_Name.SortItem) == sizeof(m_Name.RealName.szName)-1,"StServerName : object length error");
		#endif
		SetName(pName);
	}
	StServerName()	{	}

	int Cmp(const StServerName& b) const
	{
		if ( this->m_Name.SortItem.sn1 < b.m_Name.SortItem.sn1 )
			return -1;
		if ( this->m_Name.SortItem.sn1 > b.m_Name.SortItem.sn1 )
			return 1;
		//现在( this->sn1 == b.sn1 )
		if ( this->m_Name.SortItem.sn2 < b.m_Name.SortItem.sn2 )
			return -1;
		if ( this->m_Name.SortItem.sn2 > b.m_Name.SortItem.sn2 )
			return 1;
		return 0;
	}
};

struct StServerInfo
{
protected:
	int32				m_nDSIndex;
	StServerName		m_ServerName;
	uchar				m_ucAlign;
	uint32				m_ServerType;	//在分发服务器，如果是玩家，则是SERVER_TYPE_PLAYER
public:
	const StServerName& GetServerNameObj() const					{	return	m_ServerName;						}
	void				SetServerNameObj(const StServerName& obj) 	{			m_ServerName = obj;					}
	void				SetServerName(const char*p)					{			m_ServerName.SetName(p);			}
	const char*			GetServerName()	const						{	return	m_ServerName.c_str();				}
	void				SetServerType(uint32 ulType)				{			m_ServerType = ulType;				}
	uint32				GetServerType()	const						{	return	m_ServerType;						}
	int32				GetDSIndex()	const						{	return	m_nDSIndex;							}
	void				SetDSIndex(int nDSIndex)					{			m_nDSIndex	 = nDSIndex;			}

	StServerInfo(const char*pName,		 uint32 ulServerType,int32 nDSIndex)	: m_ServerName(pName),m_ServerType(SERVER_TYPE(ulServerType)),m_nDSIndex(nDSIndex)	{}
	StServerInfo(const StServerName&name,uint32 ulServerType,int32 nDSIndex)	: m_ServerName(name), m_ServerType(SERVER_TYPE(ulServerType)),m_nDSIndex(nDSIndex)	{}
	StServerInfo()	{}

	int Cmp(const StServerInfo& b) const
	{
		int32 tmp = this->GetDSIndex() - b.GetDSIndex();
		if ( tmp == 0 )
			return m_ServerName.Cmp(b.m_ServerName);
		return tmp;
	}
};

#pragma pack(pop)

inline bool operator <  (const StServerName& a,const StServerName& b)	{	return  a.Cmp(b) < 0;	}
inline bool operator == (const StServerName& a,const StServerName& b)	{	return	( a.m_Name.SortItem.sn1 == b.m_Name.SortItem.sn1 ) && ( a.m_Name.SortItem.sn2 == b.m_Name.SortItem.sn2 );	}
inline bool operator != (const StServerName& a,const StServerName& b)	{	return	( a.m_Name.SortItem.sn1 != b.m_Name.SortItem.sn1 ) || ( a.m_Name.SortItem.sn2 != b.m_Name.SortItem.sn2 );	}

inline bool operator <  (const StServerInfo& a,const StServerInfo& b)	{	return  a.Cmp(b) < 0;	}
inline bool operator == (const StServerInfo& a,const StServerInfo& b)	{	return	( a.GetDSIndex() == a.GetDSIndex() ) && ( a.GetServerNameObj() == b.GetServerNameObj() );	}
inline bool operator != (const StServerInfo& a,const StServerInfo& b)	{	return	( a.GetDSIndex() != a.GetDSIndex() ) || ( a.GetServerNameObj() != b.GetServerNameObj() );	}

//几个固定的服务器名称
const StServerName SERVER_NAME_LOCAL			("LOCAL");
const StServerName SERVER_NAME_DISPATH			("分发服务器");
const StServerName SERVER_NAME_DB				("数据库服务器");
const StServerName SERVER_NAME_SOCIALITY		("社交服务器");
const StServerName SERVER_NAME_GAME_NODE		("主逻辑服务器");
const StServerName SERVER_NAME_GAME_NODE_ALL	("全区全服服务器");
const StServerName SERVER_NAME_GAME_NODE_CROSS	("跨服服务器");

enum	enSex
{
	enSex_Unknown	= 0,
//	enSex_Girl		= 1,
//	enSex_Boy		= 2,
	enSex_Boy		= 1,
	enSex_Girl		= 2,
};

/*
double类型可精确表示的最大正整数是  2 ^ 53	=	9007199254740992 即	9007w亿
假定最终会开 < 1w个服。在ID中占4个10进制位，用YYYY表示
每种类型的ID用不同的数字开头，占一个10进制位
角色1, ID: 1YYYYxxxxxxxxxxx
帮会2, ID: 2YYYYxxxxxxxxxxx
物品8, ID: 8YYYYxxxxxxxxxxx		//物品ID已改为用所有数字
NPC 9 (ID可以重用，特殊处理)
例如:2服，第3个角色ID是 1000200000000003
这样，每个服中每种ID会有1000亿个的空间
以下，ServerID实际用的是CountryID
*/

/*
角色ID,从1000w亿开始，每个服可以用1000亿个
每个服的ID范围: [1000000000000000 + ServerID*100000000000,1000000000000000 + (ServerID+1)*100000000000 - 1)
分配函数在存储过程 get_new_actor_id_range() 中
*/
#define MIN_ACTOR_ID		1000000000000000ll	
#define MAX_ACTOR_ID		1999999999999999ll

/*
公会ID,从2000w亿开始，每个服可以用1000亿个
每个服的ID范围: [2000000000000000 + ServerID*100000000000,2000000000000000 + (ServerID+1)*100000000000 - 1)
分配函数在 lua/netdata/faction/faction_id_new.lua 中
*/
#define MIN_GUILD_ID		2000000000000000ll
#define MAX_GUILD_ID		2999999999999999ll

/*
物品ID,从8000w亿开始，每个服可以用1000亿个
每个服的ID范围: [8000000000000000 + ServerID*100000000000,8000000000000000 + (ServerID+1)*100000000000 - 1)
分配函数在存储过程 get_good_id_range() 中

每次重启每个服务器分配10w个ID,设重启1w次，则产生10亿个ID；设有10台服务器，则产生100亿个ID，足够用了
//#define MIN_GOOD_ID			8000000000000000ll	
//#define MAX_GOOD_ID			8999999999999999ll


//上面的过时了
平台ID算它有450个,则每个平台有20w亿个ID可用，则每个游戏世界有20亿
每个服的ID范围: [pfID*20000000000000 + ServerID*2000000000,pfID*20000000000000 + (ServerID+1)*2000000000 - 1)

每次重启每个服务器分配5w个ID,设重启1w次，则产生5亿个ID；设有10台服务器，则产生50亿个ID，这是极端情况，实际上应该20亿够了
*/

/*
NPC ID,每次服务器重启后会重用
由于有多个逻辑结点，每个结点用 [900x000000000000,900x999999999999) 的ID,1w亿个
分配函数在 InstanceZone.cpp 的 CInstanceZone::CreateCreature()中
*/
#define MIN_NPC_ID		 9000000000000000ll
#define MAX_NPC_ID		 9007000000000000ll

enum
{
	OWNER_TYPE_ACTOR	= 1,
	OWNER_TYPE_GUILD	= 2,
	OWNER_TYPE_ALL		= (OWNER_TYPE_ACTOR | OWNER_TYPE_GUILD),
};

inline
uint owner_type(uint64 owner_id)
{
	if ( owner_id >= MIN_ACTOR_ID && owner_id <= MAX_ACTOR_ID )
		return OWNER_TYPE_ACTOR;

	if ( owner_id >= MIN_GUILD_ID && owner_id <= MAX_GUILD_ID )
		return OWNER_TYPE_GUILD;
	//RelTrace("owner_type(%llu) ??? ",owner_id);
	return 0;
}

inline 
bool is_owner_range_fit(uint owner_type,uint64 owner_id)
{
	if ( OWNER_TYPE_ACTOR & owner_type )
	{
		if ( owner_id >= MIN_ACTOR_ID && owner_id <= MAX_ACTOR_ID )
			return true;
	}
	if ( OWNER_TYPE_GUILD & owner_type )
	{
		if ( owner_id >= MIN_GUILD_ID && owner_id <= MAX_GUILD_ID )
			return true;
	}
	return false;
}

enum
{
	ACTOR_RIGHT_MANAGER		= 1,

};

#endif
