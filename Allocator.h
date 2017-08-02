/********************************************************************
 ** Copyright(c) 2017, luzeya
 ** All rights reserved
 **
 ** 文件名：Allocator.h
 ** 创建人：路泽亚
 ** 创建日期： 2017-08-02
 **
 ** 描  述：内存分配器
 ** 注  意：1.
 ********************************************************************/
#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <cstddef>

template <class T>
class Allocator
{
public:

    typedef size_t      size_type;
    typedef ptrdiff_t   difference_type;
    typedef T*          pointer;
    typedef const T*    const_pointer;
    typedef T           value_type;

    template <class Other>
    struct rebind {
        typedef Allocator<Other> other;
    };

    Allocator() {}

    Allocator(Allocator<T> const &) {}

    Allocator<T> &operator = (Allocator<T> const &) { return *this; }

    pointer allocate(size_type s) {
        return (pointer)::operator new(s);
    }

    void deallocate(pointer ptr, size_type /*size*/) {
        ::operator delete(ptr);
    }
};

#endif /* ALLOCATOR_H */
