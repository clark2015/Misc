#ifndef table_global_var_h__
#define table_global_var_h__

#pragma pack(push,1)
struct global_var
{
    uint32  var_id;
    char    var_name[32];
    char    var_value[256];

    enum
    {
        SIZEOF_var_name         = 32,
        SIZEOF_var_value        = 256
    };

    void    fill_zero()          { memset(this,0,sizeof(*this)); }
    uint32  struct_size()  const { return sizeof(*this);         }
    void    set_var_id(uint32 value)            { var_id = value;       }
    uint32  get_var_id() const                  { return var_id;        }
    void    set_var_name(const char* value)     { if (!value) value = ""; lstrcpyn(var_name,value,sizeof(var_name)); }
    uint32      get_sizeof_var_name() const {  return sizeof(var_name); }
    const char* get_var_name() const    { return var_name;      }
    void    set_var_value(const char* value)    { if (!value) value = ""; lstrcpyn(var_value,value,sizeof(var_value)); }
    uint32      get_sizeof_var_value() const {  return sizeof(var_value);       }
    const char* get_var_value() const   { return var_value;     }
};
#pragma pack(pop)

//sizeof(global_var) == 292

#if defined(IDBCACHE_H_)
extern StMmoModuleParam*        g_pModuleParam;
struct global_var_helper
{
    struct global_var_lua_type_name : public global_var
    {
        enum
        {
            LUA_TYPE_MAGIC_NO = 33904
        };
        uint32  lua_type_magic_no;
    };

    global_var_lua_type_name Create()
    {
        global_var_lua_type_name ret;
        //����ʼ��һ�»ᾯ�棬�ҳ����ң�ԭ�����
        ret.lua_type_magic_no = global_var_lua_type_name::LUA_TYPE_MAGIC_NO;
        return ret;
    }
    static bool Check(const global_var* p)
    {
        if ( (unsigned long)p <= 0xFFFF )
        {
            RelTrace("global_var�������Ϸ�");
            return false;
        }
    #ifdef _WIN32
        if ( IsBadReadPtr (       p,sizeof(global_var_lua_type_name)) ||
             IsBadWritePtr((void*)p,sizeof(global_var_lua_type_name))   )
        {
            RelTrace("global_var�������Ϸ�(�ڴ治�ɶ���д)");
            return false;
        }
    #endif
        const global_var_lua_type_name*p1 = (const global_var_lua_type_name*)p;
        if ( p1->lua_type_magic_no != global_var_lua_type_name::LUA_TYPE_MAGIC_NO )
        {
            RelTrace("global_var�������Ϸ�:LUA_TYPE_MAGIC_NO�����");
            return false;
        }
        return true;
    }

    struct LuaLineSet
    {
        enum                {   MAGIC_NO = 0x9387146 }; //��ʵûʲô��
        ulong               m_magic_no;
        ILineSet*           m_pLineSet;
        ILineSet::iterator  m_itor;
        //����ĺ�������ʵ�ʱ��е�key���Ͷ�����һ����ulonglong���ͣ�������������
        bool ReadLine(uint32 key,global_var* p)
        {
            if ( !global_var_helper::Check(p) )
            {
                RelTrace("LuaLineSet::ReadLine() : ���벻��global_var����");
                return false;
            }
            if ( ! m_pLineSet )
            {
                RelTrace("LuaLineSet::ReadLine() : LineSet������");
                return false;
            }
            return !! m_pLineSet->ReadLine(key,p);
        }
        bool InsertLine(const global_var* p)
        {
            if ( !global_var_helper::Check(p) )
            {
                RelTrace("LuaLineSet::InsertLine() : ���벻��global_var����");
                return false;
            }
            if ( ! m_pLineSet )
            {
                RelTrace("LuaLineSet::InsertLine() : LineSet������");
                return false;
            }
            return !! m_pLineSet->InsertLine(p);
        }

        bool UpdateLine(const global_var* p)
        {
            if ( !global_var_helper::Check(p) )
            {
                RelTrace("LuaLineSet::UpdateLine() : ���벻��global_var����");
                return false;
            }
            if ( ! m_pLineSet )
            {
                RelTrace("LuaLineSet::UpdateLine() : LineSet������");
                return false;
            }
            return !! m_pLineSet->UpdateLine(p);
        }

        bool InsertOrUpdateLine(const global_var* p)
        {
            if ( !global_var_helper::Check(p) )
            {
                RelTrace("LuaLineSet::InsertOrUpdateLine() : ���벻��global_var����");
                return false;
            }
            if ( ! m_pLineSet )
            {
                RelTrace("LuaLineSet::InsertOrUpdateLine() : LineSet������");
                return false;
            }
            return !! m_pLineSet->InsertOrUpdateLine(p);
        }
        bool DeleteLine(uint32 key)
        {
            if ( ! m_pLineSet )
            {
                RelTrace("LuaLineSet::DeleteLine() : LineSet������");
                return false;
            }
            return !! m_pLineSet->DeleteLine(key);
        }
        bool FirstLine(global_var* p)
        {
            if ( !global_var_helper::Check(p) )
            {
                RelTrace("LuaLineSet::FirstLine() : ���벻��global_var����");
                return false;
            }
            if ( ! m_pLineSet )
            {
                RelTrace("LuaLineSet::FirstLine() : LineSet������");
                return false;
            }
            return !! m_pLineSet->FirstLine(m_itor,p);
        }
        bool NextLine(global_var* p)
        {
            if ( !global_var_helper::Check(p) )
            {
                RelTrace("LuaLineSet::NextLine() : ���벻��global_var����");
                return false;
            }
            if ( ! m_pLineSet )
            {
                RelTrace("LuaLineSet::NextLine() : LineSet������");
                return false;
            }
            return !! m_pLineSet->NextLine(m_itor,p);
        }

        ulong   GetLineSize()      const
        {
            if ( ! m_pLineSet )
            {
                RelTrace("LuaLineSet::GetLineSize() : LineSet������");
                return false;
            }
            return m_pLineSet->GetLineSize();
        }
        ulong   GetLineCount()      const
        {
            if ( ! m_pLineSet )
            {
                RelTrace("LuaLineSet::GetLineCount() : LineSet������");
                return false;
            }
            return m_pLineSet->GetLineCount();
        }
        ulong   GetFieldCount()      const
        {
            if ( ! m_pLineSet )
            {
                RelTrace("LuaLineSet::GetFieldCount() : LineSet������");
                return false;
            }
            return m_pLineSet->GetFieldCount();
        }
        bool    IsExist(uint32 key)  const
        {
            if ( ! m_pLineSet )
            {
                RelTrace("LuaLineSet::IsExist() : LineSet������");
                return false;
            }
            return m_pLineSet->IsExist(key);
        }
        bool IsValid()  const
        {
            return (uint32)this > 0xFFFF && !!m_pLineSet && m_magic_no == MAGIC_NO;
        }
    };

    static ILineSet* _GetLineSet()
    {
        static ILineSet*s_p_line_set = NULL;
        if ( NULL == s_p_line_set )
        {
            s_p_line_set = g_pModuleParam->pDBCache->GetLineSet("global_var");
            if ( NULL == s_p_line_set )
            {
                RelTrace("g_pModuleParam->pDBCache->GetLineSet(global_var) fail");
                assert( !"g_pModuleParam->pDBCache->GetLineSet(global_var) fail");
            }
            else
            {
                if ( s_p_line_set->GetLineSize() != sizeof(global_var) )
                {
                    RelTrace("s_p_line_set->GetLineSize() != sizeof(global_var)");
                    assert( !"s_p_line_set->GetLineSize() != sizeof(global_var)");
                    s_p_line_set = NULL;
                }
            }
        }
        return s_p_line_set;
    }

    global_var_helper() {       m_LineSet.m_pLineSet = NULL;    }
    LuaLineSet m_LineSet;
    bool _InitLineSet()
    {
        if ( m_LineSet.m_pLineSet == NULL )
        {
            m_LineSet.m_pLineSet = _GetLineSet();
            m_LineSet.m_magic_no = LuaLineSet::MAGIC_NO;
            return !!m_LineSet.m_pLineSet;
        }
        return true;
    }
    bool ReadLine(uint32 key,global_var* p)
    {
        if ( !_InitLineSet() )  return false;
        return m_LineSet.ReadLine(key,p);
    }
    bool InsertLine(const global_var* p)
    {
        if ( !_InitLineSet() )  return false;
        return m_LineSet.InsertLine(p);
    }
    bool UpdateLine(const global_var* p)
    {
        if ( !_InitLineSet() )  return false;
        return m_LineSet.UpdateLine(p);
    }
    bool InsertOrUpdateLine(const global_var* p)
    {
        if ( !_InitLineSet() )  return false;
        return m_LineSet.InsertOrUpdateLine(p);
    }
    bool DeleteLine(uint32 key)
    {
        if ( !_InitLineSet() )  return false;
        return m_LineSet.DeleteLine(key);
    }
    bool FirstLine(global_var* p)
    {
        if ( !_InitLineSet() )  return false;
        return m_LineSet.FirstLine(p);
    }
    bool NextLine(global_var* p)
    {
        if ( !_InitLineSet() )  return false;
        return m_LineSet.NextLine(p);
    }
    ulong   GetLineSize     ()
    {
        if ( !_InitLineSet() )  return false;
        return m_LineSet.GetLineSize();
    }
    ulong   GetLineCount    ()
    {
        if ( !_InitLineSet() )  return false;
        return m_LineSet.GetLineCount();
    }
    ulong   GetFieldCount   ()
    {
        if ( !_InitLineSet() )  return false;
        return m_LineSet.GetFieldCount();
    }
    bool    IsExist(uint32 key)
    {
        if ( !_InitLineSet() )  return false;
        return m_LineSet.IsExist(key);
    }

    LuaLineSet GetLineSet()
    {
        ILineSet* p =  _GetLineSet();
        if ( NULL == p)
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
        lua_tinker::class_add<global_var_lua_type_name>(L,"global_var");
        lua_tinker::class_def<global_var_lua_type_name>(L,"fill_zero",          &global_var_lua_type_name::fill_zero);
        lua_tinker::class_def<global_var_lua_type_name>(L,"struct_size",        &global_var_lua_type_name::struct_size);
        lua_tinker::class_def<global_var_lua_type_name>(L,"get_var_id",         &global_var_lua_type_name::get_var_id);
        lua_tinker::class_def<global_var_lua_type_name>(L,"set_var_id",         &global_var_lua_type_name::set_var_id);
        lua_tinker::class_def<global_var_lua_type_name>(L,"get_sizeof_var_name",                &global_var_lua_type_name::get_sizeof_var_name);
        lua_tinker::class_def<global_var_lua_type_name>(L,"get_var_name",               &global_var_lua_type_name::get_var_name);
        lua_tinker::class_def<global_var_lua_type_name>(L,"set_var_name",               &global_var_lua_type_name::set_var_name);
        lua_tinker::class_def<global_var_lua_type_name>(L,"get_sizeof_var_value",               &global_var_lua_type_name::get_sizeof_var_value);
        lua_tinker::class_def<global_var_lua_type_name>(L,"get_var_value",              &global_var_lua_type_name::get_var_value);
        lua_tinker::class_def<global_var_lua_type_name>(L,"set_var_value",              &global_var_lua_type_name::set_var_value);

        lua_tinker::class_add<global_var_helper>(L,"global_var_helper_LIB");
        lua_tinker::class_def<global_var_helper>(L,"Create",            &global_var_helper::Create);
        lua_tinker::class_def<global_var_helper>(L,"GetLineSet",        &global_var_helper::GetLineSet);
        lua_tinker::class_def<global_var_helper>(L,"ReadLine",          &global_var_helper::ReadLine);
        lua_tinker::class_def<global_var_helper>(L,"UpdateLine",        &global_var_helper::UpdateLine);
        lua_tinker::class_def<global_var_helper>(L,"InsertLine",        &global_var_helper::InsertLine);
        lua_tinker::class_def<global_var_helper>(L,"DeleteLine",        &global_var_helper::DeleteLine);
        lua_tinker::class_def<global_var_helper>(L,"InsertOrUpdateLine",&global_var_helper::InsertOrUpdateLine);
        lua_tinker::class_def<global_var_helper>(L,"FirstLine",         &global_var_helper::FirstLine);
        lua_tinker::class_def<global_var_helper>(L,"NextLine",          &global_var_helper::NextLine);
        lua_tinker::class_def<global_var_helper>(L,"GetLineSize",       &global_var_helper::GetLineSize);
        lua_tinker::class_def<global_var_helper>(L,"GetLineCount",      &global_var_helper::GetLineCount);
        lua_tinker::class_def<global_var_helper>(L,"GetFieldCount",     &global_var_helper::GetFieldCount);
        lua_tinker::class_def<global_var_helper>(L,"IsExist",           &global_var_helper::IsExist);
        static global_var_helper s_helper;
        lua_tinker::set<global_var_helper>(L,"global_var_helper",s_helper);

        lua_tinker::class_add<global_var_helper::LuaLineSet>(L,"global_var_helper_LuaLineSet");
        lua_tinker::class_def<global_var_helper::LuaLineSet>(L,"IsValid",           &global_var_helper::LuaLineSet::IsValid);
        lua_tinker::class_def<global_var_helper::LuaLineSet>(L,"ReadLine",          &global_var_helper::LuaLineSet::ReadLine);
        lua_tinker::class_def<global_var_helper::LuaLineSet>(L,"UpdateLine",        &global_var_helper::LuaLineSet::UpdateLine);
        lua_tinker::class_def<global_var_helper::LuaLineSet>(L,"InsertLine",        &global_var_helper::LuaLineSet::InsertLine);
        lua_tinker::class_def<global_var_helper::LuaLineSet>(L,"DeleteLine",        &global_var_helper::LuaLineSet::DeleteLine);
        lua_tinker::class_def<global_var_helper::LuaLineSet>(L,"InsertOrUpdateLine",&global_var_helper::LuaLineSet::InsertOrUpdateLine);
        lua_tinker::class_def<global_var_helper::LuaLineSet>(L,"FirstLine",         &global_var_helper::LuaLineSet::FirstLine);
        lua_tinker::class_def<global_var_helper::LuaLineSet>(L,"NextLine",          &global_var_helper::LuaLineSet::NextLine);
        lua_tinker::class_def<global_var_helper::LuaLineSet>(L,"GetLineSize",       &global_var_helper::LuaLineSet::GetLineSize);
        lua_tinker::class_def<global_var_helper::LuaLineSet>(L,"GetLineCount",      &global_var_helper::LuaLineSet::GetLineCount);
        lua_tinker::class_def<global_var_helper::LuaLineSet>(L,"GetFieldCount",     &global_var_helper::LuaLineSet::GetFieldCount);
        lua_tinker::class_def<global_var_helper::LuaLineSet>(L,"IsExist",           &global_var_helper::LuaLineSet::IsExist);
    }
#endif  //defined(_LUA_TINKER_H_)
};
#endif  //defined(IDBCACHE_H_)

enum
{
	GLOBAL_VAR_ID_FC20_FC		= 1,	//ս������20��ս����
	GLOBAL_VAR_ID_FC20_LEVEL	= 2,	//ս������20���ȼ�
};

#endif
