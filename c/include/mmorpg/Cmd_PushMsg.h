#ifndef __H_CMD_PUSH_MSG
#define __H_CMD_PUSH_MSG

#pragma pack(push, 1)

#define		MSGID_SERVERINFO			1	//��Ϸ������������Ϣ
#define		MSGID_HTTPHEADINFO			2	//֪ͨ������HTTPͷ��Ϣ
#define		MSGID_PUSHWEB				3	//����WEB��Ϣ
#define		MSGID_RUNCMD				4	//��������
#define		MSGID_SERVERINFOEX			5	//�°汾��Ϸ������������Ϣ

//��Ϣͷ
struct MsgHead{
	ushort				wCmd;		//������
	bool				bEnd;		//�Ƿ��������Ϣ�����һ����
};

//ƽ̨������Ϣ
//@:���ͻ�����������֮���һ����Ҫ���͵���Ϣ
struct GameServerInfo:public MsgHead{	
	uint32				PID;			//ƽ̨ID
	uint32				dwServerID;		//������ID	
	
	GameServerInfo(){
		memset(this, 0, sizeof(*this));
		wCmd = MSGID_SERVERINFO;
	}
};

//ƽ̨������Ϣ
//@:���ͻ�����������֮���һ����Ҫ���͵���Ϣ
struct GameServerInfoEx:public MsgHead{	
	uint32				PID;			//ƽ̨ID
	uint32				dwServerID;		//������ID	
	uint32				dwServerType;	//���������ͣ�ԭ���İ汾û������ֶ�

	bool	IsGameServer()	const	{	return SERVER_TYPE_GAME_NODE == dwServerType;	}
	GameServerInfoEx(){
		memset(this, 0, sizeof(*this));
		wCmd = MSGID_SERVERINFOEX;
	}
};


//httpͷ��Ϣ
struct HttpHeadInfo : public MsgHead{
	uint32				uKey;			//KEYֵ
	bool				bPost;			//�Ƿ���POST��ʽ
	uint32				uLen;			//���ݵ����ݳ���(����Ҫ�ַ����Ľ�����)
	char				szHttpHead[1];	//��Ϣͷ

	HttpHeadInfo(){
		memset(this, 0, sizeof(*this));
		wCmd = MSGID_HTTPHEADINFO;
	}
};

//web���ͻ���ͷ��Ϣ����
struct WebMsgHead : public MsgHead{		
	uint64				uiUserID;	//�û�ID(�����ڹ����������߳�ȡģ��)	
	uint32				uKey;		//��Ӧ��KEYֵ
	uint32				dwLevel;	//��Ϣ���͵���Ҫ�̶�(����0��ʾ��Ҫ��ʱ���Ե����ʹ���)
	WebMsgHead(){
		memset(this, 0, sizeof(*this));
	}
};

/*������Ϣ
	@:ʹ�÷���
	BYTE	btSendBuffer[XXXXX];
	PushWeb* pPush = (PushWeb*)btSendBuffer;
	pPush->Init(....);
*/

struct PushWeb : public WebMsgHead{		
	uint32		uMsgLen;		//��֯����Ϣ����	
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

//Ҫ����������
struct PushRunCmd:public MsgHead{
	uint32	uMsgLen;
	char	szCmd[1];
};

#pragma pack(pop)
#endif//__H_MSGDEF