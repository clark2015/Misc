#ifndef _I_MMO_MODULE_H
#define _I_MMO_MODULE_H

#include "../commInterfaceDef.h"
#include "IGameObject.h"
#include "typedef.h"
#include "SubScribeMsg.h"

struct IMmoModuleSlave
{
	enum	{	VERSION = 2	};
	virtual uint32			mmoModule_OnGetInterfaceVersion()	const = 0;

	virtual IGamePlayer*	GetPlayerByName		(const char*p)			= 0;
	virtual IGamePlayer*	GetPlayerByActorID	(uint64		ullActorID,bool bIncludeNotLogined = false)	= 0;
	virtual IGamePlayer*	GetPlayerByAccount	(const char*pPlatform,const char*pAccountName)			= 0;	//未实现，在合服上，这个函数意义不明
	virtual IGamePlayer*	GetPlayerByUserID	(uint32		ulUserID)	= 0;
	virtual IGameObject*	GetObjectByGUID		(uint64		ullGuid)	= 0;
	virtual void			GetPlayerByNameFuzzy(const char*pName,IGamePlayer** thearray,uint32& count,const IGamePlayer*pExcept = NULL)	const = 0;	//模糊查找

	virtual void			ToFirstPlayer(uint32 & pos)	const	= 0;
	virtual IGamePlayer*	GetNextPlayer(uint32 & pos)	const	= 0;	

	virtual void			SendToAllPlayer		(const void *pBuff,int nLen,uint32 ulReserved = 0)	= 0;

	virtual void			SubstribePlayerMsg	(uchar  ucMainCmd,				 struct ISubscribeSink*pSink)	= 0;	//参见 MSG_PLAYER_MSG 消息
	virtual void			SubstribePlayerMsg	(uchar  ucMainCmd,uchar ucSubCmd,struct ISubscribeSink*pSink)	= 0;	//参见 MSG_PLAYER_MSG 消息
	virtual void			SubstribeServerMsg	(ushort usCmd,					 struct ISubscribeSink*pSink)	= 0;	//参见 MSG_SERVER_MSG 消息
	//下面这几个的pSink参数现在是验证用的，也方便以后可能需要多个模块订阅
	virtual void			UnSubstribePlayerMsg(uchar  ucMainCmd,				 struct ISubscribeSink*pSink)	= 0;	//暂时是空的，没什么意义且很复杂
	virtual void			UnSubstribePlayerMsg(uchar  ucMainCmd,uchar ucSubCmd,struct ISubscribeSink*pSink)	= 0;	//暂时是空的，没什么意义且很复杂
	virtual void			UnSubstribeServerMsg(ushort usCmd,					 struct ISubscribeSink*pSink)	= 0;

	virtual uint32			NewSessionID()	= 0;	//在本服务器生存期间唯一的ID，实际上就是一个自增的内存ID
	virtual uint32			QueryOwnerInfoByName(int nDSIndex,ushort usOwnerType,const char*pName,uint32 ulQueryID) = 0;//返回sessionID,结果在MSG_QUERY_OWNER_ID_BY_NAME中返回	////加到IDBCache?

	//跟文件有关的操作，最好放进这个异步队列里，以免自己创建线程
	virtual void			AddFileAsynObj(class IAsynObj*pObj,bool bHead=false)						= 0;

	//bBinaryOutput = true，则pDes需要16字节，否则需要32+1字节
	virtual char*			SimpleMD5(const void *pSrc,int nCount,void*pDes,bool bBinaryOutput = false) = 0;
};

struct StMmoModuleParam
{
	enum	{	VERSION	= 1		};

	uint32									ulVersion;
	const struct	StMmoServerInitParam*	pInitParam;
	struct			IMmoServerSlave*		pServerSlave;
	class			IStageSlave*			pStageSlave;
	struct			ISubscribeMagr*			pSubscribeMagr;		//全局的消息订阅器
	class			ITimeLine*				pTimer;				//时间轴

	//下面是各模块的指针,以后有需要时可以增加
	struct			IMmoModuleSlave*		pMmoSlave;
	struct			IDBCache2*				pDBCache;			//数据库缓存
	struct			IGameMapMagr*			pGameMapMagr;		//地图模块
	struct			IInstanceMagr*			pInstaceMagr;		//副本管理模块
	struct			IChatModule*			pChat;				//聊天模块
	struct			IKeyWordFilter*			pKeyWordFilter;		//关健词过滤
	struct			ILuaPublic*				pLuaPublic;			//一些Lua公用函数
	struct			ICrossServerSubscribe*	pCrossSubscribe;	//跨服务器订阅
	struct			ISocialityServer*		pSocialityServer;	//社交服务器
	struct			IGameLog*				pGameLog;			//运营商要的各种接口
	void*									Reserved[5];		//全填0,方便日后增加
//	bool	IsInited()		const	{	return pInitParam && pServerSlave && pStageSlave && pSubscribeMagr	&& pTimer && pMmoSlave && pDBCache;	}
};

template <class _Interface,uint32 ulMainVer,uint32 ulMinVer,uint32 ulBuildVer>
class CMmoModuleLoader
{
	typedef _Interface * (*_CreateInterface)(uint32 ulMainVer1,uint32 ulMinVer1,uint32 ulBuildVer1,StMmoModuleParam*pParam,void*pParam1);

	_Interface*			m_pInterface;
	HMODULE 			m_hMod;
protected:
	virtual const char *GetDllFileName()const = 0;
	virtual const char *GetProcName()	const {	return "CreateMmoModule";	}
public:
	CMmoModuleLoader()
	{
		m_hMod   	 = 0;
		m_pInterface = 0;
	}
	~CMmoModuleLoader()
	{
		Clean();
	}
	void Clean()
	{
		if ( m_pInterface )
		{
			m_pInterface->Release();
			m_pInterface = 0;
		}
		if ( m_hMod )
		{
			FreeLibrary(m_hMod);
			m_hMod = 0;
		} 
	}

	bool LoadModule()
	{
		if ( !m_hMod )
			m_hMod = LoadLibrary(GetDllFileName());
		return 0 != m_hMod;
	}
	bool Create(StMmoModuleParam*pParam,void*pParam1 = 0,char szErrorMsg[] = 0 ,int nLen = 0)
	{
		Clean();
		if ( szErrorMsg && nLen > 0 )
			szErrorMsg[0] = 0;

		m_hMod = LoadLibrary(GetDllFileName());
		if ( 0 != m_hMod )
		{
			_CreateInterface func = (_CreateInterface )GetProcAddress(m_hMod,GetProcName());
			if ( 0 != func )
			{
				m_pInterface = func(ulMainVer,ulMinVer,ulBuildVer,pParam,pParam1);
				
				if ( NULL == m_pInterface )
				{
					if ( szErrorMsg && nLen > 0 )
						lstrcpyn(szErrorMsg,"creator function return NULL",nLen);
				}
			}
			else
			{
				if ( szErrorMsg && nLen > 0 )
					lstrcpyn(szErrorMsg,"can not get creator function",nLen);
			}
		}
		else
		{
			if ( szErrorMsg && nLen > 0 )
			{
				lstrcpyn(szErrorMsg,"can not LoadDLL",nLen);
				if ( nLen > 200 )
			#ifdef _WIN32
					wsprintf(szErrorMsg,"can not LoadDLL,GetLastError()=%u",GetLastError());
			#else
					wsprintf(szErrorMsg,"can not LoadDLL,dlerror()=%s",dlerror());
			#endif
			}
		}
		if ( 0 == m_pInterface )
			Clean();
		return 0 != m_pInterface;			
		}
		  _Interface *GetInterface()		{	return m_pInterface;		}
	const _Interface *GetInterface() const	{	return m_pInterface;		}
		  _Interface *operator->()			{	return m_pInterface;		}
	const _Interface *operator->()	 const	{	return m_pInterface;		}
	bool  IsValid()	const					{	return 0 != m_pInterface;	}
};

#endif

