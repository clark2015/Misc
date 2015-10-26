#ifndef _IOBJ_FILE_H_
#define _IOBJ_FILE_H_

/*
�����ļ��Ķ�д
�����ļ��п����������������ʽ����������,
�����а�����������,��������Ƕ��

2005-04-06				��袳�	�����ļ�
2005-04-15				��袳�	���ICntFileElement��
2005-04-16/17			��袳�	��ɻ�������,����ͨ��
2005-04-18				��袳�	�޸������׼�
2006-07-19				��袳�	�ĳ�dll����ʽ
								������string�Ĳ���������ֵ�ĳ�char*,������Щ����ֵ�����᷵��NULL
2007-01-22/24			��袳�  ���ص�float��Ϊdouble
								ʹ�ַ�������c���Թ淶,���������'\0'
2007-05-16		4.0.0	��袳�  GetXXXAttrib()����Ĭ��ֵ
2009-10-26		4.0.1	��袳�  ֧���������Լ������������������Ե���listʹ�á����������ΰ汾�Ա����ǰ�ļ���
2010-06-18		4.0.2	��袳�  ����ToLongLong()��GetLongLongAttrib()�ȡ����������ΰ汾�Ա����ǰ�ļ���




*/

#include <stdio.h>
#include <string.h>
#include "commInterfaceDef.h"
#include "DLTypeDefs.h"
//************************************************************************************
//Ԫ�ص�����
enum CNT_ELEMENT_TYPE
{	
	CLT_ATTRIB	= 0,
	CLT_CNT		= 1
};

//�������ʱ�����λ��
struct CNT_ENUM_POS
{
	char noname[32];
};

//����ַ�������
#define CNT_MAX_STRING_LEN	16384

//�������������Ե����֣����������,���� AnonymousElement0,AnonymousElement1
#define ANONYMOUS_ELEMENT "AnonymousElement"
inline 
bool IsAnonymousElement(const char*pName)
{
	if ( NULL == pName )
		return false;
	static int s_nLen = strlen(ANONYMOUS_ELEMENT);

	if ( strncmp(ANONYMOUS_ELEMENT,pName,s_nLen) )
		return false;
	if ( pName[s_nLen] == '0' &&  pName[s_nLen+1] == 0 )	//��һ��Ϊ'0'�ҽ���,�����һ������=0
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
//Ԫ�صĻ���
class ICntElement
{
public:	
	//ȡ����(ͬһ�����µ�������Ψһ��)
	virtual const char* 	 GetName() 	const = 0;
	virtual CNT_ELEMENT_TYPE GetType()	const = 0;
};
//************************************************************************************
//����
class IAttrib : public ICntElement
{
public:
	virtual int    ToInt()		const =	0;
	virtual double ToFloat()	const =	0;
	virtual const char* ToStr()	const =	0;	//�ҷ��ģ����᷵��NULL
	virtual int	   GetValueLen()const = 0;

	virtual void SetValue(int    nValue)  = 0;
	virtual void SetValue(double fValue)  = 0;
	virtual bool SetValue(const char* pValue,int nLen = -1) = 0;	//����len�������ִ�������'\0'

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
//����
class IContainer : public ICntElement
{
public:
	//��Ԫ��
	virtual const ICntElement *FindElement(const char *pName) const	= 0;
	virtual 	  ICntElement *FindElement(const char *pName)		= 0;
	//��������(�ӽ��)
	virtual 	  IContainer * GetChildCnt(const char *pName)	 	= 0;
	virtual const IContainer * GetChildCnt(const char *pName) const = 0;
	//������
	virtual 	  IAttrib * GetAttrib(const char *pName)		 = 0;
	virtual const IAttrib * GetAttrib(const char *pName) const   = 0;
	
	//��������,ע�����������������Ԫ����ɾ,��GetNext()���ܻ����(�պ�����)
	virtual void 	 		 	ToFirst(CNT_ENUM_POS &pos)	const = 0;
	virtual const ICntElement* 	GetNext(CNT_ENUM_POS &pos)	const = 0;

	//��������(�ӽ��)
	virtual IContainer * AddContainer(const char *pName)	= 0;
	//ɾ������(�ӽ��)
	virtual bool  		 DelContainer(const char *pName)	= 0;
	//ȡ�����,�����ǰΪroot,�򷵻�NULL
	virtual 		IContainer * GetParent()				= 0;
	virtual const  	IContainer * GetParent()	const		= 0;
	
	//��������(ͬһ�����µ�����������Ψһ��)
	virtual IAttrib * AddAttrib(const char *pName,const char *pValue,int nLen = -1) = 0;
	virtual IAttrib * AddAttrib(const char *pName,int    nValue)		= 0;
	virtual IAttrib * AddAttrib(const char *pName,double fValue)		= 0;
	//ɾ������
	virtual bool  	  DelAttrib(const char *pName)						= 0;
	
	//ɾ�����µ��������Լ�����
	virtual void Clear()		= 0;
	
	//Ƕ�׸���pCnt���������Լ�����,���ı�this������
	//���Ŀ�����и�Դ��һ�������ֵ�Ԫ��,��ʧ��
	//����Ҫ�Ȳ���˫���Ƿ����ظ�������,���Կ��ܱȽϷ�ʱ
	virtual bool CopyFrom(const IContainer *pCnt)		= 0;
	
	//ȡԪ����Ŀ
	virtual int GetElementCount()	const = 0;
	
	//ֱ��ȡ����ֵ,��GetAttrib()�����ToXXX()һ��
	//�����ڻ�pName==NULL,�򷵻�Ĭ��ֵ
	virtual int			GetIntAttrib	 (const char*pName,	int    nDefVal = 0)		const = 0;
	virtual double		GetFloatAttrib	 (const char*pName,	double fDevVal = 0)		const = 0;
	virtual const char* GetStrAttrib	 (const char*pName,	const char*pDef= "")	const = 0;
	virtual int			GetValueLen		 (const char*pName)	const = 0;
	virtual longlong	GetLongLongAttrib(const char*pName,	longlong llDefVal=0)	const = 0;
};
//************************************************************************************
//�����ļ�������
class ICntFileMagr
{
public:
	virtual void Release()  = 0;
	virtual bool Save(const char *pFileName) const 	= 0;
	virtual bool Load(const char *pFileName) 		= 0;	//ע�⺯����������
	virtual bool Save(FILE *pFile)	 const 	= 0;
	virtual bool Load(FILE *pFile)			= 0;			//ע�⺯����������

	//ȡ���ڵ�,���ڵ�����ֱ�ʼ��Ϊ"root",
	//���ļ���,��������container{..}��Χ����,��������ͨIContainer��ͬ
	//�����ò��᷵��NULL
	virtual 	  IContainer * GetRoot()		= 0;
	virtual const IContainer * GetRoot() const	= 0;
	
	//�ϲ�2��IContainerMagr,
	//���ú�,pMagr�����ݽ������
	//���������ͬ�������Ի�����,���ý�ʧ��
	virtual bool Combin(ICntFileMagr *pMagr)	= 0;
	
	//�������
	virtual void Clear() = 0;
};

//************************************************************************
#define  VER_CNT_FILE_MAIN		4		//���汾�����ǲ���������
#define  VER_CNT_FILE_MIN		0		//�ΰ汾�����Ǽ��ݵ�����(�������ӵ�)
#define  VER_CNT_FILE_BUILD		2		//build�汾�����ǹ��ܲ����bug��������
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
