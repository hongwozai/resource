/********************************************************************
 ** Copyright(c) 2017, luzeya
 ** All rights reserved
 **
 ** 文件名：FreeList.h
 ** 创建人：路泽亚
 ** 创建日期： 2017-07-30
 **
 ** 描  述：缓存链
 ** 注  意：1.
 ********************************************************************/
#ifndef FREELIST_H
#define FREELIST_H

#include <cstddef>
#include "Allocator.h"

template <class Item, class Allocator = Allocator<Item> >
class FreeList
{
public:

    /*!< mod 2的幂次返还上层一次 */
    FreeList(unsigned mod = 1, size_t unit = sizeof(Item))
        : mask((1 << mod) - 1),
          unit((unit < sizeof(void*)) ? sizeof(void*) : unit),
          head(0),
          tick(0) {}

    ~FreeList() { destroyAll(); }

    inline Item* attach();

    inline void detach(Item *item);

private:

    struct Node {
        Node *next;
    };

private:

    inline void destroyAll();

private:

    /*!< mask */
    size_t mask;

    /*!< 单个单元大小 */
    size_t unit;

    /*!< 链表头部 */
    Node  *head;

    /*!< 滴答数 */
    size_t tick;

    /*!< 分配器 */
    Allocator ItemAlloc;
};

template <class Item, class Allocator>
inline Item* FreeList<Item, Allocator>::attach()
{
    char *item;
    if (!head) {
        item = (char*)ItemAlloc.allocate(unit);
    } else {
        item = (char*)head;
        head = head->next;
    }
    return (Item*)item;
}

template <class Item, class Allocator>
inline void  FreeList<Item, Allocator>::detach(Item *item)
{
    if (!item)
        return;

    if (tick++ & mask) {
        ItemAlloc.deallocate((Item*)item, unit);
    } else {
        Node *node = (Node*)item;
        node->next = head;
        head = node;
    }
    return;
}

template <class Item, class Allocator>
inline void FreeList<Item, Allocator>::destroyAll()
{
    while (head) {
        Node *temp = head;
        head = head->next;
        ItemAlloc.deallocate((Item*)temp, unit);
    }
}

#endif /* FREELIST_H */
