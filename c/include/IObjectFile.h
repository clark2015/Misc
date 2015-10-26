#ifndef _IOBJ_FILE_H_
#define _IOBJ_FILE_H_

/*
容器文件的读写
容器文件中可以以类似于类的形式定义多个容器,
容器中包含各种属性,容器可以嵌套

2005-04-06				蓝琚成	建立文件
2005-04-15				蓝琚成	抽出ICntFileElement类
2005-04-16/17			蓝琚成	完成基本功能,测试通过
2005-04-18				蓝琚成	修改让其易记
2006-07-19				蓝琚成	改成dll的形式
								把所有string的参数及返回值改成char*,所有这些返回值都不会返回NULL
2007-01-22/24			蓝琚成  返回的float改为double
								使字符串符合c语言规范,并允许放入'\0'
2007-05-16		4.0.0	蓝琚成  GetXXXAttrib()增加默认值
2009-10-26		4.0.1	蓝琚成  支持匿名属性及容器，这样容器可以当作list使用。不升级主次版本以便跟以前的兼容
2010-06-18		4.0.2	蓝琚成  增加ToLongLong()及GetLongLongAttrib()等。不升级主次版本以便跟以前的兼容




*/

#include <stdio.h>
#include <string.h>
#include "commInterfaceDef.h"
#include "DLTypeDefs.h"
//************************************************************************************
//元素的类型
enum CNT_ELEMENT_TYPE
{	
	CLT_ATTRIB	= 0,
	CLT_CNT		= 1
};

//保存遍历时到达的位置
struct CNT_ENUM_POS
{
	char noname[32];
};

//最大字符串长度
#define CNT_MAX_STRING_LEN	16384

//匿名容器或属性的名字，后面加数字,例如 AnonymousElement0,AnonymousElement1
#define ANONYMOUS_ELEMENT "AnonymousElement"
inline 
bool IsAnonymousElement(const char*pName)
{
	if ( NULL == pName )
		return false;
	static int s_nLen = strlen(ANONYMOUS_ELEMENT);

	if ( strncmp(ANONYMOUS_ELEMENT,pName,s_nLen) )
		return false;
	if ( pName[s_nLen] == '0' &&  pName[s_nLen+1] == 0 )	//第一个为'0'且结束,否则第一个不能=0
		return true;										//
	if ( pName[s_nLen] > '9' ||  pName[s_nLen] <= '0' )		//
		return false;										//
	for ( int i=s_nLen+1; pName[i]; i++)
	{
		if ( pName[s_nLen] > '9' ||  pName[s_nLen] < '0')
			return false;
	}
	return true;
}
//************************************************************************************
//元素的基类
class ICntElement
{
public:	
	//取名字(同一容器下的名字是唯一的)
	virtual const char* 	 GetName() 	const = 0;
	virtual CNT_ELEMENT_TYPE GetType()	const = 0;
};
//************************************************************************************
//属性
class IAttrib : public ICntElement
{
public:
	virtual int    ToInt()		const =	0;
	virtual double ToFloat()	const =	0;
	virtual const char* ToStr()	const =	0;	//我发誓，不会返回NULL
	virtual int	   GetValueLen()const = 0;

	virtual void SetValue(int    nValue)  = 0;
	virtual void SetValue(double fValue)  = 0;
	virtual bool SetValue(const char* pValue,int nLen = -1) = 0;	//因有len参数，字串可以有'\0'

	virtual longlong ToLongLong()	const		= 0;
	virtual void	 SetValue(longlong llValue) = 0;

	bool IsValueEqu(const IAttrib*pAtt)	
	{	
		int nLen = GetValueLen();
		return	pAtt && pAtt->GetValueLen() == nLen && 
				!memcmp(pAtt->ToStr(),ToStr(),nLen);
	}
};
//************************************************************************************
//容器
class IContainer : public ICntElement
{
public:
	//找元素
	virtual const ICntElement *FindElement(const char *pName) const	= 0;
	virtual 	  ICntElement *FindElement(const char *pName)		= 0;
	//找子容器(子结点)
	virtual 	  IContainer * GetChildCnt(const char *pName)	 	= 0;
	virtual const IContainer * GetChildCnt(const char *pName) const = 0;
	//找属性
	virtual 	  IAttrib * GetAttrib(const char *pName)		 = 0;
	virtual const IAttrib * GetAttrib(const char *pName) const   = 0;
	
	//遍历函数,注意如果遍历过程中有元素增删,则GetNext()可能会出错(日后修正)
	virtual void 	 		 	ToFirst(CNT_ENUM_POS &pos)	const = 0;
	virtual const ICntElement* 	GetNext(CNT_ENUM_POS &pos)	const = 0;

	//增加容器(子结点)
	virtual IContainer * AddContainer(const char *pName)	= 0;
	//删除容器(子结点)
	virtual bool  		 DelContainer(const char *pName)	= 0;
	//取父结点,如果当前为root,则返回NULL
	virtual 		IContainer * GetParent()				= 0;
	virtual const  	IContainer * GetParent()	const		= 0;
	
	//增加属性(同一容器下的属性名字是唯一的)
	virtual IAttrib * AddAttrib(const char *pName,const char *pValue,int nLen = -1) = 0;
	virtual IAttrib * AddAttrib(const char *pName,int    nValue)		= 0;
	virtual IAttrib * AddAttrib(const char *pName,double fValue)		= 0;
	//删除属性
	virtual bool  	  DelAttrib(const char *pName)						= 0;
	
	//删除其下的所有属性及容器
	virtual void Clear()		= 0;
	
	//嵌套复制pCnt中所有属性及容器,不改变this的名字
	//如果目标中有跟源下一样的名字的元素,则失败
	//函数要先查找双方是否有重复的名字,所以可能比较费时
	virtual bool CopyFrom(const IContainer *pCnt)		= 0;
	
	//取元素数目
	virtual int GetElementCount()	const = 0;
	
	//直接取属性值,与GetAttrib()后调用ToXXX()一样
	//不存在或pName==NULL,则返回默认值
	virtual int			GetIntAttrib	 (const char*pName,	int    nDefVal = 0)		const = 0;
	virtual double		GetFloatAttrib	 (const char*pName,	double fDevVal = 0)		const = 0;
	virtual const char* GetStrAttrib	 (const char*pName,	const char*pDef= "")	const = 0;
	virtual int			GetValueLen		 (const char*pName)	const = 0;
	virtual longlong	GetLongLongAttrib(const char*pName,	longlong llDefVal=0)	const = 0;
};
//************************************************************************************
//容器文件管理器
class ICntFileMagr
{
public:
	virtual void Release()  = 0;
	virtual bool Save(const char *pFileName) const 	= 0;
	virtual bool Load(const char *pFileName) 		= 0;	//注意函数不可重入
	virtual bool Save(FILE *pFile)	 const 	= 0;
	virtual bool Load(FILE *pFile)			= 0;			//注意函数不可重入

	//取根节点,根节点的名字被始化为"root",
	//在文件中,它不需用container{..}包围起来,其它与普通IContainer相同
	//本调用不会返回NULL
	virtual 	  IContainer * GetRoot()		= 0;
	virtual const IContainer * GetRoot() const	= 0;
	
	//合并2个IContainerMagr,
	//调用后,pMagr的内容将被清除
	//如果根下有同名的属性或容器,调用将失败
	virtual bool Combin(ICntFileMagr *pMagr)	= 0;
	
	//清空自身
	virtual void Clear() = 0;
};

//************************************************************************
#define  VER_CNT_FILE_MAIN		4		//主版本增加是不兼容升级
#define  VER_CNT_FILE_MIN		0		//次版本增加是兼容的升级(功能增加等)
#define  VER_CNT_FILE_BUILD		2		//build版本增加是功能不变的bug修正升级
//************************************************************************
#ifdef _DEBUG
	#define _CNT_FILE_DLL "./ICntFileD"DLL_EXT
#else
	#define _CNT_FILE_DLL "./ICntFile"DLL_EXT
#endif

class CICntFileMagrLoader : public InterfaceLoader
									<ICntFileMagr,VER_CNT_FILE_MAIN,VER_CNT_FILE_MIN,VER_CNT_FILE_BUILD>
	{
	protected:
		virtual const char *GetDllFileName(){	return _CNT_FILE_DLL;		}
		virtual const char *GetProcName()	{	return "CreateCntFileMagr";	}
	public:
		ICntFileMagr * GetMagr()		{	return GetInterface();	}
	};
//**********************************************************************
typedef ICntFileMagr			IObjectFileMagr;
typedef CICntFileMagrLoader		CObjectFileLoader;

#endif
