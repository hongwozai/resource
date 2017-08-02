/********************************************************************
 ** Copyright(c) 2017, luzeya
 ** All rights reserved
 **
 ** 文件名：Arena.h
 ** 创建人：路泽亚
 ** 创建日期： 2017-08-03
 **
 ** 描  述：实存块
 ** 注  意：1.
 ********************************************************************/
#ifndef ARENA_H
#define ARENA_H

class Arena
{
public:

    typedef unsigned int uint;

    typedef unsigned long ulong;

public:

    Arena(uint size = 1024) : size(size), arena(0) {}

    ~Arena() { dispose(); }

    // 分配内存
    void *alloc(ulong nbytes);

    // 清0分配
    void *calloc(ulong count, ulong nbytes);

    // 释放Arena，也可以等待析构函数
    void dispose();

private:

    struct Hunk {
        Hunk *next;
        char *avail;
        char *limit;
    };

private:

    // 申请的块大小
    uint size;

    // 实存块链表
    Hunk *arena;
};

#endif /* ARENA_H */
