#ifndef SUBSTRIBE_MSG_DEFINE_H_
#define SUBSTRIBE_MSG_DEFINE_H_
//���ĵ���Ϣ
//����ļ��� ../../../BinServer/lua/system/SubstribeMsg.lua �� SubstribeMsg.bat �������벻Ҫ�ֹ��޸�

enum
{

	//ȫ����Ϣ                                                               (����1,        ����2,           ����3,      ����4,      ����5, ....)
	MSG_SERVER_INIT                 	= 1,	//��������ʼ��
	MSG_SERVER_CLOSE                	= 2,	//�������ر�
	MSG_SERVER_READY                	= 3,	//������ready                (ServerInfo,    bool bReady)
	MSG_RUN_COMMAND                 	= 4,	//��������                   (nDSIndex,      bool*pProced,   int argc,   char**argv)
	MSG_PLAYER_MSG                  	= 5,	//�����Ϣ����IMmoModuleSlave(Actor ID,      IGamePlayer*,   pBuff,      nLen)
	MSG_SERVER_MSG                  	= 6,	//��������Ϣ                 (ServerInfo,    cmd,            pBuff,      nLen)
	MSG_RELEASE_ALL_MODULE          	= 7,	//�ر�����ģ��֮ǰ����
	MSG_CHANGE_PLAYER_ANS           	= 8,	//
	MSG_HTTP_REQ_RESULT             	= 9,	//http����Ľ��             (success,       errmsg,         result,     nMsg,        echo0,echo1,echo2

	//DB���
	MSG_DB_DATA_READY               	= 100,	//���������ݿ�������ĳ�ʼ����
	MSG_QUERY_DB_OWNER_END_RECVED   	= 101,	//DB��ѯ���                 (OwnerID,       load_time,      QueryID,    nDSIndex)
	MSG_BEFORE_SYNC                 	= 102,	//Sync֮ǰ�ص�               (0,             0,              bool ��SyncTimer����,bWriteDisk)
	MSG_OWNER_UNLOAD                	= 103,	//                           (OwnerID,       bDeleteFromDB,  bLoaded)
	MSG_OWNER_UNLOADED              	= 104,	//                           (OwnerID,       bDeleteFromDB,  bLoaded)
	MSG_QUERY_DB_OWNER_END_SENDED   	= 105,	//DB��ѯ���(���ݿ��������) (OwnerID,       ServerInfo)
	MSG_TRIGGER                     	= 106,	//��������Ӧ                 (MAKELONGLONG(pTableName,pLineSet),MAKELONGLONG(pNewData,pOldData),(method | _time),CreateTrigger�ķ���ֵ)
	MSG_READONLY_OWNER_DATA         	= 107,	//��ѯֻ�������ݵķ���       (OwnerID,       MAKELONGLONG(tables[],linesets[]),  sessionid,  ulQueryID)
	MSG_QUERY_OWNER_ID_BY_NAME      	= 108,	//                           (OwnerID,       MAKELONGLONG(type,const char*pName),sessionid,  ulQueryID)
	MSG_OWNER_READED_OK             	= 109,	//(DB)���1�ζ�owner������   (OwnerID,       0,              true=����db,false=�ڴ�������)
	MSG_AFTER_SYNC                  	= 110,	//(DB)Sync֮��ص�           (0,             0,              0,          do_a_sync�Ƿ���ͬ��ˢ����һ��)
	MSG_LOAD_NEW_DATA               	= 111,	//��ѯ�����ݵķ���           (OwnerID,       MAKELONGLONG(table,lineset),sessionid,ulQueryID,start_key)
	MSG_ACTOR_LOGIN                 	= 112,	//(DB)ѡ���˽�ɫ��¼,ע��ReplayʱҲ���ܻ���� (OwnerID)
	MSG_ACTOR_LOGOUT                	= 113,	//(DB)�ó�                   (OwnerID)

	//NPC
	MSG_NPC_NEW                     	= 200,	//�½�NPC                    (GUID,          IGameNPC*)
	MSG_NPC_DEL                     	= 201,	//ɾ��NPC                    (GUID,          IGameNPC*)
	MSG_NPC_DEAD                    	= 202,	//NPC����                    (GUID,          IGameNPC*)

	//GOODS
	MSG_GOODS_NEW                   	= 250,	//��Ʒ����                   (GUID,          IGameGoods*)
	MSG_GOODS_DEL                   	= 251,	//��Ʒɾ��                   (GUID,          IGameGoods*)

	//��ҵ�ϵͳ���
	MSG_PLAYER_ENTER                	= 300,	//��ҽ��������             (Actor ID,      IGamePlayer*,   bTransport)
	MSG_PLAYER_LEAVE                	= 301,	//����뿪������             (Actor ID,      IGamePlayer*,   bTransport)
	MSG_PLAYER_BROKEN               	= 302,	//��Ҷ���                   (Actor ID,      IGamePlayer*)
	MSG_PLAYER_REPLAY               	= 303,	//��Ҷ�������               (Actor ID,      IGamePlayer*)
	MSG_PLAYER_SENDLOGINOK          	= 304,	//�������¼�ɹ������       (Actor ID,      IGamePlayer*,   bIsReplay,orientation)
	MSG_PLAYER_BEFORE_LEAVE         	= 305,	//MSG_PLAYER_LEAVEǰ������,ʱ����MSG_PLAYER_LEAVE_INSTANCE֮ǰ    (ActorID,IGamePlayer*,bTransport)

	//�������
	MSG_INSTANCE_NEW                	= 1000,	//�½�����                   (GUID,          IInstanceSlave*, ullForActorID)
	MSG_INSTANCE_DEL                	= 1001,	//ɾ������                   (GUID,          IInstanceSlave*)
	MSG_INSTANCE_INIT               	= 1002,	//��ʼ������                 (GUID,          IInstanceSlave*, ullForActorID)
	MSG_PLAYER_ENTER_INSTANCE       	= 1010,	//�û����븱��               (ActorID,       IGamePlayer*,       IInstanceSlave*,MAKELONG(x,y))
	MSG_PLAYER_ENTER_INSTANCE_1     	= 1011,	//�û����븱��(�ѷ�������Ϣ) (ActorID,       IGamePlayer*,       IInstanceSlave*, MapID,bFirstEnter)
	MSG_PLAYER_LEAVE_INSTANCE       	= 1012,	//                           (ActorID,       IGamePlayer*,       IInstanceSlave*,IsLeave,bTransport)
	MSG_PLAYER_TRANSPORT_RECVED     	= 1013,	//����յ��˵�ͼ�ı����Ϣ   (ActorID,       IGamePlayer*,       IInstanceSlave*, MapID,PendingCount)
	MSG_PLAYER_DEAD                 	= 1014,	//                           (ActorID,       IGamePlayer*,       IInstanceSlave*)

	//����
	MSG_SEND_PLAYER_DATA            	= 2001,	//������Ϣ�����             (MAKELONGLONG(pBuff,nLen),  pUsers,usercount,-1��ʾȫ����)
	MSG_SEND_SERVER_DATA            	= 2002,	//������Ϣ��������           (MAKELONGLONG(pBuff,nLen),MAKELONGLONG(pServerName,nDSIndex),ulServerFlag,usCmd)

	//��������
	MSG_PLAYER_LEVEL_UPDATE         	= 5000,	//�������                   (GUID,          IGamePlayer*,       uint32 newLevel,      uint32 oldLevel)
	MSG_AI_STAND_UP                 	= 6000,	//�����ܻ���վ����           (GUID,          IGameCreature*,
	MSG_AI_CAST_SPELL_SUCCESS       	= 6001,	//����ɹ�ʩ��ĳ����         (GUID,          IGameCreature*,     spellID)
	MSG_AI_BEEN_HIT                 	= 6002,	//�����ܵ�����               (GUID,          IGameCreature*,     spellID)
	MSG_SWITCH_GAME_MODE            	= 7000,	//�л��ͻ���ģʽ             (GUID,          IGamePlayer*  ,     uint32 personal)

	//
	MSG_SERVER_CRASH                	= 100000,	//������ˣ�����֤��ص�Ŷ��Ҳ����֤���߼��̣߳���֮ɶ�����ܱ�֤!

	//��Ϣ��Ŀ
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
