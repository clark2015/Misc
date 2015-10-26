#ifndef PROXY_H__
#define PROXY_H__
//�������������

enum PROXY_TYPE		//��������
{
	PROXYT_NONE			= 0,
	PROXYT_SOCKS4		= 1,
	PROXYT_SOCKS5		= 2,
	PROXYT_HTTP			= 3,
	PROXYT_TELNET		= 4,	//δʵ��
};

struct STProxyInfo
{
	enum	
	{	
		VERSION_MAIN	= 1,	
		VERSION_SUB		= 0	
	};
	ushort				m_usVerMain;	//���ṹ�İ汾
	ushort				m_usVerSub;		//

	//Proxy��Ϣ
	PROXY_TYPE			m_ProxyType;
	ushort				m_usPort;			//�����������ַ���˿�
	char				m_szAddress[256];	//
	char				m_szAuthName[256];	//��֤�û���������
	char				m_szAuthPass[256];	//

	char				m_szCmd[256];		//telnet����(δʵ��)
	bool				m_bDNSInProxy;		//�Ƿ��ڴ������������IP(δʵ��)

	STProxyInfo()
	{
		memset(this,0,sizeof(*this));
		m_usVerMain = VERSION_MAIN;
		m_usVerSub  = VERSION_SUB;
	}
};

#endif

