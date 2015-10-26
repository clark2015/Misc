#ifndef IDBCACHE_H_
#define IDBCACHE_H_
#include "../DLTypeDefs.h"
#include "../commInterfaceDef.h"
#include "../IUnknownEx.h"

struct IEnumLine
{
	//data == NULL，或返回 false 表示结束
	//注意不要在回调时做insert（delete或update可以）操作,也不可以Sync()，否则容易迭代器失效，非法操作
	virtual bool OnLine(const uchar *data)	= 0;	
};

struct IEnumLineSet
{
	//p_line_set == NULL，或返回 false 表示结束；不得在过程中增加删除line_set
	virtual bool OnLineSet(struct ILineSet*p_line_set)	= 0;	
};

//行集合
//参数中的key如果实际上不是uint64,则按c语言默认的方式转换
struct ILineSet
{
	struct iterator
	{
		uint64	i0;
		uint64	i1;
	};
	
	//insert update delete
	virtual bool	InsertOrUpdateLine	(const void *p_line)	= 0;
	virtual bool	InsertLine			(const void *p_line)	= 0;
	virtual bool	UpdateLine			(const void *p_line)	= 0;
	virtual bool	DeleteLine			(uint64 key)			= 0;
	//删除所有行
	virtual void	Clear()										= 0;

	//读与遍历
	virtual const void*ReadLine(uint64 key)					const	= 0;	//注意不要改返回的内容!!
	virtual		  void*ReadLine(uint64 key,void *p_line)	const	= 0;

	virtual const void*FirstLine(iterator&it)				const	= 0;	//注意不要改返回的内容!!
	virtual		  void*FirstLine(iterator&it,void *p_line)	const	= 0;
	virtual const void* NextLine(iterator&it)				const	= 0;	//注意不要改返回的内容!!
	virtual		  void* NextLine(iterator&it,void *p_line)	const	= 0;

	//至键值 >= key 并键值最小所在的行。注意IndexLineSet中可能有重复key的数据
	virtual const void* GotoLine(uint64 key,iterator&it)				const	= 0;	//注意不要改返回的内容!!
	virtual		  void* GotoLine(uint64 key,iterator&it,void *p_line)	const	= 0;

	virtual void		EnumLine(IEnumLine*pEnum)						= 0;
	virtual void		GetAllKeys(uint64 key[],uint32 &count)	const	= 0;

	//属性
	virtual const char*	GetTableName	()		const	= 0;
	virtual uint		GetLineSize		()		const	= 0;
	virtual uint		GetLineCount	()		const	= 0;
	virtual uint		GetFieldCount	()		const	= 0;
	virtual uint64		GetOwnerID		()		const	= 0;	//如果只有一个键，或者是IndexLineSet,返回0,否则返回ownerid
	inline	bool		IsExist(uint64 key)		const	{	return ReadLine(key)	!= NULL;	}


	//根据字段名取得数据
	virtual int			GetFieldIndex(const char*p_field_name)										const = 0;
	virtual uint64		GetIntFieldValueFromData(const void *p_line,int field_index)				const = 0;
	virtual const void*	GetFieldValuePtrFromData(const void *p_line,int field_index,int* p_len = 0)	const = 0;

	/*
	 插入、修改或删除了一个行后，把它同步到其它有SELECT权限的服务器。如果参数key不存在，则通知对方删除，否则就让对方插入或修改
	 发送的规则：
		1.只发给有SELECT权限的服务器
		2.不会发给数据库服务器
		3.如果没有owner字段，则发给所有DSIndex=0的服务器
		4.如果有  owner字段，且本服  是 SERVER_TYPE_GAME_NODE_MASK 之一,则只发给非 SERVER_TYPE_GAME_NODE_MASK 且DSIndex相同的所有服务器
		5.如果有  owner字段，且本服不是 SERVER_TYPE_GAME_NODE_MASK 之一,则发给DSIndex相同的所有服务器(因为不好判断owner在哪个GameNode里，这时会有GameNode得到没用的数据)
	 对方修改后，不会把它刷新到数据库
	 对于CreateOfflineLineSet()的数据，对方可能没有这个line_set，这时对方会失败
	 对于CreateOfflineLineSet()的数据，或我虽然有delete但没有select权限，则可能导致其它服务器误删除
	 本函数对IndexLineSet无用，因为它的key不一定是唯一的
	*/
	virtual void		SyncToNoneDB(uint64 key)	const	= 0;

	/*
		向DB请求新增的数据;
		表必须有key字段且必须是uint64类型。
		内存中会有一个key曾经达到的最大值，只会读取>这个值的数据
		返回一个session id
	*/
	virtual uint32		LoadNewData	(uint32 query_id = 0)	= 0;
};

struct IDBCache2 : public IUnknownEx2
{
	//下面几个是给模块用的
	virtual	bool			IsReady()				const 										= 0;	//是否已初始化好
	virtual void			Sync(bool bWriteDisk = false)										= 0;	//DB服务器bWriteDisk忽略，总是写,相当于调用SyncMT(false,false);
	virtual uint64			NewGUID()															= 0;
	virtual uint32			GetProcessServerID()	const										= 0;	//每次服务器启动，都会生成一个不同的ID，可以用来做LogID的高32位
	virtual uint32			GetCountryID()														= 0;	//取国家ID。这个接口不能在跨服，全区全服用。取的是合服的ID，如果没有合服的概念，就返回服务器ID好了
	virtual void			TraceData			(const char*p_table_name,uint64* owner_or_key)	= 0;
	virtual void			TraceDataPtr		(const char*p_table_name,const void*ptr)		= 0;	//假定ptr是p_table_name表格式的，根据建表信息显示它的内容
	virtual ILineSet*		GetLineSet			(const char*p_table_name)						= 0;	//针对只有一个key的情况
	virtual ILineSet*		GetLineSet			(const char*p_table_name,uint64		owner_id)	= 0;	//针对有两个key的情况
	virtual const ILineSet*	GetLineSet			(const char*p_table_name,const char*pIndexField)= 0;	//在表有索引的情况下用,此结果集在遍历过程中不可修改
	virtual ILineSet*		CreateOfflineLineSet(const char*p_table_name,uint64 owner_id,bool bGetIfExist = true)	= 0;	//为离线的玩家创建LineSet，用于支持写离线玩家，必须有 OPT_LOAD = "ALL"才能这么做	
	virtual uint32			CreateTrigger		(const char*p_table_name,uint32 method,uint32 _time = DOT_AFTER)	= 0;	//改变数据时触发 MSG_TRIGGER 消息,返回值是表的唯一ID。目前只支持 TRIGGER_AFTER
	virtual void			RemoveTrigger		(const char*p_table_name,uint32 method,uint32 _time = DOT_AFTER)	= 0;	//只有调用与CreateTrigger()次数相同时才会取消
	virtual uint32			LoadReadonlyOwnerData(int ds_index,uint64 owner_id,const char*tables[],	uint32 query_id)= 0;	//每个调用都返回一个唯一的session id，以便返回时确定是不是自己的。请订阅 MSG_READONLY_OWNER_DATA

	virtual void			reserved0()	{}
	virtual void			reserved1()	{}
	virtual void			reserved2()	{}
	virtual void			reserved3()	{}
	virtual void			reserved4()	{}

	//下面是给服务器自用的
	virtual bool			IsLoaded			(uint64 owner_id)	const								= 0;	//数据库调用LoadOwnerData()成功，或其它服务器调用成功后取完数据
	virtual bool			LoadOwnerData		(uint64 owner_id,int ds_index = -1,uint32 ulQueryID = 0)= 0;	//只要有一个表取失败，则返回false,对数据库来说，它读db,对其它服务器，它请求db服务器
	virtual void			UnloadOwnerData		(uint64 owner_id,bool deleted_from_db = false)			= 0;	//从内存中删除，必须先sync
	virtual int				GetOwnerDSIndex		(uint64 owner_id)	const								= 0;	//如果还没有load进内存，则返回-1
	virtual void			ClearOwnerMemoryOnly(uint64 owner_id)										= 0;
	virtual void			EnumLineSet			(const char*p_table_name,IEnumLineSet*pEnum)			= 0;

	virtual void			reserved5()	{}
	virtual void			reserved6()	{}
	virtual void			reserved7()	{}
	virtual void			reserved8()	{}
	virtual void			reserved9()	{}

	//下面是给给数据库服务器用的
	virtual void			SetDBConnect(class IDBConnect*p_center_connect,class IDBConnect*p_world_connect,class IDBConnect*p_sync_connect)= 0;
	virtual void			SyncMT(bool do_a_sync,bool wait)	= 0;

	virtual const void*		LoadAndReadLine(const char*p_table_name,uint64 owner_or_key)				= 0;
	virtual const void*		LoadAndReadLine(const char*p_table_name,uint64 owner_id,uint64 key)	= 0;

	virtual void			SendInitInsert (						uint64 owner_id,			const struct StServerInfo&Server)	= 0;
//	virtual void			SendInitInsert (						uint64 owner_id,uint32 srvtype,const struct StServerName&Server)	= 0;
	virtual void			SendInitUnload (						uint64 owner_id,			const struct StServerInfo&Server)	= 0;

	virtual void			SendInitInsert (const char*p_table_name,uint64 owner_id,			const struct StServerInfo&Server)	= 0;	//这两个函数貌似现在没有用到
	virtual void			SendInitUnload (const char*p_table_name,uint64 owner_id,			const struct StServerInfo&Server)	= 0;	//

	virtual void			SendInitInsert (const char*p_table_name,uint64 owner_id,uint64 key,	const struct StServerInfo&Server)	= 0;
	virtual void			SendInitUnload (const char*p_table_name,uint64 owner_id,uint64 key,	const struct StServerInfo&Server)	= 0;

	virtual uint32			GetPlatformID()		= 0;
	virtual uint32			GetFirstServerID()	= 0;

	enum
	{
		//Data Operation
		DO_LOAD			= 1,	//初始化时insert
		DO_LOAD_UPDATE	= 2,	//初始化时insert,对有OPT_LOAD_ALL选项的表，可能会数据重复发来更改过的数据。（如果是delete了的数据，则没有办法得知了）
		DO_INSERT		= 4,
		DO_UPDATE		= 8,
		DO_REPLACE		= (DO_INSERT | DO_UPDATE),
		DO_DELETE		= 16,
		DO_MASK	= 0xFF,

		//Data Operation Time
		DOT_BEFORE		= 0x100,		//
		DOT_AFTER		= 0x200,		//目前只支持 DOT_AFTER
		DOT_MASK		= 0xFF00,
	};
};

/*
#ifdef _DEBUG
	#define DBCACHE2_DLL_NAME	"./DBCache2D"DLL_EXT
#else
	#define DBCACHE2_DLL_NAME	"./DBCache2"DLL_EXT
#endif
*/
#endif
