#ifndef actor_fixed_h
#define actor_fixed_h
#if defined(IDBCACHE_H_)
extern StMmoModuleParam*        g_pModuleParam;
#endif
#pragma pack(push,1)
struct actor_fixed
{
    uint64  actor_id;
    char    account_name[41];
    char    platform[21];
    char    actor_name[24];
    uint8   actor_profession;
    uint8   actor_sex;
    char    actor_via[33];
    uint32  server_id;

    enum
    {
        SIZEOF_account_name     = 41,
        SIZEOF_platform         = 21,
        SIZEOF_actor_name       = 24,
        SIZEOF_actor_via        = 33
    };

    void        fill_zero()          { memset(this,0,sizeof(*this)); }
    uint32      struct_size()  const { return sizeof(*this);         }
    void        set_actor_id(uint64 value)      { actor_id = value;     }
    uint64      get_actor_id() const            { return actor_id;      }
    void        set_account_name(const char* value)     { if (!value) value = ""; lstrcpyn(account_name,value,sizeof(account_name)); }
    uint32      get_sizeof_account_name() const {  return sizeof(account_name); }
    const char* get_account_name() const        { return account_name;  }
    void        set_platform(const char* value) { if (!value) value = ""; lstrcpyn(platform,value,sizeof(platform)); }
    uint32      get_sizeof_platform() const {  return sizeof(platform); }
    const char* get_platform() const    { return platform;      }
    void        set_actor_name(const char* value)       { if (!value) value = ""; lstrcpyn(actor_name,value,sizeof(actor_name)); }
    uint32      get_sizeof_actor_name() const {  return sizeof(actor_name);     }
    const char* get_actor_name() const  { return actor_name;    }
    void        set_actor_profession(uint8 value)       { actor_profession = value;     }
    uint8       get_actor_profession() const    { return actor_profession;      }
    void        set_actor_sex(uint8 value)      { actor_sex = value;    }
    uint8       get_actor_sex() const           { return actor_sex;     }
    void        set_actor_via(const char* value)        { if (!value) value = ""; lstrcpyn(actor_via,value,sizeof(actor_via)); }
    uint32      get_sizeof_actor_via() const {  return sizeof(actor_via);       }
    const char* get_actor_via() const   { return actor_via;     }
    void        set_server_id(uint32 value)     { server_id = value;    }
    uint32      get_server_id() const           { return server_id;     }

    #if defined(IDBCACHE_H_)
    static      ILineSet* GetLineSet()
    {
        static ILineSet*s_p_line_set = NULL;
        if ( NULL == s_p_line_set )
        {
            s_p_line_set = g_pModuleParam->pDBCache->GetLineSet("actor_fixed");
            if ( NULL == s_p_line_set )
            {
                RelTrace("pDBCache->GetLineSet(actor_fixed) fail");
                assert( !"pDBCache->GetLineSet(actor_fixed) fail");
            }
            else
            {
                if ( s_p_line_set->GetLineSize() != sizeof(actor_fixed) )
                {
                    RelTrace("s_p_line_set->GetLineSize() != sizeof(actor_fixed)");
                    assert( !"s_p_line_set->GetLineSize() != sizeof(actor_fixed)");
                    s_p_line_set = NULL;
                }
            }
        }
        return s_p_line_set;
    }
    #endif
};
#pragma pack(pop)

//sizeof(actor_fixed) == 133


#endif
