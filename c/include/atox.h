#ifndef atox_hhhh____
#define atox_hhhh____
#include <ctype.h>
//****************************************************************
/*
strtol()/strtoul()和_strtoi64()/_strtoui64()在各编译器里的行为不一样，有的会检查溢出，有的不会，这里先不管
atoi()/atol(): vc2010会检查，vc6和glibc不检查
atoll()在glibc里会检查(与man说的不一致)。在vc里是_atoi64(),vc6不检查，vc2010检查
下面提供一个一致的，不检查，抄自vc6
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
