#ifndef ICrossServerSubscribe_h
#define ICrossServerSubscribe_h
#include "../DLTypeDefs.h"
#include "../ISubscribeMagr.h"
//����������ĵĽӿ�
//ע��Ϊ�������������Ϣ����̫Ƶ������Ϣ���л����

struct ICrossServerSubscribeSink
{
	virtual void OnCrossServerSubscribeMsgReceived(const struct StServerInfo& Server,const StSubscribeMsg*pMsg) = 0;
};

struct ICrossServerSubscribe : public IUnknownEx2
{
	virtual void Subscribe(uint32 ulServerType,int nDSIndex,int lMsg,ICrossServerSubscribeSink *pSink,int lOrder = 0)	= 0;
};


//	�˶�̫�鷳
//	virtual void UnSubscribe(uint32 ServerType,			long lMsg,ICrossServerSubscribeSink *pSink);
	//��Ҫ���÷��������ֶ��ģ�����Լ���Ƿ���̫�鷳��
//	virtual void Subscribe	(const StServerName& Server,long lMsg,ICrossServerSubscribeSink *pSink,long lOrder = 0);
//	virtual void UnSubscribe(const StServerName& Server,long lMsg,ICrossServerSubscribeSink *pSink);

#endif
