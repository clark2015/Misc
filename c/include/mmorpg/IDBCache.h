#ifndef IDBCACHE_H_
#define IDBCACHE_H_
#include "../DLTypeDefs.h"
#include "../commInterfaceDef.h"
#include "../IUnknownEx.h"

struct IEnumLine
{
	//data == NULL���򷵻� false ��ʾ����
	//ע�ⲻҪ�ڻص�ʱ��insert��delete��update���ԣ�����,Ҳ������Sync()���������׵�����ʧЧ���Ƿ�����
	virtual bool OnLine(const uchar *data)	= 0;	
};

struct IEnumLineSet
{
	//p_line_set == NULL���򷵻� false ��ʾ�����������ڹ���������ɾ��line_set
	virtual bool OnLineSet(struct ILineSet*p_line_set)	= 0;	
};

//�м���
//�����е�key���ʵ���ϲ���uint64,��c����Ĭ�ϵķ�ʽת��
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
	//ɾ��������
	virtual void	Clear()										= 0;

	//�������
	virtual const void*ReadLine(uint64 key)					const	= 0;	//ע�ⲻҪ�ķ��ص�����!!
	virtual		  void*ReadLine(uint64 key,void *p_line)	const	= 0;

	virtual const void*FirstLine(iterator&it)				const	= 0;	//ע�ⲻҪ�ķ��ص�����!!
	virtual		  void*FirstLine(iterator&it,void *p_line)	const	= 0;
	virtual const void* NextLine(iterator&it)				const	= 0;	//ע�ⲻҪ�ķ��ص�����!!
	virtual		  void* NextLine(iterator&it,void *p_line)	const	= 0;

	//����ֵ >= key ����ֵ��С���ڵ��С�ע��IndexLineSet�п������ظ�key������
	virtual const void* GotoLine(uint64 key,iterator&it)				const	= 0;	//ע�ⲻҪ�ķ��ص�����!!
	virtual		  void* GotoLine(uint64 key,iterator&it,void *p_line)	const	= 0;

	virtual void		EnumLine(IEnumLine*pEnum)						= 0;
	virtual void		GetAllKeys(uint64 key[],uint32 &count)	const	= 0;

	//����
	virtual const char*	GetTableName	()		const	= 0;
	virtual uint		GetLineSize		()		const	= 0;
	virtual uint		GetLineCount	()		const	= 0;
	virtual uint		GetFieldCount	()		const	= 0;
	virtual uint64		GetOwnerID		()		const	= 0;	//���ֻ��һ������������IndexLineSet,����0,���򷵻�ownerid
	inline	bool		IsExist(uint64 key)		const	{	return ReadLine(key)	!= NULL;	}


	//�����ֶ���ȡ������
	virtual int			GetFieldIndex(const char*p_field_name)										const = 0;
	virtual uint64		GetIntFieldValueFromData(const void *p_line,int field_index)				const = 0;
	virtual const void*	GetFieldValuePtrFromData(const void *p_line,int field_index,int* p_len = 0)	const = 0;

	/*
	 ���롢�޸Ļ�ɾ����һ���к󣬰���ͬ����������SELECTȨ�޵ķ��������������key�����ڣ���֪ͨ�Է�ɾ����������öԷ�������޸�
	 ���͵Ĺ���
		1.ֻ������SELECTȨ�޵ķ�����
		2.���ᷢ�����ݿ������
		3.���û��owner�ֶΣ��򷢸�����DSIndex=0�ķ�����
		4.�����  owner�ֶΣ��ұ���  �� SERVER_TYPE_GAME_NODE_MASK ֮һ,��ֻ������ SERVER_TYPE_GAME_NODE_MASK ��DSIndex��ͬ�����з�����
		5.�����  owner�ֶΣ��ұ������� SERVER_TYPE_GAME_NODE_MASK ֮һ,�򷢸�DSIndex��ͬ�����з�����(��Ϊ�����ж�owner���ĸ�GameNode���ʱ����GameNode�õ�û�õ�����)
	 �Է��޸ĺ󣬲������ˢ�µ����ݿ�
	 ����CreateOfflineLineSet()�����ݣ��Է�����û�����line_set����ʱ�Է���ʧ��
	 ����CreateOfflineLineSet()�����ݣ�������Ȼ��delete��û��selectȨ�ޣ�����ܵ���������������ɾ��
	 ��������IndexLineSet���ã���Ϊ����key��һ����Ψһ��
	*/
	virtual void		SyncToNoneDB(uint64 key)	const	= 0;

	/*
		��DB��������������;
		�������key�ֶ��ұ�����uint64���͡�
		�ڴ��л���һ��key�����ﵽ�����ֵ��ֻ���ȡ>���ֵ������
		����һ��session id
	*/
	virtual uint32		LoadNewData	(uint32 query_id = 0)	= 0;
};

struct IDBCache2 : public IUnknownEx2
{
	//���漸���Ǹ�ģ���õ�
	virtual	bool			IsReady()				const 										= 0;	//�Ƿ��ѳ�ʼ����
	virtual void			Sync(bool bWriteDisk = false)										= 0;	//DB������bWriteDisk���ԣ�����д,�൱�ڵ���SyncMT(false,false);
	virtual uint64			NewGUID()															= 0;
	virtual uint32			GetProcessServerID()	const										= 0;	//ÿ�η�������������������һ����ͬ��ID������������LogID�ĸ�32λ
	virtual uint32			GetCountryID()														= 0;	//ȡ����ID������ӿڲ����ڿ����ȫ��ȫ���á�ȡ���ǺϷ���ID�����û�кϷ��ĸ���ͷ��ط�����ID����
	virtual void			TraceData			(const char*p_table_name,uint64* owner_or_key)	= 0;
	virtual void			TraceDataPtr		(const char*p_table_name,const void*ptr)		= 0;	//�ٶ�ptr��p_table_name���ʽ�ģ����ݽ�����Ϣ��ʾ��������
	virtual ILineSet*		GetLineSet			(const char*p_table_name)						= 0;	//���ֻ��һ��key�����
	virtual ILineSet*		GetLineSet			(const char*p_table_name,uint64		owner_id)	= 0;	//���������key�����
	virtual const ILineSet*	GetLineSet			(const char*p_table_name,const char*pIndexField)= 0;	//�ڱ����������������,�˽�����ڱ��������в����޸�
	virtual ILineSet*		CreateOfflineLineSet(const char*p_table_name,uint64 owner_id,bool bGetIfExist = true)	= 0;	//Ϊ���ߵ���Ҵ���LineSet������֧��д������ң������� OPT_LOAD = "ALL"������ô��	
	virtual uint32			CreateTrigger		(const char*p_table_name,uint32 method,uint32 _time = DOT_AFTER)	= 0;	//�ı�����ʱ���� MSG_TRIGGER ��Ϣ,����ֵ�Ǳ��ΨһID��Ŀǰֻ֧�� TRIGGER_AFTER
	virtual void			RemoveTrigger		(const char*p_table_name,uint32 method,uint32 _time = DOT_AFTER)	= 0;	//ֻ�е�����CreateTrigger()������ͬʱ�Ż�ȡ��
	virtual uint32			LoadReadonlyOwnerData(int ds_index,uint64 owner_id,const char*tables[],	uint32 query_id)= 0;	//ÿ�����ö�����һ��Ψһ��session id���Ա㷵��ʱȷ���ǲ����Լ��ġ��붩�� MSG_READONLY_OWNER_DATA

	virtual void			reserved0()	{}
	virtual void			reserved1()	{}
	virtual void			reserved2()	{}
	virtual void			reserved3()	{}
	virtual void			reserved4()	{}

	//�����Ǹ����������õ�
	virtual bool			IsLoaded			(uint64 owner_id)	const								= 0;	//���ݿ����LoadOwnerData()�ɹ������������������óɹ���ȡ������
	virtual bool			LoadOwnerData		(uint64 owner_id,int ds_index = -1,uint32 ulQueryID = 0)= 0;	//ֻҪ��һ����ȡʧ�ܣ��򷵻�false,�����ݿ���˵������db,��������������������db������
	virtual void			UnloadOwnerData		(uint64 owner_id,bool deleted_from_db = false)			= 0;	//���ڴ���ɾ����������sync
	virtual int				GetOwnerDSIndex		(uint64 owner_id)	const								= 0;	//�����û��load���ڴ棬�򷵻�-1
	virtual void			ClearOwnerMemoryOnly(uint64 owner_id)										= 0;
	virtual void			EnumLineSet			(const char*p_table_name,IEnumLineSet*pEnum)			= 0;

	virtual void			reserved5()	{}
	virtual void			reserved6()	{}
	virtual void			reserved7()	{}
	virtual void			reserved8()	{}
	virtual void			reserved9()	{}

	//�����Ǹ������ݿ�������õ�
	virtual void			SetDBConnect(class IDBConnect*p_center_connect,class IDBConnect*p_world_connect,class IDBConnect*p_sync_connect)= 0;
	virtual void			SyncMT(bool do_a_sync,bool wait)	= 0;

	virtual const void*		LoadAndReadLine(const char*p_table_name,uint64 owner_or_key)				= 0;
	virtual const void*		LoadAndReadLine(const char*p_table_name,uint64 owner_id,uint64 key)	= 0;

	virtual void			SendInitInsert (						uint64 owner_id,			const struct StServerInfo&Server)	= 0;
//	virtual void			SendInitInsert (						uint64 owner_id,uint32 srvtype,const struct StServerName&Server)	= 0;
	virtual void			SendInitUnload (						uint64 owner_id,			const struct StServerInfo&Server)	= 0;

	virtual void			SendInitInsert (const char*p_table_name,uint64 owner_id,			const struct StServerInfo&Server)	= 0;	//����������ò������û���õ�
	virtual void			SendInitUnload (const char*p_table_name,uint64 owner_id,			const struct StServerInfo&Server)	= 0;	//

	virtual void			SendInitInsert (const char*p_table_name,uint64 owner_id,uint64 key,	const struct StServerInfo&Server)	= 0;
	virtual void			SendInitUnload (const char*p_table_name,uint64 owner_id,uint64 key,	const struct StServerInfo&Server)	= 0;

	virtual uint32			GetPlatformID()		= 0;
	virtual uint32			GetFirstServerID()	= 0;

	enum
	{
		//Data Operation
		DO_LOAD			= 1,	//��ʼ��ʱinsert
		DO_LOAD_UPDATE	= 2,	//��ʼ��ʱinsert,����OPT_LOAD_ALLѡ��ı����ܻ������ظ��������Ĺ������ݡ��������delete�˵����ݣ���û�а취��֪�ˣ�
		DO_INSERT		= 4,
		DO_UPDATE		= 8,
		DO_REPLACE		= (DO_INSERT | DO_UPDATE),
		DO_DELETE		= 16,
		DO_MASK	= 0xFF,

		//Data Operation Time
		DOT_BEFORE		= 0x100,		//
		DOT_AFTER		= 0x200,		//Ŀǰֻ֧�� DOT_AFTER
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
