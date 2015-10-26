#ifndef IKeyworldFilter_h__
#define IKeyworldFilter_h__

#include "IMmoModule.h"
#include "IGameObject.h"

struct IKeyWordFilter : public IUnknownEx2
{
	virtual bool		LoadConfig(const char*pFile) = 0;

	virtual const char* GetFirstKeyWord	(const char*pStr,int *pLen = 0)	const = 0;

	virtual bool		IsKeyWord		(const char*pStr,int *pLen = 0)	const = 0;
};

#endif
