#ifndef _IASYN_OBJ_H__
#define _IASYN_OBJ_H__
/*
2012-09-11		lanjc	  	��IAsynProc.h�и�����
2014-12-19					GetInterfaceVer()��ΪGetAsynObjVersion()
							�����˽ӿ�˳��
*/

enum  enActRet
{
	ar_del			= 0,		//ɾ��(����Release())
	ar_return_only	= 1,
};
//************************************************************************

class IAsynObj 
{
public:
	enum	{	ASYN_OBJ_VER	= 1		};
	virtual uint32 		GetAsynObjVersion()	const	{	return ASYN_OBJ_VER;				}
	virtual const char* OnGetDesc()					{	return "Unknown IAsynObj Object";	}		//�������쳣ʱ,�����������Ǹ�ʲô��,һ��debugʱ�õĶ�
	
	//�ͷ�
	virtual void 		Release() 	= 0;	
	//�ɻ��ˣ�
	virtual enActRet 	OnAct()		= 0;	
};

#endif
