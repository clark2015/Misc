#ifndef sys_config_h__
#define sys_config_h__

#if defined(IDBCACHE_H_)
extern StMmoModuleParam*        g_pModuleParam;
#endif
#pragma pack(push,1)
struct sys_config
{
    uint32  cfg_id;
    char    cfg_name[32];
    char    cfg_value[256];

    enum
    {
        SIZEOF_cfg_name         = 32,
        SIZEOF_cfg_value        = 256
    };

    void        fill_zero()          { memset(this,0,sizeof(*this)); }
    uint32      struct_size()  const { return sizeof(*this);         }
    void        set_cfg_id(uint32 value)        { cfg_id = value;       }
    uint32      get_cfg_id() const              { return cfg_id;        }
    void        set_cfg_name(const char* value) { if (!value) value = ""; lstrcpyn(cfg_name,value,sizeof(cfg_name)); }
    uint32      get_sizeof_cfg_name() const {  return sizeof(cfg_name); }
    const char* get_cfg_name() const    { return cfg_name;      }
    void        set_cfg_value(const char* value)        { if (!value) value = ""; lstrcpyn(cfg_value,value,sizeof(cfg_value)); }
    uint32      get_sizeof_cfg_value() const {  return sizeof(cfg_value);       }
    const char* get_cfg_value() const   { return cfg_value;     }

    #if defined(IDBCACHE_H_)
    static      ILineSet* GetLineSet()
    {
        static ILineSet*s_p_line_set = NULL;
        if ( NULL == s_p_line_set )
        {
            s_p_line_set = g_pModuleParam->pDBCache->GetLineSet("sys_config");
            if ( NULL == s_p_line_set )
            {
                RelTrace("pDBCache->GetLineSet(sys_config) fail");
                assert( !"pDBCache->GetLineSet(sys_config) fail");
            }
            else
            {
                if ( s_p_line_set->GetLineSize() != sizeof(sys_config) )
                {
                    RelTrace("s_p_line_set->GetLineSize() != sizeof(sys_config)");
                    assert( !"s_p_line_set->GetLineSize() != sizeof(sys_config)");
                    s_p_line_set = NULL;
                }
            }
        }
        return s_p_line_set;
    }
    #endif
};
#pragma pack(pop)

//sizeof(sys_config) == 292
//lua lua/dbcache/_print_c_struct.lua sys_config.lua


#endif
