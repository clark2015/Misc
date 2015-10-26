#ifndef _IASYN_OBJ_H__
#define _IASYN_OBJ_H__
/*
2012-09-11		lanjc	  	从IAsynProc.h中复制来
2014-12-19					GetInterfaceVer()改为GetAsynObjVersion()
							调整了接口顺序
*/

enum  enActRet
{
	ar_del			= 0,		//删除(调用Release())
	ar_return_only	= 1,
};
//************************************************************************

class IAsynObj 
{
public:
	enum	{	ASYN_OBJ_VER	= 1		};
	virtual uint32 		GetAsynObjVersion()	const	{	return ASYN_OBJ_VER;				}
	virtual const char* OnGetDesc()					{	return "Unknown IAsynObj Object";	}		//当发生异常时,用来看对象是干什么的,一般debug时用的多
	
	//释放
	virtual void 		Release() 	= 0;	
	//干活了！
	virtual enActRet 	OnAct()		= 0;	
};

#endif
