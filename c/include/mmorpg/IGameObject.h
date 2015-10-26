#ifndef IGameMapObject_H
#define IGameMapObject_H
#include "../IUnknownEx.h"
#include "mmorpgdefs.h"

struct IGameObject;
struct IGameCreature;
struct IGamePlayer;
struct IGameNPC;
struct IGameGoods;
struct IGameMap;
struct IInstanceSlave;
struct GameObjectPrototype;

//物品类型
enum OBJECT_TYPE
{
	OBJECT_TYPE_PLAYER			= 1,
	OBJECT_TYPE_NPC				= 2,
	OBJECT_TYPE_GOODS			= 4,

	OBJECT_TYPE_NONE			= 0,
	OBJECT_TYPE_CREATURE_MASK	= (OBJECT_TYPE_PLAYER | OBJECT_TYPE_NPC),
	OBJECT_TYPE_ALL				= (OBJECT_TYPE_PLAYER | OBJECT_TYPE_NPC | OBJECT_TYPE_GOODS)
};

//SetModuleData()的index,副本接口那里也用这个
enum	
{
	MODULE_DATA_INDEX_SPELL		= 0,		//魔法、技能模块的数据
	MODULE_DATA_INDEX_SPELL1	= 1,		//魔法、技能模块的数据
	MODULE_DATA_INDEX_MOVEMENT	= 2,		//NPC/玩家移动的数据
	MODULE_DATA_INDEX_MOVEEVENT	= 3,		//NPC/玩家移动的数据
	MODULE_DATA_INDEX_GAMEMAP	= 4,
	MODULE_DATA_INDEX_CHAT		= 5,
	MODULE_DATA_INDEX_RESERVE0	= 6,		//
	MODULE_DATA_INDEX_RESERVE1	= 7,		//

	MODULE_DATA_INDEX_COUNT,
};
//************************************************************************************************************
//可放进地图的物品
static const CLASSUID	IF_UID_GAMEOBJ  ("IGameObject");
static const uint32		IF_VER_GAMEOBJ	= INTERFACE_VER(1,0);
struct IGameObject	: public IUnknownEx
{
	virtual		  IGameObject*		GetObject		()					= 0;
	virtual const IGameObject*		GetObject		()			const	= 0;
	virtual		  IGameCreature*	GetCreature		()					= 0;
	virtual	const IGameCreature*	GetCreature		()			const	= 0;
	virtual		  IGamePlayer*		GetPlayer		()					= 0;
	virtual const IGamePlayer*		GetPlayer		()			const	= 0;
	virtual		  IGameNPC*			GetNPC			()					= 0;
	virtual	const IGameNPC*			GetNPC			()			const	= 0;
	virtual		  IGameGoods*		GetGoods		()					= 0;
	virtual	const IGameGoods*		GetGoods		()			const	= 0;

	virtual uint64					GetGUID			()			const	= 0;	//对玩家来说是角色ID,对NPC来说是一个临时生成的npcid
	virtual OBJECT_TYPE				GetObjectType	()			const	= 0;
	virtual POINT&					GetPos 			(POINT& pt)	const	= 0;	//取位置
	virtual int						GetPosX			()			const	= 0;	
	virtual int						GetPosY			()			const	= 0;
	virtual int						GetPosZ			()			const	= 0;
	virtual const char*				GetName			()			const	= 0;	//可能会返回一个空串("");
	virtual IGameMap*				GetGameMap		()			const	= 0;	//这个函数是可能返回NULL的
	virtual uint32					GetGameMapID	()			const	= 0;	//对于其它服务器，就要用订阅得到的gameid来算了
	virtual const char*				GetGameMapName	()			const	= 0;	//对于游戏服务器，这跟GameGameMap()->GetName() 一样，
	virtual IInstanceSlave*			GetInstance		()			const	= 0;	//这个函数是可能返回NULL的
	virtual long					GetLongProp		(long idx)	const	= 0;	//内部使用
	virtual bool					GetIsHided		()			const	= 0;	//是否隐身

    virtual void*					GetModuleData   (uint32 idx)const	= 0;
    virtual void					SetModuleData   (uint32 idx,void*p)	= 0;
	virtual void					SendNetDataToSeeMe	(const void* pSeeMeBuffer, int nSeeMeSize,	bool bWithMyself = false)	const	= 0;	//发消息给看到我的人

	virtual void					IGameObject_Reserved0()	{}
	virtual void					IGameObject_Reserved1()	{}
	virtual void					IGameObject_Reserved2()	{}
	virtual void					IGameObject_Reserved3()	{}
	virtual void					IGameObject_Reserved4()	{}
	virtual void					IGameObject_Reserved5()	{}

	inline  bool                    IsCreature()const { return(GetObjectType() &  OBJECT_TYPE_CREATURE_MASK) != 0;	}
    inline  bool                    IsPlayer()	const { return GetObjectType() == OBJECT_TYPE_PLAYER;				}
    inline  bool                    IsNPC()		const { return GetObjectType() == OBJECT_TYPE_NPC;					}
    inline  bool                    IsGoods()	const { return GetObjectType() == OBJECT_TYPE_GOODS;				}
};

//************************************************************************************************************
//生物(NPC与玩家的父类)
//注意 RECT 的遵循windows的标准：左和上的边是矩形內部，右和下则不是

enum CreatureDeathState
{
    CREATURE_DEATH_STATE_ALIVE,                //活着
    CREATURE_DEATH_STATE_JUST_DIED,            //死亡，下次事件处理
    CREATURE_DEATH_STATE_CORPSE,               //尸体状态，不能动
    CREATURE_DEATH_STATE_DEAD,                 //死亡幽灵状态，能动，但能看见其他人，其他人不能看见，
    CREATURE_DEATH_STATE_JUST_ALIVED,          //复活，下次时间时间处理
};

struct IVividCreature;
struct IVividNPC;
struct IVividPlayer;
class Creature;
class Player;
class NPC;
static const CLASSUID	IF_UID_CREATURE  ("IGameCreature");
static const uint32		IF_VER_CREATURE	= INTERFACE_VER(1,0);
struct IGameCreature :  public IGameObject
{
	virtual enSex				GetSex			()							const	= 0;
	virtual void				GetWndSize		(int&w,int&h)				const	= 0;	//所能看到的窗口大小(TILE数),这个是客户端的，地图自己根据情况另外维护一份
	virtual float				GetOrientation	()							const	= 0;
	virtual uint32				GetLevel		()							const	= 0;	//取等级
    virtual void                SetCreatureDeathState(CreatureDeathState)			= 0;
	virtual void				TalkTo(const char*pMsg,uint64 ullTo)		const	= 0;	//对某人说话
	virtual void				Talk(const char*pMsg)						const	= 0;	//对附近的人说话,只应在逻辑服务器调用，因为其它服务器不知道位置信息
	virtual void				SendNetDataTo		(const void* pISeeBuffer,	int nIMeSize,	const void* pSeeMeBuffer, int nSeeMeSize,bool bWithMyself = false)	const	= 0;	//发消息给看到我的人 (或/和) 我能看到
	virtual void				SendNetDataToISee	(const void* pISeeBuffer,	int nIMeSize,	bool bWithMyself = false)	const	= 0;	//发消息给我能看到的人

	virtual void				IGameCreature_Reserved0()	{}
	virtual void				IGameCreature_Reserved1()	{}
	virtual void				IGameCreature_Reserved2()	{}
	virtual void				IGameCreature_Reserved3()	{}
	virtual void				IGameCreature_Reserved4()	{}
	virtual void				IGameCreature_Reserved5()	{}

    inline  Creature*			GetVividCreature()			const { return static_cast<Creature*>				(GetModuleData(MODULE_DATA_INDEX_SPELL));			}
    inline  IVividCreature*		GetIVividCreature()			const { return static_cast<IVividCreature*>			(GetModuleData(MODULE_DATA_INDEX_SPELL1));			}
};

//************************************************************************************************************
//玩家
static const CLASSUID	IF_UID_PLAYER  ("IGamePlayer");
static const uint32		IF_VER_PLAYER	= INTERFACE_VER(1,0);
struct IGamePlayer : public IGameCreature
{
	virtual uint32				GetUserID		()		const	= 0;	//用户在服务器临时生成的一个ID,用于发送数据
	virtual uint32				GetLoginSID		()		const	= 0;
	virtual const char*			GetPlatform		()		const	= 0;	//Platform与AccountName确定唯一的帐号
	virtual const char*			GetAccountName	()		const	= 0;	//
	virtual uint64				GetActorID		()		const	= 0;	//取角色ID == GetGUID()
	virtual const char*			GetActorName	()		const	= 0;	//取角色名 == GetName()
	virtual uint8				GetProfession	()		const	= 0;
	virtual uint32				GetNetLag		()		const	= 0;	//取网络延时,单位ms,一个来回的时间/2
	virtual bool				GetChangedMapReceived()	const	= 0;	//登录或地图改变后，客户端是否已经收到地图改变的消息。用于过滤掉对上一个地图的动作
	virtual uint32				GetRight		()		const	= 0;	//权限
	virtual bool				IsAdult			()		const	= 0;	//是否成年(防沉迷)
	virtual bool				IsLoginOK		()		const	= 0;	//已登录成功
	virtual int					GetDSIndex		()		const	= 0;	//从哪个分发服务器过来的
	virtual const char*			GetActorVia		()		const	= 0;	//用户来源，创建角色时写入
	virtual void				DenyLogin		(uint32 t)		= 0;	//参数=结束日期
	virtual void				DenyChat(uint32 seconds)		= 0;	//禁言
	virtual bool				CanChat()				const	= 0;	//可否说话
	virtual void				SendSysMsg		(uint32 ulType,const char*pMsg)		const	= 0;	//发送系统信息，暂时都显示在聊天框里
	virtual void				SendNetData		(const void* pBuffer, int nSize)	const	= 0;

	/*
	传送相关：
	如果要进入公用地图，	应调用Transport()
	如果要创建副本，		应调用Transport()
	如果要进入已有的副本，	应调用TransportByInstanceID()
	当玩家从公用地图传送到其它副本或公用地图，则会保存“上一次所在地图”，下次调用TransportToPrev()时传回去
	*/
	virtual void				Transport			 (uint32 ulMapID,ushort x,ushort y,uint64	ullGroupID)		= 0;	//传送到某地图。如果是公有地图，则自动选线进入；如果是副本，则创建一个。如果ullGroupID!=0，则相同的ullGroupID会传到相同的地图
	virtual uint32				TransportToPrev		 ()															= 0;	//返回上次所在的公有地图。如果没有这样的地方，则失败
	virtual void				TransportByInstanceID(uint64 ullInstanceID,uint32 ulMapID,ushort x,ushort y)	= 0;	//传送到某副本。ulMapID参数用于传送到其它服务器时查询用
	virtual uint32				ReturnCity			 ()															= 0;	//如果地图有定义回城点，则随机进一个，否则调用TransportToPrev()
	
	/*
	每当开一个新服，会分配一个唯一ServerID，在此服创建的角色的ServerID都跟随这个ID，并终生不变
	每当开一个新服，CountryID = ServerID，这也是判断是否合服的条件
	合服时，玩家的ServerID不会变，但这个合服的CountryID会在旧的CountryID中随便选一个
	在跨服服务器，角色的ServerID和CountryID将会被带过去。但跨服本身没有ServerID和Country的概念
	*/
	virtual uint32				GetServerID	()		const	= 0;	//服务器ID
	virtual uint32				GetCountryID()		const	= 0;	//国家ID

	/*
		断线后还停留多久才离开
		这个值在每次进入新的副本时都会重置，默认值是公用地图3秒，副本5分钟
		这个值必须在断线前设置，断线后再设置无效
		second范围是1至1200秒
	*/
	virtual void				SetBrokenStaySecondDefault()		= 0;
	virtual void				SetBrokenStaySecond(uint32 second)	= 0;
	virtual uint32				GetBrokenStaySecond()		const	= 0;	

	virtual uint32				GetConnectTimeOS()			const	= 0;

    inline  Player*             GetVividPlayer()	const { return static_cast<Player*>      (GetModuleData(MODULE_DATA_INDEX_SPELL));  }
    inline  IVividPlayer*       GetIVividPlayer()	const { return static_cast<IVividPlayer*>(GetModuleData(MODULE_DATA_INDEX_SPELL1)); }
};

//struct NPCPrototype;
//enum NPC_Team;
enum NPC_Team
{
    NPC_TEAM_ALLIANCE       = 0,    //好友
    NPC_TEAM_HORDE          = 1,    //对立
    NPC_TEAM_ALLIANCE2       = -1,    //好友
};

//************************************************************************************************************
//NPC
static const CLASSUID	IF_UID_NPC  ("IGameNPC");
static const uint32		IF_VER_NPC	= INTERFACE_VER(1,0);
struct IGameNPC : public IGameCreature
{
    inline  NPC*				GetVividNPC() const { return static_cast<NPC*>		(GetModuleData(MODULE_DATA_INDEX_SPELL));	}
    inline  IVividNPC*          GetIVividNPC()const { return static_cast<IVividNPC*>(GetModuleData(MODULE_DATA_INDEX_SPELL1));	}
};
struct NPCPrototype;
class GameObject;
//************************************************************************************************************
//物品，如草药、剑、传送门
static const CLASSUID	IF_UID_GOODS  ("IGameGoods");
static const uint32		IF_VER_GOODS	= INTERFACE_VER(1,0);
struct IGameGoods	:  public IGameObject
{
    inline  GameObject*					GetVividGameObject() const { return static_cast<GameObject*>(GetModuleData(MODULE_DATA_INDEX_SPELL)); }
};
//************************************************************************************************************

#endif
