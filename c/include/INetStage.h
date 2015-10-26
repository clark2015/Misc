#ifndef _INetStage__H__
#define _INetStage__H__
/*
2005-05-05/07	蓝琚成	         实现INetStage
2005-07-10		      	1.1版    使用新的IServerSock(即用ACE的版本)
2006-02-05		      	1.2版    使用新的IStage及IServerSock(即Send()带pHead参数的版本)
2006-05-25		      	2.0版	 使用支持断线的IStage
2006-06-09		      	2.0.1版  修正IStageSink的OnInit()与其它回调不在同一线程的问题
2006-06-09		      	3.0.0版  使用OnInit()带pInitParam参数的IStage
								 BeginServer()改为OpenServer()
2006-07-11		      	4.0.0版  支持新的IStage，让OnKick()加delay参数
2006-07-14		      	5.0.0版  使用新的IAsynQueue及ServerSocket接口
2006-08-04		      	5.0.1版  使用新的IStage，实现CloseServer()
2006-09-14		      	6.0.0版  使用新的IStage
2006-09-19		      	6.0.1版
2006-09-22		      	6.1.0版  增加GetIStage()接口
								 sPort -> usPort
2006-10-13		      	6.1.1版  
2006-10-19		      	7.0.0版  使用增加了OnPeerInfoReady()接口IServerSocket
2006-10-23		      	7.1.0版  
2006-11-02		      	7.2.0	 使用新的IStage
2006-11-13		      	7.2.1	 修正OnTimer检查m_pHelper的bug
2006-11-16		      	7.2.2	 用新的IStage
2006-12-12		      	7.3.0	 增加INetStageUserData接口
2006-12-18		      	7.3.1	 修复退出时取ip的非法操作问题
2006-12-21		      	8.0.0	 用新的IStage
2007-01-04		      	8.0.1	 修改SetTimer时所在线程不对的问题
2007-02-09		      	9.0.0	 增加DoSendV()
2007-06-11		      	10.0.0	 使用有OnOpenSuccess()的IServerSocket，并调用IStage::OnInitSuccess()
2008-02-19		      	10.0.1	 为避免多线程问题,使用带lock的IDMagr模块,
								 并使用其中的ID数来代替自己的计算
2008-02-19		      	10.1.0	 增加TraceStat()
2008-06-11		      	10.2.0	 增加GetServerIPV4()
2008-08-16		      	10.3.0	 增加IsOpenSuccess()
2010-08-19		      	10.4.0	 配合IStage的修改
								 增加Connect接口
2010-12-13至20	      	11.0.0	 增加支持前置服务器
2011-03-08至20	      	11.0.1	 实现DoGetUserAddress()
2011-09-08		      	11.0.2   用新的IStageDriver.h
2011-09-24		      	11.0.3   支持发送多个用户相同消息时直接给前置服务器
2012-05-17				12.0.0   支持新的DelUser()接口
2012-05-24				13.0.0   支持INVALID_USER_ID 设为0
2013-05-07				14.0.0   OpenServer()增加bShortRoundID参数
*/

class IAsynProcQueue;
class IStageSink;
class IStage;
class IServerSocketClientSink;
//**********************************************************************
class INetStageUserData		//GetUserStageData()将返回此接口
	{
public:
	enum	{	VERSION = 0	};
	virtual uint32		GetVersion()	const	{	return VERSION;		}

	virtual	uint32		 GetUserIPV4()		const 	= 0;
	virtual	const char * GetUserIP()		const	= 0;	// no const
	virtual	uint16		 GetUserPort()		const 	= 0;
	virtual	uint32		 GetServerIPV4()	const	= 0;	//用户所连接的服务器IP
	virtual bool		 IsConnectByFS()	const	= 0;	//是否通过前置机连上来的
	};

class INetStageFrontServerInfo
	{
public:
	enum	{	MAX_FS_ADDR_LEN	= 36 };
	virtual uint32		GetKeyID()			const = 0;	//暂时返回0
	virtual uint32		GetNetLag()			const = 0;	//网络延时
	virtual uint16		GetPort()			const = 0;	
	virtual const char* GetAddress()		const = 0;	
	virtual const char* GetLoginTime()		const = 0;
	virtual uint32		GetUserCount()		const = 0;
	};

class INetStage
	{
public:	
	//释放
	virtual void Release()		=	0;

	//打开/关闭服务器
	virtual	bool OpenServer(uint16 usPort,IAsynProcQueue *pAsynQueue,
								IStageSink* pSink,int nMaxUser,void *pInitParam,bool bUserArraySendByStage=true,bool bShortRoundID = false)	 = 0;
	virtual bool CloseServer()	= 0;

	//取IStage
	virtual		  IStage* GetIStage()		= 0;
	virtual const IStage* GetIStage() const	= 0;

	//Trace信息,code是传给网络模块DebugCmd的代码
	virtual void  TraceStat(uint32 code= 0)const	= 0;

	//OpenServer()是否成功,在IStageSink::OnOpenSuccess()返回成功即可取得成功
	virtual bool  IsOpenSuccess()	  const = 0;

	//新建对外的连接
	virtual bool  Connect(const char*pIpAddr,uint16 usPort,IServerSocketClientSink*pSink)	= 0;

	//取前置机列表
	virtual void  GetFSList(INetStageFrontServerInfo* info[],int &nCount) = 0;

	//用来发给前置机
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
#define  VER_NET_STAGE_MAIN		14		//主版本增加是不兼容升级
#define  VER_NET_STAGE_MIN		0		//次版本增加是兼容的升级(功能增加等)
#define  VER_NET_STAGE_BUILD	0		//build版本增加是功能不变的bug修正升级
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
