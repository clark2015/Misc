#ifndef	_OBJ_CONTAINER_H_
#define	_OBJ_CONTAINER_H_
#include <map>
/*
属性容器，每个属性是唯一的,可任意增加/删除属性

蓝琚成	2005-3		建立文件
蓝琚成	2005-3-11	把id改成模板参数,以便可以用string作为id
蓝琚成	2005-4-16	改名CAttribCase -> CAttribContainer
*/
//**********************************************************
template <class	ATTRIB_CLASS,	class ID_CLASS = unsigned long>
class CAttribContainer
{
protected:
	typedef	std::map<ID_CLASS,ATTRIB_CLASS> ContainerType;
	ContainerType  m_Container;
public:
	//增加属性,如果ulAttribID不存在,则失败
	bool AddAttrib(const ID_CLASS &ulAttribID,const ATTRIB_CLASS & AttribObj);

	//改变属性,如果ulAttribID不存在,则失败
	bool SetAttrib(const ID_CLASS &ulAttribID,const ATTRIB_CLASS & AttribObj);

	//删除属性，,如果ulAttribID不存在,则失败
	bool DelAttrib(const ID_CLASS &ulAttribID)	{	return 0 !=	m_Container.erase(ulAttribID);		}

	//测试属性是否存在
	bool IsAttribExist(const ID_CLASS &ulAttribID);

	//取属性值
	ATTRIB_CLASS & GetAttrib (const ID_CLASS &ulAttribID);
	ATTRIB_CLASS & operator[](const ID_CLASS &ulAttribID)	{	return GetAttrib(ulAttribID);	}

	//取属性数目	
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
		ASSERT(	! "GetAttrib():属性不存在！" );
		//		return ATTRIB_CLASS(0);
	}

	return (*iter).second;
}
//**********************************************************
#endif

