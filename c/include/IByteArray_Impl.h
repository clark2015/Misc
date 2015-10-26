#ifndef _IBYTE_ARRAY_IMPL_H
#define _IBYTE_ARRAY_IMPL_H
#include "IByteArray.h"
#include "commtools.h"

struct ByteArray_Impl : public IByteArray
{
	virtual void		init(void *pData,uint32 ulLength,uint32 ulStorage)	{	_init(pData,ulLength,ulStorage);}
	virtual bool		checkThisPointer	()				const	{	return	_checkThisPointer	()		;	}
	virtual void		dump				()				const	{			_dump				()		;	}
	virtual const char*	data				()				const	{	return	_data				()		;	}
	virtual const char*	getName				()				const	{	return	_getName			()		;	}
	virtual void		setName				(const char*p)			{			_setName			(p)		;	}
	virtual void		setNameConst		(const char*p)			{			_setNameConst		(p)		;	}
	virtual void		setLength			(uint32 l)				{			_setLength			(l)		;	}
	virtual void		setPosition			(uint32 l)				{			_setPosition		(l)		;	}
	virtual void		clear				()						{			_clear				()		;	}
	virtual uint32		length				()				const	{	return	_length				()		;	}
	virtual uint32		position			()				const	{	return	_position			()		;	}
	virtual uint32		bytesAvailable		()				const	{	return	_bytesAvailable		()		;	}
	virtual bool		readBoolean			()						{	return	_readBoolean		()		;	}
	virtual char		readByte			()						{	return	_readByte			()		;	}
	virtual double		readDouble			()						{	return	_readDouble			()		;	}
	virtual float		readFloat			()						{	return	_readFloat			()		;	}
	virtual int			readInt				()						{	return	_readInt			()		;	}
	virtual short		readShort			()						{	return	_readShort			()		;	}
	virtual uchar		readUnsignedByte	()						{	return	_readUnsignedByte	()		;	}
	virtual uint		readUnsignedInt		()						{	return	_readUnsignedInt	()		;	}
	virtual ushort		readUnsignedShort	()						{	return	_readUnsignedShort	()		;	}
	virtual int			readStr				(char*buff,uint len)	{	return	_readStr			(buff,len);	}
	virtual longlong	readLonglong		()						{	return	_readLonglong		()		;	}
	virtual ulonglong	readUnsignedLonglong()						{	return	_readUnsignedLonglong()		;	}
	virtual void		writeBytes			(IByteArray*pArray,uint off,uint len){_writeBytes(pArray,off,len);	}
	virtual void		writeByte			(int	v)				{			_writeByte			(v)		;	}
	virtual void		writeBoolean		(bool	v)				{			_writeBoolean		(v)		;	}
	virtual void		writeDouble			(double v)				{			_writeDouble		(v)		;	}
	virtual void		writeFloat			(double	v)				{			_writeFloat			(v)		;	}
	virtual void		writeInt			(int	v)				{			_writeInt			(v)		;	}
	virtual void		writeShort			(int	v)				{			_writeShort			(v)		;	}
	virtual void		writeUnsignedInt	(uint	v)				{			_writeUnsignedInt	(v)		;	}
	virtual void		writeStr			(const char*s)			{			_writeStr			(s)		;	}
	virtual void		writeLonglong		(longlong v)			{			_writeLonglong		(v)		;	}
	virtual void		writeUnsignedLonglong(ulonglong v)			{			_writeUnsignedLonglong(v)	;	}
	virtual void		writeMemoryBlock	(const void*p,uint len)	{			_writeMemoryBlock	(p,len)	;	}
	virtual void		writeBytesRepeat	(int	v,	  uint len)	{			_writeBytesRepeat	(v,len)	;	}

protected:
	uint32		m_ulMagicNum;
	char*		m_pData;
	uint32		m_ulStorage;
	uint32		m_ulLength;
	uint32		m_ulPos;
	char		m_szName[16];
	const char* m_pName;
	enum	
	{	
		MAGIC_NUM	= 0x12345678,
		MAGIC_NUM1	= 0x87654321,
	};
public:
	bool _init(void *pData,uint32 ulLength,uint32 ulStorage)
	{
		if ( NULL == pData || ulLength > ulStorage || ulStorage == 0 )
		{
			RelTrace("IByteArray<%s>::init(%u,%u,%u)失败",m_pName,pData,ulLength,ulStorage);
			return false;
		}
		m_pData		= (char*)pData;
		m_ulLength	= ulLength;
		m_ulStorage = ulStorage;
		m_ulPos		= 0;
		return true;
	}
	~ByteArray_Impl()	{	m_ulMagicNum = MAGIC_NUM1;	}
	ByteArray_Impl()	{	m_ulMagicNum = MAGIC_NUM;	_setName(NULL);		}
	
	ByteArray_Impl(void *pData,uint32 ulLength,uint32 ulStorage,const char*pName)
	{
		m_ulMagicNum	= MAGIC_NUM;
		_setNameConst(pName);
		_init(pData,ulLength,ulStorage);
	}

	void		_setNameConst(const char*pName)	
	{	
		if ( pName )	
		{
			m_pName = pName;	
		}
		else
		{
			m_szName[0] = 0;
			m_pName = m_szName;
		}
	}
	void		_setName(const char*pName)	
	{	
		if ( pName )
			lstrcpyn(m_szName,pName ,sizeof(m_szName)); 
		else
			m_szName[0] = 0;
		m_pName = m_szName;
	}
	const char*	_getName()			const	{	return	m_pName;												}	
	bool		_checkThisPointer()	const	{	return (void*)this > (void*)0xFFFF && m_ulMagicNum == MAGIC_NUM;	}
	void		_clear()					{	m_ulLength	= 0;	m_ulPos = 0;	}
	uint32		_bytesAvailable()	const	{	return m_ulLength-m_ulPos;			}
	uint32		_length()			const	{	return m_ulLength;					}
	uint32		_position()			const	{	return m_ulPos;						}
	const char*	_data()				const	{	return m_pData;						}

	void  _setLength(uint32 l)
	{
		if ( l == m_ulLength )
			return;
		if ( l < m_ulLength )
		{
			m_ulLength = l;
			if ( m_ulPos > m_ulLength )
				m_ulPos = m_ulLength;	//as也一样
			return;
		}
		// l > m_ulLength
		uint32 ulOld = m_ulLength;
		if ( l > m_ulStorage )
		{
			RelTrace("IByteArray<%s>::setLength(%u) len > m_ulStorage(%u),将设为%u ",m_pName,l,m_ulStorage,m_ulStorage);
			m_ulLength = m_ulStorage;
		}
		else
		{
			m_ulLength = l;
		}
		//不需要设置position,as里也一样
		memset(m_pData+ulOld,0,m_ulLength-ulOld);
	}
	void  _setPosition(uint32 l)
	{
		if ( l > m_ulLength )
		{
			//as里是直接设，m_ulLength不变；如果之后写的话也修改m_ulLength,如果之后读的话就抛出错误
			RelTrace("IByteArray<%s>::setPosition(%u) > m_ulLength(%u)，将设为%u.与actionScript不同",m_pName,l,m_ulLength,m_ulLength);
			m_ulPos = m_ulLength;
		}
		else
			m_ulPos = l;
	}
	void  _dump()			const
	{
		const int CHAR_PER_LINE = 32;
		for (uint32 i=0; i< _length(); i += CHAR_PER_LINE )
		{
			char buff[CHAR_PER_LINE*2+1];
			if ( CHAR_PER_LINE + i <= _length() )
			{
				ConvBinDataToReadable(_data()+i,buff,CHAR_PER_LINE);
				buff[CHAR_PER_LINE*2] = 0;
			}
			else
			{
				ConvBinDataToReadable(_data()+i,buff,_length() - i);
				buff[(_length() - i)*2] = 0;
			}
			RelTrace("IByteArray<%s>::dump() %s",m_pName,buff);
		}
	}
	
	template<class TYPE>
	TYPE _readXXX()
	{
		if ( m_ulPos + sizeof(TYPE) > m_ulLength )
		{
			RelTrace("IByteArray<%s>::readXXX() m_ulPos(%u) + %u > m_ulLength(%u),m_ulStorage=%u",m_pName,m_ulPos, sizeof(TYPE),m_ulLength,m_ulStorage);
			return 0;
		}
		uint32 tmp = m_ulPos;
		m_ulPos += sizeof(TYPE);
		return *((TYPE*)(m_pData + tmp));
	}

	bool		_readBoolean		()	{	return _readXXX<bool>		();	}
	char		_readByte			()	{	return _readXXX<char>		();	}
	double		_readDouble			()	{	return _readXXX<double>		();	}
	float		_readFloat			()	{	return _readXXX<float>		();	}
	int			_readInt			()	{	return _readXXX<int>		();	}
	short		_readShort			()	{	return _readXXX<short>		();	}
	longlong	_readLonglong		()	{	return _readXXX<longlong>	();	}
	uchar		_readUnsignedByte	()	{	return _readXXX<uchar>		();	}
	uint		_readUnsignedInt	()	{	return _readXXX<uint>		();	}
	ushort		_readUnsignedShort	()	{	return _readXXX<ushort>		();	}
	ulonglong	_readUnsignedLonglong()	{	return _readXXX<ulonglong>	();	}
	int			_readStr(char*buff,uint len)	
	{
		if ( buff == 0 || len == 0 )
		{
			RelTrace("IByteArray<%s>::readStr(0x%X,%u)参数错误",m_pName,buff,len);
			return 0;
		}

		if ( len > _bytesAvailable() )
		{
			RelTrace("IByteArray<%s>::readStr(%u) > bytesAvailable(%u),将只读出可读部分",m_pName,len,_bytesAvailable());	//flash会抛出错误
			len = _bytesAvailable();
		}

		if ( len == 0 )
			return 0;

		for ( uint32 i=0; i<len; i++)
			buff[i] = m_pData[m_ulPos+i];	//遇到0也不管
		m_ulPos += len;
		return len;
	}

	template<class TYPE>
	void _writeXXX(TYPE v)
	{
		if ( m_ulPos + sizeof(TYPE) > m_ulStorage )
		{
			RelTrace("IByteArray<%s>::writeXXX() m_ulPos(%u) + %u > m_ulStorage(%u),m_ulLength=%u",m_pName,m_ulPos, sizeof(TYPE),m_ulStorage,m_ulLength);
			return;
		}
		TYPE*p = (TYPE*)(m_pData + m_ulPos);
		*p = v;
		m_ulPos += sizeof(TYPE);
		if ( m_ulLength < m_ulPos )
			m_ulLength = m_ulPos;
	}

	void _writeByte				(int		v)	{	_writeXXX<char>		(v);	}	//as那里是把int低 8 位将被写入字节流
	void _writeBoolean			(bool		v)	{	_writeXXX<bool>		(v);	}
	void _writeDouble			(double		v)	{	_writeXXX<double>	(v);	}
	void _writeFloat			(double		v)	{	_writeXXX<float>(float(v));	}
	void _writeInt				(int		v)	{	_writeXXX<int>		(v);	}
	void _writeUnsignedInt		(uint		v)	{	_writeXXX<uint>		(v);	}	
	void _writeShort			(int		v)	{	_writeXXX<short>	(v);	}
	void _writeLonglong			(longlong	v)	{	_writeXXX<longlong>	(v);	}
	void _writeUnsignedLonglong	(ulonglong	v)	{	_writeXXX<ulonglong>(v);	}
	void _writeStr(const char*pstr)
	{
		uint32 i = 0;
		for ( ; m_ulPos < m_ulStorage;  m_ulPos++)
		{
			m_pData[m_ulPos] = pstr[i++];
			if ( m_pData[m_ulPos] == 0 )
				break;
		}

		if ( pstr[i-1] == 0 )
		{
			m_ulPos++;
		}
		else
		{
			//写0结束？但如果一个字节都没写呢？as怎么做的？
			RelTrace("IByteArray<%s>::writeStr()不够位置，只写了部分或没有写,str=%s",m_pName,pstr);
		}

		if ( m_ulLength < m_ulPos )
			m_ulLength = m_ulPos;
	}

	void _writeBytes(IByteArray*pArray1,uint off,uint len)
	{
		ByteArray_Impl*pArray = (ByteArray_Impl*)pArray1;
		if ( !pArray->_checkThisPointer() )
		{
			RelTrace("IByteArray<%s>::writeBytes() : 指针不对",m_pName);
			return;
		}
		if ( off >= pArray->m_ulLength )
		{
			RelTrace("IByteArray<%s>::writeBytes() : off太大",m_pName);
			return;
		}
		if ( len == 0 || off + len > pArray->m_ulLength )
			len = pArray->m_ulLength - off;

		if ( len == 0 )
		{
			RelTrace("IByteArray<%s>::writeBytes() : 没有要拷的东西",m_pName);
			return;
		}

		if ( len > m_ulStorage - m_ulPos )
		{
			RelTrace("IByteArray<%s>::writeBytes() : 需要拷的数据%u > 可用的存储空间 %u，将只拷一部分",m_pName,len,m_ulStorage - m_ulPos);
			len = m_ulStorage - m_ulPos;
		}

		if ( len == 0 )
		{
			RelTrace("IByteArray<%s>::writeBytes() : 没有要拷的东西!",m_pName);
			return;
		}
		
		memcpy(m_pData + m_ulPos,pArray->m_pData + off,len);
		m_ulPos += len;

		if ( m_ulLength < m_ulPos )
			m_ulLength = m_ulPos;
	}

	void	_writeMemoryBlock(const void*s,uint len)
	{
		if ( s == 0 || len == 0 || m_ulPos + len < m_ulPos )
		{
			RelTrace("IByteArray<%s>::writeMemoryBlock(0x%X,%u)",m_pName,s,len);
			return;
		}
		if ( len > m_ulStorage - m_ulPos )
		{
			RelTrace("IByteArray<%s>::writeMemoryBlock() : 需要拷的数据%u < 可用的存储空间 %u，将只拷一部分",m_pName,len,m_ulStorage - m_ulPos);
			len = m_ulStorage - m_ulPos;
		}

		if ( len == 0 )
		{
			RelTrace("IByteArray<%s>::writeMemoryBlock() : 没有要拷的东西!",m_pName);
			return;
		}

		memcpy(m_pData + m_ulPos,s,len);
		m_ulPos += len;

		if ( m_ulLength < m_ulPos )
			m_ulLength = m_ulPos;
	}

	void	_writeBytesRepeat(int	v,	  uint len)
	{
		if ( len == 0 )
		{
		//	RelTrace("IByteArray<%s>::_writeBytesRepeat(%d,%u)",m_pName,v,len);
			return;
		}
		if ( m_ulPos + len > m_ulStorage || m_ulPos + len < m_ulPos  )
		{
			RelTrace("IByteArray<%s>::writeBytesRepeat() m_ulPos(%u) + %u > m_ulStorage(%u),m_ulLength=%u",m_pName,m_ulPos, len,m_ulStorage,m_ulLength);
			return;
		}
		memset(m_pData+m_ulPos,v,len);
		m_ulPos += len;
		if ( m_ulLength < m_ulPos )
			m_ulLength = m_ulPos;
	}
};

#endif
