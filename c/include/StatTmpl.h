#ifndef STAT_TMPL_H_
#define STAT_TMPL_H_
#include <algorithm>
/*
Í³¼ÆÄ£°å
*/

template <class keyType,class countType=ulong>
class CCountStat
{
	struct StCount
	{
		keyType		key;
		countType	count;
		bool operator < (const StCount& x)	const {	return key < x.key;	}
	};
	typedef std::vector<StCount>	COUNT_ARRAY;
	COUNT_ARRAY	m_CountArray;

		  StCount* GetByKey(const keyType& key);
	const StCount* GetByKey(const keyType& key) const;
public:
	void		Add(const keyType& key,countType count = 1);
	countType	GetCount(const keyType& key) const;

	bool Reset(const keyType& key);
	void ResetAll();
	void Clear()	{	m_CountArray.clear();	}

	typedef typename COUNT_ARRAY::const_iterator const_iterator;
	const_iterator	begin()	const	{	return m_CountArray.begin();	}
	const_iterator	end()	const	{	return m_CountArray.end();		}
};

template <class keyType,class countType>
inline   typename CCountStat<keyType,countType>::StCount* CCountStat<keyType,countType>::GetByKey(const keyType& key)
{
	/*
	COUNT_ARRAY::iterator _F = m_CountArray.begin();	
	COUNT_ARRAY::iterator _L = m_CountArray.end();	
	for ( ; _F != _L; ++_F)	
	{	
		if ( (*_F).key == key )	
		{	
			return &(*_F);
		}	
	}
	return NULL;
	*/
	StCount k;
	k.key = key;
	typename COUNT_ARRAY::iterator it = std::lower_bound(m_CountArray.begin(),m_CountArray.end(),k);
	if ( it == m_CountArray.end() || it->key != key )
		return NULL;
	return &(*it);
}

template <class keyType,class countType>
inline 	const typename CCountStat<keyType,countType>::StCount* CCountStat<keyType,countType>::GetByKey(const keyType& key) const
{
	/*
	COUNT_ARRAY::const_iterator _F = m_CountArray.begin();	
	COUNT_ARRAY::const_iterator _L = m_CountArray.end();	
	for ( ; _F != _L; ++_F)	
	{	
		if ( (*_F).key == key )	
		{	
			return &(*_F);
		}	
	}
	return NULL;
	*/
	StCount k;
	k.key = key;
	typename COUNT_ARRAY::const_iterator it = std::lower_bound(m_CountArray.begin(),m_CountArray.end(),k);
	if ( it == m_CountArray.end() || it->key != key )
		return NULL;
	return &(*it);
}

template <class keyType,class countType>
inline countType CCountStat<keyType,countType>::GetCount(const keyType& key) const
{
	const StCount *p = GetByKey(key);
	if ( p )
		return p->count;
	else
		return 0;
}

template <class keyType,class countType>
inline void CCountStat<keyType,countType>::Add(const keyType& key,countType count)
{	
	StCount *p = GetByKey(key);
	if ( p )
	{
		p->count += count;
		return;
	}

	StCount a;	
	a.key		= key;	
	a.count		= count;	
	m_CountArray.push_back(a);
	std::sort(m_CountArray.begin(),m_CountArray.end());
}	

template <class keyType,class countType>
inline bool CCountStat<keyType,countType>::Reset(const keyType& key)
{
	StCount *p = GetByKey(key);
	if ( p )
	{
		p->count = 0;
		return true;
	}
	return false;
}

template <class keyType,class countType>
inline void CCountStat<keyType,countType>::ResetAll()
{
	typename COUNT_ARRAY::iterator _F = m_CountArray.begin();	
	typename COUNT_ARRAY::iterator _L = m_CountArray.end();	
	for ( ; _F != _L; ++_F)	
	{	
		(*_F).count = 0;
	}
}
/*
#ifdef _DEBUG
void TestStatTmpl()
{
	CCountStat<ulong>		CountStat;
	CCountStat<std::string> CountStatStr;
	
	CountStat.Add(100);
	CountStat.Add(101);
	CountStat.Reset(100);
	CountStat.Reset(1);
	CountStat.ResetAll();
	CountStat.Clear();

	CountStatStr.Add("Fuck");
	CountStatStr.Add("You");
	CountStatStr.Reset("Fuck");
	CountStatStr.Reset("a");
	CountStatStr.ResetAll();
	CountStatStr.Clear();
}
#endif
*/

#endif
