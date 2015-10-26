#ifndef __xtoa__H__
#define __xtoa__H__

//³­×Ôvc6
inline 
void dl_xtoa (
        unsigned long val,
        char *buf,
        unsigned radix,
        int is_neg
        )
{
        char *p;                /* pointer to traverse string */
        char *firstdig;         /* pointer to first digit */
        char temp;              /* temp char */
        unsigned digval;        /* value of digit */

        p = buf;

        if (is_neg) {
            /* negative, so output '-' and negate */
            *p++ = '-';
            val = (unsigned long)(-(long)val);
        }

        firstdig = p;           /* save pointer to first digit */

        do {
            digval = (unsigned) (val % radix);
            val /= radix;       /* get next digit */

            /* convert to ascii and store */
            if (digval > 9)
                *p++ = (char) (digval - 10 + 'a');  /* a letter */
            else
                *p++ = (char) (digval + '0');       /* a digit */
        } while (val > 0);

        /* We now have the digit of the number in the buffer, but in reverse
           order.  Thus we reverse them now. */

        *p-- = '\0';            /* terminate string; p points to last digit */

        do {
            temp = *p;
            *p = *firstdig;
            *firstdig = temp;   /* swap *p and *firstdig */
            --p;
            ++firstdig;         /* advance to next two digits */
        } while (firstdig < p); /* repeat until halfway */
}

inline
char * dl_itoa (
        int val,
        char *buf,
        int radix
        )
{
        if (radix == 10 && val < 0)
            dl_xtoa((unsigned long)val, buf, radix, 1);
        else
            dl_xtoa((unsigned long)(unsigned int)val, buf, radix, 0);
        return buf;
}

inline
char * dl_ltoa (
        long val,
        char *buf,
        int radix
        )
{
        dl_xtoa((unsigned long)val, buf, radix, (radix == 10 && val < 0));
        return buf;
}

inline
char * dl_ultoa (
        unsigned long val,
        char *buf,
        int radix
        )
{
        dl_xtoa(val, buf, radix, 0);
        return buf;
}

#if !defined(_WIN32) || _MSC_VER >= 1600

inline
void dl_x64toa (      /* stdcall is faster and smaller... Might as well use it for the helper. */
        unsigned long long val,
        char *buf,
        unsigned radix,
        int is_neg
        )
{
        char *p;                /* pointer to traverse string */
        char *firstdig;         /* pointer to first digit */
        char temp;              /* temp char */
        unsigned digval;        /* value of digit */

        p = buf;

        if ( is_neg )
        {
            *p++ = '-';         /* negative, so output '-' and negate */
            val = (unsigned long long)(-(long long)val);
        }

        firstdig = p;           /* save pointer to first digit */

        do {
            digval = (unsigned) (val % radix);
            val /= radix;       /* get next digit */

            /* convert to ascii and store */
            if (digval > 9)
                *p++ = (char) (digval - 10 + 'a');  /* a letter */
            else
                *p++ = (char) (digval + '0');       /* a digit */
        } while (val > 0);

        /* We now have the digit of the number in the buffer, but in reverse
           order.  Thus we reverse them now. */

        *p-- = '\0';            /* terminate string; p points to last digit */

        do {
            temp = *p;
            *p = *firstdig;
            *firstdig = temp;   /* swap *p and *firstdig */
            --p;
            ++firstdig;         /* advance to next two digits */
        } while (firstdig < p); /* repeat until halfway */
}

/* Actual functions just call conversion helper with neg flag set correctly,
   and return pointer to buffer. */

inline
char * dl_i64toa (
        long long val,
        char *buf,
        int radix
        )
{
        dl_x64toa((unsigned long long)val, buf, radix, (radix == 10 && val < 0));
        return buf;
}

inline
char * dl_ui64toa (
        unsigned long long val,
        char *buf,
        int radix
        )
{
        dl_x64toa(val, buf, radix, 0);
        return buf;
}

#endif

#ifdef _WIN32
#include <stdlib.h>

#define atoll		_atoi64
#define lltoa		_i64toa
#define ulltoa		_ui64toa	

#else

#define itoa		dl_itoa
#define lltoa		dl_i64toa
#define ulltoa		dl_ui64toa	

#endif

#endif
