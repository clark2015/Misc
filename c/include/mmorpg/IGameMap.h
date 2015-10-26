#ifndef _I_GAME_MAP_H_
#define _I_GAME_MAP_H_
#include "IMmoModule.h"
#include "IGameObject.h"
#include "../ports.h"
struct IGameMapConfig;

//��ͼ���Ӵ�С
#define TILE_WIDTH	32
#define TILE_HEIGHT	32

//���Ｐ��Ʒ����������ע���ڱ����Ĺ����в���ɾ�����ƶ���������󣬷��������������������!
//ע������ӿڴ����RECT��ѭwindows�ı�׼�����Ͻ�������������½����겻����
struct IGameObjectEnum
{
	virtual bool OnGameObject(OBJECT_TYPE objType,IGameObject*pObject)	= 0;	//����false��ֹͣ
};

enum	INSTANCE_TYPE
{
	INSTANCE_TYPE_PUBLIC		= 0,	//���õ�ͼ,����
	INSTANCE_TYPE_NORMAL		= 1,	//��ͨ����
};

//��ͼ�ӿ�
struct IGameMap : public IUnknownEx2
{
	virtual uint32		GetID()													const	= 0;
	virtual const char*	GetName()												const	= 0;
	virtual const char* GetInstanceName()										const	= 0;
	virtual int			GetWidth()												const	= 0;
	virtual int			GetHeight()												const	= 0;
	virtual const IGameMapConfig* GetConfig()									const	= 0;
	virtual	void		TraceMapInfo()											const	= 0;

	virtual bool		CanWalk		(int x,int y)								const	= 0;	//�ɷ����߻���Ӿ,�൱�� CanStep(x,y) || CanSwim(x,y)
	inline	bool		CanWalk		(POINT& pt	)								const		{	return CanWalk(pt.x,			pt.y);				}
	inline	bool		CanTileWalk	(int x,int y)								const		{	return CanWalk(	  x*TILE_WIDTH,	   y*TILE_HEIGHT);	}
	inline	bool		CanTileWalk	(POINT& pt	)								const		{	return CanWalk(pt.x*TILE_WIDTH,	pt.y*TILE_HEIGHT);	}

	virtual bool		CanSwim		(int x,int y)								const	= 0;	//�ɷ���Ӿ
	inline	bool		CanSwim		(POINT& pt	)								const		{	return CanSwim(pt.x,			pt.y);				}
	inline	bool		CanTileSwim	(int x,int y)								const		{	return CanSwim(	  x*TILE_WIDTH,	   y*TILE_HEIGHT);	}
	inline	bool		CanTileSwim	(POINT& pt	)								const		{	return CanSwim(pt.x*TILE_WIDTH,	pt.y*TILE_HEIGHT);	}

	virtual bool		CanStep		(int x,int y)								const	= 0;	//�ɷ�����
	inline	bool		CanStep		(POINT& pt	)								const		{	return CanStep(pt.x,			pt.y);				}
	inline	bool		CanTileStep	(int x,int y)								const		{	return CanStep(	  x*TILE_WIDTH,	   y*TILE_HEIGHT);	}
	inline	bool		CanTileStep	(POINT& pt	)								const		{	return CanStep(pt.x*TILE_WIDTH,	pt.y*TILE_HEIGHT);	}

	virtual bool		IsTileEmpty(int x,int y)								const	= 0;	//�����Ƿ�Ϊ�գ�����Ϊ��������

	virtual bool		GetObjectPos(const IGameObject*pObject,POINT &pt)		const	= 0;	//obj���ڵ�ͼ��ͷ���false
	virtual uint32		GetObjectCount(uint32 type = OBJECT_TYPE_ALL)			const	= 0;
	virtual IGameObject*GetObject	(uint64	ullGUID)									= 0;
	virtual void		GetObjects	(uint64 ullGUIDs[],uint32& count,uint32 type = OBJECT_TYPE_ALL)					const	= 0;	//���ܴ���ָ�����飬��Ϊ����������Release()ʱɾ��Object,��ôȥ��player���ܻ�����ȥ��������������

	virtual void		InsertObject(IGameObject*p, int x,int y)						= 0;
	virtual void		RemoveObject(IGameObject*p)										= 0;
	virtual bool		MoveCreature(IGameCreature*p,int x,int y,
									IGameObject**&EnterMyView,
									IGameObject**&LeaveMyView,
									IGameObject**&EnterPlayerView,
									IGameObject**&LeavePlayerView)						= 0;

	virtual bool		CanMoveByLine(Location from,Location to)				const	= 0;
	virtual bool		FindCanWalkPoint(POINT& pt,const IGameCreature*p = 0,bool bFindEmptyTitle = false)			const	= 0;	//��һ����վ�˻����Ӿ�ĵط���ָ����ʧ��ʱtrace�õġ����ܳɹ���񣬶���õ�һ����ͼ�ڵĵ�ַ

	/*Ѱ·
	Ҫʹ�÷���ֵ������ֱ����pPoints����Ϊ���صĽ�����ܷ���pPoints��ĩβ
	����·��Ҫ�ƺܶ��䣬nCount������40һ��͹���,40*32=1280����
	*/
	virtual POINT*		FindPath(int nFromX,int nFromY,int nToX,int nToY,POINT*pPoints,int& nCount,const IGameCreature*p)	const	= 0;	

	//������ͼ
	virtual bool		IsEnuming()	const																					= 0;	//����Ƕ�ױ����������ڱ���ʱ������ɾ���ƶ��Ȳ���
	virtual	void		EnumInMapObject	(uint32	objType,										   IGameObjectEnum*pEnum)	= 0;
	virtual void		EnumISeeObject	(const	IGameObject*pObj,bool	bWithMyself,uint32 objType,IGameObjectEnum*pISee )	= 0;
	virtual void		EnumSeeMeObject	(const	IGameObject*pObj,bool	bWithMyself,uint32 objType,IGameObjectEnum*pSeeMe)	= 0;
	virtual void		EnumSeeObject	(const	IGameObject*pObj,bool	bWithMyself,uint32 objType,IGameObjectEnum*pISee,IGameObjectEnum*pSeeMe)	= 0;

	virtual	void		EnumInRectObject	(const RECT &rect,		 float angle,uint32	objType,IGameObjectEnum*pInRect	)	= 0;
	virtual	void		EnumInCircleObject	(int x,int y,int radius,			 uint32	objType,IGameObjectEnum*pIn		)	= 0;
	virtual	void		EnumInEllipseObject	(int x,int y,int a,int b,float angle,uint32	objType,IGameObjectEnum*pInCircle)	= 0;
};

//************************************************************************************************************
struct IGameMapConfig
{
	virtual uint32		GetMapID		()						const = 0;
	virtual uint32		GetMapSeriesID	()						const = 0;	//����ϵ��
	inline  bool		GetMapPrivate	()						const {	return GetMapSeriesID()	!= 0;	}
	virtual const char* GetMapName		()						const = 0;
	virtual const char* GetMapInstanceName()					const = 0;
	virtual const struct StServerName& 
						GetMapServer	()						const = 0;	//�������ĸ���������
	virtual		  char* GetMapPath		(char szPath[MAX_PATH])	const = 0;
	virtual		  char* GetMapFile		(char szFile[MAX_PATH])	const = 0;
	virtual		  char* GetMapSpellFile	(char szFile[MAX_PATH])	const = 0;
	virtual bool		GetMapIsEntry	()						const = 0;
	virtual INSTANCE_TYPE GetMapInstanceType()					const = 0;
	virtual bool		GetMapFight		()						const = 0;	//�Ƿ����ս����һ��ĸ��������������͹��õ�ͼ�е�����BOSS��ͼ����true
	virtual const char* GetMapClientExtendInfo ()				const = 0;	//�ͻ�����Ϣ��NPC��Ϣ�ַ��������᷵��NULL
	virtual uint32		GetMapFreeReleaseSecond()				const = 0;	//���ж������ɾ��,����ʱ��ȡ����ͬһ�󸱱����Ѵ����ĸ��������ֵ

	//��·������һ���ڹ��õ�ͼ�Ż��õ�
	virtual int			GetMapLineCountDefault()				const = 0;	//Ĭ�Ͽ�����·����
	virtual int			GetMapLineCountMax()					const = 0;	//������·��������
	virtual int			GetMapLineUserCountDefault()			const = 0;	//Ĭ����������
	virtual int			GetMapLineUserCountInc()				const = 0;	//��ѯ��������
																	  
	virtual bool		CanJump()								const = 0;	//�ɷ���Ծ
	virtual bool		IsSafeArea(int x,int y)					const = 0;	//�Ƿ�ȫ����
	virtual bool		GetReturnCityPoint(uint32 &ulMapID,int &x,int &y)	const = 0;	//ȡ�سǵ㣬�������false,�����TransportToPrev()
};

struct IGameMapMagr : public IUnknownEx2
{
	virtual const IGameMapConfig*	GetMapConfig	(uint32 ulMapID)const	= 0;
	virtual IGameMap*				CreateGameMap	(uint32 ulMapID)		= 0;
	virtual	void					TraceMapList	()				const	= 0;
	virtual uint32					GetPublicMapIDInThisServer()	const	= 0;	//�ڱ���������һ�������ͼID
	virtual uint32					GetPublicMapIDGlobal()			const	= 0;	//��һ��ȫ�ֵĳ����ͼID
	virtual void					GetAllPublicMapThisServer(uint32 ulMapIDs[],uint32 &count)	const	= 0;
	virtual IGameMap*				GetMapByGUID(uint64 ullID,bool bFindPublicInstance = false)	= 0;	//���ݸ���ID�����ID��NPCID����ƷID�ҵ���ͼ����Ҳѡ��ӹ��е�ͼ����MapIDȡһ��
	virtual bool					IsMyServerMap(uint32 ulMapID)	const	= 0;
	virtual uint32					GetNewbinMapID()										const = 0;
	virtual void					GetNewbinMap	 (uint32 &uiMapID,uint16 &x,uint16 &y)	const = 0;
	virtual void					GetFailDefaultMap(uint32 &uiMapID,uint16 &x,uint16 &y)	const = 0;

	//��ݽӿ�
	inline  bool			IsMapInfoExist		(uint32 ulMapID)						const {	return	GetMapConfig(ulMapID) != 0;	}
	inline  bool			GetMapPrivateByID	(uint32 ulMapID)						const {	const IGameMapConfig*p = GetMapConfig(ulMapID);	if ( !p ) return false;	return p->GetMapPrivate();	}
	inline  uint32			GetMapSeriesIDByID	(uint32 ulMapID)						const {	const IGameMapConfig*p = GetMapConfig(ulMapID);	if ( !p ) return false;	return p->GetMapSeriesID();	}
	inline  bool			GetMapIsEntryByID	(uint32 ulMapID)						const {	const IGameMapConfig*p = GetMapConfig(ulMapID);	if ( !p ) return false;	return p->GetMapIsEntry();	}
	inline  const char*		GetMapNameByID		(uint32 ulMapID)						const {	const IGameMapConfig*p = GetMapConfig(ulMapID);	if ( !p ) return NULL;	return p->GetMapName();		}
	inline  const struct	StServerName* 
							GetMapServerByID	(uint32 ulMapID)						const {	const IGameMapConfig*p = GetMapConfig(ulMapID);	if ( !p ) return NULL;	return &p->GetMapServer();	}
	inline   bool			GetMapFightByID		(uint32 ulMapID)						const {	const IGameMapConfig*p = GetMapConfig(ulMapID);	if ( !p ) return false;	return p->GetMapFight();	}
	inline  	  char*		GetMapPathByID		(uint32 ulMapID,char szPath[MAX_PATH])	const {	const IGameMapConfig*p = GetMapConfig(ulMapID);	if ( !p ) {	szPath[0] = 0;	return szPath;	}	return p->GetMapPath(szPath);		}
	inline  	  char*		GetMapFileByID		(uint32 ulMapID,char szFile[MAX_PATH])	const {	const IGameMapConfig*p = GetMapConfig(ulMapID);	if ( !p ) {	szFile[0] = 0;	return szFile;	}	return p->GetMapFile(szFile);		}
	inline  	  char*		GetMapSpellFileByID	(uint32 ulMapID,char szFile[MAX_PATH])	const {	const IGameMapConfig*p = GetMapConfig(ulMapID);	if ( !p ) {	szFile[0] = 0;	return szFile;	}	return p->GetMapSpellFile(szFile);	}
};

//************************************************************************************************************

#endif
