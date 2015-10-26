#ifndef _ISTAGE_DRIVER_H__
#define _ISTAGE_DRIVER_H__


/*
2005-04-25/26	��袳�			�����ļ�
								��ɻ�������,������
2005-04-27/28	      			����ͨ��,������һЩbug,���˼�������,�޸�AddUser()�Ĳ���
2005-05-07		      			IsDataCorrect()��LogStageState()��const����
2005-11-20		      			����û���5000->25000
								���ⲿidӰ����map��Ϊhash_map,���ܻ��Щ..
								ToFirst()��GetNext()��const����(�������岻��..)
2006-02-05/06	      			Send()��SendMsg()��pHead����
								����SendIfFlag()�����������Send()
								����GetUserFlag()/SetUserFlag()
								GetNext()��FlagMask����������û�����
2006-05-24/25	      	v3.0.0	���ӶԶ���/�����֧��
2006-05-25		      	v3.0.0	���Ӷ�DbID���ü���ѯ��֧��
2006-06-07		      	v3.0.1	KickUser()����û��Ƿ��Ѿ�����
2006-06-12		      	v4.0.0	OnInit()����pInitPraram����
								OnInit()����boolֵ
2006-07-11		      	v5.0.0	����IStageDrver::OnLeave() ->IStageDriver::OnKick()
								KickUser()�͸���IStageDrver::OnKick()����ulDelayMs����
2006-07-21		      	v5.0.1	�޸�Replay()û���޸Ķ��߱�־������
2006-08-04		      	v5.1.0  �޸Ľӿ�����	
									IStageDrver::SendMsg()	-> IStageDrver::DoSendMsg()
									IStageDrver::OnKick ()	-> IStageDrver::DoKick ()
2006-09-14		      	v6.0.0	����IStageSink::OnClose(),IStage::CloseStage(),IStageSlave::GetStageState()
2006-09-19		      	v6.0.1	Send()��һ��������û�ʱ�����Ԫ��Ϊ-1,��trace����
2006-10-13		      	v6.0.2	�µ�IAsynProc
2006-11-02		      	v7.0.0	����ӿ�
2006-11-16		      	v8.0.0	ȥ��OnUserBroken()�ķ���ֵ�������Լ�����FreeBroken()
2006-12-21		      	v9.0.0	����GetDbIDMapSize()
								����ӿ�
								SendIfFlag() Send() DoSendMsg()��pHead����ȥ��,�������˳��
								DoSendMsg() -> DoSend()
2007-01-04		      	v9.1.0	��������ͳ��
2007-02-09		      	v10.0.0	�������ߺ�GetUserStageData()û�з���NULL��bug
								����EndCacheSend()��BeginCacheSend()��DoSendV()�Ľӿ�
2007-03-02		      	v10.0.1	�������û�ʱCache�ķ�����������Ŀû����0��bug
2007-06-11		      	v11.0.0	����OnInitSuccess(),ȥ��ChangeSink()��ChangeSlave()
2007-06-13		      	v12.0.0	GetFluxIn/Out(),����CULong64
2008-03-11		      	v12.1.0	����RandL();
2008-05-20		      	v13.0.0	����GetNext2()
2010-08-19		      	v14.0.0	GetFluxIn/Out(),����ulonglong
								OnUserActive(),UserActive()��buffȥ��const����
2010-12-11		      	v15.0.0	���� IsOuterIDExist();
								SendV()�ĳ��������const	
								DoKick()��Reason����
2011-03-08��11	      	v15.0.1 ����GetUserAddress(),����DelAllUser()
								����ʱ������������ӣ������ڲ�������ϵͳ״̬
2011-08-25��27	      	v16.0.0 ����IsInnerIDExist()
								����GetOuterIDByInnerID()/GetInnerIDByOuterID()
								����SetUserAccountID()��֧�ּ�¼�ʺ�ID
								����SetActorID()     ��֧�ּ�¼�ʺ�ID
								AddUser()/OnUserEnter()����pBuff����
2011-09-08��09		    v16.0.1 ��IStage.h�з�������ļ�
								�ӿڸ���IStageSlave->IStageDriver,IStageSinkSlave->IStageSlave
								����GetStageSlave()
								��д��˵��
2013-09-11						����GetOption()
								����ShortRoundID==trueʱuserid����Ϊ0��bug
2013-10-13						DelAllUser()����nSourceID����	
2014-09-13						����RunCommand

Stage Driver��̨����:      
	IStageDriver:Stage�����ˣ�����Ϣ������ӿ�������
	IStage:ʵ��һ��Stage�������û�������������Ϣ����ʱ��������֪ͨIStageSink
������Stage�ǿ���Ƕ�׵ġ�����һ��������һ��Stage��������һ���齫̨Ҳ��һ��Stage
*/

//**********************************************************************
class IStageSlave;
class IStageSink;

//����̨��������ʵ��
class IStageDriver
{
public:
	virtual void DoSend			 (uint32 ulOuterUID,const void *pBuff,		 int  nSize			   )	= 0;
	virtual void DoSendUserArray (				    const void *pBuff,		 int  nSize,const uint32 ulOuterUIDs[],uint16 usCount)	= 0;
	virtual void DoSendV		 (uint32 ulOuterUID,const void *pBuff[],const int pnSize[],int nCount)	= 0;
	virtual void DoKick			 (uint32 ulOuterUID,uint32 ulDelayMs,	const char*pReason)					= 0;
};

class IStage
{
public:
	virtual void	Release()		= 0;

	//�ر�
	virtual void	CloseStage(uint32 ulReserved = 0)	= 0;

	//���º�������̨�������ߵ���
	//���˽�����̨
	//Լ�����addr.usSourceID�в�ֵͬ�Ļ�������(ulOuterUID>>24)==addr.usSourceID
	virtual uint32 AddUser(uint32 ulOuterUID,const StStageAddr & addr,const void* pStageUserData,void *pBuff,int nLen)	= 0;
	//�����˳���̨
	//��ص�IStageSink::OnUserBroken(),����������,���ص�IStageSlave::DoKick()
	virtual bool	DelUser(uint32 ulOuterUID,uint32 ulBrokenReason)		= 0;	
	virtual void	DelAllUser(int nSourceID)								= 0;	
	//��̨�����˷���Ϣ
	virtual void	UserActive(uint32 ulOuterUID,void *pBuff,int nSize)	= 0;

	//��ǰ��̨�ϵ�����
	virtual int		GetOnlineUserCount()	const = 0;	//���������ߵ�
	virtual int		GetBrokenUserCount()	const = 0;	//���ߵ�
	
	//ȡ��������
	virtual uint64	GetFluxIn (void *pReserved = NULL,void *pReserved1 = NULL)	const = 0;
	virtual uint64	GetFluxOut(void *pReserved = NULL,void *pReserved1 = NULL)	const = 0;

	virtual bool	IsOuterIDExist(uint32 ulOuterUID)	const = 0;
	virtual bool	IsInnerIDExist(uint32 ulInnerUID)	const = 0;	//ʵ������IStageSlave::IsUserExist()һ��

	//ID��ת
	virtual uint32	GetOuterIDByInnerID(uint32 ulInnerUID)	const = 0;
	virtual uint32	GetInnerIDByOuterID(uint32 ulOuterUID)	const = 0;

	//
	virtual IStageSlave*	GetStageSlave()	= 0;
	virtual IStageSink*		GetStageSink()	= 0;

	//ȡѡ��
	enum
	{
		OPTION_USER_ARRAY_SEND_BY_SELF	= 0,
	};
	virtual long	GetOption(uint32 opt)	const	= 0;
	virtual bool    RunCommand(int argc,char**argv)	= 0;

	enum
	{
	//	SOURCE_ID_MAX			= 255	//ֻ��һ���ֽڴ�С
		SHORT_ROUND_ZERO_BITS	= 12
	};
};

	//��ʾ״̬�������̨����������Ƿ���ȷ,�������ӦSink�Ķ�Ӧ����
//	virtual bool  TraceAndCheck() const = 0;
//**********************************************************************

enum
{
	STAGE_VER_MAIN	= 21,
	STAGE_VER_SUB	= 0,
	STAGE_VER_BUILD	= 4
};

#ifdef _DEBUG
	#define _ISTAGE_DLL_NAME	"./IStageD"DLL_EXT
#else
	#define _ISTAGE_DLL_NAME	"./IStage"DLL_EXT
#endif

class CIStageLoader
{
	typedef IStage * (*_CreateIStage)(uint32 ulVerMain,uint32 ulVerMin,uint32 ulVerBuild,
							//				IStageDriver *pDriver,IStageSink *pSink,uint16 usMaxUser,void*pInitParam,bool bUserArraySendBySelf);
											IStageDriver *pDriver,IStageSink *pSink,uint16 usMaxUser,uint32 randSeed,void*pInitParam,bool bUserArraySendBySelf,bool bShortRoundID);
	IStage* m_pStage;
	HMODULE m_hMod;

	public:	
	CIStageLoader()
	{
		m_hMod	 = 0;
		m_pStage = 0;
	}
	~CIStageLoader()
	{
		Clean();
	}
	void Clean()
	{
		if ( m_pStage )
		{
			m_pStage->Release();
			m_pStage = 0;
		}
		if ( m_hMod )
		{
			FreeLibrary(m_hMod);
			m_hMod = 0;
		} 
	}

	//pInitParam : ����IStageSink::OnStageInit()
	//bUserArraySendBySelf: ��Ҫ�����˷���ͬ����Ϣʱ���Լ���ε���IStageDriver::DoSend()���ǵ���IStageDriver::DoSendUserArray(),���߱�ǰ���������԰�����ѹ���ָ�����ģ�飨������ǰ�÷��������ַ���������
	bool Create(IStageDriver *pDriver,IStageSink *pSink,uint16 usMaxUser,uint32 randSeed,void *pInitParam,bool bUserArraySendBySelf = true,bool bShortRoundID = false)
	{
		Clean();
		if ( ! pDriver || !pSink )
			return false;
			
		m_hMod = LoadLibrary(_ISTAGE_DLL_NAME);
		if ( 0 != m_hMod )
		{
			_CreateIStage func = (_CreateIStage )GetProcAddress(m_hMod,"CreateIStage");
			if ( 0 != func )
				m_pStage = func(STAGE_VER_MAIN,STAGE_VER_SUB,STAGE_VER_BUILD,pDriver,pSink,usMaxUser,randSeed,pInitParam,bUserArraySendBySelf,bShortRoundID);
		}
		if ( 0 == m_pStage )
			Clean();
		return 0 != m_pStage;			
	}

		  IStage *GetInterface()		{	return m_pStage;		}
	const IStage *GetInterface() const	{	return m_pStage;		}
		  IStage *GetStage()			{	return m_pStage;		}
	const IStage *GetStage()	 const	{	return m_pStage;		}
		  IStage *operator->()			{	return m_pStage;		}
	bool  IsValid()				 const	{	return 0 != m_pStage;	}
};

#endif
