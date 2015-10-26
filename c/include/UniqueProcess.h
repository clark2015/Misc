#ifndef UniqueProcess_H___
#define UniqueProcess_H___
//��֤����Ψһ��
//��������ϵͳ��ʵ�ֲ�̫һ�£�һ������������ϵͳΨһ��һ���ǻ����ļ���

#ifdef _WIN32

class CUniqueProcess
{
	HANDLE	m_Handle;
public:	
	CUniqueProcess()	{	m_Handle	= 0;	}
	~CUniqueProcess()	{	End();				}
	bool Start(const char* pFileName)
	{
		if ( NULL == pFileName || 0 == pFileName[0] )
			return false;
		assert(m_Handle==0);
		m_Handle = CreateMutex(NULL,true,pFileName);
		DWORD dwErrorNum = GetLastError();
		if ( dwErrorNum == ERROR_ALREADY_EXISTS )
		{
			DbgTrace("CUniqueProcess::Start(): ERROR_ALREADY_EXISTS");
			return false;
		}
		else if ( m_Handle == NULL )
		{
			DbgTrace("CUniqueProcess::Start(): m_Handle == NULL,lasterror=%u",dwErrorNum);
			return false;
		}
		else
		{
			return true;
		}
	}
	
	void End()
	{
		if ( m_Handle != 0 )
		{
			CloseHandle(m_Handle);
			m_Handle = 0;
		}
	}
};

#else
#include <fcntl.h>
//���ϳ����ģ�û���Թ�
class CUniqueProcess
{
	int m_fd;
public:	
	CUniqueProcess()	{	m_fd	= -1;	}
	~CUniqueProcess()	{	End();			}
	
	bool Start(const char* pFileName)
	{
		if ( NULL == pFileName || 0 == pFileName[0] )
			return false;
		assert(m_fd == -1);

		int ret = 0;
		m_fd = open(pFileName, O_CREAT|O_WRONLY);
	    if(m_fd == -1) {
	      RelTrace("CUniqueProcess: open fail,errno=%d,strerror=%s",errno,strerror(errno));
	      return false;
	    }

	    static struct flock lock;
	  
	    lock.l_type		= F_WRLCK;
	    lock.l_start	= 0;
	    lock.l_whence 	= SEEK_SET;
	    lock.l_len 		= 0;
	    lock.l_pid 		= getpid();
	//	ret = fcntl(m_fd, F_SETLKW, &lock);	������һֱ�ȴ�
		ret = fcntl(m_fd, F_SETLK, &lock);
	    return ret != -1;
	}
	void End()
	{
		if ( -1 != m_fd )
		{
			close(m_fd);
			m_fd = -1;
		}
	}
};
#endif

#endif
