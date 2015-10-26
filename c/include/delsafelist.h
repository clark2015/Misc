#ifndef	_DEL_SAFE_LIST_H__
#define	_DEL_SAFE_LIST_H__
/*
2005-04		��袳�
2005-05-12			ʹ��rh8����û��ô��warning,rh7.3��֪�����
2006-09-05			ȥ��һЩû�õľɴ���
					����һ���Ż�
2007-07-27			����GetSize()
2007-12-09			����Find()
2013-06-04			Ubuntu 8.04,gcc4.2.3����ͨ��
*/

#include "commtools.h"
#include <list>
#include <vector>

typedef	void * DSL_ENUM_POS;

//��Ƕ�ױ�����ȫ��List
template < class OBJ>
class CDelSafeList : protected std::list<OBJ>	
{
protected:	
	typedef typename CDelSafeList<OBJ>::iterator		SafeListItor;
	typedef typename CDelSafeList<OBJ>::const_iterator	Const_SafeListItor;
	typedef			 std::list<SafeListItor>			PositionList;
	typedef typename std::list<SafeListItor>::iterator 	PosListItor;
		
	PositionList  m_Positions;
		
#ifdef _DEBUG
public:
	int	GetPosListCount()	{	return m_Positions.size();	}
#endif

public:
	bool IsEmpty()	const;
	void AddHead(const OBJ & obj);
	void AddTail(const OBJ & obj);
	bool IsExist(const OBJ & obj) const;
	
		  OBJ* Find(const OBJ & obj);
	const OBJ* Find(const OBJ & obj) const;

	DSL_ENUM_POS BeginEnum();
	OBJ& GetNext(DSL_ENUM_POS );
	bool IsEnd(DSL_ENUM_POS);
	void EndEnum(DSL_ENUM_POS );
	bool Remove(DSL_ENUM_POS );		//�������Ҫ����,��δ����,������ʱ����positionָ���Obj����
									//�Ѿ�ɾ��,�Ӷ�ָ������һ��Obj,��ɴ����ɾ��
	bool Remove(const OBJ &	);
	int  GetSize() const	{	return this->size();	}	//�������ģ��е��ʱ
};

//********************************************************************
template < class OBJ>
void CDelSafeList<OBJ>::AddHead(const	OBJ	&obj)
{
	this->push_front(obj);
}

template < class OBJ>
void CDelSafeList<OBJ>::AddTail(const	OBJ	&obj)
{
	this->push_back(obj);
}

template < class OBJ>
DSL_ENUM_POS  CDelSafeList<OBJ>::BeginEnum()
{
	SafeListItor pos;
	pos	= this->begin();
	m_Positions.push_back(pos);					//��λ���б��м���һ���µ�λ��

	PosListItor result = m_Positions.end();		//��λ�õ�ָ�뷵��
	--result;
	return &(*result);
}

template < class OBJ>
bool CDelSafeList<OBJ>::IsEnd(DSL_ENUM_POS position)
{
	SafeListItor *pIterator	= (SafeListItor	*)position;

	return *pIterator == this->end();
}

template < class OBJ>
OBJ& CDelSafeList<OBJ>::GetNext(DSL_ENUM_POS	position)
{
	SafeListItor *pIterator	= (SafeListItor	*)position;

	if ( *pIterator	== this->end() )
	{
		ASSERT(! "CDelSafeList<OBJ>::GetNext()����Position�ѵ��˶��е�δβ��");
	//		return OBJ(0);	//!!!
	}

	SafeListItor temp =	*pIterator;
	(*pIterator)++;									//λ��ָ����һ��
	return *temp;			//���ص�ǰֵ
}

template < class OBJ>
void CDelSafeList<OBJ>::EndEnum(DSL_ENUM_POS	position)
{
	SafeListItor *pIterator	= (SafeListItor	*)position;

	PosListItor iter	= m_Positions.begin();		//����λ���б�
	PosListItor _END	= m_Positions.end();		//
	for	(;iter != _END; ++iter)						//
	{
		if ( &(*iter) == pIterator )
		{
			m_Positions.erase(iter);				//�ҵ���
			break;
		}
	}
}

template < class OBJ>
bool CDelSafeList<OBJ>::Remove(DSL_ENUM_POS  position)
{
	SafeListItor *pIterator	= (SafeListItor	*)position;

	if ( *pIterator	== this->end() )
	{
		assert(! "CDelSafeList<OBJ>::Remove()����Position�ѵ��˶��е�δβ��");
		return false;
	}

	SafeListItor temp =	*pIterator;

	PosListItor iter = m_Positions.begin();	   		//����λ���б�
	PosListItor _END = m_Positions.end();			//
	for ( ; iter != _END; ++iter)					//
	{
		if ( *iter == temp	)
		{
			(*iter)++;
		}
	}
	
	erase(temp);
	return true;			//���ص�ǰֵ
}

template < class OBJ>
bool CDelSafeList<OBJ>::Remove(const OBJ & obj)
{
	SafeListItor _F	= this->begin();
	SafeListItor _L	= this->end();
	while( _F != _L	)
	{
		if (*_F == obj)
			break;
		else
		   ++_F;
	}
	if ( _F == _L )		   //not found
		return false;

	PosListItor	iter = m_Positions.begin();	   			//����λ���б�
	PosListItor _END = m_Positions.end();				//
	for ( ;iter != _END; ++iter)						//
	{
		if ( *iter == _F	)
		{
			(*iter)++;
		}
	}

	erase(_F);
	return true;
}

template < class OBJ>
OBJ*  CDelSafeList<OBJ>::Find(const OBJ & obj)
{
	SafeListItor _F	= this->begin();
	SafeListItor _L	= this->end();
	while( _F != _L	)
	{
		if (*_F == obj)
			return &(*_F);
		else
		   ++_F;
	}
	return NULL;
}

template < class OBJ>
const OBJ* CDelSafeList<OBJ>::Find(const OBJ & obj) const
{
	Const_SafeListItor _F	= this->begin();
	Const_SafeListItor _L	= this->end();
	while( _F != _L	)
	{
		if (*_F == obj)
			return &(*_F);
		else
		   ++_F;
	}
	return NULL;
}

template < class OBJ>
bool CDelSafeList<OBJ>::IsExist(const OBJ & obj) const 
{
	Const_SafeListItor _F	= this->begin();
	Const_SafeListItor _L	= this->end();
	while( _F != _L	)
	{
		if (*_F == obj)
			break;
		else
		   ++_F;
	}
	if ( _F == _L )		   //not found
		return false;
	return true;
}

template < class OBJ>
bool CDelSafeList<OBJ>::IsEmpty()			const
{
	return this->empty();
}

#endif

