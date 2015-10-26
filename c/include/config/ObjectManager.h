#pragma once

#include <map>
//#include "common/log.h"
#include "../mmorpg/typedef.h"
#include "../utilities/Singleton.h"
#include "LoadConfig.h"
#include "../commtools.h"

template< typename struct_name, typename map_t = std::map<uint32, struct_name*> >
class ObjectManager : private map_t, public CSingleton< ObjectManager< struct_name, map_t > >
{
    friend class CSingleton< ObjectManager< struct_name, map_t > >;
    friend class ObjectManagerAssist;
private:
    ObjectManager(){}
public:
    using map_t::iterator;
    using map_t::const_iterator;
    using map_t::begin;
    using map_t::end;
	using map_t::size;
	using map_t::empty;
public:
    typedef map_t struct_map_t;
    typedef std::pair<typename struct_map_t::const_iterator, typename struct_map_t::const_iterator> struct_pair_t;
    void LoadObjects(const char* file_name)
    {
        std::string str_file_name = "Data/";
        str_file_name += file_name;
//	#ifndef _WIN32
//		str_file_name = GBKToUTF8(str_file_name.c_str());
//	#endif
        mmo_server::Loader<struct_name, map_t>(*this).Load(str_file_name.c_str());

        //sLog.outString("从%s中读取%d项配置", file_name, _map_struct_name.size());

        return;
    }
    void ClearObjects()
    {
        for( typename map_t::iterator itr = begin(); itr != end(); ++itr )
            delete itr->second;
        this->clear();
    }
    void ReloadObjects(const char* file_name)
    {
        map_t map_temp;
        std::string str_file_name = "Data/";
        str_file_name += file_name;
        mmo_server::Loader<struct_name, map_t>(map_temp).Load(str_file_name.c_str());

        for(typename map_t::iterator itr = map_temp.begin(); itr != map_temp.end(); )
        {
            uint32 id = itr->first;
            typename map_t::iterator itr_old = this->find(id);
            if(itr_old != end())
            {
                *itr_old->second = *itr->second;
                ++itr;
            }
            else
            {
                insert(std::pair<uint32, struct_name*>(itr->first, itr->second));
                map_temp.erase(itr++);
            }
        }

        for( typename map_t::iterator itr = map_temp.begin(); itr != map_temp.end(); ++itr )
            delete itr->second;
        map_temp.clear();

        return;
    }

    const map_t& GetObjects() const { return *this; };
    struct_pair_t GetObjects(uint32 id) const { return this->equal_range(id); };
    const struct_name* GetLowerObject(uint32 id) const
    {
        typename map_t::const_iterator citr = this->lower_bound(id);
        return (citr == end()) ? NULL : citr->second;
    };

    const struct_name* GetObject(uint32 id) const
    {
        typename map_t::const_iterator citr = this->find(id);
        return (citr == end()) ? NULL : citr->second;
    }
    static const struct_name* sGetObject(uint32 id) { return ObjectManager< struct_name >::Instance().GetObject(id); }
private:
    map_t& GetObjects2() { return *this; };
    struct_name* GetObject2(uint32 id) const
    {
        typename map_t::const_iterator citr = this->find(id);
        return (citr == end()) ? NULL : citr->second;
    }
};
