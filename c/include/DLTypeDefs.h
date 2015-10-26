#ifndef DLTYPEDEFS_H_
#define DLTYPEDEFS_H_

#ifdef _WIN32
#include <Windows.h>
#endif
/*
2009-04-07		lanjc		建立文件
*/
//****************************************************************
typedef unsigned char	uchar;
typedef unsigned char	UCHAR;
typedef unsigned char	BYTE;
typedef unsigned short	ushort;
typedef unsigned short	USHORT;
typedef unsigned short	WORD;
typedef unsigned long	ulong;
typedef unsigned long	ULONG;
typedef unsigned long	DWORD;
typedef unsigned int	uint;
typedef unsigned int	UINT;
typedef int   			BOOL;

typedef unsigned char       uint8;
typedef unsigned short      uint16;
typedef unsigned int        uint32;

typedef char CHAR;
typedef short SHORT;
typedef long LONG;

typedef char                int8;
typedef short               int16;
typedef int                 int32;

#ifndef FALSE
#define FALSE          0
#endif

#ifndef TRUE
#define TRUE           1
#endif

#ifndef _WIN32
typedef struct tagPOINT
{
    LONG  x;
    LONG  y;
} POINT;	//, *PPOINT, NEAR *NPPOINT, FAR *LPPOINT;

typedef struct tagRECT
{
    LONG    left;
    LONG    top;
    LONG    right;
    LONG    bottom;
} RECT;	//, *PRECT, NEAR *NPRECT, FAR *LPRECT;

typedef struct tagSIZE
{
    LONG        cx;
    LONG        cy;
} SIZE;	//, *PSIZE, *LPSIZE;
#endif
//****************************************************************

#ifdef __GNUC__
	//这里无法  typedef long long	__int64,估计__int64是个编译器内部保留的关健字
	typedef 			long long	long64;
	typedef unsigned	long long	ulong64;
	typedef unsigned 	long long   uint64;	
	typedef 			long long   int64;	
#else	
	typedef 			__int64		long64;
	typedef unsigned	__int64		ulong64;
	typedef unsigned 	__int64     uint64;	
	typedef 			__int64     int64;	
#endif

typedef 			long64			longlong;
typedef 			ulong64			ulonglong;
typedef 			ulonglong		ULONGLONG;
typedef 			longlong		LONGLONG;

#ifdef __GNUC__
	#define	FORMAT_LL_STR	"ll"	//Trace("%lld",var64)
	
	#define LONGLONG_MIN    (-9223372036854775807ll - 1)		// == -9223372036854775808
	#define LONGLONG_MAX      9223372036854775807ll				// ==  9223372036854775807
	#define ULONGLONG_MAX     0xffffffffffffffffull				// == 18446744073709551615	//对不?
#else
	#define	FORMAT_LL_STR	"I64"	//Trace("%I64d",var64)
	
	#define LONGLONG_MIN    (-9223372036854775807i64 - 1)		// == -9223372036854775808
	#define LONGLONG_MAX      9223372036854775807i64			// ==  9223372036854775807	//922亿亿
	#define ULONGLONG_MAX     0xffffffffffffffffui64			// == 18446744073709551615	//strlen("18446744073709551615") == 20
#endif

#ifndef LODWORD
	#define LODWORD(l)				((DWORD)(l))
	#define HIDWORD(l)				((DWORD)(((ulonglong)(l) >> 32) & 0xFFFFFFFF))
#endif

#ifndef MAKEWORD
	#define MAKEWORD(a, b)      ((WORD)(((BYTE)(((DWORD)(a)) & 0xff)) | ((WORD)((BYTE)(((DWORD)(b)) & 0xff))) << 8))
	#define MAKELONG(a, b)      ((LONG)(((WORD)(((DWORD)(a)) & 0xffff)) | ((DWORD)((WORD)(((DWORD)(b)) & 0xffff))) << 16))
	#define LOWORD(l)           ((WORD)(((DWORD)(l)) & 0xffff))
	#define HIWORD(l)           ((WORD)((((DWORD)(l)) >> 16) & 0xffff))
	#define LOBYTE(w)           ((BYTE)(((DWORD)(w)) & 0xff))
	#define HIBYTE(w)           ((BYTE)((((DWORD)(w)) >> 8) & 0xff))
#endif

#define MAKELONGLONG(low,hi)	(ulonglong(ulong(low)) | ((ulonglong(ulong(hi))) << 32))
//****************************************************************

//没有溢出检查的atox函数
int 	 atoi_noerror(const char *nptr);
long 	 atol_noerror(const char *nptr);
longlong atoll_noerror(const char *nptr);

#endif
