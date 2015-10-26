#ifndef ___ISimpleHttpClient_H___20131010
#define ___ISimpleHttpClient_H___20131010
#include "commtools.h"
#ifndef _WIN32
#include <sys/types.h>
#include <sys/uio.h>	//for writev
#include <unistd.h>
#endif
class ISimpleHttpClientSink
{
public:
	virtual bool OnHeader(const char*p)					= 0;
	virtual bool OnFileStart(unsigned long ulFileSize)	= 0;
	virtual bool OnFileContex(char*p,int nLen)			= 0;
	virtual void OnFileEnd()							= 0;

	virtual char* GetRequestHead(char buff[1024],const char*pReqUrl,const char *pHost,int nContexLen,bool bPost)	{	return NULL;	}
};

/*
�����Ӧ��Contex-Length,�����ָ�������Զ��Ͽ���������OnFileEnd()������м�Ͽ��򲻵���
�����Ӧû��Contex-Length,��������Է��Ͽ��ŵ���OnFileEnd()
��Щ��̬��ҳ���᷵��Contex-Length�������ļ������м��볤�ȿ��ƣ�����ദ�����������������
*/
class CSimpleHttpClient
{
#ifndef _WIN32			
	enum
	{
		INVALID_SOCKET	= -1
	};
	
	void closesocket(int h) {	close(h);	}
#endif

	struct  sockaddr_in m_addr;
	char	m_szHost[128];
	CachedInetAddr	m_Address;

	static unsigned long ServerAddr2Long(const char*pAddr);
	bool ReadReturnFile(int hSocket,char buff[],ISimpleHttpClientSink*pSink);
	int  ConnectServer(int nSendTimeOutSecond,int nRecvTimeOutSecond);
public:
	enum {	BUFF_SIZE	= 1024	};
	CSimpleHttpClient();
	~CSimpleHttpClient();
	bool Init(const char*pAddr,unsigned short usPort = 80);
	bool IsInited()	const	{	return m_addr.sin_addr.s_addr != 0;	}

	bool RequestFile(const char*pReqUrl,ISimpleHttpClientSink*pSink);	//GET
	bool RequestFile(const char*pReqUrl,const char*pPostData,ISimpleHttpClientSink*pSink);	//POST,Լ��pPostData�е������ѱ�url_encode�����
};

//inline unsigned long 
//CSimpleHttpClient::ServerAddr2Long(const char*pAddr)
//{
//	if ( NULL == pAddr || 0 == pAddr[0] )
//		return INADDR_NONE;
//	unsigned long ulIP = inet_addr(pAddr);
//	if ( ulIP == INADDR_NONE )
//	{
//		hostent *pHostInfo = gethostbyname(pAddr);
//		if (pHostInfo==NULL)
//		{
//			RelTrace("�޷����͵�ַ:%s",pAddr);
//			return INADDR_NONE;
//		}
//	//	ulIP=((in_addr*)pHostInfo->h_addr)->S_un.S_addr;
//		ulIP=((in_addr*)pHostInfo->h_addr)->s_addr;
//	}
//	return ulIP;
//}

inline
CSimpleHttpClient::CSimpleHttpClient()
{
	memset(&m_addr,0,sizeof(m_addr));
	memset(&m_szHost,0,sizeof(m_szHost));
#ifdef _WIN32
	WSADATA ws;
	WSAStartup(0x0202,&ws);
#endif
}

inline
CSimpleHttpClient::~CSimpleHttpClient()
{
#ifdef _WIN32
	WSACleanup();
#endif
}

inline bool 
CSimpleHttpClient::Init(const char*pAddr,unsigned short usPort)
{
	if ( NULL == pAddr || 0 == pAddr[0] || strlen(pAddr) > sizeof(m_szHost) - 10)
	{
		RelTrace("CSimpleHttpClient::Init() : ��ַ����!");
		return false;
	}
	m_addr.sin_family = AF_INET;
	m_addr.sin_port = htons(usPort);

	//static uint32 lastTime = 0;
	//static char s_szLastIP[256] = {0};
	//static ulong lastIP;

	//uint32 now = OSGetTickCount();
	//if ( !strcmp(s_szLastIP,pAddr) && 
	//	now - lastTime < 3000 )
	//{
	//	m_addr.sin_addr.s_addr = lastIP;
	//}
	//else
	//{
	//	m_addr.sin_addr.s_addr = dl_inet_addr(pAddr);	//ServerAddr2Long(pAddr); 
	//	lastTime = now;
	//	lastIP   = m_addr.sin_addr.s_addr;
	//	lstrcpyn(s_szLastIP,pAddr,sizeof(s_szLastIP));
	//}
	m_addr.sin_addr.s_addr = m_Address.InetAddr(pAddr);
	wsprintf(m_szHost,"Host: %s",pAddr);

	return INADDR_NONE != m_addr.sin_addr.s_addr;
}

inline bool 
CSimpleHttpClient::ReadReturnFile(int hSocket,char buff[],ISimpleHttpClientSink*pSink)
{
	//��������Ϣ
	bool bReadHeadFinished			= false;	//Httpͷ�Ƿ��Ѷ���
	int  nPreLeave					= 0;
	unsigned long ulContexLen		= -1;
	unsigned long ulReadedContexLen	= 0;
	time_t timeBegin = time(NULL);
	while( 1 )
	{
		if ( time(NULL) - timeBegin >= 60 )
		{
			RelTrace("CSimpleHttpClient::ReadReturnFile() Timeout��(%d��)",time(NULL) - timeBegin);
			return false;
		}
		int nCount = recv(hSocket,buff + nPreLeave,BUFF_SIZE - nPreLeave - 10,0);	//-10����Ϊ���������ܻ����׷��0,Ҳ����Խ��Ŀ�����
		if ( nCount <= 0 )
		{
			if ( bReadHeadFinished && ulContexLen == -1 )
			{
				pSink->OnFileEnd();
				return true;
			}
			RelTrace("CSimpleHttpClient::ReadReturnFile() : û��Ԥ�ڵĶϿ�!");
			return false;
		}
	//	RelTrace("%s",buff + nPreLeave);
		nCount += nPreLeave;
		nPreLeave = 0;
		
		if ( !bReadHeadFinished )
		{
			const char *pStart = buff;
			int i;
			for (i=0; i <= nCount - 2; i++)
			{
				if ( buff[i+0] == '\r' && buff[i+1] == '\n' )
				{
					buff[i] = 0;
					if ( pStart == buff+i )	//����
					{
					//	if  ( ulContexLen == -1 )
					//		RelTrace("CSimpleHttpClient::ReadReturnFile() : ����:httpͷ��û��Content-Length");	//�Լ��ڻص�������ǲ���trace����
						bReadHeadFinished = true;
						if ( !pSink->OnFileStart(ulContexLen) )
						{
							return false;
						}

						nCount = nCount - (i+2);
						if ( nCount )
						{
							memmove(buff,buff+i+2,nCount);
						}
						break;
					}
					else
					{
						const static int s_nStrLen = strlen("Content-Length:");
						if ( strncasecmp("Content-Length:",pStart,s_nStrLen) == 0 )
						{
							ulContexLen = atoi(pStart + s_nStrLen + 1);
							if ( ulContexLen == 0 )
							{
							//	RelTrace("Content-Length���ȴ���:%s",pStart);
								if ( pSink->OnFileStart(0) )
								{
									pSink->OnFileEnd();
									return true;
								}
								return false;
							}
						}
						if ( !pSink->OnHeader(pStart) )
						{
							return false;
						}
						pStart = buff + i +2;
					}
				}
			}

			if ( !bReadHeadFinished )
			{
				if ( buff[nCount-1] == '\r' || buff[nCount-1] == '\n')
				{
					nPreLeave = __min(3,nCount);
					memmove(buff,buff+nCount-nPreLeave,nPreLeave);
				}
			}
		}

		if ( !bReadHeadFinished || !nCount )
			continue;

		ulReadedContexLen += nCount;
		if ( ulReadedContexLen >= ulContexLen )
		{
			if ( !pSink->OnFileContex(buff,nCount - (ulReadedContexLen - ulContexLen)) )
				return false;
		}
		else
		{
			if ( !pSink->OnFileContex(buff,nCount) )
				return false;
		}
		if ( ulContexLen != -1 && ulReadedContexLen >= ulContexLen )
		{
			if ( ulReadedContexLen > ulContexLen )
				RelTrace("CSimpleHttpClient::ReadReturnFile() : ���������������ļ�̫��?");
			pSink->OnFileEnd();
			return true;
		}
    } 

	RelTrace("CSimpleHttpClient::ReadReturnFile() : ��Ԥ����������!");
	assert(0);
	return false;
}

inline int  
CSimpleHttpClient::ConnectServer(int nSendTimeOutSecond,int nRecvTimeOutSecond)
{
	int hSocket = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if ( hSocket == INVALID_SOCKET )
	{
		RelTrace("CSimpleHttpClient::ConnectServer() : ����socketʧ��!");
		return INVALID_SOCKET;
	}
#ifdef _WIN32
	if ( SOCKET_ERROR == connect(hSocket,(struct sockaddr *)&m_addr, sizeof(m_addr)) )
#else
	if ( -1 		  == connect(hSocket,(struct sockaddr *)&m_addr, sizeof(m_addr)) )
#endif		
	{
		RelTrace("CSimpleHttpClient::ConnectServer() : connectʧ��!");
		closesocket(hSocket);
		return INVALID_SOCKET;
	}

	{	//���÷���ʱ��,����ʱ��
	#ifdef _WIN32
		int nSendTimeout = nSendTimeOutSecond*1000;
		int nRecvTimeout = nRecvTimeOutSecond*1000;
		if ( setsockopt(hSocket,SOL_SOCKET,SO_SNDTIMEO,(char *)&nSendTimeout,sizeof(int)) ||	
			 setsockopt(hSocket,SOL_SOCKET,SO_RCVTIMEO,(char *)&nRecvTimeout,sizeof(int)) )		
	#else
	    struct timeval sendTimeOut={nSendTimeOutSecond,0};
	    struct timeval recvTimeOut={nRecvTimeOutSecond,0};
	    if ( setsockopt(hSocket,SOL_SOCKET,SO_SNDTIMEO,&sendTimeOut,sizeof(sendTimeOut)) ||
	    	 setsockopt(hSocket,SOL_SOCKET,SO_RCVTIMEO,&recvTimeOut,sizeof(recvTimeOut)) )
	#endif			 
		{
			closesocket(hSocket);
			RelTrace("CSimpleHttpClient::ConnectServer() : setsockopt()ʧ��");
			return INVALID_SOCKET;
		}
	}
	return hSocket;
}

inline bool 
CSimpleHttpClient::RequestFile(const char*pReqUrl,ISimpleHttpClientSink*pSink)
{
	int nLen = 0;
	if ( !pReqUrl || (nLen = strlen(pReqUrl)) > BUFF_SIZE/2 || nLen <= 0 || NULL == pSink )
	{
		RelTrace("CSimpleHttpClient::RequestFile : ��������!");
		return false;
	}

	int hSocket = ConnectServer(10,20);
	if ( INVALID_SOCKET == hSocket )
		return false;

	char buff[BUFF_SIZE];

	//��������
	if ( NULL == pSink->GetRequestHead(buff,pReqUrl,m_szHost,0,false) )
	//	wsprintf(buff,"GET %s HTTP/1.1\r\n%s\r\n\r\n",pReqUrl,m_szHost);
		wsprintf(buff,"GET %s HTTP/1.0\r\n%s\r\n\r\n",pReqUrl,m_szHost);	//��http1.0,�����������û��Content-Length���Ͳ�֪��������
	if ( 0 >= send(hSocket, buff,strlen(buff),0) )
	{
		closesocket(hSocket);
		RelTrace("CSimpleHttpClient::RequestFile : ��������urlʧ��!");
		return false;
	}

	bool b = ReadReturnFile(hSocket,buff,pSink);
	closesocket(hSocket);
	return b;
}

inline bool 
CSimpleHttpClient::RequestFile(const char*pReqUrl,const char*pContex,ISimpleHttpClientSink*pSink)
{
	int nLen = 0;
	if ( !pReqUrl || !pContex || pContex[0] == 0 || (nLen = strlen(pReqUrl)) > BUFF_SIZE/2 || nLen <= 0 || NULL == pSink )
	{
		RelTrace("CSimpleHttpClient::RequestFile : ��������!!");
		return false;
	}

	int hSocket = ConnectServer(10,5);
	if ( INVALID_SOCKET == hSocket )
		return false;

	char buff[BUFF_SIZE];

	//��������
	int nLen1 = strlen(pContex);
//	wsprintf(buff,"POST %s HTTP/1.1\r\n%s\r\n\r\n",pReqUrl,m_szHost);
	if ( NULL == pSink->GetRequestHead(buff,pReqUrl,m_szHost,nLen1,true) )
		wsprintf(buff,
"POST %s HTTP/1.1\r\n\
Content-Type: application/x-www-form-urlencoded\r\n\
%s\r\n\
Content-Length: %d\r\n\r\n",
	pReqUrl,m_szHost,nLen1);

/*
Accept-Language: zh-cn\r\n\
Accept: image/gif, image/x-xbitmap, image/jpeg, image/pjpeg, application/msword, * /*\r\n\
Accept-Encoding: gzip, deflate\r\n\
User-Agent: Mozilla/4.0 (compatible; MSIE 5.5; Windows NT 5.0; COM+ 1.0.2204)\r\n\
Connection: Keep-Alive\r\n\
Cache-Control: no-cache\r\n
*/
#ifdef _WIN32
	if ( 0 >= send(hSocket, buff,strlen(buff),0)||
		 0 >= send(hSocket, pContex,nLen1,0)	||
		 0 >= send(hSocket, "\r\n\r\n",4,0)		)
#else
	struct iovec iov[3];
	iov[0].iov_base	= buff;
	iov[0].iov_len	= strlen(buff);
	iov[1].iov_base	= (void*)pContex;
	iov[1].iov_len	= nLen1;
	iov[2].iov_base	= (void*)"\r\n\r\n";
	iov[2].iov_len	= 4;
	if ( 0 >= writev(hSocket,iov,lengthof(iov)) )
#endif
	{
		closesocket(hSocket);
		RelTrace("CSimpleHttpClient::RequestFile : ��������url������ʧ��!");
		return false;
	}

	bool b = ReadReturnFile(hSocket,buff,pSink);
	closesocket(hSocket);
	return b;
}

#endif
