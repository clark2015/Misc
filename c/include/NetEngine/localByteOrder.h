#ifndef _LOCAL_BYTEORDER_H_
#define _LOCAL_BYTEORDER_H_

#define	   BO_BIG_ENDIAN  0
#define	   BO_LIT_ENDIAN  1
#define	   BO_UNKNOW	  2
typedef	   int	enBYTE_ORDER;

#define	LOCAL_BYTEORDER	BO_LIT_ENDIAN

#if  (LOCAL_BYTEORDER == BO_UNKNOW)
	#error  "探测不到本地机器的字节顺序"
#endif

#endif
