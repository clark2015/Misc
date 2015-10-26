#ifndef _COMM_TOOLS_H
#define _COMM_TOOLS_H
/*
2005-04			蓝琚成		建立文件
2006-02			       		增加linux下lstrcpyn()的支持
2006-06-14		       		增加linux下的Sleep()宏
2008-04-08		       		增加dl_inet_ntoa()
2008-04			       		增加CSpendTimeSum,修正dl_inet_ntoa()字节序相关的bug
2008-05-30		       		增加dl_mkdir()
2008-10-14		       		把ConvReadableDataToBin(),ConvBinDataToReadable()移来这里
				       		增加dl_url_decode()
				       		增加dl_url_encode()
2008-11-26/28	       		增加CTrueRand和CCachedTrueRand
2008-12-30		       		定义long64和ulong64
				       		修正dl_url_encode()里一个sign/usign比较的bug
2009-01-22		       		增加FilterFileNameIllegalChar()和IsSlash()
2009-02-06		       		修正(u)i64toa()的bug
2009-03-25		       		增加index()/rindex宏
				       		增加GetKeyPairValueStr(),GetKeyPairValueFloat(),GetKeyPairValueInt(),IsKeyPairExist(),
				       	    GetKeyPairValue(),GetKeyPairValueRawPtr()以帮助分析游戏选项
2009-04-01		       		增加GBKToUTF8()/UTF8ToGBK(),只支持windows版本
2009-04-07		       		u/long64放到DLTypedefs.h
2009-08-21		       		增加HexStr2Int()
2009-10-13		       		增加AnalyCommandLine()
2009-10-14		       		增加IsReadableBinData()
							修正TimeInvStr()的bug
2009-11-06		       		增加GetTimeStr(tm &,char *)
							GetKeyPairXXX()增加是否忽略Key大小写的选项
							增加TMLess()
							增加strncasecmp()的define
2009-11-09		       		增加TrimLeft(),TrimRight(),Trim()  
2010-01-19		       		增加IsGBKSpace()
2010-06-29		       		增加RunExeOnCurrentDir()
2010-08-11					增加IsInnerIP()
2013-12-13					增加base64_encode()/decode


功能:
	一些有助于平台移值的function:
	  lstrcpyn()		Windows的api移值到linux
	  sleep()/Sleep()	
	  (u)i64toa()
	  index()/rindex()
	  strncasecmp()
	  localtime_r()


	其它一些小工具类及function
		CRand				伪随机数类
		CTrueRand			真正的随机数类
		CCachedTrueRand		加缓存策略的真正的随机数类
		dl_inet_ntoa()		inet_ntoa()多线程安全版本
		GetTimeStr()		返回 "2002-12-32 12:14:54.123"
		GetCurrentYear()	返回年,如2006
		IntToCommaStr()		数字转为形如"1,234,567"的字符串
		AnalyCommandLine()	把命令行字符串分析成(argc,argv)的形式
		Bin2Int模板			二进制数字转10进制,注意只能处理10位
		CSpendTimeSum		统计花费时间的类	
		HexStr2Int			16进制的atoi
		TrimXXX()			删除字符串首尾空格
		CAddSpendTime	
		ConvReadableDataToBin()
		ConvBinDataToReadable()
		IsReadableBinData()
		GetKeyPairValueStr(),GetKeyPairValueFloat(),GetKeyPairValueInt(),IsKeyPairExist(),GetKeyPairValue(),GetKeyPairValueRawPtr()	分析值对字符串如"key=value1;key2=value2"的形式


	一些有用的宏定义
		CORE_DUMP_TRY		windows里core dump的宏,unix下是空的定义
		CORE_DUMP_CATCH
		lengthof			
		ASSERT
		
*/
#ifndef _WIN32
	#include <unistd.h>
	#include <signal.h>
	#include <ctype.h>
	#include <sys/stat.h>
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <sys/time.h>	//for gettimeofday
	#include <arpa/inet.h>
	#include <netdb.h>	
	#include <pthread.h>
#else
	#include <windows.h>
	#include <process.h>
	#include <Mmsystem.h>
	#include <direct.h>
	#include <ctype.h>
	#pragma comment(lib,"Winmm")
#endif	
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include <string.h>
#include <string>
#include <errno.h>
#include "ILogger.h"	//GBKToUTF8()/UTF8ToGBK()要用
#include "DLTypeDefs.h"
#include "ports.h"
//****************************************************************
//一些有用的宏定义
/*
1.在文件里增加
	CrashDump/ExceptionHandler.cpp/h
	CrashDump/GetWinVer.cpp/h
	CrashDump/MinVersion.cpp/h
2.加入如下代码
	#pragma comment (lib , "../Lib/CrashDump/dbghelp.lib")
	#include "../CrashDump/ExceptionHandler.h"
*/
#ifndef __GNUC__
	#define CORE_DUMP_TRY			__try{
	#define CORE_DUMP_CATCH(func)	}__except(RecordExceptionInfo(GetExceptionInformation(), func)){}
#else
	#define CORE_DUMP_TRY			{
	#define CORE_DUMP_CATCH(func)	}
#endif

//****************************************************************
//好象有些平台有inet_ntoa_r,但不知怎么确认有没有
inline
char* dl_inet_ntoa(uint32 ulIP,char szBuff[16])
{
//	wsprintf(szBuff,"%u.%u.%u.%u",(ulIP>>24) & 0xFF,(ulIP>>16) & 0xFF,(ulIP>>8) & 0xFF,(ulIP) & 0xFF);
	wsprintf(szBuff,"%u.%u.%u.%u",(ulIP) & 0xFF,(ulIP>>8) & 0xFF,(ulIP>>16) & 0xFF,(ulIP>>24) & 0xFF);
	return szBuff;
}

#if defined(_WINSOCK2API_) || defined(_WINSOCKAPI_) || !defined(_WIN32)
inline
char* dl_inet_ntoa(struct in_addr addr1,char szBuff[16])
{
	return dl_inet_ntoa(addr1.s_addr,szBuff);
}
#endif
//****************************************************************
inline
int HexStr2Int(const char*p)
{
	assert(p);
	int nResult = 0;
	for (int i=0; p[i]; i++)
	{
		char c = p[i];
		if ( c >= '0' && c <= '9' )
			nResult = nResult*16 + (c - '0');
		else if ( c >= 'a' && c <= 'f' )
			nResult = nResult*16 + (c - 'a') + 10;
		else if ( c >= 'A' && c <= 'F' )
			nResult = nResult*16 + (c - 'A') + 10;
		else
			break;
	}
	return nResult;
}
//****************************************************************
inline
void TrimLeft(char *p)
{
	if ( NULL == p )
	{
		assert(0);
		return;
	}
	char *p1 = p;
	for( ; *p1 == ' ' || *p1 == '\t' || *p1 == '\v' || *p1 == '\n' || *p1 == '\r'; ++p1 )
		;

	if ( p1 != p )
	{
		//不能用strcpy,因为它不保证目标与源重合的情况
		while(1)
		{
			*p = *p1;
			if ( *p == 0 )
				break;
			p++;
			p1++;
		}
	}
}

inline
void TrimRight(char *p)
{
	if ( NULL == p )
	{
		assert(0);
		return ;
	}

	char *p1 = p + strlen(p) - 1;
	for ( ; p1 >= p && (*p1 == ' ' || *p1 == '\t' || *p1 == '\v' || *p1 == '\n' || *p1 == '\r'); --p1)
		;
	*(p1 + 1)= 0;
}

inline
void Trim(char*p)
{
	TrimLeft(p);
	TrimRight(p);
}

//****************************************************************
inline 
bool IsSlash(char c)	{	return c == '\\' || c == '/';	}	//是否斜杠
//****************************************************************
#ifdef _WIN32
/*
过滤掉文件中的非法字符,注意:
 1.因处理/和多个点的问题太复杂，所以/也过滤掉，多个点变为一个
 2.开头和结尾的点也会被去掉
 3.没有处理com,con等关健字
*/
inline
void FilterFileNameIllegalChar(char szFileName[])
{
	assert(szFileName && szFileName[0]);
	int nLen = strlen(szFileName);
	int i;
	for (i=0; szFileName[i]; )
	{
		char & c = szFileName[i];
		if ( c  == ':'  ||
		   	 c  == '*'  ||
		   	 c  == '?'  ||
		   	 c  == '\"' ||
		   	 c  == '<'  ||
		   	 c  == '>'  || 
		   	 c  == '|'  ||
		   	 c  == '\t' ||
		   	 IsSlash(c) )
		{
			c = '_';
		}
		
		if ( (i == 0 && c == '.' ) ||	//去掉开头的点
			 (i != 0 && c == '.' && szFileName[i-1] == '.') )	//去掉重复的点
		{
			memmove(szFileName+i,szFileName+i+1,nLen-i);
		}
		else
			i++;
	}
	if ( i ==0 )
	{
		szFileName[0] = '_';
		szFileName[1] = 0;
	}
	else
	{
		if ( szFileName[i-1] == '.' )
		{
			if ( i == 1 )
			{
				szFileName[0] = '_';
				szFileName[1] = 0;
			}	
			else
				szFileName[i-1]  = 0;
		}
	}
}
/*
char szBuff[1024];
lstrcpyn(szBuff,".",sizeof(szBuff));
FilterFileNameIllegalChar(szBuff);
lstrcpyn(szBuff,"..",sizeof(szBuff));
FilterFileNameIllegalChar(szBuff);
lstrcpyn(szBuff,"...",sizeof(szBuff));
FilterFileNameIllegalChar(szBuff);
lstrcpyn(szBuff,"a..",sizeof(szBuff));
FilterFileNameIllegalChar(szBuff);
lstrcpyn(szBuff,"a.",sizeof(szBuff));
FilterFileNameIllegalChar(szBuff);
lstrcpyn(szBuff,"a",sizeof(szBuff));
FilterFileNameIllegalChar(szBuff);
lstrcpyn(szBuff,"/",sizeof(szBuff));
FilterFileNameIllegalChar(szBuff);
lstrcpyn(szBuff,"//",sizeof(szBuff));
FilterFileNameIllegalChar(szBuff);
lstrcpyn(szBuff,"/a/",sizeof(szBuff));
FilterFileNameIllegalChar(szBuff);
lstrcpyn(szBuff,"//a//a//",sizeof(szBuff));
FilterFileNameIllegalChar(szBuff);
*/
#else
inline
void FilterFileNameIllegalChar(char szFileName[])
{
	assert(!"未实现!");
}
#endif
//****************************************************************
inline
void ConvBinDataToReadable(const void* psrc,char des[],int nSrcLen)
{
	const char* src = (const char*)psrc;
	for (int i=0; i<nSrcLen; i++)
	{
		des[i*2    ] = ( src[i] >> 4 ) & 0xF;
		des[i*2 + 1] = ( src[i]      ) & 0xF;

		des[i*2    ] = des[i*2    ] >= 10 ? (des[i*2    ] + 'a' - 10) : (des[i*2    ] + '0');
		des[i*2 + 1] = des[i*2 + 1] >= 10 ? (des[i*2 + 1] + 'a' - 10) : (des[i*2 + 1] + '0');
	}
}

inline
void ConvReadableDataToBin(const char src[],void* pdes,int nSrcLen)
{
	char* des = (char*)pdes;
	assert(nSrcLen %2 == 0);
	for (int i=0; i<nSrcLen/2; i++)
	{
		char c0 = tolower(src[2*i  ]);
		char c1 = tolower(src[2*i+1]);

		assert( (c0 >= 'a' && c0 <='f') || (c0 >= '0' && c0 <='9') );
		assert( (c1 >= 'a' && c1 <='f') || (c1 >= '0' && c1 <='9') );

		c0 = (c0 >= 'a') ? (c0 - 'a' + 10) : (c0 - '0');
		c1 = (c1 >= 'a') ? (c1 - 'a' + 10) : (c1 - '0');

		des[i] = (c0 << 4) | c1;
	}
}

inline
bool IsReadableBinData(const char*p,int nCount)
{
	if ( nCount % 2 != 0 )
		return false;
	for (int i=0; i<nCount; i++)
	{
		char c = p[i];
		if ( c <='9' && c >='0' )
			continue;
		if ( c <='f' && c >='a' )
			continue;
		if ( c <='F' && c >='A' )
			continue;
		return false;
	}
	return true;
}

//****************************************************************
//把命令行字符串分析成(argc,argv)的形式,windows下可通过GetCommandLine()取得命令行
//没有考虑引号内的字符串有引号的情形
inline
int AnalyCommandLine(char*pCmdLine,char*argv[],int argvLen)
{
	if ( NULL == pCmdLine || NULL == argv || argvLen <= 0 )
		return 0;
	
	int x;
	for (x=0; x<argvLen; x++)
	{
		//去掉前面的空格
		while ( *pCmdLine && (*pCmdLine == ' ' || *pCmdLine == '\t') )
			pCmdLine++;
		if ( !(*pCmdLine) )
			return x;

		int i;
		if (  pCmdLine[0] == '"' )
		{
			for (i=1; pCmdLine[i] && pCmdLine[i] != '"' ; i++)
				;
			i++;
		}
		else
		{
			for (i=1; pCmdLine[i] && pCmdLine[i] != ' ' && pCmdLine[i] != '\t' ; i++)
				;
		}
		argv[x] = pCmdLine;

		if ( !pCmdLine[i] )
			return x+1;
		else
		{
			pCmdLine[i] = 0;
			pCmdLine = pCmdLine+i+1;
		}
	}
	return argvLen;
}
//****************************************************************

//统计花的时间
class CSpendTimeSum
{
protected:
	ulong	m_ulSpendTime;
	friend	class CAddSpendTime;
public:
	CSpendTimeSum()	{	m_ulSpendTime = 0;}

	ulong GetSum() const 	{	return m_ulSpendTime;	}
};

class CAddSpendTime
{
	CSpendTimeSum*  m_pSum;
	ulong			m_ulBegin;
public:
	CAddSpendTime(CSpendTimeSum & sum)	
	{	
		m_pSum = &sum;
	#ifdef _WIN32
		m_ulBegin = timeGetTime(); 
	#endif
	}
	~CAddSpendTime()	
	{	
	#ifdef _WIN32
		m_pSum->m_ulSpendTime += timeGetTime() - m_ulBegin;
	#endif
	}
};

//	#pragma comment(lib,"Winmm.lib")
//****************************************************************
//返回 "2002-12-32 12:14:54.123"
inline 
const char* GetTimeStr(char buff[24],bool bWithMS = false)
{
#ifdef _WIN32	
	SYSTEMTIME  systime;
	GetLocalTime(&systime);

	if ( bWithMS )
		wsprintf(buff,"%0004d-%02d-%02d %02d:%02d:%02d.%003d",
			systime.wYear,systime.wMonth,systime.wDay,systime.wHour,systime.wMinute,systime.wSecond,systime.wMilliseconds);
	else
		wsprintf(buff,"%0004d-%02d-%02d %02d:%02d:%02d",
			systime.wYear,systime.wMonth,systime.wDay,systime.wHour,systime.wMinute,systime.wSecond);
#else
	time_t tmp = time(NULL);
	struct tm systime;
	localtime_r(&tmp,&systime);

	if ( bWithMS )
		sprintf(buff,"%0004d-%02d-%02d %02d:%02d:%02d.%003d",
			1900 + systime.tm_year,1 + systime.tm_mon,systime.tm_mday,systime.tm_hour,systime.tm_min,systime.tm_sec,0);
	else
		sprintf(buff,"%0004d-%02d-%02d %02d:%02d:%02d",
			1900 + systime.tm_year,1 + systime.tm_mon,systime.tm_mday,systime.tm_hour,systime.tm_min,systime.tm_sec);
#endif		
	return buff;
}

inline
const char* GetTimeStr(const struct tm &tmSrc,char szDes[20])
{
	wsprintf(szDes,"%0004d-%02d-%02d %02d:%02d:%02d",
			1900 + tmSrc.tm_year,1 + tmSrc.tm_mon,tmSrc.tm_mday,tmSrc.tm_hour,tmSrc.tm_min,tmSrc.tm_sec);
	return szDes;
}

inline
const char* GetTimeStr(time_t t,char szDes[20])
{
	struct tm tmSrc;
	localtime_r(&t,&tmSrc);
	return GetTimeStr(tmSrc,szDes);
}
//****************************************************************
//返回年,如2006
inline 
int GetCurrentYear()
{
#ifdef _WIN32
	SYSTEMTIME t;
	GetLocalTime(&t);
	return t.wYear;
#else
	time_t tmp = time(NULL);
	struct tm systime;
	localtime_r(&tmp,&systime);
	return 1900 + systime.tm_year;
#endif
}
//****************************************************************
inline
bool TMLess(const struct tm& x, const struct tm& y)	//假定传入的参数是mktime()处理过的
{
	if ( x.tm_year	< y.tm_year )
		return true;
	if ( x.tm_year	> y.tm_year )
		return false;

	if ( x.tm_mon	< y.tm_mon )
		return true;
	if ( x.tm_mon	> y.tm_mon )
		return false;

	if ( x.tm_mday	< y.tm_mday )
		return true;
	if ( x.tm_mday	> y.tm_mday )
		return false;

	if ( x.tm_hour	< y.tm_hour )
		return true;
	if ( x.tm_hour	> y.tm_hour )
		return false;

	if ( x.tm_min	< y.tm_min )
		return true;
	if ( x.tm_min	> y.tm_min )
		return false;

	if ( x.tm_sec	< y.tm_sec )
		return true;
	if ( x.tm_sec	> y.tm_sec )
		return false;

	return false;
}
//****************************************************************
//数字转为形如"1,234,567"的字符串
inline
const char* IntToCommaStr(int x,char buff[15] = NULL, bool bChineseStyle = true)
	{
	static const char tab[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	static char s_buff[15];

	int nInv = bChineseStyle ? 4 : 3;

	char *pBuff;
	if ( 0 == buff )
		pBuff = s_buff;
	else
		pBuff = buff;

	pBuff[14] = 0;
	pBuff += 13;
	int y = x >=0 ? x : -x;
	for (int i =0 ; ; i++)
		{
		if ( (i+1) % (nInv+1) == 0 )
			{
			*pBuff = ',';
			pBuff --;
			continue;
			}

		*pBuff = tab[y%10];
		y /= 10;
		pBuff --;

		if ( !y )
			break;
		}

	if ( x < 0 )
		{
		*pBuff = '-';
		pBuff --;
		}

//	assert(pBuff + 1 >= buff);
	return pBuff + 1;
	}
//****************************************************************
//二进制数字转10进制,注意只能处理20位
template <ulonglong bindig>
struct Bin2Int
{
	enum
	{
		Value = (Bin2Int<bindig/10>::Value << 1) + Bin2Int<bindig%10>::Value
	};
};

template <>	struct Bin2Int<1>	{	enum	{	Value = 1	};	};
template <>	struct Bin2Int<0>	{	enum	{	Value = 0	};	};
/*
eg:
	ulong ul0  = Bin2Int<0>::Value;
	ulong ul1  = Bin2Int<1>::Value;
	ulong ulx0 = Bin2Int<10010101>::Value;
	ulong ulx1 = Bin2Int<11111>::Value;
*/
//****************************************************************
/*
#include <map>
#if defined (__SGI_STL_MAP) || defined (_STLP_MAP) || !defined(_WIN32)
	#include <hash_map>
	#define	stl_hash_map	hash_map
#else
	#define	stl_hash_map	map
#endif
*/
//****************************************************************

//返回"213503982334601天 23小时 59分 59秒 999毫秒"
inline
char*TimeInvStr(ulong64 ul64Ms,char pBuff[43] = NULL,bool bLong=false,bool bWithMs=true,bool bClockFormat = false)
{
	static char szBuff[64];
	
	if ( NULL == pBuff )
		pBuff = szBuff;

	ulong64 d = ul64Ms / (24*60*60*1000);
	ul64Ms	  %= 24*60*60*1000;

	ulong	h = (ulong)ul64Ms / (60*60*1000);
	ul64Ms	  %= 60*60*1000;

	ulong	m = (ulong)ul64Ms / (60*1000);
	ul64Ms	  %= 60*1000;

	ulong	s = (ulong)ul64Ms / 1000;
	ulong   ms=	(ulong)ul64Ms % 1000;
	
	if ( bClockFormat)
	{
		if ( bLong || d != 0 )
		{
			if ( bWithMs )
				wsprintf(pBuff,"%"FORMAT_LL_STR"u %2.2u:%2.2u:%2.2u.%3.3u",d,h,m,s,ms);
			else
				wsprintf(pBuff,"%"FORMAT_LL_STR"u %2.2u:%2.2u:%2.2u",d,h,m,s);
		}
		else
		{
			if ( h != 0 )
			{
				if ( bWithMs )
					wsprintf(pBuff,"%2.2u:%2.2u:%2.2u.%3.3u",h,m,s,ms);
				else
					wsprintf(pBuff,"%2.2u:%2.2u:%2.2u",h,m,s);
			}
			else if ( m != 0 )
			{
				if ( bWithMs )
					wsprintf(pBuff,"%2.2u:%2.2u.%3.3u",m,s,ms);
				else
					wsprintf(pBuff,"%2.2u:%2.2u",m,s);
			}
			else	//s总是显示，否则不好看
			{
				if ( bWithMs )
					wsprintf(pBuff,"%2.2u.%3.3u",s,ms);
				else
					wsprintf(pBuff,"%2.2u",s);
			}
		}
	}
	else if ( bLong || d != 0 )
	{
		if ( bWithMs )
			wsprintf(pBuff,"%"FORMAT_LL_STR"u天 %u小时 %u分 %u秒 %u毫秒",d,h,m,s,ms);
		else
			wsprintf(pBuff,"%"FORMAT_LL_STR"u天 %u小时 %u分 %u秒",d,h,m,s);
	}
	else
	{
		if ( h != 0 )
		{
			if ( bWithMs )
				wsprintf(pBuff,"%u小时 %u分 %u秒 %u毫秒",h,m,s,ms);
			else
				wsprintf(pBuff,"%u小时 %u分 %u秒",h,m,s);
		}
		else if ( m != 0 )
		{
			if ( bWithMs )
				wsprintf(pBuff,"%u分 %u秒 %u毫秒",m,s,ms);
			else
				wsprintf(pBuff,"%u分 %u秒",m,s);
		}
		else if ( s != 0 )
		{
			if ( bWithMs )
				wsprintf(pBuff,"%u秒 %u毫秒",s,ms);
			else
				wsprintf(pBuff,"%u秒",s);
		}
		else
		{
			if ( bWithMs )
				wsprintf(pBuff,"%u毫秒",ms);
			else
				wsprintf(pBuff,"0秒");
		}			
	}
	
	return pBuff;
}
//****************************************************************
template<typename THEINT>
bool IsOnlyOneBit(THEINT u)
{
	THEINT uu = 0;
	for (int i=0; i<sizeof(THEINT); i++)
	{
		THEINT tmp = u & 0xFF;
		if ( tmp )
		{
			if ( uu )
				return false;
			uu = tmp;
		}
		u >>= 8;
	}

	if (uu == 1  ||
		uu == 2  ||
		uu == 4  ||
		uu == 8  ||
		uu == 16 ||
		uu == 32 ||
		uu == 64 ||
		uu == 128)
		return true;
	return false;
}

//****************************************************************
#ifdef _WIN32
inline
bool RunExeOnCurrentDir(const char *pExeName,const char *pCmdLine,char szErrorReason[1024],int nReasonLen)
{
	if ( NULL == pExeName || NULL == pCmdLine || strlen(pExeName) > MAX_PATH || strlen(pCmdLine) > MAX_PATH || NULL == szErrorReason || nReasonLen <= 0 )
	{
		if ( szErrorReason && nReasonLen > 0)
			lstrcpyn(szErrorReason,"RunExeOnCurrentDir()字串长度过长",nReasonLen);
		return false;
	}
	szErrorReason[0] = 0;

	STARTUPINFO si;
	memset(&si,0,sizeof(si));
	si.cb=sizeof(si);
	PROCESS_INFORMATION pi;
	memset(&pi,0,sizeof(pi));

	char szModulePath[MAX_PATH];
	::GetModuleFileName( NULL,szModulePath,MAX_PATH );
	int nLen = strlen( szModulePath );
	if ( nLen >= MAX_PATH || nLen == 0 )
	{
		lstrcpyn(szErrorReason,"RunExeOnCurrentDir()取路径出错",nReasonLen);
		return false;
	}
	while( nLen>0 )
	{
		if( szModulePath[nLen] == '\\' )
		{
			szModulePath[nLen+1] = '\0';
			break;
		}
		nLen -- ;
	}

	char szCmdLine[2048];
#ifdef __DEFINE_CPP_OVERLOAD_STANDARD_NFUNC_0_2_ARGLIST_EX
	_snprintf_s(szCmdLine,1,sizeof(szCmdLine),"%s %s",pExeName,pCmdLine);	//对吧？
#else
	_snprintf(szCmdLine,sizeof(szCmdLine),"%s %s",pExeName,pCmdLine);
//	sprintf(szCmdLine,"%s %s",pExeName,pCmdLine);
#endif

	bool bRet = 0 != CreateProcess(pExeName,szCmdLine,NULL,NULL,FALSE,CREATE_DEFAULT_ERROR_MODE,NULL,szModulePath,&si,&pi);
	if ( bRet )
	{
		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);
		return true;
	}
	else
	{
		wsprintf(szErrorReason,"起动进程(%s)失败!",pExeName);
		return false;
	}
	
}
#else
//asdfasfd
#endif

#ifdef _WIN32
	#define getcwd	_getcwd
#endif

inline
bool IsInnerIP(ulong ulIP)
{	
	ulong ul0 = (ulIP   ) & 0xFF;
	ulong ul1 = (ulIP>>8) & 0xFF;
	return 	ul0 == 10 ||
		 	ul0 == 127||
		   (ul0 == 192 && ul1 == 168) ||
		   (ul0 == 172 && (ul1 >= 16 && ul1 <= 31));
}

#if defined(_WINSOCK2API_) || defined(_WINSOCKAPI_) || !defined(_WIN32)
inline
bool IsInnerIP(const char*pIP) 
{	
	if ( !pIP )
		return false;
	ulong ulIP = inet_addr(pIP);
	if ( INADDR_NONE == ulIP )
		return false;

	return 	IsInnerIP(ulIP);
}

inline
ulong dl_inet_addr(const char*pAddr)
{
	if ( NULL == pAddr || 0 == pAddr[0] )
		return INADDR_NONE;
	ulong ulIP = inet_addr(pAddr);
	if ( ulIP == INADDR_NONE )
	{
	#ifdef _WIN32
		hostent *pHostInfo = gethostbyname(pAddr);
		if ( NULL == pHostInfo || NULL == pHostInfo->h_addr )	//试过一次NULL == pHostInfo->h_addr,也许跟当时调用的不是gethostbyname_r有关
		{														//
			return INADDR_NONE;
		}
	//	ulIP=((in_addr*)pHostInfo->h_addr)->S_un.S_addr;
		ulIP=((in_addr*)pHostInfo->h_addr)->s_addr;
	#else
		struct hostent hostbuf, *hp;
		size_t hstbuflen;
		char tmphstbuf[1024];
		int res;
		int herr;
		hstbuflen = 1024;
		res = gethostbyname_r (pAddr, &hostbuf, tmphstbuf, hstbuflen,&hp, &herr);
		if ( res || hp == NULL || NULL == hp->h_addr )
			return INADDR_NONE;
		ulIP=((in_addr*)hp->h_addr)->s_addr;
	#endif
	}
	return ulIP;
}

class CachedInetAddr
{
	uint32	m_uiLastTime;
	char	m_szAddr[72];
	ulong	m_ulLastIP;
public:
	CachedInetAddr()
	{
		m_uiLastTime= 0;
		m_szAddr[0] = 0;
		m_ulLastIP	= INADDR_NONE;
	}
	ulong InetAddr(const char*pAddr)
	{
		if ( NULL == pAddr || 0 == pAddr[0] )
			return INADDR_NONE;
		assert(strlen(pAddr) < sizeof(m_szAddr));

		uint32 now = OSGetTickCount();
		if ( !strcmp(m_szAddr,pAddr) && now - m_uiLastTime < 3000 )
		{
			return m_ulLastIP;
		}
		else
		{
			m_uiLastTime = now;
			m_ulLastIP = dl_inet_addr(pAddr);
			lstrcpyn(m_szAddr,pAddr,sizeof(m_szAddr));
			return m_ulLastIP;
		}
	}
	ulong      GetAddr()	const	{	return m_ulLastIP;	}
	const char*GetAddrStr() const	{	return m_szAddr;	}
};

#endif

#include "xtoa.h"
#include "atox.h"
//****************************************************************
#include "commtools/EncodeDecode.h"
#include "commtools/rand.h"
#include "commtools/KeyPairValue.h"
#include "commtools/Charset.h"

#endif
