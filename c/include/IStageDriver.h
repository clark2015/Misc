#ifndef _ISTAGE_DRIVER_H__
#define _ISTAGE_DRIVER_H__


/*
2005-04-25/26	蓝琚成			建立文件
								完成基本功能,待测试
2005-04-27/28	      			测试通过,修正了一些bug,加了几个函数,修改AddUser()的参数
2005-05-07		      			IsDataCorrect()和LogStageState()加const修饰
2005-11-20		      			最大用户由5000->25000
								内外部id影射由map改为hash_map,可能会快些..
								ToFirst()和GetNext()加const修饰(好象意义不大..)
2006-02-05/06	      			Send()及SendMsg()加pHead参数
								增加SendIfFlag()和数组参数的Send()
								增加GetUserFlag()/SetUserFlag()
								GetNext()带FlagMask参数及输出用户数据
2006-05-24/25	      	v3.0.0	增加对断线/重玩的支持
2006-05-25		      	v3.0.0	增加对DbID设置及查询的支持
2006-06-07		      	v3.0.1	KickUser()检查用户是否已经断线
2006-06-12		      	v4.0.0	OnInit()增加pInitPraram参数
								OnInit()返回bool值
2006-07-11		      	v5.0.0	改名IStageDrver::OnLeave() ->IStageDriver::OnKick()
								KickUser()和改名IStageDrver::OnKick()增加ulDelayMs参数
2006-07-21		      	v5.0.1	修改Replay()没有修改断线标志的问题
2006-08-04		      	v5.1.0  修改接口名称	
									IStageDrver::SendMsg()	-> IStageDrver::DoSendMsg()
									IStageDrver::OnKick ()	-> IStageDrver::DoKick ()
2006-09-14		      	v6.0.0	增加IStageSink::OnClose(),IStage::CloseStage(),IStageSlave::GetStageState()
2006-09-19		      	v6.0.1	Send()给一个数组的用户时，如果元素为-1,不trace警告
2006-10-13		      	v6.0.2	新的IAsynProc
2006-11-02		      	v7.0.0	整理接口
2006-11-16		      	v8.0.0	去掉OnUserBroken()的返回值，由它自己调用FreeBroken()
2006-12-21		      	v9.0.0	增加GetDbIDMapSize()
								整理接口
								SendIfFlag() Send() DoSendMsg()的pHead参数去掉,整理参数顺序
								DoSendMsg() -> DoSend()
2007-01-04		      	v9.1.0	增加流量统计
2007-02-09		      	v10.0.0	修正断线后GetUserStageData()没有返回NULL的bug
								增加EndCacheSend()、BeginCacheSend()及DoSendV()的接口
2007-03-02		      	v10.0.1	修正踢用户时Cache的发送数据数数目没有置0的bug
2007-06-11		      	v11.0.0	增加OnInitSuccess(),去掉ChangeSink()和ChangeSlave()
2007-06-13		      	v12.0.0	GetFluxIn/Out(),返回CULong64
2008-03-11		      	v12.1.0	增加RandL();
2008-05-20		      	v13.0.0	增加GetNext2()
2010-08-19		      	v14.0.0	GetFluxIn/Out(),返回ulonglong
								OnUserActive(),UserActive()的buff去掉const修饰
2010-12-11		      	v15.0.0	增加 IsOuterIDExist();
								SendV()的长度数组加const	
								DoKick()加Reason参数
2011-03-08至11	      	v15.0.1 增加GetUserAddress(),增加DelAllUser()
								建立时传入随机数种子，避免内部依赖于系统状态
2011-08-25至27	      	v16.0.0 增加IsInnerIDExist()
								增加GetOuterIDByInnerID()/GetInnerIDByOuterID()
								增加SetUserAccountID()等支持记录帐号ID
								增加SetActorID()     等支持记录帐号ID
								AddUser()/OnUserEnter()增加pBuff参数
2011-09-08至09		    v16.0.1 从IStage.h中分离出本文件
								接口改名IStageSlave->IStageDriver,IStageSinkSlave->IStageSlave
								增加GetStageSlave()
								重写了说明
2013-09-11						增加GetOption()
								修正ShortRoundID==true时userid可能为0的bug
2013-10-13						DelAllUser()增加nSourceID参数	
2014-09-13						增加RunCommand

Stage Driver舞台驱动:      
	IStageDriver:Stage的踢人，发消息由这个接口来处理
	IStage:实现一个Stage，当及用户进出，及有消息到达时，由它来通知IStageSink
理论上Stage是可以嵌套的。例如一个房间是一个Stage，房间里一张麻将台也是一个Stage
*/

//**********************************************************************
class IStageSlave;
class IStageSink;

//由舞台的驱动者实现
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

	//关闭
	virtual void	CloseStage(uint32 ulReserved = 0)	= 0;

	//以下函数由舞台的驱动者调用
	//有人进入舞台
	//约定如果addr.usSourceID有不同值的话，必须(ulOuterUID>>24)==addr.usSourceID
	virtual uint32 AddUser(uint32 ulOuterUID,const StStageAddr & addr,const void* pStageUserData,void *pBuff,int nLen)	= 0;
	//有人退出舞台
	//会回调IStageSink::OnUserBroken(),但因是主动,不回调IStageSlave::DoKick()
	virtual bool	DelUser(uint32 ulOuterUID,uint32 ulBrokenReason)		= 0;	
	virtual void	DelAllUser(int nSourceID)								= 0;	
	//舞台上有人发信息
	virtual void	UserActive(uint32 ulOuterUID,void *pBuff,int nSize)	= 0;

	//当前舞台上的人数
	virtual int		GetOnlineUserCount()	const = 0;	//不包括断线的
	virtual int		GetBrokenUserCount()	const = 0;	//断线的
	
	//取数据流量
	virtual uint64	GetFluxIn (void *pReserved = NULL,void *pReserved1 = NULL)	const = 0;
	virtual uint64	GetFluxOut(void *pReserved = NULL,void *pReserved1 = NULL)	const = 0;

	virtual bool	IsOuterIDExist(uint32 ulOuterUID)	const = 0;
	virtual bool	IsInnerIDExist(uint32 ulInnerUID)	const = 0;	//实际上与IStageSlave::IsUserExist()一样

	//ID互转
	virtual uint32	GetOuterIDByInnerID(uint32 ulInnerUID)	const = 0;
	virtual uint32	GetInnerIDByOuterID(uint32 ulOuterUID)	const = 0;

	//
	virtual IStageSlave*	GetStageSlave()	= 0;
	virtual IStageSink*		GetStageSink()	= 0;

	//取选项
	enum
	{
		OPTION_USER_ARRAY_SEND_BY_SELF	= 0,
	};
	virtual long	GetOption(uint32 opt)	const	= 0;
	virtual bool    RunCommand(int argc,char**argv)	= 0;

	enum
	{
	//	SOURCE_ID_MAX			= 255	//只有一个字节大小
		SHORT_ROUND_ZERO_BITS	= 12
	};
};

	//显示状态并检查舞台中相关数据是否正确,会调用相应Sink的对应函数
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

	//pInitParam : 传给IStageSink::OnStageInit()
	//bUserArraySendBySelf: 当要给多人发相同的信息时是自己多次调用IStageDriver::DoSend()还是调用IStageDriver::DoSendUserArray(),后者比前者慢但可以把网络压力分给其它模块（例如如前置服务器，分发服务器）
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
