#ifndef _AFX_TMPL_PORT_H__
#define _AFX_TMPL_PORT_H__

#include <windows.h>
#include <assert.h>
struct __FPOSITION { };
typedef __FPOSITION* FPOSITION;

#define F_BEFORE_START_POSITION ((FPOSITION)-1L)

struct FPlex     // warning variable length structure
{
	FPlex* pNext;
	DWORD dwReserved[1];    // align on 8 byte boundary
	// BYTE data[maxNum*elementSize];

	void* data() { return this+1; }

	static FPlex* Create(FPlex*& pHead, UINT nMax, UINT cbElement)
			// like 'calloc' but no zero fill
			// may throw memory exceptions
	{
		assert(nMax > 0 && cbElement > 0);
		FPlex* p = (FPlex*) new BYTE[sizeof(FPlex) + nMax * cbElement];
				// may throw exception
		p->pNext = pHead;
		pHead = p;  // change head (adds in reverse order for simplicity)
		return p;
	}

	void FreeDataChain()       // free this one and links
	{
		FPlex* p = this;
		while (p != NULL)
		{
			BYTE* bytes = (BYTE*) p;
			FPlex* pNext = p->pNext;
			delete[] bytes;
			p = pNext;
		}
	}
};

inline bool	FIsValidAddress(const void* lp, UINT nBytes,BOOL bReadWrite = true)	
{	
	return (lp != NULL && !IsBadReadPtr(lp, nBytes) &&
		(!bReadWrite || !IsBadWritePtr((LPVOID)lp, nBytes)));
}
#define F_ASSERT_VALID(x)		assert(FIsValidAddress((x),1))	


template<class ARG_KEY>
inline UINT FHashKey(ARG_KEY key)
{
	// default identity hash - works for most primitive values
	return ((UINT)(void*)(DWORD)key) >> 4;
}

template<> inline UINT FHashKey<LPCSTR> (LPCSTR key)
{
	UINT nHash = 0;
	while (*key)
		nHash = (nHash<<5) + nHash + *key++;
	return nHash;
}

template<class TYPE>
inline void FConstructElements(TYPE* pElements, int nCount)
{
	assert(nCount == 0 ||
		FIsValidAddress(pElements, nCount * sizeof(TYPE)));

	// first do bit-wise zero initialization
	memset((void*)pElements, 0, nCount * sizeof(TYPE));

	// then call the constructor(s)
	for (; nCount--; pElements++)
		::new((void*)pElements) TYPE;
}

template<class TYPE>
inline void FDestructElements(TYPE* pElements, int nCount)
{
	assert(nCount == 0 ||
		FIsValidAddress(pElements, nCount * sizeof(TYPE)));

	// call the destructor(s)
	for (; nCount--; pElements++)
		pElements->~TYPE();
}

template<class TYPE>
inline void FCopyElements(TYPE* pDest, const TYPE* pSrc, int nCount)
{
	assert(nCount == 0 ||
		FIsValidAddress(pDest, nCount * sizeof(TYPE)));
	assert(nCount == 0 ||
		FIsValidAddress(pSrc, nCount * sizeof(TYPE)));
	
	// default is element-copy using assignment
	while (nCount--)
		*pDest++ = *pSrc++;
}

template<class TYPE, class ARG_TYPE>
inline BOOL FCompareElements(const TYPE* pElement1, const ARG_TYPE* pElement2)
{
	assert(FIsValidAddress(pElement1, sizeof(TYPE), FALSE));
	assert(FIsValidAddress(pElement2, sizeof(ARG_TYPE), FALSE));

	return *pElement1 == *pElement2;
}


//**************************************************************************
template<class TYPE, class ARG_TYPE>
class FArray 
{
public:
// Construction
	FArray();

// Attributes
	int GetSize() const;
	int GetUpperBound() const;
	void SetSize(int nNewSize, int nGrowBy = -1);

// Operations
	// Clean up
	void FreeExtra();
	void RemoveAll();

	// Accessing elements
	TYPE GetAt(int nIndex) const;
	void SetAt(int nIndex, ARG_TYPE newElement);
	TYPE& ElementAt(int nIndex);

	// Direct Access to the element data (may return NULL)
	const TYPE* GetData() const;
	TYPE* GetData();

	// Potentially growing the array
	void SetAtGrow(int nIndex, ARG_TYPE newElement);
	int Add(ARG_TYPE newElement);
	int Append(const FArray& src);
	void Copy(const FArray& src);

	// overloaded operator helpers
	TYPE operator[](int nIndex) const;
	TYPE& operator[](int nIndex);

	// Operations that move elements around
	void InsertAt(int nIndex, ARG_TYPE newElement, int nCount = 1);
	void RemoveAt(int nIndex, int nCount = 1);
	void InsertAt(int nStartIndex, FArray* pNewArray);

// Implementation
protected:
	TYPE* m_pData;   // the actual array of data
	int m_nSize;     // # of elements (upperBound - 1)
	int m_nMaxSize;  // max allocated
	int m_nGrowBy;   // grow amount

public:
	~FArray();
};

/////////////////////////////////////////////////////////////////////////////
// FArray<TYPE, ARG_TYPE> inline functions

template<class TYPE, class ARG_TYPE>
inline int FArray<TYPE, ARG_TYPE>::GetSize() const
	{ return m_nSize; }
template<class TYPE, class ARG_TYPE>
inline int FArray<TYPE, ARG_TYPE>::GetUpperBound() const
	{ return m_nSize-1; }
template<class TYPE, class ARG_TYPE>
inline void FArray<TYPE, ARG_TYPE>::RemoveAll()
	{ SetSize(0, -1); }
template<class TYPE, class ARG_TYPE>
inline TYPE FArray<TYPE, ARG_TYPE>::GetAt(int nIndex) const
	{ assert(nIndex >= 0 && nIndex < m_nSize);
		return m_pData[nIndex]; }
template<class TYPE, class ARG_TYPE>
inline void FArray<TYPE, ARG_TYPE>::SetAt(int nIndex, ARG_TYPE newElement)
	{ assert(nIndex >= 0 && nIndex < m_nSize);
		m_pData[nIndex] = newElement; }
template<class TYPE, class ARG_TYPE>
inline TYPE& FArray<TYPE, ARG_TYPE>::ElementAt(int nIndex)
	{ assert(nIndex >= 0 && nIndex < m_nSize);
		return m_pData[nIndex]; }
template<class TYPE, class ARG_TYPE>
inline const TYPE* FArray<TYPE, ARG_TYPE>::GetData() const
	{ return (const TYPE*)m_pData; }
template<class TYPE, class ARG_TYPE>
inline TYPE* FArray<TYPE, ARG_TYPE>::GetData()
	{ return (TYPE*)m_pData; }
template<class TYPE, class ARG_TYPE>
inline int FArray<TYPE, ARG_TYPE>::Add(ARG_TYPE newElement)
	{ int nIndex = m_nSize;
		SetAtGrow(nIndex, newElement);
		return nIndex; }
template<class TYPE, class ARG_TYPE>
inline TYPE FArray<TYPE, ARG_TYPE>::operator[](int nIndex) const
	{ return GetAt(nIndex); }
template<class TYPE, class ARG_TYPE>
inline TYPE& FArray<TYPE, ARG_TYPE>::operator[](int nIndex)
	{ return ElementAt(nIndex); }

/////////////////////////////////////////////////////////////////////////////
// FArray<TYPE, ARG_TYPE> out-of-line functions

template<class TYPE, class ARG_TYPE>
FArray<TYPE, ARG_TYPE>::FArray()
{
	m_pData = NULL;
	m_nSize = m_nMaxSize = m_nGrowBy = 0;
}

template<class TYPE, class ARG_TYPE>
FArray<TYPE, ARG_TYPE>::~FArray()
{
	F_ASSERT_VALID(this);

	if (m_pData != NULL)
	{
		FDestructElements<TYPE>(m_pData, m_nSize);
		delete[] (BYTE*)m_pData;
	}
}

template<class TYPE, class ARG_TYPE>
void FArray<TYPE, ARG_TYPE>::SetSize(int nNewSize, int nGrowBy)
{
	F_ASSERT_VALID(this);
	assert(nNewSize >= 0);

	if (nGrowBy != -1)
		m_nGrowBy = nGrowBy;  // set new size

	if (nNewSize == 0)
	{
		// shrink to nothing
		if (m_pData != NULL)
		{
			FDestructElements<TYPE>(m_pData, m_nSize);
			delete[] (BYTE*)m_pData;
			m_pData = NULL;
		}
		m_nSize = m_nMaxSize = 0;
	}
	else if (m_pData == NULL)
	{
		// create one with exact size
#ifdef SIZE_T_MAX
		assert(nNewSize <= SIZE_T_MAX/sizeof(TYPE));    // no overflow
#endif
		m_pData = (TYPE*) new BYTE[nNewSize * sizeof(TYPE)];
		FConstructElements<TYPE>(m_pData, nNewSize);
		m_nSize = m_nMaxSize = nNewSize;
	}
	else if (nNewSize <= m_nMaxSize)
	{
		// it fits
		if (nNewSize > m_nSize)
		{
			// initialize the new elements
			FConstructElements<TYPE>(&m_pData[m_nSize], nNewSize-m_nSize);
		}
		else if (m_nSize > nNewSize)
		{
			// destroy the old elements
			FDestructElements<TYPE>(&m_pData[nNewSize], m_nSize-nNewSize);
		}
		m_nSize = nNewSize;
	}
	else
	{
		// otherwise, grow array
		int nGrowBy = m_nGrowBy;
		if (nGrowBy == 0)
		{
			// heuristically determine growth when nGrowBy == 0
			//  (this avoids heap fragmentation in many situations)
			nGrowBy = m_nSize / 8;
			nGrowBy = (nGrowBy < 4) ? 4 : ((nGrowBy > 1024) ? 1024 : nGrowBy);
		}
		int nNewMax;
		if (nNewSize < m_nMaxSize + nGrowBy)
			nNewMax = m_nMaxSize + nGrowBy;  // granularity
		else
			nNewMax = nNewSize;  // no slush

		assert(nNewMax >= m_nMaxSize);  // no wrap around
#ifdef SIZE_T_MAX
		assert(nNewMax <= SIZE_T_MAX/sizeof(TYPE)); // no overflow
#endif
		TYPE* pNewData = (TYPE*) new BYTE[nNewMax * sizeof(TYPE)];

		// copy new data from old
		memcpy(pNewData, m_pData, m_nSize * sizeof(TYPE));

		// construct remaining elements
		assert(nNewSize > m_nSize);
		FConstructElements<TYPE>(&pNewData[m_nSize], nNewSize-m_nSize);

		// get rid of old stuff (note: no destructors called)
		delete[] (BYTE*)m_pData;
		m_pData = pNewData;
		m_nSize = nNewSize;
		m_nMaxSize = nNewMax;
	}
}

template<class TYPE, class ARG_TYPE>
int FArray<TYPE, ARG_TYPE>::Append(const FArray& src)
{
	F_ASSERT_VALID(this);
	assert(this != &src);   // cannot append to itself

	int nOldSize = m_nSize;
	SetSize(m_nSize + src.m_nSize);
	FCopyElements<TYPE>(m_pData + nOldSize, src.m_pData, src.m_nSize);
	return nOldSize;
}

template<class TYPE, class ARG_TYPE>
void FArray<TYPE, ARG_TYPE>::Copy(const FArray& src)
{
	F_ASSERT_VALID(this);
	assert(this != &src);   // cannot append to itself

	SetSize(src.m_nSize);
	FCopyElements<TYPE>(m_pData, src.m_pData, src.m_nSize);
}

template<class TYPE, class ARG_TYPE>
void FArray<TYPE, ARG_TYPE>::FreeExtra()
{
	F_ASSERT_VALID(this);

	if (m_nSize != m_nMaxSize)
	{
		// shrink to desired size
#ifdef SIZE_T_MAX
		assert(m_nSize <= SIZE_T_MAX/sizeof(TYPE)); // no overflow
#endif
		TYPE* pNewData = NULL;
		if (m_nSize != 0)
		{
			pNewData = (TYPE*) new BYTE[m_nSize * sizeof(TYPE)];
			// copy new data from old
			memcpy(pNewData, m_pData, m_nSize * sizeof(TYPE));
		}

		// get rid of old stuff (note: no destructors called)
		delete[] (BYTE*)m_pData;
		m_pData = pNewData;
		m_nMaxSize = m_nSize;
	}
}

template<class TYPE, class ARG_TYPE>
void FArray<TYPE, ARG_TYPE>::SetAtGrow(int nIndex, ARG_TYPE newElement)
{
	F_ASSERT_VALID(this);
	assert(nIndex >= 0);

	if (nIndex >= m_nSize)
		SetSize(nIndex+1, -1);
	m_pData[nIndex] = newElement;
}

template<class TYPE, class ARG_TYPE>
void FArray<TYPE, ARG_TYPE>::InsertAt(int nIndex, ARG_TYPE newElement, int nCount /*=1*/)
{
	F_ASSERT_VALID(this);
	assert(nIndex >= 0);    // will expand to meet need
	assert(nCount > 0);     // zero or negative size not allowed

	if (nIndex >= m_nSize)
	{
		// adding after the end of the array
		SetSize(nIndex + nCount, -1);   // grow so nIndex is valid
	}
	else
	{
		// inserting in the middle of the array
		int nOldSize = m_nSize;
		SetSize(m_nSize + nCount, -1);  // grow it to new size
		// destroy intial data before copying over it
		FDestructElements<TYPE>(&m_pData[nOldSize], nCount);
		// shift old data up to fill gap
		memmove(&m_pData[nIndex+nCount], &m_pData[nIndex],
			(nOldSize-nIndex) * sizeof(TYPE));

		// re-init slots we copied from
		FConstructElements<TYPE>(&m_pData[nIndex], nCount);
	}

	// insert new value in the gap
	assert(nIndex + nCount <= m_nSize);
	while (nCount--)
		m_pData[nIndex++] = newElement;
}

template<class TYPE, class ARG_TYPE>
void FArray<TYPE, ARG_TYPE>::RemoveAt(int nIndex, int nCount)
{
	F_ASSERT_VALID(this);
	assert(nIndex >= 0);
	assert(nCount >= 0);
	assert(nIndex + nCount <= m_nSize);

	// just remove a range
	int nMoveCount = m_nSize - (nIndex + nCount);
	FDestructElements<TYPE>(&m_pData[nIndex], nCount);
	if (nMoveCount)
		memmove(&m_pData[nIndex], &m_pData[nIndex + nCount],
			nMoveCount * sizeof(TYPE));
	m_nSize -= nCount;
}

template<class TYPE, class ARG_TYPE>
void FArray<TYPE, ARG_TYPE>::InsertAt(int nStartIndex, FArray* pNewArray)
{
	F_ASSERT_VALID(this);
	assert(pNewArray != NULL);
	F_ASSERT_VALID(pNewArray);
	assert(nStartIndex >= 0);

	if (pNewArray->GetSize() > 0)
	{
		InsertAt(nStartIndex, pNewArray->GetAt(0), pNewArray->GetSize());
		for (int i = 0; i < pNewArray->GetSize(); i++)
			SetAt(nStartIndex + i, pNewArray->GetAt(i));
	}
}
//***************************************************************************


template<class TYPE, class ARG_TYPE>
class FList 
{
protected:
	struct FNode
	{
		FNode* pNext;
		FNode* pPrev;
		TYPE data;
	};
public:
// Construction
	FList(int nBlockSize = 10);

// Attributes (head and tail)
	// count of elements
	int GetCount() const;
	BOOL IsEmpty() const;

	// peek at head or tail
	TYPE& GetHead();
	TYPE GetHead() const;
	TYPE& GetTail();
	TYPE GetTail() const;

// Operations
	// get head or tail (and remove it) - don't call on empty list !
	TYPE RemoveHead();
	void RemoveHeadOnly(); //> 新加的函数
	TYPE RemoveTail();

	// add before head or after tail
	FPOSITION AddHead(ARG_TYPE newElement);
	FPOSITION AddTail(ARG_TYPE newElement);

	// add another list of elements before head or after tail
	void AddHead(FList* pNewList);
	void AddTail(FList* pNewList);

	// remove all elements
	void RemoveAll();

	// iteration
	FPOSITION GetHeadPosition() const;
	FPOSITION GetTailPosition() const;
	TYPE& GetNext(FPOSITION& rPosition); // return *Position++
	TYPE GetNext(FPOSITION& rPosition) const; // return *Position++
	TYPE& GetPrev(FPOSITION& rPosition); // return *Position--
	TYPE GetPrev(FPOSITION& rPosition) const; // return *Position--

	// getting/modifying an element at a given position
	TYPE& GetAt(FPOSITION position);
	TYPE GetAt(FPOSITION position) const;
	void SetAt(FPOSITION pos, ARG_TYPE newElement);
	void RemoveAt(FPOSITION position);

	// inserting before or after a given position
	FPOSITION InsertBefore(FPOSITION position, ARG_TYPE newElement);
	FPOSITION InsertAfter(FPOSITION position, ARG_TYPE newElement);

	// helper functions (note: O(n) speed)
	FPOSITION Find(ARG_TYPE searchValue, FPOSITION startAfter = NULL) const;
		// defaults to starting at the HEAD, return NULL if not found
	FPOSITION FindIndex(int nIndex) const;
		// get the 'nIndex'th element (may return NULL)

// Implementation
protected:
	FNode* m_pNodeHead;
	FNode* m_pNodeTail;
	int m_nCount;
	FNode* m_pNodeFree;
	struct FPlex* m_pBlocks;
	int m_nBlockSize;

	FNode* NewNode(FNode*, FNode*);
	void FreeNode(FNode*);

public:
	~FList();
};

/////////////////////////////////////////////////////////////////////////////
// FList<TYPE, ARG_TYPE> inline functions

template<class TYPE, class ARG_TYPE>
inline int FList<TYPE, ARG_TYPE>::GetCount() const
	{ return m_nCount; }
template<class TYPE, class ARG_TYPE>
inline BOOL FList<TYPE, ARG_TYPE>::IsEmpty() const
	{ return m_nCount == 0; }
template<class TYPE, class ARG_TYPE>
inline TYPE& FList<TYPE, ARG_TYPE>::GetHead()
	{ assert(m_pNodeHead != NULL);
		return m_pNodeHead->data; }
template<class TYPE, class ARG_TYPE>
inline TYPE FList<TYPE, ARG_TYPE>::GetHead() const
	{ assert(m_pNodeHead != NULL);
		return m_pNodeHead->data; }
template<class TYPE, class ARG_TYPE>
inline TYPE& FList<TYPE, ARG_TYPE>::GetTail()
	{ assert(m_pNodeTail != NULL);
		return m_pNodeTail->data; }
template<class TYPE, class ARG_TYPE>
inline TYPE FList<TYPE, ARG_TYPE>::GetTail() const
	{ assert(m_pNodeTail != NULL);
		return m_pNodeTail->data; }
template<class TYPE, class ARG_TYPE>
inline FPOSITION FList<TYPE, ARG_TYPE>::GetHeadPosition() const
	{ return (FPOSITION) m_pNodeHead; }
template<class TYPE, class ARG_TYPE>
inline FPOSITION FList<TYPE, ARG_TYPE>::GetTailPosition() const
	{ return (FPOSITION) m_pNodeTail; }
template<class TYPE, class ARG_TYPE>
inline TYPE& FList<TYPE, ARG_TYPE>::GetNext(FPOSITION& rPosition) // return *Position++
	{ FNode* pNode = (FNode*) rPosition;
		assert(FIsValidAddress(pNode, sizeof(FNode)));
		rPosition = (FPOSITION) pNode->pNext;
		return pNode->data; }
template<class TYPE, class ARG_TYPE>
inline TYPE FList<TYPE, ARG_TYPE>::GetNext(FPOSITION& rPosition) const // return *Position++
	{ FNode* pNode = (FNode*) rPosition;
		assert(FIsValidAddress(pNode, sizeof(FNode)));
		rPosition = (FPOSITION) pNode->pNext;
		return pNode->data; }
template<class TYPE, class ARG_TYPE>
inline TYPE& FList<TYPE, ARG_TYPE>::GetPrev(FPOSITION& rPosition) // return *Position--
	{ FNode* pNode = (FNode*) rPosition;
		assert(FIsValidAddress(pNode, sizeof(FNode)));
		rPosition = (FPOSITION) pNode->pPrev;
		return pNode->data; }
template<class TYPE, class ARG_TYPE>
inline TYPE FList<TYPE, ARG_TYPE>::GetPrev(FPOSITION& rPosition) const // return *Position--
	{ FNode* pNode = (FNode*) rPosition;
		assert(FIsValidAddress(pNode, sizeof(FNode)));
		rPosition = (FPOSITION) pNode->pPrev;
		return pNode->data; }
template<class TYPE, class ARG_TYPE>
inline TYPE& FList<TYPE, ARG_TYPE>::GetAt(FPOSITION position)
	{ FNode* pNode = (FNode*) position;
		assert(FIsValidAddress(pNode, sizeof(FNode)));
		return pNode->data; }
template<class TYPE, class ARG_TYPE>
inline TYPE FList<TYPE, ARG_TYPE>::GetAt(FPOSITION position) const
	{ FNode* pNode = (FNode*) position;
		assert(FIsValidAddress(pNode, sizeof(FNode)));
		return pNode->data; }
template<class TYPE, class ARG_TYPE>
inline void FList<TYPE, ARG_TYPE>::SetAt(FPOSITION pos, ARG_TYPE newElement)
	{ FNode* pNode = (FNode*) pos;
		assert(FIsValidAddress(pNode, sizeof(FNode)));
		pNode->data = newElement; }

template<class TYPE, class ARG_TYPE>
FList<TYPE, ARG_TYPE>::FList(int nBlockSize)
{
	assert(nBlockSize > 0);

	m_nCount = 0;
	m_pNodeHead = m_pNodeTail = m_pNodeFree = NULL;
	m_pBlocks = NULL;
	m_nBlockSize = nBlockSize;
}

template<class TYPE, class ARG_TYPE>
void FList<TYPE, ARG_TYPE>::RemoveAll()
{
	F_ASSERT_VALID(this);

	// destroy elements
	FNode* pNode;
	for (pNode = m_pNodeHead; pNode != NULL; pNode = pNode->pNext)
		FDestructElements<TYPE>(&pNode->data, 1);
	//	pNode->data.~TYPE();		//> 学习vc7.0的做法

	m_nCount = 0;
	m_pNodeHead = m_pNodeTail = m_pNodeFree = NULL;
	m_pBlocks->FreeDataChain();
	m_pBlocks = NULL;
}

template<class TYPE, class ARG_TYPE>
FList<TYPE, ARG_TYPE>::~FList()
{
	RemoveAll();
	assert(m_nCount == 0);
}

/////////////////////////////////////////////////////////////////////////////
// Node helpers
//
// Implementation note: FNode's are stored in FPlex blocks and
//  chained together. Free blocks are maintained in a singly linked list
//  using the 'pNext' member of FNode with 'm_pNodeFree' as the head.
//  Used blocks are maintained in a doubly linked list using both 'pNext'
//  and 'pPrev' as links and 'm_pNodeHead' and 'm_pNodeTail'
//   as the head/tail.
//
// We never free a FPlex block unless the List is destroyed or RemoveAll()
//  is used - so the total number of FPlex blocks may grow large depending
//  on the maximum past size of the list.
//

template<class TYPE, class ARG_TYPE>
typename FList<TYPE, ARG_TYPE>::FNode*
FList<TYPE, ARG_TYPE>::NewNode(typename FList::FNode* pPrev, typename FList::FNode* pNext)
{
	if (m_pNodeFree == NULL)
	{
		// add another block
		FPlex* pNewBlock = FPlex::Create(m_pBlocks, m_nBlockSize,
				 sizeof(FNode));

		// chain them into free list
		FNode* pNode = (FNode*) pNewBlock->data();
		// free in reverse order to make it easier to debug
		pNode += m_nBlockSize - 1;
		for (int i = m_nBlockSize-1; i >= 0; i--, pNode--)
		{
			pNode->pNext = m_pNodeFree;
			m_pNodeFree = pNode;
		}
	}
	assert(m_pNodeFree != NULL);  // we must have something

	FList::FNode* pNode = m_pNodeFree;
	m_pNodeFree = m_pNodeFree->pNext;
	pNode->pPrev = pPrev;
	pNode->pNext = pNext;
	m_nCount++;
	assert(m_nCount > 0);  // make sure we don't overflow


	FConstructElements<TYPE>(&pNode->data, 1);

	return pNode;
}

template<class TYPE, class ARG_TYPE>
void FList<TYPE, ARG_TYPE>::FreeNode(typename FList::FNode* pNode)
{
	FDestructElements<TYPE>(&pNode->data, 1); //>学习vc7.0
//	pNode->data.~TYPE();

	pNode->pNext = m_pNodeFree;
	m_pNodeFree = pNode;
	m_nCount--;
	assert(m_nCount >= 0);  // make sure we don't underflow

	// if no more elements, cleanup completely
	if (m_nCount == 0)
		RemoveAll();
}

template<class TYPE, class ARG_TYPE>
FPOSITION FList<TYPE, ARG_TYPE>::AddHead(ARG_TYPE newElement)
{
	F_ASSERT_VALID(this);

	FNode* pNewNode = NewNode(NULL, m_pNodeHead);
	pNewNode->data = newElement;
	if (m_pNodeHead != NULL)
		m_pNodeHead->pPrev = pNewNode;
	else
		m_pNodeTail = pNewNode;
	m_pNodeHead = pNewNode;
	return (FPOSITION) pNewNode;
}

template<class TYPE, class ARG_TYPE>
FPOSITION FList<TYPE, ARG_TYPE>::AddTail(ARG_TYPE newElement)
{
	F_ASSERT_VALID(this);

	FNode* pNewNode = NewNode(m_pNodeTail, NULL);
	pNewNode->data = newElement;
	if (m_pNodeTail != NULL)
		m_pNodeTail->pNext = pNewNode;
	else
		m_pNodeHead = pNewNode;
	m_pNodeTail = pNewNode;
	return (FPOSITION) pNewNode;
}

template<class TYPE, class ARG_TYPE>
void FList<TYPE, ARG_TYPE>::AddHead(FList* pNewList)
{
	F_ASSERT_VALID(this);

	assert(pNewList != NULL);
	F_ASSERT_VALID(pNewList);

	// add a list of same elements to head (maintain order)
	FPOSITION pos = pNewList->GetTailPosition();
	while (pos != NULL)
		AddHead(pNewList->GetPrev(pos));
}

template<class TYPE, class ARG_TYPE>
void FList<TYPE, ARG_TYPE>::AddTail(FList* pNewList)
{
	F_ASSERT_VALID(this);
	assert(pNewList != NULL);
	F_ASSERT_VALID(pNewList);

	// add a list of same elements
	FPOSITION pos = pNewList->GetHeadPosition();
	while (pos != NULL)
		AddTail(pNewList->GetNext(pos));
}

template<class TYPE, class ARG_TYPE>
TYPE FList<TYPE, ARG_TYPE>::RemoveHead()
{
	F_ASSERT_VALID(this);
	assert(m_pNodeHead != NULL);  // don't call on empty list !!!
	assert(FIsValidAddress(m_pNodeHead, sizeof(FNode)));

	FNode* pOldNode = m_pNodeHead;
	TYPE returnValue = pOldNode->data;

	m_pNodeHead = pOldNode->pNext;
	if (m_pNodeHead != NULL)
		m_pNodeHead->pPrev = NULL;
	else
		m_pNodeTail = NULL;
	FreeNode(pOldNode);
	return returnValue;
}

template<class TYPE, class ARG_TYPE>
void FList<TYPE, ARG_TYPE>::RemoveHeadOnly()
{
	F_ASSERT_VALID(this);
	assert(m_pNodeHead != NULL);  // don't call on empty list !!!
	assert(FIsValidAddress(m_pNodeHead, sizeof(FNode)));

	FNode* pOldNode = m_pNodeHead;
	TYPE returnValue = pOldNode->data;

	m_pNodeHead = pOldNode->pNext;
	if (m_pNodeHead != NULL)
		m_pNodeHead->pPrev = NULL;
	else
		m_pNodeTail = NULL;
	FreeNode(pOldNode);
}

template<class TYPE, class ARG_TYPE>
TYPE FList<TYPE, ARG_TYPE>::RemoveTail()
{
	F_ASSERT_VALID(this);
	assert(m_pNodeTail != NULL);  // don't call on empty list !!!
	assert(FIsValidAddress(m_pNodeTail, sizeof(FNode)));

	FNode* pOldNode = m_pNodeTail;
	TYPE returnValue = pOldNode->data;

	m_pNodeTail = pOldNode->pPrev;
	if (m_pNodeTail != NULL)
		m_pNodeTail->pNext = NULL;
	else
		m_pNodeHead = NULL;
	FreeNode(pOldNode);
	return returnValue;
}

template<class TYPE, class ARG_TYPE>
FPOSITION FList<TYPE, ARG_TYPE>::InsertBefore(FPOSITION position, ARG_TYPE newElement)
{
	F_ASSERT_VALID(this);

	if (position == NULL)
		return AddHead(newElement); // insert before nothing -> head of the list

	// Insert it before position
	FNode* pOldNode = (FNode*) position;
	FNode* pNewNode = NewNode(pOldNode->pPrev, pOldNode);
	pNewNode->data = newElement;

	if (pOldNode->pPrev != NULL)
	{
		assert(FIsValidAddress(pOldNode->pPrev, sizeof(FNode)));
		pOldNode->pPrev->pNext = pNewNode;
	}
	else
	{
		assert(pOldNode == m_pNodeHead);
		m_pNodeHead = pNewNode;
	}
	pOldNode->pPrev = pNewNode;
	return (FPOSITION) pNewNode;
}

template<class TYPE, class ARG_TYPE>
FPOSITION FList<TYPE, ARG_TYPE>::InsertAfter(FPOSITION position, ARG_TYPE newElement)
{
	F_ASSERT_VALID(this);

	if (position == NULL)
		return AddTail(newElement); // insert after nothing -> tail of the list

	// Insert it before position
	FNode* pOldNode = (FNode*) position;
	assert(FIsValidAddress(pOldNode, sizeof(FNode)));
	FNode* pNewNode = NewNode(pOldNode, pOldNode->pNext);
	pNewNode->data = newElement;

	if (pOldNode->pNext != NULL)
	{
		assert(FIsValidAddress(pOldNode->pNext, sizeof(FNode)));
		pOldNode->pNext->pPrev = pNewNode;
	}
	else
	{
		assert(pOldNode == m_pNodeTail);
		m_pNodeTail = pNewNode;
	}
	pOldNode->pNext = pNewNode;
	return (FPOSITION) pNewNode;
}

template<class TYPE, class ARG_TYPE>
void FList<TYPE, ARG_TYPE>::RemoveAt(FPOSITION position)
{
	F_ASSERT_VALID(this);

	FNode* pOldNode = (FNode*) position;
	assert(FIsValidAddress(pOldNode, sizeof(FNode)));

	// remove pOldNode from list
	if (pOldNode == m_pNodeHead)
	{
		m_pNodeHead = pOldNode->pNext;
	}
	else
	{
		assert(FIsValidAddress(pOldNode->pPrev, sizeof(FNode)));
		pOldNode->pPrev->pNext = pOldNode->pNext;
	}
	if (pOldNode == m_pNodeTail)
	{
		m_pNodeTail = pOldNode->pPrev;
	}
	else
	{
		assert(FIsValidAddress(pOldNode->pNext, sizeof(FNode)));
		pOldNode->pNext->pPrev = pOldNode->pPrev;
	}
	FreeNode(pOldNode);
}

template<class TYPE, class ARG_TYPE>
FPOSITION FList<TYPE, ARG_TYPE>::FindIndex(int nIndex) const
{
	F_ASSERT_VALID(this);

	if (nIndex >= m_nCount || nIndex < 0)
		return NULL;  // went too far

	FNode* pNode = m_pNodeHead;
	while (nIndex--)
	{
		assert(FIsValidAddress(pNode, sizeof(FNode)));
		pNode = pNode->pNext;
	}
	return (FPOSITION) pNode;
}

template<class TYPE, class ARG_TYPE>
FPOSITION FList<TYPE, ARG_TYPE>::Find(ARG_TYPE searchValue, FPOSITION startAfter) const
{
	F_ASSERT_VALID(this);

	FNode* pNode = (FNode*) startAfter;
	if (pNode == NULL)
	{
		pNode = m_pNodeHead;  // start at head
	}
	else
	{
		assert(FIsValidAddress(pNode, sizeof(FNode)));
		pNode = pNode->pNext;  // start after the one specified
	}

	for (; pNode != NULL; pNode = pNode->pNext)
		if (FCompareElements<TYPE>(&pNode->data, &searchValue))
			return (FPOSITION)pNode;
	return NULL;
}

//*****************************************************************************************

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
class FMap
{
protected:
	// Association
	struct FAssoc
	{
		FAssoc* pNext;
		UINT nHashValue;  // needed for efficient iteration
		KEY key;
		VALUE value;
	};
public:
// Construction
	FMap(int nBlockSize = 10);

// Attributes
	// number of elements
	int GetCount() const;
	BOOL IsEmpty() const;

	// Lookup
	BOOL Lookup(ARG_KEY key, VALUE& rValue) const;

// Operations
	// Lookup and add if not there
	VALUE& operator[](ARG_KEY key);

	// add a new (key, value) pair
	void SetAt(ARG_KEY key, ARG_VALUE newValue);

	// removing existing (key, ?) pair
	BOOL RemoveKey(ARG_KEY key);
	void RemoveAll();

	// iterating all (key, value) pairs
	FPOSITION GetStartPosition() const;
	void GetNextAssoc(FPOSITION& rNextPosition, KEY& rKey, VALUE& rValue) const;

	// advanced features for derived classes
	UINT GetHashTableSize() const;
	void InitHashTable(UINT hashSize, BOOL bAllocNow = TRUE);

// Implementation
protected:
	FAssoc** m_pHashTable;
	UINT m_nHashTableSize;
	int m_nCount;
	FAssoc* m_pFreeList;
	struct FPlex* m_pBlocks;
	int m_nBlockSize;

	FAssoc* NewAssoc();
	void FreeAssoc(FAssoc*);
	FAssoc* GetAssocAt(ARG_KEY, UINT&) const;

public:
	~FMap();
};

/////////////////////////////////////////////////////////////////////////////
// FMap<KEY, ARG_KEY, VALUE, ARG_VALUE> inline functions

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
inline int FMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::GetCount() const
	{ return m_nCount; }
template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
inline BOOL FMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::IsEmpty() const
	{ return m_nCount == 0; }
template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
inline void FMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::SetAt(ARG_KEY key, ARG_VALUE newValue)
	{ (*this)[key] = newValue; }
template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
inline FPOSITION FMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::GetStartPosition() const
	{ return (m_nCount == 0) ? NULL : F_BEFORE_START_POSITION; }
template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
inline UINT FMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::GetHashTableSize() const
	{ return m_nHashTableSize; }

/////////////////////////////////////////////////////////////////////////////
// FMap<KEY, ARG_KEY, VALUE, ARG_VALUE> out-of-line functions

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
FMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::FMap(int nBlockSize)
{
	assert(nBlockSize > 0);

	m_pHashTable = NULL;
	m_nHashTableSize = 17;  // default size
	m_nCount = 0;
	m_pFreeList = NULL;
	m_pBlocks = NULL;
	m_nBlockSize = nBlockSize;
}

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
void FMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::InitHashTable(
	UINT nHashSize, BOOL bAllocNow)
//
// Used to force allocation of a hash table or to override the default
//   hash table size of (which is fairly small)
{
	F_ASSERT_VALID(this);
	assert(m_nCount == 0);
	assert(nHashSize > 0);

	if (m_pHashTable != NULL)
	{
		// free hash table
		delete[] m_pHashTable;
		m_pHashTable = NULL;
	}

	if (bAllocNow)
	{
		m_pHashTable = new FAssoc* [nHashSize];
		memset(m_pHashTable, 0, sizeof(FAssoc*) * nHashSize);
	}
	m_nHashTableSize = nHashSize;
}

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
void FMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::RemoveAll()
{
//	F_ASSERT_VALID(this);

	if (m_pHashTable != NULL)
	{
		// destroy elements (values and keys)
		for (UINT nHash = 0; nHash < m_nHashTableSize; nHash++)
		{
			FAssoc* pAssoc;
			for (pAssoc = m_pHashTable[nHash]; pAssoc != NULL;
			  pAssoc = pAssoc->pNext)
			{
				FDestructElements<VALUE>(&pAssoc->value, 1);
				FDestructElements<KEY>(&pAssoc->key, 1);
			}
		}
	}

	// free hash table
	delete[] m_pHashTable;
	m_pHashTable = NULL;

	m_nCount = 0;
	m_pFreeList = NULL;
	m_pBlocks->FreeDataChain();
	m_pBlocks = NULL;
}

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
FMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::~FMap()
{
	RemoveAll();
	assert(m_nCount == 0);
}

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
typename FMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::FAssoc*
FMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::NewAssoc()
{
	if (m_pFreeList == NULL)
	{
		// add another block
		FPlex* newBlock = FPlex::Create(m_pBlocks, m_nBlockSize, sizeof(FMap::FAssoc));
		// chain them into free list
		FMap::FAssoc* pAssoc = (FMap::FAssoc*) newBlock->data();
		// free in reverse order to make it easier to debug
		pAssoc += m_nBlockSize - 1;
		for (int i = m_nBlockSize-1; i >= 0; i--, pAssoc--)
		{
			pAssoc->pNext = m_pFreeList;
			m_pFreeList = pAssoc;
		}
	}
	assert(m_pFreeList != NULL);  // we must have something

	FMap::FAssoc* pAssoc = m_pFreeList;
	m_pFreeList = m_pFreeList->pNext;
	m_nCount++;
	assert(m_nCount > 0);  // make sure we don't overflow
	FConstructElements<KEY>(&pAssoc->key, 1);
	FConstructElements<VALUE>(&pAssoc->value, 1);   // special construct values
	return pAssoc;
}

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
void FMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::FreeAssoc(typename FMap::FAssoc* pAssoc)
{
	FDestructElements<VALUE>(&pAssoc->value, 1);
	FDestructElements<KEY>(&pAssoc->key, 1);
	pAssoc->pNext = m_pFreeList;
	m_pFreeList = pAssoc;
	m_nCount--;
	assert(m_nCount >= 0);  // make sure we don't underflow

	// if no more elements, cleanup completely
	if (m_nCount == 0)
		RemoveAll();
}

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
typename FMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::FAssoc*
FMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::GetAssocAt(ARG_KEY key, UINT& nHash) const
// find association (or return NULL)
{
	nHash = FHashKey<ARG_KEY>(key) % m_nHashTableSize;

	if (m_pHashTable == NULL)
		return NULL;

	// see if it exists
	FAssoc* pAssoc;
	for (pAssoc = m_pHashTable[nHash]; pAssoc != NULL; pAssoc = pAssoc->pNext)
	{
		if (FCompareElements(&pAssoc->key, &key))
			return pAssoc;
	}
	return NULL;
}

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
BOOL FMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::Lookup(ARG_KEY key, VALUE& rValue) const
{
	F_ASSERT_VALID(this);

	UINT nHash;
	FAssoc* pAssoc = GetAssocAt(key, nHash);
	if (pAssoc == NULL)
		return FALSE;  // not in map

	rValue = pAssoc->value;
	return TRUE;
}

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
VALUE& FMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::operator[](ARG_KEY key)
{
	F_ASSERT_VALID(this);

	UINT nHash;
	FAssoc* pAssoc;
	if ((pAssoc = GetAssocAt(key, nHash)) == NULL)
	{
		if (m_pHashTable == NULL)
			InitHashTable(m_nHashTableSize);

		// it doesn't exist, add a new Association
		pAssoc = NewAssoc();
		pAssoc->nHashValue = nHash;
		pAssoc->key = key;
		// 'pAssoc->value' is a constructed object, nothing more

		// put into hash table
		pAssoc->pNext = m_pHashTable[nHash];
		m_pHashTable[nHash] = pAssoc;
	}
	return pAssoc->value;  // return new reference
}

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
BOOL FMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::RemoveKey(ARG_KEY key)
// remove key - return TRUE if removed
{
	F_ASSERT_VALID(this);

	if (m_pHashTable == NULL)
		return FALSE;  // nothing in the table

	FAssoc** ppAssocPrev;
	ppAssocPrev = &m_pHashTable[FHashKey<ARG_KEY>(key) % m_nHashTableSize];

	FAssoc* pAssoc;
	for (pAssoc = *ppAssocPrev; pAssoc != NULL; pAssoc = pAssoc->pNext)
	{
		if (FCompareElements(&pAssoc->key, &key))
		{
			// remove it
			*ppAssocPrev = pAssoc->pNext;  // remove from list
			FreeAssoc(pAssoc);
			return TRUE;
		}
		ppAssocPrev = &pAssoc->pNext;
	}
	return FALSE;  // not found
}

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
void FMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::GetNextAssoc(FPOSITION& rNextPosition,
	KEY& rKey, VALUE& rValue) const
{
	F_ASSERT_VALID(this);
	assert(m_pHashTable != NULL);  // never call on empty map

	FAssoc* pAssocRet = (FAssoc*)rNextPosition;
	assert(pAssocRet != NULL);

	if (pAssocRet == (FAssoc*) F_BEFORE_START_POSITION)
	{
		// find the first association
		for (UINT nBucket = 0; nBucket < m_nHashTableSize; nBucket++)
			if ((pAssocRet = m_pHashTable[nBucket]) != NULL)
				break;
		assert(pAssocRet != NULL);  // must find something
	}

	// find next association
	assert(FIsValidAddress(pAssocRet, sizeof(FAssoc)));
	FAssoc* pAssocNext;
	if ((pAssocNext = pAssocRet->pNext) == NULL)
	{
		// go to next bucket
		for (UINT nBucket = pAssocRet->nHashValue + 1;
		  nBucket < m_nHashTableSize; nBucket++)
			if ((pAssocNext = m_pHashTable[nBucket]) != NULL)
				break;
	}

	rNextPosition = (FPOSITION) pAssocNext;

	// fill in return data
	rKey = pAssocRet->key;
	rValue = pAssocRet->value;
}


#endif
