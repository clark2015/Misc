#ifndef _CMD_PLAYER_H_
#define _CMD_PLAYER_H_
#include "../DLTypeDefs.h"
#include "mmorpgdefs.h"

enum
{	
	//��������
	//DB����������������
	PLAYER_MAIN_LOGIN			        = 50,		//��¼,50���ֵһ��Ҫ�̶����������ַ���������ͻ

	//Game����������������
	PLAYER_MAIN_MOVE	                = 1,		//�ƶ���صĵ���Ϣ
	PLAYER_MAIN_QUEST			        = 2,		//������ħ���뼼��
	PLAYER_MAIN_SPELL			        = 3,		//������ħ���뼼��
	PLAYER_MAIN_LUA		                = 5,		//ר�Ž���lua�������Ϣ
	PLAYER_MAIN_LUA_INSTANCE	        = 6,		//����������ڸ���lua�������Ϣ
	PLAYER_MAIN_ACTOR_ACTION	        = 7,		//��һ,���������� Cmd_Player_Action.h
	PLAYER_MAIN_LUA8			        = 8,		//lua��Ϣ
	PLAYER_MAIN_PRACTICE		        = 9,		//����ϵͳ
	PLAYER_MAIN_TRADE_GS		        = 10,		//��Ϸ����ģ��
	PLAYER_MAIN_STORE					= 11,		//�����̵�ϵͳ
	PLAYER_MAIN_MISC			        = 13,		//����絯������
	PLAYER_MAIN_CHAT_GS			        = 14,		//��GameServer��������Ϣ
	
	//�罻����������������
	PLAYER_MAIN_SOCIALITY_START			= 30,		//�罻��������ʼֵ�����ַ��������õ�
	PLAYER_MAIN_SOCIALITY		        = 30,		//�罻����Ϣ���������죬��ᡢ����
	PLAYER_MAIN_TRADE			        = 31,		//����ģ��
	PLAYER_MAIN_CHAT			        = 32,		//������Ϣ
};

#pragma pack(push,1)

struct StMapPoint
{
    uint16  usPosX;
    uint16  usPosY;
};

struct StPlayerSendRecv_GUID
{
	uint64	ullGUID;
};
#pragma pack(pop)

#include "Cmd_Player_Login.h"
#endif
