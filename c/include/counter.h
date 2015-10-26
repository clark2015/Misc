/*
文件: counter.h
功能: 提供计数器的原子操作

2004-4		蓝琚成		建立文件
2004-8-31	蓝琚成		增加对windows下vc的支持

todo : 用xadd代替add,以便让函数有返回值
*/

#ifndef _COUNTER_H__
#define _COUNTER_H__
#include "pvoper.h"

//#ifdef _WIN32		//!!!!!!!1win 32不一定是在x86上!!!
#ifdef _M_IX86
#define _X_86_CPU_
#endif

#ifndef __i386__
#define _X_86_CPU_
#endif

class CCounter
	{
protected:
	#ifndef _X_86_CPU_
		CLock	m_Lock;
	#endif
	
	volatile int m_nCount;	//一定要加volatile ! 不懂为什么
private:

#ifdef _X_86_CPU_
  #ifdef __GNUC__
	static inline void X86Inc(volatile int *pLC)
		{
		__asm__ volatile ( "lock incl (%%eax)" :  : "a"(pLC) );
		}
	
	static inline void X86Dec(volatile int *pLC)
		{
		__asm__ volatile ( "lock decl (%%eax)" :  : "a"(pLC) );
		}

	static inline void X86Add(volatile int *pLC,int l)
		{
		__asm__ volatile ( "lock addl %%ebx,(%%eax)" : : "b"(l),"a"(pLC) );
		}
	
	static inline void X86Sub(volatile int *pLC,int l)
		{
		__asm__ volatile ( "lock subl %%ebx,(%%eax)" : : "b"(l),"a"(pLC) );
		}
  #else	//__GNUC__
	static inline void X86Inc(volatile int *pLC)
		{
		__asm 
			{	
			mov eax,[pLC]
			lock inc DWORD ptr [eax]
			}
		}
	
	static inline void X86Dec(volatile int *pLC)
		{
		__asm 
			{	
			mov eax,[pLC]
			lock dec DWORD ptr [eax]	
			}
		}

	static inline void X86Add(volatile int *pLC,int l)
		{
		__asm 
			{	
			mov eax,[pLC]
			mov edx,l
			lock add DWORD ptr [eax],edx
			}
		}
	
	static inline void X86Sub(volatile int *pLC,int l)
		{
		__asm 
			{	
			mov eax,[pLC]
			mov edx,l
			lock sub DWORD ptr [eax],edx
			}
		}  
  #endif
#endif
public:
	CCounter()	{	m_nCount = 0;	}
	
	inline int  Get()	const	{	return m_nCount;	}
	inline void Set(int l)		{	m_nCount = l;		}
	inline void Inc()
		{
		#ifdef _X_86_CPU_
			X86Inc(&m_nCount);
		#else			
			m_Lock.Lock();
			m_nCount++;
			m_Lock.Unlock();
		#endif
		}
	inline void Dec()
		{
		#ifdef _X_86_CPU_
			X86Dec(&m_nCount);
		#else			
			m_Lock.Lock();
			m_nCount--;
			m_Lock.Unlock();
		#endif
		}
	
	inline void Add(int l)
		{
		#ifdef _X_86_CPU_
			X86Add(&m_nCount,l);
		#else			
			m_Lock.Lock();
			m_nCount += l;
			m_Lock.Unlock();
		#endif
		}
	
	inline void Sub(int l)
		{
		#ifdef _X_86_CPU_
			X86Sub(&m_nCount,l);
		#else			
			m_Lock.Lock();
			m_nCount -= l;
			m_Lock.Unlock();
		#endif
		}
		
	inline void operator += (int l)	{	Add(l);	}
	inline void operator -= (int l)	{	Sub(l);	}
	inline void operator ++ (int)	{	Inc();	}
	inline void operator ++ ()		{	Inc();	}
	inline void operator -- (int)	{	Dec();	}
	inline void operator -- ()		{	Dec();	}
	
	inline void operator =  (int l)	{	Set(l);			}
	inline operator int() 			{	return Get();	}
	};

#endif
