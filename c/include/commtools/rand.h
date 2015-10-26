
//随机数类
class CRand
{
	unsigned long m_ulSeed; 
//	unsigned long _DoRand()	{	m_ulSeed = (m_ulSeed*1385340371) + 1745445211;	return m_ulSeed;	}
	unsigned long _DoRand()	{	m_ulSeed = (m_ulSeed*1453487209) + 1745445211;	return m_ulSeed;	}
public:
	CRand()	{	m_ulSeed = 0;		}

	void SRand(unsigned long s)		{	m_ulSeed = s;		}
	
	unsigned long  RandL()	
	{
		unsigned long ulTmp1 = _DoRand();
		unsigned long ulTmp2 = _DoRand();
		return (ulTmp1 >> 16) | (ulTmp2 & 0xFFFF0000);
	}

	unsigned short RandS()	
	{
	//	return (unsigned short)((_DoRand() >> 8) & 0xFFFF);
		return (unsigned short) (_DoRand() >> 16);
	}

	int Rand()		//与vc带的rand()一样，返回 0 - 0x7FFF 之间的一个随机值
	{
	//	return (_DoRand() >> 8) & 0x7FFF;
		return (_DoRand() >>16) & 0x7FFF;
	}

	bool RandBuff(void *pBuff,ulong ulLen)
	{
		if ( NULL == pBuff || ulLen == 0 )
		{
			assert(0);
			return false;
		}
		/*
		unsigned long *pl = (unsigned long *)pBuff;
		ulong ulC4 = ulLen >> 2;
		for (ulong i = 0; i < ulC4; i++)
		{
			*pl = RandL();
			pl++;
		}

		ulong ulC1 = ulLen & 0x3;
		if ( ulC1 != 0 )
		{
			unsigned long l = RandL();
				  unsigned char*p0 = ((unsigned char*)pBuff) + ulLen - ulC1;
			const unsigned char*p1 = (const unsigned char*)(&l);
			for (unsigned long i=0; i<ulC1; i++)
			{
				p0[i] = p1[i];
			}
		}
		*/
		unsigned short *ps = (unsigned short *)pBuff;
		unsigned long  ulS = ulLen >> 1;
		for (ulong i = 0; i < ulS; i++)
		{
			*ps = RandS();
			ps++;
		}
		if ( ulLen & 1 )
		{
			unsigned char *pc = (unsigned char *)pBuff;
			pc += ulLen-1;
			pc[0] = (unsigned char)RandS();
		}
		return true;
	}
};

#ifdef _WIN32
	#include <Wincrypt.h>
//	#ifndef HCRYPTPROV
	#ifndef PROV_RSA_FULL
		#define PROV_RSA_FULL           1
		#define CRYPT_VERIFYCONTEXT     0xF0000000
		typedef unsigned long HCRYPTPROV;
	
		typedef	WINADVAPI BOOL	(WINAPI	*_CryptReleaseContext)(HCRYPTPROV hProv,DWORD dwFlags);
		typedef	WINADVAPI BOOL	(WINAPI *_CryptAcquireContextA)(HCRYPTPROV *phProv,LPCSTR pszContainer,LPCSTR pszProvider,DWORD dwProvType,DWORD dwFlags);
		typedef	WINADVAPI BOOL	(WINAPI *_CryptGenRandom)(HCRYPTPROV hProv,DWORD dwLen,BYTE *pbBuffer);
	
	class   CTrueRand
	{  
	protected:
		static _CryptReleaseContext		CryptReleaseContext;
		static _CryptAcquireContextA	CryptAcquireContext;
		static _CryptGenRandom			CryptGenRandom;
	
		static void InitTrueRand()
		{
			if ( !CryptReleaseContext )
			{
				HMODULE	hMod = LoadLibrary("Advapi32.dll");
				CryptReleaseContext = (_CryptReleaseContext) GetProcAddress(hMod,"CryptReleaseContext");
				CryptAcquireContext = (_CryptAcquireContextA)GetProcAddress(hMod,"CryptAcquireContextA");
				CryptGenRandom		= (_CryptGenRandom)		 GetProcAddress(hMod,"CryptGenRandom");
				assert(CryptReleaseContext && CryptAcquireContext && CryptGenRandom);
				//没有FreeLibrary..
			}
		}
	#else
	class   CTrueRand
	{  
	protected:
		static void InitTrueRand(){}
	#endif
	
		HCRYPTPROV   m_hProv;
	public:  
		CTrueRand()		
		{	
			InitTrueRand();
			m_hProv = (HCRYPTPROV)NULL;	
		}
		~CTrueRand()	
		{	
			if   (m_hProv)
			{
				BOOL b = CryptReleaseContext(m_hProv,0);  
				assert(b);
			}
		}
	
		bool Init()
		{
			if ( m_hProv )
				return true;
			BOOL b = CryptAcquireContext(&m_hProv,  NULL,   NULL,  PROV_RSA_FULL,   CRYPT_VERIFYCONTEXT);  
			assert(b);
			return !!b;
		}
	
		unsigned long  RandL()	
		{
			assert(m_hProv);
			unsigned long ulRet;
			BOOL b = CryptGenRandom(m_hProv,sizeof(ulRet),  (unsigned char*)(&ulRet));
			assert(b);
			return ulRet;
		}
		unsigned short RandS()	
		{
			assert(m_hProv);
			unsigned short usRet;
			BOOL b = CryptGenRandom(m_hProv,sizeof(usRet),  (unsigned char*)(&usRet));
			assert(b);
			return usRet;
		}
		int Rand()
		{
			assert(m_hProv);
			unsigned short usRet;
			BOOL b = CryptGenRandom(m_hProv,sizeof(usRet),  (unsigned char*)(&usRet));
			assert(b);
			return usRet & 0x7FFF;
		}

		bool RandBuff(void *pBuff,ulong ulLen)
		{
			if ( !m_hProv || NULL == pBuff || ulLen == 0 )
			{
				assert(0);
				return false;
			}

			BOOL b = CryptGenRandom(m_hProv,ulLen, (unsigned char*)pBuff);
			assert(b);
			return !!b;
		}
	};  
#elif defined(__linux__)
	class   CTrueRand
	{ 
		
	
	public:
		CTrueRand()		
		{	
		}
		~CTrueRand()	
		{	
		}
	
		bool Init()
		{
			return true;
		}
			 
		unsigned long  RandL()	
		{
			unsigned long ulRet;
			RandBuff(&ulRet,sizeof(ulRet));
			return ulRet;
		}
		unsigned short RandS()	
		{
			unsigned short usRet;
			RandBuff(&usRet,sizeof(usRet));
			return usRet;
		}
		int Rand()			 
		{
			unsigned short usRet;
			RandBuff(&usRet,sizeof(usRet));
			return usRet & 0x7FFF;
		}
			 
		bool RandBuff(void *pBuff,ulong ulLen)
		{
			if ( NULL == pBuff || ulLen == 0 )
			{
				assert(0);
				return false;
			}
						
			FILE*pfile = fopen("/dev/urandom","rb");
			if (NULL== fopen)
			{
				assert(!"error open urandom file");
				return 0;
			}
			size_t sz = fread(pBuff,1,ulLen,pfile);
			if (sz != ulLen)
			{
				assert(!"error read");
				fclose(pfile);	
				return false;
			}
			fclose(pfile);
			return true;
		}
	};

#else
//	#pragma message "CTrueRand 和 CCachedTrueRand 可能要实现一下!"
	class   CTrueRand
	{
		CRand m_rand;
	public:
		bool Init()	{	m_rand.SRand(time(NULL) ^ getpid() ^ (long)this);	return true;	}
		unsigned long  RandL()	{	m_rand.RandL();	}
		unsigned short RandS()	{	m_rand.RandS();	}
		int Rand()				{	m_rand.Rand();	}
	};
#endif

template <class RandClass,int RND_BUFF_SIZE = 256>
class   tmplCachedRand //: public CTrueRand		似乎没有必要继承CTrueRand
{ 	
protected:
	unsigned char	m_RandCache[RND_BUFF_SIZE];	
	int				m_nCurrentCount;	

	RandClass		m_Rand;	
public:	
	tmplCachedRand()		
	{	
		m_nCurrentCount = 0;	
	}	

	unsigned long  RandL()		
	{	
		if ( m_nCurrentCount < sizeof(long) )	
		{	
			bool b = m_Rand.RandBuff(m_RandCache,sizeof(m_RandCache));	
			assert(b);	
			m_nCurrentCount = sizeof(m_RandCache);	
		}	

		m_nCurrentCount -= sizeof(long);	
		unsigned long *p = (unsigned long*)(m_RandCache + m_nCurrentCount);	
		return (*p);	
	}	

	unsigned short RandS()		
	{	
		if ( m_nCurrentCount < sizeof(short) )	
		{	
			bool b = m_Rand.RandBuff(m_RandCache,sizeof(m_RandCache));	
			assert(b);	
			m_nCurrentCount = sizeof(m_RandCache);	
		}	

		m_nCurrentCount -= sizeof(short);	
		unsigned short *p = (unsigned short*)(m_RandCache + m_nCurrentCount);	
		return (*p);	
	}	

	int Rand()	
	{	
		if ( m_nCurrentCount < sizeof(short) )	
		{	
			bool b = m_Rand.RandBuff(m_RandCache,sizeof(m_RandCache));	
			assert(b);	
			m_nCurrentCount = sizeof(m_RandCache);	
		}	

		m_nCurrentCount -= sizeof(short);	
		unsigned short *p = (unsigned short * )(m_RandCache + m_nCurrentCount);	
		return (*p) & 0x7FFF;	
	}	

	bool RandBuff(void *pBuff,ulong ulLen)	
	{	
		return m_Rand.RandBuff(pBuff,ulLen);	
	}
};	

class CCachedTrueRand : public tmplCachedRand<CTrueRand>
{
public:
	bool Init()	{	return m_Rand.Init(); }
};
/*
windows要在代码里加上:
#ifdef _WIN32
#ifndef HCRYPTPROV
	_CryptReleaseContext	CTrueRand::CryptReleaseContext	= NULL;
	_CryptAcquireContextA	CTrueRand::CryptAcquireContext	= NULL;
	_CryptGenRandom			CTrueRand::CryptGenRandom		= NULL;
#endif
#endif

速度测试代码如下，windows下CTrueRand比Rand慢40倍,CCachedTrueRand比CRand慢3倍
我的机器上CCachedTrueRand每秒可产生差不多470w个
	#include "../../Include/SpendTimeCheck.h"
	#define RAND_COUNT 1000000

	CRand			testRand;
	CTrueRand		testTrueRand;
	CCachedTrueRand	testCacheTrueRand;
	testRand.SRand(time(NULL));
	testTrueRand.Init();
	testCacheTrueRand.Init();
		
	double fVal;
	{
		CheckSpendTime check("Rand",1);
		fVal = 0;
		for (int i=0 ; i<RAND_COUNT; i++)
		{
			fVal += testRand.Rand();
		}
	}
	RelTrace("fVal = %f",fVal/RAND_COUNT);

	{
		CheckSpendTime check("TrueRand",1);
		fVal = 0;
		for (int i=0 ; i<RAND_COUNT; i++)
		{
			fVal += testTrueRand.Rand();
		}
	}
	RelTrace("fVal = %f",fVal/RAND_COUNT);

	{
		CheckSpendTime check("CCachedTrueRand",1);
		fVal = 0;
		for (int i=0 ; i<RAND_COUNT; i++)
		{
			fVal += testCacheTrueRand.Rand();
		}
	}
	RelTrace("fVal = %f",fVal/RAND_COUNT);
*/
