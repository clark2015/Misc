#ifndef PROXY_H__
#define PROXY_H__
//代理服务器定义

enum PROXY_TYPE		//代理类型
{
	PROXYT_NONE			= 0,
	PROXYT_SOCKS4		= 1,
	PROXYT_SOCKS5		= 2,
	PROXYT_HTTP			= 3,
	PROXYT_TELNET		= 4,	//未实现
};

struct STProxyInfo
{
	enum	
	{	
		VERSION_MAIN	= 1,	
		VERSION_SUB		= 0	
	};
	ushort				m_usVerMain;	//本结构的版本
	ushort				m_usVerSub;		//

	//Proxy信息
	PROXY_TYPE			m_ProxyType;
	ushort				m_usPort;			//代理服务器地址及端口
	char				m_szAddress[256];	//
	char				m_szAuthName[256];	//验证用户名及密码
	char				m_szAuthPass[256];	//

	char				m_szCmd[256];		//telnet命令(未实现)
	bool				m_bDNSInProxy;		//是否在代理查找域名的IP(未实现)

	STProxyInfo()
	{
		memset(this,0,sizeof(*this));
		m_usVerMain = VERSION_MAIN;
		m_usVerSub  = VERSION_SUB;
	}
};

#endif

