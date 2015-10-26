#ifndef CMD_DISPATCH_SERVER_H
#define CMD_DISPATCH_SERVER_H

enum
{
	DISPATCH_SERVER_SEND_HEARTBEAT		= 1,		//����
	DISPATCH_SERVER_SEND_HEARTBEAT_2	= 2,		//����
	DISPATCH_SERVER_SEND_LOGIN_RESULT	= 3,		//��������¼���
	DISPATCH_SERVER_RECV_LOGIN			= 50,		//��¼

	DISPATCH_SERVER_SEND_USER_ENTER		= 100,
	DISPATCH_SERVER_SEND_USER_LEAVE		= 101,
	DISPATCH_SERVER_SEND_USER_DATA		= 102,
	DISPATCH_SERVER_SEND_SERVER_READY	= 103,		//ͨ����������������
	DISPATCH_SERVER_SEND_SERVER_DATA	= 104,
	DISPATCH_SERVER_SEND_SERVER_RUN		= 105,		//��������������
	DISPATCH_SERVER_SEND_RAND_SEED		= 106,		//(��)��ʱ�������������
	DISPATCH_SERVER_RECV_USER_KICK		= 151,
	DISPATCH_SERVER_RECV_USER_DATA		= 152,
	DISPATCH_SERVER_RECV_SERVER_DATA	= 153,		//ת������������
	DISPATCH_SERVER_RECV_TRANSMIT_USER	= 154,		//�����û�������������
	DISPATCH_SERVER_RECV_SERVER_RUN_RET	= 155,

	DISPATCH_SERVER_PING				= 200,
	DISPATCH_SERVER_PING_ANS			= 201,

	DISPATCH_SERVER_FOR_REPLAY			= 225,		//Ϊ¼����Ʊ���

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
	uint32			ulDispServerRecvTime1;	//�⼸�������ף�timeGetTime()��ͬ�����У���ȡ�Ŀ��ܱ���ȡ�Ļ�С
	uint32			ulRecvTime;				//
	uint32			ulDispServerRecvTime2;	//
};

struct StDispatchServer_TransmitUser
{
	uint32			ulUserID;
	StServerName	ServerName;	//Ŀ�������
	uint8			ucAlign;
//	uint64			ullActorID;
	char			Buff[1];
};

struct StDispatchServer_ServerData
{
	uint32			ulServerType_Send;	//����ʱû�ã�����ʱ�õ����ͷ������ı�־
	uint32			ulServerType_Recv;	//������Ϊ0����ʹ��ServerName������ServerNameֻ��һ����Ч
	StServerName	ServerName;			//
	uint8			ucAlign;
	uint16			usCmd;
	uint16			usBigDataOrder;		//���ʹ�����ʱ��˳��ţ���Ϊ�����1-65535,�������ݿɴ�249M
	uint32			ulBigDataLen;		//�����ݳ���
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
	uint16	usCount;	//�û�ID��
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
	uint32			ulTime;			//time(NULL)��ֵ��˫����ʱ�䲻������5����
	StServerName	ServerName;		//����ڵ���������԰�������ĵ�ͼ���ַ�����
	char			align;
	char			szPasswd[34];	//���Ǹĳ� md5(DSPSERVER_DEFAULT_PASS + ulTime)
};
#define DSPSERVER_DEFAULT_PASS	"G*&^$#R*&^DS#r4tt&Y&Y#H83@&%823"

struct StDispatchServer_CmdLineLogin
{
	enum
	{
		VERSION		= 1,
		MAGIC_NO	= 0xa8433ebc,

		//usFlags�ֶε�ֵ
		FLAGS_NEED_RESULT	= 1,	//��Ҫ���ؽ��
		FLAGS_NO_NET_CACHE	= 2,	//�����Ƿ���Ҫ���ݷ��ͻ�������⣬���ڲ�����Ӧ�ٶȣ���FLAGS_NEED_RESULT������ʱ��������
	};
	uint32			ulMagicNO;
	uint16			usVersion;
	uint16			usFlags;						//0=����Ҫ���ؽ����1=��Ҫ���ؽ��
	uint32			ulTime;							//time(NULL)��ֵ��˫����ʱ�䲻������5����
	char			szWorldName[WORLD_NAME_LEN];	//��֤��
	char			szPasswd[34];					//���Ǹĳ� md5(DSPSERVER_CMDLINE_PASS + ulTime)
	char			szServerName[SERVER_NAME_LEN];	//��������,��ds,db,gs,sc
	char			szCmdLine[1];

	bool IsNeedResult()	const	{	return !!(FLAGS_NEED_RESULT & usFlags);			}
	bool IsNoNetCache()	const	{	return !!(FLAGS_NO_NET_CACHE & usFlags);		}
	bool FlagsOK()		const	{	return 0 == (usFlags & (~(FLAGS_NEED_RESULT|FLAGS_NO_NET_CACHE)));	}	//��û�ж���ı�־
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
	uint16	usSecondPreSeed;	//��Ŷ�����ᷢ��һ������
	uint16	usSeedCount;
	uint32	ulRandSeed[1];

	ulong	Size()	const	{	return sizeof(*this) +	(usSeedCount) * sizeof(ulong) - sizeof(ulong);	 }
};

//������
struct StDispatchServer_HeartBeat2
{
	uint16	usCount;
};

#pragma pack(pop)

//����Ӧ�÷����ȥ...
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
		RelTrace("DsIndex=%d,Cmd=%s(%u),Len=%u,%sbuff=%s",nDSIndex,DispatchServerCmdStr(ucCmd),ucCmd,usLen,usLen>nMaxTraceBuff?"����̫��":"",szLastBuff);
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
	SERVER_TYPE			m_ServerType;	//�ڷַ����������������ң�����SERVER_TYPE_PLAYER
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
