#ifndef _IJSON_PARSER_H__
#define _IJSON_PARSER_H__
#include "commInterfaceDef.h"
/*
JSON词法及文法分析接口
*/
//***********************************************************************
//词法分析器的接收器
class IJsonLexSink
{
public:
	//文件开始结束
	virtual bool OnFileBegin()	= 0;
	virtual bool OnFileEnd()	= 0;

	//遇到对象定义,(大括号{}括着的)
	virtual bool OnObjectBegin()= 0;
	virtual bool OnObjectEnd()	= 0;

	//遇到array定义,(中括号[]括着的)
	virtual bool OnArrayBegin()	= 0;
	virtual bool OnArrayEnd()	= 0;

	//遇到冒号
	virtual bool OnColon()	= 0;

	//遇到逗号
	virtual bool OnComma()	= 0;

	//遇到数字
	virtual bool OnDigitInt  (long	 lDig,const char*pStr)	= 0;
	virtual bool OnDigitInt64(int64	llDig,const char*pStr)	= 0;
	virtual bool OnDigitFloat(double fDig,const char*pStr)	= 0;

	//遇到字符串
	virtual bool OnString(const char*pStr,int nLen)	= 0;

	//遇到bool值
	virtual bool OnBool(bool b)	= 0;

	//遇到null对象
	virtual bool OnNullObj()	= 0;
};

//词法分析器
class IJsonLex
{
public:
	virtual void Release()	= 0;
	virtual bool Parser(const char *pJson,IJsonLexSink*pSyntax)	= 0;
};
//***********************************************************************
//语法树
class IJsonSyntaxTree
{
public:
	virtual void Release()	= 0;
//	virtual bool Parser(const char *pJson)	= 0;
};

//***********************************************************************
#define JSON_LEX_VER_MAIN	1
#define JSON_LEX_VER_SUB	0
#define JSON_LEX_VER_BUILD	0

#ifdef _DEBUG
	#define JSON_PARSER_DLL_NAME	"./JsonParserD"DLL_EXT
#else
	#define JSON_PARSER_DLL_NAME	"./JsonParser"DLL_EXT
#endif

class CIJsonLexLoader : public InterfaceLoader<IJsonLex,JSON_LEX_VER_MAIN,JSON_LEX_VER_SUB,JSON_LEX_VER_BUILD>
{
protected:
	virtual const char *GetDllFileName()	{	return JSON_PARSER_DLL_NAME;	}
	virtual const char *GetProcName()		{	return "CreateIJsonLex";		}
};


#endif
