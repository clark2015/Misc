#ifndef actor_detail_h
#define actor_detail_h
#if defined(IDBCACHE_H_)
extern StMmoModuleParam*        g_pModuleParam;
#endif
#pragma pack(push,1)
struct actor_detail
{
    uint64  actor_id;
    uint16  actor_faceid;
    uint64  instance_id;
    uint32  map_id;
    uint16  actor_x;
    uint16  actor_y;
    uint32  map_id_prev;
    uint16  actor_x_prev;
    uint16  actor_y_prev;
    int16   actor_direction;
    int32   actor_last_login;
    int32   actor_last_logout;
    uint32  actor_last_ip;
    uint32  actor_onlinetime;
    uint32  actor_right;
    uint8   is_adult;
    uint8   actor_never_login;
    uint32  deny_deadline;
    uint32  deny_chat_deadline;
	uint32  map_id_exit;

    void        fill_zero()          { memset(this,0,sizeof(*this)); }
    uint32      struct_size()  const { return sizeof(*this);         }
    void        set_actor_id(uint64 value)      { actor_id = value;     }
    uint64      get_actor_id() const            { return actor_id;      }
    void        set_actor_faceid(uint16 value)  { actor_faceid = value; }
    uint16      get_actor_faceid() const        { return actor_faceid;  }
    void        set_instance_id(uint64 value)   { instance_id = value;  }
    uint64      get_instance_id() const         { return instance_id;   }
    void        set_map_id(uint32 value)        { map_id = value;       }
    uint32      get_map_id() const              { return map_id;        }
    void        set_actor_x(uint16 value)       { actor_x = value;      }
    uint16      get_actor_x() const             { return actor_x;       }
    void        set_actor_y(uint16 value)       { actor_y = value;      }
    uint16      get_actor_y() const             { return actor_y;       }
    void        set_map_id_prev(uint32 value)   { map_id_prev = value;  }
    uint32      get_map_id_prev() const         { return map_id_prev;   }
    void        set_actor_x_prev(uint16 value)  { actor_x_prev = value; }
    uint16      get_actor_x_prev() const        { return actor_x_prev;  }
    void        set_actor_y_prev(uint16 value)  { actor_y_prev = value; }
    uint16      get_actor_y_prev() const        { return actor_y_prev;  }
    void        set_actor_direction(int16 value)        { actor_direction = value;      }
    int16       get_actor_direction() const     { return actor_direction;       }
    void        set_actor_last_login(int32 value)       { actor_last_login = value;     }
    int32       get_actor_last_login() const    { return actor_last_login;      }
    void        set_actor_last_logout(int32 value)      { actor_last_logout = value;    }
    int32       get_actor_last_logout() const   { return actor_last_logout;     }
    void        set_actor_last_ip(uint32 value) { actor_last_ip = value;        }
    uint32      get_actor_last_ip() const       { return actor_last_ip; }
    void        set_actor_onlinetime(uint32 value)      { actor_onlinetime = value;     }
    uint32      get_actor_onlinetime() const    { return actor_onlinetime;      }
    void        set_actor_right(uint32 value)   { actor_right = value;  }
    uint32      get_actor_right() const         { return actor_right;   }
    void        set_is_adult(uint8 value)       { is_adult = value;     }
    uint8       get_is_adult() const            { return is_adult;      }
    void        set_actor_never_login(uint8 value)      { actor_never_login = value;    }
    uint8       get_actor_never_login() const   { return actor_never_login;     }
    void        set_deny_deadline(uint32 value) { deny_deadline = value;        }
    uint32      get_deny_deadline() const       { return deny_deadline; }
    void        set_deny_chat_deadline(uint32 value)    { deny_chat_deadline = value;   }
    uint32      get_deny_chat_deadline() const  { return deny_chat_deadline;    }

    #if defined(IDBCACHE_H_)
    static      ILineSet* GetLineSet()
    {
        static ILineSet*s_p_line_set = NULL;
        if ( NULL == s_p_line_set )
        {
            s_p_line_set = g_pModuleParam->pDBCache->GetLineSet("actor_detail");
            if ( NULL == s_p_line_set )
            {
                RelTrace("pDBCache->GetLineSet(actor_detail) fail");
                assert( !"pDBCache->GetLineSet(actor_detail) fail");
            }
            else
            {
                if ( s_p_line_set->GetLineSize() != sizeof(actor_detail) )
                {
                    RelTrace("s_p_line_set->GetLineSize() != sizeof(actor_detail)");
                    assert( !"s_p_line_set->GetLineSize() != sizeof(actor_detail)");
                    s_p_line_set = NULL;
                }
            }
        }
        return s_p_line_set;
    }
    #endif
};
#pragma pack(pop)

//sizeof(actor_detail) == 66 + 4

#endif
