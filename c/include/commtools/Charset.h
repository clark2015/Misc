#define GBK_UTF8_CONV_FAST_STRLEN	512		//�������������GBK�ַ�������С��������֣��Ͳ���Ҫ�����ڴ棬���Լӿ��ٶ�
#define GBK_UTF8_CONV_ADDLEN		4
//��һ���Ż�û��������ֱ���������string������ڴ沢д��ȥ������������һ���ڴ�����copy�Ķ���
#ifdef _WIN32
inline
std::string& GBKToUTF8(const char*pGBKStr,std::string &strOutUTF8)
{
	assert(pGBKStr);
	WCHAR  BuffUnicode[GBK_UTF8_CONV_FAST_STRLEN     + 8 + GBK_UTF8_CONV_ADDLEN];
	WCHAR* pStrUnicode	= BuffUnicode;
	char   BuffUtf8	  [GBK_UTF8_CONV_FAST_STRLEN*3/2 + 8 + GBK_UTF8_CONV_ADDLEN];
	char*  pStrUtf8		= BuffUtf8;

	if ( pGBKStr[0] == 0 )
	{
		strOutUTF8 = "";
		return strOutUTF8;
	}

	//��ת��unicode�ж೤
	int n = MultiByteToWideChar(CP_ACP, 0, pGBKStr, -1, NULL, 0);
	if ( n <= 0 )
	{
		RelTrace("GBKToUTF8()����MultiByteToWideChar()ʧ��1:%u",GetLastError());
		strOutUTF8 = "";
		goto out;
	}
	if ( n > lengthof(BuffUnicode) - GBK_UTF8_CONV_ADDLEN )	//���Ǻܷ��ģ����Լ�Сһ�㣬�Ժ���ϸ����һ��
	{
		pStrUnicode = new WCHAR[n + GBK_UTF8_CONV_ADDLEN];	//
	}
	else
	{
		pStrUnicode = BuffUnicode;
	}
	//ת����unicode
	if ( MultiByteToWideChar(CP_ACP, 0, pGBKStr, -1, pStrUnicode, n) <= 0 )
	{
		RelTrace("GBKToUTF8()����MultiByteToWideChar()ʧ��2:%u",GetLastError());
		strOutUTF8 = "";
		goto out;
	}

	//��ת����utf8�ж೤
	n = WideCharToMultiByte(CP_UTF8, 0, pStrUnicode, -1, NULL, 0, NULL, NULL);
	if ( n <= 0 )
	{
		RelTrace("GBKToUTF8()����WideCharToMultiByte()ʧ��1:%u",GetLastError());
		strOutUTF8 = "";
		goto out;
	}
	else if ( n > sizeof(BuffUtf8) - GBK_UTF8_CONV_ADDLEN )	//���Ǻܷ��ģ����Լ�Сһ�㣬�Ժ���ϸ����һ��
	{
		pStrUtf8 = new char[n + GBK_UTF8_CONV_ADDLEN];
	}
	else
	{
		pStrUtf8 = BuffUtf8;
	}
	//ת����utf8
	if ( WideCharToMultiByte(CP_UTF8, 0, pStrUnicode, -1, pStrUtf8, n, NULL, NULL) <= 0 )
	{
		RelTrace("GBKToUTF8()����WideCharToMultiByte()ʧ��2:%u",GetLastError());
		strOutUTF8 = "";
		goto out;
	}

	//���÷���ֵ
	strOutUTF8 = pStrUtf8;
out:
	if ( pStrUnicode != BuffUnicode )
		delete[]pStrUnicode;
	if ( pStrUtf8 != BuffUtf8 )
		delete[]pStrUtf8;
	return strOutUTF8;
}

inline
std::string& UTF8ToGBK(const char*pUTF8Str,std::string& strGBK)
{
	assert(pUTF8Str);
	WCHAR BuffUnicode[GBK_UTF8_CONV_FAST_STRLEN*2 + 8 + GBK_UTF8_CONV_ADDLEN];
	WCHAR*pStrUnicode = BuffUnicode;
	char  BuffGBK	 [GBK_UTF8_CONV_FAST_STRLEN   + 8 + GBK_UTF8_CONV_ADDLEN];
	char *pStrGBK 	  = BuffGBK;
	
	if ( pUTF8Str[0] == 0 )
	{
		strGBK = "";
		return strGBK;
	}
	
	//��ת����unicode�ĳ���
	int len = MultiByteToWideChar(CP_UTF8, 0, pUTF8Str, -1, NULL, 0);
	if ( len <= 0 )
	{
		RelTrace("UTF8ToGBK()����MultiByteToWideChar()ʧ��1:%u",GetLastError());
		strGBK = "";
		goto out;
	}
	else if ( len > lengthof(BuffUnicode) - GBK_UTF8_CONV_ADDLEN )	//���Ǻܷ��ģ����Լ�Сһ�㣬�Ժ���ϸ����һ��
	{
		pStrUnicode = new WCHAR[len + GBK_UTF8_CONV_ADDLEN];
	}
	else
	{
	//	pStrUnicode = BuffUnicode;
	}
	//ת����Unicode
	if ( MultiByteToWideChar(CP_UTF8, 0, (LPCTSTR)pUTF8Str, -1, pStrUnicode, len) <= 0 )
	{
		RelTrace("UTF8ToGBK()����MultiByteToWideChar()ʧ��2:%u",GetLastError());
		strGBK = "";
		goto out;
	}

	//��ת����GBK�ĳ���
	len = WideCharToMultiByte(CP_ACP, 0, pStrUnicode, -1, NULL, 0, NULL, NULL);
	if ( len <= 0 )
	{
		RelTrace("UTF8ToGBK()����WideCharToMultiByte()ʧ��1:%u",GetLastError());
		strGBK = "";
		goto out;
	}
	else if ( len > sizeof(BuffGBK) - GBK_UTF8_CONV_ADDLEN )	//���Ǻܷ��ģ����Լ�Сһ�㣬�Ժ���ϸ����һ��
	{
		pStrGBK = new char[len + GBK_UTF8_CONV_ADDLEN];
	}
	else
	{
	//	pStrGBK = BuffGBK;
	}
	if ( WideCharToMultiByte(CP_ACP,0, pStrUnicode, -1, pStrGBK, len, NULL, NULL) <= 0 )
	{
		RelTrace("UTF8ToGBK()����WideCharToMultiByte()ʧ��2:%u",GetLastError());
		strGBK = "";
		goto out;
	}

	strGBK = pStrGBK;
out:
	if ( pStrGBK != BuffGBK )
		delete[]pStrGBK;
	if ( pStrUnicode != BuffUnicode )
		delete[]pStrUnicode;
	return strGBK;
}

class CGBKToUTF8
{
public:
	std::string& Conv(const char*pGBKStr,std::string &strOutUTF8)
	{
		return GBKToUTF8(pGBKStr,strOutUTF8);
	}
};
class CUTF8ToGBK
{
public:
	std::string& Conv(const char*pUTF8Str,std::string& strGBK)
	{
		return UTF8ToGBK(pUTF8Str,strGBK);
	}
};
#else

#include <iconv.h>

class CGBKUTF8ConvBase
{
protected:
	iconv_t m_handle;
public:
	CGBKUTF8ConvBase()
	{
		m_handle = (iconv_t)-1;
	}
	~CGBKUTF8ConvBase()
	{
		if ( (iconv_t)-1 != m_handle )
		{
			iconv_close(m_handle);
			m_handle = (iconv_t)-1;
		}
	}
};

class CGBKToUTF8 : public CGBKUTF8ConvBase
{
public:
	std::string& Conv(const char*pGBKStr,std::string &strOutUTF8)
	{
		strOutUTF8.clear();
		if ( NULL == pGBKStr || 0 == pGBKStr[0] )
		{
			return strOutUTF8;
		}

		if ( (iconv_t)-1 == m_handle )
		{
			m_handle = iconv_open("UTF-8//IGNORE","GBK");
			if ( (iconv_t)-1 == m_handle )
			{
				RelTrace("CGBKToUTF8::Conv() : iconv_open() fail,errno=%d",errno);
				return strOutUTF8;
			}
		}
		char* src = (char*)pGBKStr;		//iconv()����ë���ز�����constָ�룬����Ϊ��

		size_t inlen  = strlen(src) + 1;
		size_t outlen = inlen*3/2;
		char outbuff[GBK_UTF8_CONV_FAST_STRLEN*3/2 + 8];
		char*des = outbuff;
		if ( outlen > sizeof(outbuff) )
			des = new char[outlen];
		char *in  = src;
		char *out = des;

		if ( -1 == iconv(m_handle,&in,&inlen,&out,&outlen) )
		{
			RelTrace("CGBKToUTF8::Conv() fail,errno=%d,str=%s",errno,pGBKStr);
		}
		else
		{
			strOutUTF8 = des;
		}

		if ( des != outbuff )
			delete []des;
		return strOutUTF8;
	}
};

class CUTF8ToGBK : public CGBKUTF8ConvBase
{
public:
	std::string& Conv(const char*pUTF8Str,std::string& strGBK)
	{
		strGBK.clear();
		if ( NULL == pUTF8Str || 0 == pUTF8Str[0] )
		{
			return strGBK;
		}
		if ( (iconv_t)-1 == m_handle )
		{
			m_handle = iconv_open("GBK//IGNORE","UTF-8");
			if ( (iconv_t)-1 == m_handle )
			{
				RelTrace("CUTF8ToGBK::Conv() : iconv_open() fail,errno=%d",errno);
				return strGBK;
			}
		}
		char* src = (char*)pUTF8Str;		//iconv()����ë���ز�����constָ�룬����Ϊ��

		size_t inlen  = strlen(src) + 1;
		size_t outlen = inlen;
		char outbuff[GBK_UTF8_CONV_FAST_STRLEN + 8];
		char*des = outbuff;
		if ( outlen > sizeof(outbuff) )
			des = new char[outlen];
		char *in  = src;
		char *out = des;

		if ( -1 == iconv(m_handle,&in,&inlen,&out,&outlen) )
		{
			RelTrace("CUTF8ToGBK::Conv() : iconv() fail,errno=%d",errno);
		}
		else
		{
			strGBK = des;
		}

		if ( des != outbuff )
			delete []des;
		return strGBK;	
	}
};

inline
std::string& GBKToUTF8(const char*pGBKStr,std::string &strOutUTF8)
{
	CGBKToUTF8 g2u;
	return g2u.Conv(pGBKStr,strOutUTF8);
}

inline
std::string& UTF8ToGBK(const char*pUTF8Str,std::string& strGBK)
{
	CUTF8ToGBK u2g;
	return u2g.Conv(pUTF8Str,strGBK);
}

#endif
#undef GBK_UTF8_CONV_ADDLEN
#undef GBK_UTF8_CONV_FAST_STRLEN

inline
std::string GBKToUTF8(const char*pGBKStr)
{
	std::string strOutUTF8;
	return GBKToUTF8(pGBKStr,strOutUTF8);
}

inline
std::string UTF8ToGBK(const char*pUTF8Str)
{
	std::string strGBK;
	return UTF8ToGBK(pUTF8Str,strGBK);
}

inline
char*UTF8ToGBK(const char*pSrc,char*pDes,int nMaxLen)
{
	assert(pSrc&&pDes && nMaxLen > 0 );
	std::string name;

	UTF8ToGBK(pSrc,name);
	lstrcpyn(pDes,name.c_str(),nMaxLen);
	return pDes;
}

inline
bool IsGBKSpace(unsigned short usGBKChar,bool *pbNotGBKChar = NULL)
{
	unsigned char c1 = (unsigned char)(usGBKChar);
	unsigned char c2 = (unsigned char)(usGBKChar>>8);
	
	static const bool NOT_GBK_RET	= true;		//���usGBKChar����GBK����,����ʲô

	if ( c1 < (unsigned char)0x81 )	
		goto not_gbk_char;
	if ( c2 == (unsigned char)0x7F || c2 == (unsigned char)0xFF )
		goto not_gbk_char;
	if ( c2 < (unsigned char)0x40 )
		goto not_gbk_char;
	
	if ( pbNotGBKChar )
		*pbNotGBKChar = false;

	//0x81 �� 0xA0
	if ( c1 < (unsigned char)0xA1 )
	{
		return false;
	}

	//0xA1 �� 0xA9
	if ( c1 < (unsigned char)0xAA )
	{
		if ( c1 < (unsigned char)0xA8 )		//0xA7 )
		{
			if ( c2 < (unsigned char)0xA1 )
				return true;
			if ( c1 == (unsigned char)0xA1 )
			{
				if ( c2 == (unsigned char)0xA1 )	//0xA2 )
					return true;
				return false;
			}
			if ( c1 == (unsigned char)0xA2 )
			{
				if (c2 > (unsigned char)0xAA && c2 < (unsigned char)0xB1)
					return true;
				if ( c2 == (unsigned char)0xE3)
					return true;
				if ( c2 == (unsigned char)0xE4)
					return true;
				if ( c2 == (unsigned char)0xEF)
					return true;
				if ( c2 == (unsigned char)0xF0)		//0xF1)
					return true;
				if ( c2 >  (unsigned char)0xFC)
					return true;
				return false;
			}
			if ( c1 == (unsigned char)0xA3 )
			{
			//	if ( c2 == (unsigned char)0xFE)
			//		return true;
				return false;
			}
			if ( c1 == (unsigned char)0xA4 )
			{
				if ( c2 >  (unsigned char)0xF3)
					return true;
				return false;
			}
			if ( c1 == (unsigned char)0xA5 )
			{
				if ( c2 >  (unsigned char)0xF6)
					return true;
				return false;
			}
			if ( c1 == (unsigned char)0xA6 )
			{
				if (c2 > (unsigned char)0xB8 && c2 < (unsigned char)0xC1)
					return true;
				if (c2 > (unsigned char)0xD8 && c2 < (unsigned char)0xE0)
					return true;
				if ( c2 == (unsigned char)0xEC)
					return true;
				if ( c2 == (unsigned char)0xED)
					return true;
				if ( c2 == (unsigned char)0xF3)
					return true;
				if ( c2 >  (unsigned char)0xF5)
					return true;
				return false;
			}
		//	if ( c1 == (unsigned char)0xA7 )
		//	assert( c1 == (unsigned char)0xA7 )
			{
				if (c2 > (unsigned char)0xC1 && c2 < (unsigned char)0xD1)
					return true;
				if ( c2 >  (unsigned char)0xF1)
					return true;
				return false;
			}
		}

		if ( c1 == (unsigned char)0xA8 )
		{
			if ( c2 == (unsigned char)0xA0 ||
				 (c2 > (unsigned char)0xC0 && c2 < (unsigned char)0xC5) || 
				 c2 > (unsigned char)0xE9 )		//0xC9 )
				return true;
			return false;
		}

	//	if ( c1 == (unsigned char)0xA9 )
	//	assert(c1 == (unsigned char)0xA9);
		{
			if ( c2 == (unsigned char)0x58 ||
				 c2 == (unsigned char)0x5B ||
				 (c2 > (unsigned char)0x5C && c2 < (unsigned char)0x60) || 
				 (c2 > (unsigned char)0x96 && c2 < (unsigned char)0xA4) || 
				 c2 > (unsigned char)0xF4 )
				return true;
			return false;
		}
	}

	//0xAA �� 0xAF
	if ( c1 < (unsigned char)0xB0 )
	{
		if ( c2 > (unsigned char)0xA0 )
			return true;
		return false;
	}

	//0xB0 �� 0xF7
	if ( c1 < (unsigned char)0xF8 )
	{
		if ( c1 == (unsigned char)0xD7 && c2 > (unsigned char)0xF9 )	
			return true;
		return false;
	}

	//0xF8 �� 0xFE
	if ( c1 < (unsigned char)0xFF )		//������ 0xAA �� 0xAF һ��
	{
		if ( c2 > (unsigned char)0xA0 )
			return true;
		return false;
	}

	//0xFF
not_gbk_char:
	if ( pbNotGBKChar )
		*pbNotGBKChar = true;
	return 	NOT_GBK_RET;
}
