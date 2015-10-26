#ifndef _IGAMELOG_H
#define _IGAMELOG_H
#include "IMmoModule.h"
#include "IGameObject.h"

struct IGameLog : public IUnknownEx2
{
	virtual void AddLog(const IGamePlayer*pPlayer,const char*pInterface,const char*pStr)	= 0;	//����ӿڻ��Զ�ת����utf8�����������url_encode

	//�Ѵ�Ϊ��¼ interface,gname,gid,sid,oldsid,time,dept ��(pPlayer��Ϊ��ʱ) user,roleid
	virtual void LogStart	(const IGamePlayer*pPlayer,const char*pInterface,uint32 sid = -1)		= 0;
	virtual void LogAddStr	(const char*pKey,const char*pValue,bool bConvToUtf8,bool bNeedUrlEncode)= 0;
	virtual void LogAddInt	(const char*pKey, int64 i) = 0;
	virtual void LogAddUInt	(const char*pKey,uint64 i) = 0;
	virtual void LogAddIPV4	(const char*pKey,uint32 i) = 0;
	virtual void LogEnd()	= 0;

	//
	virtual void LogStart360(const IGamePlayer*pPlayer,const char*pInterface,uint32 sid = -1)	= 0;
	virtual void LogAddSign360(const char*pInfo)	= 0;
};

#endif
