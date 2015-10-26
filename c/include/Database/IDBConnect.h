//�������ݿ������
/*
2002-9    		��袳�			�����ļ������CDBConnect,CPgSQL,CQuery
2002-10-7		��袳�			�޸���һЩbug,��struct DBDepend����ڲ���
2003-05-26 		��袳�			����exception.h,ʹ���ļ���������tinycxx
2006-08-16/17	��袳�			�ļ�����������CDBConnect -> IDBConnect
								�����ɽӿڣ�string���char*��
								ʵ��CMysql
								ȥ��exception.h�����������׳��쳣
								����EnumData()
								��IQueryResult����Placement new����
2007-05-16		��袳�			����64λint(sign/unsign)������֧��
2008-01-11		��袳�	v5.0.0	�̳�IUnknownEx2
								����ȡ��ѯ�����Ĺ���
2008-01-25/28	��袳�	v6.0.0	֧��BLOB����(ʵ��PushParamBLOB())
								�����˲���������ĳ���
								����ValueLength()�ӿ�
								IDataEnumSink::OnFieldValue()�ӳ��Ȳ���
								������һЩbuff��bug
2008-04-04��06	��袳�	v7.0.0	����SQLite֧��
								AddQuote()����bBlob����
								�������������ز���ֵ,ԭ����void��
								����DBDependOption()�ӿ�,��ʵ��
								����IDBDepend -> IQueryResult
2008-04-23		��袳�	v8.0.0	����GetQueryTime()
2008-05-26		��袳�	v9.0.0	MAX_PARAM_COUNT 20->64,��CQuery��buff����,����Ӱ汾
								ExecSQL()ִ�ж�������Ͽ����ӵ�����
2008-12-30		��袳�	v9.0.1	�ӿ�CLASSUIDԭ��д��"IUnknowEx2�ӿ�"��
2009-03-24		��袳�	v10.0.0	OpenSQL()��CallProcedure()�� bMaybeNoResult ����
2010-10-25		��袳�	v10.0.1	����SQL Server��֧��
	��11-xx						MAX_PARAM_COUNT 100->64��Ӧ���㹻�ˣ������ڲ���������
2010-11-16		��袳�	v10.0.1	CQuery���ӵ���������PushParamInt64()/PushParamUInt64()
2011-03-03		��袳�	v11.0.0	����Ping()
								����mysqlִ�г������ݿ�ı������
								mysql����������mysql_options()
2012-11-02		��袳�	v12.0.0	����ȡһ��������ֵ�� FieldValues()
								����EnumData2()
2012-12-18		��袳�	v13.0.0	����IStatement����Ӧ����




*/

#ifndef _IDBCONNECT_H_
#define _IDBCONNECT_H_

#include "../IUnknownEx.h"
#include "../commInterfaceDef.h"

class CQuery;

#define IDBCONNECT_VER_MAIN		13
#define IDBCONNECT_VER_SUB		0
#define IDBCONNECT_VER_BUILD	0

static const CLASSUID	IF_UID_DBCONNECT ("IDBConnect�ӿ�");
static const ulong		IF_VER_DBCONNECT = INTERFACE_VER(IDBCONNECT_VER_MAIN,IDBCONNECT_VER_SUB);
//********************************************************************************

class IDBConnect : public IUnknownEx2
{
public:
	//���ݿ�����
	virtual bool Connect(const char*pHost,ushort usPort,const char *pDBName,
					const char*pUserName,const char *pPasswd,void *pReserved = 0) = 0;
	virtual void DisConnect() = 0;

	//����֧��
	virtual bool BeginTransaction() = 0;
	virtual bool Commit  ()         = 0;
	virtual bool Rollback()         = 0;

	//�������ݿ�ʱ��ĺ���������"now()"
	virtual const char * GetTimeFucntion(bool bBracket = true) const = 0;   

	//��������ס�ַ���pSrc,���滻�����ַ�,�üӵ�SQL�����
	//����: nSrcLen = pSrc����(�ַ�������strlen(pSrc))
	//      sizeof(szDes) >= nSrcLen*2 + 4 (mysql����+3,SQLite��+4)
	//�ɹ�����szDes,���򷵻�NULL
	virtual char*	AddQuote(const void*pSrc,char szDes[],int nSrcLen,ulong bBlob = false) const = 0;

	//ȡ��ѯ����(OpenSQL,EexcSQL,CallProcedure,CallFunction)�Ĵ���,ע��洢����������м�����ѯ
	virtual ulong	GetQueryCount()						const	= 0;

	//ȡ���ݿ���û����˶���ʱ��
	virtual ulong	GetQueryTime(ulong ulReserved = 0)	const	= 0;

	//��ȡ�����������ݿ���ص�ѡ��
	virtual ulong	DBDependOption(ulong ulCode,ulong ulParam0=0,ulong ulParam1=0,ulong ulParam2=0)	{	return 0;	}

	//ping��ȷ�����ݿ�����
	virtual bool	Ping()	{	return false;	}

	struct IStatement : protected IUnknownEx2
	{
		friend class CQuery;

		//���ò���ֵ����ʽ
		virtual void SetParamChar	(int n,char		v)	= 0;
		virtual void SetParamUChar	(int n,uchar	v)	= 0;
		virtual void SetParamShort	(int n,short	v)	= 0;
		virtual void SetParamUShort	(int n,ushort	v)	= 0;
		virtual void SetParamInt	(int n,int		v)	= 0;
		virtual void SetParamUInt	(int n,uint		v)	= 0;
		virtual void SetParamInt64	(int n,longlong	v)	= 0;
		virtual void SetParamUInt64	(int n,ulonglong v)	= 0;
		virtual void SetParamFloat	(int n,float    v)	= 0;		//�ðɣ���IDBConnect::PushParamFloat()��̫һ��
		virtual void SetParamDouble	(int n,double   v)	= 0;
		virtual void SetParamStr	(int n,const char*pStr, int nMaxLen)			= 0;	//���ʵ������var str,����һ���̶��ģ��Ȳ�����
		virtual void SetParamBlob	(int n,const void*pBlob,int nLen,int nMaxLen)	= 0;

		//�������ֵ����ʽ
		virtual void SetResultFormatInteger	(int n,int size,bool bUnsigned)	= 0;
		virtual void SetResultFormatFloat	(int n)							= 0;
		virtual void SetResultFormatDouble	(int n)							= 0;
		virtual void SetResultFormatStr		(int n,int size)				= 0;
		virtual void SetResultFormatBlob	(int n,int size)				= 0;

		//ȡ���
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
	//���º�������CQueryʹ��

	//CQuery�������ݿ���صĽṹ
	virtual struct IQueryResult * NewQueryResult(void *pPlacement,int nLen) = 0;

	//ִ��SQL����
	virtual bool ExecSQL(IQueryResult *pQueryResult,const char* pSQL) = 0;
	virtual bool OpenSQL(IQueryResult *pQueryResult,const char* pSQL,bool bMaybeNoResult = false) = 0;
	
	virtual IStatement * CreateStatement(const char*pSQL,int nSQLLen)	= 0;
	virtual bool ExecStatement(IQueryResult *pQueryResult,IStatement* pStmt) = 0;
	virtual bool OpenStatement(IQueryResult *pQueryResult,IStatement* pStmt,bool bMaybeNoResult = false) = 0;

	//ִ�д洢���̺ͺ���
	// 1.�ݲ�֧��out���������ȫ�������ݼ���ʽ����
	// 2.Ŀǰֻ֧��ѹ��MAX_PARAM_COUNT�����������ù���/�����󣬻��Զ�����CleanParam()���ѹ��Ĳ���
	// 3.���е�ָ�������ָ�����ݶ����ᱻ���ƣ����Լ���֤���ù���/����ǰָ����ָ��������Ч
	// 4.��������������ջ
	virtual bool PushParamInt	(IQueryResult *pQueryResult,int i)					= 0;
	virtual bool PushParamUInt	(IQueryResult *pQueryResult,unsigned int i)			= 0;
	virtual bool PushParamInt64 (IQueryResult *pQueryResult,UINT high,UINT low)		= 0;
	virtual bool PushParamUInt64(IQueryResult *pQueryResult,UINT high,UINT low)		= 0;
	virtual bool PushParamIntStr(IQueryResult *pQueryResult,const char*pStr)		= 0;
	virtual bool PushParamStr	(IQueryResult *pQueryResult,const char*pStr)		= 0;
	virtual bool PushParamFloat	(IQueryResult *pQueryResult,double d)				= 0;
	virtual bool PushParamBLOB	(IQueryResult *pQueryResult,const void*p,ushort len)= 0;	//ע�ⲻҪ��̫���ģ�������һ��40k���Ĳ���Ӧ�ÿ������������ٴ�Ͳ��ܱ�֤��
	virtual void CleanParam		(IQueryResult *pQueryResult) 						= 0;
	virtual bool CallProcedure	(IQueryResult *pQueryResult,const char*pName,bool bWithRecordSet,bool bMaybeNoResult = false)	= 0;
	virtual bool CallFunction	(IQueryResult *pQueryResult,const char*pName,const char*pAs = NULL)	= 0;

	//SQL����صĽ��
	virtual int			 ColCount  (IQueryResult *pQueryResult)  const = 0;
	virtual int			 LineCount (IQueryResult *pQueryResult)  const = 0;
	virtual const char * FieldName (IQueryResult *pQueryResult,int nCol)									const = 0;
	virtual const char * FieldValue(IQueryResult *pQueryResult,int nLine,int nCol,ulong *pulLen=0)			const = 0;
	virtual int			FieldValues(IQueryResult *pQueryResult,int nLine,const char*values[],ulong pulLen[]=0)	const = 0;
	virtual ulong		ValueLength(IQueryResult *pQueryResult,int nLine,int nCol)							const = 0;

	//�������ݡ���Ȼ�����FieldValue()Ҳ��������������
	// 1. mysql�Ľ�����Ǹ������ڲ�����ʱ��Щ��
	// 2. ��Ҫ��ε���FieldValue()ʱ���ܶ���ν�ļ��
	virtual void EnumData (IQueryResult* pQueryResult,class IDataEnumSink* pSink,bool bNeedFieldName = true,bool bNeedFieldValue = true)	const = 0;
	virtual void EnumData2(IQueryResult* pQueryResult,class IDataEnumSink2*pSink,bool bNeedFieldName = true,bool bNeedFieldValue = true)	const = 0;
};

//���������Ľ�����
class IDataEnumSink
{
public:
	//�õ��ֶ���������false����������Ի����OnFieldValue()
	virtual bool OnFieldName (int nCol,const char*pName)	= 0;

	//��ʼ�µ�һ�У�����false�����
	virtual bool OnNewLine   (int nLine) = 0;	
	//�õ��ֶ�ֵ������false�����
	virtual bool OnFieldValue(int nLine,int nCol,const char*pValue,ulong ulValueLen)	= 0;

	//����������Ƿ񷵻�false������������
	virtual void OnEnumEnd()	= 0;
};

class IDataEnumSink2
{
public:
	//�ֶ���,ע�����ָ����������Ч
	virtual void OnFieldName (const char*names[])	= 0;	

	//��ʼ�µ�һ�У�����false�����
	virtual bool OnNewLine   (int nLine,const char*values[],ulong lengths[]) = 0;	

	//����������Ƿ񷵻�false������������
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

	//��ղ�ѯ�����
	void Close()        {   if (m_pQueryResult) m_pQueryResult->ClearData();  }

	//ִ��SQL��䣬�޷��ؽ��
	bool ExecSQL(const char* SQL)								{   return m_pDBC->ExecSQL(m_pQueryResult,SQL);		}
	//ִ��SQL��䣬��ȡ���صĽ����
	bool OpenSQL(const char* SQL,bool bMaybeNoResult = false)	{   return m_pDBC->OpenSQL(m_pQueryResult,SQL,bMaybeNoResult);	}

	virtual bool CreateStatement(const char*pSQL,int nSQLLen)
	{
		if ( m_pStmt )
		{
			assert(0);	//��֪�����������Ҫ��Ҫ�ͷ�
			m_pStmt->Release();
		}
		m_pStmt = m_pDBC->CreateStatement(pSQL,nSQLLen);
		return !!m_pStmt;
	}
	virtual IDBConnect::IStatement*	GetStatement()			{	return m_pStmt;	}
	virtual bool ExecStatement()							{	return m_pDBC->ExecStatement(m_pQueryResult,m_pStmt);					}
	virtual bool OpenStatement(bool bMaybeNoResult = false)	{	return m_pDBC->OpenStatement(m_pQueryResult,m_pStmt,bMaybeNoResult);	}

	//ִ�д洢���̺ͺ���
	//�뿴IDBConnect����Ӧ��˵��
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

	//ȡ������Ĵ�С
	int ColCount () const   {   return m_pDBC->ColCount(m_pQueryResult);     }
	int LineCount() const   {   return m_pDBC->LineCount(m_pQueryResult);    }

	//ȡ�ֶ�����ֵ
	const char * FieldName  (		   int nCol)					const {	return m_pDBC->FieldName (m_pQueryResult,nCol);					}
	const char * FieldValue (int nLine,int nCol,ulong *pulLen=0)	const {	return m_pDBC->FieldValue(m_pQueryResult,  nLine,nCol,pulLen);	}
	const char * FieldValue (		   int nCol,ulong *pulLen=0)	const {	return m_pDBC->FieldValue(m_pQueryResult,m_nLine,nCol,pulLen);	}
	int			 FieldValues(int nLine,const char*v[],ulong l[])	const {	return m_pDBC->FieldValues(m_pQueryResult,nLine,  v,l);			}
	int			 FieldValues(		   const char*v[],ulong l[])	const {	return m_pDBC->FieldValues(m_pQueryResult,m_nLine,v,l);			}
	ulong		 ValueLength(int nLine,int nCol)					const {	return m_pDBC->ValueLength(m_pQueryResult,  nLine,nCol);		}
	ulong		 ValueLength(		   int nCol)					const {	return m_pDBC->ValueLength(m_pQueryResult,m_nLine,nCol);		}

	//ȡ�ֶ�ֵ����ת����char*����
	const char* StrValue  (const char *pFieldName)	{   return FieldValue(FieldNum(pFieldName));        }
	const char* StrValue  (int nCol)				{   return FieldValue(nCol);						}
	const char* StrValue  (int nLine,int nCol)		{   return FieldValue(nLine,nCol);					}
	//ȡ�ֶ�ֵ����ת����int����
	int			IntValue  (const char *pFieldName)  {   return atoi(FieldValue(FieldNum(pFieldName)));  }
	int			IntValue  (int nCol)				{   return atoi(FieldValue(nCol));					}
	int			IntValue  (int nLine,int nCol)		{   return atoi(FieldValue(nLine,nCol));			}
	//ȡ�ֶ�ֵ����ת����double����
	double		FloatValue(const char *pFieldName)  {   return atof(FieldValue(FieldNum(pFieldName)));  }
	double		FloatValue(int nCol)				{   return atof(FieldValue(nCol));					}
	double		FloatValue(int nLine,int nCol)		{   return atof(FieldValue(nLine,nCol));			}

	//ȡ�ֶ�����Ӧ���к�
	int FieldNum(const char *pFieldName)        const
	{
		if ( NULL == pFieldName )
		{
			assert(pFieldName);
			return -1;
		}
		for ( int i=ColCount()-1; i>=0; i-- )
			if ( ! strcmp(FieldName(i),pFieldName) )    //����򷵻�
				return i;
		return -1;
	}

	//֧�ֱ���
	void First()    {   m_nLine = 0;  }
	void Next()     {   m_nLine++;    }
	bool IsEnd()	{   return m_nLine >= LineCount();    }

	void EnumData (class IDataEnumSink* pSink,bool bNeedFieldName = true,bool bNeedFieldValue = true)	const	{	m_pDBC->EnumData (m_pQueryResult,pSink,bNeedFieldName,bNeedFieldValue);	}
	void EnumData2(class IDataEnumSink2*pSink,bool bNeedFieldName = true,bool bNeedFieldValue = true)	const	{	m_pDBC->EnumData2(m_pQueryResult,pSink,bNeedFieldName,bNeedFieldValue);	}

	//����
	const char* GetTimeFucntion(bool bBracket = true) const { return m_pDBC->GetTimeFucntion(bBracket); }
	char* AddQuote(const void*pSrc,char szDes[],int nSrcLen,ulong bBlob = false) const { return m_pDBC->AddQuote(pSrc,szDes,nSrcLen,bBlob);	}

private:
	//��ֹ���ƺ͸�ֵ
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
			return "�����ֵĸ�";
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
