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

//��Ʒ����
enum OBJECT_TYPE
{
	OBJECT_TYPE_PLAYER			= 1,
	OBJECT_TYPE_NPC				= 2,
	OBJECT_TYPE_GOODS			= 4,

	OBJECT_TYPE_NONE			= 0,
	OBJECT_TYPE_CREATURE_MASK	= (OBJECT_TYPE_PLAYER | OBJECT_TYPE_NPC),
	OBJECT_TYPE_ALL				= (OBJECT_TYPE_PLAYER | OBJECT_TYPE_NPC | OBJECT_TYPE_GOODS)
};

//SetModuleData()��index,�����ӿ�����Ҳ�����
enum	
{
	MODULE_DATA_INDEX_SPELL		= 0,		//ħ��������ģ�������
	MODULE_DATA_INDEX_SPELL1	= 1,		//ħ��������ģ�������
	MODULE_DATA_INDEX_MOVEMENT	= 2,		//NPC/����ƶ�������
	MODULE_DATA_INDEX_MOVEEVENT	= 3,		//NPC/����ƶ�������
	MODULE_DATA_INDEX_GAMEMAP	= 4,
	MODULE_DATA_INDEX_CHAT		= 5,
	MODULE_DATA_INDEX_RESERVE0	= 6,		//
	MODULE_DATA_INDEX_RESERVE1	= 7,		//

	MODULE_DATA_INDEX_COUNT,
};
//************************************************************************************************************
//�ɷŽ���ͼ����Ʒ
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

	virtual uint64					GetGUID			()			const	= 0;	//�������˵�ǽ�ɫID,��NPC��˵��һ����ʱ���ɵ�npcid
	virtual OBJECT_TYPE				GetObjectType	()			const	= 0;
	virtual POINT&					GetPos 			(POINT& pt)	const	= 0;	//ȡλ��
	virtual int						GetPosX			()			const	= 0;	
	virtual int						GetPosY			()			const	= 0;
	virtual int						GetPosZ			()			const	= 0;
	virtual const char*				GetName			()			const	= 0;	//���ܻ᷵��һ���մ�("");
	virtual IGameMap*				GetGameMap		()			const	= 0;	//��������ǿ��ܷ���NULL��
	virtual uint32					GetGameMapID	()			const	= 0;	//������������������Ҫ�ö��ĵõ���gameid������
	virtual const char*				GetGameMapName	()			const	= 0;	//������Ϸ�����������GameGameMap()->GetName() һ����
	virtual IInstanceSlave*			GetInstance		()			const	= 0;	//��������ǿ��ܷ���NULL��
	virtual long					GetLongProp		(long idx)	const	= 0;	//�ڲ�ʹ��
	virtual bool					GetIsHided		()			const	= 0;	//�Ƿ�����

    virtual void*					GetModuleData   (uint32 idx)const	= 0;
    virtual void					SetModuleData   (uint32 idx,void*p)	= 0;
	virtual void					SendNetDataToSeeMe	(const void* pSeeMeBuffer, int nSeeMeSize,	bool bWithMyself = false)	const	= 0;	//����Ϣ�������ҵ���

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
//����(NPC����ҵĸ���)
//ע�� RECT ����ѭwindows�ı�׼������ϵı��Ǿ��΃Ȳ����Һ�������

enum CreatureDeathState
{
    CREATURE_DEATH_STATE_ALIVE,                //����
    CREATURE_DEATH_STATE_JUST_DIED,            //�������´��¼�����
    CREATURE_DEATH_STATE_CORPSE,               //ʬ��״̬�����ܶ�
    CREATURE_DEATH_STATE_DEAD,                 //��������״̬���ܶ������ܿ��������ˣ������˲��ܿ�����
    CREATURE_DEATH_STATE_JUST_ALIVED,          //����´�ʱ��ʱ�䴦��
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
	virtual void				GetWndSize		(int&w,int&h)				const	= 0;	//���ܿ����Ĵ��ڴ�С(TILE��),����ǿͻ��˵ģ���ͼ�Լ������������ά��һ��
	virtual float				GetOrientation	()							const	= 0;
	virtual uint32				GetLevel		()							const	= 0;	//ȡ�ȼ�
    virtual void                SetCreatureDeathState(CreatureDeathState)			= 0;
	virtual void				TalkTo(const char*pMsg,uint64 ullTo)		const	= 0;	//��ĳ��˵��
	virtual void				Talk(const char*pMsg)						const	= 0;	//�Ը�������˵��,ֻӦ���߼����������ã���Ϊ������������֪��λ����Ϣ
	virtual void				SendNetDataTo		(const void* pISeeBuffer,	int nIMeSize,	const void* pSeeMeBuffer, int nSeeMeSize,bool bWithMyself = false)	const	= 0;	//����Ϣ�������ҵ��� (��/��) ���ܿ���
	virtual void				SendNetDataToISee	(const void* pISeeBuffer,	int nIMeSize,	bool bWithMyself = false)	const	= 0;	//����Ϣ�����ܿ�������

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
//���
static const CLASSUID	IF_UID_PLAYER  ("IGamePlayer");
static const uint32		IF_VER_PLAYER	= INTERFACE_VER(1,0);
struct IGamePlayer : public IGameCreature
{
	virtual uint32				GetUserID		()		const	= 0;	//�û��ڷ�������ʱ���ɵ�һ��ID,���ڷ�������
	virtual uint32				GetLoginSID		()		const	= 0;
	virtual const char*			GetPlatform		()		const	= 0;	//Platform��AccountNameȷ��Ψһ���ʺ�
	virtual const char*			GetAccountName	()		const	= 0;	//
	virtual uint64				GetActorID		()		const	= 0;	//ȡ��ɫID == GetGUID()
	virtual const char*			GetActorName	()		const	= 0;	//ȡ��ɫ�� == GetName()
	virtual uint8				GetProfession	()		const	= 0;
	virtual uint32				GetNetLag		()		const	= 0;	//ȡ������ʱ,��λms,һ�����ص�ʱ��/2
	virtual bool				GetChangedMapReceived()	const	= 0;	//��¼���ͼ�ı�󣬿ͻ����Ƿ��Ѿ��յ���ͼ�ı����Ϣ�����ڹ��˵�����һ����ͼ�Ķ���
	virtual uint32				GetRight		()		const	= 0;	//Ȩ��
	virtual bool				IsAdult			()		const	= 0;	//�Ƿ����(������)
	virtual bool				IsLoginOK		()		const	= 0;	//�ѵ�¼�ɹ�
	virtual int					GetDSIndex		()		const	= 0;	//���ĸ��ַ�������������
	virtual const char*			GetActorVia		()		const	= 0;	//�û���Դ��������ɫʱд��
	virtual void				DenyLogin		(uint32 t)		= 0;	//����=��������
	virtual void				DenyChat(uint32 seconds)		= 0;	//����
	virtual bool				CanChat()				const	= 0;	//�ɷ�˵��
	virtual void				SendSysMsg		(uint32 ulType,const char*pMsg)		const	= 0;	//����ϵͳ��Ϣ����ʱ����ʾ���������
	virtual void				SendNetData		(const void* pBuffer, int nSize)	const	= 0;

	/*
	������أ�
	���Ҫ���빫�õ�ͼ��	Ӧ����Transport()
	���Ҫ����������		Ӧ����Transport()
	���Ҫ�������еĸ�����	Ӧ����TransportByInstanceID()
	����Ҵӹ��õ�ͼ���͵������������õ�ͼ����ᱣ�桰��һ�����ڵ�ͼ�����´ε���TransportToPrev()ʱ����ȥ
	*/
	virtual void				Transport			 (uint32 ulMapID,ushort x,ushort y,uint64	ullGroupID)		= 0;	//���͵�ĳ��ͼ������ǹ��е�ͼ�����Զ�ѡ�߽��룻����Ǹ������򴴽�һ�������ullGroupID!=0������ͬ��ullGroupID�ᴫ����ͬ�ĵ�ͼ
	virtual uint32				TransportToPrev		 ()															= 0;	//�����ϴ����ڵĹ��е�ͼ�����û�������ĵط�����ʧ��
	virtual void				TransportByInstanceID(uint64 ullInstanceID,uint32 ulMapID,ushort x,ushort y)	= 0;	//���͵�ĳ������ulMapID�������ڴ��͵�����������ʱ��ѯ��
	virtual uint32				ReturnCity			 ()															= 0;	//�����ͼ�ж���سǵ㣬�������һ�����������TransportToPrev()
	
	/*
	ÿ����һ���·��������һ��ΨһServerID���ڴ˷������Ľ�ɫ��ServerID���������ID������������
	ÿ����һ���·���CountryID = ServerID����Ҳ���ж��Ƿ�Ϸ�������
	�Ϸ�ʱ����ҵ�ServerID����䣬������Ϸ���CountryID���ھɵ�CountryID�����ѡһ��
	�ڿ������������ɫ��ServerID��CountryID���ᱻ����ȥ�����������û��ServerID��Country�ĸ���
	*/
	virtual uint32				GetServerID	()		const	= 0;	//������ID
	virtual uint32				GetCountryID()		const	= 0;	//����ID

	/*
		���ߺ�ͣ����ò��뿪
		���ֵ��ÿ�ν����µĸ���ʱ�������ã�Ĭ��ֵ�ǹ��õ�ͼ3�룬����5����
		���ֵ�����ڶ���ǰ���ã����ߺ���������Ч
		second��Χ��1��1200��
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
    NPC_TEAM_ALLIANCE       = 0,    //����
    NPC_TEAM_HORDE          = 1,    //����
    NPC_TEAM_ALLIANCE2       = -1,    //����
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
//��Ʒ�����ҩ������������
static const CLASSUID	IF_UID_GOODS  ("IGameGoods");
static const uint32		IF_VER_GOODS	= INTERFACE_VER(1,0);
struct IGameGoods	:  public IGameObject
{
    inline  GameObject*					GetVividGameObject() const { return static_cast<GameObject*>(GetModuleData(MODULE_DATA_INDEX_SPELL)); }
};
//************************************************************************************************************

#endif
