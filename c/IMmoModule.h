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
	virtual IGamePlayer*	GetPlayerByAccount	(const char*pPlatform,const char*pAccountName)			= 0;	//δʵ�֣��ںϷ��ϣ�����������岻��
	virtual IGamePlayer*	GetPlayerByUserID	(uint32		ulUserID)	= 0;
	virtual IGameObject*	GetObjectByGUID		(uint64		ullGuid)	= 0;
	virtual void			GetPlayerByNameFuzzy(const char*pName,IGamePlayer** thearray,uint32& count,const IGamePlayer*pExcept = NULL)	const = 0;	//ģ������

	virtual void			ToFirstPlayer(uint32 & pos)	const	= 0;
	virtual IGamePlayer*	GetNextPlayer(uint32 & pos)	const	= 0;	

	virtual void			SendToAllPlayer		(const void *pBuff,int nLen,uint32 ulReserved = 0)	= 0;

	virtual void			SubstribePlayerMsg	(uchar  ucMainCmd,				 struct ISubscribeSink*pSink)	= 0;	//�μ� MSG_PLAYER_MSG ��Ϣ
	virtual void			SubstribePlayerMsg	(uchar  ucMainCmd,uchar ucSubCmd,struct ISubscribeSink*pSink)	= 0;	//�μ� MSG_PLAYER_MSG ��Ϣ
	virtual void			SubstribeServerMsg	(ushort usCmd,					 struct ISubscribeSink*pSink)	= 0;	//�μ� MSG_SERVER_MSG ��Ϣ
	//�����⼸����pSink������������֤�õģ�Ҳ�����Ժ������Ҫ���ģ�鶩��
	virtual void			UnSubstribePlayerMsg(uchar  ucMainCmd,				 struct ISubscribeSink*pSink)	= 0;	//��ʱ�ǿյģ�ûʲô�����Һܸ���
	virtual void			UnSubstribePlayerMsg(uchar  ucMainCmd,uchar ucSubCmd,struct ISubscribeSink*pSink)	= 0;	//��ʱ�ǿյģ�ûʲô�����Һܸ���
	virtual void			UnSubstribeServerMsg(ushort usCmd,					 struct ISubscribeSink*pSink)	= 0;

	virtual uint32			NewSessionID()	= 0;	//�ڱ������������ڼ�Ψһ��ID��ʵ���Ͼ���һ���������ڴ�ID
	virtual uint32			QueryOwnerInfoByName(int nDSIndex,ushort usOwnerType,const char*pName,uint32 ulQueryID) = 0;//����sessionID,�����MSG_QUERY_OWNER_ID_BY_NAME�з���	////�ӵ�IDBCache?

	//���ļ��йصĲ�������÷Ž�����첽����������Լ������߳�
	virtual void			AddFileAsynObj(class IAsynObj*pObj,bool bHead=false)						= 0;

	//bBinaryOutput = true����pDes��Ҫ16�ֽڣ�������Ҫ32+1�ֽ�
	virtual char*			SimpleMD5(const void *pSrc,int nCount,void*pDes,bool bBinaryOutput = false) = 0;
};

struct StMmoModuleParam
{
	enum	{	VERSION	= 1		};

	uint32									ulVersion;
	const struct	StMmoServerInitParam*	pInitParam;
	struct			IMmoServerSlave*		pServerSlave;
	class			IStageSlave*			pStageSlave;
	struct			ISubscribeMagr*			pSubscribeMagr;		//ȫ�ֵ���Ϣ������
	class			ITimeLine*				pTimer;				//ʱ����

	//�����Ǹ�ģ���ָ��,�Ժ�����Ҫʱ��������
	struct			IMmoModuleSlave*		pMmoSlave;
	struct			IDBCache2*				pDBCache;			//���ݿ⻺��
	struct			IGameMapMagr*			pGameMapMagr;		//��ͼģ��
	struct			IInstanceMagr*			pInstaceMagr;		//��������ģ��
	struct			IChatModule*			pChat;				//����ģ��
	struct			IKeyWordFilter*			pKeyWordFilter;		//�ؽ��ʹ���
	struct			ILuaPublic*				pLuaPublic;			//һЩLua���ú���
	struct			ICrossServerSubscribe*	pCrossSubscribe;	//�����������
	struct			ISocialityServer*		pSocialityServer;	//�罻������
	struct			IGameLog*				pGameLog;			//��Ӫ��Ҫ�ĸ��ֽӿ�
	void*									Reserved[5];		//ȫ��0,�����պ�����
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

