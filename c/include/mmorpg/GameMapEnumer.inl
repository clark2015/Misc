
inline bool CEnumForSendBuff::OnGameObject(OBJECT_TYPE objType,IGameObject*pObject)
{
	if ( Nesting_OnGameObject(objType,pObject) )
		Nesting_DoTheJob();
	return true;
}

inline bool CEnumForSendBuff::Nesting_OnGameObject(OBJECT_TYPE objType,const IGameObject*pObject)
{
	if ( NULL == pObject )
		return m_nUserCount > 0;

	if ( 0 == (objType & OBJECT_TYPE_PLAYER) )
		return false;
	m_ulUserIDs[m_nUserCount++] = pObject->GetPlayer()->GetUserID();
	return m_nUserCount >= lengthof(m_ulUserIDs);
}

inline void CEnumForSendBuff::Nesting_DoTheJob()
{
	if ( m_nUserCount )
	{
		GetStageSlave()->Send(m_item.m_pBuff,m_item.m_nLen,m_ulUserIDs,m_nUserCount);
		if ( !m_vtItem.empty() )
		{
			ITEM_LIST::iterator _F = m_vtItem.begin();
			ITEM_LIST::iterator _L = m_vtItem.end();
			for ( ; _F != _L; ++_F )
			{
				GetStageSlave()->Send((*_F).m_pBuff,(*_F).m_nLen,m_ulUserIDs,m_nUserCount);
			}
		}
		m_nUserCount = 0;
	}
}

inline bool CEnumForCollectUserID::OnGameObject(OBJECT_TYPE objType,IGameObject*pObject)
{
	if ( objType == OBJECT_TYPE_PLAYER )
		m_ulUserIDs[m_nCount++] = pObject->GetPlayer()->GetUserID();
	return true;
}

inline bool CEnumForCollectUserID::Nesting_OnGameObject(OBJECT_TYPE objType,const IGameObject*pObject)
{
	if ( objType == OBJECT_TYPE_PLAYER )
		m_ulUserIDs[m_nCount++] = pObject->GetPlayer()->GetUserID();
	return false;
}

inline bool CEnumForObjectPtr::OnGameObject(OBJECT_TYPE objType,IGameObject*pObject)
{
	Nesting_OnGameObject(objType,pObject);
	return true;
}
inline bool CEnumForObjectPtr::Nesting_OnGameObject(OBJECT_TYPE objType,const IGameObject*pObject)
{
	assert(m_nCount<lengthof(m_Objects));
	if ( (objType & m_ulObjectType) && m_nCount<lengthof(m_Objects) )
		m_Objects[m_nCount++] = pObject;
	return false;
}

inline bool CEnumForObjectGUID::OnGameObject(OBJECT_TYPE objType,IGameObject*pObject)
{
	Nesting_OnGameObject(objType,pObject);
	return true;
}
inline bool CEnumForObjectGUID::Nesting_OnGameObject(OBJECT_TYPE objType,const IGameObject*pObject)
{
	assert(m_nCount<lengthof(m_GUIDs));
	if ( (objType & m_ulObjectType) && m_nCount<lengthof(m_GUIDs) )
		m_GUIDs[m_nCount++] = pObject->GetGUID();
	return false;
}


