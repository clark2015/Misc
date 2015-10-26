#ifndef ICrossServerSubscribe_h
#define ICrossServerSubscribe_h
#include "../DLTypeDefs.h"
#include "../ISubscribeMagr.h"
//跨服务器订阅的接口
//注意为避免服务器间消息发送太频繁，消息是有缓存的

struct ICrossServerSubscribeSink
{
	virtual void OnCrossServerSubscribeMsgReceived(const struct StServerInfo& Server,const StSubscribeMsg*pMsg) = 0;
};

struct ICrossServerSubscribe : public IUnknownEx2
{
	virtual void Subscribe(uint32 ulServerType,int nDSIndex,int lMsg,ICrossServerSubscribeSink *pSink,int lOrder = 0)	= 0;
};


//	退订太麻烦
//	virtual void UnSubscribe(uint32 ServerType,			long lMsg,ICrossServerSubscribeSink *pSink);
	//不要搞用服务器名字订阅，否则对检查是否订阅太麻烦了
//	virtual void Subscribe	(const StServerName& Server,long lMsg,ICrossServerSubscribeSink *pSink,long lOrder = 0);
//	virtual void UnSubscribe(const StServerName& Server,long lMsg,ICrossServerSubscribeSink *pSink);

#endif
