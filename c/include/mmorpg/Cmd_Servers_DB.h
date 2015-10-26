#ifndef CMD_SERVERS_DB_H
#define CMD_SERVERS_DB_H
//数据库服务器的消息结构

#pragma pack(push,1)
//#ifdef _WIN32
//	#pragma warning(disable:4200)
//#endif

#define CDKEY_LEN	21	//包括结尾的0
struct StDBServer_CDKey_Get
{
	uint64		ullActorID;
	uint32		ulServerID;
	uint32		ulItemID;
	char		szAccountName[ACCOUNT_NAME_LEN];
	char		szPlatform[PLATFORM_NAME_LEN];
	char		szActorName[ACTOR_NAME_LEN];
	char		szCDKey[CDKEY_LEN];
};
struct StDBServer_CDKey_Result
{
	StDBServer_CDKey_Get get;
	uint16		usCode;
	char		szMsg[1];
};

struct StDBServer_RecvHttpProc
{
	uint32		ulMsg;		//这几个参数会写回回调消息里
	uint64		ullEcho0;	//
	uint64		ullEcho1;	//
	uint64		ullEcho2;	//
	char		szHost[64];
	uint16		usPort;
	uint16		usParamCount;
	uint16		usLen;
	char		szUrlAndParam[1];	//最后的请求类似于 http://$szHost:$usPort$szUrlAndParam
	uint32 GetSize() const	{	return sizeof(*this) + usLen -	1;	}
	void Init(const char*pHost,ushort usPort,const char*pUrl,uint32 ulMsg,uint64 ullEcho0,uint64 ullEcho1,uint64 ullEcho2)
	{
        this->ulMsg     = ulMsg;
		this->ullEcho0	= ullEcho0;
		this->ullEcho1	= ullEcho1;
        this->ullEcho2  = ullEcho2;
		this->usPort	= usPort;
		usParamCount = 0;
		usLen = strlen(pUrl) + 1;
		lstrcpyn(szUrlAndParam,	pUrl,	usLen);
		lstrcpyn(szHost,		pHost,	sizeof(szHost));
	}
	void AddParam(const char*pName,const char*pValue)
	{
		usParamCount ++;
		uint16 n = strlen(pName) + 1;
		lstrcpyn(szUrlAndParam + usLen,pName,n);
		usLen += n;
		n = strlen(pValue) + 1;
		lstrcpyn(szUrlAndParam + usLen,pValue,n);
		usLen += n;
	}
	void AddParam(const char*pName,int n)
	{
		char szBuff[32];
		dl_itoa(n,szBuff,10);
		AddParam(pName,szBuff);
	}
	void AddParam(const char*pName,uint n)
	{
		char szBuff[32];
		dl_ultoa(n,szBuff,10);
		AddParam(pName,szBuff);
	}
	void AddParam(const char*pName,uint64 n)
	{
		char szBuff[32];
		dl_ui64toa(n,szBuff,10);
		AddParam(pName,szBuff);
	}
};

struct StDBServer_SendHttpProc
{
	uint32		ulMsg;
	uint64		ullEcho0;
	uint64		ullEcho1;
	uint64		ullEcho2;
	bool		bSuccess;
	char		szErrorMsg[32];
	char		szResult[1];
};

struct StDBServer_Sync
{
	uint64		ullReserved;
	uint16		usReserved;
};

struct StDBServer_NewGoodIDRange
{
	uint32		uiServerID_;
	uint32		uiProcessServerID;
	uint64		ullStartGoodID;
	uint32		uiCount;
};

struct StDBServer_Logout
{
	uint64		ullActorID;
	uint32		ulLoginID;
};

#pragma pack(pop)

#endif
