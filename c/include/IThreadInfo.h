#ifndef IThreadInfo_H
#define IThreadInfo_H

struct IThreadInfo
{
	virtual void Release()							= 0;
	virtual void SetThreadInfo	 (const char*pName)	= 0;
	virtual void UnSetThreadInfo()					= 0;
	virtual void TraceThreadInfo()					= 0;
};

#define VER_ITHREADINFO_MAIN	1
#define VER_ITHREADINFO_SUB		0
#define VER_ITHREADINFO_BUILD	0

#ifdef _DEBUG
	#define THREADINFO_DLL_NAME	"./ThreadInfoD"DLL_EXT
#else
	#define THREADINFO_DLL_NAME	"./ThreadInfo"DLL_EXT
#endif

class CThreadInfoLoader : public InterfaceLoader<IThreadInfo,VER_ITHREADINFO_MAIN,VER_ITHREADINFO_SUB,VER_ITHREADINFO_BUILD>
{
protected:
	virtual const char *GetDllFileName()	{	return THREADINFO_DLL_NAME;	}
	virtual const char *GetProcName()		{	return "CreateIThreadInfo";		}
};

inline void IThreadInfo_SetCurrentThreadName(const char*pName)
{
	CThreadInfoLoader	Loader;
	if ( !Loader.Create() )
		return;
	Loader->SetThreadInfo(pName);
}

inline void IThreadInfo_UnSetCurrentThreadName()
{
	CThreadInfoLoader	Loader;
	if ( !Loader.Create() )
		return;
	Loader->UnSetThreadInfo();
}

inline void IThreadInfo_TraceThreadInfo()
{
	CThreadInfoLoader	Loader;
	if ( !Loader.Create() )
		return;
	Loader->TraceThreadInfo();
}

#endif
