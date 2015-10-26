#ifndef table_character_prop_H
#define table_character_prop_H

#if defined(IDBCACHE_H_)
extern StMmoModuleParam*        g_pModuleParam;
inline
ILineSet* character_prop_GetLineSet() 
{                                                                                   
    static ILineSet*s_p_line_set = NULL;                                            
    if ( NULL == s_p_line_set )                                                     
    {                                                                               
        s_p_line_set = g_pModuleParam->pDBCache->GetLineSet("character_prop");      
        if ( NULL == s_p_line_set )                                                 
        {                                                                           
            RelTrace("g_pModuleParam->pDBCache->GetLineSet(character_prop) fail");  
            assert( !"g_pModuleParam->pDBCache->GetLineSet(character_prop) fail");  
        }                                                                           
        else                                                                        
        {                                                                           
        /*    if ( s_p_line_set->GetLineSize() != sizeof(character_prop) )      
            {                                                                       
                RelTrace("s_p_line_set->GetLineSize() != sizeof(character_prop)");  
                assert( !"s_p_line_set->GetLineSize() != sizeof(character_prop)");  
                s_p_line_set = NULL;                                                
            }                                                                       
			*/
        }                                                                           
    }                                                                               
    return s_p_line_set;                                                            
}

inline
int32 character_prop_get_level(const void*p_prop)
{
	ILineSet*pLineSet = character_prop_GetLineSet();
	if ( NULL == pLineSet || NULL == p_prop )
		return 0;
	static int field_index = -1;
	if ( field_index < 0 )
		field_index = pLineSet->GetFieldIndex("level");
	return (int32)pLineSet->GetIntFieldValueFromData(p_prop,field_index);
}

#endif

#pragma pack(push,1)
struct character_prop_nt	//not table
{
    uint64  actor_id;
    int32   level;
	void    fill_zero()     { memset(this,0,sizeof(*this)); }
};
#pragma pack(pop)

#endif
