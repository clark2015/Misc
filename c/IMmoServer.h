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
	BROKEN_REASON_TRANSMIT				= 3,	//TransmitUser,��ʱ����������ݿ�
												//������ֵ�� BROKEN_REASON

	//����FreeBrokenUser��ԭ��
	FREE_USER_REASON_NONE				= 0,
	FREE_USER_REASON_BROKEN_TIMEOUT		= 1,	//����̫��
	FREE_USER_REASON_KICK_LEAVE			= 2,	//KickLeave�����shutdown����,��KickUser()֮��ֱ�ӵ���FreeBrokenUser()
	FREE_USER_REASON_TRANSMIT			= 3,	//����mmoss_TransmitUser()���͵�������
	FREE_USER_REASON_TRANSMIT_BROKEN	= 4,	//�Զ����û������˴��ͽӿ�

	DS_INDEX_ALL						= -2	//��ʱ��������
};

//����������
struct IMmoServerSlave : public IUnknownEx2
{
	enum	{	VERSION = 2	};
	virtual uint32		mmoss_GetInterfaceVersion()			const = 0;

	//����ȡ�õ�ʱ����ܱ���ʵʱ������(��������²��ᳬ��1��),��������ʹ���ط�ʱ������ʵ����ʱһ��
	virtual time_t		mmoss_time()										const = 0;	//�൱��time()����,���̰߳�ȫ
	virtual struct tm&	mmoss_localtime(struct tm & result,uint32*pMS = 0)	const = 0;	//�൱��localtime()��localtime_r(),���̰߳�ȫ

	//������Ϣ������������,usCmd������ Cmd_Server.h,���������ֺ����Ͷ����� mmorpgdefs.h
	virtual	void		mmoss_SendToServer(ushort usCmd,  const void*pBuff,int nLen,const StServerName& ToServer,int nDSIndex)	= 0;
	virtual	void		mmoss_SendToServer(ushort usCmd,  const void*pBuff,int nLen,uint32 serverType,			 int nDSIndex)	= 0;

	//�����û�������������,OnUserBroken()���ᱻ���á��������Ѷ��ߣ����Ϊ����FreeBrokenUser(FREE_USER_REASON_TRANSMIT_BROKEN)
	//���͡�����ʧ��:
	//   �������Ϊ����������֮��Ͽ��ˣ��������˶����ߣ�����Ҫ����
	//   �������Ϊ����ԭ���ǲ�����Ҫһ�����أ���(���У���Ϊ������Ŀ��������Ѿ����ڣ��޷��õ�������)
	virtual	void		mmoss_TransmitUser(uint32 ulUserID,const void*pBuff,int nLen,const StServerName& ToServer)	= 0;	//int nDSIndex ����Ҫ����Ϊ�����û�ID���Բ鵽�����Ϣ��

	//������ IMmoServerSink::mmos_OnGetServerType() �Ľ��,��Щ����ģ�������Ҫ֪�����
	virtual uint32		mmoss_GetServerType()							const = 0;	

	//�������Ƿ�����(Ҫ����,���Ƕ��̰߳�ȫ��)
	virtual bool		mmoss_IsServerReady(const StServerName& Server,int nDSIndex)	const = 0;

	//�����
	virtual uint32		mmoss_Rand32()	= 0;	//����0-0xFFFFFFFF
	inline  uint32			  Rand32()	{	return mmoss_Rand32();	}
	virtual int			mmoss_Rand()	= 0;	//����0-0x7FFF(��vc��RAND_MAX,��gcc��RAND_MAX��ͬ)
	inline  int				  Rand()	{	return mmoss_Rand();	}

	//¼���ط�ʱ�������Ա���������,��������ʱ�˵��ûᱻ����
	virtual void		mmoss_ReplayBreakPoint(const char*pDesc = 0,const char*pFile = 0,int nLine= 0)	= 0;

	//���� mmos_OnRunCommand(int nDSIndex=-1,int argc,char*argv[],bool &bProced);
	virtual void		mmoss_RunCommand(const char*pCmd) = 0;

	//bLock=trueʱ����������Ƕ��̰߳�ȫ�ġ�
	//ʵ����������list,һ��������һ�����ӡ����ӵ��ٶ�Ӧ�û��һЩ
	//����ʱ����ȡ��������listִ�У���ȡ�м�����
	virtual void		mmoss_AddAsynObj(class IAsynObj*pObj,bool bLock)	= 0;
	virtual void		mmoss_ClearAsynObj()					= 0;

	//true=�����طţ�false=��ʽ����
	virtual bool		mmoss_IsReplaying()	const				= 0;

	//���Ƿ��ظ�������ߵ��ַ���,����ʹ�õĸ�ʽ��"0,Okey"
	virtual void		mmoss_SetRunCommandResult(const char*p)	= 0;

	//�����������û�����ҵ����ķ����ӵķ�����
	virtual void		mmoss_SendCommandLine(const char*pIP,ushort usPort,const char*pServerName,const char*pCmd)	= 0;

	//ע��¼���ط�ʱ������ʧ�ܡ�ע�ⲻҪ��Ҫ���������Ӱ��¼����
	virtual bool		mmoss_Connect(const char*pIpAddr,ushort usPort,class IServerSocketClientSink*pSink)			= 0;

	//�������������ݽ���һ��IByteArray�����Դ���Lua��
	virtual IByteArray*	mmoss_CreateByteArray(void *pData,uint32 ulLength,uint32 ulStorage,void *pPlacementNew = 0,uint32 ulPlacementNewLen = 0,const char*pName = 0)	const = 0;

	enum{
		_RAND_MAX = 0x7FFF		//gcc��RAND_MAX��vc�Ĳ�ͬ��������vc��ֵ
	};
};

#define ReplayBreakPoint()	mmoss_ReplayBreakPoint(NULL,__FILE__,__LINE__)

//�������ص��ӿ�
struct IMmoServerSink
{
	enum	{	VERSION = 1	};

	virtual uint32		mmos_OnGetInterfaceVersion()	const = 0;
	virtual uint32		mmos_OnGetServerType()			const = 0;
	virtual IStageSink* mmos_OnGetStageSink()				  = 0;

	//��ʼ��
	virtual bool		mmos_OnInit(struct IMmoServerSlave*pSlave,const struct StMmoServerInitParam*pParam)	= 0;

	//ִ���ֹ����������
	virtual void		mmos_OnRunCommand(int nDSIndex,int argc,char*argv[],bool &bProced)					= 0;

	//ĳ�����������Ϸַ���������
	virtual void		mmos_OnServerReady(const StServerInfo& Server,bool bReady)							= 0;

	//������������������Ϣ
	virtual void		mmos_OnServerMsg(const StServerInfo& FromServer,ushort usCmd,char *pBuff,int nLen)	= 0;
};

struct StMmoServerInitParam
{
	enum	{	VERSION = 2	};

	uint32				ulVersion;
	char				szInitParam[256];	//���Է������ļ�·��
	char				szWorldName[WORLD_NAME_LEN];
	char				szServerName[SERVER_NAME_LEN];
	ushort				usMaxUserCount;
	ILuaWrapper*		pLuaWrapper;
	ushort				usDSIndex0Port;
	int					nDSCount;		//ע�����ֵ�ǻ����ӵ�
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
	default									:	return "δ֪";
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
