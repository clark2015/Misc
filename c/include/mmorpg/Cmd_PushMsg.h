#ifndef __H_CMD_PUSH_MSG
#define __H_CMD_PUSH_MSG

#pragma pack(push, 1)

#define		MSGID_SERVERINFO			1	//游戏服务器基本信息
#define		MSGID_HTTPHEADINFO			2	//通知服务器HTTP头信息
#define		MSGID_PUSHWEB				3	//推送WEB消息
#define		MSGID_RUNCMD				4	//运行命令
#define		MSGID_SERVERINFOEX			5	//新版本游戏服务器基本信息

//消息头
struct MsgHead{
	ushort				wCmd;		//命令码
	bool				bEnd;		//是否是组包消息的最后一个包
};

//平台基本信息
//@:当客户端连接上来之后第一条需要推送的消息
struct GameServerInfo:public MsgHead{	
	uint32				PID;			//平台ID
	uint32				dwServerID;		//服务器ID	
	
	GameServerInfo(){
		memset(this, 0, sizeof(*this));
		wCmd = MSGID_SERVERINFO;
	}
};

//平台基本信息
//@:当客户端连接上来之后第一条需要推送的消息
struct GameServerInfoEx:public MsgHead{	
	uint32				PID;			//平台ID
	uint32				dwServerID;		//服务器ID	
	uint32				dwServerType;	//服务器类型，原来的版本没有这个字段

	bool	IsGameServer()	const	{	return SERVER_TYPE_GAME_NODE == dwServerType;	}
	GameServerInfoEx(){
		memset(this, 0, sizeof(*this));
		wCmd = MSGID_SERVERINFOEX;
	}
};


//http头信息
struct HttpHeadInfo : public MsgHead{
	uint32				uKey;			//KEY值
	bool				bPost;			//是否是POST方式
	uint32				uLen;			//传递的数据长度(不需要字符串的结束符)
	char				szHttpHead[1];	//信息头

	HttpHeadInfo(){
		memset(this, 0, sizeof(*this));
		wCmd = MSGID_HTTPHEADINFO;
	}
};

//web推送基本头消息定义
struct WebMsgHead : public MsgHead{		
	uint64				uiUserID;	//用户ID(用来在管理器进行线程取模用)	
	uint32				uKey;		//对应的KEY值
	uint32				dwLevel;	//消息推送的重要程度(大于0表示需要超时重试的推送次数)
	WebMsgHead(){
		memset(this, 0, sizeof(*this));
	}
};

/*推送消息
	@:使用方法
	BYTE	btSendBuffer[XXXXX];
	PushWeb* pPush = (PushWeb*)btSendBuffer;
	pPush->Init(....);
*/

struct PushWeb : public WebMsgHead{		
	uint32		uMsgLen;		//组织的消息长度	
	uchar		btData[1];		//msg

	PushWeb(){
		memset(this, 0, sizeof(*this));
	}

	inline void Init(uint64 uiUserID,  uint32 dwLevel, uint32 uKey, uint32 uMsgLen, uchar* pData)
	{
		this->uiUserID = uiUserID;		
		this->dwLevel = dwLevel;
		this->uKey = uKey;
		this->uMsgLen = uMsgLen;
		memcpy(this->btData, pData, uMsgLen);
	}
};

//要求运行命令
struct PushRunCmd:public MsgHead{
	uint32	uMsgLen;
	char	szCmd[1];
};

#pragma pack(pop)
#endif//__H_MSGDEF