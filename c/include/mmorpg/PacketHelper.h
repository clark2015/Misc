#pragma once

#include <string.h>

/*
  封包助手类
*/
class PacketHelper
{
public:
	//////////////////////////// constructor //////////////////////////////////////////////
	PacketHelper(void)
	{
		this->Init();
	}

	PacketHelper(int theData)
	{
		this->Init();
		this->AddData(theData);
	}

	PacketHelper(const void *pTheData, int length)
	{
		this->Init();
		this->AddData(pTheData, length);
	}

	/////////////////////////// destructor ///////////////////////////////////////////////
	~PacketHelper(void)
	{
	}

	/////////////////////////// public methods ///////////////////////////////////////////////
	template <class theclass>
	theclass* AddData(const theclass& data)
	{
		if (this->IsOverflow(sizeof(data)) )
		{
			return NULL;
		}
		theclass*p = (theclass*)this->m_pCurrentBuffer;
		*p = data;
		this->m_pCurrentBuffer += sizeof(data);
		return p;
	}
	void* AddData(const void *pTheData, int length)
	{
		void *ptmp = AddUninitedData(length);
		if ( ptmp )
		{
			if(pTheData == NULL)
			{
				memset(ptmp, 0, length);
			}
			else
			{
				memcpy(ptmp, pTheData, length);
			}
		}
		return ptmp;
	}
	void*AddUninitedData(int length)
	{
		if (this->IsOverflow(length) || length <= 0)
		{
			return NULL;
		}
		void *ptmp = this->m_pCurrentBuffer;
		this->m_pCurrentBuffer += length;
		return ptmp;
	}

	char* AddString(const char*pStr,bool bWithZeroEnd = true)
	{
		if ( !pStr )
			return NULL ;
		int nLen = strlen(pStr);
		if ( bWithZeroEnd )
			nLen = nLen + 1;
		if ( nLen )
			return (char*)AddData(pStr,nLen);
		return NULL;
	}

	/*const*/ void* GetPackage(void)
	{
		return this->m_DataBuffer;
	}

	int GetLength(void) const
	{
		return this->m_pCurrentBuffer - this->m_DataBuffer;
	}

	void SetLength(int len)	//丢弃一部份数据
	{
		if ( GetLength() < len )
		{
			assert(0);
			return;
		}
		this->m_pCurrentBuffer = this->m_DataBuffer + len;
	}

	void Init(void)
	{
		this->m_pCurrentBuffer = this->m_DataBuffer;
	}

private:
	/////////////////////////// private methods ///////////////////////////////////////////////
	bool IsOverflow(int length) const
	{
		return this->m_pCurrentBuffer - this->m_DataBuffer + length > BUFFER_LENGTH;
	}

	/////////////////////////// private fileds ///////////////////////////////////////////////
	static const int BUFFER_LENGTH = 4000;	//8192;
	char m_DataBuffer[BUFFER_LENGTH];
	char *m_pCurrentBuffer;
};

