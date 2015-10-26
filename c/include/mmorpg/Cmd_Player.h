#ifndef _CMD_PLAYER_H_
#define _CMD_PLAYER_H_
#include "../DLTypeDefs.h"
#include "mmorpgdefs.h"

enum
{	
	//主命令码
	//DB服务器的主命令码
	PLAYER_MAIN_LOGIN			        = 50,		//登录,50这个值一定要固定，否则会跟分发服务器冲突

	//Game服务器的主命令码
	PLAYER_MAIN_MOVE	                = 1,		//移动相关的的消息
	PLAYER_MAIN_QUEST			        = 2,		//明亮的魔法与技能
	PLAYER_MAIN_SPELL			        = 3,		//明亮的魔法与技能
	PLAYER_MAIN_LUA		                = 5,		//专门交给lua处理的消息
	PLAYER_MAIN_LUA_INSTANCE	        = 6,		//交给玩家所在副本lua处理的消息
	PLAYER_MAIN_ACTOR_ACTION	        = 7,		//玩家活动,子命令码在 Cmd_Player_Action.h
	PLAYER_MAIN_LUA8			        = 8,		//lua消息
	PLAYER_MAIN_PRACTICE		        = 9,		//修炼系统
	PLAYER_MAIN_TRADE_GS		        = 10,		//游戏交易模块
	PLAYER_MAIN_STORE					= 11,		//神秘商店系统
	PLAYER_MAIN_MISC			        = 13,		//杂项，如弹出窗口
	PLAYER_MAIN_CHAT_GS			        = 14,		//给GameServer的聊天消息
	
	//社交服务器的主命令码
	PLAYER_MAIN_SOCIALITY_START			= 30,		//社交服务器起始值，给分发服务器用的
	PLAYER_MAIN_SOCIALITY		        = 30,		//社交类消息，例如聊天，帮会、好友
	PLAYER_MAIN_TRADE			        = 31,		//交易模块
	PLAYER_MAIN_CHAT			        = 32,		//聊天消息
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
