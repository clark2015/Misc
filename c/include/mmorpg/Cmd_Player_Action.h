#ifndef _CMD_PLAYER_Action_H_
#define _CMD_PLAYER_Action_H_

//PLAYER_MAIN_ACTOR_ACTION 的子命令码及结构
enum
{
										// 结构						解释
	PLAYER_SEND_TRANSPORT_RECVED= 8,	//							客户端收到 PLAYER_RECV_TRANSPORT 后要发出这个消息，服务器收到它之前将忽略客户端的运动消息
	PLAYER_SEND_CHANGE_LINE		= 9,	//uint16					要求换线
	PLAYER_SEND_GET_LINE_INFO	= 10,	//无内容					取线路信息

	PLAYER_RECV_LEAVE			= 102,	//							玩家离开，只会在退出服务器时发送，离开可视区域不会
	PLAYER_RECV_OBJECT_DEL		= 109,	//StPlayerRecv_ObjectDel	物品从地图上删除
	PLAYER_RECV_TRANSPORT		= 116,	//StPlayerRecv_Transport	传送到新的地图、副本
	PLAYER_RECV_LINE_INFO		= 117,	//struct{lineid,usercount}[] 线路信息,数组,元素是一个16位线路号,一个16位用户数 
	PLAYER_RECV_SAFE_AREA		= 118,	//bool						进入/离开安全区域

	//***********************************************************************************
	//PLAYER_MAIN_MISC的子命令码
	PLAYER_SEND_PING_ANS		= 2,	//无						Ping回应
	PLAYER_SEND_A_LOG			= 3,	//							客户端要求服务器log,将记到a_log表
	PLAYER_SEND_POINT_LOG		= 4,	//										
	PLAYER_SEND_CDKEY			= 6,	//							cdkey
	
	PLAYER_RECV_PING			= 101,	//StPlayerRecv_Ping			Ping
	PLAYER_RECV_CDKEY			= 107,
	PLAYER_RECV_KICK_REASON		= 108,	//一个uint16的原因			玩家被踢的原因,1=顶号
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
		RelTrace("Warning：ClientPointLogToDB() :no account_name field");
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
	uint32	ulNetLag;	//网络延时，单位ms，来回时间/2
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
	char	szInstanceName[MAP_NAME_LEN];	//副本名
	uchar	ucReserved0;	
	bool	bFight;							//是否战斗地图
	uchar	ucInstanceType;					//0:公用地图,1:普通副本,2:竟技场
	uint32	ulReserved0;	
	uint16	usLine;							//当前线路
	uint32	ulServerMapID;
	uint32	ulInstanceSerialID;				//副本系列
	char	szClientExtendInfo[1];			//0结尾的信息字符串,一般是npc信息
};

#pragma pack(pop)

#endif
