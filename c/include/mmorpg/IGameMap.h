#ifndef _I_GAME_MAP_H_
#define _I_GAME_MAP_H_
#include "IMmoModule.h"
#include "IGameObject.h"
#include "../ports.h"
struct IGameMapConfig;

//地图格子大小
#define TILE_WIDTH	32
#define TILE_HEIGHT	32

//生物及物品迭代器，请注意在遍历的过程中不能删除，移动，插入对象，否则迭代器出错后果很严重!
//注意遍历接口传入的RECT遵循windows的标准：左上角坐标包含，右下角坐标不包含
struct IGameObjectEnum
{
	virtual bool OnGameObject(OBJECT_TYPE objType,IGameObject*pObject)	= 0;	//返回false则停止
};

enum	INSTANCE_TYPE
{
	INSTANCE_TYPE_PUBLIC		= 0,	//公用地图,城镇
	INSTANCE_TYPE_NORMAL		= 1,	//普通副本
};

//地图接口
struct IGameMap : public IUnknownEx2
{
	virtual uint32		GetID()													const	= 0;
	virtual const char*	GetName()												const	= 0;
	virtual const char* GetInstanceName()										const	= 0;
	virtual int			GetWidth()												const	= 0;
	virtual int			GetHeight()												const	= 0;
	virtual const IGameMapConfig* GetConfig()									const	= 0;
	virtual	void		TraceMapInfo()											const	= 0;

	virtual bool		CanWalk		(int x,int y)								const	= 0;	//可否行走或游泳,相当于 CanStep(x,y) || CanSwim(x,y)
	inline	bool		CanWalk		(POINT& pt	)								const		{	return CanWalk(pt.x,			pt.y);				}
	inline	bool		CanTileWalk	(int x,int y)								const		{	return CanWalk(	  x*TILE_WIDTH,	   y*TILE_HEIGHT);	}
	inline	bool		CanTileWalk	(POINT& pt	)								const		{	return CanWalk(pt.x*TILE_WIDTH,	pt.y*TILE_HEIGHT);	}

	virtual bool		CanSwim		(int x,int y)								const	= 0;	//可否游泳
	inline	bool		CanSwim		(POINT& pt	)								const		{	return CanSwim(pt.x,			pt.y);				}
	inline	bool		CanTileSwim	(int x,int y)								const		{	return CanSwim(	  x*TILE_WIDTH,	   y*TILE_HEIGHT);	}
	inline	bool		CanTileSwim	(POINT& pt	)								const		{	return CanSwim(pt.x*TILE_WIDTH,	pt.y*TILE_HEIGHT);	}

	virtual bool		CanStep		(int x,int y)								const	= 0;	//可否行走
	inline	bool		CanStep		(POINT& pt	)								const		{	return CanStep(pt.x,			pt.y);				}
	inline	bool		CanTileStep	(int x,int y)								const		{	return CanStep(	  x*TILE_WIDTH,	   y*TILE_HEIGHT);	}
	inline	bool		CanTileStep	(POINT& pt	)								const		{	return CanStep(pt.x*TILE_WIDTH,	pt.y*TILE_HEIGHT);	}

	virtual bool		IsTileEmpty(int x,int y)								const	= 0;	//格子是否为空，参数为像素坐标

	virtual bool		GetObjectPos(const IGameObject*pObject,POINT &pt)		const	= 0;	//obj不在地图里就返回false
	virtual uint32		GetObjectCount(uint32 type = OBJECT_TYPE_ALL)			const	= 0;
	virtual IGameObject*GetObject	(uint64	ullGUID)									= 0;
	virtual void		GetObjects	(uint64 ullGUIDs[],uint32& count,uint32 type = OBJECT_TYPE_ALL)					const	= 0;	//不能传入指针数组，因为如果用来最后Release()时删除Object,那么去掉player可能会连带去掉它的坐骑，宠物等

	virtual void		InsertObject(IGameObject*p, int x,int y)						= 0;
	virtual void		RemoveObject(IGameObject*p)										= 0;
	virtual bool		MoveCreature(IGameCreature*p,int x,int y,
									IGameObject**&EnterMyView,
									IGameObject**&LeaveMyView,
									IGameObject**&EnterPlayerView,
									IGameObject**&LeavePlayerView)						= 0;

	virtual bool		CanMoveByLine(Location from,Location to)				const	= 0;
	virtual bool		FindCanWalkPoint(POINT& pt,const IGameCreature*p = 0,bool bFindEmptyTitle = false)			const	= 0;	//找一个可站人或可游泳的地方，指针是失败时trace用的。不管成功与否，都会得到一个地图内的地址

	/*寻路
	要使用返回值而不是直接用pPoints，因为返回的结果可能放在pPoints的末尾
	除非路径要绕很多弯，nCount传入大概40一般就够了,40*32=1280象素
	*/
	virtual POINT*		FindPath(int nFromX,int nFromY,int nToX,int nToY,POINT*pPoints,int& nCount,const IGameCreature*p)	const	= 0;	

	//遍历地图
	virtual bool		IsEnuming()	const																					= 0;	//不能嵌套遍历，不能在遍历时做插入删除移动等操作
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
	virtual uint32		GetMapSeriesID	()						const = 0;	//副本系列
	inline  bool		GetMapPrivate	()						const {	return GetMapSeriesID()	!= 0;	}
	virtual const char* GetMapName		()						const = 0;
	virtual const char* GetMapInstanceName()					const = 0;
	virtual const struct StServerName& 
						GetMapServer	()						const = 0;	//可以在哪个服务器开
	virtual		  char* GetMapPath		(char szPath[MAX_PATH])	const = 0;
	virtual		  char* GetMapFile		(char szFile[MAX_PATH])	const = 0;
	virtual		  char* GetMapSpellFile	(char szFile[MAX_PATH])	const = 0;
	virtual bool		GetMapIsEntry	()						const = 0;
	virtual INSTANCE_TYPE GetMapInstanceType()					const = 0;
	virtual bool		GetMapFight		()						const = 0;	//是否可以战斗，一般的副本、竟技场，和公用地图中的世界BOSS地图返回true
	virtual const char* GetMapClientExtendInfo ()				const = 0;	//客户端信息，NPC信息字符串，不会返回NULL
	virtual uint32		GetMapFreeReleaseSecond()				const = 0;	//空闲多少秒后删除,运行时将取所有同一大副本的已创建的副本的最大值

	//线路参数，一般在公用地图才会用到
	virtual int			GetMapLineCountDefault()				const = 0;	//默认开启线路条数
	virtual int			GetMapLineCountMax()					const = 0;	//开启线路条数上限
	virtual int			GetMapLineUserCountDefault()			const = 0;	//默认人数容量
	virtual int			GetMapLineUserCountInc()				const = 0;	//轮询人数增量
																	  
	virtual bool		CanJump()								const = 0;	//可否跳跃
	virtual bool		IsSafeArea(int x,int y)					const = 0;	//是否安全区域
	virtual bool		GetReturnCityPoint(uint32 &ulMapID,int &x,int &y)	const = 0;	//取回城点，如果返回false,请调用TransportToPrev()
};

struct IGameMapMagr : public IUnknownEx2
{
	virtual const IGameMapConfig*	GetMapConfig	(uint32 ulMapID)const	= 0;
	virtual IGameMap*				CreateGameMap	(uint32 ulMapID)		= 0;
	virtual	void					TraceMapList	()				const	= 0;
	virtual uint32					GetPublicMapIDInThisServer()	const	= 0;	//在本服务器找一个城镇地图ID
	virtual uint32					GetPublicMapIDGlobal()			const	= 0;	//找一个全局的城镇地图ID
	virtual void					GetAllPublicMapThisServer(uint32 ulMapIDs[],uint32 &count)	const	= 0;
	virtual IGameMap*				GetMapByGUID(uint64 ullID,bool bFindPublicInstance = false)	= 0;	//根据副本ID、玩家ID、NPCID、物品ID找到地图，可也选择从公有地图根据MapID取一个
	virtual bool					IsMyServerMap(uint32 ulMapID)	const	= 0;
	virtual uint32					GetNewbinMapID()										const = 0;
	virtual void					GetNewbinMap	 (uint32 &uiMapID,uint16 &x,uint16 &y)	const = 0;
	virtual void					GetFailDefaultMap(uint32 &uiMapID,uint16 &x,uint16 &y)	const = 0;

	//快捷接口
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
