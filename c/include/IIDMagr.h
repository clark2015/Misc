#ifndef _IID_MAGR_H__	//不要乱改这个串,在ITimer.h里要用到
#define _IID_MAGR_H__
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "commInterfaceDef.h"
/*
功能:把对象与一个全局唯一的ID对应起来

在我的pIII-500机器上,不停地加入删除的话,大约要5.86小时后ID回绕
如果每秒加入100个,则0xFFFFFFFF/(365*24*60*60*100)=1.36年后回绕

2005-05-06	蓝琚成	v1.0	实现IObjIDMagr的基本功能,测试通过
2008-02-19	蓝琚成	v2.0	是否要加锁,现在改成默认为true
							IObjIDMagr::GetCount()加const后缀
							IObjWithID::GetMyID()加const后缀,去掉virtual
							增加IObjIDMagr::SetNeedLock()
							增加IObjIDMagr::Release()
							增加IObjWithID及CObjWithID2
							现在可以new出一个新的IObjIDMagr了,而不只限于使用系统的
2012-04-13	蓝琚成	v2.0	保存返回ID也!=0
2012-05-24	蓝琚成	v3.0	INVALID_ID定义为0，原来是 ulong(-1)，实际上0和-1都不会返回
*/


#define INVALID_ID			0		//实际上也不会== ulong(-1)

#define	IDMAGR_VER_MAIN		2
#define	IDMAGR_VER_SUB		0
#define	IDMAGR_VER_BUILD	0
//**************************************************************************
class IObjWithID;

class IObjIDMagr
{
	friend class IObjWithID;
private:	//应设为保护or私有,使除IObjWithID类外其它代码都不能用
	virtual ulong AddObject(IObjWithID *pObj)	= 0;
	virtual bool  DelObject(IObjWithID *pObj)	= 0;
public:
	virtual void Release()				= 0;
	virtual void SetNeedLock(bool b)	= 0;
	virtual int  GetCount()	const		= 0;
	
	virtual IObjWithID * GetObject(ulong ulID)		= 0;
	virtual ulong     	 GetID(IObjWithID * pObj)	= 0;	//非常费时,最好不要用,在AddObject()时自己记住返回值好了
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
			RelTrace("IObjWithID::GenerateID(): AddObject()返回INVALID_ID");	
		m_ulMyID___ = ulID;		//要错误检查!否则代码强行调用构造函数....	
	}	

	void RemoveID(IObjIDMagr*	pObjIDMagr)	
	{	
		assert(pObjIDMagr);	
		if ( false == pObjIDMagr->DelObject(this) )	
			RelTrace("IObjWithID::DelID(): DelObject()返回失败,my id is 0x%X",m_ulMyID___);	
	}	
public:	
	ulong GetMyID()	const	{	return m_ulMyID___;		}	
};	

/*
1.所有继承自此类的类,都自动从系统获得一个全局唯一的id,此id在相当长的一段时间内是不会出现重复的
2.所有继承自此类的类都应注意其拷贝构造函数,不要直接copy成员m_ulMyID___，而应该自己申请一个
3.所有继承自此类的类都应注意其赋值操作符,不要覆盖掉m_ulMyID___成员
4.以上2,3项，特别注意不要使用memcpy之类的函数来复制对象
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
			RelTrace("打不到dll %s",IDMAGR_DLL_NAME);
			return false;
		}

		_GetObjIDMagr func = (_GetObjIDMagr)GetProcAddress(m_hDll,pFunctionName);
		if ( func )
		{
			m_pInterface = func(IDMAGR_VER_MAIN,IDMAGR_VER_SUB,IDMAGR_VER_BUILD);
		}
		else
		{
			RelTrace("CObjIDMagrLoader::Create() : 找不到引出func : %s",pFunctionName);
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
