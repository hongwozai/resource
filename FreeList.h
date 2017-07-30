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

template <class Item>
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
};

template <class Item>
inline Item* FreeList<Item>::attach()
{
    char *item;
    if (!head) {
        item = new char[unit];
    } else {
        item = (char*)head;
        head = head->next;
    }
    return (Item*)item;
}

template <class Item>
inline void  FreeList<Item>::detach(Item *item)
{
    if (!item)
        return;

    if (tick++ & mask) {
        delete[] item;
    } else {
        Node *node = (Node*)item;
        node->next = head;
        head = node;
    }
    return;
}

template <class Item>
inline void FreeList<Item>::destroyAll()
{
    while (head) {
        Node *temp = head;
        head = head->next;
        delete[] temp;
    }
}

#endif /* FREELIST_H */
