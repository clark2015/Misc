#ifndef _I_SocialiyServer_H_________
#define _I_SocialiyServer_H_________
#include "IMmoModule.h"
#include "IGameObject.h"

struct ISocialityServer : public IUnknownEx2
{
	virtual bool SendChangePlayer(const char*pModName,const char*pDes,uint logreason,ushort usReasonCode,ulonglong ullInnerID,IGamePlayer*pPlayer,IGamePlayer*pPlayer2,int64 silver,int64 gold=0,int64 exp=0,int64 pow=0)	= 0;
//	virtual void SendCallQuestScriptCheck(IGamePlayer*pPlayer,uint32 ulQuestID)	= 0;
};

#endif
