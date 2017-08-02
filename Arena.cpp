/********************************************************************
 ** Copyright(c) 2017, luzeya
 ** All rights reserved
 **
 ** 文件名：Arena.cpp
 ** 创建人：路泽亚
 ** 创建日期： 2017-08-03
 **
 ** 描  述：实存块
 ** 注  意：1.
 ********************************************************************/
#include <cstring>
#include <cstdlib>
#include <cstddef>
#include "Arena.h"

void *Arena::alloc(ulong nbytes)
{
    void *ptr;
    Hunk *hunk;
    ulong len = sizeof(Hunk) + size + ((nbytes >= size) ? nbytes : 0);

    for (hunk = arena; hunk != NULL; hunk = hunk->next) {
        if (hunk->avail + nbytes <= hunk->limit) {
            ptr = hunk->avail;
            hunk->avail += nbytes;
            return ptr;
        }
    }

    hunk = arena;

    // newhunk
    arena = (Hunk*) new char[len];

    arena->avail = (char*)arena + sizeof(Hunk);
    arena->limit = (char*)arena + len;
    arena->next = hunk;

    ptr = arena->avail;
    arena->avail += nbytes;
    return ptr;
}

void *Arena::calloc(ulong count, ulong nbytes)
{
    ulong size = count * nbytes;
    void *ptr = alloc(size);
    memset(ptr, 0, size);
    return ptr;
}

void Arena::dispose()
{
    Hunk *temp;
    while (arena) {
        temp = arena;
        arena = arena->next;
        delete[] temp;
    }
}