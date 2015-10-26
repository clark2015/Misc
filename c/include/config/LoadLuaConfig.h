#ifndef LOAD_LUA_CONFIG_H_
#define LOAD_LUA_CONFIG_H_
#include "csv_parser.h"
#include "../lua/lua.hpp"
#include "../portable_hash_map.h"
//		#pragma comment (lib , "../Lib/lua/lua52D.lib")

//TODO:Ƕ�׶�ȡ

class LuaField : public field
{
private:	
	int		m_luaType;
//	void*	m_pTable;
public:
	int		GetLuaType()	const	{	return m_luaType;	}	
	
	LuaField(int type,const char*pValue,size_t len) : field(pValue,len == -1 ? strlen(pValue) : len)
	{
		m_luaType 	= type;
	//	m_pTable	= NULL;
	}	
};

class LuaConfig
{
	struct __LuaFieldHasher
	{
		std::tr1::hash<std::string>	hx;
		size_t operator()(const LuaField& a) const	
		{	
			return hx(a.GetString());	
		}

		bool operator()(const LuaField& a, const LuaField& b) const		//�����Ƿ����
		{
			if ( a.GetLuaType() != b.GetLuaType() )
				return false;
			return a.GetString() == b.GetString();
		}
	};
	typedef unordered_map<LuaField,LuaField,__LuaFieldHasher,__LuaFieldHasher>	MAP_TYPE;
	MAP_TYPE	m_map;
public:
	bool Load(const char*pFIleName,const char*pTableName=NULL);
	bool Load(lua_State*L);

	const LuaField* GetValue(const char*pName,int lua_type = LUA_TSTRING,size_t len = -1) const		//LUA_TNUMBER
	{
		LuaField lf(lua_type,pName,len);
		MAP_TYPE::const_iterator it = m_map.find(lf);
		if( it == m_map.end() )
			return NULL;
		return &it->second;
	}
};

inline
bool LuaConfig::Load(const char*pFileName,const char*pTableName)
{
	if ( !pFileName || 0 == pFileName[0] )
	{
		RelTrace("LuaConfig::Load() �ļ�������");
		return false;
	}

	lua_State*L = luaL_newstate();
	luaL_openlibs(L);
	
	if ( 0 != luaL_loadfile(L,pFileName) ||
		 0 != lua_pcall(L, 0, 0, 0) )
	{
		RelTrace("LuaConfig::Load() �ļ�%s������",pFileName);
		lua_close(L);
		return false;
	}

	if ( 0 == pTableName || 0 == pTableName[0] )
		pTableName = "_G";

	lua_getglobal(L,pTableName);
	if ( lua_type(L,-1) != LUA_TTABLE )
	{
		RelTrace("LuaConfig::Load() �ļ�%s�Ҳ�����%s",pFileName,pTableName);
		lua_close(L);
		return false;
	}

	Load(L);
	lua_pop(L,1);
	lua_close(L);
	return true;
}

inline
bool LuaConfig::Load(lua_State*L)
{
	m_map.clear();

	// �������沽��ǰ�Ƚ� table ѹ��ջ�� 
	int nIndex = lua_gettop( L );  // ȡ table ����ֵ 
	lua_pushnil( L );  // nil ��ջ��Ϊ��ʼ key 
	while( 0 != lua_next( L, nIndex ) ) 
	{ 
		// ����ջ����-1���� value��-2 λ���Ƕ�Ӧ�� key 
		// ��������ж� key ��ʲô���Ҷ� value ���и��ִ��� 

		int typeKey   = lua_type(L,-2);
		int typeValue = lua_type(L,-1);
		
		size_t sizeKey;
		size_t sizeValue;
		const char*strKey;
		const char*strValue;
		
		switch(typeKey)
		{
		case LUA_TBOOLEAN:
			strKey	= lua_toboolean(L,-2) ? "1":"0";
			sizeKey	= 1;
			break;
		case LUA_TLIGHTUSERDATA:
		case LUA_TNUMBER:
		case LUA_TTABLE	:
		case LUA_TFUNCTION:
		case LUA_TUSERDATA:
		case LUA_TTHREAD:
		//	strKey = lua_tolstring(L,-2,&sizeKey);	����ȷ����LUA_TSTRING,�����ܶ�key����lua_tolstring(),�����Ӱ����һ��lua_nextkey
			lua_pushvalue(L,-2);
			strKey = lua_tolstring(L,-1,&sizeKey);
			lua_pop(L,1);
			break;
		case LUA_TSTRING:
			strKey = lua_tolstring(L,-2,&sizeKey);
			break;
		default:
			strKey = "";
			sizeKey= 0;
			RelTrace("LuaConfig::Load() : ʲô����(%u)��",typeKey);
			break;
		}

		switch(typeValue)
		{
		case LUA_TBOOLEAN:
			strValue	= lua_toboolean(L,-1) ? "1":"0";
			sizeValue	= 1;
			break;
		case LUA_TLIGHTUSERDATA:
		case LUA_TNUMBER:
		case LUA_TSTRING:
		case LUA_TTABLE	:
		case LUA_TFUNCTION:
		case LUA_TUSERDATA:
		case LUA_TTHREAD:
			strValue = lua_tolstring(L,-1,&sizeValue);
			break;
		default:
			strValue = "";
			sizeValue= 0;
			RelTrace("LuaConfig::Load() : ʲô����(%u)��",typeKey);
			break;
		}

		std::string a(strKey,sizeKey);
		LuaField key	(typeKey,	strKey,sizeKey);
		LuaField value	(typeValue,	strValue,sizeValue);

	//	DbgTrace("key %s,t=%u",strKey,typeKey);

		m_map.insert(MAP_TYPE::value_type(key,value));

		lua_pop( L, 1 );  // ���� value���� key ����ջ�� 
	}

	return true;
}

#endif
