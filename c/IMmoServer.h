#ifndef IMMOSERVER_H
#define IMMOSERVER_H
#include "../ILuaWrap.h"
#include <time.h>
#include "../IUnknownEx.h"
#include "mmorpgdefs.h"
#include "../IByteArray.h"
class IStageSink;

enum
{
	BROKEN_REASON_TRANSMIT				= 3,	//TransmitUser,这时必须更新数据库
												//其它的值在 BROKEN_REASON

	//调用FreeBrokenUser的原因
	FREE_USER_REASON_NONE				= 0,
	FREE_USER_REASON_BROKEN_TIMEOUT		= 1,	//断线太久
	FREE_USER_REASON_KICK_LEAVE			= 2,	//KickLeave命令或shutdown命令,即KickUser()之后直接调用FreeBrokenUser()
	FREE_USER_REASON_TRANSMIT			= 3,	//调用mmoss_TransmitUser()传送到服务器
	FREE_USER_REASON_TRANSMIT_BROKEN	= 4,	//对断线用户调用了传送接口

	DS_INDEX_ALL						= -2	//暂时放这里了
};

//服务器助手
struct IMmoServerSlave : public IUnknownEx2
{
	enum	{	VERSION = 2	};
	virtual uint32		mmoss_GetInterfaceVersion()			const = 0;

	//这里取得的时间可能比真实时间略慢(多数情况下不会超过1秒),但这样做使得重放时能与真实运行时一样
	virtual time_t		mmoss_time()										const = 0;	//相当于time()调用,多线程安全
	virtual struct tm&	mmoss_localtime(struct tm & result,uint32*pMS = 0)	const = 0;	//相当于localtime()或localtime_r(),多线程安全

	//发送消息到其它服务器,usCmd定义在 Cmd_Server.h,服务器名字和类型定义在 mmorpgdefs.h
	virtual	void		mmoss_SendToServer(ushort usCmd,  const void*pBuff,int nLen,const StServerName& ToServer,int nDSIndex)	= 0;
	virtual	void		mmoss_SendToServer(ushort usCmd,  const void*pBuff,int nLen,uint32 serverType,			 int nDSIndex)	= 0;

	//传送用户到其它服务器,OnUserBroken()将会被调用。如果玩家已断线，则代为调用FreeBrokenUser(FREE_USER_REASON_TRANSMIT_BROKEN)
	//发送、传送失败:
	//   如果是因为服务器网络之间断开了，会所有人都断线，不需要处理
	//   如果是因为其它原因，是不是需要一个返回？？(不行，因为可能是目标服务器已经不在，无法得到这个结果)
	virtual	void		mmoss_TransmitUser(uint32 ulUserID,const void*pBuff,int nLen,const StServerName& ToServer)	= 0;	//int nDSIndex 不需要，因为根据用户ID可以查到这个信息了

	//将返回 IMmoServerSink::mmos_OnGetServerType() 的结果,有些公用模块可能需要知道这个
	virtual uint32		mmoss_GetServerType()							const = 0;	

	//服务器是否在线(要搜索,不是多线程安全的)
	virtual bool		mmoss_IsServerReady(const StServerName& Server,int nDSIndex)	const = 0;

	//随机数
	virtual uint32		mmoss_Rand32()	= 0;	//返回0-0xFFFFFFFF
	inline  uint32			  Rand32()	{	return mmoss_Rand32();	}
	virtual int			mmoss_Rand()	= 0;	//返回0-0x7FFF(即vc的RAND_MAX,跟gcc的RAND_MAX不同)
	inline  int				  Rand()	{	return mmoss_Rand();	}

	//录象重放时断下来以便输入命令,正常运行时此调用会被忽略
	virtual void		mmoss_ReplayBreakPoint(const char*pDesc = 0,const char*pFile = 0,int nLine= 0)	= 0;

	//调用 mmos_OnRunCommand(int nDSIndex=-1,int argc,char*argv[],bool &bProced);
	virtual void		mmoss_RunCommand(const char*pCmd) = 0;

	//bLock=true时，这个操作是多线程安全的。
	//实际上有两个list,一个加锁，一个不加。不加的速度应该会快一些
	//运行时会先取不加锁的list执行，再取有加锁的
	virtual void		mmoss_AddAsynObj(class IAsynObj*pObj,bool bLock)	= 0;
	virtual void		mmoss_ClearAsynObj()					= 0;

	//true=正在重放，false=正式运行
	virtual bool		mmoss_IsReplaying()	const				= 0;

	//这是返回给命令发送者的字符串,建议使用的格式："0,Okey"
	virtual void		mmoss_SetRunCommandResult(const char*p)	= 0;

	//发送命令到其它没有与我的中心服连接的服务器
	virtual void		mmoss_SendCommandLine(const char*pIP,ushort usPort,const char*pServerName,const char*pCmd)	= 0;

	//注意录像重放时，都会失败。注意不要不要让这个函数影响录像功能
	virtual bool		mmoss_Connect(const char*pIpAddr,ushort usPort,class IServerSocketClientSink*pSink)			= 0;

	//用于用网络数据建立一个IByteArray，可以传给Lua用
	virtual IByteArray*	mmoss_CreateByteArray(void *pData,uint32 ulLength,uint32 ulStorage,void *pPlacementNew = 0,uint32 ulPlacementNewLen = 0,const char*pName = 0)	const = 0;

	enum{
		_RAND_MAX = 0x7FFF		//gcc的RAND_MAX跟vc的不同，这里用vc的值
	};
};

#define ReplayBreakPoint()	mmoss_ReplayBreakPoint(NULL,__FILE__,__LINE__)

//服务器回调接口
struct IMmoServerSink
{
	enum	{	VERSION = 1	};

	virtual uint32		mmos_OnGetInterfaceVersion()	const = 0;
	virtual uint32		mmos_OnGetServerType()			const = 0;
	virtual IStageSink* mmos_OnGetStageSink()				  = 0;

	//初始化
	virtual bool		mmos_OnInit(struct IMmoServerSlave*pSlave,const struct StMmoServerInitParam*pParam)	= 0;

	//执行手工输入的命令
	virtual void		mmos_OnRunCommand(int nDSIndex,int argc,char*argv[],bool &bProced)					= 0;

	//某个服务器连上分发服务器了
	virtual void		mmos_OnServerReady(const StServerInfo& Server,bool bReady)							= 0;

	//其它服务器发来的消息
	virtual void		mmos_OnServerMsg(const StServerInfo& FromServer,ushort usCmd,char *pBuff,int nLen)	= 0;
};

struct StMmoServerInitParam
{
	enum	{	VERSION = 2	};

	uint32				ulVersion;
	char				szInitParam[256];	//可以放配置文件路径
	char				szWorldName[WORLD_NAME_LEN];
	char				szServerName[SERVER_NAME_LEN];
	ushort				usMaxUserCount;
	ILuaWrapper*		pLuaWrapper;
	ushort				usDSIndex0Port;
	int					nDSCount;		//注意这个值是会增加的
	char				szLogDir[128];
	char				szExtendInfo[256];
};

inline const char*FreeUserReasonStr(uint32 ulFreeReason)
{
	switch (ulFreeReason)
	{
	case FREE_USER_REASON_NONE				:	return "FREE_USER_REASON_NONE";			
	case FREE_USER_REASON_BROKEN_TIMEOUT	:	return "FREE_USER_REASON_BROKEN_TIMEOUT";	
	case FREE_USER_REASON_KICK_LEAVE		:	return "FREE_USER_REASON_KICK_LEAVE";		
	case FREE_USER_REASON_TRANSMIT			:	return "FREE_USER_REASON_TRANSMIT";
	case FREE_USER_REASON_TRANSMIT_BROKEN	:	return "FREE_USER_REASON_TRANSMIT_BROKEN";	
	default									:	return "未知";
	}
}
//*****************************************************************************************

enum
{
	MMOSERVER_VER_MAIN	= 1,
	MMOSERVER_VER_SUB	= 0,
	MMOSERVER_VER_BUILD	= 0
};

class CMmoServerLogicLoader
{
	typedef IMmoServerSink* (*_GetMmoServerLogic)(uint32 ulVerMain,uint32 ulVerMin,uint32 ulVerBuild,const char*pServerName);
	IMmoServerSink*		m_pServerSink;
	IStageSink*			m_pStageSink;
	uint32				m_ulServerType;
	HMODULE				m_hMod;
	char				m_szError[128];

public:	
	CMmoServerLogicLoader()
	{
		m_hMod				= 0;
		m_pServerSink		= 0;
		m_pStageSink		= 0;
		m_ulServerType		= 0;
		m_szError[0]		= 0;
	}
	~CMmoServerLogicLoader()
	{
		Clean();
	}
	const char*GetError()	const {	return m_szError;	}
	void Clean()
	{
		m_pServerSink		= 0;
		m_pStageSink		= 0;
		if ( m_hMod )
		{
			FreeLibrary(m_hMod);
			m_hMod = 0;
		} 
	}

	bool Create(const char*pDllName,const char*pServerName)
	{
		Clean();
		m_szError[0] = 0;
		if ( ! pDllName || strlen(pDllName) > 100)
		{
			lstrcpyn(m_szError,"Create() : dll name is null or too long",sizeof(m_szError));
			return false;
		}
			
		m_hMod = LoadLibrary(pDllName);
		if ( 0 != m_hMod )
		{
			_GetMmoServerLogic func = (_GetMmoServerLogic )GetProcAddress(m_hMod,"GetMmoServerLogic");
			if ( 0 != func )
			{
				m_pServerSink = func(MMOSERVER_VER_MAIN,MMOSERVER_VER_SUB,MMOSERVER_VER_BUILD,pServerName);
				if ( ! m_pServerSink )
					lstrcpyn(m_szError,"GetMmoServerLogic return null",sizeof(m_szError));
			}
			else
			{
				lstrcpyn(m_szError,"can not get function GetMmoServerLogic()",sizeof(m_szError));
			}
		}
		else
		{
		#ifdef _WIN32
			wsprintf(m_szError,"can not Load dll:%s,GetLastError()=%u",pDllName,GetLastError());
		#else
			wsprintf(m_szError,"can not Load dll:%s,dlerror()=%s",pDllName,dlerror());
		#endif
		}
		if ( 0 == m_pServerSink )
		{
			Clean();
		}
		else
		{
			m_pStageSink	= m_pServerSink->mmos_OnGetStageSink();
			m_ulServerType	= m_pServerSink->mmos_OnGetServerType();
		}
		return 0 != m_pServerSink;			
	}

	IStageSink*			GetStageLogic()				{	return m_pStageSink;	}
	IMmoServerSink*		GetServerLogic()			{	return m_pServerSink;	}
	uint32				GetServerType()		const	{	return m_ulServerType;	}
};
//*****************************************************************************************
class CTempByteArray
{
protected:
	char m_buffObj [56];
	IByteArray*m_pArray;
public:
	CTempByteArray(IMmoServerSlave*pSlave,void *pData,uint32 ulLength,uint32 ulStorage,const char*pName = NULL)		{	m_pArray = pSlave->mmoss_CreateByteArray(pData,ulLength,ulStorage,&m_buffObj,sizeof(m_buffObj),pName);	}
	~CTempByteArray()			{	m_pArray->Release();	}

	IByteArray*GetByteArray()	{	return m_pArray;		}
	IByteArray*operator -> ()	{	return m_pArray;		}
};

class CTempByteArrayAllocData : public CTempByteArray
{
protected:
	char m_buffData[4000];
public:
	CTempByteArrayAllocData(IMmoServerSlave*pSlave,const char*pName = NULL) : CTempByteArray(pSlave,m_buffData,0,sizeof(m_buffData),pName)
	{
	}
};
//*****************************************************************************************

#endif
