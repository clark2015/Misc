#ifndef _GameMapEnumer_h_
#define _GameMapEnumer_h_
#include "IGameMap.h"
#include "../IStage.h"
#include "../fastalloc.h"
#include <vector>

//for Ƕ�׵ı���
class INestingJobEnum
{
public:
	virtual bool	Nesting_OnGameObject(OBJECT_TYPE objType,const IGameObject*pObject)	= 0;	//����true��ʾ��Ҫ���� Nesting_DoTheJob ������,ע�������˷���trueҲ����� Nesting_DoTheJob
	virtual void	Nesting_DoTheJob()	= 0;
};

class CEnumForNestingJob : public IGameObjectEnum
{
//	std::vector<INestingJobEnum*>	m_All;
	typedef my_vector(INestingJobEnum*)	ENUMER_LIST;
	ENUMER_LIST	m_All;
	virtual bool OnGameObject(OBJECT_TYPE objType,IGameObject*pObject)
	{
		ENUMER_LIST::iterator _F = m_All.begin();
		ENUMER_LIST::iterator _L = m_All.end();
		bool bSend = false;
		for ( ; _F != _L; ++_F )
		{
			if ( (*_F)->Nesting_OnGameObject(objType,pObject) )
				bSend = true;
		}
		if ( bSend )
		{
			_F = m_All.begin();
			for ( ; _F != _L; ++_F )
				(*_F)->Nesting_DoTheJob();
		}
		return true;
	}
public:
	void Add(INestingJobEnum*pEnum)
	{
		m_All.push_back(pEnum);
	}
};
//************************************************************************************************************
//for �������ݸ�����
struct CEnumForSendBuff : public IGameObjectEnum
						, public INestingJobEnum 
{
	struct StItem
	{
		int					m_nLen;
		const void*			m_pBuff;
	};
	StItem				m_item;
//	vector<StItem>		m_vtItem;
	typedef my_vector(StItem)	ITEM_LIST;
	ITEM_LIST			m_vtItem;
	int					m_nUserCount;
	uint32				m_ulUserIDs[1000];
	
	virtual bool	OnGameObject(OBJECT_TYPE objType,IGameObject*pObject);
	virtual bool	Nesting_OnGameObject(OBJECT_TYPE objType,const IGameObject*pObject);
	virtual void	Nesting_DoTheJob();
public:
	CEnumForSendBuff(const void*pBuff,int nLen)	{	m_item.m_pBuff = (const char*)pBuff;	m_item.m_nLen = nLen;	m_nUserCount = 0;	}
	void Add(const void*pBuff,int nLen)	
	{	
		StItem item;
		item.m_pBuff	= pBuff;
		item.m_nLen		= nLen;
		m_vtItem.push_back(item);
	}
};

//************************************************************************************************************
//�ɼ��û�ID,�����Ƚϴ�
class CEnumForCollectUserID : public IGameObjectEnum
							, public INestingJobEnum 
{
	virtual bool	OnGameObject(OBJECT_TYPE objType,IGameObject*pObject);
	virtual bool	Nesting_OnGameObject(OBJECT_TYPE objType,const IGameObject*pObject);
	virtual void	Nesting_DoTheJob()	{}
public:
	int		m_nCount;
	uint32	m_ulUserIDs[STAGE_MAX_USERS];
	
	void Init()	{	m_nCount = 0;	}	
};
//************************************************************************************************************
//�ɼ�����ָ��,�����Ƚϴ�
struct CEnumForObjectPtr : public IGameObjectEnum
						 , public INestingJobEnum 
{
	virtual bool	OnGameObject(OBJECT_TYPE objType,IGameObject*pObject);
	virtual bool	Nesting_OnGameObject(OBJECT_TYPE objType,const IGameObject*pObject);
	virtual void	Nesting_DoTheJob()	{}

	uint32			m_ulObjectType;
public:
	int					m_nCount;
	const IGameObject*	m_Objects[STAGE_MAX_USERS];

	void Init(uint32 ulObjType)	{	m_nCount = 0;	m_ulObjectType = ulObjType;	assert(ulObjType&OBJECT_TYPE_ALL);	 }	
};
//************************************************************************************************************
//�ɼ�����GUID,�����Ƚϴ�
struct CEnumForObjectGUID: public IGameObjectEnum
						 , public INestingJobEnum 
{
	virtual bool	OnGameObject(OBJECT_TYPE objType,IGameObject*pObject);
	virtual bool	Nesting_OnGameObject(OBJECT_TYPE objType,const IGameObject*pObject);
	virtual void	Nesting_DoTheJob()	{}

	uint32			m_ulObjectType;
public:
	int				m_nCount;
	uint64			m_GUIDs[STAGE_MAX_USERS];

	void Init(uint32 ulObjType)	{	m_nCount = 0;	m_ulObjectType = ulObjType;	assert(ulObjType&OBJECT_TYPE_ALL);	 }	
};
//************************************************************************************************************
#include "GameMapEnumer.inl"
#endif
