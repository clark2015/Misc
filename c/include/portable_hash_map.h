#ifndef	portable_hash_map_h
#define portable_hash_map_h

#ifdef _WIN32
	#include <hash_map>
	#include <hash_set>
	using std::hash_map;
	using std::hash_set;
	using std::hash_multimap;
	using std::hash_multiset;
	#if _MSC_VER >= 1600
		#include <unordered_map>
		#include <unordered_set>
		using std::unordered_map;
		using std::unordered_set;
	#endif
#else
	#include <ext/hash_map>
	#include <ext/hash_set>
	using __gnu_cxx::hash_map;
	using __gnu_cxx::hash_set;
	using __gnu_cxx::hash_multimap;
	using __gnu_cxx::hash_multiset;
	#include <tr1/unordered_map>
	#include <tr1/unordered_set>
	using std::tr1::unordered_map;
	using std::tr1::unordered_set;
	#include <string>
namespace __gnu_cxx
{
	template<>
	struct hash<ulonglong>
	{
	  size_t operator()(ulonglong __x) const { return size_t(__x) ^ size_t(__x >> 32); }
	};

	template<>
	struct hash<std::string>
	{
		size_t operator()(const std::string& __x) const 
		{
			const char*s = __x.c_str();
			unsigned long h = 0;    
			for ( ;*s;++s)
				h = 5*h + *s;
			return size_t(h);
		}
	};
};
#endif


#endif
