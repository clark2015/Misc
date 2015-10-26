#ifndef IInstanceZone_h_
#define IInstanceZone_h_
#include "IMmoModule.h"

struct NPCPrototype;
struct GameObjectPrototype;
struct CreatureData;
struct AreaTrigger;

class  VividInstance;
struct IVividInstance;
struct IInstanceEvent;
struct Size_t;
//副本逻辑的函数
struct IInstanceSlave
{
	//取属性
	virtual uint64					GetInstanceID()											const	= 0;	//一个临时的 GUID
	virtual	struct IGameMap*		GetGameMap()													= 0;
	virtual int						GetLine()												const	= 0;	//取线路
    virtual void					SetModuleData(uint32 index, void* pData)						= 0;
    virtual void *					GetModuleData(uint32 index)								const	= 0;
	virtual bool					GetIsPrivate()											const	= 0;	//是否副本，取的地图属性
	virtual bool					GetIsFight()											const	= 0;	//是否战斗地图，取的地图属性
	virtual IInstanceSlave*			GetBrotherInstance(uint32 ulMapID)								= 0;	//取同一副本系列的其它副本实例

	//增删物品、NPC、玩家
	virtual void					InsertPlayer(IGamePlayer*pPlayer,long  x,long  y)				= 0;
	virtual void					RemovePlayer(IGamePlayer*pPlayer,bool bIsLeave,bool bTransport)	= 0;
	virtual IGameNPC*				CreateCreature	(int x,int y,float angle,const char*name)		= 0;	//创建NPC
	virtual IGameGoods*				CreateGoods		(int x,int y,const char*name)					= 0;	//创建传送门，草药等
	virtual bool					DeleteObject	(ObjectGuid guid)								= 0;	//删除NPC或物品，如果传入的是玩家或不存在，则失败

	//取物品、NPC、玩家
    virtual IGameNPC*				GetNPCIfCanInteractWith (IGamePlayer*pPlayer,ObjectGuid guid)	= 0;
    virtual IGameGoods*				GetGGIfCanInteractWith  (IGamePlayer*pPlayer,ObjectGuid guid)   = 0;
    virtual IGameCreature*			GetCreatureByGuid       (ObjectGuid guid)                       = 0;
    virtual IGameObject*			GetObjectByGuid         (ObjectGuid guid)                       = 0;
    virtual IGamePlayer*			GetPlayerByGuid         (ObjectGuid guid)                       = 0;
    virtual IGameNPC*				GetNPCByGuid            (ObjectGuid guid)                       = 0;

	//发送消息
	virtual void					SendNetData(const void*pBuff,int nLen)					const	= 0;	//发消息给副本内所有人
	virtual void					SendChangeMap(IGamePlayer*pPlayer,int x,int y)			const	= 0;

	//其它操作
	virtual void					SetCompleted()													= 0;	//已完成,后面不会再用到这个副本及我的兄弟副本了。设置之后，这个副本可以比较快地删除掉以回收内存。公用地图不需要调用这个
	virtual IInstanceSlave*			TransportAllPlayer(uint32 ulMapID,ushort x,ushort y)			= 0;	//把整个副本里的人都传到指定地图,只能在同一进程内使用，失败返回NULL

	//明亮
    VividInstance*					GetVividInstance()			const { return static_cast<VividInstance*>		 (GetModuleData(MODULE_DATA_INDEX_SPELL));			}
    IVividInstance*					GetIVividInstance()			const { return static_cast<IVividInstance*>		 (GetModuleData(MODULE_DATA_INDEX_SPELL));			}
    IInstanceEvent*					GetInstanceEvent()			const { return static_cast<IInstanceEvent*>		 (GetModuleData(MODULE_DATA_INDEX_MOVEEVENT));		}
};

struct IInstanceMagr : public IUnknownEx2
{
	virtual IInstanceSlave*			GetInstance			(uint64 ullInstanceID)				= 0;
	virtual IInstanceSlave*			GetPublicInstance	(uint32 ulMapID,int nLine = -1)		= 0;
	virtual IInstanceSlave*			GetInstanceByGroup	(uint64 ullGroup)					= 0;
	virtual IInstanceSlave*			CreateInstanceZone	(uint32 ulMapID,uint64 ullForActorID,IInstanceSlave*pOld)		= 0;	//ullForActorID是用来传给MSG_INSTANCE_NEW/MSG_INSTANCE_INIT的
	virtual void					TraceInstanceList	()									= 0;
	virtual uint64					NewGroupID			()									= 0;
	virtual void					AddGroupMap			(uint64 ullGroupID,IInstanceSlave*pSlave,bool bFailTrace = true)= 0;
	virtual void					SendLineInfo		(IGamePlayer*pPlayer,uint32 ulMapID) const	= 0;

	//玩家操作暂时放这里
	virtual IGamePlayer*			CreateGamePlayer(uint32 ulUserID,const struct StCommServer_TransmitUser_FromDB_V2*pEnter)	= 0;
	virtual void					DeleteGamePlayer(IGamePlayer*pPlayer)								= 0;
	virtual void					SetLoginSID		(IGamePlayer*pPlayer,uint32 ulSID)					= 0;
	virtual void					SetDBInfo		(IGamePlayer*pPlayer)								= 0;
	virtual bool					IsDBInfoReceived(IGamePlayer*pPlayer)						const	= 0;
	virtual void					WriteBackDBCache(IGamePlayer*pPlayer)								= 0;
	virtual void					OnBroken		(IGamePlayer*pPlayer,uint32 ulBrokenReason)			= 0;
	virtual void					OnReplay		(IGamePlayer*pPlayer,uint32 ulUserID)				= 0;
	virtual void					OnPingAns		(IGamePlayer*pPlayer)								= 0;
	virtual void					SetIsTransport	(IGamePlayer*pPlayer,bool bTransport,uint64 ullGroupID)	= 0;
	virtual bool					GetIsTransport	(IGamePlayer*pPlayer)						const	= 0;
	virtual uint64					GetLoginGroupID	(IGamePlayer*pPlayer)						const	= 0;
	virtual uint32					GetEnterProcessTime(IGamePlayer*pPlayer)					const	= 0;
};

#endif
