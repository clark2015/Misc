#ifndef __COMM_INTERFACE_DEF_H__
#define __COMM_INTERFACE_DEF_H__

/*
2005			蓝琚成		建立文件
2005-05-07		      		增加InterfaceLoader模板
2005-05-12		      		增加#pragma comment(lib,"kernel32.lib"),等
2007-02-28		      		定义windows下的dlopen()等宏
2009-04-07		      		去掉一些typedef并include DLTypeDefs.h
2010-08-12		      		Create函数增加返回错误字符串的参数
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32 
	#include <windows.h>
	#define EXPORT_FUNC __declspec(dllexport)
	#define DLL_EXT	".dll"
	#pragma comment(lib,"user32.lib")
	#pragma comment(lib,"gdi32.lib")
	#pragma comment(lib,"kernel32.lib")

	#define dlopen(x,y)	LoadLibrary(x)
	#define dlclose		FreeLibrary
	#define dlsym		GetProcAddress
#else	//linux/unix
	#include <dlfcn.h>
	#define EXPORT_FUNC __attribute ((visibility("default")))
	#define DLL_EXT	".so"
	
	#define LoadLibrary(dllfilename)	dlopen(dllfilename,RTLD_LAZY)
	#define FreeLibrary					dlclose
	#define GetProcAddress				dlsym
	typedef void * HMODULE;
#endif	//_WIN32 

#include "DLTypeDefs.h"
//#include "commtools.h"	//linux中需要lstrcpyn的定义	//有冲突，用strncpy
//****************************************************************

template <class _Interface,uint32 ulMainVer,uint32 ulMinVer,uint32 ulBuildVer>
class InterfaceLoader
	{
		typedef _Interface * (*_CreateInterface)(uint32 ulMainVer1,uint32 ulMinVer1,uint32 ulBuildVer1);

		_Interface* m_pInterface;
		HMODULE 	m_hMod;
	protected:
		virtual const char *GetDllFileName()	= 0;
		virtual const char *GetProcName()		= 0;
		char* SetDLErrorMsg(char*des,const char*src,int nLen)
		{
		#ifdef _WIN32
			lstrcpyn(des,src,nLen);
		#else
			const char*pErr = dlerror();
			if( pErr == NULL )
				pErr = "";
			snprintf(des,nLen,"%s. dlerror=%s",src,pErr);
		#endif
			return des;
		}
	public:
		InterfaceLoader()
			{
			m_hMod   	 = 0;
			m_pInterface = 0;
			}
		~InterfaceLoader()
			{
			Clean();
			}
		void Clean()
			{
			if ( m_pInterface )
				{
				m_pInterface->Release();
				m_pInterface = 0;
				}
			if ( m_hMod )
				{
				FreeLibrary(m_hMod);
				m_hMod = 0;
				} 
			}
		bool Create(char szErrorMsg[] = 0 ,int nLen = 0)
			{
			Clean();
			if ( szErrorMsg && nLen > 0 )
				szErrorMsg[0] = 0;

			m_hMod = LoadLibrary(GetDllFileName());
			if ( 0 != m_hMod )
				{
				_CreateInterface func = (_CreateInterface )GetProcAddress(m_hMod,GetProcName());
				if ( 0 != func )
					{
					m_pInterface = func(ulMainVer,ulMinVer,ulBuildVer);
					if ( NULL == m_pInterface )
						{
						if ( szErrorMsg && nLen > 0 )
							{
							SetDLErrorMsg(szErrorMsg,"create function return NULL",nLen);
							}
						}
					}
				else
					{
					if ( szErrorMsg && nLen > 0 )
						{
						SetDLErrorMsg(szErrorMsg,"can not get create function",nLen);
						}
					}
				}
			else
				{
				if ( szErrorMsg && nLen > 0 )
					{
					SetDLErrorMsg(szErrorMsg,"can not LoadDLL",nLen);
					}
				}
			if ( 0 == m_pInterface )
				Clean();
			return 0 != m_pInterface;			
			}
			  _Interface *GetInterface()		{	return m_pInterface;		}
		const _Interface *GetInterface() const	{	return m_pInterface;		}
			  _Interface *operator->()			{	return m_pInterface;		}
		const _Interface *operator->()	 const	{	return m_pInterface;		}
		bool  IsValid()	const					{	return 0 != m_pInterface;	}
	};

#endif
