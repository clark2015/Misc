#ifndef _ISTAGE_H__
#define _ISTAGE_H__
/*
2005-04-25/26	蓝琚成			建立文件
								完成基本功能,待测试
2011-09-08		      	v16.0.1 分离出无关的内容到IStageDriver.h中,并改接口名
2011-09-24		      	v16.0.2 支持发送多个用户相同消息时直接给驱动者
2012-04-13至16	      	v16.0.2 保证UserID!=0，以免有些别人写的模块写错判断语句导致问题。ulOuterUID也不能传进0
								内部RoundUID设为每用户一个，避免很小的短期重复可能性
2012-04-27	      		v16.0.2 内部增加了一个FreeIndex的set，以加快AddUser()在人很多时的查找速度
2012-05-17				v17.0.0 FreeBrokenUser(),OnUserLeave(),OnUserBroken(),DelUser()增加原因参数
								删除旧的棋牌相关的函数
2012-05-24				v18.0.0 INVALID_USER_ID 设为0,原来是uint32(-1)，实际上两个值都不会返回
								INVALID_USER_ID_LONGLONG 也设为0
								上面两个修改，主要是为方便lua里容易表示和理解
								优化遍历用户，减少大多数据情况下的循环次数
2012-07-24				v19.0.0 去掉Rand()和RandL()
2012-20-24				v19.1.0 增加GetCurrentUserID()
2013-05-07				v20.0.0 创建时增加bShortRoundID参数
2013-08-22				v20.0.1 增加GetMaxUserCount()
2013-09-19				v21.0.0 StStageAddr结构增加usSourceID字段
2013-10-14				v21.0.0 对有不同usSourceID的用户，在DoSendUserArray()OuterID最高8位==usSourceID
2014-07-16				v21.0.1 bShortRoundID=true时，从空出的位从原来的8改为12
								TraceAndCheck()及OnTraceAndCheck()接口去掉
2014-08-15				v21.0.2 删除SetUserAccountID()及相关函数
2014-09-10				v21.0.3 增加SetUserCacheException()
2014-10-29				v21.0.4 OnInit(),OnInitSuccess(),OnMsg()改名为OnStageInit(),OnStageInitSuccess(),OnStageMsg()

Stage(舞台):所有玩家在同一个舞台中，统一管理玩家进、出、网络消息、断线等

IStageSink	用于接收事件，如用户进，出，用户消息等
IStageSlave	用于处理事件，如发送消息给用户，踢用户，取IP地址,等
*/

#include "DLTypeDefs.h"

#define STAGE_MAX_USERS				25000
#define INVALID_USER_ID				0			//事实上也!=uint32(-1)
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
	ushort	usSourceID;		//用户来源，在游戏中，例如从不同的分发服务器连过来的
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
	//踢断线，ulDelayMs表示多少ms后才实际断开连接，这是避免数据没发送完就关掉了连接;pReason有时可能方便客户端分析
	//此函数会回调IStageSink::OnUserBroken()和IStageDriver::OnLeave();
	virtual bool	KickUser(uint32 ulUserID,uint32 ulDelayMs = 0,const char*pReason = 0)		  =	0;	

	//用户存在否
	virtual bool	IsUserExist(uint32 ulUserID)	const				= 0;

	//取用户地址
	virtual bool	GetUserAddress(uint32 ulUserID,StStageAddr & addr)	= 0;

	//发送数据
	virtual void	Send	  (const void *pBuff,int nLen,uint32 ulUserID						 ) = 0;
	virtual void	Send	  (const void *pBuff,int nLen,const uint32 ulUserIDs[],ushort usCount) = 0;
	//对所有用户,如果(GetUserFlag() & ulFlagMask) != 0 则发送数据给他
	virtual void	SendIfFlag(const void *pBuff,int nLen,uint32 ulFlagMask						 ) = 0;

	//cache住要发送的数据，调用EndCacheSend()时才发，以减少io调用的次数
	virtual void	BeginCacheSend(void * = 0)				    = 0;
	virtual void	EndCacheSend()								= 0;
	virtual bool	SetUserCacheException(uint32 ulUserID)		= 0;

	//设置用户标志,初始为0
	//如果用户不存在,函数返回false或0
	virtual bool	SetUserFlag(uint32 ulUserID,uint32 ulFlags)	= 0;
	virtual uint32	GetUserFlag(uint32 ulUserID)	const		= 0;

	//设置用户数据指针
	virtual bool	SetUserData(uint32 ulUserID,void *pData)	= 0;
	virtual void*	GetUserData(uint32 ulUserID)				= 0;

	//取IStage::AddUser()时传入的参数
	virtual const void* GetUserStageData(uint32 ulUserID) const	= 0;	

	//取用户队列头位置
	virtual void	ToFirst(uint32 & pos)		const = 0;
	//取位于pos之后,且(GetUserFlag() & ulFlagMask) != 0的用户id及数据,pos将自动递增
	virtual uint32	GetNext (uint32 & pos,uint32 ulFlagMask = -1,void **ppUserData = 0,bool*pIsBroken = 0) const = 0;	//返回INVALID_USER_ID则结束
	virtual uint32	GetNext2(uint32 & pos,					     void **ppUserData = 0,bool*pIsBroken = 0) const = 0;	//返回INVALID_USER_ID则结束

	//用户断线重入了，新ID将被注消，连接将被重新映射到ulBrokenID,OnUserReplay()会被回调
	virtual bool	Replay(uint32 ulNewID,uint32 ulBrokenID)	= 0;
	//断线的可以走了,ulFreeReason由用户自已定义
	virtual bool	FreeBrokenUser(uint32 ulUserID,uint32 ulFreeReason = 0)	= 0;
	//是否断线了
	virtual bool	IsUserBroken(uint32 ulUserID) const			= 0;

	//舞台状态
	virtual STAGE_STATUS GetStageState()		  const			= 0;

	//取用户数
	virtual int			GetUserCount(int *pnOnlineCount=0,int *pnBrokenCount=0)	const			= 0;

	//支持记录数据库ID(角色ID)，每个用户只能设一个Actor,并且必须是全局唯一的，也不能是-1
	virtual bool		SetUserActorID(uint32 ulUserID,uint64 ullActorID)						= 0;
	virtual uint64		GetUserActorID(uint32 ulUserID)	const									= 0;
	virtual uint32		GetUserByActorID(uint64 ullActorID,void **ppUserData,bool *pbBroken)	= 0;
	virtual bool		UnSetUserActorID(uint32 ulUserID)										= 0;
	virtual int			GetActorIDMapSize()				const									= 0;		//可以用来当成已登录人数

	//取当前用户，即服务器正在即时处理这个用户从客户端发来的某个动作。如下情况会设置此值:
	//1.OnUserEnter() 和 OnUserActive()
	//2.由于网络原因的 OnUserBroken() 
	//3.OnUserReplay()时,看调用Replay()的情况，如果ulNewID==当前值，则把ulBrokenID设为当前值
	//其它时候，包括 OnUserLeave()不会设置此值,因为它是由 FreeBrokenUser() 产生的
	//如果没有设置则返回 INVALID_USER_ID
	virtual uint32		GetCurrentUserID()		const	= 0;

	//历史最大用户数
	virtual int			GetMaxUserCount()		const	= 0;
};

	////与上面的SetUserActorID()等类似，这里是记录帐号ID
	////这里假设了每个account只能有一个actor登录
	//virtual bool			SetUserAccountID(uint32 ulUserID,uint64 ullAccountID)					= 0;
	//virtual uint64		GetUserAccountID(uint32 ulUserID)const									= 0;
	//virtual uint32		GetUserByAccountID(uint64 ullAccountID,void**ppUserData,bool*pbBroken)=0;
	//virtual bool			UnSetUserAccountID(uint32 ulUserID)										= 0;
	//virtual int			GetAccountIDMapSize()			const									= 0;

enum BROKEN_REASON
{
	BROKEN_REASON_NONE	 = 0,	//不是在断线回调状态，暂时没用
	BROKEN_REASON_KICK	 = 1,	//游戏逻辑踢
	BROKEN_REASON_CLIENT = 2,	//客户端断开或其它服务器踢
	//其它由DelUser()时传入
};

class IStageSink
{
public:
	//初始化
	virtual bool OnStageInit(IStageSlave *pSlave,void *pInitParam)	= 0;
	virtual bool OnStageInitSuccess()								= 0;	//驱动者初始化成功，例如网络层Listening成功

	//可能是不再监听网络，等
	virtual void OnStageClose(uint32 ulReserved)					= 0;

	//管理信息.不一定由IStage发出,例如可以是定时器信息,数据库返回信息等,请求停止信息等.
	virtual void OnStageMsg(uint32 ulMsg,uint32 ulParam0,uint32 ulParam1,uint32 ulParam2,uint32 ulParam3)	= 0;

	//用户进/出
	virtual void OnUserEnter(uint32 ulUserID,char *pBuff,int nLen)	= 0;
	virtual void OnUserLeave(uint32 ulUserID,uint32 ulFreeReason)	= 0;	//ulFreeReason由FreeBrokenUser()传入

	//用户断线，ulReason参数请看BROKEN_REASON.如果不需要断线的话，请调用IStageSlave::FreeBrokenUser()
	virtual void OnUserBroken(uint32 ulUserID,uint32 ulBrokenReason)= 0;
	//当调用IStageSlave::Replay()时回调
	virtual void OnUserReplay(uint32 ulUserID)						= 0;

	//用户发送消息过来
	virtual void OnUserActive(uint32 ulUserID,char *pBuff,int nLen)	= 0;
};

	//显示状态并检查舞台中相关数据是否正确
//	virtual bool OnTraceAndCheck() const = 0;	
//**********************************************************************
	
#endif
