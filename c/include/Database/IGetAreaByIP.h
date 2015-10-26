#ifndef _IGET_AREA_BY_IP_H__
#define _IGET_AREA_BY_IP_H__
//通过IP取地区信息

#include "../../Include/IUnknownEx.h"
#include "../../Include/commInterfaceDef.h"
//**************************************************************************************
static const CLASSUID	IF_UID_GET_AREA_BY_IP  ("IGetAreaByIP");
static const ulong		IF_VER_GET_AREA_BY_IP = INTERFACE_VER(2,0);

class IGetAreaByIP : public IUnknownEx2
{
public:
	virtual bool InitDatabase()	= 0;
	virtual void FreeDatabase()	= 0;
	virtual bool IsDBLoaded() const = 0;

	//取地区，如果数据库没初始化,LoadGetAreaByIP()会初始化，GetAreaByIP()不会
	virtual char* GetAreaByIP	 (ulong ulIP,char szBuff[16],int nLen)	const = 0;
	virtual char* LoadGetAreaByIP(ulong ulIP,char szBuff[16],int nLen)		  = 0;	
};
//**************************************************************************************
#define	VER_GET_AREA_BY_IP_MAIN		2
#define	VER_GET_AREA_BY_IP_SUB		0
#define	VER_GET_AREA_BY_IP_BUILD	0

#ifdef _DEBUG
	#define GET_AREA_BY_IP_DLL	"GetAreaByIPD"DLL_EXT
#else
	#define GET_AREA_BY_IP_DLL	"GetAreaByIP"DLL_EXT
#endif

class CGetAreaByIPLoader : public InterfaceLoader<IGetAreaByIP,
							VER_GET_AREA_BY_IP_MAIN,VER_GET_AREA_BY_IP_SUB,VER_GET_AREA_BY_IP_BUILD>
{
protected:
	virtual const char *GetDllFileName()	{	return GET_AREA_BY_IP_DLL;		}
	virtual const char *GetProcName()		{	return "GetGetAreaByIP";		}
public:
};
//**************************************************************************************
#endif
