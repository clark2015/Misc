#ifndef I_LUAPUBLIC_H
#define I_LUAPUBLIC_H
#include "../DLTypeDefs.h"
struct lua_State;

struct ILuaPublic : public IUnknownEx2
{
	//注册IServerSlave,IStageSlave,及各种全局变量
	virtual bool RegisterGlobal(lua_State* s)		= 0;
	
	//注册IGameObject,IGamePlayer,IGameNPC等接口
	virtual bool RegisterIGameObject(lua_State* s)	= 0;
};


inline IByteArray* ToArray(lua_State *L,int index = 1,const char*pFunName = "")
{
	IByteArray*pArray = (IByteArray*)lua_touserdata(L,index);
	if  ( (uint32)pArray < 0xFFFF || !pArray->checkThisPointer() )
	{
		RelTrace("%s: ByteArray参数错误",pFunName);
		return NULL;
	}
	return pArray;
}
#ifdef _LUA_TINKER_H_
inline
uint64 GetGuidFromLua(lua_State *L,int idx)
{
	const uint64*p = (const uint64*)lua_topointer(L,idx);
	if( (uint32)p > 0xFFFF  )
		return *p;
	else
		return 0;
}

inline
void PushGuidToLua(lua_State *L,uint64 guid)
{
	lua_tinker::push(L,guid);
}

#define ADD_FUN_TO_LIB(name,func)	\
	lua_pushstring(L,name);			\
	lua_pushcfunction(L,func);		\
	lua_settable(L,-3);	

#define ADD_INT_TO_LIB(name,i)	\
	lua_pushstring(L,name);		\
	lua_pushinteger(L,i);		\
	lua_settable(L,-3);	
#endif

#endif
