#ifndef _IID_MAGR_H__	//��Ҫ�Ҹ������,��ITimer.h��Ҫ�õ�
#define _IID_MAGR_H__
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "commInterfaceDef.h"
/*
����:�Ѷ�����һ��ȫ��Ψһ��ID��Ӧ����

���ҵ�pIII-500������,��ͣ�ؼ���ɾ���Ļ�,��ԼҪ5.86Сʱ��ID����
���ÿ�����100��,��0xFFFFFFFF/(365*24*60*60*100)=1.36������

2005-05-06	��袳�	v1.0	ʵ��IObjIDMagr�Ļ�������,����ͨ��
2008-02-19	��袳�	v2.0	�Ƿ�Ҫ����,���ڸĳ�Ĭ��Ϊtrue
							IObjIDMagr::GetCount()��const��׺
							IObjWithID::GetMyID()��const��׺,ȥ��virtual
							����IObjIDMagr::SetNeedLock()
							����IObjIDMagr::Release()
							����IObjWithID��CObjWithID2
							���ڿ���new��һ���µ�IObjIDMagr��,����ֻ����ʹ��ϵͳ��
2012-04-13	��袳�	v2.0	���淵��IDҲ!=0
2012-05-24	��袳�	v3.0	INVALID_ID����Ϊ0��ԭ���� ulong(-1)��ʵ����0��-1�����᷵��
*/


#define INVALID_ID			0		//ʵ����Ҳ����== ulong(-1)

#define	IDMAGR_VER_MAIN		2
#define	IDMAGR_VER_SUB		0
#define	IDMAGR_VER_BUILD	0
//**************************************************************************
class IObjWithID;

class IObjIDMagr
{
	friend class IObjWithID;
private:	//Ӧ��Ϊ����or˽��,ʹ��IObjWithID�����������붼������
	virtual ulong AddObject(IObjWithID *pObj)	= 0;
	virtual bool  DelObject(IObjWithID *pObj)	= 0;
public:
	virtual void Release()				= 0;
	virtual void SetNeedLock(bool b)	= 0;
	virtual int  GetCount()	const		= 0;
	
	virtual IObjWithID * GetObject(ulong ulID)		= 0;
	virtual ulong     	 GetID(IObjWithID * pObj)	= 0;	//�ǳ���ʱ,��ò�Ҫ��,��AddObject()ʱ�Լ���ס����ֵ����
};

inline IObjIDMagr* GetObjIDMagr();

class IObjWithID	
{	
	friend class CObjWithID;	
	friend class CObjWithID2;	
private:	
	ulong m_ulMyID___;	

	void GenerateID(IObjIDMagr*	pObjIDMagr)	
	{	
		assert(pObjIDMagr);	
		ulong ulID  = pObjIDMagr->AddObject(this);	
		if ( ulID == INVALID_ID )	
			RelTrace("IObjWithID::GenerateID(): AddObject()����INVALID_ID");	
		m_ulMyID___ = ulID;		//Ҫ������!�������ǿ�е��ù��캯��....	
	}	

	void RemoveID(IObjIDMagr*	pObjIDMagr)	
	{	
		assert(pObjIDMagr);	
		if ( false == pObjIDMagr->DelObject(this) )	
			RelTrace("IObjWithID::DelID(): DelObject()����ʧ��,my id is 0x%X",m_ulMyID___);	
	}	
public:	
	ulong GetMyID()	const	{	return m_ulMyID___;		}	
};	

/*
1.���м̳��Դ������,���Զ���ϵͳ���һ��ȫ��Ψһ��id,��id���൱����һ��ʱ�����ǲ�������ظ���
2.���м̳��Դ�����඼Ӧע���俽�����캯��,��Ҫֱ��copy��Աm_ulMyID___����Ӧ���Լ�����һ��
3.���м̳��Դ�����඼Ӧע���丳ֵ������,��Ҫ���ǵ�m_ulMyID___��Ա
4.����2,3��ر�ע�ⲻҪʹ��memcpy֮��ĺ��������ƶ���
*/
class CObjWithID : public IObjWithID
{
public:
	const CObjWithID & operator = (const CObjWithID & a)	{	return *this;	}
	CObjWithID(const CObjWithID & a)
	{
		GenerateID(GetObjIDMagr());
	}
	CObjWithID()
	{
		GenerateID(GetObjIDMagr());
	}
	~CObjWithID()
	{
		RemoveID(GetObjIDMagr());
	}
};

class CObjWithID2 : public IObjWithID
{
	IObjIDMagr*	m_pObjIDMagr;
public:
	const CObjWithID2 & operator = (const CObjWithID2 & a)	{	return *this;	}
	CObjWithID2(const CObjWithID2 & a)
	{
		m_pObjIDMagr = a.m_pObjIDMagr;
		GenerateID(a.m_pObjIDMagr);
	}
	CObjWithID2(IObjIDMagr*	pObjIDMagr)
	{
		m_pObjIDMagr = pObjIDMagr;
		GenerateID(pObjIDMagr);
	}
	~CObjWithID2()
	{
		RemoveID(m_pObjIDMagr);
	}
};

//**************************************************************************
#define IDMAGR_DLL_NAME	    "./idmagr"DLL_EXT
class CObjIDMagrLoader
{
	typedef IObjIDMagr * (*_GetObjIDMagr)(ulong ulMainVer,ulong ulMinVer,ulong ulBuildVer);
	IObjIDMagr* m_pInterface;
	HMODULE		m_hDll;

	bool Create(const char*pFunctionName)
	{
		if ( m_pInterface )
			return true;

		if ( 0 == pFunctionName || 0 == pFunctionName[0] )
		{
			RelTrace("CObjIDMagrLoader::Create() : funcname null");
			return false;
		}

		m_hDll = LoadLibrary(IDMAGR_DLL_NAME);
		if ( 0 == m_hDll )
		{
			RelTrace("�򲻵�dll %s",IDMAGR_DLL_NAME);
			return false;
		}

		_GetObjIDMagr func = (_GetObjIDMagr)GetProcAddress(m_hDll,pFunctionName);
		if ( func )
		{
			m_pInterface = func(IDMAGR_VER_MAIN,IDMAGR_VER_SUB,IDMAGR_VER_BUILD);
		}
		else
		{
			RelTrace("CObjIDMagrLoader::Create() : �Ҳ�������func : %s",pFunctionName);
		}

		if ( !func || ! m_pInterface )
		{
			FreeLibrary(m_hDll);
			m_hDll = 0;
			return false;
		}
		return true;
	}

public:
	CObjIDMagrLoader()	{	m_pInterface = 0;	m_hDll = 0;	}
	~CObjIDMagrLoader()	{	Close();						}
	
	bool GetSysObjIDMagr()	{	return Create("GetObjIDMagr");	}
	bool NewObjIDMagr()		{	return Create("NewObjIDMagr");	}

	void Close()
	{
		if ( m_pInterface )
		{
			m_pInterface->Release();
			m_pInterface = 0;
		}
		if ( m_hDll )
		{
			FreeLibrary(m_hDll);
			m_hDll = 0;
		}
	}
	
	const	IObjIDMagr* GetInterface()	const	{	return m_pInterface;		}
			IObjIDMagr* GetInterface()			{	return m_pInterface;		}
	const	IObjIDMagr* operator ->()	const	{	return m_pInterface;		}
			IObjIDMagr* operator ->()			{	return m_pInterface;		}
	bool	IsValid()					const	{	return 0 != m_pInterface;	}
};
//**************************************************************************

inline 
IObjIDMagr* GetObjIDMagr()
{
	static CObjIDMagrLoader Loader;
	static bool  bFirst = true;
	if ( bFirst )
	{
		bFirst = false;
		Loader.GetSysObjIDMagr();
	}

	return Loader.GetInterface();
}
//**************************************************************************
#endif
