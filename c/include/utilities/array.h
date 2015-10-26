#ifndef ARRAY_H_INLCUDE__
#define ARRAY_H_INLCUDE__

#include <assert.h>
#include <stdexcept>
//#include <xutility>
#pragma warning(push)
#pragma warning(disable:4996)
namespace mmo_server {

template<class T, size_t N = 0x1000>
class darray {
public:
    size_t  m_size;
    T       m_elems[N];    // fixed-size darray of elements of type T
        
public:
    // type definitions
    typedef T              value_type;
    typedef T*             iterator;
    typedef const T*       const_iterator;
    typedef T&             reference;
    typedef const T&       const_reference;
    typedef size_t    size_type;
        
    darray() : m_size(0) {}
    darray( const T* pT, int nSize )
    {
        assert( nSize <= N );
        m_size = nSize;
        for( int i = 0; i < m_size; ++i )
        {
            m_elems[i] = pT[i];
        }
    }
    // iterator support
    iterator begin() { return m_elems; }
    const_iterator begin() const { return m_elems; }
    iterator end() { return m_elems+m_size; }
    const_iterator end() const { return m_elems+m_size; }

    typedef std::reverse_iterator<iterator> reverse_iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
    
    reverse_iterator rbegin() { return reverse_iterator(end()); }
    const_reverse_iterator rbegin() const {
        return const_reverse_iterator(end());
    }
    reverse_iterator rend() { return reverse_iterator(begin()); }
    const_reverse_iterator rend() const {
        return const_reverse_iterator(begin());
    }
        
    // operator[]
    reference operator[](size_type i) 
    { 
        assert( i < m_size && "out of range" ); 
        return m_elems[i];
    }
        
    const_reference operator[](size_type i) const 
    {     
        assert( i < m_size && "out of range" ); 
        return m_elems[i]; 
    }
        
    // at() with range check
    reference at(size_type i) { rangecheck(i); return m_elems[i]; }
    const_reference at(size_type i) const { rangecheck(i); return m_elems[i]; }
        
    // front() and back()
    reference front() 
    { 
        return m_elems[0]; 
    }
        
    const_reference front() const 
    {
        return m_elems[0];
    }
        
    reference back() 
    { 
        assert( !empty() && "out of range" ); 
        return m_elems[m_size-1]; 
    }
        
    const_reference back() const 
    { 
        assert( !empty() && "out of range" ); 
        return m_elems[m_size-1]; 
    }
        
    // size is constant
    size_type size() const { return m_size; }
    bool empty() const { return m_size == 0; }
    static size_type max_size() { return N; }
    enum { static_size = N };
        

    iterator erase( iterator itr ) {
        std::copy( itr + 1, end(), itr );
        m_size -= 1;
        return itr;
    }
        
    //iterator erase( iterator first, iterator last ) {
    //    std::copy( last, end(), begin() );
    //    m_size -= std::distance(first, last);
    //    return itr;
    //}
        
    void push_back(const T& x){
        assert(m_size < N);
        if(m_size >= N) return;

        m_elems[m_size] = x;
        ++m_size;
    }
        
    void pop_back(){
        assert(m_size > 0);
        --m_size;
    }

    void clear(){
        m_size = 0;
    }

    void resize(size_t size) {
        m_size = size;
    }

    // swap (note: linear complexity)
    void swap (darray<T,N>& y) {
        std::swap_ranges(begin(),end(),y.begin());
    }
        
    // direct access to data (read-only)
    const T* data() const { return m_elems; }
        
    // use darray as C darray (direct read/write access to data)
    T* c_array() { return m_elems; }
        
    // assignment with type conversion
    template <class T2>
        darray<T,N>& operator= (const darray<T2,N>& rhs) {
        std::copy(rhs.begin(),rhs.end(), begin());
        return *this;
    }
        
    // assign one value to all elements
    void assign (const T& value)
    {
        std::fill_n(begin(),size(),value);
    }
        
    void assign (iterator first, iterator last)
    {
        clear();
        m_size = std::distance(first, last);
        assert( m_size <= N );
        std::copy(first, last, begin());
    }
        
    void assign (const_iterator first, const_iterator last)
    {
        clear();
        m_size = std::distance(first, last);
        assert( m_size <= N );
        std::copy(first, last, begin());
    }

    iterator insert(iterator first, iterator last)
    {
        assert( N <= m_size + std::distance(first, last) );
        std::copy(first, last, end());
        m_size += std::distance(first, last);
        return end();
    }
        
    void reserve(int n) {
        rangecheck(n);
    }
    // check range (may be private because it is static)
    void rangecheck (size_type i) {
        if (i > max_size()) { 
            throw std::range_error("darray<>: index out of range");
        }
    }
};

// comparisons
template<class T, size_t N>
bool operator== (const darray<T,N>& x, const darray<T,N>& y) {
    return std::equal(x.begin(), x.end(), y.begin());
}
template<class T, size_t N>
bool operator< (const darray<T,N>& x, const darray<T,N>& y) {
    return std::lexicographical_compare(x.begin(),x.end(),y.begin(),y.end());
}
template<class T, size_t N>
bool operator!= (const darray<T,N>& x, const darray<T,N>& y) {
    return !(x==y);
}
template<class T, size_t N>
bool operator> (const darray<T,N>& x, const darray<T,N>& y) {
    return y<x;
}
template<class T, size_t N>
bool operator<= (const darray<T,N>& x, const darray<T,N>& y) {
    return !(y<x);
}
template<class T, size_t N>
bool operator>= (const darray<T,N>& x, const darray<T,N>& y) {
    return !(x<y);
}

// global swap()
template<class T, size_t N>
inline void swap (darray<T,N>& x, darray<T,N>& y) {
    x.swap(y);
}


} /* namespace mmo_server */

#pragma warning(pop)
#endif //ARRAY_H_INLCUDE__
