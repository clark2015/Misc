#ifndef _ILUAWRAP_H__
#define _ILUAWRAP_H__
#include "DLTypeDefs.h"
#include "lua/lua.hpp"
#include "IByteArray.h"
#include "IUnknownEx.h"

class ILuaWrapper : public IUnknownEx
{
public:
	virtual lua_State*GetState()			= 0;	

	virtual int  GetTop() const				= 0;	
	virtual void Pop(int n = 1)				= 0;	

	virtual void PushUserData(void*p)			= 0;	
	virtual void PushBool(bool value)			= 0;	
	virtual void PushInt(int value)				= 0;	
	virtual void PushUInt(uint value)			= 0;	
	virtual void PushNumber(lua_Number value)	= 0;	
	virtual void PushFunction(lua_CFunction f)	= 0;
	virtual void PushStr(const char*p)			= 0;	
	virtual void PushNil()						= 0;	
	virtual void PushValue(int idx=-1)			= 0;
	virtual void PushByteArray(IByteArray*p)	= 0;

	virtual void*			ToUserData	(int idx = -1)	const	= 0;
	virtual bool			ToBool		(int idx = -1)	const	= 0;
	virtual int				ToInt		(int idx = -1)	const	= 0;
	virtual uint			ToUInt		(int idx = -1)	const	= 0;
	virtual lua_Number		ToNumber	(int idx = -1)	const	= 0;
	virtual lua_CFunction	ToFunction	(int idx = -1)	const	= 0;
	virtual const char*		ToStr		(int idx = -1)	const	= 0;
	virtual IByteArray*		ToByteArray	(int idx = -1,const char*pFuncName=NULL)	const	= 0;	//函数名是出错时Trace用的

	virtual void	RegisterFunction(const char*pName,	lua_CFunction fn)	= 0;
	virtual void	RegisterNumber  (const char*pName,	lua_Number	number)	= 0;
	virtual void	RegisterStr		(const char*pName,	const char*	pstr)	= 0;
	virtual void	RegisterLib		(const char*lib,	const luaL_Reg *l)	= 0;	//注意注册后要Pop
	virtual void	NewTable()					= 0;
	virtual void	SetTable(int idx = -3)		= 0;

	virtual bool DoFile(const char*pFileName)						= 0;
	virtual void SetCallFunc(const char*pFunctionName )				= 0;
	virtual bool PCall(int nArg,int nRes)							= 0;
	virtual void CallFunction(const char*func,const char*sig,...)	= 0;
	virtual void TraceInfo() const									= 0;

	virtual void GetGlobal(const char*key)				= 0;
	virtual void GetTable (int idx)						= 0;
	virtual void GetField (const char*key,int idx = -1)	= 0;
	virtual void GetField (int key,		  int idx = -1)	= 0;
	virtual void SetField (const char*key,int idx = -1)	= 0;
	virtual void SetField (int key,		  int idx = -1)	= 0;
	/*
	virtual bool		GetFieldBool	(const char*key,int idx = -1)	= 0;
	virtual bool		GetFieldBool	(int key,		int idx = -1)	= 0;
	virtual int			GetFieldInt		(const char*key,int idx = -1)	= 0;
	virtual int			GetFieldInt		(int key,		int idx = -1)	= 0;
	virtual lua_Number	GetFieldNumber	(const char*key,int idx = -1)	= 0;
	virtual lua_Number	GetFieldNumber	(int key,		int idx = -1)	= 0;
	virtual char*		GetFieldStr		(const char*key,char*s,int nLen,int idx = -1)	= 0;
	virtual char*		GetFieldStr		(int key,		char*s,int nLen,int idx = -1)	= 0;
	*/

	virtual int  GetType (int idx = -1)	= 0;	// lua_type	LUA_TNIL 等
	
	
	virtual void PrintCallStack(lua_State* L,int start = 0)	const = 0;
};

//***********************************************************************************

#endif
