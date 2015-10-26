inline
const char*dl_url_decode(const char *szSrc,char *pDes)
{
	pDes[0] = 0;
	unsigned long len = strlen(szSrc);
	int i;
	const char* p = szSrc;
	for ( i=0; *p != '\0'; i++,p++)
	{
		switch ( *p )
		{
		case '+':
			pDes[i] = ' ';
			break;
		case '%':
			if ( len - ( p - pDes ) > 2 )
			{
				char c1 = toupper( *( ++p ) );
				char c2 = toupper( *( ++p ) );
				c1 = ( c1 >= 'A' ? ( c1 -'A' + 10 ) : ( c1 - '0' ) );	//可不可能是小写?可不可能大于'F'?
				c2 = ( c2 >= 'A' ? ( c2 -'A' + 10 ) : ( c2 - '0' ) );	//似乎别人的代码也没处理，以后要用时注意一下
				pDes[i] = char( ( c1 << 4 ) + c2 );
			}
			break;
		default:
			pDes[i] = *p;
			break;
		}
	}
	pDes[i] = 0;
	return pDes;
}

//注意pDes长度应该至少是szSrc的3倍+1
//2013-11-28:修改得更标准，除-_.处所有非字母数字都改成%XX的形式。如果raw=true则空格改为%20，否则转换为+
//参考http://www.oschina.net/code/explore/php-5.2.15/ext/standard/url.c
inline
const char*dl_url_encode(const char *szSrc, char *pDes,bool raw = true)
{
	int nPos = 0;
	for (const unsigned char*p = (const unsigned char*)szSrc; *p; ++p)
    {
        if( (!raw) && *p == ' ' )
		{
            pDes[nPos++] = '+';
		}
        else if((*p < '0' && *p != '-' && *p != '.') ||	
				(*p < 'A' && *p  > '9') ||
				(*p > 'Z' && *p  < 'a' && *p != '_') ||
                (*p > 'z'))
        {
            char tmp;
			pDes[nPos++] = '%';
			tmp = ((*p>>4)&0x0F) + '0';  if( tmp > '9' ) tmp += 7u;  pDes[nPos++] = tmp;
			tmp = ( *p    &0x0F) + '0';  if( tmp > '9' ) tmp += 7u;  pDes[nPos++] = tmp;
        }
        else
		{
			pDes[nPos++] = *p;
		}
    }

	pDes[nPos] = 0;

    return pDes;
}
/*
inline
const char*dl_url_encode(const char *szSrc, char *pDes)
{
	int nPos = 0;
	for (const unsigned char*p = (const unsigned char*)szSrc; *p; ++p)
    {
        if( *p == '+' )
		{
			pDes[nPos++] = '%';
			pDes[nPos++] = '2';
			pDes[nPos++] = 'B';
		}
        else if( *p == ' ' )
		{
            pDes[nPos++] = '+';
		}
        else if( *p == '%' )
		{
			pDes[nPos++] = '%';
			pDes[nPos++] = '2';
			pDes[nPos++] = '5';
		}
        else if( *p == '=' )
		{
			pDes[nPos++] = '%';
			pDes[nPos++] = '3';
			pDes[nPos++] = 'D';
		}
        else if( *p == '&' )
		{
			pDes[nPos++] = '%';
			pDes[nPos++] = '2';
			pDes[nPos++] = '6';
		}
        else if( *p<33 || *p>127 )
        {
            char tmp;
			pDes[nPos++] = '%';
			tmp = ((*p>>4)&0x0F) + '0';  if( tmp > '9' ) tmp += 7u;  pDes[nPos++] = tmp;
			tmp = ( *p    &0x0F) + '0';  if( tmp > '9' ) tmp += 7u;  pDes[nPos++] = tmp;
        }
        else
		{
			pDes[nPos++] = *p;
		}
    }

	pDes[nPos] = 0;

    return pDes;
}
*/
//****************************************************************
//取自putty的misc.c
inline
void base64_encode_atom(const unsigned char *data, int n, char *out)
{
    static const char base64_chars[] =
		"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    unsigned word;

    word = data[0] << 16;
    if (n > 1)
		word |= data[1] << 8;
    if (n > 2)
		word |= data[2];
    out[0] = base64_chars[(word >> 18) & 0x3F];
    out[1] = base64_chars[(word >> 12) & 0x3F];
    if (n > 1)
		out[2] = base64_chars[(word >> 6) & 0x3F];
    else
		out[2] = '=';
    if (n > 2)
		out[3] = base64_chars[word & 0x3F];
    else
		out[3] = '=';
}

inline
char* base64_encode(const void *data, int n, char *out,int *plen = NULL)
{
	int i = 0, j = 0;
	for (; i < n; i += 3, j += 4)
		base64_encode_atom((const unsigned char *)data+i,(n-i > 3 ? 3 : n-i), out+j);
	out[j] = 0;
	if ( plen )
		*plen = j;
	return out;
}
//****************************************************************
/*
 修改自下面的url,不动态分配内存
 http://demon.tw/programming/c-php-base64_decode.html
 */
inline
unsigned char *base64_decode(const unsigned char *str, int strict,unsigned char *result)
{
    static const char base64_pad = '=';
    static const short base64_reverse_table[256] = {
        -2, -2, -2, -2, -2, -2, -2, -2, -2, -1, -1, -2, -2, -1, -2, -2,
        -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
        -1, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, 62, -2, -2, -2, 63,
        52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -2, -2, -2, -2, -2, -2,
        -2,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
        15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -2, -2, -2, -2, -2,
        -2, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
        41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -2, -2, -2, -2, -2,
        -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
        -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
        -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
        -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
        -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
        -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
        -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
        -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2
    };
    const unsigned char *current = str;
    int length = strlen((const char*)str);
    int ch, i = 0, j = 0, k;
    
//    unsigned char *result;
    
//    result = (unsigned char *)malloc(length + 1);
    
    while ((ch = *current++) != '\0' && length-- > 0) {
        if (ch == base64_pad) {
            if (*current != '=' && (i % 4) == 1) {
            //    free(result);
                return NULL;
            }
            continue;
        }

        ch = base64_reverse_table[ch];
        if ((!strict && ch < 0) || ch == -1) { 
            continue;
        } else if (ch == -2) {
        //    free(result);
            return NULL;
        }

        switch(i % 4) {
        case 0:
            result[j] = ch << 2;
            break;
        case 1:
            result[j++] |= ch >> 4;
            result[j] = (ch & 0x0f) << 4;
            break;
        case 2:
            result[j++] |= ch >>2;
            result[j] = (ch & 0x03) << 6;
            break;
        case 3:
            result[j++] |= ch;
            break;
        }
        i++;
    }

    k = j;
    
    if (ch == base64_pad) {
        switch(i % 4) {
        case 1:
        //    free(result);
            return NULL;
        case 2:
            k++;
        case 3:
            result[k++] = 0;
        }
    }

    result[j] = '\0';
    return result;
}
/*	上面已经有了，不要自找纠结
inline
unsigned char *base64_encode(const unsigned char *str)
{
    static const char base64_table[] =
    { 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
      'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
      'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
      'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
      '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/', '\0'
    };
    static const char base64_pad = '=';
    const unsigned char *current = str;
    int length = strlen(str);
    unsigned char *p;
    unsigned char *result;

    if ((length + 2) < 0 || ((length + 2) / 3) >= (1 << (sizeof(int) * 8 - 2))) {
        return NULL;
    }

    result = (unsigned char *)malloc(((length + 2) / 3) * 4 * sizeof(char) + 1);
    p = result;

    while (length > 2) { 
        *p++ = base64_table[current[0] >> 2];
        *p++ = base64_table[((current[0] & 0x03) << 4) + (current[1] >> 4)];
        *p++ = base64_table[((current[1] & 0x0f) << 2) + (current[2] >> 6)];
        *p++ = base64_table[current[2] & 0x3f];

        current += 3;
        length -= 3; 
    }

    if (length != 0) {
        *p++ = base64_table[current[0] >> 2];
        if (length > 1) {
            *p++ = base64_table[((current[0] & 0x03) << 4) + (current[1] >> 4)];
            *p++ = base64_table[(current[1] & 0x0f) << 2];
            *p++ = base64_pad;
        } else {
            *p++ = base64_table[(current[0] & 0x03) << 4];
            *p++ = base64_pad;
            *p++ = base64_pad;
        }
    }
    *p = '\0';
    return result;
}
*/
