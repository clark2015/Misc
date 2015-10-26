#ifndef owner_property_h___
#define owner_property_h___
#include "../IDBCache.h"

#pragma pack(push,1)
struct owner_property
{
    uint64  owner_id;
    uint32  prop_id;
    int32   prop_int32;
    int64   prop_int64;
    union
    {
        unsigned char full_buff[1026];
        struct
        {
			unsigned short len;
            unsigned char  buff[1024];
        }part;
    }prop_extend;

    enum
    {
        SIZEOF_prop_extend      = 1026
    };

    void    fill_zero()          { memset(this,0,sizeof(*this)); }
    uint32  struct_size()  const { return sizeof(*this);         }
    void    set_owner_id(uint64 value)          { owner_id = value;		}
    uint64  get_owner_id() const                { return owner_id;		}
    void    set_prop_id(uint32 value)           { prop_id = value;		}
    uint32  get_prop_id() const                 { return prop_id;		}
    void    set_prop_int32(int32 value)         { prop_int32 = value;	}
    int32   get_prop_int32() const              { return prop_int32;	}
    void    set_prop_int64(int64 value)         { prop_int64 = value;	}
    int64   get_prop_int64() const              { return prop_int64;	}
};
#pragma pack(pop)

//sizeof(owner_property) == 1050

#if defined(IDBCACHE_H_)
extern StMmoModuleParam*  g_pModuleParam;
struct owner_property_helper
{
    struct owner_property_lua_type_name : public owner_property
    {
        enum
        {
            LUA_TYPE_MAGIC_NO = 582417
        };
        uint32  lua_type_magic_no;
    };

    owner_property_lua_type_name Create()
    {
        owner_property_lua_type_name ret;
        //不初始化一下会警告，且程序会挂，原因待查
        ret.lua_type_magic_no = owner_property_lua_type_name::LUA_TYPE_MAGIC_NO;
        return ret;
    }
    static bool Check(const owner_property* p)
    {
        if ( (unsigned long)p <= 0xFFFF )
        {
            RelTrace("owner_property参数不合法");
            return false;
        }
        const owner_property_lua_type_name*p1 = (const owner_property_lua_type_name*)p;
        if ( p1->lua_type_magic_no != owner_property_lua_type_name::LUA_TYPE_MAGIC_NO )
        {
            RelTrace("owner_property参数不合法:LUA_TYPE_MAGIC_NO不相等");
            return false;
        }
        return true;
    }

    struct LuaLineSet
    {
        enum                {   MAGIC_NO = 0x9387146 }; //其实没什么用
        ulong               m_magic_no;
        ILineSet*           m_pLineSet;
        ILineSet::iterator  m_itor;
        //下面的函数都用实际表中的key类型而不是一定是uint64类型，以免编译器搞错
        bool ReadLine(uint32 key,owner_property* p)
        {
            if ( !owner_property_helper::Check(p) )
            {
                RelTrace("LuaLineSet::ReadLine() : 传入不是owner_property对象");
                return false;
            }
            if ( ! m_pLineSet )
            {
                RelTrace("LuaLineSet::ReadLine() : LineSet不可用");
                return false;
            }
            return !! m_pLineSet->ReadLine(key,p);
        }
        bool InsertLine(const owner_property* p)
        {
            if ( !owner_property_helper::Check(p) )
            {
                RelTrace("LuaLineSet::InsertLine() : 传入不是owner_property对象");
                return false;
            }
            if ( ! m_pLineSet )
            {
                RelTrace("LuaLineSet::InsertLine() : LineSet不可用");
                return false;
            }
            return !! m_pLineSet->InsertLine(p);
        }

        bool UpdateLine(const owner_property* p)
        {
            if ( !owner_property_helper::Check(p) )
            {
                RelTrace("LuaLineSet::UpdateLine() : 传入不是owner_property对象");
                return false;
            }
            if ( ! m_pLineSet )
            {
                RelTrace("LuaLineSet::UpdateLine() : LineSet不可用");
                return false;
            }
            return !! m_pLineSet->UpdateLine(p);
        }

        bool InsertOrUpdateLine(const owner_property* p)
        {
            if ( !owner_property_helper::Check(p) )
            {
                RelTrace("LuaLineSet::InsertOrUpdateLine() : 传入不是owner_property对象");
                return false;
            }
            if ( ! m_pLineSet )
            {
                RelTrace("LuaLineSet::InsertOrUpdateLine() : LineSet不可用");
                return false;
            }
            return !! m_pLineSet->InsertOrUpdateLine(p);
        }
        bool DeleteLine(uint32 key)
        {
            if ( ! m_pLineSet )
            {
                RelTrace("LuaLineSet::DeleteLine() : LineSet不可用");
                return false;
            }
            return !! m_pLineSet->DeleteLine(key);
        }
        bool FirstLine(owner_property* p)
        {
            if ( !owner_property_helper::Check(p) )
            {
                RelTrace("LuaLineSet::FirstLine() : 传入不是owner_property对象");
                return false;
            }
            if ( ! m_pLineSet )
            {
                RelTrace("LuaLineSet::FirstLine() : LineSet不可用");
                return false;
            }
            return !! m_pLineSet->FirstLine(m_itor,p);
        }
        bool NextLine(owner_property* p)
        {
            if ( !owner_property_helper::Check(p) )
            {
                RelTrace("LuaLineSet::NextLine() : 传入不是owner_property对象");
                return false;
            }
            if ( ! m_pLineSet )
            {
                RelTrace("LuaLineSet::NextLine() : LineSet不可用");
                return false;
            }
            return !! m_pLineSet->NextLine(m_itor,p);
        }

        ulong   GetLineSize     ()      const
        {
            if ( ! m_pLineSet )
            {
                RelTrace("LuaLineSet::GetLineSize() : LineSet不可用");
                return false;
            }
            return m_pLineSet->GetLineSize();
        }
        ulong   GetLineCount    ()      const
        {
            if ( ! m_pLineSet )
            {
                RelTrace("LuaLineSet::GetLineCount() : LineSet不可用");
                return false;
            }
            return m_pLineSet->GetLineCount();
        }
        ulong   GetFieldCount   ()      const
        {
            if ( ! m_pLineSet )
            {
                RelTrace("LuaLineSet::GetFieldCount() : LineSet不可用");
                return false;
            }
            return m_pLineSet->GetFieldCount();
        }
        bool    IsExist(uint32 key)  const
        {
            if ( ! m_pLineSet )
            {
                RelTrace("LuaLineSet::IsExist() : LineSet不可用");
                return false;
            }
            return m_pLineSet->IsExist(key);
        }
        bool IsValid()  const
        {
            return (uint32)this > 0xFFFF && !!m_pLineSet && m_magic_no == MAGIC_NO;
        }
    };

    static ILineSet* _GetLineSet(uint64 ownerid)
    {
        ILineSet*p = g_pModuleParam->pDBCache->GetLineSet("owner_property",ownerid);
        if ( !p )
            return NULL;
        if ( sizeof(owner_property) != p->GetLineSize() )
        {
            RelTrace("sizeof(owner_property) != p->GetLineSize()");
            assert( !"sizeof(owner_property) != p->GetLineSize()");
            return NULL;
        }
        return p;
    }

    LuaLineSet GetLineSet(uint64 ownerid)
    {
        ILineSet* p =  _GetLineSet(ownerid);
        if ( NULL == p )
        {
            static LuaLineSet s;
            s.m_pLineSet = NULL;
            s.m_magic_no = LuaLineSet::MAGIC_NO;
            return s;
        }
        LuaLineSet ret;
        ret.m_pLineSet = p;
        ret.m_magic_no = LuaLineSet::MAGIC_NO;
        return ret;
    }

#if defined(_LUA_TINKER_H_)
    static void register_class(lua_State*L)
    {
        lua_tinker::class_add<owner_property_lua_type_name>(L,"owner_property");
        lua_tinker::class_def<owner_property_lua_type_name>(L,"fill_zero",              &owner_property_lua_type_name::fill_zero);
        lua_tinker::class_def<owner_property_lua_type_name>(L,"struct_size",			&owner_property_lua_type_name::struct_size);
        lua_tinker::class_def<owner_property_lua_type_name>(L,"get_owner_id",           &owner_property_lua_type_name::get_owner_id);
        lua_tinker::class_def<owner_property_lua_type_name>(L,"set_owner_id",           &owner_property_lua_type_name::set_owner_id);
        lua_tinker::class_def<owner_property_lua_type_name>(L,"get_prop_id",            &owner_property_lua_type_name::get_prop_id);
        lua_tinker::class_def<owner_property_lua_type_name>(L,"set_prop_id",            &owner_property_lua_type_name::set_prop_id);
        lua_tinker::class_def<owner_property_lua_type_name>(L,"get_prop_int32",         &owner_property_lua_type_name::get_prop_int32);
        lua_tinker::class_def<owner_property_lua_type_name>(L,"set_prop_int32",         &owner_property_lua_type_name::set_prop_int32);
        lua_tinker::class_def<owner_property_lua_type_name>(L,"get_prop_int64",         &owner_property_lua_type_name::get_prop_int64);
        lua_tinker::class_def<owner_property_lua_type_name>(L,"set_prop_int64",         &owner_property_lua_type_name::set_prop_int64);

        lua_tinker::class_add<owner_property_helper>(L,"owner_property_helper_LIB");
        lua_tinker::class_def<owner_property_helper>(L,"Create",            &owner_property_helper::Create);
        lua_tinker::class_def<owner_property_helper>(L,"GetLineSet",        &owner_property_helper::GetLineSet);
        static owner_property_helper s_helper;
        lua_tinker::set<owner_property_helper>(L,"owner_property_helper",s_helper);

        lua_tinker::class_add<owner_property_helper::LuaLineSet>(L,"owner_property_helper_LuaLineSet");
        lua_tinker::class_def<owner_property_helper::LuaLineSet>(L,"IsValid",           &owner_property_helper::LuaLineSet::IsValid);
        lua_tinker::class_def<owner_property_helper::LuaLineSet>(L,"ReadLine",          &owner_property_helper::LuaLineSet::ReadLine);
        lua_tinker::class_def<owner_property_helper::LuaLineSet>(L,"UpdateLine",        &owner_property_helper::LuaLineSet::UpdateLine);
        lua_tinker::class_def<owner_property_helper::LuaLineSet>(L,"InsertLine",        &owner_property_helper::LuaLineSet::InsertLine);
        lua_tinker::class_def<owner_property_helper::LuaLineSet>(L,"DeleteLine",        &owner_property_helper::LuaLineSet::DeleteLine);
        lua_tinker::class_def<owner_property_helper::LuaLineSet>(L,"InsertOrUpdateLine",&owner_property_helper::LuaLineSet::InsertOrUpdateLine);
        lua_tinker::class_def<owner_property_helper::LuaLineSet>(L,"FirstLine",         &owner_property_helper::LuaLineSet::FirstLine);
        lua_tinker::class_def<owner_property_helper::LuaLineSet>(L,"NextLine",          &owner_property_helper::LuaLineSet::NextLine);
        lua_tinker::class_def<owner_property_helper::LuaLineSet>(L,"GetLineSize",       &owner_property_helper::LuaLineSet::GetLineSize);
        lua_tinker::class_def<owner_property_helper::LuaLineSet>(L,"GetLineCount",      &owner_property_helper::LuaLineSet::GetLineCount);
        lua_tinker::class_def<owner_property_helper::LuaLineSet>(L,"GetFieldCount",     &owner_property_helper::LuaLineSet::GetFieldCount);
        lua_tinker::class_def<owner_property_helper::LuaLineSet>(L,"IsExist",           &owner_property_helper::LuaLineSet::IsExist);
    }
#endif
};
#endif

//对旧代码做点支持
//#include "../IDBCache.h"
#include "../../ILogger.h"
//extern StMmoModuleParam*	g_pModuleParam;
inline
bool __IsPropExist(uint64 ullOwnerID,ulong ulPropID)
{
	ILineSet* pLineSet = owner_property_helper::_GetLineSet(ullOwnerID);
	if ( NULL == pLineSet )
	{
		RelTrace("GetLineSet(%s,%llu)not exist?","owner_property",ullOwnerID);
		return false;
	}
	return pLineSet->IsExist(ulPropID);
}

inline
void* __ReadPropBuff(uint64 ullOwnerID,ulong ulPropID,void*pBuff,ulong&ulLen )
{
	ILineSet* pLineSet = owner_property_helper::_GetLineSet(ullOwnerID);
	if ( NULL == pLineSet )
	{
		RelTrace("GetLineSet(%s,%llu)not exist?","owner_property",ullOwnerID);
		ulLen = 0;
		return NULL;
	}
	const owner_property*pProp = (const owner_property*)pLineSet->ReadLine(ulPropID);
	if ( pProp && pBuff && ulLen > 0 )
	{
		ulLen = __min(ulLen,pProp->prop_extend.part.len);
		if ( ulLen )
			memcpy(pBuff,pProp->prop_extend.part.buff,ulLen);
		return pBuff;
	}
	ulLen = 0;
	return NULL;
}

inline
bool __WritePropBuff(uint64 ullOwnerID,ulong ulPropID,const void *pBuff,ulong ulLen)
{
	ILineSet* pLineSet = owner_property_helper::_GetLineSet(ullOwnerID);
	if ( NULL == pLineSet )
	{
		RelTrace("GetLineSet(%s,%llu)not exist?","owner_property",ullOwnerID);
		ulLen = 0;
		return false;
	}
	owner_property w;
	w.owner_id = ullOwnerID;
    w.prop_id	= ulPropID;
    w.prop_int32= 0;
    w.prop_int64= 0;
	if ( pBuff && ulLen > 0 )
	{
		w.prop_extend.part.len = (ushort)__min(ulLen,sizeof(w.prop_extend.part.buff));
		memcpy(w.prop_extend.part.buff,pBuff,w.prop_extend.part.len);
	}
	else
	{
		w.prop_extend.part.len = 0;
	}
	return pLineSet->UpdateLine(&w);
}

inline
bool	__CreateProp(uint64 ullOwnerID,ulong ulPropID,long lLongValue,longlong	llValue,const void*pExtBuff = 0,ulong ulExtBuffLen = 0)
{
	ILineSet* pLineSet = owner_property_helper::_GetLineSet(ullOwnerID);
	if ( NULL == pLineSet )
	{
		RelTrace("GetLineSet(%s,%llu)not exist?","owner_property",ullOwnerID);
		return false;
	}
	owner_property w;
	w.owner_id = ullOwnerID;
    w.prop_id	= ulPropID;
    w.prop_int32= lLongValue;
    w.prop_int64= llValue;
	if ( pExtBuff && ulExtBuffLen > 0 )
	{
		w.prop_extend.part.len = (ushort)__min(ulExtBuffLen,sizeof(w.prop_extend.part.buff));
		memcpy(w.prop_extend.part.buff,pExtBuff,w.prop_extend.part.len);
	}
	else
	{
		w.prop_extend.part.len = 0;
	}
	return pLineSet->InsertLine(&w);
}

inline
bool __DeleteProp(uint64 ullOwnerID,ulong ulPropID)
{
	ILineSet* pLineSet = owner_property_helper::_GetLineSet(ullOwnerID);
	if ( NULL == pLineSet )
	{
		RelTrace("GetLineSet(%s,%llu)not exist?","owner_property",ullOwnerID);
		return false;
	}
	return pLineSet->DeleteLine(ulPropID);
}

#endif
