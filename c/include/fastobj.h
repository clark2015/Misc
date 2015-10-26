#ifndef _FAST_OBJ_H_
#define _FAST_OBJ_H_

#include <list>
#include "ILogger.h"
//*******************************************************************************************
/*
优化32K以下内存的对象分配
*/

#include "fast_alloc_tmpl.h"

#ifdef _DEBUG 
	#define _FASTOBJ_ADD_DEBUG_DATA_
#endif

#ifdef NO_FAST_OBJECT

class CFastObj	
{	
};

#else

class CFastObj
	{
	#ifdef _FASTOBJ_ADD_DEBUG_DATA_
		enum {	DEBUG_MAGIC_NO = 0XCFA570B1	};	//debug信息
	#endif		
	protected:
	//	virtual const char * ClassName()	{	return "CFastObj";	};	//没用，内存溢出的话，会覆盖掉
	public:
		virtual ~CFastObj()	{}	//否则delete时传入的大小可能不对
		
		void * operator new (size_t n)
			{
	//		RelTrace("CFastObj::new(%d)",n);

			//为debug信息增加内存		
			#ifdef _FASTOBJ_ADD_DEBUG_DATA_
				{
				n += 2*sizeof(int);
				}
			#endif

			//分配内存
			void *pRet = CommonPooledAlloc::allocate(n);
			
			//写入debug信息
			#ifdef _FASTOBJ_ADD_DEBUG_DATA_
				{
				int  *pInt  = (int  *)pRet;
				pInt[0] = DEBUG_MAGIC_NO;
				
				char *pChar = (char *)pRet;
				pChar += n - sizeof(int);
				pInt  =  (int*)pChar;
				pInt[0] = DEBUG_MAGIC_NO;
				
				pRet  = ((int*)pRet) + 1;
				}
			#endif
				
			return pRet;
			}
			
		void   operator delete(void *p,size_t n)
			{
			//验证debug信息
			#ifdef _FASTOBJ_ADD_DEBUG_DATA_
				{
				bool bFlowed = false;

				int * pInt = (int *)p;
				if ( pInt[-1] != DEBUG_MAGIC_NO )
					{
					RelTrace("CFastObj::delete() : 内存下溢");	
					bFlowed = true;
				//	exit(1)	;
					}
			//	memset(p,0xFD,n);
				//覆盖掉虚表指针，如果再有调用虚函数，就会出错了
				*pInt = 0;

				char *pChar = (char *)p;
				pChar += n;
				int *pInt1 = (int *)pChar;
				if ( pInt1[0] != DEBUG_MAGIC_NO )
					{	
					RelTrace("CFastObj::delete() : 内存上溢");
					bFlowed = true;
				//	exit(1);
					}

				if ( bFlowed )
					{
					RelTrace("CFastObj::delete() : 对象大小:%d,实际分配:%d,内存头尾Magic_NO变为%d和%d",
						n,n + 2*sizeof(int),pInt[-1],pInt1[0]);
					assert(!"内存溢出");
					}
				
				n += 2*sizeof(int);
				pChar = (char *)p;
				pChar -= sizeof(int);
				p = pChar;
				}
			#endif
		
			//内存去配
			CommonPooledAlloc::deallocate(p,n);
			}
	};

#endif //#ifndef NO_FAST_OBJECT
//*******************************************************************************************
#endif
