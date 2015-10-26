#ifndef _INetStage__H__
#define _INetStage__H__
/*
2005-05-05/07	��袳�	         ʵ��INetStage
2005-07-10		      	1.1��    ʹ���µ�IServerSock(����ACE�İ汾)
2006-02-05		      	1.2��    ʹ���µ�IStage��IServerSock(��Send()��pHead�����İ汾)
2006-05-25		      	2.0��	 ʹ��֧�ֶ��ߵ�IStage
2006-06-09		      	2.0.1��  ����IStageSink��OnInit()�������ص�����ͬһ�̵߳�����
2006-06-09		      	3.0.0��  ʹ��OnInit()��pInitParam������IStage
								 BeginServer()��ΪOpenServer()
2006-07-11		      	4.0.0��  ֧���µ�IStage����OnKick()��delay����
2006-07-14		      	5.0.0��  ʹ���µ�IAsynQueue��ServerSocket�ӿ�
2006-08-04		      	5.0.1��  ʹ���µ�IStage��ʵ��CloseServer()
2006-09-14		      	6.0.0��  ʹ���µ�IStage
2006-09-19		      	6.0.1��
2006-09-22		      	6.1.0��  ����GetIStage()�ӿ�
								 sPort -> usPort
2006-10-13		      	6.1.1��  
2006-10-19		      	7.0.0��  ʹ��������OnPeerInfoReady()�ӿ�IServerSocket
2006-10-23		      	7.1.0��  
2006-11-02		      	7.2.0	 ʹ���µ�IStage
2006-11-13		      	7.2.1	 ����OnTimer���m_pHelper��bug
2006-11-16		      	7.2.2	 ���µ�IStage
2006-12-12		      	7.3.0	 ����INetStageUserData�ӿ�
2006-12-18		      	7.3.1	 �޸��˳�ʱȡip�ķǷ���������
2006-12-21		      	8.0.0	 ���µ�IStage
2007-01-04		      	8.0.1	 �޸�SetTimerʱ�����̲߳��Ե�����
2007-02-09		      	9.0.0	 ����DoSendV()
2007-06-11		      	10.0.0	 ʹ����OnOpenSuccess()��IServerSocket��������IStage::OnInitSuccess()
2008-02-19		      	10.0.1	 Ϊ������߳�����,ʹ�ô�lock��IDMagrģ��,
								 ��ʹ�����е�ID���������Լ��ļ���
2008-02-19		      	10.1.0	 ����TraceStat()
2008-06-11		      	10.2.0	 ����GetServerIPV4()
2008-08-16		      	10.3.0	 ����IsOpenSuccess()
2010-08-19		      	10.4.0	 ���IStage���޸�
								 ����Connect�ӿ�
2010-12-13��20	      	11.0.0	 ����֧��ǰ�÷�����
2011-03-08��20	      	11.0.1	 ʵ��DoGetUserAddress()
2011-09-08		      	11.0.2   ���µ�IStageDriver.h
2011-09-24		      	11.0.3   ֧�ַ��Ͷ���û���ͬ��Ϣʱֱ�Ӹ�ǰ�÷�����
2012-05-17				12.0.0   ֧���µ�DelUser()�ӿ�
2012-05-24				13.0.0   ֧��INVALID_USER_ID ��Ϊ0
2013-05-07				14.0.0   OpenServer()����bShortRoundID����
*/

class IAsynProcQueue;
class IStageSink;
class IStage;
class IServerSocketClientSink;
//**********************************************************************
class INetStageUserData		//GetUserStageData()�����ش˽ӿ�
	{
public:
	enum	{	VERSION = 0	};
	virtual uint32		GetVersion()	const	{	return VERSION;		}

	virtual	uint32		 GetUserIPV4()		const 	= 0;
	virtual	const char * GetUserIP()		const	= 0;	// no const
	virtual	uint16		 GetUserPort()		const 	= 0;
	virtual	uint32		 GetServerIPV4()	const	= 0;	//�û������ӵķ�����IP
	virtual bool		 IsConnectByFS()	const	= 0;	//�Ƿ�ͨ��ǰ�û���������
	};

class INetStageFrontServerInfo
	{
public:
	enum	{	MAX_FS_ADDR_LEN	= 36 };
	virtual uint32		GetKeyID()			const = 0;	//��ʱ����0
	virtual uint32		GetNetLag()			const = 0;	//������ʱ
	virtual uint16		GetPort()			const = 0;	
	virtual const char* GetAddress()		const = 0;	
	virtual const char* GetLoginTime()		const = 0;
	virtual uint32		GetUserCount()		const = 0;
	};

class INetStage
	{
public:	
	//�ͷ�
	virtual void Release()		=	0;

	//��/�رշ�����
	virtual	bool OpenServer(uint16 usPort,IAsynProcQueue *pAsynQueue,
								IStageSink* pSink,int nMaxUser,void *pInitParam,bool bUserArraySendByStage=true,bool bShortRoundID = false)	 = 0;
	virtual bool CloseServer()	= 0;

	//ȡIStage
	virtual		  IStage* GetIStage()		= 0;
	virtual const IStage* GetIStage() const	= 0;

	//Trace��Ϣ,code�Ǵ�������ģ��DebugCmd�Ĵ���
	virtual void  TraceStat(uint32 code= 0)const	= 0;

	//OpenServer()�Ƿ�ɹ�,��IStageSink::OnOpenSuccess()���سɹ�����ȡ�óɹ�
	virtual bool  IsOpenSuccess()	  const = 0;

	//�½����������
	virtual bool  Connect(const char*pIpAddr,uint16 usPort,IServerSocketClientSink*pSink)	= 0;

	//ȡǰ�û��б�
	virtual void  GetFSList(INetStageFrontServerInfo* info[],int &nCount) = 0;

	//��������ǰ�û�
	virtual void  SetServerName(const char*pName)	= 0;
	};

inline
const char* GetStageUserIP(IStageSlave*pSlave,uint32 ulUserID)
{
	INetStageUserData* pUserData = (INetStageUserData*)pSlave->GetUserStageData(ulUserID);	
	if ( NULL == pUserData )
	{
		assert(pUserData);
		return "";
	}
	else
	{
		const char*pIP = pUserData->GetUserIP();
		if ( NULL == pIP || 0 == pIP[0] )
			return "";
		return pIP;
	}
}

inline
uint32 GetStageUserConnectedIPV4(IStageSlave*pSlave,uint32 ulUserID)
{
	INetStageUserData* pUserData = (INetStageUserData*)pSlave->GetUserStageData(ulUserID);	
	if ( NULL == pUserData )
	{
		assert(pUserData);
		return -1;
	}
	else
	{
		return pUserData->GetServerIPV4();
	}
}

//************************************************************************
#define  VER_NET_STAGE_MAIN		14		//���汾�����ǲ���������
#define  VER_NET_STAGE_MIN		0		//�ΰ汾�����Ǽ��ݵ�����(�������ӵ�)
#define  VER_NET_STAGE_BUILD	0		//build�汾�����ǹ��ܲ����bug��������
//************************************************************************
#ifdef _DEBUG
	#define _NET_STATE_DLL "./INetStageD"DLL_EXT
#else
	#define _NET_STATE_DLL "./INetStage"DLL_EXT
#endif

class CINetStageLoader : public InterfaceLoader
									<INetStage,VER_NET_STAGE_MAIN,VER_NET_STAGE_MIN,VER_NET_STAGE_BUILD>
	{
	protected:
	virtual const char *GetDllFileName(){	return _NET_STATE_DLL;			}
	virtual const char *GetProcName()	{	return "CreateINetStage";		}

	public:
		  INetStage *GetNetStage()		  {	return GetInterface();			}
	const INetStage *GetNetStage()	const {	return GetInterface();			}
	};
//**********************************************************************

#endif
