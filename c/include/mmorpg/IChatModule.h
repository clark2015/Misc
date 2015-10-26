#ifndef ICHATMODULE_H
#define ICHATMODULE_H
#include "IMmoModule.h"
#include "IGameObject.h"

struct IChatModule : public IUnknownEx2
{
	//ϵͳ��Ϣ
	virtual int	 FillSysMsg (char buff[1024],uint64 ullToActorID, const char*pMsg)	const = 0;

	virtual void SendCreatureChat(const char*pMsg,const IGameCreature*pFrom,uint64 ullToActorID)	const = 0;

	//������Ϣ,bAddToHead:�Ƿ�ӵ�����ͷ����Ϣ����Ҫ<100�ֽ�
	virtual bool AddBrodcastMsg(const char*pMsg,bool bAddToHead = true)	= 0;
};

#endif
