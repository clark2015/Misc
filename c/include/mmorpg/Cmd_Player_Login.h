#ifndef _CMD_PLAYER_LOGIN_H_
#define _CMD_PLAYER_LOGIN_H_
#include "../DLTypeDefs.h"
#include "mmorpgdefs.h"
#include "Cmd_Player_PlatformInfo.h"

enum
{
										// �ṹ						����
	//��¼��������
	PLAYER_RECV_LOGIN_SUCC		= 1,
	PLAYER_RECV_LOGIN_FAIL		= 2,
	PLAYER_RECV_ALL_ACTOR		= 3,	//StPlayerRecv_AllActor		�·����н�ɫ��Ϣ
	PLAYER_RECV_NEW_ACTOR_RES	= 4,	//StPlayerRecv_NewActorResult�½���ɫ�Ľ��
	PLAYER_RECV_DEL_ACTOR_RES	= 5,	//		ɾ����ɫ�Ľ��
	PLAYER_RECV_PING_DB			= 6,	//
	PLAYER_RECV_RAND_NAME		= 7,	//StPlayerRecv_RandName		���������ɫ��
	PLAYER_RECV_LOGIN_TOKEN		= 8,	//32�ֽ��ַ���+0��β		������Ϣ

	PLAYER_SEND_NEW_ACTOR		= 100,	//StPlayerSend_NewActor		�½���ɫ
	PLAYER_SEND_DEL_ACTOR		= 101,	//StPlayerSendRecv_GUID		ɾ����ɫ
	PLAYER_SEND_USE_ACTOR		= 102,	//StPlayerSend_UseActor		ʹ�����actor ID��¼
	PLAYER_SEND_PING_DB			= 103,
	PLAYER_SEND_RAND_NAME		= 105,	//StPlayerSend_RandName		ȡ�������				
};

#pragma pack(push,1)

struct StPlayerSend_RandName
{
	uint8	sex;
};

struct StPlayerRecv_RandName
{
	char szRandName[ACTOR_NAME_LEN];
};

struct StPlayerSend_UseActor
{
	uint64	ullActorID;
	uint16	usViewWidth;	//���Ӵ��ڵĸ߿�,����ûʲô����
	uint16	usViewHeight;	//
};

struct StPlayerRecv_AllActor
{
	uint16	usCount;
	uint16	usStructLen;
	struct	StItem
	{
		uint64		ullActorID;
		char		szActorName[ACTOR_NAME_LEN];
		uint8		ucLevel;
		uint8		profession;	//ְҵ
		uint8		sex;
		uint8		cFive;		//����
		uint16		usBanneret;
		uint16		usPosX;
		uint16		usPosY;
		uint32		ulMapID;
		char		szMapName[MAP_NAME_LEN];
		bool		bNeverLogin;
	};
	StItem item[1];

	uint32	GetSize()	const	{	return 4 + usCount*sizeof(StItem);	 }
};

struct StPlayerSend_NewActor
{
	char		szActorName[ACTOR_NAME_LEN];
	uint8		profession;	//ְҵ
	uint8		sex;
	uint8		five;		//����
	uint16		usFaceID;	//����
};

struct StPlayerRecv_NewActorResult
{
	uint64		ullActorID;	//0��ʾʧ��
	uint16		usCode;		//��ʱ����
	char		szReason[1];//����ɹ����������StPlayerSend_NewActor
};

struct StPlayerRecv_DelActorResult
{
	uint64		ullActorID;	//��������ɾ����ID
	uint16		usCode;		//0��ʾ�ɹ�
	char		szReason[1];
};

//��¼��Ϣ
struct StPlayerLogin
{
	enum
	{
		MAGIC_NO = 0x24568735
	};

	uint32		ulMagicNo;
	uint64		ullAccountID;	//ullActorID;
	uint16		usViewWidth;	//���Ӵ��ڵĸ߿�
	uint16		usViewHeight;	//
	char		szUserPass[USER_PASS_LEN];
};

struct StPlayerLogin_V2
{
	enum
	{
		MAGIC_NO = 0x24568735
	};

	uint32		ulMagicNo;
	uint64		ullAccountID;	//ullActorID;
	uint16		usViewWidth;	//���Ӵ��ڵĸ߿�
	uint16		usViewHeight;	//
	char		szUserPass[USER_PASS_LEN];
	uint32		uiServerID;
};

struct StPlayerBirthItem
{
	enum
	{
		STATUS_DIE			= 1,	//�Ѿ����ˣ�����ʬ��

		FLAGS_CREATE		= 1,	//1=NPC����Ʒ����������ҵ�¼,0=���������Ұ
		FLAGS_HAS_Z			= 64,	//��z���꣬����Ϊ0
		FLAGS_HAS_SPEED_MULT= 128,	//�ٶȱ���,����Ϊ1
	};
	uint64		ullActorID;
	char		szActorName[ACTOR_NAME_LEN];
	float		orientation;	//����
	uint8		ucState;
	uint8		profession;		//ְҵ
	char		sex;
	uint16		usPosX;
	uint16		usPosY;
	uint16		usFlags;
};

struct StPlayerLoginResult
{
	StPlayerBirthItem	item;
	struct other
	{
		uint64			ullReserved;					//ullAccountID;
		char			szInstanceName[MAP_NAME_LEN];	//������
	//	uint64			ullReserved0;					
		uint32			uiFunctionOpended3;				//�ѿ����Ĺ���
		uint32			ulReserved0;
		uint16			usViewWidth;					//���Ӵ��ڵĸ߿�
		uint16			usViewHeight;					//
		uint32			ulClientMapID;					//��ͼID
		bool			bFirstLogin;					//��һ�ε�¼
		bool			bFight;							//�Ƿ�ս����ͼ
		uint8			ucInstanceType;					//0:���õ�ͼ,1:��ͨ����
		uint32			ulShowCloth;					//���
		uint32			ulShowWeapon;					//���
		uint32          ulShowWeaponEffect;				//���
		uint16			usLine;							//��ǰ��·
		uint32			ulServerMapID;
		uint32			ulInstanceSerialID;				//����ϵ��
		uint32			uiFunctionOpended1;				//�ѿ����Ĺ���
		uint32			uiFunctionOpended2;				//�ѿ����Ĺ���
		uint32			now;							//=time(NULL)
		char			szClientExtendInfo[1];			//0��β�Ŀͻ�����Ϣ��npc��Ϣ�ַ���
	};
	other o;
};
#pragma pack(pop)

#endif
