#ifndef _IJSON_PARSER_H__
#define _IJSON_PARSER_H__
#include "commInterfaceDef.h"
/*
JSON�ʷ����ķ������ӿ�
*/
//***********************************************************************
//�ʷ��������Ľ�����
class IJsonLexSink
{
public:
	//�ļ���ʼ����
	virtual bool OnFileBegin()	= 0;
	virtual bool OnFileEnd()	= 0;

	//����������,(������{}���ŵ�)
	virtual bool OnObjectBegin()= 0;
	virtual bool OnObjectEnd()	= 0;

	//����array����,(������[]���ŵ�)
	virtual bool OnArrayBegin()	= 0;
	virtual bool OnArrayEnd()	= 0;

	//����ð��
	virtual bool OnColon()	= 0;

	//��������
	virtual bool OnComma()	= 0;

	//��������
	virtual bool OnDigitInt  (long	 lDig,const char*pStr)	= 0;
	virtual bool OnDigitInt64(int64	llDig,const char*pStr)	= 0;
	virtual bool OnDigitFloat(double fDig,const char*pStr)	= 0;

	//�����ַ���
	virtual bool OnString(const char*pStr,int nLen)	= 0;

	//����boolֵ
	virtual bool OnBool(bool b)	= 0;

	//����null����
	virtual bool OnNullObj()	= 0;
};

//�ʷ�������
class IJsonLex
{
public:
	virtual void Release()	= 0;
	virtual bool Parser(const char *pJson,IJsonLexSink*pSyntax)	= 0;
};
//***********************************************************************
//�﷨��
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
