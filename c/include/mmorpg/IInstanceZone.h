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
//�����߼��ĺ���
struct IInstanceSlave
{
	//ȡ����
	virtual uint64					GetInstanceID()											const	= 0;	//һ����ʱ�� GUID
	virtual	struct IGameMap*		GetGameMap()													= 0;
	virtual int						GetLine()												const	= 0;	//ȡ��·
    virtual void					SetModuleData(uint32 index, void* pData)						= 0;
    virtual void *					GetModuleData(uint32 index)								const	= 0;
	virtual bool					GetIsPrivate()											const	= 0;	//�Ƿ񸱱���ȡ�ĵ�ͼ����
	virtual bool					GetIsFight()											const	= 0;	//�Ƿ�ս����ͼ��ȡ�ĵ�ͼ����
	virtual IInstanceSlave*			GetBrotherInstance(uint32 ulMapID)								= 0;	//ȡͬһ����ϵ�е���������ʵ��

	//��ɾ��Ʒ��NPC�����
	virtual void					InsertPlayer(IGamePlayer*pPlayer,long  x,long  y)				= 0;
	virtual void					RemovePlayer(IGamePlayer*pPlayer,bool bIsLeave,bool bTransport)	= 0;
	virtual IGameNPC*				CreateCreature	(int x,int y,float angle,const char*name)		= 0;	//����NPC
	virtual IGameGoods*				CreateGoods		(int x,int y,const char*name)					= 0;	//���������ţ���ҩ��
	virtual bool					DeleteObject	(ObjectGuid guid)								= 0;	//ɾ��NPC����Ʒ��������������һ򲻴��ڣ���ʧ��

	//ȡ��Ʒ��NPC�����
    virtual IGameNPC*				GetNPCIfCanInteractWith (IGamePlayer*pPlayer,ObjectGuid guid)	= 0;
    virtual IGameGoods*				GetGGIfCanInteractWith  (IGamePlayer*pPlayer,ObjectGuid guid)   = 0;
    virtual IGameCreature*			GetCreatureByGuid       (ObjectGuid guid)                       = 0;
    virtual IGameObject*			GetObjectByGuid         (ObjectGuid guid)                       = 0;
    virtual IGamePlayer*			GetPlayerByGuid         (ObjectGuid guid)                       = 0;
    virtual IGameNPC*				GetNPCByGuid            (ObjectGuid guid)                       = 0;

	//������Ϣ
	virtual void					SendNetData(const void*pBuff,int nLen)					const	= 0;	//����Ϣ��������������
	virtual void					SendChangeMap(IGamePlayer*pPlayer,int x,int y)			const	= 0;

	//��������
	virtual void					SetCompleted()													= 0;	//�����,���治�����õ�����������ҵ��ֵܸ����ˡ�����֮������������ԱȽϿ��ɾ�����Ի����ڴ档���õ�ͼ����Ҫ�������
	virtual IInstanceSlave*			TransportAllPlayer(uint32 ulMapID,ushort x,ushort y)			= 0;	//��������������˶�����ָ����ͼ,ֻ����ͬһ������ʹ�ã�ʧ�ܷ���NULL

	//����
    VividInstance*					GetVividInstance()			const { return static_cast<VividInstance*>		 (GetModuleData(MODULE_DATA_INDEX_SPELL));			}
    IVividInstance*					GetIVividInstance()			const { return static_cast<IVividInstance*>		 (GetModuleData(MODULE_DATA_INDEX_SPELL));			}
    IInstanceEvent*					GetInstanceEvent()			const { return static_cast<IInstanceEvent*>		 (GetModuleData(MODULE_DATA_INDEX_MOVEEVENT));		}
};

struct IInstanceMagr : public IUnknownEx2
{
	virtual IInstanceSlave*			GetInstance			(uint64 ullInstanceID)				= 0;
	virtual IInstanceSlave*			GetPublicInstance	(uint32 ulMapID,int nLine = -1)		= 0;
	virtual IInstanceSlave*			GetInstanceByGroup	(uint64 ullGroup)					= 0;
	virtual IInstanceSlave*			CreateInstanceZone	(uint32 ulMapID,uint64 ullForActorID,IInstanceSlave*pOld)		= 0;	//ullForActorID����������MSG_INSTANCE_NEW/MSG_INSTANCE_INIT��
	virtual void					TraceInstanceList	()									= 0;
	virtual uint64					NewGroupID			()									= 0;
	virtual void					AddGroupMap			(uint64 ullGroupID,IInstanceSlave*pSlave,bool bFailTrace = true)= 0;
	virtual void					SendLineInfo		(IGamePlayer*pPlayer,uint32 ulMapID) const	= 0;

	//��Ҳ�����ʱ������
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
