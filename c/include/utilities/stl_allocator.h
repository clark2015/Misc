#pragma once
#include <map>
#include "allocator.h"

template<class T>  
class user_defined_allocator : public std::allocator<T>  
{  
public:  
    typedef std::allocator<T> base_type;  

    template<class Other>  
    struct rebind  
    {  
        typedef user_defined_allocator<Other> other;  
    };  

    user_defined_allocator() {}  
  
    user_defined_allocator(user_defined_allocator<T> const&) {}  
  
    user_defined_allocator<T>& operator=(user_defined_allocator<T> const&) { return (*this); }  

    template<class Other>  
    user_defined_allocator(user_defined_allocator<Other> const&) {}  

    template<class Other>  
    user_defined_allocator<T>& operator=(user_defined_allocator<Other> const&) { return (*this); }  
  
    class __T : public T, public Allocator<__T, 1000>
    {
    };

    base_type::pointer allocate(base_type::size_type count) {
        return new __T;
        //return (base_type::allocate(count));
    }  
  
    void deallocate(base_type::pointer ptr, base_type::size_type count) {
        delete (__T*)(ptr);
        //base_type::deallocate(ptr, count);
    }  
};  
