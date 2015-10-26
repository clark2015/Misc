#ifndef _FASTOBJ_ALLOC_TMPL_H
#define _FASTOBJ_ALLOC_TMPL_H

//������sgi stl��__default_alloc_template<>���㷨,�����ͷ��ڴ�,��������е�
//Ԫ����ĿС��_MAX_ITEMS_PER_LISTʱ���ͷ��ڴ�
//������ȱ�����ڴ���CPU�����е������ûsgi�Ǹ���ô�ã��Ժ�����취

#include "pvoper.h"
#include <assert.h>
//*******************************************************************************

template <int _ALIGN4 = 256/4,int _LIST_COUNT = 32,int _MAX_ITEMS_PER_LIST = 350>	//���256*32 = 8192
class PooledAllocate
	{
	protected:
		enum{	_ALIGN = _ALIGN4*4,  _MAX_BLOCK = _ALIGN*_LIST_COUNT	};
		static void*			m_p[_LIST_COUNT];
		static int				m_nFree[_LIST_COUNT];
		static unsigned long	m_ulAllocatedMemory;	//����ʹ���е��ڴ���(��������m_p���)
		static CLock			m_Lock;	//�ĳɶ��Ҳ�����...
	protected:
		static  size_t _S_freelist_index(size_t __bytes)	{	return (((__bytes) + _ALIGN-1)/_ALIGN - 1);			}
		static  size_t _S_round_up(size_t __bytes)			{	return (((__bytes) + _ALIGN-1) & ~(_ALIGN - 1));	}
		
	public:	
		static size_t  realsize(size_t __n)
			{
			if ( __n > _MAX_BLOCK) 
				return __n;
			return _S_round_up(__n);
			}
    	static void*   allocate(size_t __n)
    		{
	    	if ( __n > _MAX_BLOCK) 
	    		return malloc(__n);
	    		
	    	size_t l = _S_freelist_index(__n);
	    	void *pRet ;
	
	    	m_Lock.Lock();
			m_ulAllocatedMemory += __n;
	    	if ( m_p[l] == 0 )
		    	{
	    		m_Lock.Unlock();
	    		pRet = malloc(_S_round_up(__n));
	    		}
	    	else
	    		{
	    		m_nFree[l] -- ;
		    	pRet = m_p[l];
		    	void **tmp = (void **)pRet;
	    		m_p[l] = tmp[0];
	    		m_Lock.Unlock();
	    		}
	    	
	    	return pRet;
		    }

    	static void    deallocate(void* __p, size_t __n)
    		{
		//	RelTrace("%d,%d\n",__n,_MAX_BLOCK);
	    	if ( __n > _MAX_BLOCK)
	    		{
	    		free(__p);
	    		return;
	    		}
	    		
	    	size_t l = _S_freelist_index(__n);
	
	    	if ( m_nFree[l] > _MAX_ITEMS_PER_LIST )			//�����⼸��û�м���,����m_nFree[l]����
				{											//��_MAX_ITEMS_PER_LISTҪ��һ��,������
	    		free(__p);									//����ʲô��Ӱ��
				m_ulAllocatedMemory -= __n;					//���m_ulAllocatedMemory��ֵҲ�п�����Щ���⣬����ֻ��trace�õĲ�Ҫ��
				}
	    	else											
	    		{										
	    		m_Lock.Lock();
				m_ulAllocatedMemory -= __n;
				m_nFree[l] ++;
			//	RelTrace("FreeList[%d] count  = %d\n",l,m_nFree[l]);
				
				void **tmp = (void **)__p;
				tmp[0] = m_p[l];
				
				m_p[l] = __p;
	    		m_Lock.Unlock();
	    		}
    		}
    		
    	static ulong trace_memory()
    		{
			ulong ulTotal = 0;
    		for (int i=0; i<_LIST_COUNT; i++)
    			{
    			if ( m_nFree[i] )
    				{
					ulTotal += m_nFree[i]*_ALIGN*(i+1);
    				RelTrace("PooledAllocate%5.5u,List%2u,%5uB/Block,count=%3u,totMem=%u ",_MAX_BLOCK,i,_ALIGN*(i+1),m_nFree[i],m_nFree[i]*_ALIGN*(i+1));
    				}
    			}
			return ulTotal;
    		}

		static ulong get_allocated_memory()
			{
			return m_ulAllocatedMemory;
			}

		//���ڱ���vc debugʱ��ʾ�ڴ�й©
		static void Clean()
			{
			m_Lock.Lock();
			for (int i=0; i<_LIST_COUNT; i++)
				{
				void *p = m_p[i];
				for (int j=0; j<m_nFree[i]; j++)
					{
					assert(p >= (void*) 1000);
					void **tmp = (void**)p;
					void *tmp1 = tmp[0];
					free(p);
					p = tmp1;
					}
				m_nFree[i]	= 0;
				m_p[i]		= 0;
				}
			m_Lock.Unlock();
			}
	};
/*
template <int _ALIGN4 ,int _LIST_COUNT ,int _MAX_ITEMS_PER_LIST >
    inline bool operator==(const PooledAllocate<_ALIGN4,_LIST_COUNT,_MAX_ITEMS_PER_LIST >&, const PooledAllocate<_ALIGN4,_LIST_COUNT,_MAX_ITEMS_PER_LIST >&)   { return true; }

template <int _ALIGN4 ,int _LIST_COUNT ,int _MAX_ITEMS_PER_LIST >
    inline bool operator!=(const PooledAllocate<_ALIGN4,_LIST_COUNT,_MAX_ITEMS_PER_LIST >&, const PooledAllocate<_ALIGN4,_LIST_COUNT,_MAX_ITEMS_PER_LIST >&)   { return false; }
*/
template <int _ALIGN4 ,int _LIST_COUNT ,int _MAX_ITEMS_PER_LIST >
	void *PooledAllocate<_ALIGN4,_LIST_COUNT,_MAX_ITEMS_PER_LIST >::m_p[_LIST_COUNT];

template <int _ALIGN4 ,int _LIST_COUNT ,int _MAX_ITEMS_PER_LIST >
	int PooledAllocate<_ALIGN4,_LIST_COUNT,_MAX_ITEMS_PER_LIST >::m_nFree[_LIST_COUNT];

template <int _ALIGN4 ,int _LIST_COUNT ,int _MAX_ITEMS_PER_LIST >
	CLock PooledAllocate<_ALIGN4,_LIST_COUNT,_MAX_ITEMS_PER_LIST >::m_Lock;

template <int _ALIGN4 ,int _LIST_COUNT ,int _MAX_ITEMS_PER_LIST >
	unsigned long PooledAllocate<_ALIGN4,_LIST_COUNT,_MAX_ITEMS_PER_LIST >::m_ulAllocatedMemory;

//*******************************************************************************
typedef PooledAllocate<8/4,   16,400>	CPooled128B;	//���8	  * 16 = 128	,�����((8   + 128 )*16/2)*400 =   435.2K
typedef PooledAllocate<256/4, 16,200>	CPooled4K;		//���256 * 16 = 4096	,�����((256 + 4096)*16/2)*200 =  6.9632M
typedef PooledAllocate<2048/4,16,100>	CPooled32K;		//���2048* 16 = 32768	,�����((2048+32768)*16/2)*100 = 27.8528M
//*******************************************************************************

struct CommonPooledAlloc
	{
	static size_t  realsize(size_t n)
		{
		if ( n <= 128 )
			return CPooled128B::realsize(n);
		else if ( n <= 4096 )
			return CPooled4K::realsize(n);
		else
			return CPooled32K::realsize(n);
		}
	static void*   allocate(size_t n)
		{
		if ( n <= 128 )
			return CPooled128B::allocate(n);
		else if ( n <= 4096 )
			return CPooled4K::allocate(n);
		else
			return CPooled32K::allocate(n);
		}

	static void    deallocate(void* p, size_t n)
		{
		if ( n <= 128 )
			CPooled128B::deallocate(p,n);
		else if ( n <= 4096 )
			CPooled4K::deallocate(p,n);
		else
			CPooled32K::deallocate(p,n);
		}

	//��malloc,freeһ���Ĳ����ӿڣ���������Ҫ�Լ���ס����
	static void* __malloc(size_t n)
		{
		if ( n == 0 )
			return NULL;
		size_t sz = n + sizeof(size_t);
		size_t *p = (size_t *)allocate(sz);
		*p = sz;
		return p+1;
		}
	static void __free(void*p)
		{
		if ( NULL == p )
			return;
		size_t *psz = (size_t *)p;
		psz--;
		deallocate(psz,*psz);
		}

	static void TraceMemory()	
		{
		unsigned long	ulInPool = 0;
		unsigned long   ulInUsed = 0;
		ulInPool += CPooled128B::trace_memory();
		ulInUsed += CPooled128B::get_allocated_memory();
		ulInPool += CPooled4K::trace_memory();
		ulInUsed += CPooled4K::get_allocated_memory();
		ulInPool += CPooled32K::trace_memory();
		ulInUsed += CPooled32K::get_allocated_memory();
		RelTrace("memory in pool = %u,memory in use = %u",ulInPool,ulInUsed);
		}
		
	static void CleanPool()
		{
		CPooled128B::Clean();
		CPooled4K::Clean();
		CPooled32K::Clean();
		}
	};
#endif
