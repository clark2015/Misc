#ifndef LIST_H_INCLUDE_20140802__
#define LIST_H_INCLUDE_20140802__

#include <assert.h>
#include <string>

class NodeList
{
public :

    class Node
    {
    public :

        Node() 
            :  m_pNext(0), 
                m_pPrev(0)
        {
        }

        ~Node() 
        {
            assert(m_pNext == 0);
            assert(m_pPrev == 0);
        }

        Node *Next() const
        {
            return m_pNext;
        }
        
        Node *Prev() const
        {
            return m_pPrev;
        }

        void Next(Node *pNext)
        {
            m_pNext = pNext;

            if (pNext)
            {
                pNext->m_pPrev = this;
            }
        }

        void Unlink()
        {
            if (m_pPrev)
            {
                m_pPrev->m_pNext = m_pNext;
            }

            if (m_pNext)
            {
                m_pNext->m_pPrev = m_pPrev;
            }
    
            m_pNext = 0;
            m_pPrev = 0;
        }

    private :
        Node *m_pNext;
        Node *m_pPrev;
    };

    NodeList() 
        :  m_pHead(0), 
        m_pTail(0),
        m_numNodes(0) 
    {
    }

    void PushNode(
        Node *pNode)
    {
        pNode->Next(m_pHead);

        m_pHead = pNode;

        if(m_pTail == 0)
            m_pTail = pNode;

        ++m_numNodes;
    }

    Node *PopHead()
    {
        Node *pNode = m_pHead;

        if (pNode)
        {
            RemoveNode(pNode);
        }

        return pNode;
    }
    
    Node *PopTail()
    {
        Node *pNode = m_pTail;

        if (pNode)
        {
            RemoveNode(pNode);
        }

        return pNode;
    }

    Node *Head() const
    {
        return m_pHead;
    }


    size_t Count() const
    {
        return m_numNodes;
    }

    bool Empty() const
    {
        return (0 == m_numNodes);
    }

    void RemoveNode(Node *pNode)
    {
        if (pNode == m_pHead)
        {
            m_pHead = pNode->Next();
        }

        if (pNode == m_pTail)
        {
            m_pTail = pNode->Prev();
        }

        pNode->Unlink();

        --m_numNodes;
    }

private :

    Node *m_pHead; 
    Node *m_pTail; 

    size_t m_numNodes;
};

///////////////////////////////////////////////////////////////////////////////
// TNodeList
///////////////////////////////////////////////////////////////////////////////

template <class T> class TNodeList : public NodeList
{
    public :
            
        T *PopHead();

        T *PopTail();
    
        T *Head() const;

        static T *Next(const T *pNode);
};

template <class T>
T *TNodeList<T>::PopHead()
{
    return static_cast<T*>(NodeList::PopHead());
}

template <class T>
T *TNodeList<T>::PopTail()
{
    return static_cast<T*>(NodeList::PopTail());
}

template <class T>
T *TNodeList<T>::Head() const
{
    return static_cast<T*>(NodeList::Head());
}

template <class T>
T *TNodeList<T>::Next(const T *pNode)
{
    return static_cast<T*>(pNode->Next());
}


#endif //LIST_H_INCLUDE_20140802__
