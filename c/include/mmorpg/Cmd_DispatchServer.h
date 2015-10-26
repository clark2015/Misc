#ifndef CMD_DISPATCH_SERVER_H
#define CMD_DISPATCH_SERVER_H

enum
{
	DISPATCH_SERVER_SEND_HEARTBEAT		= 1,		//心跳
	DISPATCH_SERVER_SEND_HEARTBEAT_2	= 2,		//心跳
	DISPATCH_SERVER_SEND_LOGIN_RESULT	= 3,		//服务器登录结果
	DISPATCH_SERVER_RECV_LOGIN			= 50,		//登录

	DISPATCH_SERVER_SEND_USER_ENTER		= 100,
	DISPATCH_SERVER_SEND_USER_LEAVE		= 101,
	DISPATCH_SERVER_SEND_USER_DATA		= 102,
	DISPATCH_SERVER_SEND_SERVER_READY	= 103,		//通报服务器连上来了
	DISPATCH_SERVER_SEND_SERVER_DATA	= 104,
	DISPATCH_SERVER_SEND_SERVER_RUN		= 105,		//服务器运行命令
	DISPATCH_SERVER_SEND_RAND_SEED		= 106,		//(不)定时更新随机数种子
	DISPATCH_SERVER_RECV_USER_KICK		= 151,
	DISPATCH_SERVER_RECV_USER_DATA		= 152,
	DISPATCH_SERVER_RECV_SERVER_DATA	= 153,		//转发服务器数据
	DISPATCH_SERVER_RECV_TRANSMIT_USER	= 154,		//传送用户到其它服务器
	DISPATCH_SERVER_RECV_SERVER_RUN_RET	= 155,

	DISPATCH_SERVER_PING				= 200,
	DISPATCH_SERVER_PING_ANS			= 201,

	DISPATCH_SERVER_FOR_REPLAY			= 225,		//为录像机制保留

//	HEARTBEAT_INV	= 50,
	HEARTBEAT_INV	= 10,
};

#pragma pack(push,1)

struct StServerCmdHead
{
	uchar ucCmd;
};

struct StDispatchServer_Ping
{
	enum	
	{	
		CODE_TRACE				= 1,
		CODE_DB_RECV_DSINDEX_0	= 2,
		CODE_DB_RECV_DSINDEX_N	= 3
	};
	StServerName	ServerName;
	uint8			ucAlign;
	uint32			ulCode;
	uint32			ulPingTime;
	uint32			ulDispServerRecvTime1;	//这几个不靠谱，timeGetTime()不同进程中，后取的可能比先取的还小
	uint32			ulRecvTime;				//
	uint32			ulDispServerRecvTime2;	//
};

struct StDispatchServer_TransmitUser
{
	uint32			ulUserID;
	StServerName	ServerName;	//目标服务器
	uint8			ucAlign;
//	uint64			ullActorID;
	char			Buff[1];
};

struct StDispatchServer_ServerData
{
	uint32			ulServerType_Send;	//发送时没用，接收时得到发送服务器的标志
	uint32			ulServerType_Recv;	//如果这个为0，则使用ServerName，它跟ServerName只能一个有效
	StServerName	ServerName;			//
	uint8			ucAlign;
	uint16			usCmd;
	uint16			usBigDataOrder;		//发送大数据时的顺序号，因为最大是1-65535,这样数据可达249M
	uint32			ulBigDataLen;		//大数据长度
	char			Data[1];
};

struct StDispatchServer_ServerReady
{
	uint32			ulServerType;
	StServerName	ServerName;
	bool			bReady;
};

struct StDispatchServer_UserEnter
{
	uint32			ulFromServerType;
	StServerName	FromServerName;
	uint32			ulUserID;
	uint32			ulIP;
	uint16			usPort;
	char			UserBuff[1];
};

struct StDispatchServer_UserLeave
{
	uint32	ulUserID;
};

struct StDispatchServer_Send_UserData
{
	uint32	ulUserID;
	char	msg[1];
};

struct StDispatchServer_Recv_UserData
{
	uint16	usCount;	//用户ID数
	uint32	ulUserID[1];
//	char	msg[1];
};

struct StDispatchServer_UserKick
{
	uint32	ulUserID;
	uint32	ulDelayMs;
	char	szMsg[1];
};

struct StDispatchServer_ServerLogin
{
	enum	
	{
		VERSION		= 1,
		MAGIC_NO	= 0x79571687
	};

	uint32			ulMagicNO;
	uint32			ulVersion;
	uint32			ulServerType;
	uint32			ulTime;			//time(NULL)的值，双方的时间不能相差超过5分钟
	StServerName	ServerName;		//例如节点服务器可以把所管理的地图名字发上来
	char			align;
	char			szPasswd[34];	//考虑改成 md5(DSPSERVER_DEFAULT_PASS + ulTime)
};
#define DSPSERVER_DEFAULT_PASS	"G*&^$#R*&^DS#r4tt&Y&Y#H83@&%823"

struct StDispatchServer_CmdLineLogin
{
	enum
	{
		VERSION		= 1,
		MAGIC_NO	= 0xa8433ebc,

		//usFlags字段的值
		FLAGS_NEED_RESULT	= 1,	//需要返回结果
		FLAGS_NO_NET_CACHE	= 2,	//设置是否需要数据发送缓存的例外，用于测试响应速度，在FLAGS_NEED_RESULT有设置时才有用了
	};
	uint32			ulMagicNO;
	uint16			usVersion;
	uint16			usFlags;						//0=不需要返回结果，1=需要返回结果
	uint32			ulTime;							//time(NULL)的值，双方的时间不能相差超过5分钟
	char			szWorldName[WORLD_NAME_LEN];	//验证用
	char			szPasswd[34];					//考虑改成 md5(DSPSERVER_CMDLINE_PASS + ulTime)
	char			szServerName[SERVER_NAME_LEN];	//服务器名,或ds,db,gs,sc
	char			szCmdLine[1];

	bool IsNeedResult()	const	{	return !!(FLAGS_NEED_RESULT & usFlags);			}
	bool IsNoNetCache()	const	{	return !!(FLAGS_NO_NET_CACHE & usFlags);		}
	bool FlagsOK()		const	{	return 0 == (usFlags & (~(FLAGS_NEED_RESULT|FLAGS_NO_NET_CACHE)));	}	//有没有多余的标志
};
#define DSPSERVER_CMDLINE_PASS	"a@#_=#GYT*&^&%T7~!!@35ds4*&#2(@"
inline
int FillCmdlineLogin(unsigned char buff[4000],const char*pCmd,const char*pWorldName,const char*pServerName,ushort usAddFlags = 0)
{
	ushort usFlags	= usAddFlags;
	if ( usFlags	== 0 )
	{
		const char*p = pCmd;
		while (isspace(*p) )
			p++;
		if ( !strncasecmp(p,"getresult",     strlen("getresult"))	||
			 !strncasecmp(p,"gettrace",      strlen("gettrace"))	||
			 !strncasecmp(p,"gettracesimple",strlen("gettracesimple")) )
		{
			usFlags		|= StDispatchServer_CmdLineLogin::FLAGS_NEED_RESULT;
		}
	}

	memset(buff,0,sizeof(StDispatchServer_CmdLineLogin));
	buff[0] = DISPATCH_SERVER_RECV_LOGIN;
	StDispatchServer_CmdLineLogin*login = (StDispatchServer_CmdLineLogin*)(buff+1);
	login->ulMagicNO	= StDispatchServer_CmdLineLogin::MAGIC_NO;
	login->usVersion	= StDispatchServer_CmdLineLogin::VERSION;;
	login->ulTime		= (uint32)time(NULL);
	login->usFlags		= usFlags;
	lstrcpyn(login->szWorldName,pWorldName,				sizeof(login->szWorldName));
	lstrcpyn(login->szPasswd,	DSPSERVER_CMDLINE_PASS,	sizeof(login->szPasswd));
	lstrcpyn(login->szServerName,pServerName,			sizeof(login->szServerName));
	lstrcpyn(login->szCmdLine,pCmd,4000 - 1 - sizeof(StDispatchServer_CmdLineLogin));
	return 1+sizeof(StDispatchServer_CmdLineLogin) + strlen(login->szCmdLine);
}

struct StDispatchServer_ServerLoginResult
{
	enum	{	VERSION = 2	};

	uint32	ulVersion;
	uint16	usSecondPreSeed;	//大概多少秒会发来一个种子
	uint16	usSeedCount;
	uint32	ulRandSeed[1];

	ulong	Size()	const	{	return sizeof(*this) +	(usSeedCount) * sizeof(ulong) - sizeof(ulong);	 }
};

//心跳包
struct StDispatchServer_HeartBeat2
{
	uint16	usCount;
};

#pragma pack(pop)

//下面应该分离出去...
#include <vector>
#include <algorithm>
using namespace std;

inline
const char*DispatchServerCmdStr(uchar cmd)
{
	switch (cmd)
	{
	case DISPATCH_SERVER_SEND_HEARTBEAT		:	return "DISPATCH_SERVER_SEND_HEARTBEAT";
	case DISPATCH_SERVER_SEND_HEARTBEAT_2	:	return "DISPATCH_SERVER_SEND_HEARTBEAT_2";
	case DISPATCH_SERVER_SEND_LOGIN_RESULT	:	return "DISPATCH_SERVER_SEND_LOGIN_RESULT";
	case DISPATCH_SERVER_RECV_LOGIN			:	return "DISPATCH_SERVER_RECV_LOGIN";
	case DISPATCH_SERVER_SEND_USER_ENTER	:	return "DISPATCH_SERVER_SEND_USER_ENTER";
	case DISPATCH_SERVER_SEND_USER_LEAVE	:	return "DISPATCH_SERVER_SEND_USER_LEAVE";
	case DISPATCH_SERVER_SEND_USER_DATA		:	return "DISPATCH_SERVER_SEND_USER_DATA";
	case DISPATCH_SERVER_SEND_SERVER_READY	:	return "DISPATCH_SERVER_SEND_SERVER_READY";
	case DISPATCH_SERVER_SEND_SERVER_DATA	:	return "DISPATCH_SERVER_SEND_SERVER_DATA";
	case DISPATCH_SERVER_SEND_SERVER_RUN	:	return "DISPATCH_SERVER_SEND_SERVER_RUN";
	case DISPATCH_SERVER_SEND_RAND_SEED		:	return "DISPATCH_SERVER_SEND_RAND_SEED";
	case DISPATCH_SERVER_RECV_USER_KICK		:	return "DISPATCH_SERVER_RECV_USER_KICK";
	case DISPATCH_SERVER_RECV_USER_DATA		:	return "DISPATCH_SERVER_RECV_USER_DATA";
	case DISPATCH_SERVER_RECV_SERVER_DATA	:	return "DISPATCH_SERVER_RECV_SERVER_DATA";
	case DISPATCH_SERVER_RECV_TRANSMIT_USER	:	return "DISPATCH_SERVER_RECV_TRANSMIT_USER";
	case DISPATCH_SERVER_PING				:	return "DISPATCH_SERVER_PING";
	case DISPATCH_SERVER_PING_ANS			:	return "DISPATCH_SERVER_PING_ANS";
	case DISPATCH_SERVER_FOR_REPLAY			:	return "DISPATCH_SERVER_FOR_REPLAY";
	default: return "Unknown dispatch server command code";
	}
}

inline 
void TraceCmdStruct(ushort usDSIndex,uchar ucCmd,const uchar*pBuff,ushort usLen)
{
	{
	//	char szLastBuff[5000*2];
		char szLastBuff[1024];
		const int nMaxTraceBuff = (sizeof(szLastBuff) - 200)/2;
		if ( usLen > 0 && pBuff )
		{
			ushort usLen1 = usLen;
			if ( usLen1 > nMaxTraceBuff )
				usLen1 = nMaxTraceBuff;
			ConvBinDataToReadable(pBuff,szLastBuff,usLen1);
			szLastBuff[usLen1*2] = 0;
		}
		else
		{
			szLastBuff[0] = 0;
		}
		int nDSIndex = usDSIndex == 0xFFFF ? -1 : usDSIndex;
		RelTrace("DsIndex=%d,Cmd=%s(%u),Len=%u,%sbuff=%s",nDSIndex,DispatchServerCmdStr(ucCmd),ucCmd,usLen,usLen>nMaxTraceBuff?"长度太长":"",szLastBuff);
	}

	switch (ucCmd)
	{
	case DISPATCH_SERVER_SEND_USER_DATA		:
		{
			RelTrace("struct = StDispatchServer_Send_UserData,sizeof(struct) = %u,LogicDataLen=%u",sizeof(StDispatchServer_Send_UserData),usLen - sizeof(StDispatchServer_Send_UserData) + 1);
			const StDispatchServer_Send_UserData*p = (const StDispatchServer_Send_UserData*)pBuff;
			const uchar*msg = (const uchar*)p->msg;
			if (	  usLen >= sizeof(StDispatchServer_Send_UserData) + 2 )
				RelTrace("         ulUserID=0x%X,msg[0]=%u,msg[1]=%u,msg[2]=%u",p->ulUserID,msg[0],msg[1],msg[2]);
			else if ( usLen >= sizeof(StDispatchServer_Send_UserData) + 1 )
				RelTrace("         ulUserID=0x%X,msg[0]=%u,msg[1]=%u",			p->ulUserID,msg[0],msg[1]);
			else if ( usLen >= sizeof(StDispatchServer_Send_UserData) )
				RelTrace("         ulUserID=0x%X,msg[0]=%u,msg[1]=%u",			p->ulUserID,msg[0]);
			else
				RelTrace("         message lentgh is wrong");
		}
		break;
	case DISPATCH_SERVER_SEND_SERVER_DATA	:
	case DISPATCH_SERVER_RECV_SERVER_DATA	:
		{
			RelTrace("struct = StDispatchServer_ServerData ,sizeof(struct) = %u,LogicDataLen=%u",sizeof(StDispatchServer_ServerData),usLen - sizeof(StDispatchServer_ServerData) + 1);
			const StDispatchServer_ServerData*p = (const StDispatchServer_ServerData*)pBuff;
			const uchar*Data = (const uchar*)p->Data;
			if (	  usLen >= sizeof(StDispatchServer_ServerData) + 2 )
				RelTrace("         ServerType_Send=0x%X,ServerType_Recv=0x%X,ServerName=[%s],Cmd=%u,msg[0]=%u,msg[1]=%u,msg[2]=%u",	p->ulServerType_Send,p->ulServerType_Recv,p->ServerName.c_str(),p->usCmd,Data[0],Data[1],Data[2]);
			else if ( usLen >= sizeof(StDispatchServer_Send_UserData) + 1 )
				RelTrace("         ServerType_Send=0x%X,ServerType_Recv=0x%X,ServerName=[%s],Cmd=%u,msg[0]=%u,msg[1]=%u",			p->ulServerType_Send,p->ulServerType_Recv,p->ServerName.c_str(),p->usCmd,Data[0],Data[1]);
			else if ( usLen >= sizeof(StDispatchServer_Send_UserData) )
				RelTrace("         ServerType_Send=0x%X,ServerType_Recv=0x%X,ServerName=[%s],Cmd=%u,msg[0]=%u",						p->ulServerType_Send,p->ulServerType_Recv,p->ServerName.c_str(),p->usCmd,Data[0]);
			else if ( usLen >= sizeof(StDispatchServer_Send_UserData) - 1)
				RelTrace("         ServerType_Send=0x%X,ServerType_Recv=0x%X,ServerName=[%s],Cmd=%u",								p->ulServerType_Send,p->ulServerType_Recv,p->ServerName.c_str(),p->usCmd);
			else
				RelTrace("         message lentgh is wrong");
		}
		break;
	case DISPATCH_SERVER_FOR_REPLAY			:
		if ( usLen >= 1 )
			RelTrace("DISPATCH_SERVER_FOR_REPLAY SubCmd=%u",pBuff[0]);
		else
			RelTrace("DISPATCH_SERVER_FOR_REPLAY message lentgh is wrong");
		break;
	case DISPATCH_SERVER_RECV_USER_DATA		:	break;
	case DISPATCH_SERVER_SEND_HEARTBEAT		:	break;
	case DISPATCH_SERVER_SEND_HEARTBEAT_2	:	break;
	case DISPATCH_SERVER_SEND_LOGIN_RESULT	:	break;
	case DISPATCH_SERVER_RECV_LOGIN			:	break;
	case DISPATCH_SERVER_SEND_USER_ENTER	:	break;
	case DISPATCH_SERVER_SEND_USER_LEAVE	:	break;
	case DISPATCH_SERVER_SEND_SERVER_READY	:	break;
	case DISPATCH_SERVER_SEND_SERVER_RUN	:	break;
	case DISPATCH_SERVER_SEND_RAND_SEED		:	break;
	case DISPATCH_SERVER_RECV_USER_KICK		:	break;
	case DISPATCH_SERVER_RECV_TRANSMIT_USER	:	break;
	case DISPATCH_SERVER_PING				:	break;
	case DISPATCH_SERVER_PING_ANS			:	break;
	case DISPATCH_SERVER_RECV_SERVER_RUN_RET:	break;
	default: 
		RelTrace("Unknown dispatch server command code,struct is unknown");
		break;
	}
}

struct StServerDetail
{
protected:
	StServerName		m_ServerName;
	SERVER_TYPE			m_ServerType;	//在分发服务器，如果是玩家，则是SERVER_TYPE_PLAYER
public:
	const StServerName& GetServerNameObj() const					{	return m_ServerName;				}
	void				SetServerNameObj(const StServerName& obj) 	{			m_ServerName = obj;			}
	void				SetServerName(const char*p)					{	m_ServerName.SetName(p);			}
	const char*			GetServerName()	const						{	return m_ServerName.c_str();		}
	void				SetServerType(ulong ulType)					{	m_ServerType = (SERVER_TYPE)ulType;	}
	SERVER_TYPE			GetServerType()	const						{	return m_ServerType;				}
};

inline bool LessServerDetailPointer(const StServerDetail* p1,const StServerDetail* p2)
{
	return p1->GetServerNameObj() < p2->GetServerNameObj();
}

class CServerNameMap
{
protected:
	typedef					vector<StServerDetail*>	SERVER_LIST;
	SERVER_LIST				m_vtServers;
public:
	StServerDetail* GetServerDetailByName(const StServerName & name)
	{
		char buff[sizeof(StServerDetail)+100];
		StServerDetail *temp = (StServerDetail *)buff;
		temp->SetServerNameObj(name);
		SERVER_LIST::iterator it = 
			std::lower_bound(m_vtServers.begin(),m_vtServers.end(),temp,LessServerDetailPointer);
		if ( it == m_vtServers.end() )
			return NULL;
		if ( name != (*it)->GetServerNameObj() )
			return NULL;
		else
			return *it;
	}
	void InsertServerDetail(StServerDetail*p)
	{
		assert(GetServerDetailByName(p->GetServerNameObj()) == NULL);
		m_vtServers.push_back(p);
		std::sort(m_vtServers.begin(),m_vtServers.end(),LessServerDetailPointer);
	}

	void RemoveServerDetail(StServerDetail*p)
	{
		SERVER_LIST::iterator it = find(m_vtServers.begin(),m_vtServers.end(),p);
		if ( it != m_vtServers.end() )
			m_vtServers.erase(it);
		else
			RelTrace("RemoveServerDetail(): not added to server vector???");
	}

	size_t	GetServerCount()	const	{	return m_vtServers.size();	}
};

#endif
