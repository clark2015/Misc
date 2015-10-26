//几个数据库操作类
/*
2002-9    		蓝琚成			建立文件，完成CDBConnect,CPgSQL,CQuery
2002-10-7		蓝琚成			修改了一些bug,把struct DBDepend变成内部类
2003-05-26 		蓝琚成			增加exception.h,使本文件不依赖于tinycxx
2006-08-16/17	蓝琚成			文件及类名更改CDBConnect -> IDBConnect
								提练成接口，string变成char*等
								实现CMysql
								去掉exception.h的依赖，不抛出异常
								增加EnumData()
								让IQueryResult可以Placement new出来
2007-05-16		蓝琚成			增加64位int(sign/unsign)参数的支持
2008-01-11		蓝琚成	v5.0.0	继承IUnknownEx2
								增加取查询次数的功能
2008-01-25/28	蓝琚成	v6.0.0	支持BLOB类型(实现PushParamBLOB())
								增大了参数可容许的长度
								增加ValueLength()接口
								IDataEnumSink::OnFieldValue()加长度参数
								修正了一些buff的bug
2008-04-04至06	蓝琚成	v7.0.0	增加SQLite支持
								AddQuote()增加bBlob参数
								几个事务函数返回布尔值,原来是void的
								增加DBDependOption()接口,无实现
								改名IDBDepend -> IQueryResult
2008-04-23		蓝琚成	v8.0.0	增加GetQueryTime()
2008-05-26		蓝琚成	v9.0.0	MAX_PARAM_COUNT 20->64,因CQuery的buff不够,必须加版本
								ExecSQL()执行多条语句会断开连接的问题
2008-12-30		蓝琚成	v9.0.1	接口CLASSUID原来写成"IUnknowEx2接口"了
2009-03-24		蓝琚成	v10.0.0	OpenSQL()和CallProcedure()加 bMaybeNoResult 参数
2010-10-25		蓝琚成	v10.0.1	增加SQL Server的支持
	至11-xx						MAX_PARAM_COUNT 100->64，应该足够了，否则内部不够用了
2010-11-16		蓝琚成	v10.0.1	CQuery增加单个参数的PushParamInt64()/PushParamUInt64()
2011-03-03		蓝琚成	v11.0.0	增加Ping()
								修正mysql执行出错数据库改变的问题
								mysql重连设置用mysql_options()
2012-11-02		蓝琚成	v12.0.0	增加取一行所有列值的 FieldValues()
								增加EnumData2()
2012-12-18		蓝琚成	v13.0.0	增加IStatement及相应处理




*/

#ifndef _IDBCONNECT_H_
#define _IDBCONNECT_H_

#include "../IUnknownEx.h"
#include "../commInterfaceDef.h"

class CQuery;

#define IDBCONNECT_VER_MAIN		13
#define IDBCONNECT_VER_SUB		0
#define IDBCONNECT_VER_BUILD	0

static const CLASSUID	IF_UID_DBCONNECT ("IDBConnect接口");
static const ulong		IF_VER_DBCONNECT = INTERFACE_VER(IDBCONNECT_VER_MAIN,IDBCONNECT_VER_SUB);
//********************************************************************************

class IDBConnect : public IUnknownEx2
{
public:
	//数据库连接
	virtual bool Connect(const char*pHost,ushort usPort,const char *pDBName,
					const char*pUserName,const char *pPasswd,void *pReserved = 0) = 0;
	virtual void DisConnect() = 0;

	//事务支持
	virtual bool BeginTransaction() = 0;
	virtual bool Commit  ()         = 0;
	virtual bool Rollback()         = 0;

	//返回数据库时间的函数名，如"now()"
	virtual const char * GetTimeFucntion(bool bBracket = true) const = 0;   

	//用引号括住字符串pSrc,并替换逃逸字符,好加到SQL语句中
	//必须: nSrcLen = pSrc长度(字符串则是strlen(pSrc))
	//      sizeof(szDes) >= nSrcLen*2 + 4 (mysql中是+3,SQLite是+4)
	//成功返回szDes,否则返回NULL
	virtual char*	AddQuote(const void*pSrc,char szDes[],int nSrcLen,ulong bBlob = false) const = 0;

	//取查询调用(OpenSQL,EexcSQL,CallProcedure,CallFunction)的次数,注意存储过程里可能有几条查询
	virtual ulong	GetQueryCount()						const	= 0;

	//取数据库调用花费了多少时间
	virtual ulong	GetQueryTime(ulong ulReserved = 0)	const	= 0;

	//获取或设置与数据库相关的选项
	virtual ulong	DBDependOption(ulong ulCode,ulong ulParam0=0,ulong ulParam1=0,ulong ulParam2=0)	{	return 0;	}

	//ping以确认数据库连接
	virtual bool	Ping()	{	return false;	}

	struct IStatement : protected IUnknownEx2
	{
		friend class CQuery;

		//设置参数值及格式
		virtual void SetParamChar	(int n,char		v)	= 0;
		virtual void SetParamUChar	(int n,uchar	v)	= 0;
		virtual void SetParamShort	(int n,short	v)	= 0;
		virtual void SetParamUShort	(int n,ushort	v)	= 0;
		virtual void SetParamInt	(int n,int		v)	= 0;
		virtual void SetParamUInt	(int n,uint		v)	= 0;
		virtual void SetParamInt64	(int n,longlong	v)	= 0;
		virtual void SetParamUInt64	(int n,ulonglong v)	= 0;
		virtual void SetParamFloat	(int n,float    v)	= 0;		//好吧，跟IDBConnect::PushParamFloat()不太一致
		virtual void SetParamDouble	(int n,double   v)	= 0;
		virtual void SetParamStr	(int n,const char*pStr, int nMaxLen)			= 0;	//这个实际上是var str,还有一个固定的，先不管了
		virtual void SetParamBlob	(int n,const void*pBlob,int nLen,int nMaxLen)	= 0;

		//设置输出值及格式
		virtual void SetResultFormatInteger	(int n,int size,bool bUnsigned)	= 0;
		virtual void SetResultFormatFloat	(int n)							= 0;
		virtual void SetResultFormatDouble	(int n)							= 0;
		virtual void SetResultFormatStr		(int n,int size)				= 0;
		virtual void SetResultFormatBlob	(int n,int size)				= 0;

		//取结果
		virtual uint GetDataLen()	const = 0;
	//	virtual void*First()	= 0;
		virtual void*Next()		= 0;
	};

	struct IQueryResult
	{
		virtual void ClearData() = 0;
		virtual void Release()	 = 0;
	};
	enum	{	MAX_PARAM_COUNT	= 64	};
protected:
	friend class CQuery;
	//以下函数，供CQuery使用

	//CQuery中与数据库相关的结构
	virtual struct IQueryResult * NewQueryResult(void *pPlacement,int nLen) = 0;

	//执行SQL命令
	virtual bool ExecSQL(IQueryResult *pQueryResult,const char* pSQL) = 0;
	virtual bool OpenSQL(IQueryResult *pQueryResult,const char* pSQL,bool bMaybeNoResult = false) = 0;
	
	virtual IStatement * CreateStatement(const char*pSQL,int nSQLLen)	= 0;
	virtual bool ExecStatement(IQueryResult *pQueryResult,IStatement* pStmt) = 0;
	virtual bool OpenStatement(IQueryResult *pQueryResult,IStatement* pStmt,bool bMaybeNoResult = false) = 0;

	//执行存储过程和函数
	// 1.暂不支持out参数，结果全部以数据集形式返回
	// 2.目前只支持压入MAX_PARAM_COUNT个参数，调用过程/函数后，会自动调用CleanParam()清除压入的参数
	// 3.所有的指针参数所指的内容都不会被复制，请自己保证调用过程/函数前指钟所指内容仍有效
	// 4.参数自左至右入栈
	virtual bool PushParamInt	(IQueryResult *pQueryResult,int i)					= 0;
	virtual bool PushParamUInt	(IQueryResult *pQueryResult,unsigned int i)			= 0;
	virtual bool PushParamInt64 (IQueryResult *pQueryResult,UINT high,UINT low)		= 0;
	virtual bool PushParamUInt64(IQueryResult *pQueryResult,UINT high,UINT low)		= 0;
	virtual bool PushParamIntStr(IQueryResult *pQueryResult,const char*pStr)		= 0;
	virtual bool PushParamStr	(IQueryResult *pQueryResult,const char*pStr)		= 0;
	virtual bool PushParamFloat	(IQueryResult *pQueryResult,double d)				= 0;
	virtual bool PushParamBLOB	(IQueryResult *pQueryResult,const void*p,ushort len)= 0;	//注意不要传太长的，现在有一个40k长的参数应该可以正常处理，再大就不能保证了
	virtual void CleanParam		(IQueryResult *pQueryResult) 						= 0;
	virtual bool CallProcedure	(IQueryResult *pQueryResult,const char*pName,bool bWithRecordSet,bool bMaybeNoResult = false)	= 0;
	virtual bool CallFunction	(IQueryResult *pQueryResult,const char*pName,const char*pAs = NULL)	= 0;

	//SQL命令返回的结果
	virtual int			 ColCount  (IQueryResult *pQueryResult)  const = 0;
	virtual int			 LineCount (IQueryResult *pQueryResult)  const = 0;
	virtual const char * FieldName (IQueryResult *pQueryResult,int nCol)									const = 0;
	virtual const char * FieldValue(IQueryResult *pQueryResult,int nLine,int nCol,ulong *pulLen=0)			const = 0;
	virtual int			FieldValues(IQueryResult *pQueryResult,int nLine,const char*values[],ulong pulLen[]=0)	const = 0;
	virtual ulong		ValueLength(IQueryResult *pQueryResult,int nLine,int nCol)							const = 0;

	//遍历数据。虽然上面的FieldValue()也可以做到，但：
	// 1. mysql的结果集是个链表，在查找行时有些慢
	// 2. 需要多次调用FieldValue()时做很多无谓的检查
	virtual void EnumData (IQueryResult* pQueryResult,class IDataEnumSink* pSink,bool bNeedFieldName = true,bool bNeedFieldValue = true)	const = 0;
	virtual void EnumData2(IQueryResult* pQueryResult,class IDataEnumSink2*pSink,bool bNeedFieldName = true,bool bNeedFieldValue = true)	const = 0;
};

//遍历操作的接收器
class IDataEnumSink
{
public:
	//得到字段名，返回false则结束，但仍会调用OnFieldValue()
	virtual bool OnFieldName (int nCol,const char*pName)	= 0;

	//开始新的一行，返回false则结束
	virtual bool OnNewLine   (int nLine) = 0;	
	//得到字段值，返回false则结束
	virtual bool OnFieldValue(int nLine,int nCol,const char*pValue,ulong ulValueLen)	= 0;

	//不论上面的是否返回false，都会调用这个
	virtual void OnEnumEnd()	= 0;
};

class IDataEnumSink2
{
public:
	//字段名,注意参数指针调用完就无效
	virtual void OnFieldName (const char*names[])	= 0;	

	//开始新的一行，返回false则结束
	virtual bool OnNewLine   (int nLine,const char*values[],ulong lengths[]) = 0;	

	//不论上面的是否返回false，都会调用这个
	virtual void OnEnumEnd()	= 0;
};

//********************************************************************************
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

class CQuery
{
protected:
	int m_nLine;

	IDBConnect *m_pDBC;
	IDBConnect::IQueryResult*	m_pQueryResult;
	IDBConnect::IStatement*		m_pStmt;
	
	char	m_DbDep[1792];
public:
	CQuery(IDBConnect *p)
	{
		m_pStmt			= NULL;
		m_pQueryResult	= NULL;
		SetDB(p);
	}
	~CQuery()   
	{
		if (m_pQueryResult)	
			m_pQueryResult->Release();     
		if ( m_pStmt )
			m_pStmt->Release();
	}

	void SetDB(IDBConnect *p)
	{
		if ( m_pQueryResult )
		{
			m_pQueryResult->Release();
			m_pQueryResult = NULL;
		}
		if ( m_pStmt )
		{
			m_pStmt->Release();
			m_pStmt = NULL;
		}
		m_pDBC  = p;
		m_nLine = 0;
		if ( m_pDBC )
			m_pQueryResult = p->NewQueryResult(m_DbDep,sizeof(m_DbDep));
	}

	//清空查询结果集
	void Close()        {   if (m_pQueryResult) m_pQueryResult->ClearData();  }

	//执行SQL语句，无返回结果
	bool ExecSQL(const char* SQL)								{   return m_pDBC->ExecSQL(m_pQueryResult,SQL);		}
	//执行SQL语句，并取返回的结果集
	bool OpenSQL(const char* SQL,bool bMaybeNoResult = false)	{   return m_pDBC->OpenSQL(m_pQueryResult,SQL,bMaybeNoResult);	}

	virtual bool CreateStatement(const char*pSQL,int nSQLLen)
	{
		if ( m_pStmt )
		{
			assert(0);	//不知道这样结果集要不要释放
			m_pStmt->Release();
		}
		m_pStmt = m_pDBC->CreateStatement(pSQL,nSQLLen);
		return !!m_pStmt;
	}
	virtual IDBConnect::IStatement*	GetStatement()			{	return m_pStmt;	}
	virtual bool ExecStatement()							{	return m_pDBC->ExecStatement(m_pQueryResult,m_pStmt);					}
	virtual bool OpenStatement(bool bMaybeNoResult = false)	{	return m_pDBC->OpenStatement(m_pQueryResult,m_pStmt,bMaybeNoResult);	}

	//执行存储过程和函数
	//请看IDBConnect中相应的说明
	bool PushParamInt	(int i)					{	return m_pDBC->PushParamInt	  (m_pQueryResult,i);		}
	bool PushParamUInt	(unsigned int i)		{	return m_pDBC->PushParamUInt  (m_pQueryResult,i);		}
	bool PushParamInt64 (UINT high,UINT low)	{	return m_pDBC->PushParamInt64 (m_pQueryResult,high,low);	}
	bool PushParamInt64 (longlong  l)			{	return m_pDBC->PushParamInt64 (m_pQueryResult,HIDWORD(l), LODWORD(l));	}
	bool PushParamUInt64(UINT high,UINT low)	{	return m_pDBC->PushParamUInt64(m_pQueryResult,high,low);	}
	bool PushParamUInt64(ulonglong ul)			{	return m_pDBC->PushParamUInt64(m_pQueryResult,HIDWORD(ul),LODWORD(ul));	}
	bool PushParamIntStr(const char*pStr)		{	return m_pDBC->PushParamIntStr(m_pQueryResult,pStr);		}
	bool PushParamStr	(const char*pStr)		{	return m_pDBC->PushParamStr	  (m_pQueryResult,pStr);		}
	bool PushParamFloat	(double d)				{	return m_pDBC->PushParamFloat (m_pQueryResult,d);		}
	bool PushParamBLOB	(const void*p,ushort len){	return m_pDBC->PushParamBLOB  (m_pQueryResult,p,len);	}
	void CleanParam		() 						{	m_pDBC->CleanParam(m_pQueryResult);						}
	bool CallProcedure	(const char*pName,bool bWithRecordSet = true,bool bMaybeNoResult = false)	{	return m_pDBC->CallProcedure(m_pQueryResult,pName,bWithRecordSet,bMaybeNoResult);	}
	bool CallFunction	(const char*pName,const char*pAs = NULL)									{	return m_pDBC->CallFunction (m_pQueryResult,pName,pAs);								}

	//取结果集的大小
	int ColCount () const   {   return m_pDBC->ColCount(m_pQueryResult);     }
	int LineCount() const   {   return m_pDBC->LineCount(m_pQueryResult);    }

	//取字段名及值
	const char * FieldName  (		   int nCol)					const {	return m_pDBC->FieldName (m_pQueryResult,nCol);					}
	const char * FieldValue (int nLine,int nCol,ulong *pulLen=0)	const {	return m_pDBC->FieldValue(m_pQueryResult,  nLine,nCol,pulLen);	}
	const char * FieldValue (		   int nCol,ulong *pulLen=0)	const {	return m_pDBC->FieldValue(m_pQueryResult,m_nLine,nCol,pulLen);	}
	int			 FieldValues(int nLine,const char*v[],ulong l[])	const {	return m_pDBC->FieldValues(m_pQueryResult,nLine,  v,l);			}
	int			 FieldValues(		   const char*v[],ulong l[])	const {	return m_pDBC->FieldValues(m_pQueryResult,m_nLine,v,l);			}
	ulong		 ValueLength(int nLine,int nCol)					const {	return m_pDBC->ValueLength(m_pQueryResult,  nLine,nCol);		}
	ulong		 ValueLength(		   int nCol)					const {	return m_pDBC->ValueLength(m_pQueryResult,m_nLine,nCol);		}

	//取字段值，并转化成char*类型
	const char* StrValue  (const char *pFieldName)	{   return FieldValue(FieldNum(pFieldName));        }
	const char* StrValue  (int nCol)				{   return FieldValue(nCol);						}
	const char* StrValue  (int nLine,int nCol)		{   return FieldValue(nLine,nCol);					}
	//取字段值，并转化成int类型
	int			IntValue  (const char *pFieldName)  {   return atoi(FieldValue(FieldNum(pFieldName)));  }
	int			IntValue  (int nCol)				{   return atoi(FieldValue(nCol));					}
	int			IntValue  (int nLine,int nCol)		{   return atoi(FieldValue(nLine,nCol));			}
	//取字段值，并转化成double类型
	double		FloatValue(const char *pFieldName)  {   return atof(FieldValue(FieldNum(pFieldName)));  }
	double		FloatValue(int nCol)				{   return atof(FieldValue(nCol));					}
	double		FloatValue(int nLine,int nCol)		{   return atof(FieldValue(nLine,nCol));			}

	//取字段名对应的列号
	int FieldNum(const char *pFieldName)        const
	{
		if ( NULL == pFieldName )
		{
			assert(pFieldName);
			return -1;
		}
		for ( int i=ColCount()-1; i>=0; i-- )
			if ( ! strcmp(FieldName(i),pFieldName) )    //相等则返回
				return i;
		return -1;
	}

	//支持遍历
	void First()    {   m_nLine = 0;  }
	void Next()     {   m_nLine++;    }
	bool IsEnd()	{   return m_nLine >= LineCount();    }

	void EnumData (class IDataEnumSink* pSink,bool bNeedFieldName = true,bool bNeedFieldValue = true)	const	{	m_pDBC->EnumData (m_pQueryResult,pSink,bNeedFieldName,bNeedFieldValue);	}
	void EnumData2(class IDataEnumSink2*pSink,bool bNeedFieldName = true,bool bNeedFieldValue = true)	const	{	m_pDBC->EnumData2(m_pQueryResult,pSink,bNeedFieldName,bNeedFieldValue);	}

	//其它
	const char* GetTimeFucntion(bool bBracket = true) const { return m_pDBC->GetTimeFucntion(bBracket); }
	char* AddQuote(const void*pSrc,char szDes[],int nSrcLen,ulong bBlob = false) const { return m_pDBC->AddQuote(pSrc,szDes,nSrcLen,bBlob);	}

private:
	//禁止复制和赋值
	CQuery(const CQuery& query)				{	assert(0);	}
	void operator = (const CQuery& query)	{	assert(0);	}
};

//********************************************************************************

#include "DBCommon.h"

class CDBConnectFactory
{
	IDBConnect* m_pInterface;
	HMODULE 	m_hLib;
	HMODULE 	m_hPreload;

	typedef IDBConnect * (*_CreateDbConnect)(ulong ulVerMain,ulong ulVerSub,ulong ulVerBuild,ulong ulReserved);

	const char* GetDllName(enDB_TYPE type)
	{
		switch (type)
		{
		case DBTYPE_MYSQL:
			return "./MySqlConnect";
		case DBTYPE_PGSQL:
			return "./PgSqlConnect";
		case DBTYPE_SQLITE:
			return "./SQLiteConnect";
		case DBTYPE_SQLSERVER:
			return "./SQLServerConnect";
		default:
			assert(0);
			return "无名字的歌";
		}
	}
	const char* GetPreloadDllName(enDB_TYPE type)
	{
	#ifndef _WIN32
		if ( DBTYPE_SQLITE == type )
			return "./libsqlite3.so.0";
	#endif
		return NULL;
	}
public:
	CDBConnectFactory()
	{
		m_pInterface = NULL;
		m_hLib		 = NULL;
		m_hPreload	 = NULL;
	}
	~CDBConnectFactory()
	{
		Close();
	}

	void Close()
	{
		if ( m_pInterface )
		{
			m_pInterface->Release();
			m_pInterface  = NULL;
		}
		if ( m_hLib )
		{
			FreeLibrary(m_hLib);
			m_hLib  = NULL;
		}
		if ( m_hPreload )
		{
			FreeLibrary(m_hPreload);
			m_hPreload  = NULL;
		}
	}

	bool Create(enDB_TYPE type)
	{
		if ( m_pInterface )
		{
			RelTrace("CDBConnectFactory::Create() : pls close first");
			return false;
		}

		{
			const char*pPreload = GetPreloadDllName(type);
			if ( pPreload )
			{
				m_hPreload = LoadLibrary(pPreload);
				if ( NULL == m_hPreload )
				{
				#ifdef _WIN32
					RelTrace("Can not load dll:%s,GetLastError()=%u",pPreload,GetLastError());
				#else
					RelTrace("Can not load dll:%s,dlerror=%s",pPreload,dlerror());
				#endif
					return false;
				}
			}
		}
		char szDllName[1024];
	#ifdef _DEBUG
		wsprintf(szDllName,"%sD%s",GetDllName(type),DLL_EXT);
	#else
		wsprintf(szDllName,"%s%s", GetDllName(type),DLL_EXT);
	#endif

		m_hLib = LoadLibrary (szDllName);
		if ( NULL == m_hLib )
		{
		#ifdef _WIN32
			RelTrace("Can not load dll:%s,GetLastError()=%u",szDllName,GetLastError());
		#else
			RelTrace("Can not load dll:%s,dlerror=%s",szDllName,dlerror());
		#endif
			Close();
			return false;
		}
	
		_CreateDbConnect createfunc;
		createfunc  =  (_CreateDbConnect )GetProcAddress(m_hLib, "CreateDbConnect");
		if ( NULL == createfunc )
		{
		#ifdef _WIN32
			RelTrace("function CreateDbConnect() not found,GetLastError()=%u",GetLastError());
		#else
			RelTrace("function CreateDbConnect() not found,dlerror=%s",dlerror());
		#endif
			Close();
			return false;
		}
		m_pInterface = createfunc(IDBCONNECT_VER_MAIN,IDBCONNECT_VER_SUB,IDBCONNECT_VER_BUILD,0);

		if ( NULL == m_pInterface )
		{
			Close();
			return false;
		}

		return true;
	}

	bool  IsValid()	const					{	return m_pInterface != NULL;	}
		  IDBConnect* GetInterface()		{	return m_pInterface;			}
	const IDBConnect* GetInterface() const	{	return m_pInterface;			}
		  IDBConnect* operator ->()			{	return m_pInterface;			}
	const IDBConnect* operator ->()	 const	{	return m_pInterface;			}
};
#endif
