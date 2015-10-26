#ifndef CONFIGLOADER_H_INCLUDE_20120524__
#define CONFIGLOADER_H_INCLUDE_20120524__

#include "csv_parser.h"

namespace mmo_server
{
/*
template<class T>
static bool IsValid(T* pT)
{
    return true;
}
*/
template<class T>
uint32 GetObjectID(T* /*pT*/)
{
    return 0;
}

template< class T, class Map_t = std::map<uint32, T*> >
class Loader
{
    //typedef std::map<uint32, T*> Map_t;
public:
    Loader(Map_t& _map)
        : _map_data( _map )
    {}

    bool Load(const char* file_name)
    {
	    csv_parser file_parser;

	    // Specify the file to parse
	    file_parser.init(file_name);

	    // Check to see if there are more records, then grab each row one at a time
	    while(file_parser.has_more_rows())
	    {
		    // Get the record
		    vt_csv_row vt_row;
            
            file_parser.get_row(vt_row);
		    map_csv_row row;
            
            file_parser.get_map_row(row, vt_row);

            T* pT = new T();
            if ( !LoadData(row, pT) )
           // if(!IsValid<T>(pT))
            {
                delete pT;
                continue;
            }
            
            uint32 id = GetObjectID(pT);
            if(id == 0)
            {
                id = field(vt_row[0]);

                if(id == 0)
                {
                    delete pT;
                    continue;
                }
            }

        //    Insert<Map_t>(id, pT);
        	Insert(id, pT);
	    }

        return true;
    }
private:
/*	
    template<typename Container>
    void Insert(uint32 id, T* pT);
    template<>
    void Insert<std::multimap<uint32, T*>>(uint32 id, T* pT) {
        _map_data.insert(std::pair<uint32, T*>(id, pT));
    }
    template<>
    void Insert<std::map<uint32, T*>>(uint32 id, T* pT) {
        std::pair< Map_t::iterator,bool > ret = _map_data.insert(std::pair<uint32, T*>(id, pT));
        if(!ret.second)
        {
            //sLog.outError("LoadSaveConfig::Loader::Load duplicate data");
            delete pT;
        }
    }
    void Insert(uint32 id, T* pT) {
        std::pair<typename Map_t::iterator,bool > ret = _map_data.insert(std::pair<uint32, T*>(id, pT));
        if(!ret.second)
        {
            //sLog.outError("LoadSaveConfig::Loader::Load duplicate data");
            delete pT;
        }
    }
*/
    
    void Insert(uint32 id, T* pT) {
        _map_data.insert(std::pair<uint32, T*>(id, pT));
    }
private:
    Map_t&                 _map_data;
};

};
#endif //LOADCONFIG_H_INCLUDE_20120524__
