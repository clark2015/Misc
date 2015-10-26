#ifndef	_OBJ_CONTAINER_H_
#define	_OBJ_CONTAINER_H_
#include <map>
/*
����������ÿ��������Ψһ��,����������/ɾ������

��袳�	2005-3		�����ļ�
��袳�	2005-3-11	��id�ĳ�ģ�����,�Ա������string��Ϊid
��袳�	2005-4-16	����CAttribCase -> CAttribContainer
*/
//**********************************************************
template <class	ATTRIB_CLASS,	class ID_CLASS = unsigned long>
class CAttribContainer
{
protected:
	typedef	std::map<ID_CLASS,ATTRIB_CLASS> ContainerType;
	ContainerType  m_Container;
public:
	//��������,���ulAttribID������,��ʧ��
	bool AddAttrib(const ID_CLASS &ulAttribID,const ATTRIB_CLASS & AttribObj);

	//�ı�����,���ulAttribID������,��ʧ��
	bool SetAttrib(const ID_CLASS &ulAttribID,const ATTRIB_CLASS & AttribObj);

	//ɾ�����ԣ�,���ulAttribID������,��ʧ��
	bool DelAttrib(const ID_CLASS &ulAttribID)	{	return 0 !=	m_Container.erase(ulAttribID);		}

	//���������Ƿ����
	bool IsAttribExist(const ID_CLASS &ulAttribID);

	//ȡ����ֵ
	ATTRIB_CLASS & GetAttrib (const ID_CLASS &ulAttribID);
	ATTRIB_CLASS & operator[](const ID_CLASS &ulAttribID)	{	return GetAttrib(ulAttribID);	}

	//ȡ������Ŀ	
	int GetAttribCount()	{	return m_Container.size();	}
};
//**********************************************************
template <class	ATTRIB_CLASS,	class ID_CLASS>
bool CAttribContainer<ATTRIB_CLASS,ID_CLASS>::AddAttrib(const ID_CLASS &ulAttribID,const ATTRIB_CLASS & AttribObj)
{
	ContainerType::value_type  val(ulAttribID,AttribObj);
	std::pair<ContainerType::iterator,bool> ret = m_Container.insert( val );
	return ret.second;
}

template <class	ATTRIB_CLASS,	class ID_CLASS>
bool CAttribContainer<ATTRIB_CLASS,ID_CLASS>::SetAttrib(const ID_CLASS &ulAttribID,const ATTRIB_CLASS & AttribObj)
{
	ContainerType::iterator iter	= m_Container.find(ulAttribID);
	if ( iter == m_Container.end() )
		return false;

	(*iter).second = AttribObj;
		return true;
}

template <class	ATTRIB_CLASS,	class ID_CLASS>
bool CAttribContainer<ATTRIB_CLASS,ID_CLASS>::IsAttribExist(const ID_CLASS &ulAttribID)
{
	ContainerType::iterator iter	= m_Container.find(ulAttribID);
	if ( iter == m_Container.end() )
		return false;
	else
		return true;
}

template <class	ATTRIB_CLASS,	class ID_CLASS>
ATTRIB_CLASS & CAttribContainer<ATTRIB_CLASS,ID_CLASS>::GetAttrib(const ID_CLASS &ulAttribID)
{
	ContainerType::iterator iter	= m_Container.find(ulAttribID);
	if ( iter == m_Container.end() )
	{
		ASSERT(	! "GetAttrib():���Բ����ڣ�" );
		//		return ATTRIB_CLASS(0);
	}

	return (*iter).second;
}
//**********************************************************
#endif

