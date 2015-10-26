#ifndef MEMORY_H_INCLUDE_20111115__
#define MEMORY_H_INCLUDE_20111115__

template< class T >
class CStaticArray
{
public:
    // T要继承于下面这个类
    class CArrayData{
		friend class CStaticArray<T>;
    public:
        CArrayData() : m_bFree(true){
        }
        void SetFree(){
            assert(!m_bFree);
            m_bFree = true;
        }
        void SetUsed(){
            m_bFree = false;
        }
        bool IsFree() const{
            return m_bFree;
        }
		unsigned short GetMyIndex() const	{
			return m_wMyIndex;
		}
    private:
		void SetMyIndex(unsigned short wIndex){
			m_wMyIndex = wIndex;
		}
	private:
        bool m_bFree;
		unsigned short m_wMyIndex;
    };
    
public:
    CStaticArray( int nSize );
    virtual ~CStaticArray();
    
    int  GetFreeKey();
    int  GetSize() const;
    T &  operator []( int nIndex );
    
private:
    T *  m_pElements;
    int  m_nSize;
    int  m_nCursor;	
};

template< class T >
CStaticArray< T >::CStaticArray( int nSize )
{
	assert(nSize <= 65000 && nSize > 0 );
    m_pElements	= new T[ nSize ];
    m_nSize		= nSize;
    m_nCursor	= 0;
	assert(m_pElements);
	for (unsigned short i=0; i<nSize; i++)
		m_pElements[i].SetMyIndex(i);
}

template< class T >
CStaticArray< T >::~CStaticArray()
{
    if ( m_pElements )
        delete[] m_pElements;
}

template< class T >
int CStaticArray< T >::GetSize() const
{
    return m_nSize;
}

template< class T >
int CStaticArray< T >::GetFreeKey()
{
    int nFind;
    
    for ( nFind = ++m_nCursor; nFind < m_nSize; nFind++ )
    {
        if ( m_pElements[nFind].IsFree() )
        {
            m_nCursor++;
            return nFind;
        }
    }
    
    for ( nFind = 0; nFind < m_nCursor; nFind++ )
    {
        if ( m_pElements[nFind].IsFree() )
        {
            m_nCursor++;
            return nFind;
        }
    }
    
    return -1;
}

template< class T >
T & CStaticArray< T >::operator []( int nIndex )
{
    return m_pElements[ nIndex ];
}

//////////////////////////////////////////////////////////////////////////
// 静态分配内存
template< class T >
class CStaticAllocator : public CStaticArray< T >
{
public:
    CStaticAllocator( int nSize );
    virtual ~CStaticAllocator();

    T * GetFreeItem();
};

template< class T >
CStaticAllocator< T >::CStaticAllocator(int nSize)
:CStaticArray< T >(nSize)
{
}

template< class T >
CStaticAllocator< T >::~CStaticAllocator()
{
}

template< class T >
T * CStaticAllocator< T >::GetFreeItem()
{
    int nFreeKey = GetFreeKey();

    if(nFreeKey == -1) return NULL;

    CStaticArray< T >::operator [](nFreeKey).SetUsed();
    return &CStaticArray< T >::operator [](nFreeKey);
}

/////////////////////////////////////////////////////////////////////////

template< unsigned long BufferSize = 4096 >
class CIOBuffer : public CNodeList::Node
{
private:
    static CAllocator< CIOBuffer< BufferSize > >  ms_allocator;

    unsigned short  m_usIndex;
    unsigned short  m_usCmdID;
    unsigned short  m_usCmdLen;
    char            m_cBuffer[BufferSize];
public:
    static CIOBuffer*  Allocate() {
        return ms_allocator.Allocate();
    }

    void    Release() {
        ms_allocator.Release( this );
    }
public:
    void SetBuffer( unsigned short ucCmdID, const char* pBuffer, int nLen )
    {
        assert( nLen <= BufferSize );
        m_usCmdID = ucCmdID;
        m_usCmdLen = nLen;
        memcpy( m_cBuffer, pBuffer, nLen );
    }

    void SetIndex( unsigned short usIndex )
    {
        m_usIndex = usIndex;
    }

    unsigned short GetIndex() const {
        return m_usIndex;
    }

    const char* GetCmdBuffer( ) const {
        return m_cBuffer;
    }

    unsigned short GetCmdLen() const {
        return m_usCmdLen;
    }

    unsigned short GetCmdID() const {
        return m_usCmdID;
    }
};

#endif //MEMORY_H_INCLUDE_20111115__