#ifndef atox_hhhh____
#define atox_hhhh____
#include <ctype.h>
//****************************************************************
/*
strtol()/strtoul()��_strtoi64()/_strtoui64()�ڸ������������Ϊ��һ�����еĻ���������еĲ��ᣬ�����Ȳ���
atoi()/atol(): vc2010���飬vc6��glibc�����
atoll()��glibc�����(��man˵�Ĳ�һ��)����vc����_atoi64(),vc6����飬vc2010���
�����ṩһ��һ�µģ�����飬����vc6
*/

inline
long atol_noerror(const char *nptr)
{
        int c;              /* current char */
        long total;         /* current total */
        int sign;           /* if '-', then negative, otherwise positive */

        /* skip whitespace */
        while ( isspace((int)(unsigned char)*nptr) )
            ++nptr;

        c = (int)(unsigned char)*nptr++;
        sign = c;           /* save sign indication */
        if (c == '-' || c == '+')
            c = (int)(unsigned char)*nptr++;    /* skip sign */

        total = 0;

        while (isdigit(c)) {
            total = 10 * total + (c - '0');     /* accumulate digit */
            c = (int)(unsigned char)*nptr++;    /* get next char */
        }

        if (sign == '-')
            return -total;
        else
            return total;   /* return result, negated if necessary */
}

inline
int atoi_noerror(const char *nptr)
{
	return atol_noerror(nptr);
}

inline
longlong atoll_noerror(const char *nptr)
{
        int c;              /* current char */
        longlong total;      /* current total */
        int sign;           /* if '-', then negative, otherwise positive */

        /* skip whitespace */
        while ( isspace((int)(unsigned char)*nptr) )
            ++nptr;

        c = (int)(unsigned char)*nptr++;
        sign = c;           /* save sign indication */
        if (c == '-' || c == '+')
            c = (int)(unsigned char)*nptr++;    /* skip sign */

        total = 0;

        while (isdigit(c)) {
            total = 10 * total + (c - '0');     /* accumulate digit */
            c = (int)(unsigned char)*nptr++;    /* get next char */
        }

        if (sign == '-')
            return -total;
        else
            return total;   /* return result, negated if necessary */
}

#endif
