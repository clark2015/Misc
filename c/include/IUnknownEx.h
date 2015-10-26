#ifndef _IUNKNOWN_EX_H__
#define _IUNKNOWN_EX_H__

#include <assert.h>
#include <memory.h>
#include "commtools.h"
//*****************************************************************
//类唯一ID 定义
struct CLASSUID		//class unique id
{
	char buff[16];	//与guid一样长度,应该够了

	CLASSUID()	{	memset(this,0,sizeof(*this));	}
	CLASSUID(const char *pClassDesc)
	{
		assert(pClassDesc && pClassDesc[0]);
		assert(sizeof(buff) > strlen(pClassDesc));
		memset(this,0,sizeof(*this));
		lstrcpyn(buff,pClassDesc,sizeof(buff));
	}

	CLASSUID & operator = (const CLASSUID & obj0)
	{	
		memcpy(buff,obj0.buff,sizeof(buff));	
		return *this;
	}
};

inline bool operator == (const CLASSUID & obj0,const CLASSUID & obj1)	
{	
	return !memcmp(obj0.buff,obj1.buff,sizeof(obj0.buff));	
}

inline bool operator != (const CLASSUID & obj0,const CLASSUID & obj1)
{
	return !!memcmp(obj0.buff,obj1.buff,sizeof(obj0.buff));	
}

#define INTERFACE_VER(usMain,usSub)		\
(										\
 (uint32)								\
 (										\
    (((uint32)(usMain))<<16) |			\
    ((usSub) & 0xFFFF)					\
 )										\
)

inline uint16 GetMainVer(uint32 ulVer)	{	return ushort(ulVer >> 16);		}
inline uint16 GetSubVer (uint32 ulVer)	{	return ushort(ulVer & 0xFFFF);	}

inline bool IsVersionFit(uint32 ulCurrent,uint32 ulRequest)
{
	return	( GetMainVer(ulCurrent) == GetMainVer(ulRequest) ) &&
			( GetSubVer(ulCurrent)	>= GetSubVer(ulRequest)  );
}

//*****************************************************************
static const CLASSUID	IF_UID_IUNKNOWEX  ("IUnknowEx接口");
static const uint32		IF_VER_IUNKNOWEX	= INTERFACE_VER(1,0);

static const CLASSUID	IF_UID_IUNKNOWEX2 ("IUnknowEx2接口");
static const uint32		IF_VER_IUNKNOWEX2 = INTERFACE_VER(1,0);

class IUnknownEx
{
public:
	//查询接口
	virtual void* QueryInterface(const CLASSUID & cuid,uint32 ulVer)
	{
		if ( cuid == IF_UID_IUNKNOWEX && IsVersionFit(IF_VER_IUNKNOWEX,ulVer) )
			return this;
		return NULL;
	}
};

class IUnknownEx2 : public IUnknownEx
{
public:
	virtual void* QueryInterface(const CLASSUID & cuid,uint32 ulVer)
	{
		if ( cuid == IF_UID_IUNKNOWEX2 && IsVersionFit(IF_VER_IUNKNOWEX2,ulVer) )
			return this;
		return IUnknownEx::QueryInterface(cuid,ulVer);
	}

	//释放
	virtual void  Release()		= 0;
};

//*****************************************************************
//以前拼错了 :(
//typedef IUnknownEx	IUnknowEx;
//typedef IUnknownEx2	IUnknowEx2;

#endif
