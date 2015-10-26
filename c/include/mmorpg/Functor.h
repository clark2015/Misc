#ifndef FUNCTOR20091210_H_INCLUDE__
#define FUNCTOR20091210_H_INCLUDE__

#include "TypeList.h"
#include <list>
#include <algorithm>
#include <cstring>

namespace mmo_server
{
template <class _R, class TypeList>
class _FunctorImpl;

template<class _R>
class _FunctorImpl<_R, TypeList0>
{
protected:
    typedef _R(*_Fun)(const _FunctorImpl<_R, TypeList0>*);
    _FunctorImpl(_Fun __fun) : _M_fun(__fun){}
public:
    _R operator()()
    {
        return (*_M_fun)(this);
    }
private:
    _Fun    _M_fun;
};

template<class _R, class _Arg1>
class _FunctorImpl<_R, TypeList1<_Arg1> >
{
protected:
    typedef _R(*_Fun)(const _FunctorImpl<_R, TypeList1<_Arg1> > *, _Arg1);
    _FunctorImpl(_Fun __fun) : _M_fun(__fun){}
public:
    _R operator()(_Arg1 __arg1)
    {
        return (*_M_fun)(this, __arg1);
    }
private:
    _Fun    _M_fun;
};

template<class _R, class _Arg1, class _Arg2>
class _FunctorImpl<_R, TypeList2<_Arg1, _Arg2> >
{
protected:
    typedef _R(*_Fun)(const _FunctorImpl<_R, TypeList2<_Arg1, _Arg2> >*, _Arg1, _Arg2);
    _FunctorImpl(_Fun __fun) : _M_fun(__fun){}
public:
    _R operator()(_Arg1 __arg1, _Arg2 __arg2)
    {
        return (*_M_fun)(this, __arg1, __arg2);
    }
private:
    _Fun    _M_fun;
};

template<class _R, class _Arg1, class _Arg2, class _Arg3>
class _FunctorImpl<_R, TypeList3<_Arg1, _Arg2, _Arg3> >
{
protected:
    typedef _R(*_Fun)(const _FunctorImpl<_R, TypeList3<_Arg1, _Arg2, _Arg3> >*, _Arg1, _Arg2, _Arg3);
    _FunctorImpl(_Fun __fun) : _M_fun(__fun){}
public:
    _R operator()(_Arg1 __arg1, _Arg2 __arg2, _Arg3 __arg3)
    {
        return (*_M_fun)(this, __arg1, __arg2, __arg3);
    }
private:
    _Fun    _M_fun;
};

template<typename _ParentFunctor,
    typename _PointerToMemFn, 
    typename _PointerToObj>
class _MemFunHandler : public _FunctorImpl<typename _ParentFunctor::_ResultType, typename _ParentFunctor::_ParmList>
{
    //friend typename _ParentFunctor;
    typedef _FunctorImpl<typename _ParentFunctor::_ResultType, typename _ParentFunctor::_ParmList> _Parent;
public:
    _MemFunHandler(const _PointerToObj& __pObj, _PointerToMemFn __pMemFn)
        : _Parent(&_MemFunHandler<_ParentFunctor, _PointerToMemFn, _PointerToObj>::_invoker),
        _M_pObj(__pObj), _M_pMemFn(__pMemFn)
    {
    }

    static typename _ParentFunctor::_ResultType _invoker(const _Parent* pT)
    {
        const _MemFunHandler<_ParentFunctor, _PointerToMemFn, _PointerToObj>* pThis = static_cast<const _MemFunHandler<_ParentFunctor, _PointerToMemFn, _PointerToObj>*>(pT);
        return (*pThis->_M_pObj.*pThis->_M_pMemFn)();
    }

    static typename _ParentFunctor::_ResultType _invoker(
        const _Parent* pT,
        typename _ParentFunctor::_Arg1 __arg1)
    {
        const _MemFunHandler<_ParentFunctor, _PointerToMemFn, _PointerToObj>* pThis = static_cast<const _MemFunHandler<_ParentFunctor, _PointerToMemFn, _PointerToObj>*>(pT);
        return ((*pThis->_M_pObj).*pThis->_M_pMemFn)(__arg1);
    }

    static typename _ParentFunctor::_ResultType _invoker(
        const _Parent* pT,
        typename _ParentFunctor::_Arg1 __arg1,
        typename _ParentFunctor::_Arg2 __arg2)
    {
        const _MemFunHandler<_ParentFunctor, _PointerToMemFn, _PointerToObj>* pThis = static_cast<const _MemFunHandler<_ParentFunctor, _PointerToMemFn, _PointerToObj>*>(pT);
        return ((*pThis->_M_pObj).*pThis->_M_pMemFn)(__arg1, __arg2);
    }


    static typename _ParentFunctor::_ResultType _invoker(
        const _Parent* pT,
        typename _ParentFunctor::_Arg1 __arg1,
        typename _ParentFunctor::_Arg2 __arg2,
        typename _ParentFunctor::_Arg3 __arg3)
    {
        const _MemFunHandler<_ParentFunctor, _PointerToMemFn, _PointerToObj>* pThis = static_cast<const _MemFunHandler<_ParentFunctor, _PointerToMemFn, _PointerToObj>*>(pT);
        return ((*pThis->_M_pObj).*pThis->_M_pMemFn)(__arg1, __arg2, __arg3);
    }

private:
    _PointerToObj _M_pObj;
    _PointerToMemFn _M_pMemFn;
};

template <typename _R, typename TypeList = TypeList0>
class Functor
{
    typedef _FunctorImpl<_R, TypeList> _Impl;
    Functor(){memset(_M_buf, 0, sizeof(_M_buf));}
public:
    typedef Functor< _R, TypeList > FUNCTOR;
    typedef _R       _ResultType;
    typedef TypeList _ParmList;
    typedef typename _TypeAt<TypeList, 0>::_Result _Arg1;
    typedef typename _TypeAt<TypeList, 1>::_Result _Arg2;
    typedef typename _TypeAt<TypeList, 2>::_Result _Arg3;
    typedef typename _TypeAt<TypeList, 3>::_Result _Arg4;


    template <class _PointerToObj, class _PointerToMemFn>    
    Functor(const _PointerToObj& __pObj, _PointerToMemFn __pMemFn, const char* desc = NULL) 
    {
        memset(_M_buf, 0, sizeof(_M_buf));
#ifdef _WINDOW
        static_assert(sizeof(_M_buf) >= sizeof(_MemFunHandler<Functor, _PointerToMemFn, _PointerToObj>), "保留内存空间不足");
#endif
        _M_desc = desc;
        (new ((void*)_M_buf) _MemFunHandler<Functor, _PointerToMemFn, _PointerToObj>(__pObj, __pMemFn));
    }

    _R operator()()                                         { return (*GetImpl())(); }
    _R operator()(_Arg1 __arg1)                             { return (*GetImpl())(__arg1); }
    _R operator()(_Arg1 __arg1,_Arg2 __arg2)                { return (*GetImpl())(__arg1, __arg2); }
    _R operator()(_Arg1 __arg1,_Arg2 __arg2,_Arg3 __arg3)   { return (*GetImpl())(__arg1, __arg2, __arg3); }
    _Impl* GetImpl() const { return (_Impl*)_M_buf; }
    
    bool operator==(Functor<_R, TypeList> const & rhs) const
    {
        return memcmp(_M_buf, rhs._M_buf, sizeof(_M_buf)) == 0;
    }
    
    template< class _TRet, class _TList >
    bool operator==(Functor<_TRet, _TList> const & rhs) const
    {
        return false;
    }

    template< class _TRet, class _TList >
    bool operator!=(Functor<_TRet, _TList> const & rhs) const
    {
        return !operator==(rhs);
    }
    
    //friend bool operator< (const FUNCTOR& l, const FUNCTOR& r) 
	//{
    //    return memcmp(l._M_buf, r._M_buf, sizeof(FUNCTOR::_M_buf));
    //}
    bool operator<(Functor<_R, TypeList> const & rhs) const
    {
        return memcmp(_M_buf, rhs._M_buf, sizeof(_M_buf)) < 0;
    }
private:
    class _SizeType1
    {
    };
    class _SizeType2
    {
    };
    class _SizeType3 : public _SizeType1, public _SizeType2
    {
        //i'm not figure out, but i just know in this case the sizeof the function addr is 8
    };
    const char* _M_desc;
    char  _M_buf[sizeof(_MemFunHandler<Functor<_R, TypeList>,void (_SizeType3::*)(void),_SizeType3 *>)];
};

template <class _R>
class Functor0 : public Functor< _R, TypeList0 >
{
public:
    template <class _PointerToObj, class _PointerToMemFn>    
    Functor0(const _PointerToObj& __pObj, _PointerToMemFn __pMemFn, const char* __desc) : Functor< _R, TypeList0 >(__pObj, __pMemFn, __desc) {}
};

template <class _R, class _Arg1>
class Functor1 : public Functor< _R, TypeList1<_Arg1> >
{
public:
    template <class _PointerToObj, class _PointerToMemFn>    
    Functor1(const _PointerToObj& __pObj, _PointerToMemFn __pMemFn, const char* __desc) : Functor< _R, TypeList1<_Arg1> >(__pObj, __pMemFn, __desc) {}
};

template <class _R, class _Arg1, class _Arg2>
class Functor2 : public Functor< _R, TypeList2<_Arg1, _Arg2> >
{
public:
    template <class _PointerToObj, class _PointerToMemFn>    
    Functor2(const _PointerToObj& __pObj, _PointerToMemFn __pMemFn, const char* __desc) : Functor< _R, TypeList2<_Arg1, _Arg2> >(__pObj, __pMemFn, __desc) {}
};

template <class _R, class _Arg1, class _Arg2, class _Arg3>
class Functor3 : public Functor< _R, TypeList3<_Arg1, _Arg2, _Arg3> >
{
    template <class _PointerToObj, class _PointerToMemFn>    
    Functor3(const _PointerToObj& __pObj, _PointerToMemFn __pMemFn, const char* __desc) : Functor< _R, TypeList3<_Arg1, _Arg2, _Arg3> >(__pObj, __pMemFn, __desc) {}
};

template <class _Ret, class _Tp>
inline Functor0<_Ret> CreateFunctor(_Tp* __p, _Ret (_Tp::*__f)(), const char* desc = NULL)
  { return Functor0<_Ret>(__p, __f, desc); }

template <class _Ret, class _Tp, class _Arg1>
inline Functor1<_Ret, _Arg1> CreateFunctor(_Tp* __p, _Ret (_Tp::*__f)(_Arg1), const char* desc = NULL)
  { return Functor1<_Ret, _Arg1>(__p, __f, desc); }

template <class _Ret, class _Tp, class _Arg1, class _Arg2>
inline Functor2<_Ret, _Arg1, _Arg2> CreateFunctor(_Tp* __p, _Ret (_Tp::*__f)(_Arg1, _Arg2), const char* desc = NULL)
  { return Functor2<_Ret, _Arg1, _Arg2>(__p, __f, desc); }

template <class _Ret, class _Tp, class _Arg1, class _Arg2, class _Arg3>
inline Functor3<_Ret, _Arg1, _Arg2, _Arg3> CreateFunctor(_Tp* __p, _Ret (_Tp::*__f)(_Arg1, _Arg2, _Arg3), const char* desc = NULL)
  { return Functor3<_Ret, _Arg1, _Arg2, _Arg3>(__p, __f, desc); }

/*
template <class _Ret, class _Tp, class _Arg1, class _Arg2, class _Arg3>
inline Functor<_Ret, _Arg1, _Arg2, _Arg3> Functor(_Tp* __p, _Ret (_Tp::*__f)(_Arg1, _Arg2, _Arg3))
  { return Functor<_Ret, _Arg1, _Arg2, _Arg3>(__p, __f); }
*/

///////////////////////////////////////////////////////////////////////////////////////////////
template <class FUNCTOR>
class FunctorsList
{
	typedef std::list<FUNCTOR> ListFunctor;
public:
	// Add functor to list.
	void Add( const FUNCTOR &f )
	{
		m_functors.push_back( f );
	}
	// Remvoe functor from list.
	void Remove( const FUNCTOR &f )
	{
		typename ListFunctor::iterator it = std::find( m_functors.begin(),m_functors.end(),f );
		if (it != m_functors.end())
		{
			m_functors.erase( it );
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// Call all functors in this list.
	// Also several template functions for multiple parameters.
	//////////////////////////////////////////////////////////////////////////
	void Call()
	{
		for (typename ListFunctor::iterator it = m_functors.begin(); it != m_functors.end(); ++it)
		{
			(*it)();
		}
	}

	template <class T1>
	void Call( const T1 &param1 )
	{
		for (typename ListFunctor::iterator it = m_functors.begin(); it != m_functors.end(); ++it)
		{
			(*it)( param1 );
		}
	}

	template <class T1,class T2>
	void Call( const T1 &param1,const T2 &param2 )
	{
		for (typename ListFunctor::iterator it = m_functors.begin(); it != m_functors.end(); ++it)
		{
			(*it)( param1,param2 );
		}
	}

	template <class T1,class T2,class T3>
	void Call( const T1 &param1,const T2 &param2,const T3 &param3 )
	{
		for (typename ListFunctor::iterator it = m_functors.begin(); it != m_functors.end(); ++it)
		{
			(*it)( param1,param2,param3 );
		}
	}

private:
	ListFunctor m_functors;
};

} //namespace mmo_server
#endif //FUNCTOR20091210_H_INCLUDE__
