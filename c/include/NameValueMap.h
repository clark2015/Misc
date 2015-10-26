#ifndef NAME_PTR_MAP_H
#define NAME_PTR_MAP_H
//#include <hash_map>
#include "portable_hash_map.h"
/*
//linux和sgi stl下未测试

#if defined (__SGI_STL_MAP) || defined (_STLP_MAP) || !defined(_WIN32)
struct NamePtrMap_str_hash
{
    size_t operator()(const char* str) const
    {
        return __stl_hash_string(str);
    }
};

namespace std
{
    template <>
    struct NamePtrMap_equal_to<const char*> : public binary_function<const char*, const char*, bool>
    {
        bool operator()(const char* str1, const char* str2) const
        {
            return 0==strcmp(str1, str2);
        }
    };
};
#else
struct NamePtrMap_less_str
{
	bool operator()(const char* s1, const char* s2) const
	{
		return strcmp(s1, s2) < 0;
	}
};
#endif


struct CNamePtrMap
{
#if defined (__SGI_STL_MAP) || defined (_STLP_MAP) || !defined(_WIN32)	
	typedef std::hash_map<const char*,void*,NamePtrMap_str_hash,std::NamePtrMap_equal_to<const char*> >	MAP_TYPE;
#else		
	typedef std::hash_map<const char*,void*,std::hash_compare<const char*,NamePtrMap_less_str> >		MAP_TYPE;
#endif
	
	MAP_TYPE	m_NamePtrMap;
public:
	bool  Insert(const  char*pName,void*pValue)		//注意这个函数假设pName不会删除掉
	{	
		if  ( !pName )
			return false;
		std::pair<MAP_TYPE::iterator,bool> ret = m_NamePtrMap.insert(MAP_TYPE::value_type(pName,pValue));	
		return ret.second;
	}
	void* Get(const char*pName)
	{
		if  ( !pName )
			return NULL;
		MAP_TYPE::iterator it = m_NamePtrMap.find(pName);
		if ( it == m_NamePtrMap.end() )
			return NULL;
		return it->second;
	}
	bool Remove(const char*pName)
	{
		if  ( !pName )
			return false;
		return 0 != m_NamePtrMap.erase(pName);
	}
};
*/
/*
#include <string>
class CNameValueMap
{
#ifdef _WIN32
	typedef hash_map<const std::string,void* >		MAP_TYPE;
#else
	typedef unordered_map<const std::string,void* >	MAP_TYPE;
#endif
	
	MAP_TYPE	m_NameValueMap;
public:
	bool  Insert(const std::string & sName,void*pValue)	
	{	
		std::pair<MAP_TYPE::iterator,bool> ret = m_NameValueMap.insert(MAP_TYPE::value_type(sName,pValue));	
		return ret.second;
	}
	void* Get(const std::string & sName) const
	{
		MAP_TYPE::const_iterator it = m_NameValueMap.find(sName);
		if ( it == m_NameValueMap.end() )
			return NULL;
		return it->second;
	}
	bool Remove(const std::string & sName)
	{
		return 0 != m_NameValueMap.erase(sName);
	}

	typedef MAP_TYPE::iterator			iterator;
	typedef MAP_TYPE::const_iterator	const_iterator;
	iterator		begin()			{	return m_NameValueMap.begin();	}
	const_iterator	begin()	const	{	return m_NameValueMap.begin();	}
	iterator		end()			{	return m_NameValueMap.end();	}
	const_iterator	end()	const	{	return m_NameValueMap.end();	}
};
*/
//未测试
#include <map>
//#if defined (_MSC_VER) && !defined (__SGI_STL_MAP) && !defined (_STLP_MAP) 

template<int MAX_STR_LEN,class _ValueType=void*>
class CLimitedLenNameValueMap
{
public:
	class limited_length_string
	{
		size_t	m_hash_value;
		char	m_str_buff[ (MAX_STR_LEN+1 + 3) & ~(4-1)];
		void calc_hash() 
		{
			assert(sizeof(m_str_buff) >= MAX_STR_LEN+1);
		#ifdef _MSC_VER
			// hash _Keyval to size_t value by pseudorandomizing transform
			long _Quot = (long)(stdext::hash_value(m_str_buff) & LONG_MAX);
			ldiv_t _Qrem = _CSTD ldiv(_Quot, 127773);

			_Qrem.rem = 16807 * _Qrem.rem - 2836 * _Qrem.quot;
			if (_Qrem.rem < 0)
				_Qrem.rem += LONG_MAX;
			m_hash_value = ((size_t)_Qrem.rem);
		#else
		//	static std::tr1::hash<const char*> h_;
		//	m_hash_value =	h_((const char*)m_str_buff);
			//上面这句不知道为什么不行？
			const char*s = m_str_buff;
			unsigned long h = 0;    
			for ( ;*s;++s)
				h = 5*h + *s;
			m_hash_value = size_t(h);
		#endif
		}
	public:
		limited_length_string()
		{
			m_str_buff[0] = 0;
			calc_hash();
		}

		limited_length_string(const char*p)
		{
			if ( p )
			{
				lstrcpyn(m_str_buff,p,sizeof(m_str_buff));
			}
			else
			{
				m_str_buff[0] = 0;
				assert(0);
			}
			calc_hash();
		}

		limited_length_string(const std::string & s)
		{
			lstrcpyn(m_str_buff,s.c_str(),sizeof(m_str_buff));
			calc_hash();
		}

		limited_length_string(const limited_length_string & s)
		{
			memcpy(this,&s,sizeof(*this));
		}

		size_t get_hash_value()	const {	return m_hash_value;	}


	#ifdef _MSC_VER
		bool less_then(const limited_length_string& o) const
		{
			return strcmp(m_str_buff,o.m_str_buff) < 0;
		}
	#else
		bool equal_to(const limited_length_string& o) const
		{
			if ( m_hash_value != o.m_hash_value )
				return false;
			return !strcmp(m_str_buff,o.m_str_buff);
		}
	#endif
		
	};
private:
#ifdef _MSC_VER
	struct my_str_hash_compare
	{
		enum	{	bucket_size = 1		};
		size_t	operator()(const limited_length_string& _Keyval1)										const	{	return _Keyval1.get_hash_value();		}
		bool	operator()(const limited_length_string& _Keyval1, const limited_length_string& _Keyval2)const	{	return _Keyval1.less_then(_Keyval2);	}
	};
	typedef		hash_map<const limited_length_string,_ValueType,my_str_hash_compare>	MAP_TYPE;
#else
	struct my_str_hash
	{
		size_t	operator()(const limited_length_string& _Keyval1)										const	{	return _Keyval1.get_hash_value();		}
	};
	struct my_str_equto
	{
		bool	operator()(const limited_length_string& _Keyval1, const limited_length_string& _Keyval2)const	{	return _Keyval1.equal_to(_Keyval2);	}
	};
	typedef		unordered_map<const limited_length_string,_ValueType,my_str_hash,my_str_equto>	MAP_TYPE;
#endif
	MAP_TYPE	m_NameValueMap;
public:
	//******************************************************************************
	bool  Insert(const limited_length_string & sName,const _ValueType& value)
	{	
		std::pair<typename MAP_TYPE::iterator,bool> ret = m_NameValueMap.insert(typename MAP_TYPE::value_type(sName,value));	
		return ret.second;
	}
	const _ValueType& Get(const limited_length_string & sName,const _ValueType& _nullValue = _ValueType(0)) const
	{
		typename MAP_TYPE::const_iterator it = m_NameValueMap.find(sName);
		if ( it == m_NameValueMap.end() )
			return _nullValue;
		return it->second;
	}
//	const _ValueType* GetPtr(const limited_length_string & sName) const
	_ValueType* GetPtr(const limited_length_string & sName)
	{
	//	typename MAP_TYPE::const_iterator it = m_NameValueMap.find(sName);
		typename MAP_TYPE::iterator		  it = m_NameValueMap.find(sName);
		if ( it == m_NameValueMap.end() )
			return NULL;
		return &it->second;
	}
	bool Remove(const limited_length_string & sName)
	{
		return 0 != m_NameValueMap.erase(sName);
	}

	//要慎用，因为遍历顺序无保证，录像功能会有问题,现在仅dbcache用到
	typedef typename MAP_TYPE::iterator			iterator;
	typedef typename MAP_TYPE::const_iterator	const_iterator;
	iterator		begin()			{	return m_NameValueMap.begin();	}
	const_iterator	begin()	const	{	return m_NameValueMap.begin();	}
	iterator		end()			{	return m_NameValueMap.end();	}
	const_iterator	end()	const	{	return m_NameValueMap.end();	}
	size_t			size()	const	{	return m_NameValueMap.size();	}
};

//#endif

/*
	CNamePtrMap m;
	m.Insert("XX",(void*)1);
	m.Insert("YY",(void*)2);
	m.Insert("ZZ",(void*)3);

	RelTrace("%d",m.Get("XX"));
	RelTrace("%d",m.Get("YY"));
	RelTrace("%d",m.Get("ZZ"));
	const char a[] = "YY";
	RelTrace("%d",m.Get(a));
	
	assert(m.Remove("XX"));
	assert(m.Remove("YY"));
	assert(m.Remove("ZZ"));
*/
#endif
