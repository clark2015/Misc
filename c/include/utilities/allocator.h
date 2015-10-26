#ifndef MEM_LIST_H_INCLUDE_20111115__
#define MEM_LIST_H_INCLUDE_20111115__

#include <assert.h>
#include <string>
#include "./node_list.h"

///////////////////////////////////////////////////////////////////////////////
// _Allocator
///////////////////////////////////////////////////////////////////////////////

template < class T, unsigned int MaxFreeCount>
class _Allocator
{
public:
    class Object : public NodeList::Node, public T
    {
    public:
        Object(){}

        void Reset( ) {
            memset(static_cast<T*>(this), 0, sizeof(T));
        }
    };
    
    //template < typename T, unsigned int MaxFreeCount> friend class Allocator;
    _Allocator(){}
public :
    static T *Allocate();
    
    static void Free(T *pT);

    //第一次分配调用构造函数，以后不调用
    static T *New();
    
    static void Delete(T *pT);
    
    static void Flush();

    static size_t ActiveCount();
    
private :

    typedef TNodeList<Object> TList;
    
    static TList m_freeList;
    static TList m_activeList;
    
    // No copies do not implement
    _Allocator(const _Allocator &rhs);
    _Allocator &operator=(const _Allocator &rhs);
};

template < class T, unsigned int MaxFreeCount>
TNodeList<typename _Allocator<T, MaxFreeCount>::Object> _Allocator<T, MaxFreeCount>::m_freeList;
template < class T, unsigned int MaxFreeCount>
TNodeList<typename _Allocator<T, MaxFreeCount>::Object> _Allocator<T, MaxFreeCount>::m_activeList;
///////////////////////////////////////////////////////////////////////////////
// _Allocator
///////////////////////////////////////////////////////////////////////////////

template < class T, unsigned int MaxFreeCount >
T *_Allocator<T, MaxFreeCount>::Allocate()
{
    Object *pObject = NULL;

    if (!m_freeList.Empty())
    {
        pObject = m_freeList.PopHead();
    }
    else
    {
        pObject = static_cast<Object*>(operator new(sizeof(Object)));
        
        if (!pObject)
        {
            throw std::string("Out of memory!");
        }
    }

    //call node's constructor
    NodeList::Node * pNode = pObject;
    new (pNode) NodeList::Node();

    m_activeList.PushNode(pObject);

    pObject->Reset();

    return pObject;
}

template < class T, unsigned int MaxFreeCount >
void _Allocator<T, MaxFreeCount>::Free(T *pT)
{
    if (!pT)
    {
        return;
    }

    // unlink from the in use list
    Object* pObject = static_cast<Object*>(pT);

    m_activeList.RemoveNode(pObject);
    
    //call node's destructor
    NodeList::Node * pNode = pObject;
    pNode->~Node();

    if (MaxFreeCount == 0 || 
         m_freeList.Count() < MaxFreeCount)
    {
        // add to the free list
        m_freeList.PushNode(pObject);
    }
    else
    {
        delete reinterpret_cast<char*>(pObject);
    }
}

template < class T, unsigned int MaxFreeCount >
T *_Allocator<T, MaxFreeCount>::New()
{
    Object *pObject = NULL;

    if (!m_freeList.Empty())
    {
        pObject = m_freeList.PopHead();
    }
    else
    {
        pObject = new Object;
        
        if (!pObject)
        {
            throw std::string("Out of memory!");
        }
    }

    m_activeList.PushNode(pObject);
    
    pObject->Reset( );

    return pObject;
}

template < class T, unsigned int MaxFreeCount >
void _Allocator<T, MaxFreeCount>::Flush()
{
    while (!m_activeList.Empty())
    {
        //内存泄漏
        assert(0 && "内存泄漏");
        Object* pObject = m_activeList.PopHead();

        NodeList::Node * pNode = pObject;
        pNode->~Node();

        delete reinterpret_cast<char*>(pObject);
    }

    while (!m_freeList.Empty())
    {
        Object* pObject = m_freeList.PopHead();

        NodeList::Node * pNode = pObject;
        pNode->~Node();

        delete reinterpret_cast<char*>(pObject);
    }
}

template < class T, unsigned int MaxFreeCount >
size_t _Allocator<T, MaxFreeCount>::ActiveCount()
{
    return m_activeList.Count();
}

#ifdef _DEBUG
template<class T, unsigned int MaxFreeCount = 10>
#else
template<class T, unsigned int MaxFreeCount = 1000>
#endif
class Allocator
{
private:
    static _Allocator<T, MaxFreeCount>  _allocator;
public:
    static void * operator new(unsigned int size){ return _allocator.Allocate(); }

    static void operator delete(void *p){_allocator.Free(static_cast<T*>(p));}

    static void __Destroy() { _allocator.Flush(); }

    static uint32 UseCount( ) { return _allocator.ActiveCount(); }
};
template<class T, unsigned int MaxFreeCount>
_Allocator<T, MaxFreeCount> Allocator<T, MaxFreeCount>::_allocator;
#endif //MEM_LIST_H_INCLUDE_20111115__
