#ifndef _IBYTE_ARRAY_
#define _IBYTE_ARRAY_
//ģ��actionscript ��� ByteArray

#include "IUnknownEx.h"

struct IByteArray : public IUnknownEx2
{
	//����C++ʹ�����ӵ�
	virtual void		init(void *pData,uint32 ulLength,uint32 ulStorage)	= 0;
	virtual bool		checkThisPointer	()				const	= 0;
	virtual void		dump				()				const	= 0;
	virtual const char*	data				()				const	= 0;
	virtual const char*	getName				()				const	= 0;
	virtual void		setName				(const char*p)			= 0;
	virtual void		setNameConst		(const char*p)			= 0;
	virtual void		setLength			(uint32 l)				= 0;
	virtual void		setPosition			(uint32 l)				= 0;

	//��actionscriptһ����
	virtual void		clear				()						= 0;
	virtual uint32		length				()				const	= 0;
	virtual uint32		position			()				const	= 0;
	virtual uint32		bytesAvailable		()				const	= 0;

	virtual bool		readBoolean			()						= 0;
	virtual char		readByte			()						= 0;
	virtual double		readDouble			()						= 0;
	virtual float		readFloat			()						= 0;
	virtual int			readInt				()						= 0;
	virtual short		readShort			()						= 0;
	virtual uchar		readUnsignedByte	()						= 0;
	virtual uint		readUnsignedInt		()						= 0;
	virtual ushort		readUnsignedShort	()						= 0;
	virtual int			readStr				(char*buff,uint len)	= 0;
	virtual longlong	readLonglong		()						= 0;	//as��û���
	virtual ulonglong	readUnsignedLonglong()						= 0;	//as��û���,֮������Ҫһ��unsigned�汾������Ϊ�����������ֱ�Ӹ�ֵ��һ����ͬ���͵�ֵ����������as��
	
	virtual void		writeBytes			(IByteArray*pArray,uint off,uint len)	= 0;
	virtual void		writeByte			(int	v)				= 0;	//��int�� 8 λ����д���ֽ���
	virtual void		writeBoolean		(bool	v)				= 0;
	virtual void		writeDouble			(double v)				= 0;
	virtual void		writeFloat			(double	v)				= 0;
	virtual void		writeInt			(int	v)				= 0;
	virtual void		writeShort			(int	v)				= 0;
	virtual void		writeUnsignedInt	(uint	v)				= 0;	//as��Ϊʲô�����,ȴû��writeUnsignedShort��
	virtual void		writeStr			(const char*s)			= 0;
	virtual void		writeLonglong		(longlong v)			= 0;	//as��û���
	virtual void		writeUnsignedLonglong(ulonglong v)			= 0;	//as��û���
	virtual void		writeMemoryBlock	(const void*p,uint len)	= 0;	//as��û���
	virtual void		writeBytesRepeat	(int	v,	  uint len)	= 0;	//as��û���
};

#endif
