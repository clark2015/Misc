#ifndef _FAST_OBJ_H_
#define _FAST_OBJ_H_

#include <list>
#include "ILogger.h"
//*******************************************************************************************
/*
�Ż�32K�����ڴ�Ķ������
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
		enum {	DEBUG_MAGIC_NO = 0XCFA570B1	};	//debug��Ϣ
	#endif		
	protected:
	//	virtual const char * ClassName()	{	return "CFastObj";	};	//û�ã��ڴ�����Ļ����Ḳ�ǵ�
	public:
		virtual ~CFastObj()	{}	//����deleteʱ����Ĵ�С���ܲ���
		
		void * operator new (size_t n)
			{
	//		RelTrace("CFastObj::new(%d)",n);

			//Ϊdebug��Ϣ�����ڴ�		
			#ifdef _FASTOBJ_ADD_DEBUG_DATA_
				{
				n += 2*sizeof(int);
				}
			#endif

			//�����ڴ�
			void *pRet = CommonPooledAlloc::allocate(n);
			
			//д��debug��Ϣ
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
			//��֤debug��Ϣ
			#ifdef _FASTOBJ_ADD_DEBUG_DATA_
				{
				bool bFlowed = false;

				int * pInt = (int *)p;
				if ( pInt[-1] != DEBUG_MAGIC_NO )
					{
					RelTrace("CFastObj::delete() : �ڴ�����");	
					bFlowed = true;
				//	exit(1)	;
					}
			//	memset(p,0xFD,n);
				//���ǵ����ָ�룬������е����麯�����ͻ������
				*pInt = 0;

				char *pChar = (char *)p;
				pChar += n;
				int *pInt1 = (int *)pChar;
				if ( pInt1[0] != DEBUG_MAGIC_NO )
					{	
					RelTrace("CFastObj::delete() : �ڴ�����");
					bFlowed = true;
				//	exit(1);
					}

				if ( bFlowed )
					{
					RelTrace("CFastObj::delete() : �����С:%d,ʵ�ʷ���:%d,�ڴ�ͷβMagic_NO��Ϊ%d��%d",
						n,n + 2*sizeof(int),pInt[-1],pInt1[0]);
					assert(!"�ڴ����");
					}
				
				n += 2*sizeof(int);
				pChar = (char *)p;
				pChar -= sizeof(int);
				p = pChar;
				}
			#endif
		
			//�ڴ�ȥ��
			CommonPooledAlloc::deallocate(p,n);
			}
	};

#endif //#ifndef NO_FAST_OBJECT
//*******************************************************************************************
#endif
