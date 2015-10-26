#ifndef ICHATMODULE_H
#define ICHATMODULE_H
#include "IMmoModule.h"
#include "IGameObject.h"

struct IChatModule : public IUnknownEx2
{
	//系统消息
	virtual int	 FillSysMsg (char buff[1024],uint64 ullToActorID, const char*pMsg)	const = 0;

	virtual void SendCreatureChat(const char*pMsg,const IGameCreature*pFrom,uint64 ullToActorID)	const = 0;

	//喇叭消息,bAddToHead:是否加到队列头。消息长度要<100字节
	virtual bool AddBrodcastMsg(const char*pMsg,bool bAddToHead = true)	= 0;
};

#endif
