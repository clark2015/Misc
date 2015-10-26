#ifndef _ISUBSCRIBE_MAGR_H_
#define _ISUBSCRIBE_MAGR_H_

/*
��Ϣ����ϵͳ
2005-01-28		��袳�			�����ļ�,ʵ���˻�������
2005-01-30						����virtual void SetNeedLock(bool b)����
2005-01-31						�����������Ϊÿ��Ϣ��ͬ
								������һЩTrace
2005-02-19		      			����OnMsgDelay()��OnCheckDelayTimeOut()
								ÿ�ص���һ��Sink,�������Ϣ�Ƿ�ע����
2005-05-12		      	v1.00	����dll,дloader
2007-08-13/15	      	v1.01	һЩСbug��������������
2007-08-20/21	      	v2.00	ȥ���������������
								����ʱ����lOrder��������ȷ���ص�˳��
								ɾ��OnCheckDelayTimeOut(),�Լ���Timer
								����S/GetLong(),S/GetName()�ӿ�
2008-12-10		      	v2.10	OnMsg()��ΪOnSubscribeMsgReceived()
								ɾ����OnMsgUnRegistered()
2010-08-16		      	v2.11	���µ�ITimeAxis::GetLongTickCount(),������ȫ��������ulong64.h��
2011-10-29		      	v3.00	ǰ2�����Ĳ�����Ϊint64
2012-06-26		      	v3.01	����CreateMagr()
2013					v4.08	�Ѳ����ĳɳ��ȿɱ������
								ȥ������ʱ����Ϣ������
								����vc2010��g++�±���
2014							��Ϣ����long->int



*/
#include "DLTypeDefs.h"
#include "commInterfaceDef.h"
#include "IUnknownEx.h"
#include <assert.h>
struct ISubscribeMagr;

#pragma pack(push,1)
struct StSubscribeMsg
{
	int			nMsg;
	int			nCount;		//allParam �����ж೤
	int64		allParam[4];
};
#pragma pack(pop)

struct ISubscribeSink
{
	virtual void OnSubscribeMsgReceived(struct ISubscribeMagr *pMgr,const StSubscribeMsg*pMsg)
	{
		assert(pMsg && pMsg->nCount==4);
		int		  nMsg	=		pMsg->nMsg;
		int64 llParam1	=		pMsg->allParam[0];
		int64 llParam2	=		pMsg->allParam[1];
		long	lParam3	= (long)pMsg->allParam[2];
		long	lParam4	= (long)pMsg->allParam[3];
		OnSubscribeMsgReceived(pMgr,nMsg,llParam1,llParam2,lParam3,lParam4);
	}

	//���ݾɵĽӿ�
	virtual void OnSubscribeMsgReceived(ISubscribeMagr *pMgr,long lMsg,int64 llParam1,int64 llParam2,long lParam3,long lParam4)
	{
	}

	virtual void OnSubscribeCmdReceived(ISubscribeMagr *pMgr,int nSourceID,int argc,char** argv,char*)
	{
	}
};

//���Ĺ���
struct ISubscribeMagr : public IUnknownEx2
{
	//������һ�����Ĺ�����
	virtual ISubscribeMagr*	CreateMagr()	const = 0;

	//����緢����ϵ
	virtual void		SetLong(long l)			= 0;
	virtual long		GetLong()	const		= 0;
	virtual void		SetName(const char*p)	= 0;
	virtual const char* GetName()	const		= 0;
	
	//���ò���ʱ�Ƿ���Ҫ����,defaultΪfalse
	virtual void SetNeedLock(bool b)		= 0;
	
	//����һ���ɶ��ĵ���Ϣ��ֻ����ע�����Ϣ�ſɶ���
	virtual bool RegisterMsg  (int	nMsg)	= 0;
	virtual bool UnRegisterMsg(int  nMsg)	= 0;

	//����/ɾ��һ������,���֮ǰ�Ѿ����Ĺ�,�᷵��ʧ��
	//lOrderȷ�����õ��Ⱥ�˳��,ԽСԽ��.����򰴶��ĵ��Ⱥ�˳��
	virtual bool Subscribe	(int nMsg,ISubscribeSink *pSink,int nOrder = 0)	= 0;
	virtual bool UnSubscribe(int nMsg,ISubscribeSink *pSink)					= 0;
	//����������ע�����Ϣ
	virtual bool SubscribeAll  (ISubscribeSink *pSink,int nOrder = 0)	 		= 0;
	virtual bool UnSubscribeAll(ISubscribeSink *pSink)	 						= 0;
	
	//��Ϣ�����ڴ���Ϣ���ص����ж��ĵ�ISubscribeSink::OnSubscribeMsgReceived(),����ֵ��ʱû��
	virtual void OnMsg(const StSubscribeMsg*pMsg) = 0;

	//��Ϣ����,����һ���ٻص������lMilliSec == 0,Ҳ�������ϻص�,Լ250ms���һ��
	//ע����������ٶ�ITimeLine�Ѿ�������
	virtual bool OnMsgDelay(unsigned int milliSec,const StSubscribeMsg*pMsg)	= 0;	//δʵ��

	//δʵ��
	virtual void SubscribeCmd	(const char*pCmd,ISubscribeSink *pSink,int nOrder = 0)	= 0;
	virtual bool UnSubscribeCmd	(const char*pCmd,ISubscribeSink *pSink)					= 0;
	virtual void OnCommandLine	(int nSourceID,int argc,char**argv,char* = NULL)		= 0;

	//���ݾɵĽӿ�
	inline	void OnMsg(	int nMsg,
				int64 llParam1	= 0,
				int64 llParam2	= 0,
				long lParam3	= 0,
				long lParam4	= 0)
	{
		OnMsg4(nMsg,llParam1,llParam2,lParam3,lParam4);
	}

	inline	void OnMsg4(int nMsg,
				int64 llParam1	= 0,
				int64 llParam2	= 0,
				int64 llParam3	= 0,
				int64 llParam4	= 0)
	{
		char buff[sizeof(StSubscribeMsg) + 2*sizeof(int64)];
		StSubscribeMsg*pMsg = (StSubscribeMsg*)buff;
		pMsg->nMsg			= nMsg;
		pMsg->nCount		= 4;
		pMsg->allParam[0]	= llParam1;
		pMsg->allParam[1]	= llParam2;
		pMsg->allParam[2]	= llParam3;
		pMsg->allParam[3]	= llParam4;
		OnMsg(pMsg);
	}

	inline	void OnMsg6(int nMsg,
				int64 llParam1	= 0,
				int64 llParam2	= 0,
				int64 llParam3	= 0,
				int64 llParam4	= 0,
				int64 llParam5	= 0,
				int64 llParam6	= 0)
	{
		char buff[sizeof(StSubscribeMsg) + 2*sizeof(int64)];
		StSubscribeMsg*pMsg = (StSubscribeMsg*)buff;
		pMsg->nMsg			= nMsg;
		pMsg->nCount		= 6;
		pMsg->allParam[0]	= llParam1;
		pMsg->allParam[1]	= llParam2;
		pMsg->allParam[2]	= llParam3;
		pMsg->allParam[3]	= llParam4;
		pMsg->allParam[4]	= llParam5;
		pMsg->allParam[5]	= llParam6;
		OnMsg(pMsg);
	}
};

static const CLASSUID	IF_UID_SUBSCRIBE_MAGR ("ISubscribeMagr");
static const uint32		IF_VER_SUBSCRIBE_MAGR = INTERFACE_VER(1,0);

//************************************************************************
#define  VER_SUBSCRIBE_MAGR_MAIN	4		//���汾�����ǲ���������
#define  VER_SUBSCRIBE_MAGR_MIN		0		//�ΰ汾�����Ǽ��ݵ�����(�������ӵ�)
#define  VER_SUBSCRIBE_MAGR_BUILD	0		//build�汾�����ǹ��ܲ����bug��������
//************************************************************************

class CISubscribeMagrLoader : public InterfaceLoader<ISubscribeMagr,VER_SUBSCRIBE_MAGR_MAIN,VER_SUBSCRIBE_MAGR_MIN,VER_SUBSCRIBE_MAGR_BUILD>
{
#ifdef _DEBUG
	virtual const char *GetDllFileName(){	return "./SubscribeMagr2D"DLL_EXT;	}
#else
	virtual const char *GetDllFileName(){	return "./SubscribeMagr2" DLL_EXT;	}
#endif
	virtual const char *GetProcName()	{	return "CreateISubscribeMagr";		}
};

#endif


