#ifndef READDISPATCHCFG_H
#define READDISPATCHCFG_H

#pragma pack(push,1)

struct StDispatchConfig
{
};

#pragma pack(pop)


bool ReadConfig(const char*pFileName)
{
	lua_State*L = luaL_newstate();
	luaL_openlibs(L);
	
	if ( 0 != luaL_loadfile(L,pFileName) ||
		 0 != lua_pcall(L, 0, 0, 0) )
		return 0;
	
	lua_getglobal(L,"DispatchServer");
	lua_getfield(L,-1,"IP");
		lstrcpyn(output->szDispServerIP,m_LuaWrapper.ToStr(),sizeof(output->szDispServerIP));
		m_LuaWrapper.Pop();
		lua_getfield(m_LuaWrapper.LS(),-1,"PORT");
		output->usDispServerPort = m_LuaWrapper.ToInt();
		m_LuaWrapper.Pop();
		lua_getfield(m_LuaWrapper.LS(),-1,"MAX_USER");
		m_InitParam.usMaxUserCount	= m_LuaWrapper.ToInt();
		if (m_InitParam.usMaxUserCount >= STAGE_MAX_USERS - 1000 ||
			m_InitParam.usMaxUserCount <= 0 )
		{
			RelTrace("最大用户数错");
			return false;
		}
		m_LuaWrapper.Pop();
		lua_getfield(m_LuaWrapper.LS(),-1,"WORLD_NAME");
		output->szCaption[0] = '[';
		lstrcpyn(output->szCaption + 1,m_LuaWrapper.ToStr(),50);
		lstrcpyn(m_InitParam.szWorldName,output->szCaption + 1,sizeof(m_InitParam.szWorldName));
		output->szCaption[strlen(output->szCaption)] = ']';
		output->szCaption[strlen(output->szCaption)] = '[';
		output->szCaption[strlen(output->szCaption)+1] = 0;
		m_LuaWrapper.Pop();
		m_LuaWrapper.Pop();
		
	lua_close(s);
}




#endif
