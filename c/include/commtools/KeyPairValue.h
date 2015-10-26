/*
分析值对,例如 
	const char*pStr = "MyStr=hello;MyInt=1234;MyFloat=7.89";
	
	assert( IsKeyPairExist(pStr,"MyInt") );
	int 	n = GetKeyPairValueInt(pStr,"MyInt");			//n  == 1234
	double 	f = GetKeyPairValueFloat(pStr,"MyFloat");		//f  == 7.89
	double 	f1= GetKeyPairValueFloat(pStr,"NotKey",3.14);	//f1 == 3.14
	char szBuff[100];
	const char*p = GetKeyPairValueStr(pStr,"MyStr",szBuff,sizeof(szBuff));	//p == szBuff == "hello"
*/

//查找成功则返回指针，否则返回NULL
inline
const char* GetKeyPairValueRawPtr(const char*pStr,const char*pKey,bool bNoCase = false,char cDelimiter1 = '=',char cDelimiter2 = ';')
{
	assert(pStr && pKey && pKey[0] && cDelimiter1 != cDelimiter2 && cDelimiter1 != 0);

	if ( pStr[0] == 0 )		//没有给出选项，是正常的
		return NULL;

	const char *p0,*p1,*p2;

	int nKeyLen = strlen(pKey);

	p0 = pStr;
	while ( *p0 == cDelimiter2 || *p0 == cDelimiter1 )
		p0 ++;
	if ( *p0 == 0 )
		return NULL;
	for	( ; ; )
	{
		p1 = strchr(p0,cDelimiter1);
		if ( !p1 )	
		{
			return NULL;
		}

		if ( !bNoCase )
		{
			if ( nKeyLen == p1 - p0 && !strncmp		(pKey,p0,nKeyLen))	//找到了
				return p1+1;
		}
		else
		{
			if ( nKeyLen == p1 - p0 && !strncasecmp	(pKey,p0,nKeyLen))	//找到了
				return p1+1;
		}

		p2 = strchr(p1+1,cDelimiter2);
		if ( !p2 )
		{
			return NULL;
		}
		
		p0 = p2+1;
	}
}

//查找成功则返回true，否则返回false,不论成功与否,szValue都会设置，失败时szValue[0]=0
inline
bool GetKeyPairValue(const char*pStr,const char*pKey,char szValue[],int nLen,bool bNoCase = false,char cDelimiter1 = '=',char cDelimiter2 = ';')
{
	assert(szValue && nLen > 0);
	const char*pValue = GetKeyPairValueRawPtr(pStr,pKey,bNoCase,cDelimiter1 ,cDelimiter2 );
	if ( !pValue )
	{
		szValue[0] = 0;
		return false;
	}

	const char*pEnd = strchr(pValue,cDelimiter2);
	if ( !pEnd )
	{
		lstrcpyn(szValue,pValue,nLen);
	}
	else
	{
		if ( pValue == pEnd )	//这是=后跟';'的情况
		{
			szValue[0] = 0;
		}
		else
		{
			if ( nLen > pEnd-pValue )
			{
				memcpy(szValue,pValue,pEnd-pValue);
				szValue[pEnd-pValue] = 0;
			}
			else
			{
				lstrcpyn(szValue,pValue,nLen);
			}
		}
	}

	return true;
}

inline
bool IsKeyPairExist(const char*pStr,const char*pKey,bool bNoCase = false,char cDelimiter1 = '=',char cDelimiter2 = ';')
{
	return NULL != GetKeyPairValueRawPtr(pStr,pKey,bNoCase,cDelimiter1,cDelimiter2);
}

inline
int GetKeyPairValueInt(const char*pStr,const char*pKey,int nDefault = 0,bool bNoCase = false,char cDelimiter1 = '=',char cDelimiter2 = ';')
{
	const char*pValue = GetKeyPairValueRawPtr(pStr,pKey,bNoCase,cDelimiter1 ,cDelimiter2 );
	if ( !pValue )
		return nDefault;
	return atoi(pValue);
}

inline
double GetKeyPairValueFloat(const char*pStr,const char*pKey,double fDefault = 0.0,bool bNoCase = false,char cDelimiter1 = '=',char cDelimiter2 = ';')
{
	const char*pValue = GetKeyPairValueRawPtr(pStr,pKey,bNoCase,cDelimiter1 ,cDelimiter2 );
	if ( !pValue )
		return fDefault;
	return atof(pValue);
}

inline
const char* GetKeyPairValueStr(const char*pStr,const char*pKey,char szValue[],int nLen,const char*pDfault = "",bool bNoCase = false,char cDelimiter1 = '=',char cDelimiter2 = ';')
{
	if ( !GetKeyPairValue(pStr,pKey,szValue,nLen,bNoCase,cDelimiter1,cDelimiter2) )
	{
		if ( pDfault )
			lstrcpyn(szValue,pDfault,nLen);
		else
			return pDfault;
	}
	return  szValue;
}
/*
		//test
		{
			const char*pOpt = "O1=123;O2=;=;=99;O4=;O5=0.33;O6=danglun";
			int n;
			double f;
			const char*p;
			char szValue[300];

			n = GetKeyPairValueInt(pOpt,"O1");
			n = GetKeyPairValueInt(pOpt,"O4");
			n = GetKeyPairValueInt(pOpt,"XYZ",99);

			f = GetKeyPairValueFloat(pOpt,"O5");
			f = GetKeyPairValueFloat(pOpt,"O4");
			f = GetKeyPairValueFloat(pOpt,"XYZ",99);

			p =	GetKeyPairValueStr(pOpt,"O6",szValue,sizeof(szValue));
			p =	GetKeyPairValueStr(pOpt,"O4",szValue,sizeof(szValue));
			p = GetKeyPairValueStr(pOpt,"XYZ",szValue,sizeof(szValue),"KKK");
		}
*/
//****************************************************************
