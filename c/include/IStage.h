#ifndef _ISTAGE_H__
#define _ISTAGE_H__
/*
2005-04-25/26	��袳�			�����ļ�
								��ɻ�������,������
2011-09-08		      	v16.0.1 ������޹ص����ݵ�IStageDriver.h��,���Ľӿ���
2011-09-24		      	v16.0.2 ֧�ַ��Ͷ���û���ͬ��Ϣʱֱ�Ӹ�������
2012-04-13��16	      	v16.0.2 ��֤UserID!=0��������Щ����д��ģ��д���ж���䵼�����⡣ulOuterUIDҲ���ܴ���0
								�ڲ�RoundUID��Ϊÿ�û�һ���������С�Ķ����ظ�������
2012-04-27	      		v16.0.2 �ڲ�������һ��FreeIndex��set���Լӿ�AddUser()���˺ܶ�ʱ�Ĳ����ٶ�
2012-05-17				v17.0.0 FreeBrokenUser(),OnUserLeave(),OnUserBroken(),DelUser()����ԭ�����
								ɾ���ɵ�������صĺ���
2012-05-24				v18.0.0 INVALID_USER_ID ��Ϊ0,ԭ����uint32(-1)��ʵ��������ֵ�����᷵��
								INVALID_USER_ID_LONGLONG Ҳ��Ϊ0
								���������޸ģ���Ҫ��Ϊ����lua�����ױ�ʾ�����
								�Ż������û������ٴ����������µ�ѭ������
2012-07-24				v19.0.0 ȥ��Rand()��RandL()
2012-20-24				v19.1.0 ����GetCurrentUserID()
2013-05-07				v20.0.0 ����ʱ����bShortRoundID����
2013-08-22				v20.0.1 ����GetMaxUserCount()
2013-09-19				v21.0.0 StStageAddr�ṹ����usSourceID�ֶ�
2013-10-14				v21.0.0 ���в�ͬusSourceID���û�����DoSendUserArray()OuterID���8λ==usSourceID
2014-07-16				v21.0.1 bShortRoundID=trueʱ���ӿճ���λ��ԭ����8��Ϊ12
								TraceAndCheck()��OnTraceAndCheck()�ӿ�ȥ��
2014-08-15				v21.0.2 ɾ��SetUserAccountID()����غ���
2014-09-10				v21.0.3 ����SetUserCacheException()
2014-10-29				v21.0.4 OnInit(),OnInitSuccess(),OnMsg()����ΪOnStageInit(),OnStageInitSuccess(),OnStageMsg()

Stage(��̨):���������ͬһ����̨�У�ͳһ������ҽ�������������Ϣ�����ߵ�

IStageSink	���ڽ����¼������û����������û���Ϣ��
IStageSlave	���ڴ����¼����緢����Ϣ���û������û���ȡIP��ַ,��
*/

#include "DLTypeDefs.h"

#define STAGE_MAX_USERS				25000
#define INVALID_USER_ID				0			//��ʵ��Ҳ!=uint32(-1)
#define INVALID_USER_ID_LONGLONG	0
//**********************************************************************
enum STAGE_STATUS
{
	STAGE_STATE_INITING	= 10,
	STAGE_STATE_NORMAL	= 20,
	STAGE_STATE_CLOSING	= 30,
	STAGE_STATE_CLOSED	= 40,
};
//**********************************************************************
struct StStageAddr
{
	ushort	usSourceID;		//�û���Դ������Ϸ�У�����Ӳ�ͬ�ķַ���������������
	ushort	usAddrType;
	union ADDR_UNION
	{
		struct IPV4
		{
			uint32	ulIP;
			ushort	usPort;
		}
		ipv4;
	}Address;
};

//**********************************************************************
class IStageSlave
{
public:	
	//�߶��ߣ�ulDelayMs��ʾ����ms���ʵ�ʶϿ����ӣ����Ǳ�������û������͹ص�������;pReason��ʱ���ܷ���ͻ��˷���
	//�˺�����ص�IStageSink::OnUserBroken()��IStageDriver::OnLeave();
	virtual bool	KickUser(uint32 ulUserID,uint32 ulDelayMs = 0,const char*pReason = 0)		  =	0;	

	//�û����ڷ�
	virtual bool	IsUserExist(uint32 ulUserID)	const				= 0;

	//ȡ�û���ַ
	virtual bool	GetUserAddress(uint32 ulUserID,StStageAddr & addr)	= 0;

	//��������
	virtual void	Send	  (const void *pBuff,int nLen,uint32 ulUserID						 ) = 0;
	virtual void	Send	  (const void *pBuff,int nLen,const uint32 ulUserIDs[],ushort usCount) = 0;
	//�������û�,���(GetUserFlag() & ulFlagMask) != 0 �������ݸ���
	virtual void	SendIfFlag(const void *pBuff,int nLen,uint32 ulFlagMask						 ) = 0;

	//cacheסҪ���͵����ݣ�����EndCacheSend()ʱ�ŷ����Լ���io���õĴ���
	virtual void	BeginCacheSend(void * = 0)				    = 0;
	virtual void	EndCacheSend()								= 0;
	virtual bool	SetUserCacheException(uint32 ulUserID)		= 0;

	//�����û���־,��ʼΪ0
	//����û�������,��������false��0
	virtual bool	SetUserFlag(uint32 ulUserID,uint32 ulFlags)	= 0;
	virtual uint32	GetUserFlag(uint32 ulUserID)	const		= 0;

	//�����û�����ָ��
	virtual bool	SetUserData(uint32 ulUserID,void *pData)	= 0;
	virtual void*	GetUserData(uint32 ulUserID)				= 0;

	//ȡIStage::AddUser()ʱ����Ĳ���
	virtual const void* GetUserStageData(uint32 ulUserID) const	= 0;	

	//ȡ�û�����ͷλ��
	virtual void	ToFirst(uint32 & pos)		const = 0;
	//ȡλ��pos֮��,��(GetUserFlag() & ulFlagMask) != 0���û�id������,pos���Զ�����
	virtual uint32	GetNext (uint32 & pos,uint32 ulFlagMask = -1,void **ppUserData = 0,bool*pIsBroken = 0) const = 0;	//����INVALID_USER_ID�����
	virtual uint32	GetNext2(uint32 & pos,					     void **ppUserData = 0,bool*pIsBroken = 0) const = 0;	//����INVALID_USER_ID�����

	//�û����������ˣ���ID����ע�������ӽ�������ӳ�䵽ulBrokenID,OnUserReplay()�ᱻ�ص�
	virtual bool	Replay(uint32 ulNewID,uint32 ulBrokenID)	= 0;
	//���ߵĿ�������,ulFreeReason���û����Ѷ���
	virtual bool	FreeBrokenUser(uint32 ulUserID,uint32 ulFreeReason = 0)	= 0;
	//�Ƿ������
	virtual bool	IsUserBroken(uint32 ulUserID) const			= 0;

	//��̨״̬
	virtual STAGE_STATUS GetStageState()		  const			= 0;

	//ȡ�û���
	virtual int			GetUserCount(int *pnOnlineCount=0,int *pnBrokenCount=0)	const			= 0;

	//֧�ּ�¼���ݿ�ID(��ɫID)��ÿ���û�ֻ����һ��Actor,���ұ�����ȫ��Ψһ�ģ�Ҳ������-1
	virtual bool		SetUserActorID(uint32 ulUserID,uint64 ullActorID)						= 0;
	virtual uint64		GetUserActorID(uint32 ulUserID)	const									= 0;
	virtual uint32		GetUserByActorID(uint64 ullActorID,void **ppUserData,bool *pbBroken)	= 0;
	virtual bool		UnSetUserActorID(uint32 ulUserID)										= 0;
	virtual int			GetActorIDMapSize()				const									= 0;		//�������������ѵ�¼����

	//ȡ��ǰ�û��������������ڼ�ʱ��������û��ӿͻ��˷�����ĳ��������������������ô�ֵ:
	//1.OnUserEnter() �� OnUserActive()
	//2.��������ԭ��� OnUserBroken() 
	//3.OnUserReplay()ʱ,������Replay()����������ulNewID==��ǰֵ�����ulBrokenID��Ϊ��ǰֵ
	//����ʱ�򣬰��� OnUserLeave()�������ô�ֵ,��Ϊ������ FreeBrokenUser() ������
	//���û�������򷵻� INVALID_USER_ID
	virtual uint32		GetCurrentUserID()		const	= 0;

	//��ʷ����û���
	virtual int			GetMaxUserCount()		const	= 0;
};

	////�������SetUserActorID()�����ƣ������Ǽ�¼�ʺ�ID
	////���������ÿ��accountֻ����һ��actor��¼
	//virtual bool			SetUserAccountID(uint32 ulUserID,uint64 ullAccountID)					= 0;
	//virtual uint64		GetUserAccountID(uint32 ulUserID)const									= 0;
	//virtual uint32		GetUserByAccountID(uint64 ullAccountID,void**ppUserData,bool*pbBroken)=0;
	//virtual bool			UnSetUserAccountID(uint32 ulUserID)										= 0;
	//virtual int			GetAccountIDMapSize()			const									= 0;

enum BROKEN_REASON
{
	BROKEN_REASON_NONE	 = 0,	//�����ڶ��߻ص�״̬����ʱû��
	BROKEN_REASON_KICK	 = 1,	//��Ϸ�߼���
	BROKEN_REASON_CLIENT = 2,	//�ͻ��˶Ͽ���������������
	//������DelUser()ʱ����
};

class IStageSink
{
public:
	//��ʼ��
	virtual bool OnStageInit(IStageSlave *pSlave,void *pInitParam)	= 0;
	virtual bool OnStageInitSuccess()								= 0;	//�����߳�ʼ���ɹ������������Listening�ɹ�

	//�����ǲ��ټ������磬��
	virtual void OnStageClose(uint32 ulReserved)					= 0;

	//������Ϣ.��һ����IStage����,��������Ƕ�ʱ����Ϣ,���ݿⷵ����Ϣ��,����ֹͣ��Ϣ��.
	virtual void OnStageMsg(uint32 ulMsg,uint32 ulParam0,uint32 ulParam1,uint32 ulParam2,uint32 ulParam3)	= 0;

	//�û���/��
	virtual void OnUserEnter(uint32 ulUserID,char *pBuff,int nLen)	= 0;
	virtual void OnUserLeave(uint32 ulUserID,uint32 ulFreeReason)	= 0;	//ulFreeReason��FreeBrokenUser()����

	//�û����ߣ�ulReason�����뿴BROKEN_REASON.�������Ҫ���ߵĻ��������IStageSlave::FreeBrokenUser()
	virtual void OnUserBroken(uint32 ulUserID,uint32 ulBrokenReason)= 0;
	//������IStageSlave::Replay()ʱ�ص�
	virtual void OnUserReplay(uint32 ulUserID)						= 0;

	//�û�������Ϣ����
	virtual void OnUserActive(uint32 ulUserID,char *pBuff,int nLen)	= 0;
};

	//��ʾ״̬�������̨����������Ƿ���ȷ
//	virtual bool OnTraceAndCheck() const = 0;	
//**********************************************************************
	
#endif
