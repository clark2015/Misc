#ifndef SUBSTRIBE_MSG_DEFINE_H_
#define SUBSTRIBE_MSG_DEFINE_H_
//订阅的信息
//这个文件由 ../../../BinServer/lua/system/SubstribeMsg.lua 与 SubstribeMsg.bat 产生，请不要手工修改

enum
{

	//全局消息                                                               (参数1,        参数2,           参数3,      参数4,      参数5, ....)
	MSG_SERVER_INIT                 	= 1,	//服务器初始化
	MSG_SERVER_CLOSE                	= 2,	//服务器关闭
	MSG_SERVER_READY                	= 3,	//服务器ready                (ServerInfo,    bool bReady)
	MSG_RUN_COMMAND                 	= 4,	//运行命令                   (nDSIndex,      bool*pProced,   int argc,   char**argv)
	MSG_PLAYER_MSG                  	= 5,	//玩家消息，看IMmoModuleSlave(Actor ID,      IGamePlayer*,   pBuff,      nLen)
	MSG_SERVER_MSG                  	= 6,	//服务器消息                 (ServerInfo,    cmd,            pBuff,      nLen)
	MSG_RELEASE_ALL_MODULE          	= 7,	//关闭所有模块之前调用
	MSG_CHANGE_PLAYER_ANS           	= 8,	//
	MSG_HTTP_REQ_RESULT             	= 9,	//http请求的结果             (success,       errmsg,         result,     nMsg,        echo0,echo1,echo2

	//DB相关
	MSG_DB_DATA_READY               	= 100,	//接收完数据库服务器的初始数据
	MSG_QUERY_DB_OWNER_END_RECVED   	= 101,	//DB查询完成                 (OwnerID,       load_time,      QueryID,    nDSIndex)
	MSG_BEFORE_SYNC                 	= 102,	//Sync之前回调               (0,             0,              bool 由SyncTimer触发,bWriteDisk)
	MSG_OWNER_UNLOAD                	= 103,	//                           (OwnerID,       bDeleteFromDB,  bLoaded)
	MSG_OWNER_UNLOADED              	= 104,	//                           (OwnerID,       bDeleteFromDB,  bLoaded)
	MSG_QUERY_DB_OWNER_END_SENDED   	= 105,	//DB查询完成(数据库服务器用) (OwnerID,       ServerInfo)
	MSG_TRIGGER                     	= 106,	//触发器响应                 (MAKELONGLONG(pTableName,pLineSet),MAKELONGLONG(pNewData,pOldData),(method | _time),CreateTrigger的返回值)
	MSG_READONLY_OWNER_DATA         	= 107,	//查询只读表数据的返回       (OwnerID,       MAKELONGLONG(tables[],linesets[]),  sessionid,  ulQueryID)
	MSG_QUERY_OWNER_ID_BY_NAME      	= 108,	//                           (OwnerID,       MAKELONGLONG(type,const char*pName),sessionid,  ulQueryID)
	MSG_OWNER_READED_OK             	= 109,	//(DB)完成1次读owner的请求   (OwnerID,       0,              true=读了db,false=内存中已有)
	MSG_AFTER_SYNC                  	= 110,	//(DB)Sync之后回调           (0,             0,              0,          do_a_sync是否再同步刷新了一次)
	MSG_LOAD_NEW_DATA               	= 111,	//查询新数据的返回           (OwnerID,       MAKELONGLONG(table,lineset),sessionid,ulQueryID,start_key)
	MSG_ACTOR_LOGIN                 	= 112,	//(DB)选择了角色登录,注意Replay时也可能会调用 (OwnerID)
	MSG_ACTOR_LOGOUT                	= 113,	//(DB)拿出                   (OwnerID)

	//NPC
	MSG_NPC_NEW                     	= 200,	//新建NPC                    (GUID,          IGameNPC*)
	MSG_NPC_DEL                     	= 201,	//删除NPC                    (GUID,          IGameNPC*)
	MSG_NPC_DEAD                    	= 202,	//NPC死亡                    (GUID,          IGameNPC*)

	//GOODS
	MSG_GOODS_NEW                   	= 250,	//物品掉落                   (GUID,          IGameGoods*)
	MSG_GOODS_DEL                   	= 251,	//物品删除                   (GUID,          IGameGoods*)

	//玩家的系统相关
	MSG_PLAYER_ENTER                	= 300,	//玩家进入服务器             (Actor ID,      IGamePlayer*,   bTransport)
	MSG_PLAYER_LEAVE                	= 301,	//玩家离开服务器             (Actor ID,      IGamePlayer*,   bTransport)
	MSG_PLAYER_BROKEN               	= 302,	//玩家断线                   (Actor ID,      IGamePlayer*)
	MSG_PLAYER_REPLAY               	= 303,	//玩家断线重玩               (Actor ID,      IGamePlayer*)
	MSG_PLAYER_SENDLOGINOK          	= 304,	//发送完登录成功后调用       (Actor ID,      IGamePlayer*,   bIsReplay,orientation)
	MSG_PLAYER_BEFORE_LEAVE         	= 305,	//MSG_PLAYER_LEAVE前被调用,时间在MSG_PLAYER_LEAVE_INSTANCE之前    (ActorID,IGamePlayer*,bTransport)

	//副本相关
	MSG_INSTANCE_NEW                	= 1000,	//新建副本                   (GUID,          IInstanceSlave*, ullForActorID)
	MSG_INSTANCE_DEL                	= 1001,	//删除副本                   (GUID,          IInstanceSlave*)
	MSG_INSTANCE_INIT               	= 1002,	//初始化副本                 (GUID,          IInstanceSlave*, ullForActorID)
	MSG_PLAYER_ENTER_INSTANCE       	= 1010,	//用户加入副本               (ActorID,       IGamePlayer*,       IInstanceSlave*,MAKELONG(x,y))
	MSG_PLAYER_ENTER_INSTANCE_1     	= 1011,	//用户加入副本(已发进入消息) (ActorID,       IGamePlayer*,       IInstanceSlave*, MapID,bFirstEnter)
	MSG_PLAYER_LEAVE_INSTANCE       	= 1012,	//                           (ActorID,       IGamePlayer*,       IInstanceSlave*,IsLeave,bTransport)
	MSG_PLAYER_TRANSPORT_RECVED     	= 1013,	//玩家收到了地图改变的消息   (ActorID,       IGamePlayer*,       IInstanceSlave*, MapID,PendingCount)
	MSG_PLAYER_DEAD                 	= 1014,	//                           (ActorID,       IGamePlayer*,       IInstanceSlave*)

	//杂项
	MSG_SEND_PLAYER_DATA            	= 2001,	//发送消息给玩家             (MAKELONGLONG(pBuff,nLen),  pUsers,usercount,-1表示全部人)
	MSG_SEND_SERVER_DATA            	= 2002,	//发送消息给服务器           (MAKELONGLONG(pBuff,nLen),MAKELONGLONG(pServerName,nDSIndex),ulServerFlag,usCmd)

	//玩家其它活动
	MSG_PLAYER_LEVEL_UPDATE         	= 5000,	//玩家升级                   (GUID,          IGamePlayer*,       uint32 newLevel,      uint32 oldLevel)
	MSG_AI_STAND_UP                 	= 6000,	//生物受击后站起来           (GUID,          IGameCreature*,
	MSG_AI_CAST_SPELL_SUCCESS       	= 6001,	//生物成功施放某技能         (GUID,          IGameCreature*,     spellID)
	MSG_AI_BEEN_HIT                 	= 6002,	//生物受到攻击               (GUID,          IGameCreature*,     spellID)
	MSG_SWITCH_GAME_MODE            	= 7000,	//切换客户端模式             (GUID,          IGamePlayer*  ,     uint32 personal)

	//
	MSG_SERVER_CRASH                	= 100000,	//程序挂了，不保证会回调哦，也不保证在逻辑线程，总之啥都不能保证!

	//消息数目
	TOTAL_MSG_COUNT	= 50,
};

#define   REGISTER_ALL_SUBSTRIBE_MSG(sub)    {\
   int allmsgs[] = {	MSG_SERVER_INIT,MSG_SERVER_CLOSE,MSG_SERVER_READY,MSG_RUN_COMMAND,MSG_PLAYER_MSG,MSG_SERVER_MSG,MSG_RELEASE_ALL_MODULE,MSG_CHANGE_PLAYER_ANS,MSG_HTTP_REQ_RESULT,MSG_DB_DATA_READY,MSG_QUERY_DB_OWNER_END_RECVED,MSG_BEFORE_SYNC,MSG_OWNER_UNLOAD,MSG_OWNER_UNLOADED,MSG_QUERY_DB_OWNER_END_SENDED,MSG_TRIGGER,MSG_READONLY_OWNER_DATA,MSG_QUERY_OWNER_ID_BY_NAME,MSG_OWNER_READED_OK,MSG_AFTER_SYNC,MSG_LOAD_NEW_DATA,MSG_ACTOR_LOGIN,MSG_ACTOR_LOGOUT,MSG_NPC_NEW,MSG_NPC_DEL,MSG_NPC_DEAD,MSG_GOODS_NEW,MSG_GOODS_DEL,MSG_PLAYER_ENTER,MSG_PLAYER_LEAVE,MSG_PLAYER_BROKEN,MSG_PLAYER_REPLAY,MSG_PLAYER_SENDLOGINOK,MSG_PLAYER_BEFORE_LEAVE,MSG_INSTANCE_NEW,MSG_INSTANCE_DEL,MSG_INSTANCE_INIT,MSG_PLAYER_ENTER_INSTANCE,MSG_PLAYER_ENTER_INSTANCE_1,MSG_PLAYER_LEAVE_INSTANCE,MSG_PLAYER_TRANSPORT_RECVED,MSG_PLAYER_DEAD,MSG_SEND_PLAYER_DATA,MSG_SEND_SERVER_DATA,MSG_PLAYER_LEVEL_UPDATE,MSG_AI_STAND_UP,MSG_AI_CAST_SPELL_SUCCESS,MSG_AI_BEEN_HIT,MSG_SWITCH_GAME_MODE,MSG_SERVER_CRASH	};\
 for (int i=0; i<lengthof(allmsgs); i++) sub->RegisterMsg(allmsgs[i]);\
}

#define UnREGISTER_ALL_SUBSTRIBE_MSG(sub)    {\
   int allmsgs[] = {	MSG_SERVER_INIT,MSG_SERVER_CLOSE,MSG_SERVER_READY,MSG_RUN_COMMAND,MSG_PLAYER_MSG,MSG_SERVER_MSG,MSG_RELEASE_ALL_MODULE,MSG_CHANGE_PLAYER_ANS,MSG_HTTP_REQ_RESULT,MSG_DB_DATA_READY,MSG_QUERY_DB_OWNER_END_RECVED,MSG_BEFORE_SYNC,MSG_OWNER_UNLOAD,MSG_OWNER_UNLOADED,MSG_QUERY_DB_OWNER_END_SENDED,MSG_TRIGGER,MSG_READONLY_OWNER_DATA,MSG_QUERY_OWNER_ID_BY_NAME,MSG_OWNER_READED_OK,MSG_AFTER_SYNC,MSG_LOAD_NEW_DATA,MSG_ACTOR_LOGIN,MSG_ACTOR_LOGOUT,MSG_NPC_NEW,MSG_NPC_DEL,MSG_NPC_DEAD,MSG_GOODS_NEW,MSG_GOODS_DEL,MSG_PLAYER_ENTER,MSG_PLAYER_LEAVE,MSG_PLAYER_BROKEN,MSG_PLAYER_REPLAY,MSG_PLAYER_SENDLOGINOK,MSG_PLAYER_BEFORE_LEAVE,MSG_INSTANCE_NEW,MSG_INSTANCE_DEL,MSG_INSTANCE_INIT,MSG_PLAYER_ENTER_INSTANCE,MSG_PLAYER_ENTER_INSTANCE_1,MSG_PLAYER_LEAVE_INSTANCE,MSG_PLAYER_TRANSPORT_RECVED,MSG_PLAYER_DEAD,MSG_SEND_PLAYER_DATA,MSG_SEND_SERVER_DATA,MSG_PLAYER_LEVEL_UPDATE,MSG_AI_STAND_UP,MSG_AI_CAST_SPELL_SUCCESS,MSG_AI_BEEN_HIT,MSG_SWITCH_GAME_MODE,MSG_SERVER_CRASH	};\
 for (int i=0; i<lengthof(allmsgs); i++) sub->UnRegisterMsg(allmsgs[i]);\
}

#endif //SUBSTRIBE_MSG_DEFINE_H_
