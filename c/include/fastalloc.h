#ifndef FAST_ALLOC_H__
#define FAST_ALLOC_H__
//为vc订做的stl分配器
//2012-08-22	lanjc	可以用了，应该没问题吧
#include <map>

#if defined (_MSC_VER) && !defined (__SGI_STL_MAP) && !defined (_STLP_MAP) && !defined(NO_FAST_STL_ALLOC)

#include "fast_alloc_tmpl.h"
#include <xmemory>

template<class _Ty>
	class FAST_STL_ALLOC
	{
public:
	typedef typename _Ty value_type;

	typedef value_type _FARQ *pointer;
	typedef value_type _FARQ& reference;
	typedef const value_type _FARQ *const_pointer;
	typedef const value_type _FARQ& const_reference;

	typedef _SIZT size_type;
	typedef _PDFT difference_type;

	template<class _Other>
		struct rebind
		{	// convert this type to _ALLOCATOR<_Other>
		typedef FAST_STL_ALLOC<_Other> other;
		};

	pointer address(reference _Val) const
		{	// return address of mutable _Val
		return ((pointer) &(char&)_Val);
		}

	const_pointer address(const_reference _Val) const
		{	// return address of nonmutable _Val
		return ((const_pointer) &(char&)_Val);
		}

	FAST_STL_ALLOC() _THROW0()
		{	// construct default allocator (do nothing)
		}

	FAST_STL_ALLOC(const FAST_STL_ALLOC<_Ty>&) _THROW0()
		{	// construct by copying (do nothing)
		}

	template<class _Other>
		FAST_STL_ALLOC(const FAST_STL_ALLOC<_Other>&) _THROW0()
		{	// construct from a related allocator (do nothing)
		}

	template<class _Other>
		FAST_STL_ALLOC<_Ty>& operator=(const FAST_STL_ALLOC<_Other>&)
		{	// assign from a related allocator (do nothing)
		return (*this);
		}

	void deallocate(pointer _Ptr, size_type s)
		{	// deallocate object at _Ptr, ignore size
	//	::operator delete(_Ptr);
		CommonPooledAlloc::deallocate(_Ptr,s);
		}

	pointer allocate(size_type _Count)
		{	// allocate array of _Count elements
	//	return (_Allocate(_Count, (pointer)0));
		if ( _Count <= 0 )	return 0;	//好象跟new的标准不一样，不过 _Allocate 就是这样处理的
		return (pointer) CommonPooledAlloc::allocate(_Count * sizeof (_Ty));
		}

	pointer allocate(size_type _Count, const void _FARQ *)
		{	// allocate array of _Count elements, ignore hint
	//	return (allocate(_Count));
		if ( _Count <= 0 )	return 0;	//好象跟new的标准不一样，不过 _Allocate 就是这样处理的
		return (pointer) CommonPooledAlloc::allocate(_Count * sizeof (_Ty));
		}

	void construct(pointer _Ptr, const _Ty& _Val)
		{	// construct object at _Ptr with value _Val
		std::_Construct(_Ptr, _Val);
		}

	void construct(pointer _Ptr, _Ty&& _Val)
		{	// construct object at _Ptr with value _Val
		::new ((void _FARQ *)_Ptr) _Ty(_STD forward<_Ty>(_Val));
		}

	template<class _Other>
		void construct(pointer _Ptr, _Other&& _Val)
		{	// construct object at _Ptr with value _Val
		::new ((void _FARQ *)_Ptr) _Ty(_STD forward<_Other>(_Val));
		}

	void destroy(pointer _Ptr)
		{	// destroy object at _Ptr
		std::_Destroy(_Ptr);
		}

	_SIZT max_size() const _THROW0()
		{	// estimate maximum array size
		_SIZT _Count = (_SIZT)(-1) / sizeof (_Ty);
		return (0 < _Count ? _Count : 1);
		}
	};

#define my_list(v)					std::list		  <v,	FAST_STL_ALLOC<v> >
#define my_vector(v)				std::vector		  <v,	FAST_STL_ALLOC<v> >
#define my_set(k)					std::set		  <k,	std::less<k>,FAST_STL_ALLOC<k> > 
#define my_map(k,v)					std::map		  <k,v,	std::less<k>,FAST_STL_ALLOC<std::pair<const k, v> > > 
#define my_multimap(k,v)			std::multimap	  <k,v,	std::less<k>,FAST_STL_ALLOC<std::pair<const k, v> > > 
#define my_hash_set(k)				std::hash_set	  <k,	std::hash_compare<k, std::less<k> >,FAST_STL_ALLOC<k> >
#define my_hash_multiset(k)			std::hash_multiset<k,	std::hash_compare<k, std::less<k> >,FAST_STL_ALLOC<k> >
#define my_hash_map(k,v)			std::hash_map	  <k,v,	std::hash_compare<k, std::less<k> >,FAST_STL_ALLOC<std::pair<const k, v> > >
#define my_hash_multimap(k,v)		std::hash_multimap<k,v,	std::hash_compare<k, std::less<k> >,FAST_STL_ALLOC<std::pair<const k, v> > >
#define my_unordered_map(k,v)			 unordered_map<k,v, std::tr1::hash<k>,std::equal_to<k>,	FAST_STL_ALLOC<std::pair<const k, v> > >
#define my_unordered_multimap(k,v)  unordered_multimap<k,v, std::tr1::hash<k>,std::equal_to<k>,	FAST_STL_ALLOC<std::pair<const k, v> > >

#define _unordered_map			unordered_map
#define _unordered_set			unordered_set

#else
#define my_list(v)				std::list		  <v>
#define my_vector(v)			std::vector		  <v>
#define my_set(k)				std::set		  <k>
#define my_map(k,v)				std::map		  <k,v>
#define my_multimap(k,v)		std::multimap	  <k,v>
#define my_hash_set(k)				 hash_set	  <k>
#define my_hash_map(k,v)			 hash_map	  <k,v>
#define my_hash_multimap(k,v)		 hash_multimap<k,v>
//#define my_unordered_map(k,v)		 unordered_map<k,v>
#define my_unordered_map(k,v)		 hash_map<k,v>
#define my_unordered_multimap(k,v)   hash_multimap<k,v>

#define _unordered_map				hash_map
#define _unordered_set				hash_set

#endif

#endif
