#ifndef _ISUBSCRIBE_MAGR_H_
#define _ISUBSCRIBE_MAGR_H_

/*
消息订阅系统
2005-01-28		蓝琚成			建立文件,实现了基本功能
2005-01-30						增加virtual void SetNeedLock(bool b)函数
2005-01-31						把最大订阅数改为每消息不同
								增加了一些Trace
2005-02-19		      			增加OnMsgDelay()和OnCheckDelayTimeOut()
								每回调完一个Sink,都检查消息是否注册了
2005-05-12		      	v1.00	做成dll,写loader
2007-08-13/15	      	v1.01	一些小bug修正及代码整理
2007-08-20/21	      	v2.00	去掉最大订阅数的限制
								订阅时增加lOrder参数，以确定回调顺序
								删除OnCheckDelayTimeOut(),自己调Timer
								增加S/GetLong(),S/GetName()接口
2008-12-10		      	v2.10	OnMsg()改为OnSubscribeMsgReceived()
								删除掉OnMsgUnRegistered()
2010-08-16		      	v2.11	用新的ITimeAxis::GetLongTickCount(),现在完全不依赖于ulong64.h了
2011-10-29		      	v3.00	前2个订阅参数改为int64
2012-06-26		      	v3.01	增加CreateMagr()
2013					v4.08	把参数改成长度可变的数组
								去掉创建时的消息数限制
								并在vc2010和g++下编译
2014							消息类型long->int



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
	int			nCount;		//allParam 数组有多长
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

	//兼容旧的接口
	virtual void OnSubscribeMsgReceived(ISubscribeMagr *pMgr,long lMsg,int64 llParam1,int64 llParam2,long lParam3,long lParam4)
	{
	}

	virtual void OnSubscribeCmdReceived(ISubscribeMagr *pMgr,int nSourceID,int argc,char** argv,char*)
	{
	}
};

//订阅管理
struct ISubscribeMagr : public IUnknownEx2
{
	//建立另一个订阅管理器
	virtual ISubscribeMagr*	CreateMagr()	const = 0;

	//与外界发生联系
	virtual void		SetLong(long l)			= 0;
	virtual long		GetLong()	const		= 0;
	virtual void		SetName(const char*p)	= 0;
	virtual const char* GetName()	const		= 0;
	
	//设置操作时是否需要加锁,default为false
	virtual void SetNeedLock(bool b)		= 0;
	
	//增加一个可订阅的消息，只有已注册的消息才可订阅
	virtual bool RegisterMsg  (int	nMsg)	= 0;
	virtual bool UnRegisterMsg(int  nMsg)	= 0;

	//增加/删除一个订阅,如果之前已经订阅过,会返回失败
	//lOrder确定调用的先后顺序,越小越早.相等则按订阅的先后顺序
	virtual bool Subscribe	(int nMsg,ISubscribeSink *pSink,int nOrder = 0)	= 0;
	virtual bool UnSubscribe(int nMsg,ISubscribeSink *pSink)					= 0;
	//订阅所有已注册的消息
	virtual bool SubscribeAll  (ISubscribeSink *pSink,int nOrder = 0)	 		= 0;
	virtual bool UnSubscribeAll(ISubscribeSink *pSink)	 						= 0;
	
	//消息到，在此消息里会回调所有订阅的ISubscribeSink::OnSubscribeMsgReceived(),返回值暂时没用
	virtual void OnMsg(const StSubscribeMsg*pMsg) = 0;

	//消息来了,但等一下再回调。如果lMilliSec == 0,也不会马上回调,约250ms检查一次
	//注意这个函数假定ITimeLine已经被驱动
	virtual bool OnMsgDelay(unsigned int milliSec,const StSubscribeMsg*pMsg)	= 0;	//未实现

	//未实现
	virtual void SubscribeCmd	(const char*pCmd,ISubscribeSink *pSink,int nOrder = 0)	= 0;
	virtual bool UnSubscribeCmd	(const char*pCmd,ISubscribeSink *pSink)					= 0;
	virtual void OnCommandLine	(int nSourceID,int argc,char**argv,char* = NULL)		= 0;

	//兼容旧的接口
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
#define  VER_SUBSCRIBE_MAGR_MAIN	4		//主版本增加是不兼容升级
#define  VER_SUBSCRIBE_MAGR_MIN		0		//次版本增加是兼容的升级(功能增加等)
#define  VER_SUBSCRIBE_MAGR_BUILD	0		//build版本增加是功能不变的bug修正升级
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


