#pragma once

#include <string.h>

/*
  解包助手类
*/
class UnpackHelper
{
public:
	//////////////////////////// constructor //////////////////////////////////////////////
	UnpackHelper(void)
	{
		this->Init();
	}

	/////////////////////////// destructor ///////////////////////////////////////////////
	~UnpackHelper(void)
	{
		if (this->m_pDataBuffer != NULL)
		{
			//delete this->m_pDataBuffer;      //delete_this->m_pDataBuffer
			this->Init();
		}
	}

	/////////////////////////// public methods ///////////////////////////////////////////////
	bool InitPackage(const void *pTheData, int length)
	{
		//this->m_pDataBuffer = new char[length];		//析构函数中释放: delete_this->m_pDataBuffer
		this->m_pDataBuffer = (char*)pTheData;

		if (this->m_pDataBuffer == NULL)
		{
			return false;
		}
		else
		{
			//memmove(this->m_pDataBuffer, pTheData, length);
			this->m_pCurrentData = this->m_pDataBuffer;
			this->m_BufferLength = length;
		}

		return true;
	}

	template <class theclass>
	bool GetData(theclass* pTheData)
	{
		if ( this->IsOverflow(sizeof(*pTheData)) || pTheData == NULL )
		{
			return false;
		}
		const theclass*p = (const theclass*)this->m_pCurrentData;
		*pTheData = *p;
		this->m_pCurrentData += sizeof(*pTheData);
		return true;
	}
	bool GetData(void* pTheData, int length)
	{
		if (this->IsOverflow(length) || pTheData == NULL || length <= 0)
		{
			return false;
		}
		else
		{
			memmove(pTheData, this->m_pCurrentData, length);
			this->m_pCurrentData += length;
		}

		return true;
	}

	void* GetPackage(void)
	{
		return this->m_pCurrentData;
	}

	int GetLength(void) const
	{
		return this->m_BufferLength - (this->m_pCurrentData - this->m_pDataBuffer);
	}


private:
	/////////////////////////// private methods ///////////////////////////////////////////////
	void Init(void)
	{
		this->m_pDataBuffer = NULL;
		this->m_pCurrentData = NULL;
		this->m_BufferLength = 0;
	}

	bool IsOverflow(int length) const
	{
		return this->m_pCurrentData - this->m_pDataBuffer + length > m_BufferLength;
	}

	/////////////////////////// private fileds ///////////////////////////////////////////////
	char *m_pDataBuffer;
	char *m_pCurrentData;
	int m_BufferLength;
};

