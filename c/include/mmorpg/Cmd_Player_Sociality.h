#ifndef Cmd_Player_Sociality_h
#define Cmd_Player_Sociality_h

//�罻��������Ϣ
enum
{
//	PLAYER_SEND_BAG				= 2,		//������
//	PLAYER_SEND_GUILD			= 3,		//�����Ϣ
//	PLAYER_SEND_USER_RELATION	= 4,		//��ҹ�ϵ
//	PLAYER_SEND_GUIDE			= 5,		//��������,�����ã�����
//	PLAYER_SEND_TEAM			= 6,		//���
//	PLAYER_SEND_RANKING_LIST	= 7,		//���а�
//	PLAYER_SEND_MAIL			= 8,		//�ʼ�
	PLAYER_SEND_QUERY_USER_FUZZY= 10,		//ģ���������

//	PLAYER_RECV_ANS_OWNER		= 101,		//��������
//	PLAYER_RECV_GUILD			= 102,		//�����Ϣ
//	PLAYER_RECV_USER_RELATION	= 103,		//��ҹ�ϵ
//	PLAYER_RECV_GUIDE			= 104,		//��������
//	PLAYER_RECV_TEAM			= 105,		//���
//	PLAYER_RECV_RANKING_LIST	= 106,		//���а�
//	PLAYER_RECV_MAIL			= 107,		//�ʼ�
	PLAYER_RECV_QUERY_USER_FUZZY= 110,		//ģ���������


	//���� PLAYER_MAIN_CHAT,PLAYER_MAIN_CHAT_GS��������
	PLAYER_SEND_CHAT			= 1,		//����
	PLAYER_RECV_CHAT			= 100,		//����
};

#pragma pack(push,1)

struct StPlayerSend_QueryUserFuzzy
{
	uint16	usCount;	//������ٸ�
	char	name[1];
};

struct StPlayerRecv_QueryUserFuzzy
{
	uint64	ullActorID;
	uint16	usLevel;
	uint8	ucProfession;	//ְҵ
	char	szName[1];
};

struct StPlayer_Chat		//���÷ֳ�2����
{
	enum
	{
		FLAGS_SYS		= 1,		//��ϵͳ���ģ�������ҿͻ��˷���
		FLAGS_NPC		= 2,		//˵�����ǲ���NPC
	};
	uint8		ucChannel;		//����Ƶ��
	uint8		ucSubChannel;
	bool		bIsGM;
	uint8		ucReserved;	
	uint64		ullToActorID;
	uint64		ullFromActorID;	//ϵͳ��Ϣ��0
	union
	{
		char		szToActorName	[ACTOR_NAME_LEN];	//˽��ʱ����ָ������
		char		szFromActorName	[ACTOR_NAME_LEN];	//˭����,˽���������ͨ����
	};
	uint8		ucFlags;
	uint64		ullGuildID;		//���ID,�����ID
	uint32		uiVipRank;		//vip�ȼ�
	uint32		nReserved;		//����
	uint16		usOffSet;		//���ֶε�szMsg֮���ƫ�ƣ�����һ����4 ushort(VPlanLevel) + ushort(msg_size)
	char		szMsg[1];		//������Ϣ
};

#pragma pack(pop)

#endif
