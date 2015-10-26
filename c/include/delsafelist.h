#ifndef	_DEL_SAFE_LIST_H__
#define	_DEL_SAFE_LIST_H__
/*
2005-04		蓝琚成
2005-05-12			使在rh8编译没那么多warning,rh7.3不知道如何
2006-09-05			去掉一些没用的旧代码
					做了一点优化
2007-07-27			增加GetSize()
2007-12-09			增加Find()
2013-06-04			Ubuntu 8.04,gcc4.2.3编译通过
*/

#include "commtools.h"
#include <list>
#include <vector>

typedef	void * DSL_ENUM_POS;

//可嵌套遍历安全的List
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
	bool Remove(DSL_ENUM_POS );		//这个函数要慎用,因未测试,且运行时参数position指向的Obj可能
									//已经删除,从而指向了另一个Obj,造成错误的删除
	bool Remove(const OBJ &	);
	int  GetSize() const	{	return this->size();	}	//数出来的，有点费时
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
	m_Positions.push_back(pos);					//在位置列表中加入一个新的位置

	PosListItor result = m_Positions.end();		//把位置的指针返回
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
		ASSERT(! "CDelSafeList<OBJ>::GetNext()出错，Position已到了队列的未尾！");
	//		return OBJ(0);	//!!!
	}

	SafeListItor temp =	*pIterator;
	(*pIterator)++;									//位置指向下一个
	return *temp;			//返回当前值
}

template < class OBJ>
void CDelSafeList<OBJ>::EndEnum(DSL_ENUM_POS	position)
{
	SafeListItor *pIterator	= (SafeListItor	*)position;

	PosListItor iter	= m_Positions.begin();		//遍历位置列表
	PosListItor _END	= m_Positions.end();		//
	for	(;iter != _END; ++iter)						//
	{
		if ( &(*iter) == pIterator )
		{
			m_Positions.erase(iter);				//找到了
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
		assert(! "CDelSafeList<OBJ>::Remove()出错，Position已到了队列的未尾！");
		return false;
	}

	SafeListItor temp =	*pIterator;

	PosListItor iter = m_Positions.begin();	   		//遍历位置列表
	PosListItor _END = m_Positions.end();			//
	for ( ; iter != _END; ++iter)					//
	{
		if ( *iter == temp	)
		{
			(*iter)++;
		}
	}
	
	erase(temp);
	return true;			//返回当前值
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

	PosListItor	iter = m_Positions.begin();	   			//遍历位置列表
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

